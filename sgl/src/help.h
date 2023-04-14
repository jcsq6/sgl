#pragma once
#include <string>
#include "object/buffers.h"
#include "shaders/render_shader.h"
#include "object/shape_data.h"

SGL_BEG
DETAIL_BEG
void setup_shader(render_shader &program, const mat4 &m, const lighting_engine *engine, const texture *text, const abstract_material *mat, vec4 color);

const mat4 &identity_ref();

vbo &rect_obj_vbo();
vbo &rect_norm_vbo();
const vbo &rect_textPos_vbo();

// const vbo &rect_data_2d();

const vbo &cube_texture_coords();
const vbo &cube_norms();
const vbo &cube_pts();
DETAIL_END
SGL_END