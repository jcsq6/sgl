#pragma once
#include "shaders.h"
#include <vector>

SGL_BEG

// corresponding type of uniform in glsl shader must have the same member names as in this type
struct material : uniform_type
{
	// what color reflects under ambient lighting (normally the surface's color)
	vec3 ambient;
	// what color reflects under diffuse lighting (normally the surface's color)
	vec3 diffuse;
	// what color the specular highlight reflects
	vec3 specular;
	float shininess;

	void send(shader &program, const std::string &name) const override;
};

// corresponding type of uniform in glsl shader must have the same member names as in this type
struct texture_material : uniform_type
{
	using sampler2D = int;
	// texture
	sampler2D diffuse;
	// what color the specular highlight reflects
	sampler2D specular;
	float shininess;

	void send(shader &program, const std::string &name) const override;
};

// corresponding type of uniform in glsl shader must have the same member names as in this type
struct global_light : uniform_type
{
	vec3 ambient;

	void send(shader &program, const std::string &name) const override;
};

//// corresponding type of uniform in glsl shader must have the same member names as in this type
//struct basic_light : uniform_type
//{
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//
//	void send(shader &program, const std::string &name) const override;
//};

// corresponding type of uniform in glsl shader must have the same member names as in this type
struct directional_light : uniform_type
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;

	void send(shader &program, const std::string &name) const override;
};

// corresponding type of uniform in glsl shader must have the same member names as in this type
struct positional_light : uniform_type
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;

	// calculated with formula 1 / (constant + linear * distance + quadratic * distance * distance)

	// values above 1 will lower light intensity, values under one will increase light intensity
	float constant;

	// changes the slope of intensity decrease over time
	float linear;

	// values close to 0 will increase the range of the light
	// larger values decrease the range
	float quadratic;

	void send(shader &program, const std::string &name) const override;
};

// corresponding type of uniform in glsl shader must have the same member names as in this type
struct spotlight : uniform_type
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	vec3 position;
	float cutoff_angle;
	float outer_cutoff_angle;
	
	// calculated with formula 1 / (constant + linear * distance + quadratic * distance * distance)

	// values above 1 will lower light intensity, values under one will increase light intensity
	float constant;

	// changes the slope of intensity decrease over time
	float linear;

	// values close to 0 will increase the range of the light
	// larger values decrease the range
	float quadratic;

	void send(shader &program, const std::string &name) const override;
};

class lighting_engine
{
public:
	inline lighting_engine() : m_global{}, m_has_global{} {}

	inline bool has_global_light() const { return m_has_global; }
	inline void set_global_light(global_light light) { m_has_global = true; m_global = light; }
	inline void remove_global_light() { m_has_global = false; }
	inline global_light get_global_light() const { return m_global; }

	inline bool has_directional_lights() const { return m_directional.size(); }
	inline bool has_positional_lights() const { return m_positional.size(); }
	inline bool has_spotlights() const { return m_spotlight.size(); }

	inline std::size_t directional_lights_size() const { return m_directional.size(); }
	inline std::size_t positional_lights_size() const { return m_positional.size(); }
	inline std::size_t spotlights_size() const { return m_spotlight.size(); }

	inline void add_directional_light(const directional_light &light) { m_directional.push_back(light); }
	inline void add_positional_light(const positional_light &light) { m_positional.push_back(light); }
	inline void add_spotlight(const spotlight &light) { m_spotlight.push_back(light); }

	inline void remove_directional_light(std::vector<directional_light>::const_iterator it) { m_directional.erase(it); }
	inline void remove_positional_light(std::vector<positional_light>::const_iterator it) { m_positional.erase(it); }
	inline void remove_spotlight(std::vector<spotlight>::const_iterator it) { m_spotlight.erase(it); }

	inline auto directional_lights_begin() const { return m_directional.begin(); }
	inline auto directional_lights_end() const { return m_directional.end(); }

	inline auto positional_lights_begin() const { return m_positional.begin(); }
	inline auto positional_lights_end() const { return m_positional.end(); }

	inline auto spotlights_begin() const { return m_spotlight.begin(); }
	inline auto spotlights_end() const { return m_spotlight.end(); }
private:
	std::vector<directional_light> m_directional;
	std::vector<positional_light> m_positional;
	std::vector<spotlight> m_spotlight;

	global_light m_global;
	bool m_has_global;
};

SGL_END