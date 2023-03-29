#include "object/shapes.h"
#include "object/render_target.h"
#include "help.h"
#include "context_lock/context_lock.h"

SGL_BEG
namespace shapes_detail
{
	render_shader &get_shader()
	{
		using namespace variables;
		static std::string vertex_source = "void main() { gl_Position = sgl_ModelViewProj * vec4(sgl_Pos, 1.0); }";
		static std::string fragment_source = "void main() { sgl_OutColor = sgl_Color; }";
		static render_shader shader(vertex_source, fragment_source, sgl_ModelViewProj | sgl_Pos | sgl_Color);
		return shader;
	}

	vao make_shape_vao(const vbo &buffer, vec_len dim)
	{
		detail::vao_lock lvao;
		detail::vbo_lock lvbo;

		vao res;
		res.generate();

		res.use();

		buffer.use();

		glEnableVertexAttribArray(render_shader::pos_attribute_loc);
		glVertexAttribPointer(render_shader::pos_attribute_loc, dim, GL_FLOAT, GL_FALSE, 0, (void *)0);

		return res;
	}
}

class cube_type : public render_type
{
public:
	cube_type() : render_type()
	{
		static vbo _vbo = detail::cube_pts().get_buffer();
		m_vao = shapes_detail::make_shape_vao(_vbo, 3);
	}

	void draw(render_target& target) const override
	{
		detail::vao_lock lock;
		detail::fbo_lock flock;

		bind_target(target);

		m_vao.use();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	static cube_type& get_instance()
	{
		static cube_type res;
		return res;
	}
};

template <bool rotatable>
cube_obj<rotatable>::cube_obj() : render_obj(cube_type::get_instance())
{
}

template <bool rotatable>
cube_obj<rotatable>::cube_obj(vec3 min, vec3 size) : render_obj(cube_type::get_instance()), movable_obj(min), scalable_obj(size)
{
}

template <bool rotatable>
void cube_obj<rotatable>::draw(render_target& target) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, {0, 0, 0, 1});

	render_obj::type->draw(target);
}

template <bool rotatable>
void cube_obj<rotatable>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.color);

	render_obj::type->draw(target);
}

class rectangle_type : public render_type
{
public:
	rectangle_type() : render_type(shapes_detail::make_shape_vao(detail::rect_obj_vbo(), 3)) {}

	void draw(render_target& target) const override
	{
		detail::vao_lock vlock;
		detail::cull_face_lock clock;
		detail::fbo_lock flock;

		glDisable(GL_CULL_FACE);

		bind_target(target);

		m_vao.use();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	static rectangle_type& get_instance()
	{
		static rectangle_type res;
		return res;
	}
};

template <bool rotatable>
rectangle_obj<rotatable>::rectangle_obj() : render_obj(rectangle_type::get_instance())
{
}

template <bool rotatable>
rectangle_obj<rotatable>::rectangle_obj(vec3 min, vec2 size, vec3 right, vec3 up) : render_obj(rectangle_type::get_instance()), movable_obj(min),
																					m_right{normalize(right)}, m_up{normalize(up)}, m_sz{size}
{
}

template <bool rotatable>
void rectangle_obj<rotatable>::draw(render_target& target) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	setup_buffer();
	
	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, { 0, 0, 0, 1 });

	render_obj::type->draw(target);
}

template <bool rotatable>
void rectangle_obj<rotatable>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	setup_buffer();

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.color);

	render_obj::type->draw(target);
}

template <bool rotatable>
void rectangle_obj<rotatable>::setup_buffer() const
{
	auto v = render_obj::get_vao().get_attribute(0);

	auto data = v. template get_data<vec3>(GL_WRITE_ONLY);
	vec3 right = m_sz.x * m_right;
	vec3 up = m_sz.y * m_up;
	data[0] = {};
	data[1] = right;
	data[2] = right + up;
	data[3] = up;
}

template <>
point_obj<2>::point_obj(vec2 center, float size) : render_obj(rectangle_type::get_instance()), m_center{ center }, m_size{ size }
{
}

template <>
point_obj<3>::point_obj(vec3 center, float size) : render_obj(cube_type::get_instance()), m_center{ center }, m_size{ size }
{
}

template <vec_len dim>
point_obj<dim>::point_obj() : render_obj(rectangle_type::get_instance()), m_center{}, m_size { 1 }
{
}

template <>
void point_obj<2>::apply_transform() const
{
	model *= translate(vec3(m_center));
}

template <>
void point_obj<2>::draw(render_target& target) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	// set buffer
	{
		auto v = render_obj::get_vao().get_attribute(0);

		auto data = v. template get_data<vec3>(GL_WRITE_ONLY);
		float half = m_size / 2;

		data[0] = { -half, -half, 0 };
		data[1] = { half, -half, 0 };
		data[2] = { half, half, 0 };
		data[3] = { -half, half, 0 };
	}

	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, { 0, 0, 0, 1 });

	render_obj::type->draw(target);
}

template <>
void point_obj<2>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	// set buffer
	{
		auto v = render_obj::get_vao().get_attribute(0);

		auto data = v. template get_data<vec3>(GL_WRITE_ONLY);
		float half = m_size / 2;

		data[0] = { -half, -half, 0 };
		data[1] = { half, -half, 0 };
		data[2] = { half, half, 0 };
		data[3] = { -half, half, 0 };
	}

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.color);

	render_obj::type->draw(target);
}

