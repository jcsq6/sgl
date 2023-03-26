#include "shaders/render_shader.h"

SGL_BEG

#define pos_loc 0
#define normal_loc 1
#define color_loc 2
#define textPos_loc 3

#define STR_2(x) #x
#define STR(x) STR_2(x)


void render_shader::generate_shader(const std::string &vertex, const std::string &fragment, unsigned int includes)
{
	using namespace variables;

	m_includes = includes;

	std::string vertex_src = "#version 410 core\n";
	std::string frag_src = "#version 410 core\n";

	// uniforms
	if (has_color_uniform())
	{
		static std::string val = "uniform vec4 " + variable_name<sgl_Color> + ';';
		vertex_src += val;
		frag_src += val;
	}
	if (has_view_uniform())
	{
		static std::string val = "uniform mat4 " + variable_name<sgl_View> + ';';
		vertex_src += val;
		frag_src += val;
	}
	if (has_model_uniform())
	{
		static std::string val = "uniform mat4 " + variable_name<sgl_Model> + ';';
		vertex_src += val;
		frag_src += val;
	}
	if (has_modelViewProj_uniform())
	{
		static std::string val = "uniform mat4 " + variable_name<sgl_ModelViewProj> + ';';
		vertex_src += val;
		frag_src += val;
	}
	if (has_texture_uniform())
	{
		static std::string val = "uniform sampler2D " + variable_name<sgl_Texture> + ';';
		vertex_src += val;
		frag_src += val;
	}

	// attributes
	if (has_pos_attribute())
	{
		static std::string val = "layout (location = " STR(pos_loc) ") in vec3 " + variable_name<sgl_Pos> + ';';
		vertex_src += val;
	}
	if (has_normal_attribute())
	{
		static std::string val = "layout (location = " STR(normal_loc) ") in vec3 " + variable_name<sgl_Normal> + ';';
		vertex_src += val;
	}
	if (has_color_attribute())
	{
		static std::string val = "layout (location = " STR(color_loc) ") in vec4 " + variable_name<sgl_ColorAttrib> + ';';
		vertex_src += val;
	}
	if (has_textPos_attribute())
	{
		static std::string val = "layout (location = " STR(textPos_loc) ") in vec2 " + variable_name<sgl_TextPos> + ';';
		vertex_src += val;
	}

	// out/in variables
	if (has_pos_out_var())
	{
		static std::string vert_val = "out vec3 " + variable_name<sgl_VertPos> + ';';
		static std::string frag_val = "in vec3 " + variable_name<sgl_VertPos> + ';';
		vertex_src += vert_val;
		frag_src += frag_val;
	}
	if (has_normal_out_var())
	{
		static std::string vert_val = "out vec3 " + variable_name<sgl_VertNormal> + ';';
		static std::string frag_val = "in vec3 " + variable_name<sgl_VertNormal> + ';';
		vertex_src += vert_val;
		frag_src += frag_val;
	}
	if (has_color_out_var())
	{
		static std::string vert_val = "out vec4 " + variable_name<sgl_VertColor> + ';';
		static std::string frag_val = "in vec4 " + variable_name<sgl_VertColor> + ';';
		vertex_src += vert_val;
		frag_src += frag_val;
	}
	if (has_textPos_out_var())
	{
		static std::string vert_val = "out vec2 " + variable_name<sgl_VertTextPos> + ';';
		static std::string frag_val = "in vec2 " + variable_name<sgl_VertTextPos> + ';';
		vertex_src += vert_val;
		frag_src += frag_val;
	}

	vertex_src += vertex;

	frag_src += "out vec4 sgl_OutColor;";
	frag_src += fragment;

	m_shader.load_from_memory(vertex_src, frag_src);
}

unsigned int render_shader::pos_attribute_loc = pos_loc;
unsigned int render_shader::normal_attribute_loc = normal_loc;
unsigned int render_shader::color_attribute_loc = color_loc;
unsigned int render_shader::textPos_attribute_loc = textPos_loc;
SGL_END