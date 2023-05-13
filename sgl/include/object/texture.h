#pragma once
#include "macro.h"
#include "gl_object.h"
#include "context_lock/context_lock.h"

#include <string>
#include <GL/glew.h>

SGL_BEG
class texture : public gl_object
{
	GLuint id;
	int width;
	int height;
	int nr_channels;

public:
	inline texture() : id{}, width{}, height{}, nr_channels{} {}
	inline ~texture()
	{
		destroy();
	}

	inline texture(const std::string &file_name, GLenum target_format) : id{}
	{
		load(file_name, target_format);
	}

	inline texture(GLenum target_format, const void *data, GLsizei width, GLsizei height, int channel_count, bool flip = true) : id{}
	{
		load(target_format, data, width, height, channel_count, flip);
	}

	inline texture(texture &&other) noexcept : id{other.id}, width{other.width}, height{other.height}, nr_channels{other.nr_channels}
	{
		other.id = other.width = other.height = other.nr_channels = 0;
	}

	inline texture &operator=(texture &&other) noexcept
	{
		destroy();
		id = other.id;
		other.id = other.width = other.height = other.nr_channels = 0;
		return *this;
	}

	inline void generate() override
	{
		destroy();
		glGenTextures(1, &id);
	}

	// make sure to activate texture unit before this
	inline void use() const override
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	inline void destroy() override
	{
		glDeleteTextures(1, &id);
	}

	inline unsigned int index() const override
	{
		return id;
	}

	inline void set_parameter(GLenum property, GLint value)
	{
		detail::texture_lock lock;
		use();
		glTexParameteri(GL_TEXTURE_2D, property, value);
	}

	inline void set_parameter(GLenum property, const GLint *value)
	{
		detail::texture_lock lock;
		use();
		glTexParameteriv(GL_TEXTURE_2D, property, value);
	}

	inline void set_parameter(GLenum property, GLfloat value)
	{
		detail::texture_lock lock;
		use();
		glTexParameterf(GL_TEXTURE_2D, property, value);
	}

	inline void set_parameter(GLenum property, const GLfloat *value)
	{
		detail::texture_lock lock;
		use();
		glTexParameterfv(GL_TEXTURE_2D, property, value);
	}

	inline int get_width() const
	{
		return width;
	}

	inline int get_height() const
	{
		return height;
	}

	inline int get_channels() const
	{
		return nr_channels;
	}

	void load(const std::string &file_name, GLenum target_format);

	void load(GLenum target_format, const void *data, GLsizei width, GLsizei height, int channel_count, bool flip = true);
	void reserve(GLenum target_format, GLsizei width, GLsizei height);

	inline static void quit()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	inline static void activate_unit(int unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
	}
};
SGL_END