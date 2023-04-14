#pragma once
#include "lighting.h"
#include <vector>

SGL_BEG

namespace variables
{
	enum variable_type
	{
		// uniform for constant color
		sgl_Color = 1,
		// uniform for view matrix
		sgl_View = sgl_Color << 1,
		// uniform for model matrix
		sgl_Model = sgl_View << 1,
		// uniform for projection matrix
		sgl_Proj = sgl_Model << 1,
		// uniform for view * model matrix
		sgl_ModelView = sgl_Proj << 1,
		// uniform for proj * view * model matrix
		sgl_ModelViewProj = sgl_ModelView << 1,
		// uniform for the inverse of the model view matrix
		sgl_InverseModelView = sgl_ModelViewProj << 1,
		// uniform for texture 
		sgl_Texture = sgl_InverseModelView << 1,

		// uniform for material
		// material type has the following form:
		// struct sgl_Material_t
		// {
		//	 vec3 ambient;
		//	 vec3 diffuse;
		//	 vec3 specular;
		//	 float shininess;
		// };
		sgl_Material = sgl_Texture << 1,

		// uniform for texture material
		// texture material type has the following form:
		// struct sgl_TextureMaterial_t
		// {
		//   sampler2D diffuse;
		//	 sampler2D specular;
		//	 float shininess;
		// };
		sgl_TextureMaterial = sgl_Material << 1,

		// uniform for global light
		// global light type has the following form:
		// struct sgl_GlobalLight_t
		// {
		//   vec3 ambient;
		// };
		sgl_GlobalLight = sgl_TextureMaterial << 1,

		// uniform for array of directional lights
		// array size is stored in uniform int sgl_DirectionalLightsSize
		// directional light type has the following form:
		// struct sgl_DirectionalLight_t
		// {
		//   vec3 ambient;
		//   vec3 diffuse;
		//   vec3 specular;
		//   vec3 direction;
		// };
		sgl_DirectionalLights = sgl_GlobalLight << 1,

		// uniform for array of positional lights
		// array size is stored in uniform int sgl_PostionalLightsSize
		// positional light type has the following form:
		// struct sgl_PositionalLight_t
		// {
		//   vec3 ambient;
		//	 vec3 diffuse;
		//	 vec3 specular;
		//	 vec3 position;
		//   float constant;
		//	 float linear;
		//	 float quadratic;
		// };
		sgl_PositionalLights = sgl_DirectionalLights << 1,

		// uniform for array of spotlights
		// array size is stored in uniform int sgl_SpotlightsSize
		// sptlight type has the following form:
		// struct sgl_Spotlight_t
		// {
		//	 vec3 ambient;
		//	 vec3 diffuse;
		//	 vec3 specular;
		//	 vec3 direction;
		//	 vec3 position;
		//	 float cutoff_angle;
		//	 float outer_cutoff_angle;
		//	 float constant;
		//	 float linear;
		//	 float quadratic;
		// };
		sgl_Spotlights = sgl_PositionalLights << 1,
		// attribute for vertex input into vertex shader
		sgl_Pos = sgl_Spotlights << 1,
		// attribute for normal input into vertex shader
		sgl_Normal = sgl_Pos << 1,
		// attribute for color input into vertex shader
		sgl_ColorAttrib = sgl_Normal << 1,
		// attribute for texture coordinates into vertex shader
		sgl_TextPos = sgl_ColorAttrib << 1,
		// output of vertex shader/ input to fragment shader for vertex position
		sgl_VertPos = sgl_TextPos << 1,
		// output of vertex shader/ input to fragment shader for vertex normal
		sgl_VertNormal = sgl_VertPos << 1,
		// output of vertex shader/ input to fragment shader for vertex color
		sgl_VertColor = sgl_VertNormal << 1,
		// output of vertex shader/ input to fragment shader for texture coordinates
		sgl_VertTextPos = sgl_VertColor << 1,
	};
}

