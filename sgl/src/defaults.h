#pragma once
#include <string>
#include "object/buffers.h"
#include "shaders/render_shader.h"
#include "object/shape_data.h"

namespace global_defaults
{

	const sgl::mat4 &identity_ref();

	sgl::vbo &rect_obj_vbo();

	// const sgl::vbo &rect_data_2d();

	const sgl::shape_data_array<3 * 12, 2> &cube_texture_coords();
	const sgl::shape_data_array<3 * 12, 3> &cube_norms();
	const sgl::shape_data_array<3 * 12, 3> &cube_pts();
}