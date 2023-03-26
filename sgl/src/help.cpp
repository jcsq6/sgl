#include "help.h"
#include "context_lock/context_lock.h"

#include "object/render_target.h"

SGL_BEG
DETAIL_BEG
void setup_shader(const gtexture &text, const mat4 &m, render_shader &program)
{
	if (program.has_texture_uniform())
		program.set_texture_uniform(text);

	if (program.has_color_uniform())
		program.set_color_uniform(get_draw_color());

	auto *v = get_view();
	auto *p = get_projection();

	if (!p)
		p = &detail::identity_ref();
	if (!v)
		v = &detail::identity_ref();

	if (program.has_modelViewProj_uniform() || program.has_modelView_uniform())
	{
		mat4 mv = *v;
		mv *= m;

		if (program.has_modelView_uniform())
			program.set_modelView_uniform(mv);
		if (program.has_modelViewProj_uniform())
			program.set_modelViewProj_uniform((*p) * mv);
	}

	if (program.has_model_uniform())
		program.set_model_uniform(m);
	if (program.has_view_uniform())
		program.set_view_uniform(*v);
	if (program.has_proj_uniform())
		program.set_proj_uniform(*p);

	program.bind();
}

void setup_shader(const mat4 &m, render_shader &program)
{
	if (program.has_color_uniform())
		program.set_color_uniform(get_draw_color());

	auto *v = get_view();
	auto *p = get_projection();

	if (!p)
		p = &detail::identity_ref();
	if (!v)
		v = &detail::identity_ref();

	if (program.has_modelViewProj_uniform() || program.has_modelView_uniform())
	{
		mat4 mv = *v;
		mv *= m;

		if (program.has_modelView_uniform())
			program.set_modelView_uniform(mv);
		if (program.has_modelViewProj_uniform())
			program.set_modelViewProj_uniform((*p) * mv);
	}

	if (program.has_model_uniform())
		program.set_model_uniform(m);
	if (program.has_view_uniform())
		program.set_view_uniform(*v);
	if (program.has_proj_uniform())
		program.set_proj_uniform(*p);

	program.bind();
}

const mat4 &detail::identity_ref()
{
	static auto id = identity();
	return id;
}

vbo make_rect_vbo()
{
	vbo v;
	v.generate();
	v.reserve_data(4 * sizeof(vec3), GL_STREAM_DRAW);
	return v;
}

vbo &rect_obj_vbo()
{
	static vbo res = make_rect_vbo();
	return res;
}

// const vbo& rect_data_2d()
// {
// 	static shape_data_array<4, 2> rect = {
// 		{
// 			{0, 0},
// 			{1, 0},
// 			{1, 1},
// 			{0, 1},
// 		}};
// 	static auto res = rect.get_buffer();
// 	return res;
// }

const shape_data_array<3 * 12, 2> &cube_texture_coords()
{
	using namespace sgl;

	static shape_data_array<3 * 12, 2> text_coords{ {
			// FRONT FACE
			{0, 0}, {1, 0}, {0, 1},
			{0, 1}, {1, 0}, {1, 1},

			// TOP FACE
			{0, 0}, {1, 0}, {0, 1},
			{0, 1}, {1, 0}, {1, 1},

			// BACK FACE
			{0, 0}, {1, 0}, {0, 1},
			{0, 1}, {1, 0}, {1, 1},

			// BOTTOM FACE
			{0, 0}, {1, 0}, {0, 1},
			{0, 1}, {1, 0}, {1, 1},

			// LEFT FACE
			{0, 0}, {1, 0}, {0, 1},
			{0, 1}, {1, 0}, {1, 1},

			// RIGHT FACE
			{0, 0}, {1, 0}, {0, 1},
			{0, 1}, {1, 0}, {1, 1},
		} };

	return text_coords;
}

const shape_data_array<3 * 12, 3> &cube_norms()
{
	using namespace sgl;

	constexpr vec3 A(1, 0, 0);
	constexpr vec3 B(0, 0, 0);
	constexpr vec3 C(0, 0, 1);
	constexpr vec3 D(1, 0, 1);
	constexpr vec3 E(1, 1, 0);
	constexpr vec3 F(0, 1, 0);
	constexpr vec3 G(0, 1, 1);
	constexpr vec3 H(1, 1, 1);

	constexpr vec3 front_norm = cross(A - B, A - E);
	constexpr vec3 top_norm = cross(E - F, F - H);
	constexpr vec3 back_norm = cross(C - D, C - G);
	constexpr vec3 bottom_norm = cross(B - A, B - C);
	constexpr vec3 left_norm = cross(D - A, D - H);
	constexpr vec3 right_norm = cross(B - C, B - F);

	static shape_data_array<3 * 12, 3> norms{ {
			// FRONT FACE
			front_norm, front_norm, front_norm,
			front_norm, front_norm, front_norm,

			// TOP FACE
			top_norm, top_norm, top_norm,
			top_norm, top_norm, top_norm,

			// BACK FACE
			back_norm, back_norm, back_norm,
			back_norm, back_norm, back_norm,

			// BOTTOM FACE
			bottom_norm, bottom_norm, bottom_norm,
			bottom_norm, bottom_norm, bottom_norm,

			// LEFT FACE
			left_norm, left_norm, left_norm,
			left_norm, left_norm, left_norm,

			// RIGHT FACE
			right_norm, right_norm, right_norm,
			right_norm, right_norm, right_norm,
		} };

	return norms;
}

const shape_data_array<3 * 12, 3> &cube_pts()
{
	using namespace sgl;

	constexpr vec3 A(1, 0, 0);
	constexpr vec3 B(0, 0, 0);
	constexpr vec3 C(0, 0, 1);
	constexpr vec3 D(1, 0, 1);
	constexpr vec3 E(1, 1, 0);
	constexpr vec3 F(0, 1, 0);
	constexpr vec3 G(0, 1, 1);
	constexpr vec3 H(1, 1, 1);

	static shape_data_array<3 * 12, 3> pts{ {
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
		} };

	return pts;
}
DETAIL_END
SGL_END