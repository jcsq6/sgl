#include "object/sprite.h"

#include "context_lock/context_lock.h"

#include "object/render_target.h"

#include "shaders/shaders.h"
#include "math/mat.h"

#include "defaults.h"

SGL_BEG

namespace sprite_detail
{
	render_shader &get_shader()
	{
		using namespace variables;
		static std::string vertex_source = "void main() { gl_Position = sgl_ModelViewProj * vec4(sgl_Pos, 1.0); sgl_VertTextPos = sgl_TextPos; }";
		static std::string fragment_source = "void main() { sgl_OutColor = texture(sgl_Texture, sgl_VertTextPos); }";
		static sgl::render_shader shader(vertex_source, fragment_source, sgl_ModelViewProj | sgl_Pos | sgl_TextPos | sgl_VertTextPos | sgl_Texture);
		return shader;
	}

	inline void setup_shader(const gtexture &text, const mat4 &m, render_shader &program)
	{

		program.set_texture_uniform(text);
		program.set_color_uniform(get_draw_color());

		auto *v = get_view();
		auto *p = get_projection();

		if (!p)
			p = &global_defaults::identity_ref();
		if (!v)
			v = &global_defaults::identity_ref();

		if (program.has_modelViewProj_uniform())
			program.set_modelViewProj_uniform((*p) * (*v) * m);
		if (program.has_model_uniform())
			program.set_model_uniform(m);
		if (program.has_view_uniform())
			program.set_view_uniform(*v);
		if (program.has_proj_uniform())
			program.set_proj_uniform(*p);

		program.bind();
	}
}

class sprite_type : public render_type
{
public:
	sprite_type() : render_type()
	{
		detail::vao_lock lvao;
		detail::vbo_lock lvbo;
		
		m_vao.generate();
		m_vao.use();

		static sgl::shape_data_array<4, 2> text_pts_data = {
			{
				{0, 0},
				{1, 0},
				{1, 1},
				{0, 1},
			}};

		static auto text_pts_buffer = text_pts_data.get_buffer();

		global_defaults::rect_obj_vbo().use();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		text_pts_buffer.use();
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
	}

	void draw(render_target &target) const override
	{
		detail::vao_lock vlock;
		detail::cull_face_lock clock;
		detail::fbo_lock flock;

		glDisable(GL_CULL_FACE);

		bind_target(target);

		m_vao.use();
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	static sprite_type& get_instance()
	{
		static sprite_type res;
		return res;
	}
};

template <bool rotatable>
sprite<rotatable>::sprite(const gtexture &texture, vec3 min, vec2 size, vec3 right, vec3 up) : render_obj(sprite_type::get_instance()),
																							   movable_obj(min),
																							   rectangle_obj<rotatable>::rectangle_obj(min, size, right, up),
																							   m_texture(&texture)
{
}

template <bool rotatable>
void sprite<rotatable>::draw(render_target& target) const
{
	detail::shader_lock vlock;

	rectangle_obj<rotatable>::setup_buffer();
	
	sprite_detail::setup_shader(*m_texture, base_transformable_obj::model, sprite_detail::get_shader());

	render_obj::type->draw(target);
}

template class sprite<false>;
template class sprite<true>;

SGL_END
