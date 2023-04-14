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

	vao make_shape_vao(const vbo &points, const vbo &normals, const vbo &text_points, vec_len dim)
	{
		detail::vao_lock lvao;
		detail::vbo_lock lvbo;

		vao res;
		res.generate();

		res.use();

		points.use();
		glEnableVertexAttribArray(render_shader::pos_attribute_loc);
		glVertexAttribPointer(render_shader::pos_attribute_loc, dim, GL_FLOAT, GL_FALSE, 0, (void *)0);

		normals.use();
		glEnableVertexAttribArray(render_shader::normal_attribute_loc);
		glVertexAttribPointer(render_shader::normal_attribute_loc, dim, GL_FLOAT, GL_FALSE, 0, (void *)0);

		text_points.use();
		glEnableVertexAttribArray(render_shader::textPos_attribute_loc);
		glVertexAttribPointer(render_shader::textPos_attribute_loc, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

		return res;
	}
}

class cube_type : public render_type
{
public:
	cube_type() : render_type(shapes_detail::make_shape_vao(detail::cube_pts(), detail::cube_norms(), detail::cube_texture_coords(), 3)) {}

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

	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, nullptr, {0, 0, 0, 1});

	render_obj::type->draw(target);
}

template <bool rotatable>
void cube_obj<rotatable>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.material, settings.color);

	render_obj::type->draw(target);
}

class rectangle_type : public render_type
{
public:
	rectangle_type() : render_type(shapes_detail::make_shape_vao(detail::rect_obj_vbo(), detail::rect_norm_vbo(), detail::rect_textPos_vbo(), 3)) {}

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
	
	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, nullptr, { 0, 0, 0, 1 });

	render_obj::type->draw(target);
}

template <bool rotatable>
void rectangle_obj<rotatable>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	setup_buffer();

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.material, settings.color);

	render_obj::type->draw(target);
}

template <bool rotatable>
void rectangle_obj<rotatable>::setup_buffer() const
{
	auto pos_vbo = render_obj::get_vao().get_attribute(render_shader::pos_attribute_loc);

	auto data = pos_vbo. template get_data<vec3>(GL_WRITE_ONLY);
	vec3 right = m_sz.x * m_right;
	vec3 up = m_sz.y * m_up;
	data[0] = {};
	data[1] = right;
	data[2] = right + up;
	data[3] = up;

	auto norm_vbo = render_obj::get_vao().get_attribute(render_shader::normal_attribute_loc);
	auto norm = normalize(cross(right, -up));
	
	// test to see which is faster
	vec3 norm_array[4] = { norm, norm, norm, norm };
	norm_vbo.attach_data(norm_array, GL_DYNAMIC_DRAW);

	/*auto norm_data = norm_vbo. template get_data<vec3>(GL_WRITE_ONLY);
	norm_data[0] = norm_data[1] = norm_data[2] = norm_data[3] = norm;*/
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

void point_obj_2_setup_buffer(float half, const vao &_vao)
{
	auto v = _vao.get_attribute(render_shader::pos_attribute_loc);

	auto data = v. template get_data<vec3>(GL_WRITE_ONLY);

	/*vec3 min(-half, -half, 0);
	vec3 right(half, -half, 0);
	vec3 max(half, half, 0);
	vec3 up(-half, half, 0);*/
	data[0] = { -half, -half, 0 };
	data[1] = { half, -half, 0 };
	data[2] = { half,  half, 0 };
	data[3] = { -half,  half, 0 };

	auto norm_vbo = _vao.get_attribute(render_shader::normal_attribute_loc);

	// cross(right - min, right - max);
	// cross((half, -half) - (-half, -half), (half, -half) - (half, half));
	// cross((2 * half, 0), (0, -2 * half))
	auto norm = normalize(cross(vec3(2 * half, 0, 0), vec3(0, -2 * half)));

	// test to see which is faster
	vec3 norm_array[4] = { norm, norm, norm, norm };
	norm_vbo.attach_data(norm_array, GL_DYNAMIC_DRAW);
}


template <>
void point_obj<2>::draw(render_target& target) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	point_obj_2_setup_buffer(m_size / 2, render_obj::get_vao());

	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, nullptr, { 0, 0, 0, 1 });

	render_obj::type->draw(target);
}

template <>
void point_obj<2>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	point_obj_2_setup_buffer(m_size / 2, render_obj::get_vao());

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, nullptr, settings.color);

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

	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, nullptr, { 0, 0, 0, 1 });

	render_obj::type->draw(target);
}

template <>
void point_obj<3>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock slock;

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.material, settings.color);

	render_obj::type->draw(target);
}

template <bool scalable, bool rotatable>
line_obj<scalable, rotatable>::line_obj() : render_obj(cube_type::get_instance()), movable_obj(),
											m_end{}, m_width{.05f}
{
}
template <bool scalable, bool rotatable>
line_obj<scalable, rotatable>::line_obj(vec3 beg, vec3 end, float line_width) : render_obj(cube_type::get_instance()), movable_obj(beg),
																				m_end{end}, m_width{line_width}
{
}

template <bool scalable, bool rotatable>
void line_obj<scalable, rotatable>::draw(render_target& target) const
{
	base_transformable_obj::update_model();

	detail::shader_lock lock;

	detail::setup_shader(shapes_detail::get_shader(), base_transformable_obj::model, nullptr, nullptr, nullptr, { 0, 0, 0, 1 });

	render_obj::type->draw(target);
}

template <bool scalable, bool rotatable>
void line_obj<scalable, rotatable>::draw(render_target &target, const render_settings &settings) const
{
	base_transformable_obj::update_model();

	detail::shader_lock lock;

	render_shader &shader = settings.shader ? *settings.shader : shapes_detail::get_shader();
	detail::setup_shader(shader, base_transformable_obj::model, settings.engine, nullptr, settings.material, settings.color);

	render_obj::type->draw(target);
}

template <bool scalable, bool rotatable>
void line_obj<scalable, rotatable>::apply_transform() const
{
	base_transformable_obj::model *= translate(movable_obj::m_loc);
	base_transformable_obj::model *= scale({ m_width, m_width, distance(movable_obj::m_loc, m_end) });
	base_transformable_obj::model *= rot(angle(movable_obj::m_loc, m_end), cross(movable_obj::m_loc, m_end));

	transformable_obj<true, scalable, rotatable>::apply_transform();
}

//template <bool scalable, bool rotatable>
//void setup_buffer()
//{
//	auto v = render_obj::get_vao().get_attribute(0);
//	auto data = v. template get_data<vec3>(GL_READ_WRITE);
//
//	auto end = m_end - movable_obj::m_loc;
//	auto dir = normalize(end);
//	sgl::vec3 right;
//	if (dir.x || dir.z)
//		right = normalize(cross(dir, sgl::vec3{ 0, 1, 0 }));
//	else
//		right = { 0, 0, 1 };
//	auto up = normalize(cross(right, dir));
//	float half_width = m_width / 2;
//
//	// bottom right
//	data[0] = half_width * (right - up);
//
//	// bottom left
//	data[1] = -half_width * (right + up);
//
//	// top left
//	data[2] = -half_width * (right - up);
//
//	// top right
//	data[3] = half_width * (right + up);
//
//	data[4] = end + data[0];
//	data[5] = end + data[1];
//	data[6] = end + data[2];
//	data[7] = end + data[3];
//}

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