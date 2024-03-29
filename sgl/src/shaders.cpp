#include "shaders/shaders.h"
#include "shaders/lighting.h"
#include "context_lock/context_lock.h"
#include "utils/error.h"
#include <GL/glew.h>

SGL_BEG
class shader_source
{
	GLuint id;

public:
	inline ~shader_source()
	{
		glDeleteShader(id);
		id = 0;
	}

	inline shader_source(shader_source &&o) noexcept : id{o.id}
	{
		o.id = 0;
	}
	inline shader_source &operator=(shader_source &&o) noexcept
	{
		glDeleteShader(id);
		id = o.id;
		o.id = 0;
		return *this;
	}

	inline shader_source(int shader_type) : id{glCreateShader(shader_type)}
	{
	}

	shader_source(const shader_source &) = delete;
	shader_source &operator=(const shader_source &) = delete;

	inline void attach_source(const std::string &src) const
	{
		auto s = src.c_str();
		glShaderSource(id, 1, &s, nullptr);
	}

	inline void compile() const
	{
		glCompileShader(id);

		GLint completed{};

		glGetShaderiv(id, GL_COMPILE_STATUS, &completed);
		if (completed == GL_FALSE)
		{
			std::string message = "Shader compilation failed.";
			int length{};
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			if (length)
			{
				message += " Log: ";
				std::size_t old_len = message.size();
				message.resize(old_len + length);
				int d{};
				glGetShaderInfoLog(id, length, &d, message.data() + old_len);
			}

			detail::log_error(error(std::move(message), error_code::shader_compilation_failure));
		}
	}

	inline void attach_toProgram(GLuint prog)
	{
		glAttachShader(prog, id);
		glDeleteShader(id);
		id = 0;
	}
};

template <typename... Ts>
unsigned int get_program(Ts &&...shaders)
{
	unsigned int id = glCreateProgram();

	(shaders.attach_toProgram(id), ...);

	glLinkProgram(id);

	GLint completed = 0;

	glGetProgramiv(id, GL_LINK_STATUS, &completed);
	if (completed == GL_FALSE)
	{
		std::string message = "Program compilation failed.";
		int length{};
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		if (length)
		{
			message += " Log: ";
			std::size_t old_len = message.size();
			message.resize(old_len + length);
			int d{};
			glGetProgramInfoLog(id, length, &d, message.data() + old_len);
		}

		detail::log_error(error(std::move(message), error_code::shader_compilation_failure));
	}

	return id;
}

void shader::load_from_memory(const std::string &vertex_source, const std::string &geometry_source, const std::string &fragment_source)
{
	destroy();

	shader_source vertex(GL_VERTEX_SHADER);
	vertex.attach_source(vertex_source);
	vertex.compile();

	shader_source geometry(GL_GEOMETRY_SHADER);
	geometry.attach_source(geometry_source);
	geometry.compile();

	shader_source fragment(GL_FRAGMENT_SHADER);
	fragment.attach_source(fragment_source);
	fragment.compile();

	id = get_program(vertex, geometry, fragment);
}

void shader::load_from_memory(const std::string &vertex_source, const std::string &fragment_source)
{
	destroy();

	shader_source vertex(GL_VERTEX_SHADER);
	vertex.attach_source(vertex_source);
	vertex.compile();

	shader_source fragment(GL_FRAGMENT_SHADER);
	fragment.attach_source(fragment_source);
	fragment.compile();

	id = get_program(vertex, fragment);
}

void shader::set_uniform(const std::string &name, float val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniform1f(get_loc(name), val);
}
void shader::set_uniform(const std::string &name, vec2 val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniform2fv(get_loc(name), 1, value(val));
}
void shader::set_uniform(const std::string &name, vec3 val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniform3fv(get_loc(name), 1, value(val));
}
void shader::set_uniform(const std::string &name, vec4 val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniform4fv(get_loc(name), 1, value(val));
}

void shader::set_uniform(const std::string &name, int val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniform1i(get_loc(name), val);
}
void shader::set_uniform(const std::string &name, ivec2 val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniform2iv(get_loc(name), 1, value(val));
}
void shader::set_uniform(const std::string &name, ivec3 val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniform3iv(get_loc(name), 1, value(val));
}
void shader::set_uniform(const std::string &name, ivec4 val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniform4iv(get_loc(name), 1, value(val));
}

