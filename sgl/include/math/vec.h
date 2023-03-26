#pragma once
#include "macro.h"
#include <cmath>

SGL_BEG

using vec_len = unsigned int;

template <typename T, vec_len n>
struct vec;

template <typename T>
struct is_vec
{
	static constexpr bool value = false;
};

template <typename T, vec_len length>
struct is_vec<vec<T, length>>
{
	static constexpr bool value = true;
};

template <typename T, vec_len w, vec_len h>
struct mat;

template <typename T>
struct is_mat
{
	static constexpr bool value = false;
};

template <typename T, vec_len w, vec_len h>
struct is_mat<mat<T, w, h>>
{
	static constexpr bool value = true;
};

template <typename T>
struct vec<T, 1>
{
	static constexpr vec_len size() { return 1; }

	using value_type = T;

	T x;

	constexpr vec(const vec &other) = default;
	constexpr vec &operator=(const vec &other) = default;

	constexpr vec() : x{} {}

	template <typename ValT>
	constexpr vec(ValT _x) : x{static_cast<ValT>(_x)} {}

	// template <typename OT, vec_len Osz, typename = std::enable_if_t<Osz != 1>>
	// constexpr operator vec<OT, Osz>() const { return vec<OT, Osz>{static_cast<OT>(x)}; }

	template <typename OT, vec_len Osz>
	constexpr vec(vec<OT, Osz> other) : vec(other.x) {}

	template <typename OT, vec_len Osz>
	constexpr vec &operator=(vec<OT, Osz> other)
	{
		x = static_cast<T>(other.x);
		return *this;
	}

	inline T *data() { return reinterpret_cast<T*>(this); }
	inline T const *data() const { return reinterpret_cast<const T*>(this); }

	inline T &operator[](vec_len i) { return data()[i]; }
	inline T operator[](vec_len i) const { return data()[i]; }
	
