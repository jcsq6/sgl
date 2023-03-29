#include "shaders/render_shader.h"

#define pos_loc 0
#define normal_loc 1
#define color_loc 2
#define textPos_loc 3

#define STR_2(x) #x
#define STR(x) STR_2(x)

SGL_BEG

void render_shader::generate_shader(const std::string &vertex, const std::string &fragment, unsigned int variables)
{
	using namespace variables;

	m_variables = variables;

	std::string vertex_src = "#version 410 core\n";
	std::string frag_src = "#version 410 core\n";

	// uniforms
	if (has_color_uniform())
	{
		static const std::string val = "uniform vec4 sgl_Color;";
		vertex_src += val;
		frag_src += val;
	}
	if (has_view_uniform())
	{
		static const std::string val = "uniform mat4 sgl_View;";
		vertex_src += val;
		frag_src += val;
	}
	if (has_model_uniform())
	{
		static const std::string val = "uniform mat4 sgl_Model;";
		vertex_src += val;
		frag_src += val;
	}
	if (has_modelView_uniform())
	{
		static const std::string val = "uniform mat4 sgl_ModelView;";
		vertex_src += val;
		frag_src += val;
	}
	if (has_modelViewProj_uniform())
	{
		static const std::string val = "uniform mat4 sgl_ModelViewProj;";
		vertex_src += val;
		frag_src += val;
	}
	if (has_texture_uniform())
	{
		static const std::string val = "uniform sampler2D sgl_Texture;";
		vertex_src += val;
		frag_src += val;
	}
	if (has_material_uniform())
	{
		static const std::string type = "struct sgl_Material_t { vec3 ambient; vec3 diffuse; vec3 specular; float shininess; };";
		static const std::string val = "uniform sgl_Material_t sgl_Material;";
		(vertex_src += type) += val;
		frag_src += val;
	}
	if (has_textureMaterial_uniform())
	{
		static const std::string type = "struct sgl_TextureMaterial_t { sampler2D diffuse; sampler2D specular; float shininess; }; ";
		static const std::string val = "uniform sgl_TextureMaterial_t sgl_TextureMaterial;";
		(vertex_src += type) += val;
		frag_src += val;
	}
	if (has_globalLight_uniform())
	{
		static const std::string type = "struct sgl_GlobalLight_t { vec3 ambient; };";
		static const std::string val = "uniform sgl_GlobalLight_t sgl_GlobalLight;";
		(vertex_src += type) += val;
		frag_src += val;
	}

	if (m_directionals.size())
		variables |= variables::sgl_DirectionalLights;
	if (has_directionalLights_uniform())
	{
		if (!m_directionals.size())
			m_directionals = { "sgl_DirectionalLights[0]" };
		static const std::string type = "struct sgl_DirectionalLight_t { vec3 ambient; vec3 diffuse; vec3 specular; vec3 direction; };";
		static const std::string val = "uniform int sgl_DirectionalLightsSize; uniform sgl_DirectionalLight_t sgl_DirectionalLights[";
		std::string tag = std::to_string(m_directionals.size()) + "];";
		((vertex_src += type) += val) += tag;
		(frag_src += val) += tag;
	}

	if (m_positionals.size())
		variables |= variables::sgl_PositionalLights;
	if (has_positionalLights_uniform())
	{
		if (!m_positionals.size())
			m_positionals = { "sgl_PositionalLights[0]" };
		static const std::string type = "struct sgl_PositionalLight_t { vec3 ambient; vec3 diffuse; vec3 specular; vec3 position; float constant; float linear; float quadratic; };";
		static const std::string val = "uniform int sgl_PostionalLightsSize; uniform sgl_PositionalLight_t sgl_PositionalLights[";
		std::string tag = std::to_string(m_positionals.size()) + "];";
		((vertex_src += type) += val) += tag;
		(frag_src += val) += tag;
	}

	if (m_spotlights.size())
		variables |= variables::sgl_Spotlights;
	if (has_spotlights_uniform())
	{
		if (!m_spotlights.size())
			m_spotlights = { "sgl_Spotlights[0]" };
		static const std::string type = "struct sgl_Spotlight_t { vec3 ambient; vec3 diffuse; vec3 specular; vec3 direction; vec3 position; float cutoff_angle; float outer_cutoff_angle; float constant; float linear; float quadratic; };";
		static const std::string val = "uniform int sgl_SpotlightsSize; uniform sgl_Spotlight_t sgl_Spotlights[";
		std::string tag = std::to_string(m_spotlights.size()) + "];";
		((vertex_src += type) += val) += tag;
		(frag_src += val) += tag;
	}

	// attributes
	if (has_pos_attribute())
	{
		static const std::string val = "layout (location = " STR(pos_loc) ") in vec3 sgl_Pos;";
		vertex_src += val;
	}
	if (has_normal_attribute())
	{
		static const std::string val = "layout (location = " STR(normal_loc) ") in vec3 sgl_Normal;";
		vertex_src += val;
	}
	if (has_color_attribute())
	{
		static const std::string val = "layout (location = " STR(color_loc) ") in vec4 sgl_ColorAttrib;";
		vertex_src += val;
	}
	if (has_textPos_attribute())
	{
		static const std::string val = "layout (location = " STR(textPos_loc) ") in vec2 sgl_TextPos;";
		vertex_src += val;
	}

	// out/in variables
	if (has_pos_out_var())
	{
		static const std::string vert_val = "out vec3 sgl_VertPos;";
		static const std::string frag_val = "in vec3 sgl_VertPos;";
		vertex_src += vert_val;
		frag_src += frag_val;
	}
	if (has_normal_out_var())
	{
		static const std::string vert_val = "out vec3 sgl_VertNormal;";
		static const std::string frag_val = "in vec3 sgl_VertNormal;";
		vertex_src += vert_val;
		frag_src += frag_val;
	}
	if (has_color_out_var())
	{
		static const std::string vert_val = "out vec4 sgl_VertColor;";
		static const std::string frag_val = "in vec4 sgl_VertColor;";
		vertex_src += vert_val;
		frag_src += frag_val;
	}
	if (has_textPos_out_var())
	{
		static const std::string vert_val = "out vec2 sgl_VertTextPos;";
		static const std::string frag_val = "in vec2 sgl_VertTextPos;";
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