void shader::set_uniform(const std::string &name, const mat3 &val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniformMatrix3fv(get_loc(name), 1, GL_FALSE, value(val));
}
void shader::set_uniform(const std::string &name, const mat4 &val)
{
	detail::shader_lock context;
	glUseProgram(id);
	glUniformMatrix4fv(get_loc(name), 1, GL_FALSE, value(val));
}

void shader::set_uniform(const std::string &name, const texture &val)
{
	textures[get_loc(name)] = &val;
}

void shader::bind()
{
	glUseProgram(id);

	auto it = textures.begin();
	auto end = textures.end();
	for (int i = 0; it != end; ++i, ++it)
	{
		// send uniform location
		glUniform1i(it->first, i);
		// activate texture
		texture::activate_unit(i);
		// bind corresponding texture
		it->second->use();
	}
}

void shader::destroy()
{
	glDeleteProgram(id);
}

int shader::get_loc(const std::string &name)
{
	return glGetUniformLocation(id, name.c_str());
}

void material::send(shader &program, const std::string &name) const
{
	detail::shader_lock context;

	glUseProgram(program.index());

	std::string member = name + ".ambient";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(ambient));

	member = name;
	member += ".diffuse";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(diffuse));

	member = name;
	member += ".specular";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(specular));

	member = name;
	member += ".shininess";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), shininess);
}

void texture_material::send(shader &program, const std::string &name) const
{
	detail::shader_lock context;

	glUseProgram(program.index());

	std::string member;
	if (diffuse)
	{
		member = name + ".diffuse";
		program.set_uniform(member, *diffuse);
	}
	
	if (specular)
	{
		member = name;
		member += ".specular";
		program.set_uniform(member, *specular);
	}

	member = name;
	member += ".shininess";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), shininess);
}

void global_light::send(shader &program, const std::string &name) const
{
	detail::shader_lock context;

	glUseProgram(program.index());

	std::string member = name + ".ambient";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(ambient));
}

//void basic_light::send(shader &program, const std::string &name) const
//{
//	detail::shader_lock context;
//
//	glUseProgram(program.index());
//
//	std::string member = name + ".ambient";
//	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(ambient));
//
//	member = name;
//	member += ".diffuse";
//	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(diffuse));
//
//	member = name;
//	member += ".specular";
//	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(specular));
//}

void directional_light::send(shader &program, const std::string &name) const
{
	detail::shader_lock context;

	glUseProgram(program.index());

	std::string member = name + ".ambient";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(ambient));

	member = name;
	member += ".diffuse";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(diffuse));

	member = name;
	member += ".specular";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(specular));

	member = name;
	member += ".direction";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(direction));
}

void positional_light::send(shader &program, const std::string &name) const
{
	detail::shader_lock context;

	glUseProgram(program.index());

	std::string member = name + ".ambient";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(ambient));

	member = name;
	member += ".diffuse";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(diffuse));

	member = name;
	member += ".specular";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(specular));

	member = name;
	member += ".position";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(position));

	member = name;
	member += ".constant";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), constant);

	member = name;
	member += ".linear";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), linear);

	member = name;
	member += ".quadratic";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), quadratic);
}

void spotlight::send(shader &program, const std::string &name) const
{
	detail::shader_lock context;

	glUseProgram(program.index());

	std::string member = name + ".ambient";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(ambient));

	member = name;
	member += ".diffuse";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(diffuse));

	member = name;
	member += ".specular";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(specular));

	member = name;
	member += ".direction";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(direction));

	member = name;
	member += ".position";
	glUniform3fv(glGetUniformLocation(program.index(), member.c_str()), 1, value(position));

	member = name;
	member += ".cutoff_angle";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), cutoff_angle);


	member = name;
	member += ".outer_cutoff_angle";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), outer_cutoff_angle);

	member = name;
	member += ".constant";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), constant);

	member = name;
	member += ".linear";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), linear);

	member = name;
	member += ".quadratic";
	glUniform1f(glGetUniformLocation(program.index(), member.c_str()), quadratic);
}

SGL_END