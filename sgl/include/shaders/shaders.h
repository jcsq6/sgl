#pragma once
#include "macro.h"
#include "object/gtexture.h"
#include "math/mat.h"

#include <string>
#include <map>

SGL_BEG

/// <summary>
/// OpenGL shader class with uniform support
/// </summary>
class shader
{
public:
	inline shader() : id{}, textures{} {}

	inline ~shader()
	{
		destroy();
		id = 0;
	}

	inline shader(shader &&o) noexcept : id{o.id}, textures{std::move(o.textures)}
	{
		o.id = 0;
	}

	inline shader &operator=(shader &&o) noexcept
	{
		if (id)
			destroy();
		id = o.id;
		o.id = 0;
		textures = std::move(o.textures);
		return *this;
	}

	shader(const shader &) = delete;
	shader &operator=(const shader &) = delete;

	void load_from_memory(const std::string &vertex_source, const std::string &geometry_source, const std::string &fragment_source);
	void load_from_memory(const std::string &vertex_source, const std::string &fragment_source);

	void set_uniform(const std::string &name, float val);
	void set_uniform(const std::string &name, vec2 val);
	void set_uniform(const std::string &name, vec3 val);
	void set_uniform(const std::string &name, vec4 val);

	void set_uniform(const std::string &name, int val);
	void set_uniform(const std::string &name, ivec2 val);
	void set_uniform(const std::string &name, ivec3 val);
	void set_uniform(const std::string &name, ivec4 val);

	void set_uniform(const std::string &name, const mat3 &val);
	void set_uniform(const std::string &name, const mat4 &val);

	void set_uniform(const std::string &name, const gtexture &val);

	void bind() const;

private:
	unsigned int id;

	std::map<int, const gtexture *> textures;

	void destroy();
	int get_loc(const std::string &name);
};

SGL_END