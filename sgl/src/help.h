#pragma once
#include <string>
#include "object/buffers.h"
#include "shaders/render_shader.h"
#include "object/shape_data.h"

SGL_BEG
DETAIL_BEG
void setup_shader(render_shader &program, const mat4 &m, const lighting_engine *engine, const gtexture *text, vec4 color);

const mat4 &identity_ref();

vbo &rect_obj_vbo();

// const vbo &rect_data_2d();

const shape_data_array<3 * 12, 2> &cube_texture_coords();
const shape_data_array<3 * 12, 3> &cube_norms();
const shape_data_array<3 * 12, 3> &cube_pts();
DETAIL_END
SGL_END