template <>
void point_obj<3>::apply_transform() const
{
	vec3 size(m_size, m_size, m_size);
	model *= translate(m_center - size / 2);
	model *= scale(size);
}

template <>
void point_obj<3>::draw(render_target& target) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, { 0, 0, 0, 1 });

	render_obj::type->draw(target);
}

template <>
void point_obj<3>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.color);

	render_obj::type->draw(target);
}

class line_type : public render_type
{
public:
	line_type() : render_type()
	{
		constexpr unsigned short A = 0;
		constexpr unsigned short B = 1;
		constexpr unsigned short C = 2;
		constexpr unsigned short D = 3;
		constexpr unsigned short E = 4;
		constexpr unsigned short F = 5;
		constexpr unsigned short G = 6;
		constexpr unsigned short H = 7;

		static unsigned short index_buffer[] = {
			// FRONT FACE
			A, B, E,
			E, B, F,

			// TOP FACE
			E, F, H,
			H, F, G,

			// BACK FACE
			C, D, G,
			G, D, H,

			// BOTTOM FACE
			B, A, C,
			C, A, D,

			// LEFT FACE
			D, A, H,
			H, A, E,

			// RIGHT FACE
			B, C, F,
			F, C, G,
		};

		static auto make_ebo = []()
		{
			ebo res;
			res.generate();
			res.attach_data(index_buffer, GL_STATIC_DRAW);
			return res;
		};

		static ebo _ebo = make_ebo();

		// bottom
		// +x, -y, -z
		constexpr vec3 A_pt(.5, -.5, -.5);
		// -x, -y, -z
		constexpr vec3 B_pt(-.5, -.5, -.5);
		// -x, -y, +z
		constexpr vec3 C_pt(-.5, -.5, .5);
		// +x, -y, +z
		constexpr vec3 D_pt(.5, -.5, .5);
			
		// top
		// +x, +y, -z
		constexpr vec3 E_pt(.5, .5, -.5);
		// -x, +y, -z
		constexpr vec3 F_pt(-.5, .5, -.5);
		// -x, +y, +z
		constexpr vec3 G_pt(-.5, .5, .5);
		// +x, +y, +z
		constexpr vec3 H_pt(.5, .5, .5);

		static shape_data_array<8, 3> pt_buffer = {A_pt, B_pt, C_pt, D_pt, E_pt, F_pt, G_pt, H_pt};
		static vbo _vbo = pt_buffer.get_buffer(GL_STREAM_DRAW);

		detail::vbo_lock lvbo;
		detail::ebo_lock lebo;
		detail::vao_lock lvao;

		m_vao.generate();
		m_vao.use();

		_vbo.use();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
		_ebo.use();

		m_vao.quit();
	}

	void draw(render_target &target) const override
	{
		detail::vao_lock lock;
		detail::fbo_lock flock;

		bind_target(target);

		m_vao.use();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
	}

	static line_type& get_instance()
	{
		static line_type res;
		return res;
	}
};

template <bool scalable, bool rotatable>
line_obj<scalable, rotatable>::line_obj() : render_obj(line_type::get_instance()), movable_obj(),
											m_end{}, m_width{.05f}
{
}
template <bool scalable, bool rotatable>
line_obj<scalable, rotatable>::line_obj(vec3 beg, vec3 end, float line_width) : render_obj(line_type::get_instance()), movable_obj(beg),
																				m_end{end}, m_width{line_width}
{
}

template <bool scalable, bool rotatable>
void line_obj<scalable, rotatable>::draw(render_target& target) const
{
	base_transformable_obj::update_model();

	detail::shader_lock lock;

	setup_buffer();

	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, { 0, 0, 0, 1 });

	render_obj::type->draw(target);
}

template <bool scalable, bool rotatable>
void line_obj<scalable, rotatable>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock lock;

	setup_buffer();

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.color);

	render_obj::type->draw(target);
}

template <bool scalable, bool rotatable>
void line_obj<scalable, rotatable>::setup_buffer() const
{
	auto v = render_obj::get_vao().get_attribute(0);
	auto data = v. template get_data<vec3>(GL_READ_WRITE);

	auto end = m_end - movable_obj::m_loc;
	auto dir = normalize(end);
	sgl::vec3 right;
	if (dir.x || dir.z)
		right = normalize(cross(dir, sgl::vec3{ 0, 1, 0 }));
	else
		right = { 0, 0, 1 };
	auto up = normalize(cross(right, dir));
	float half_width = m_width / 2;

	// bottom right
	data[0] = half_width * (right - up);

	// bottom left
	data[1] = -half_width * (right + up);

	// top left
	data[2] = -half_width * (right - up);

	// top right
	data[3] = half_width * (right + up);

	data[4] = end + data[0];
	data[5] = end + data[1];
	data[6] = end + data[2];
	data[7] = end + data[3];
}

template class rectangle_obj<false>;
template class rectangle_obj<true>;

template class cube_obj<false>;
template class cube_obj<true>;

template class line_obj<false, false>;
template class line_obj<true, true>;
template class line_obj<true, false>;
template class line_obj<false, true>;

template class point_obj<2>;
template class point_obj<3>;

SGL_END