/// <summary>
/// Standardized shader program class with standard variables
/// OPTIONAL VARIABLES
/// Standardized uniforms
///		vec4 sgl_Color; // contains a constant color
///		mat4 sgl_View; // contains the view matrix
///		mat4 sgl_Model; // contains the model matrix
///		mat4 sgl_Proj; // contains the projection matrix
///		mat4 sgl_ModelView; // containts the model-view matrix
///		mat4 sgl_ModelViewProj; // contains the model-view-projection matrix
///		mat4 sgl_InverseModelView; // contains the inverse of the model view matrix
///		sampler2D sgl_Texture; // contains the texture index
///		sgl_Material_t sgl_Material; // contains the material
///		sgl_TextureMaterial_t sgl_TextureMaterial; // contains the texture material
///		sgl_GlobalLight_t sgl_GlobalLight; // contains the global light
///		sgl_DirectionalLight_t sgl_DirectionalLights[sgl_DirectionalLightsSize]; // contains array of directional lights
///		sgl_PositionalLight_t sgl_PositionalLights[sgl_PostionalLightsSize]; // contains array of positional lights
///		sgl_Spotlight_t sgl_Spotlights[sgl_SpotlightsSize]; // contains array of spotlights
/// Standardized attributes for vertex shader:
///		vec3 sgl_Pos; // contains the vertex position
///		vec3 sgl_Normal; // contains the vertex normal
///		vec4 sgl_ColorAttrib; // contains the vertex color
///		vec2 sgl_TextPos; // contains the vertex texture coordinate
/// Standardized out variables for the vertex shader
///		vec3 sgl_VertPos; // contains the vertex position
///		vec3 sgl_VertNormal; // contains the vertex normal
///		vec4 sgl_VertColor; // contains the vertex color
///		vec2 sgl_VertTextPos; // contains the vertex texture coordinate
/// Standardized in variables for the fragment shader:
///		vec3 sgl_VertPos; // contains the vertex position
///		vec3 sgl_VertNormal; // contains the vertex normal
///		vec4 sgl_VertColor; // contains the vertex color
///		vec2 sgl_VertTextPos; // contains the vertex texture coordinate
/// NON-OPTIONAL VARIABLES
///	Standardized out variable for the fragment shader:
///		vec4 sgl_OutColor; // contains the fragment color
/// </summary>
class render_shader
{
public:
	inline render_shader() :
		m_variables{}
	{}

	/// <summary>
	/// generate a custom shader according to bitmask variables
	/// won't use lighting
	/// </summary>
	/// <param name="vertex">Vertex shader code including main function. Uniforms, version, in/out variables, and attributes should not be included</param>
	/// <param name="fragment">Fragment shader code including main function. Uniforms, version, and in/out variables should not be included</param>
	/// <param name="variables">Bit mask of all variables to be included. Fields are defined in variables::variable_type</param>
	inline render_shader(const std::string &vertex, const std::string &fragment, unsigned int variables) : render_shader() { generate_shader(vertex, fragment, variables); }

	/// <summary>
	/// generate a custom shader according to bitmask variables
	/// uses lighting
	/// </summary>
	/// <param name="vertex">Vertex shader code including main function. Uniforms, version, in/out variables, and attributes should not be included</param>
	/// <param name="fragment">Fragment shader code including main function. Uniforms, version, and in/out variables should not be included</param>
	/// <param name="num_directional">Number of directional lights to be used</param>
	/// <param name="num_positional">Number of positional lights to be used</param>
	/// <param name="num_spotlights">Number of spotlights to be used</param>
	/// <param name="variables">Bit mask of all variables to be included. Fields are defined in variables::variable_type</param>
	inline render_shader(const std::string &vertex, const std::string &fragment, unsigned int num_directional, unsigned int num_positional, unsigned int num_spotlights, unsigned int variables) : render_shader()
	{
		set_num_directionalLights(num_directional);
		set_num_positionalLights(num_positional);
		set_num_spotlights(num_spotlights);
		generate_shader(vertex, fragment, variables);
	}

	inline void set_num_directionalLights(unsigned int size)
	{
		if (size < m_directionals.size())
			m_directionals.resize(size);
		else
		{
			m_directionals.reserve(size);
			for (std::size_t i = m_directionals.size(); i != size; ++i)
				m_directionals.push_back("sgl_DirectionalLights[" + std::to_string(i) + ']');
		}
	}

