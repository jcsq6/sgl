#pragma once
#include "macro.h"
#include <GL/glew.h>

SGL_BEG

DETAIL_BEG

template <int pname>
class context_lock;

template <>
class context_lock<GL_CURRENT_PROGRAM>
{
public:
	context_lock() : prev{}
	{
		glGetIntegerv(GL_CURRENT_PROGRAM, (int*)&prev);
	}
	~context_lock()
	{
		glUseProgram(prev);
	}
private:
	GLuint prev;
};

template <>
class context_lock<GL_VERTEX_ARRAY_BINDING>
{
public:
	context_lock() : prev{}
	{
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (int*)&prev);
	}
	~context_lock()
	{
		glBindVertexArray(prev);
	}
private:
	GLuint prev;
};

template <>
class context_lock<GL_TEXTURE_BINDING_2D>
{
public:
	context_lock() : prev{}
	{
		glGetIntegerv(GL_TEXTURE_BINDING_2D, (int*)&prev);
	}
	~context_lock()
	{
		glBindTexture(GL_TEXTURE_2D, prev);
	}
private:
	GLuint prev;
};

template <>
class context_lock<GL_ARRAY_BUFFER_BINDING>
{
public:
	context_lock() : prev{}
	{
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (int*)&prev);
	}
	~context_lock()
	{
		glBindBuffer(GL_ARRAY_BUFFER, prev);
	}
private:
	GLuint prev;
};


// make sure ebo lock is declared before a vao lock, if neccessary
// if declared after, then the ebo lock will destruct before the vao lock, and will unbind the ebo from the vao
template <>
class context_lock<GL_ELEMENT_ARRAY_BUFFER_BINDING>
{
public:
	context_lock() : prev{}
	{
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (int*)&prev);
	}
	~context_lock()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prev);
	}
private:
	GLuint prev;
};

template <>
class context_lock<GL_FRAMEBUFFER_BINDING>
{
public:
	context_lock() : prev{}
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, (int*)&prev);
	}
	~context_lock()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, prev);
	}
private:
	GLuint prev;
};

template <>
class context_lock<GL_RENDERBUFFER_BINDING>
{
public:
	context_lock() : prev{}
	{
		glGetIntegerv(GL_RENDERBUFFER_BINDING, (int*)&prev);
	}
	~context_lock()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, prev);
	}
private:
	GLuint prev;
};

template <>
class context_lock<GL_BLEND>
{
public:
	context_lock() : prev{}
	{
		glGetBooleanv(GL_BLEND, &prev);
	}
	~context_lock()
	{
		if (prev)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}
private:
	GLboolean prev;
};

template <>
class context_lock<GL_VIEWPORT>
{
public:
	context_lock() : prev{}
	{
		glGetIntegerv(GL_VIEWPORT, prev);
	}
	~context_lock()
	{
		glViewport(prev[0], prev[1], prev[2], prev[3]);
	}
private:
	int prev[4];
};

template <>
class context_lock<GL_LINE_WIDTH>
{
public:
	context_lock() : prev{}
	{
		glGetFloatv(GL_LINE_WIDTH, &prev);
	}
	~context_lock()
	{
		glLineWidth(prev);
	}
private:
	float prev;
};

template <>
class context_lock<GL_CULL_FACE>
{
public:
	context_lock() : prev_state{}, prev_mode{}
	{
		glGetBooleanv(GL_CULL_FACE, &prev_state);
		glGetIntegerv(GL_CULL_FACE_MODE, &prev_mode);
	}
	~context_lock()
	{
		glCullFace(prev_mode);
		if (prev_state)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}
private:
	GLboolean prev_state;
	int prev_mode;
};


using shader_lock = context_lock<GL_CURRENT_PROGRAM>;
using texture_lock = context_lock<GL_TEXTURE_BINDING_2D>;
using vao_lock = context_lock<GL_VERTEX_ARRAY_BINDING>;
using vbo_lock = context_lock<GL_ARRAY_BUFFER_BINDING>;
using ebo_lock = context_lock<GL_ELEMENT_ARRAY_BUFFER_BINDING>;
using fbo_lock = context_lock<GL_FRAMEBUFFER_BINDING>;
using rbo_lock = context_lock<GL_RENDERBUFFER_BINDING>;
using blend_lock = context_lock<GL_BLEND>;
using viewport_lock = context_lock<GL_VIEWPORT>;
using line_width_lock = context_lock<GL_LINE_WIDTH>;
using cull_face_lock = context_lock<GL_CULL_FACE>;

//inline constexpr int DRAW_LOCK = 0;
//template <>
//class context_lock<DRAW_LOCK>
//{
//public:
//	context_lock() = default;
//	~context_lock() = default;
//private:
//	blend_lock bcontext;
//	shader_lock scontext;
//	vbo_lock vbcontext;
//	ebo_lock elock;
//	vao_lock vcontext;
//	texture_lock tcontext;
//	fbo_lock fcontext;
//	viewport_lock vlock;
//	cull_face_lock clock;
//};
//
//using draw_lock = context_lock<DRAW_LOCK>;

template <GLenum target>
inline constexpr GLenum binding = 0;

template <>
inline constexpr GLenum binding<GL_VERTEX_ARRAY> = GL_VERTEX_ARRAY_BINDING;

template <>
inline constexpr GLenum binding<GL_ARRAY_BUFFER> = GL_ARRAY_BUFFER_BINDING;

template <>
inline constexpr GLenum binding<GL_ELEMENT_ARRAY_BUFFER> = GL_ELEMENT_ARRAY_BUFFER_BINDING;

template <>
inline constexpr GLenum binding<GL_FRAMEBUFFER> = GL_FRAMEBUFFER_BINDING;

template <>
inline constexpr GLenum binding<GL_RENDERBUFFER> = GL_RENDERBUFFER_BINDING;


DETAIL_END

SGL_END