#pragma once
#include "shaders.h"

SGL_BEG

namespace variables
{
	enum variable_type
	{
		// uniform for constant color
		sgl_Color = 0x0001,
		// uniform for view matrix
		sgl_View = 0x0002,
		// uniform for model matrix
		sgl_Model = 0x0004,
		// uniform for projection matrix
		sgl_Proj = 0x0008,
		// uniform for proj * view * model matrix
		sgl_ModelViewProj = 0x0010,
		// uniform for texture 
		sgl_Texture = 0x0020,
		// attribute for vertex input into vertex shader
		sgl_Pos = 0x0040,
		// attribute for normal input into vertex shader
		sgl_Normal = 0x0080,
		// attribute for color input into vertex shader
		sgl_ColorAttrib = 0x0100,
		// attribute for texture coordinates into vertex shader
		sgl_TextPos = 0x0200,
		// output of vertex shader/ input to fragment shader for vertex position
		sgl_VertPos = 0x0400,
		// output of vertex shader/ input to fragment shader for vertex normal
		sgl_VertNormal = 0x0800,
		// output of vertex shader/ input to fragment shader for vertex color
		sgl_VertColor = 0x1000,
		// output of vertex shader/ input to fragment shader for texture coordinates
		sgl_VertTextPos = 0x2000,
	};

	template <variable_type type>
	std::string variable_name;

	template <> std::string variable_name<variables::sgl_Color> = "sgl_Color";
	template <> std::string variable_name<variables::sgl_View> = "sgl_View";
	template <> std::string variable_name<variables::sgl_Model> = "sgl_Model";
	template <> std::string variable_name<variables::sgl_Proj> = "sgl_Proj";
	template <> std::string variable_name<variables::sgl_ModelViewProj> = "sgl_ModelViewProj";
	template <> std::string variable_name<variables::sgl_Texture> = "sgl_Texture";
	template <> std::string variable_name<variables::sgl_Pos> = "sgl_Pos";
	template <> std::string variable_name<variables::sgl_Normal> = "sgl_Normal";
	template <> std::string variable_name<variables::sgl_ColorAttrib> = "sgl_ColorAttrib";
	template <> std::string variable_name<variables::sgl_TextPos> = "sgl_TextPos";
	template <> std::string variable_name<variables::sgl_VertPos> = "sgl_VertPos";
	template <> std::string variable_name<variables::sgl_VertNormal> = "sgl_VertNormal";
	template <> std::string variable_name<variables::sgl_VertColor> = "sgl_VertColor";
	template <> std::string variable_name<variables::sgl_VertTextPos> = "sgl_VertTextPos";
}

/// <summary>
/// Standardized shader program class with standard variables that must be used
/// TODO: add ModelView uniform
/// OPTIONAL VARIABLES
/// Standardized uniforms
///		vec4 sgl_Color; // contains a constant color
///		mat4 sgl_View; // contains the view matrix
///		mat4 sgl_Model; // contains the model matrix
///		mat4 sgl_Proj; // contains the projection matrix
///		mat4 sgl_ModelViewProj; // contains the model-view-projection matrix
///		sampler2D sgl_Texture; // contains the texture index
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
	inline render_shader() : m_shader{}, m_includes{} {}

	/// <summary>
	/// generate a custom shader according to bitmask includes
	/// </summary>
	/// <param name="vertex">Vertex shader code including main function. Uniforms, version, in/out variables, and attributes should not be included</param>
	/// <param name="fragment">Fragment shader code including main function. Uniforms, version, and in/out variables should not be included</param>
	/// <param name="includes">Bit mask of all variables to be included. Fields are defined in variables::variable_type</param>
	inline render_shader(const std::string &vertex, const std::string &fragment, unsigned int includes) : render_shader() { generate_shader(vertex, fragment, includes); }
	
	/// <summary>
	/// generate a custom shader according to bitmask includes
	/// </summary>
	/// <param name="vertex">Vertex shader code including main function. Uniforms, version, in/out variables, and attributes should not be included</param>
	/// <param name="fragment">Fragment shader code including main function. Uniforms, version, and in/out variables should not be included</param>
	/// <param name="includes">Bit mask of all variables to be included. Fields are defined in variables::variable_type</param>
	void generate_shader(const std::string &vertex, const std::string &fragment, unsigned int includes);

	inline void set_color_uniform(vec4 color) { m_shader.set_uniform(variables::variable_name<variables::sgl_Color>, color); }
	inline void set_view_uniform(const mat4 &view) { m_shader.set_uniform(variables::variable_name<variables::sgl_View>, view); }
	inline void set_model_uniform(const mat4 &model) { m_shader.set_uniform(variables::variable_name<variables::sgl_Model>, model); }
	inline void set_proj_uniform(const mat4 &proj) { m_shader.set_uniform(variables::variable_name<variables::sgl_Proj>, proj); }
	inline void set_modelViewProj_uniform(const mat4 &modelViewProj) { m_shader.set_uniform(variables::variable_name<variables::sgl_ModelViewProj>, modelViewProj); }
	inline void set_texture_uniform(const gtexture &texture) { m_shader.set_uniform(variables::variable_name<variables::sgl_Texture>, texture); }

	inline bool has_color_uniform() const { return m_includes & variables::sgl_Color; }
	inline bool has_view_uniform() const { return m_includes & variables::sgl_View; }
	inline bool has_model_uniform() const { return m_includes & variables::sgl_Model; }
	inline bool has_proj_uniform() const { return m_includes & variables::sgl_Proj; }
	inline bool has_modelViewProj_uniform() const { return m_includes & variables::sgl_ModelViewProj; }
	inline bool has_texture_uniform() const { return m_includes & variables::sgl_Texture; }
	inline bool has_pos_attribute() const { return m_includes & variables::sgl_Pos; }
	inline bool has_normal_attribute() const { return m_includes & variables::sgl_Normal; }
	inline bool has_color_attribute() const { return m_includes & variables::sgl_ColorAttrib; }
	inline bool has_textPos_attribute() const { return m_includes & variables::sgl_TextPos; }
	inline bool has_pos_out_var() const { return m_includes & variables::sgl_VertPos; }
	inline bool has_normal_out_var() const { return m_includes & variables::sgl_VertNormal; }
	inline bool has_color_out_var() const { return m_includes & variables::sgl_VertColor; }
	inline bool has_textPos_out_var() const { return m_includes & variables::sgl_VertTextPos; }

	inline void bind() const { m_shader.bind(); }

	static unsigned int pos_attribute_loc;
	static unsigned int normal_attribute_loc;
	static unsigned int color_attribute_loc;
	static unsigned int textPos_attribute_loc;

private:
	shader m_shader;
	unsigned int m_includes;
};
SGL_END