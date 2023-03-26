#pragma once
#include "vec.h"

SGL_BEG

struct bound
{
	vec3 min;
	vec3 dims;

	inline vec3 max() const
	{
		return min + dims;
	}
};

struct rect
{
	vec2 min;
	vec2 dims;

	inline vec2 max() const
	{
		return min + dims;
	}
};

SGL_END