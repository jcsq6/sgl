#include "object/gtexture.h"
#include "math/vec.h"
#include <stdexcept>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "../dep/stb_image.h"

SGL_BEG

void set_defaults()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	static vec4 border_col{0, 0, 0, 0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, value(border_col));
}

void gtexture::load(const std::string &file_name, GLenum target_format)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(file_name.data(), &width, &height, &nr_channels, 0);

	if (!data)
		throw std::runtime_error("Couldn't open image " + file_name);

	GLint format;

	switch (nr_channels)
	{
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		throw std::runtime_error("Unrecognized image format for image " + file_name);
	}

	switch (target_format)
	{
	case GL_DEPTH_COMPONENT:
	case GL_RED:
		nr_channels = 1;
		break;
	case GL_RG:
		nr_channels = 2;
		break;
	case GL_RGB:
		nr_channels = 3;
		break;
	case GL_RGBA:
		nr_channels = 4;
		break;
	default:
		throw std::runtime_error("Unrecognized image format");
	}

	if (!id)
		generate();
	
	detail::texture_lock lock;

	use();
	glTexImage2D(GL_TEXTURE_2D, 0, target_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	set_defaults();

	stbi_image_free(data);
}

void gtexture::load(GLenum target_format, const void *data, GLsizei width, GLsizei height, int channel_count, bool flip)
{
	this->width = width;
	this->height = height;

	GLenum pixel_format;

	switch (channel_count)
	{
	case 1:
		pixel_format = GL_RED;
		break;
	case 2:
		pixel_format = GL_RG;
		break;
	case 3:
		pixel_format = GL_RGB;
		break;
	case 4:
		pixel_format = GL_RGBA;
		break;
	default:
		throw std::runtime_error("Unrecognized image format");
	}

	switch (target_format)
	{
	case GL_DEPTH_COMPONENT:
	case GL_RED:
		nr_channels = 1;
		break;
	case GL_RG:
		nr_channels = 2;
		break;
	case GL_RGB:
		nr_channels = 3;
		break;
	case GL_RGBA:
		nr_channels = 4;
		break;
	default:
		throw std::runtime_error("Unrecognized image format");
	}

	if (flip)
	{
		int w = width * channel_count;

		const unsigned char *old_data = reinterpret_cast<const unsigned char *>(data);
		unsigned char *new_data = new unsigned char[height * w];

		for (int y = 0; y < height; ++y)
		{
			auto *in = old_data + (height - y - 1) * w;
			std::copy(in, in + w, new_data + y * w);
		}

		data = new_data;
	}

	if (!id)
		generate();
	
	detail::texture_lock lock;

	use();
	glTexImage2D(GL_TEXTURE_2D, 0, target_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	set_defaults();

	if (flip)
		delete[] reinterpret_cast<const unsigned char *>(data);
}

void gtexture::reserve(GLenum target_format, GLsizei width, GLsizei height)
{
	this->width = width;
	this->height = height;

	switch (target_format)
	{
	case GL_DEPTH_COMPONENT:
	case GL_RED:
		nr_channels = 1;
		break;
	case GL_RG:
		nr_channels = 2;
		break;
	case GL_RGB:
		nr_channels = 3;
		break;
	case GL_RGBA:
		nr_channels = 4;
		break;
	default:
		throw std::runtime_error("Unrecognized image format");
	}

	if (!id)
		generate();

	detail::texture_lock lock;

	use();
	glTexImage2D(GL_TEXTURE_2D, 0, target_format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	set_defaults();
}
SGL_END