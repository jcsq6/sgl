#pragma once
#include "macro.h"

SGL_BEG
class gl_object
{
public:
	gl_object() = default;
	gl_object(const gl_object &) = delete;
	gl_object &operator=(const gl_object &) = delete;

	virtual void generate() = 0;
	virtual void use() const = 0;
	virtual void destroy() = 0;
	virtual unsigned int index() const = 0;
	virtual ~gl_object() = default;
};

SGL_END