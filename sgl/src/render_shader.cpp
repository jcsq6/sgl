#include "shaders/render_shader.h"
#include "utils/error.h"

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
	if (has_proj_uniform())
	{
		static const std::string val = "uniform mat4 sgl_Proj;";
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
	if (has_inverseModelView_uniform())
	{
		static const std::string val = "uniform mat4 sgl_InverseModelView;";
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
		(frag_src += type) += val;
	}
	if (has_textureMaterial_uniform())
	{
		static const std::string type = "struct sgl_TextureMaterial_t { sampler2D diffuse; sampler2D specular; float shininess; }; ";
		static const std::string val = "uniform sgl_TextureMaterial_t sgl_TextureMaterial;";
		(vertex_src += type) += val;
		(frag_src += type) += val;
	}
	if (has_globalLight_uniform())
	{
		static const std::string type = "struct sgl_GlobalLight_t { vec3 ambient; };";
		static const std::string val = "uniform sgl_GlobalLight_t sgl_GlobalLight;";
		(vertex_src += type) += val;
		(frag_src += type) += val;
	}

	if (m_directionals.size())
		m_variables |= variables::sgl_DirectionalLights;
	if (has_directionalLights_uniform())
	{
		if (!m_directionals.size())
			m_directionals = { "sgl_DirectionalLights[0]" };
		static const std::string type = "struct sgl_DirectionalLight_t { vec3 ambient; vec3 diffuse; vec3 specular; vec3 direction; };";
		static const std::string val = "uniform int sgl_DirectionalLightsSize; uniform sgl_DirectionalLight_t sgl_DirectionalLights[";
		std::string tag = std::to_string(m_directionals.size()) + "];";
		((vertex_src += type) += val) += tag;
		((frag_src += type) += val) += tag;
	}

	if (m_positionals.size())
		m_variables |= variables::sgl_PositionalLights;
	if (has_positionalLights_uniform())
	{
		if (!m_positionals.size())
			m_positionals = { "sgl_PositionalLights[0]" };
		static const std::string type = "struct sgl_PositionalLight_t { vec3 ambient; vec3 diffuse; vec3 specular; vec3 position; float constant; float linear; float quadratic; };";
		static const std::string val = "uniform int sgl_PostionalLightsSize; uniform sgl_PositionalLight_t sgl_PositionalLights[";
		std::string tag = std::to_string(m_positionals.size()) + "];";
		((vertex_src += type) += val) += tag;
		((frag_src += type) += val) += tag;
	}

	if (m_spotlights.size())
		m_variables |= variables::sgl_Spotlights;
	if (has_spotlights_uniform())
	{
		if (!m_spotlights.size())
			m_spotlights = { "sgl_Spotlights[0]" };
		static const std::string type = "struct sgl_Spotlight_t { vec3 ambient; vec3 diffuse; vec3 specular; vec3 direction; vec3 position; float cutoff_angle; float outer_cutoff_angle; float constant; float linear; float quadratic; };";
		static const std::string val = "uniform int sgl_SpotlightsSize; uniform sgl_Spotlight_t sgl_Spotlights[";
		std::string tag = std::to_string(m_spotlights.size()) + "];";
		((vertex_src += type) += val) += tag;
		((frag_src += type) += val) += tag;
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

render_shader phong_shader(unsigned int num_directional, unsigned int num_positional, unsigned int num_spotlights, unsigned int variables)
{
	constexpr int includes = variables::sgl_Model | variables::sgl_View | variables::sgl_Proj | variables::sgl_Pos | variables::sgl_Normal | variables::sgl_VertNormal | variables::sgl_InverseModelView;

	variables |= includes;

	bool has_world_pos = num_positional || num_spotlights;

	if ((variables & variables::sgl_TextureMaterial) && (variables & variables::sgl_Material))
	{
		detail::log_error(error("Cannot have both texture material and material in one lighting shader.", error_code::invalid_argument));
		return {};
	}

	if (!(variables & variables::sgl_TextureMaterial) && !(variables & variables::sgl_Material))
	{
		detail::log_error(error("Must have either texture material or material in a lighting shader.", error_code::invalid_argument));
		return {};
	}

	std::string vertex = "out vec3 view_pos;";
	std::string fragment = "in vec3 view_pos;";

	const std::string *ambient_material_name;
	const std::string *diffuse_material_name;
	const std::string *specular_material_name;
	const std::string *name;
	if (variables & variables::sgl_TextureMaterial)
	{
		variables |= variables::sgl_TextPos | variables::sgl_VertTextPos;
		static const std::string diffuse_name = "vec3(texture(sgl_TextureMaterial.diffuse, sgl_VertTextPos))";
		static const std::string specular_name = "vec3(texture(sgl_TextureMaterial.specular, sgl_VertTextPos))";
		static const std::string _name = "sgl_TextureMaterial";
		ambient_material_name = &diffuse_name;
		diffuse_material_name = &diffuse_name;
		specular_material_name = &specular_name;
		name = &_name;
	}
	else
	{
		static const std::string ambient_name = "sgl_Material.ambient";
		static const std::string diffuse_name = "sgl_Material.diffuse";
		static const std::string specular_name = "sgl_Material.specular";
		static const std::string _name = "sgl_Material";
		ambient_material_name = &ambient_name;
		diffuse_material_name = &diffuse_name;
		specular_material_name = &specular_name;
		name = &_name;
	}
	
	if (has_world_pos)
	{
		vertex +=
			"out vec3 world_pos;"
			"void main(){"
			"world_pos = (sgl_Model * vec4(sgl_Pos, 1.0)).xyz;";

		fragment += "in vec3 world_pos;";
	}
	else
	{
		vertex += "void main(){";
	}

	if (variables & variables::sgl_TextureMaterial)
		vertex += "sgl_VertTextPos = sgl_TextPos;";

	vertex += 
		"view_pos = (sgl_View * sgl_Model * vec4(sgl_Pos, 1.0)).xyz;"
		"sgl_VertNormal = normalize(mat3(transpose(sgl_InverseModelView)) * sgl_Normal);"
		"gl_Position = sgl_Proj * sgl_View * sgl_Model * vec4(sgl_Pos, 1.0);}";
	

	render_shader res;
	if (num_directional)
	{
		res.set_num_directionalLights(num_directional);
		fragment +=
			"vec3 calc_directional(int i){"

			"vec3 ambient = sgl_DirectionalLights[i].ambient *";
		fragment += *ambient_material_name;
		fragment += ';';
		
		fragment += 
			"vec3 light_dir = (sgl_View * vec4(-sgl_DirectionalLights[i].direction, 0.0)).xyz;"

			"float diffuse_factor = max(dot(sgl_VertNormal, light_dir), 0.0);"
			"vec3 diffuse = diffuse_factor * sgl_DirectionalLights[i].diffuse *";
		fragment += *diffuse_material_name;
		fragment += ';';

		fragment +=
			"vec3 view_dir = normalize(-view_pos);"
			"vec3 reflect_dir = reflect(-light_dir, sgl_VertNormal);"
			"float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.0),";
		fragment += *name;

		fragment +=
			".shininess);"
			"vec3 specular = specular_factor * sgl_DirectionalLights[i].specular * ";
		fragment += *specular_material_name;
		fragment += ';';

		fragment +=
			"return (ambient + diffuse + specular);}";
	}

	if (num_positional)
	{
		res.set_num_positionalLights(num_positional);
		fragment +=
			"vec3 calc_positional(int i){"

			"vec3 ambient = sgl_PositionalLights[i].ambient *";
		fragment += *ambient_material_name;
		fragment += ';';

		fragment +=
			"vec3 view_light_pos = (sgl_View * vec4(sgl_PositionalLights[i].position, 1.0)).xyz;"
			"vec3 light_dir = normalize(view_light_pos - view_pos);"

			"float diffuse_factor = max(dot(sgl_VertNormal, light_dir), 0.0);"
			"vec3 diffuse = diffuse_factor * sgl_PositionalLights[i].diffuse *";
		fragment += *diffuse_material_name;
		fragment += ';';

		fragment +=

			"vec3 view_dir = normalize(-view_pos);"
			"vec3 reflect_dir = reflect(-light_dir, sgl_VertNormal);"
			"float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.0),";
		fragment += *name;
			
		fragment +=
			".shininess);"
			"vec3 specular = specular_factor * sgl_PositionalLights[i].specular *";
		fragment += *specular_material_name;
		fragment += ';';

		fragment +=
			"float distance = length(sgl_PositionalLights[i].position - world_pos);"
			"float intensity = 1.0 / (sgl_PositionalLights[i].constant + sgl_PositionalLights[i].linear * distance + sgl_PositionalLights[i].quadratic * distance * distance);"
			"return intensity * (ambient + diffuse + specular);}";
	}

	if (num_spotlights)
	{
		res.set_num_spotlights(num_spotlights);
		fragment +=
			"vec3 calc_spotlight(int i) {"
			"vec3 ambient = sgl_Spotlights[i].ambient *";
		fragment += *ambient_material_name;
		fragment += ';';
		
		fragment +=
			"vec3 view_light_pos = (sgl_View * vec4(sgl_Spotlights[i].position, 1.0)).xyz;"
			"vec3 light_dir = normalize(view_light_pos - view_pos);"

			"float diffuse_factor = max(dot(sgl_VertNormal, light_dir), 0.0);"
			"vec3 diffuse = diffuse_factor * sgl_Spotlights[i].diffuse *";
		fragment += *diffuse_material_name;
		fragment += ';';

		fragment +=
			"vec3 view_dir = normalize(-view_pos);"
			"vec3 reflect_dir = reflect(-light_dir, sgl_VertNormal);"
			"float specular_factor = pow(max(dot(view_dir, reflect_dir), 0.0),";
		fragment += *name;
			
		fragment +=
			".shininess);"
			"vec3 specular = specular_factor * sgl_Spotlights[i].specular *";
		fragment += *specular_material_name;
		fragment += ';';

		fragment +=
			"vec3 world_light_dir = sgl_Spotlights[i].position - world_pos;"
			"float costheta = dot(normalize(world_light_dir), normalize(-sgl_Spotlights[i].direction));"
			"float diff = sgl_Spotlights[i].cutoff_angle - sgl_Spotlights[i].outer_cutoff_angle;"
			"float falloff_factor = clamp((costheta - sgl_Spotlights[i].outer_cutoff_angle) / diff, 0.0, 1.0);"

			"float distance = length(world_light_dir);"
			"float intensity = 1.0 / (sgl_Spotlights[i].constant + sgl_Spotlights[i].linear * distance + sgl_Spotlights[i].quadratic * distance * distance);"

			"return intensity * (ambient + falloff_factor * (diffuse + specular)); }";
	}

	fragment +=
		"void main() {"
		"vec3 results = vec3(0, 0, 0);";

	if (num_directional)
		fragment += "for (int i = 0; i < sgl_DirectionalLightsSize; ++i) { results += calc_directional(i); }";
	if (num_positional)
		fragment += "for (int i = 0; i < sgl_PostionalLightsSize; ++i) { results += calc_positional(i); }";
	if (num_spotlights)
		fragment += "for (int i = 0; i < sgl_SpotlightsSize; ++i) { results += calc_spotlight(i); }";

	fragment += "sgl_OutColor = vec4(results, 1.0); }";

	res.generate_shader(vertex, fragment, variables);

	return res;
}
SGL_END