	template <typename ValT>
	constexpr vec &operator+=(vec<ValT, 1> o)
	{
		x += o.x;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator-=(vec<ValT, 1> o)
	{
		x -= o.x;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator/=(vec<ValT, 1> o)
	{
		x /= o.x;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator*=(vec<ValT, 1> o)
	{
		x *= o.x;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator/=(ValT o)
	{
		x /= o;
		return *this;
	}
	template <typename ValT>
	constexpr vec &operator*=(ValT o)
	{
		x *= o;
		return *this;
	}

	// for constant expressions
	constexpr value_type at(vec_len i)
	{
		switch (i)
		{
		case 0:
			return x;
		}
		return 0;
	}
};

template <typename T>
struct vec<T, 2>
{
	static constexpr vec_len size() { return 2; }

	using value_type = T;

	T x, y;

	constexpr vec(const vec &other) = default;
	constexpr vec &operator=(const vec &other) = default;

	template <typename... ValT, typename = std::enable_if_t<sizeof...(ValT) <= 2>>
	constexpr vec(ValT... vals) : vec(static_cast<T>(vals)...) {}

	constexpr vec() : x{}, y{} {}

	constexpr vec(T _x) : x{_x}, y{} {}
	constexpr vec(T _x, T _y) : x{_x}, y{_y} {}

	template <typename OT, typename... ValT>
	constexpr vec(vec<OT, 1> other, ValT... vals) : vec(other.x, vals...) {}

	template <typename OT, vec_len Osz, typename = std::enable_if_t<Osz >= 2>>
	constexpr vec(vec<OT, Osz> other) : vec(other.x, other.y) {}

	// template <typename OT, vec_len Osz, typename = std::enable_if_t<Osz != 2>>
	// constexpr operator vec<OT, Osz>() const { return vec<OT, Osz>{static_cast<OT>(x), static_cast<OT>(y)}; }

	template <typename OT, vec_len Osz>
	constexpr vec &operator=(vec<OT, Osz> other)
	{
		x = static_cast<T>(other.x);
		if constexpr (other.size() >= 2)
			y = static_cast<T>(other.y);
		else
			y = {};
		return *this;
	}

	inline T *data() { return reinterpret_cast<T*>(this); }
	inline T const *data() const { return reinterpret_cast<const T*>(this); }

	inline T &operator[](vec_len i) { return data()[i]; }
	inline T operator[](vec_len i) const { return data()[i]; }
	
	template <typename ValT>
	constexpr vec &operator+=(vec<ValT, 2> o)
	{
		x += o.x;
		y += o.y;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator-=(vec<ValT, 2> o)
	{
		x -= o.x;
		y -= o.y;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator/=(vec<ValT, 2> o)
	{
		x /= o.x;
		y /= o.y;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator*=(vec<ValT, 2> o)
	{
		x *= o.x;
		y *= o.y;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator/=(ValT o)
	{
		x /= o;
		y /= o;
		return *this;
	}
	template <typename ValT>
	constexpr vec &operator*=(ValT o)
	{
		x *= o;
		y *= o;
		return *this;
	}

	// for constant expressions
	constexpr value_type at(vec_len i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		}
		return 0;
	}
};

template <typename T>
struct vec<T, 3>
{
	static constexpr vec_len size() { return 3; }

	using value_type = T;

	T x, y, z;

	constexpr vec(const vec &other) = default;
	constexpr vec &operator=(const vec &other) = default;

	constexpr vec() : x{}, y{}, z{} {}

	template <typename... ValT, typename = std::enable_if_t<sizeof...(ValT) <= 3>>
	constexpr vec(ValT... vals) : vec(static_cast<T>(vals)...) {}

	constexpr vec(T _x) : x{_x}, y{}, z{} {}
	constexpr vec(T _x, T _y) : x{_x}, y{_y}, z{} {}
	constexpr vec(T _x, T _y, T _z) : x{_x}, y{_y}, z{_z} {}

	template <typename OT, typename... ValT>
	constexpr vec(vec<OT, 1> other, ValT... vals) : vec(other.x, vals...) {}

	template <typename OT, typename... ValT>
	constexpr vec(vec<OT, 2> other, ValT... vals) : vec(other.x, other.y, vals...) {}

	template <typename OT, vec_len Osz, typename = std::enable_if_t<Osz >= 3>>
	constexpr vec(vec<OT, Osz> other) : vec(other.x, other.y, other.z) {}

	// template <typename OT, vec_len Osz, typename = std::enable_if_t<Osz != 3>>
	// constexpr operator vec<OT, Osz>() const { return vec<OT, Osz>{static_cast<OT>(x), static_cast<OT>(y), static_cast<OT>(z)}; }

	template <typename OT, vec_len Osz>
	constexpr vec &operator=(vec<OT, Osz> other)
	{
		x = static_cast<T>(other.x);
		if constexpr (other.size() >= 2)
		{
			y = static_cast<T>(other.y);
			if constexpr (other.size() >= 3)
				z = static_cast<T>(other.z);
			else
				z = {};
		}
		else
			y = z = {};
		return *this;
	}

	inline T *data() { return reinterpret_cast<T*>(this); }
	inline T const *data() const { return reinterpret_cast<const T*>(this); }

	inline T &operator[](vec_len i) { return data()[i]; }
	inline T operator[](vec_len i) const { return data()[i]; }
	
	template <typename ValT>
	constexpr vec &operator+=(vec<ValT, 3> o)
	{
		x += o.x;
		y += o.y;
		z += o.z;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator-=(vec<ValT, 3> o)
	{
		x -= o.x;
		y -= o.y;
		z -= o.z;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator/=(vec<ValT, 3> o)
	{
		x /= o.x;
		y /= o.y;
		z /= o.z;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator*=(vec<ValT, 3> o)
	{
		x *= o.x;
		y *= o.y;
		z *= o.z;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator/=(ValT o)
	{
		x /= o;
		y /= o;
		z /= o;
		return *this;
	}
	template <typename ValT>
	constexpr vec &operator*=(ValT o)
	{
		x *= o;
		y *= o;
		z *= o;
		return *this;
	}

	// for constant expressions
	constexpr value_type at(vec_len i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return 0;
	}
};

template <typename T>
struct vec<T, 4>
{
	static constexpr vec_len size() { return 4; }

	using value_type = T;

	T x, y, z, w;

	constexpr vec(const vec &other) = default;
	constexpr vec &operator=(const vec &other) = default;

	constexpr vec() : x{}, y{}, z{}, w{} {}

	template <typename... ValT, typename = std::enable_if_t<sizeof...(ValT) <= 4>>
	constexpr vec(ValT... vals) : vec(static_cast<T>(vals)...) {}

	constexpr vec(T _x) : x{_x}, y{}, z{} {}
	constexpr vec(T _x, T _y) : x{_x}, y{_y}, z{} {}
	constexpr vec(T _x, T _y, T _z) : x{_x}, y{_y}, z{_z} {}
	constexpr vec(T _x, T _y, T _z, T _w) : x{_x}, y{_y}, z{_z}, w{_w} {}

	template <typename OT, typename... ValT>
	constexpr vec(vec<OT, 1> other, ValT... vals) : vec(other.x, vals...) {}

	template <typename OT, typename... ValT>
	constexpr vec(vec<OT, 2> other, ValT... vals) : vec(other.x, other.y, vals...) {}

	template <typename OT, typename... ValT>
	constexpr vec(vec<OT, 3> other, ValT... vals) : vec(other.x, other.y, other.z, vals...) {}

	template <typename OT>
	constexpr vec(vec<OT, 4> other) : vec(other.x, other.y, other.z, other.w) {}

	// template <typename OT, vec_len Osz, typename = std::enable_if_t<Osz != 4>>
	// constexpr operator vec<OT, Osz>() const { return vec<OT, Osz>{static_cast<OT>(x), static_cast<OT>(y), static_cast<OT>(z), static_cast<OT>(w)}; }

	template <typename OT, vec_len Osz>
	constexpr vec &operator=(vec<OT, Osz> other)
	{
		x = static_cast<T>(other.x);
		if constexpr (other.size() >= 2)
		{
			y = static_cast<T>(other.y);
			if constexpr (other.size() >= 3)
			{
				z = static_cast<T>(other.z);
				if constexpr (other.size() >= 4)
					w = static_cast<T>(other.w);
				else
					w = {};
			}
			else
				z = w = {};
		}
		else
			y = z = w = {};
		return *this;
	}

	inline T *data() { return reinterpret_cast<T*>(this); }
	inline T const *data() const { return reinterpret_cast<const T*>(this); }

	inline T &operator[](vec_len i) { return data()[i]; }
	inline T operator[](vec_len i) const { return data()[i]; }
	
	template <typename ValT>
	constexpr vec &operator+=(vec<ValT, 4> o)
	{
		x += o.x;
		y += o.y;
		z += o.z;
		w += o.w;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator-=(vec<ValT, 4> o)
	{
		x -= o.x;
		y -= o.y;
		z -= o.z;
		w -= o.w;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator/=(vec<ValT, 4> o)
	{
		x /= o.x;
		y /= o.y;
		z /= o.z;
		w /= o.w;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator*=(vec<ValT, 4> o)
	{
		x *= o.x;
		y *= o.y;
		z *= o.z;
		w *= o.w;
		return *this;
	}

	template <typename ValT>
	constexpr vec &operator/=(ValT o)
	{
		x /= o;
		y /= o;
		z /= o;
		w /= o;
		return *this;
	}
	template <typename ValT>
	constexpr vec &operator*=(ValT o)
	{
		x *= o;
		y *= o;
		z *= o;
		w *= o;
		return *this;
	}

	// for constant expressions
	constexpr value_type at(vec_len i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		}
		return 0;
	}
};

template <typename AT, typename BT, vec_len sz>
constexpr auto operator+(vec<AT, sz> a, vec<BT, sz> b)
{
	return vec<decltype(AT{} + BT{}), sz>(a) += b;
}

template <typename AT, typename BT, vec_len sz>
constexpr auto operator-(vec<AT, sz> a, vec<BT, sz> b)
{
	return vec<decltype(AT{} - BT{}), sz>(a) -= b;
}

template <typename AT, typename BT, vec_len sz>
constexpr auto operator*(vec<AT, sz> a, vec<BT, sz> b)
{
	return vec<decltype(AT{} * BT{}), sz>(a) *= b;
}

template <typename AT, typename BT, vec_len sz>
constexpr auto operator/(vec<AT, sz> a, vec<BT, sz> b)
{
	return vec<decltype(AT{} / BT{}), sz>(a) /= b;
}

template <typename AT, vec_len sz, typename BT>
constexpr auto operator*(vec<AT, sz> a, BT b)
{
	return vec<decltype(AT{} * BT{}), sz>(a) *= b;
}

template <typename AT, typename BT, vec_len sz, typename = std::enable_if_t<!is_mat<AT>::value && !is_vec<AT>::value>>
constexpr auto operator*(AT a, vec<BT, sz> b)
{
	return b * a;
}

template <typename AT, vec_len sz, typename BT>
constexpr auto operator/(vec<AT, sz> a, BT b)
{
	return vec<decltype(AT{} / BT{}), sz>(a) /= b;
}

template <typename T, vec_len sz>
constexpr vec<T, sz> operator-(vec<T, sz> a)
{
	return a *= -1.f;
}

template <typename AT, typename BT, vec_len sz>
inline bool operator==(vec<AT, sz> a, vec<BT, sz> b)
{
	for (vec_len i = 0; i < sz; ++i)
		if (a[i] != b[i])
			return false;
	return true;
}

template <typename AT, typename BT, vec_len sz>
inline bool operator!=(vec<AT, sz> a, vec<BT, sz> b)
{
	return !(a == b);
}

template <vec_len sz>
inline vec<bool, sz> operator!(vec<bool, sz> a)
{
	for (vec_len i = 0; i < sz; ++i)
		a[i] = !a[i];
	return a;
}

template <typename AT, typename BT>
constexpr decltype(AT{} * BT{}) dot(vec<AT, 1> a, vec<BT, 1> b)
{
	return a.x * b.x;
}

template <typename AT, typename BT>
constexpr decltype(AT{} * BT{}) dot(vec<AT, 2> a, vec<BT, 2> b)
{
	return a.x * b.x + a.y * b.y;
}

template <typename AT, typename BT>
constexpr decltype(AT{} * BT{}) dot(vec<AT, 3> a, vec<BT, 3> b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename AT, typename BT>
constexpr decltype(AT{} * BT{}) dot(vec<AT, 4> a, vec<BT, 4> b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template <typename AT, typename BT>
constexpr vec<decltype(AT{} * BT{}), 3> cross(vec<AT, 3> a, vec<BT, 3> b)
{
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x};
}

template <typename T, vec_len sz>
inline T const *value(const vec<T, sz>& a)
{
	return a.data();
}

template <typename T, vec_len sz>
inline T *value(vec<T, sz> &a)
{
	return a.data();
}

template <typename T, vec_len sz>
constexpr T magnitude2(vec<T, sz> v)
{
	return dot(v, v);
}

template <typename T, vec_len sz>
constexpr T magnitude(vec<T, sz> v)
{
	return std::sqrt(magnitude2(v));
}

template <typename T, vec_len sz>
constexpr vec<T, sz> normalize(vec<T, sz> v)
{
	return v / magnitude(v);
}

template <typename T>
constexpr vec<T, 2> perpindicular(vec<T, 2> v)
{
	return {-v.y, v.x};
}

template <typename T>
constexpr T det(vec<T, 2> a, vec<T, 2> b)
{
	return a.x * b.y - b.x * a.y;
}
template <typename T>
constexpr T cross(vec<T, 2> a, vec<T, 2> b)
{
	return a.x * b.y - a.y * b.x;
}

template <typename T>
constexpr T perp_dot(vec<T, 2> a, vec<T, 2> b)
{
	return cross(a, b);
}

template <typename T, vec_len sz>
constexpr T distance(vec<T, sz> a, vec<T, sz> b)
{
	return magnitude(a - b);
}
template <typename T, vec_len sz>
constexpr T distance2(vec<T, sz> a, vec<T, sz> b)
{
	return magnitude2(a - b);
}
template <typename T, vec_len sz>
constexpr bool is_in_radius(vec<T, sz> a, vec<T, sz> b, T r)
{
	return distance2(a, b) <= r * r;
}
template <typename T, vec_len sz>
constexpr T angle(vec<T, sz> a, vec<T, sz> b)
{
	return std::acos(dot(a, b) / (magnitude(a) * magnitude(b)));
}

template <typename T>
constexpr T angle(vec<T, 2> a)
{
	return atan2(a.y, a.x);
}

using vec1 = vec<float, 1>;
using vec2 = vec<float, 2>;
using vec3 = vec<float, 3>;
using vec4 = vec<float, 4>;

using ivec1 = vec<int, 1>;
using ivec2 = vec<int, 2>;
using ivec3 = vec<int, 3>;
using ivec4 = vec<int, 4>;

using bvec1 = vec<bool, 1>;
using bvec2 = vec<bool, 2>;
using bvec3 = vec<bool, 3>;
using bvec4 = vec<bool, 4>;

using dvec1 = vec<double, 1>;
using dvec2 = vec<double, 2>;
using dvec3 = vec<double, 3>;
using dvec4 = vec<double, 4>;

SGL_END