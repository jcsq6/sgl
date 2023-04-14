#include "help.h"
#include "context_lock/context_lock.h"

#include "object/render_target.h"

SGL_BEG
DETAIL_BEG

void setup_shader(render_shader &program, const mat4 &m, const lighting_engine *engine, const texture *text, const abstract_material *mat, vec4 color)
{
	if (program.has_texture_uniform() && text)
		program.set_texture_uniform(*text);

	if (engine)
		program.set_lighting_uniforms(*engine);

	if (program.has_color_uniform())
		program.set_color_uniform(color);

	if (mat)
	{
		auto material_base = dynamic_cast<const material *>(mat);
		if (material_base && program.has_material_uniform())
			program.set_material_uniform(*material_base);
		auto text_material_base = dynamic_cast<const texture_material *>(mat);
		if (text_material_base && program.has_textureMaterial_uniform())
			program.set_textureMaterial_uniform(*text_material_base);
	}

	auto *v = get_view();
	auto *p = get_projection();

	if (!p)
		p = &detail::identity_ref();
	if (!v)
		v = &detail::identity_ref();

	if (program.has_modelViewProj_uniform() || program.has_modelView_uniform() || program.has_inverseModelView_uniform())
	{
		mat4 mv = *v;
		mv *= m;

		if (program.has_inverseModelView_uniform())
			program.set_inverseModelView_uniform(inverse(mv));
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

const mat4 &identity_ref()
{
	static const auto id = identity();
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

vbo &rect_norm_vbo()
{
	static vbo res = make_rect_vbo();
	return res;
}

const vbo &rect_textPos_vbo()
{
	// assumes that the other vbo's follow this convection: min, right, max, up
	static shape_data_array<4, 2> text_coords{ {
		{0, 0},
		{1, 0},
		{1, 1},
		{0, 1},
	}};
	static vbo buffer = text_coords.get_buffer();
	return buffer;
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

const vbo &cube_texture_coords()
{
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

	static vbo buffer = text_coords.get_buffer();

	return buffer;
}

const vbo &cube_norms()
{
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

	static vbo buffer = norms.get_buffer();

	return buffer;
}

const vbo &cube_pts()
{
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

	static vbo buffer = pts.get_buffer();

	return buffer;
}
DETAIL_END
SGL_END