	inline void set_num_positionalLights(unsigned int size)
	{
		if (size < m_positionals.size())
			m_positionals.resize(size);
		else
		{
			m_positionals.reserve(size);
			for (std::size_t i = m_positionals.size(); i != size; ++i)
				m_positionals.push_back("sgl_PositionalLights[" + std::to_string(i) + ']');
		}
	}

	inline void set_num_spotlights(unsigned int size)
	{
		if (size < m_spotlights.size())
			m_spotlights.resize(size);
		else
		{
			m_spotlights.reserve(size);
			for (std::size_t i = m_spotlights.size(); i != size; ++i)
				m_spotlights.push_back("sgl_Spotlights[" + std::to_string(i) + ']');
		}
	}

	inline unsigned int get_num_directionalLights() const { return static_cast<unsigned int>(m_directionals.size()); }
	inline unsigned int get_num_positionalLights() const { return static_cast<unsigned int>(m_positionals.size()); }
	inline unsigned int get_num_spotlights() const { return static_cast<unsigned int>(m_spotlights.size()); }
	
	/// <summary>
	/// generate a custom shader according to bitmask variables
	/// If there are more than one light (type of direcitonal/postional/spotlights) defined in the shader, but the respective bit is not set in variables, the bit will be added to variables
	/// </summary>
	/// <param name="vertex">Vertex shader code including main function. Uniforms, version, in/out variables, and attributes should not be included</param>
	/// <param name="fragment">Fragment shader code including main function. Uniforms, version, and in/out variables should not be included</param>
	/// <param name="variables">Bit mask of all variables to be included. Fields are defined in variables::variable_type</param>
	void generate_shader(const std::string &vertex, const std::string &fragment, unsigned int variables);

	inline void set_color_uniform(vec4 color) { m_shader.set_uniform("sgl_Color", color); }
	inline void set_view_uniform(const mat4 &view) { m_shader.set_uniform("sgl_View", view); }
	inline void set_model_uniform(const mat4 &model) { m_shader.set_uniform("sgl_Model", model); }
	inline void set_proj_uniform(const mat4 &proj) { m_shader.set_uniform("sgl_Proj", proj); }
	inline void set_modelView_uniform(const mat4 &modelView) { m_shader.set_uniform("sgl_ModelView", modelView); }
	inline void set_modelViewProj_uniform(const mat4 &modelViewProj) { m_shader.set_uniform("sgl_ModelViewProj", modelViewProj); }
	inline void set_inverseModelView_uniform(const mat4 &inverseModelView) { m_shader.set_uniform("sgl_InverseModelView", inverseModelView); }
	inline void set_texture_uniform(const texture &texture) { m_shader.set_uniform("sgl_Texture", texture); }

	inline void set_material_uniform(const material &mat) { m_shader.set_uniform("sgl_Material", mat); }
	inline void set_textureMaterial_uniform(const texture_material &mat) { m_shader.set_uniform("sgl_TextureMaterial", mat); }
	inline void set_globalLight_uniform(global_light light) { m_shader.set_uniform("sgl_GlobalLight", light); }
	
	inline void set_directionalLights_uniform(unsigned int i, const directional_light &light) { m_shader.set_uniform(m_directionals[i], light); }
	inline void set_positionalLights_uniform(unsigned int i, const positional_light &light) { m_shader.set_uniform(m_positionals[i], light); }
	inline void set_spotlights_uniform(unsigned int i, const spotlight &light) { m_shader.set_uniform(m_spotlights[i], light); }

