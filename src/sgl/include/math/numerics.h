#pragma once
#include <limits>
#include <type_traits>

#include "macro.h"

SGL_BEG
template <typename T = double>
constexpr T pi()
{
	return static_cast<T>(3.14159265358979323846);
}

template <typename T = double>
constexpr T radians(T degrees)
{
	return degrees * pi<T>() / 180.f;
}

template <typename T = double>
constexpr T degrees(T radians)
{
	return radians * 180.f / pi<T>();
}

template <typename T>
constexpr int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

template <typename T>
constexpr T gcd(T a, T b)
{
	if (b == 0)
		return a;
	return gcd(b, a % b);
}

template <typename T>
constexpr T lcm(T a, T b)
{
	return (a * b) / gcd(a, b);
}

SGL_END