	inline bool has_color_uniform() const { return m_variables & variables::sgl_Color; }
	inline bool has_view_uniform() const { return m_variables & variables::sgl_View; }
	inline bool has_model_uniform() const { return m_variables & variables::sgl_Model; }
	inline bool has_proj_uniform() const { return m_variables & variables::sgl_Proj; }
	inline bool has_modelView_uniform() const { return m_variables & variables::sgl_ModelView; }
	inline bool has_modelViewProj_uniform() const { return m_variables & variables::sgl_ModelViewProj; }
	inline bool has_inverseModelView_uniform() const { return m_variables & variables::sgl_InverseModelView; }
	inline bool has_texture_uniform() const { return m_variables & variables::sgl_Texture; }
	inline bool has_material_uniform() const { return m_variables & variables::sgl_Material; }
	inline bool has_textureMaterial_uniform() const { return m_variables & variables::sgl_TextureMaterial; }
	inline bool has_globalLight_uniform() const { return m_variables & variables::sgl_GlobalLight; }
	inline bool has_directionalLights_uniform() const { return m_variables & variables::sgl_DirectionalLights; }
	inline bool has_positionalLights_uniform() const { return m_variables & variables::sgl_PositionalLights; }
	inline bool has_spotlights_uniform() const { return m_variables & variables::sgl_Spotlights; }
	inline bool has_pos_attribute() const { return m_variables & variables::sgl_Pos; }
	inline bool has_normal_attribute() const { return m_variables & variables::sgl_Normal; }
	inline bool has_color_attribute() const { return m_variables & variables::sgl_ColorAttrib; }
	inline bool has_textPos_attribute() const { return m_variables & variables::sgl_TextPos; }
	inline bool has_pos_out_var() const { return m_variables & variables::sgl_VertPos; }
	inline bool has_normal_out_var() const { return m_variables & variables::sgl_VertNormal; }
	inline bool has_color_out_var() const { return m_variables & variables::sgl_VertColor; }
	inline bool has_textPos_out_var() const { return m_variables & variables::sgl_VertTextPos; }

	/// <summary>
	/// Set lighting uniforms as defined in engine. Will only set the amount of lights as defined in the shader, potentially ignoring some lights in engine
	/// </summary>
	/// <param name="engine">Lighting engine with uniforms</param>
	inline void set_lighting_uniforms(const lighting_engine &engine)
	{
		if (has_globalLight_uniform() && engine.has_global_light())
			set_globalLight_uniform(engine.get_global_light());

		if (has_directionalLights_uniform())
		{
			unsigned int min = engine.directional_lights_size() < get_num_directionalLights() ? static_cast<unsigned int>(engine.directional_lights_size()) : get_num_directionalLights();
			auto dir_lights = engine.directional_lights_begin();
			for (unsigned int i = 0; i < min; ++i)
				set_directionalLights_uniform(i, dir_lights[i]);
		}

		if (has_positionalLights_uniform())
		{
			unsigned int min = engine.positional_lights_size() < get_num_positionalLights() ? static_cast<unsigned int>(engine.positional_lights_size()) : get_num_positionalLights();
			auto dir_lights = engine.positional_lights_begin();
			for (unsigned int i = 0; i < min; ++i)
				set_positionalLights_uniform(i, dir_lights[i]);
		}

		if (has_spotlights_uniform())
		{
			unsigned int min = engine.spotlights_size() < get_num_spotlights() ? static_cast<unsigned int>(engine.spotlights_size()) : get_num_spotlights();
			auto dir_lights = engine.spotlights_begin();
			for (unsigned int i = 0; i < min; ++i)
				set_spotlights_uniform(i, dir_lights[i]);
		}
	}

	inline void bind()
	{
		static const std::string directionals_size = "sgl_DirectionalLightsSize";
		static const std::string positionals_size = "sgl_PostionalLightsSize";
		static const std::string spotlights_size = "sgl_SpotlightsSize";

		if (has_directionalLights_uniform())
			m_shader.set_uniform(directionals_size, static_cast<int>(m_directionals.size()));
		if (has_positionalLights_uniform())
			m_shader.set_uniform(positionals_size, static_cast<int>(m_positionals.size()));
		if (has_spotlights_uniform())
			m_shader.set_uniform(spotlights_size, static_cast<int>(m_spotlights.size()));

		m_shader.bind();
	}

	static unsigned int pos_attribute_loc;
	static unsigned int normal_attribute_loc;
	static unsigned int color_attribute_loc;
	static unsigned int textPos_attribute_loc;

private:
	shader m_shader;
	std::vector<std::string> m_directionals;
	std::vector<std::string> m_positionals;
	std::vector<std::string> m_spotlights;
	unsigned int m_variables;
};

render_shader phong_shader(unsigned int num_directional, unsigned int num_positional, unsigned int num_spotlights, unsigned int variables);

SGL_END