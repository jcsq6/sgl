#pragma once
#include "vec.h"
#include <iterator>

SGL_BEG
template <typename T, vec_len w, vec_len h>
struct mat;

template <typename T, typename T1, vec_len h1, vec_len w1, vec_len w2>
constexpr auto operator*(const mat<T, w1, h1> &a, const mat<T1, w2, w1> &b);

template <typename T, vec_len w, vec_len h>
struct mat
{
	vec<T, h> data[w];

	constexpr auto &operator[](vec_len i)
	{
		return data[i];
	}

	constexpr const auto &operator[](vec_len i) const
	{
		return data[i];
	}

	template <typename OT>
	constexpr mat &operator=(const mat<OT, w, h> &o)
	{
		for (vec_len col = 0; col < width(); ++col)
			data[col] = o[col];
		return *this;
	}

	template <typename OT, vec_len ow, vec_len oh>
	constexpr mat &operator*=(const mat<OT, ow, oh> &o)
	{
		return *this = *this * o;
	}

	template <typename OT>
	constexpr mat &operator*=(const mat<OT, w, w> &o)
	{
		for (vec_len row = 0; row < height(); ++row)
		{
			// to be non-destructive
			vec<T, width()> new_row;
			for (vec_len o_col = 0; o_col < width(); ++o_col)
			{
				double d = 0;
				for (vec_len i = 0; i < width(); ++i)
					d += data[i][row] * o[o_col][i];
				new_row[o_col] = static_cast<T>(d);
			}
			// assign row
			for (vec_len i = 0; i < width(); ++i)
				data[i][row] = new_row[i];
		}
		return *this;
	}

	template <typename OT>
	constexpr mat &operator*=(OT o)
	{
		for (vec_len col = 0; col < width(); ++col)
				data[col] *= o;
	}

	template <typename OT>
	constexpr mat &operator/=(OT o)
	{
		for (vec_len col = 0; col < width(); ++col)
				data[col] /= o;
	}

	constexpr vec<T, w> get_row(vec_len row_num) const
	{
		vec<T, w> res;
		for (vec_len i = 0; i < width(); ++i)
			res[i] = data[i][row_num];
		return res;
	}

	constexpr static vec_len size()
	{
		return w;
	}

	constexpr static vec_len width()
	{
		return w;
	}

	constexpr static vec_len height()
	{
		return h;
	}
};

using mat2 = mat<float, 2, 2>;
using mat3 = mat<float, 3, 3>;
using mat4 = mat<float, 4, 4>;

using dmat2 = mat<double, 2, 2>;
using dmat3 = mat<double, 3, 3>;
using dmat4 = mat<double, 4, 4>;

// w1xh1 * w2xw1 --> w2xh1
template <typename T, typename T1, vec_len h1, vec_len w1, vec_len w2>
constexpr auto operator*(const mat<T, w1, h1> &a, const mat<T1, w2, w1> &b)
{
	mat<decltype(float{} * T{} * T1{}), w2, h1> res;

	for (vec_len a_row = 0; a_row < a.height(); ++a_row)
	{
		for (vec_len b_col = 0; b_col < b.width(); ++b_col)
		{
			double d = 0;
			for (vec_len i = 0; i < a.width(); ++i)
				d += a[i][a_row] * b[b_col][i];
			res[b_col][a_row] = static_cast<T>(d);
		}
	}

	return res;
}

template <typename S, typename T, vec_len w, vec_len h, typename = std::enable_if_t<!is_vec<S>::value>>
constexpr auto operator*(S scale, const mat<T, w, h> &a)
{
	mat<decltype(float{} * S{} * T{}), w, h> res;
	for (vec_len col{}; col < a.width(); ++col)
			res[col] = a[col] * scale;
	return res;
}

template <typename S, typename T, vec_len w, vec_len h, typename = std::enable_if_t<!is_vec<S>::value>>
constexpr auto operator*(const mat<T, w, h> &a, S scale)
{
	return scale * a;
}

template <typename M, typename V>
constexpr auto operator*(const mat<M, 1, 1> &a, vec<V, 1> b)
{
	vec<decltype(M{} * V{}), 1> res = a[0].x * b.x;
	return res;
}

template <typename M, typename V>
constexpr auto operator*(const mat<M, 2, 2> &a, vec<V, 2> b)
{
	vec<decltype(M{} * V{}), 2> res{a[0].x * b.x + a[1].x * b.y, a[0].y * b.x + a[1].y * b.y};
	return res;
}

template <typename M, typename V>
constexpr auto operator*(const mat<M, 3, 3> &a, vec<V, 3> b)
{
	vec<decltype(M{} * V{}), 3> res{
		a[0].x * b.x + a[1].x * b.y + a[2].x * b.z,
		a[0].y * b.x + a[1].y * b.y + a[2].y * b.z,
		a[0].z * b.x + a[1].z * b.y + a[2].z * b.z,
		};
	return res;
}

template <typename M, typename V>
constexpr auto operator*(const mat<M, 4, 4> &a, vec<V, 4> b)
{
	vec<decltype(M{} * V{}), 4> res{
		a[0].x * b.x + a[1].x * b.y + a[2].x * b.z + a[3].x * b.w,
		a[0].y * b.x + a[1].y * b.y + a[2].y * b.z + a[3].y * b.w,
		a[0].z * b.x + a[1].z * b.y + a[2].z * b.z + a[3].z * b.w,
		a[0].w * b.x + a[1].w * b.y + a[2].w * b.z + a[3].w * b.w,
		};
	return res;
}

template <typename T>
using to_float = decltype(T{} * float{});

template <typename T>
constexpr mat<to_float<T>, 4, 4> perspective(T fovy, T aspect, T _near, T _far)
{
	using tp = to_float<T>;
	tp n = _near;
	tp f = _far;
	tp t = std::tan(fovy / 2.f) * n;
	tp b = -t;
	tp r = t * aspect;
	tp l = b * aspect;

	mat<tp, 4, 4> res {
		vec<tp, 4>{2 * n / (r - l), 0, 0, 0},
		vec<tp, 4>{0, 2 * n / (t - b), 0, 0},
		vec<tp, 4>{(r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n), -1},
		vec<tp, 4>{0, 0, -2 * (f * n) / (f - n), 0},
	};

	return res;
}

template <typename T>
constexpr mat<to_float<T>, 4, 4> orient(vec<T, 3> right, vec<T, 3> up, vec<T, 3> dir)
{
	using t = to_float<T>;
	mat<t, 4, 4> res {
		vec<t, 4>{right.x, up.x, dir.x, 0},
		vec<t, 4>{right.y, up.y, dir.y, 0},
		vec<t, 4>{right.z, up.z, dir.z, 0},
		vec<t, 4>{0, 0, 0, 1},
	};

	return res;
}

template <typename T>
constexpr mat<to_float<T>, 4, 4> ortho_mat(T left, T right, T bottom, T top, T _near, T _far)
{
	using t = to_float<T>;
	mat<t, 4, 4> res {
		vec<t, 4>{2.f / (right - left), 0.f, 0.f, 0.f},
		vec<t, 4>{0.f, 2.f / (top - bottom), 0.f, 0.f},
		vec<t, 4>{0.f, 0.f, -2.f / (_far - _near), 0.f},
		vec<t, 4>{-1.f * (right + left) / (right - left), -1.f * (top + bottom) / (top - bottom), -1.f * (_far + _near) / (_far - _near), 1.f},
	};

	return res;
}

template <typename T>
constexpr mat<to_float<T>, 4, 4> x_rot(T rads)
{
	using t = to_float<T>;
	mat<t, 4, 4> res {
		vec<t, 4>{1.f, 0.f, 0.f, 0.f},
		vec<t, 4>{0.f, std::cos(rads), std::sin(rads), 0.f},
		vec<t, 4>{0.f, -std::sin(rads), std::cos(rads), 0.f},
		vec<t, 4>{0.f, 0.f, 0.f, 1.f},
	};

	return res;
}

template <typename T>
constexpr mat<to_float<T>, 4, 4> y_rot(T rads)
{
	using t = to_float<T>;
	auto c = std::cos(rads);
	auto s = std::sin(rads);

	mat<t, 4, 4> res {
		vec<t, 4>{c, 0.f, -s, 0.f},
		vec<t, 4>{0.f, 1.f, 0.f, 0.f},
		vec<t, 4>{s, 0.f, c, 0.f},
		vec<t, 4>{0.f, 0.f, 0.f, 1.f},
	};

	return res;
}

template <typename T>
constexpr mat<to_float<T>, 4, 4> z_rot(T rads)
{
	using t = to_float<T>;
	auto c = std::cos(rads);
	auto s = std::sin(rads);

	mat<t, 4, 4> res {
		vec<t, 4>{c, s, 0.f, 0.f},
		vec<t, 4>{-s, c, 0.f, 0.f},
		vec<t, 4>{0.f, 0.f, 1.f, 0.f},
		vec<t, 4>{0.f, 0.f, 0.f, 1.f},
	};

	return res;
}

template <typename T = float>
constexpr mat<to_float<T>, 4, 4> rot(T rads, vec<T, 3> axis)
{
	using t = to_float<T>;

	auto u = normalize(axis);

	auto c = std::cos(rads);
	auto s = std::sin(rads);
	mat<t, 4, 4> res {
		vec<t, 4>{c + u.x * u.x * (1 - c), u.y * u.x * (1 - c) + u.z * s, u.z * u.x * (1 - c) - u.y * s, 0.f},
		vec<t, 4>{u.x * u.y * (1 - c) - u.z * s, c + u.y * u.y * (1 - c), u.z * u.y * (1 - c) + u.x * s, 0.f},
		vec<t, 4>{u.x * u.z * (1 - c) + u.y * s, u.y * u.z * (1 - c) - u.x * s, c + u.z * u.z * (1 - c), 0.f},
		vec<t, 4>{0.f, 0.f, 0.f, 1.f},
	};

	return res;
}

template <typename T = float>
constexpr mat<to_float<T>, 3, 3> rot2d(T rads)
{
	using t = to_float<T>;

	auto c = std::cos(rads);
	auto s = std::sin(rads);

	mat<t, 3, 3> res {
		vec<t, 3>{c, s, 0},
		vec<t, 3>{-s, c, 0},
		vec<t, 3>{0, 0, 1},
	};

	return res;
}

template <typename T = float>
constexpr mat<to_float<T>, 4, 4> translate(vec<T, 3> loc)
{
	using t = to_float<T>;

	mat<t, 4, 4> res {
		vec<t, 4>{1.f, 0.f, 0.f, 0.f},
		vec<t, 4>{0.f, 1.f, 0.f, 0.f},
		vec<t, 4>{0.f, 0.f, 1.f, 0.f},
		vec<t, 4>{loc.x, loc.y, loc.z, 1.f},
	};

	return res;
}

template <typename T = float>
constexpr mat<to_float<T>, 3, 3> translate2d(vec<T, 2> loc)
{
	using t = to_float<T>;

	mat<t, 3, 3> res {
		vec<t, 3>{1.f, 0.f, 0.f},
		vec<t, 3>{0.f, 1.f, 0.f},
		vec<t, 3>{loc.x, loc.y, 1.f},
	};

	return res;
}

template <typename T = float>
constexpr mat<to_float<T>, 4, 4> scale(vec<T, 3> s)
{
	using t = to_float<T>;

	mat<t, 4, 4> res {
		vec<t, 4>{s.x, 0.f, 0.f, 0.f},
		vec<t, 4>{0.f, s.y, 0.f, 0.f},
		vec<t, 4>{0.f, 0.f, s.z, 0.f},
		vec<t, 4>{0.f, 0.f, 0.f, 1.f},
	};

	return res;
}

template <typename T = float>
constexpr mat<to_float<T>, 3, 3> scale2d(vec<T, 2> s)
{
	using t = to_float<T>;
	mat<t, 4, 4> res {
		vec<t, 3>{s.x, 0.f, 0.f},
		vec<t, 3>{0.f, s.y, 0.f},
		vec<t, 3>{0.f, 0.f, 1.f},
	};

	return res;
}

template <typename T = float, vec_len w = 4>
constexpr mat<T, w, w> identity()
{
	mat<T, w, w> res{};
	for (vec_len i = 0; i < w; ++i)
		res[i][i] = 1;
	return res;
}

template <typename T>
inline constexpr to_float<T> det(T a)
{
	return a;
}

template <typename T>
constexpr mat<to_float<T>, 1, 1> det(const mat<T, 1, 1> &c)
{
	return c;
}

template <typename T>
inline constexpr to_float<T> det(T a, T b, T c, T d)
{
	using fl = to_float<T>;
	return static_cast<fl>(a) * d - static_cast<fl>(b) * c;
}

template <typename T>
constexpr to_float<T> det(const mat<T, 2, 2> &c)
{
	return det(c[0][0], c[1][0],
			   c[0][1], c[1][1]);
}

template <typename T>
inline constexpr to_float<T> det(T a, T b, T c, T d, T e, T f, T g, T h, T i)
{
	return a * det(e, f, h, i) -
		   b * det(d, f, g, i) +
		   c * det(d, e, g, h);
}

template <typename T>
constexpr to_float<T> det(const mat<T, 3, 3> &c)
{
	return det(c[0][0], c[1][0], c[2][0],
			   c[0][1], c[1][1], c[2][1],
			   c[0][2], c[1][2], c[2][2]);
}

template <typename T>
inline constexpr to_float<T> det(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p)
{
	return a * det(f, g, h, j, k, l, n, o, p) -
		   b * det(e, g, h, i, k, l, m, o, p) +
		   c * det(e, f, h, i, j, l, m, n, p) -
		   d * det(e, f, g, i, j, k, m, n, o);
}

template <typename T>
constexpr to_float<T> det(const mat<T, 4, 4> &c)
{
	return det(c[0][0], c[1][0], c[2][0], c[3][0],
			   c[0][1], c[1][1], c[2][1], c[3][1],
			   c[0][2], c[1][2], c[2][2], c[3][2],
			   c[0][3], c[1][3], c[2][3], c[3][3]);
}

template <typename T, vec_len w, vec_len h>
constexpr mat<T, h, w> transpose(const mat<T, w, h> &m)
{
	mat<T, h, w> res;
	for (vec_len x = 0; x < w; ++x)
		for (vec_len y = 0; y < h; ++y)
			res[y][x] = m[x][y];
	return res;
}

template <typename T>
constexpr mat<T, 4, 4> transpose(const mat<T, 4, 4> &m)
{
	mat<T, 4, 4> res {{
		{m[0][0], m[1][0], m[2][0], m[3][0]},
		{m[0][1], m[1][1], m[2][1], m[3][1]},
		{m[0][2], m[1][2], m[2][2], m[3][2]},
		{m[0][3], m[1][3], m[2][3], m[3][3]},
	}};

	return res;
}

template <typename T>
constexpr mat<T, 3, 3> transpose(const mat<T, 3, 3> &m)
{
	mat<T, 3, 3> res {{
		{m[0][0], m[1][0], m[2][0]},
		{m[0][1], m[1][1], m[2][1]},
		{m[0][2], m[1][2], m[2][2]},
	}};

	return res;
}

template <typename T>
constexpr mat<T, 2, 2> transpose(const mat<T, 2, 2> &m)
{
	mat<T, 2, 2> res {{
		{m[0][0], m[1][0]},
		{m[0][1], m[1][1]},
	}};

	return res;
}

template <typename T>
constexpr mat<to_float<T>, 2, 2> cofactor(const mat<T, 2, 2> &m)
{
	mat<to_float<T>, 2, 2> res {{
		{det(m[1][1]), -det(m[1][0])},
		{-det(m[0][1]), det(m[0][0])},
	}};

	return res;
}

template <typename T>
constexpr mat<to_float<T>, 3, 3> cofactor(const mat<T, 3, 3> &m)
{
	mat<to_float<T>, 3, 3> res {{
		{det(m[1][1], m[2][1],
			 m[1][2], m[2][2]),
		 -det(m[1][0], m[2][0],
			  m[1][2], m[2][2]),
		 det(m[1][0], m[2][0],
			 m[1][1], m[2][1])},
		{-det(m[0][1], m[2][1],
			  m[0][2], m[2][2]),
		 det(m[0][0], m[2][0],
			 m[0][2], m[2][2]),
		 -det(m[0][0], m[2][0],
			  m[0][1], m[2][1])},
		{det(m[0][1], m[1][1],
			 m[0][2], m[1][2]),
		 -det(m[0][0], m[1][0],
			  m[0][2], m[1][2]),
		 det(m[0][0], m[1][0],
			 m[0][1], m[1][1])},
	}};

	return res;
}

template <typename T>
constexpr mat<to_float<T>, 4, 4> cofactor(const mat<T, 4, 4> &m)
{
	mat<to_float<T>, 4, 4> res {{
		{det(m[1][1], m[2][1], m[3][1],
			 m[1][2], m[2][2], m[3][2],
			 m[1][3], m[2][3], m[3][3]),
		 -det(m[1][0], m[2][0], m[3][0],
			  m[1][2], m[2][2], m[3][2],
			  m[1][3], m[2][3], m[3][3]),
		 det(m[1][0], m[2][0], m[3][0],
			 m[1][1], m[2][1], m[3][1],
			 m[1][3], m[2][3], m[3][3]),
		 -det(m[1][0], m[2][0], m[3][0],
			  m[1][1], m[2][1], m[3][1],
			  m[1][2], m[2][2], m[3][2])},
		{-det(m[0][1], m[2][1], m[3][1],
			  m[0][2], m[2][2], m[3][2],
			  m[0][3], m[2][3], m[3][3]),
		 det(m[0][0], m[2][0], m[3][0],
			 m[0][2], m[2][2], m[3][2],
			 m[0][3], m[2][3], m[3][3]),
		 -det(m[0][0], m[2][0], m[3][0],
			  m[0][1], m[2][1], m[3][1],
			  m[0][3], m[2][3], m[3][3]),
		 det(m[0][0], m[2][0], m[3][0],
			 m[0][1], m[2][1], m[3][1],
			 m[0][2], m[2][2], m[3][2])},
		{det(m[0][1], m[1][1], m[3][1],
			 m[0][2], m[1][2], m[3][2],
			 m[0][3], m[1][3], m[3][3]),
		 -det(m[0][0], m[1][0], m[3][0],
			  m[0][2], m[1][2], m[3][2],
			  m[0][3], m[1][3], m[3][3]),
		 det(m[0][0], m[1][0], m[3][0],
			 m[0][1], m[1][1], m[3][1],
			 m[0][3], m[1][3], m[3][3]),
		 -det(m[0][0], m[1][0], m[3][0],
			  m[0][1], m[1][1], m[3][1],
			  m[0][2], m[1][2], m[3][2])},
		{-det(m[0][1], m[1][1], m[2][1],
			  m[0][2], m[1][2], m[2][2],
			  m[0][3], m[1][3], m[2][3]),
		 det(m[0][0], m[1][0], m[2][0],
			 m[0][2], m[1][2], m[2][2],
			 m[0][3], m[1][3], m[2][3]),
		 -det(m[0][0], m[1][0], m[2][0],
			  m[0][1], m[1][1], m[2][1],
			  m[0][3], m[1][3], m[2][3]),
		 det(m[0][0], m[1][0], m[2][0],
			 m[0][1], m[1][1], m[2][1],
			 m[0][2], m[1][2], m[2][2])},
	}};

	return res;
}

template <typename T, vec_len w>
constexpr mat<to_float<T>, w, w> adjoint(const mat<T, w, w> &m)
{
	auto res = transpose(cofactor(m));
	return res;
}

template <typename T, vec_len w>
constexpr mat<to_float<T>, w, w> inverse(const mat<T, w, w> &m)
{
	auto res = (1 / det(m)) * adjoint(m);
	return res;
}

template <typename T, vec_len w, vec_len h>
T const *value(const mat<T, w, h> &m)
{
	return reinterpret_cast<T const *>(m.data);
}

template <typename T, vec_len w, vec_len h>
T *value(mat<T, w, h> &m)
{
	return reinterpret_cast<T *>(m.data);
}

template <typename InputIt, typename T, vec_len sz>
constexpr void translate(InputIt begin, InputIt end, vec<T, sz> offset)
{
	for (; begin != end; ++begin)
		*begin += offset;
}

// move in relation to origin
template <typename InputIt, typename T, vec_len sz>
constexpr void move_to(InputIt begin, InputIt end, vec<T, sz> loc, vec<T, sz> origin)
{
	auto off = loc - origin;
	translate(begin, end, off);
}

// rotate around origin
template <typename InputIt, typename T>
constexpr void rotate(InputIt begin, InputIt end, float rads, sgl::vec<T, 2> origin)
{
	auto mat = sgl::rot2d(rads);

	for (; begin != end; ++begin)
	{
		*begin -= origin;
		*begin = mat * *begin;
		*begin += origin;
	}
}

// rotate about axis around origin
template <typename InputIt, typename T>
constexpr void rot(InputIt begin, InputIt end, T rads, sgl::vec<T, 3> axis, sgl::vec<T, 3> origin)
{
	auto mat = sgl::rot(rads, axis);

	for (; begin != end; ++begin)
	{
		*begin -= origin;
		*begin = mat * vec<T, 4>(*begin, 1);
		*begin += origin;
	}
}

template <typename InputIt>
constexpr auto center(InputIt begin, InputIt end)
{
	using vec_t = typename std::iterator_traits<InputIt>::value_type;
    static_assert(sgl::is_vec<vec_t>::value, "InputIt value_type must be a vec");
	sgl::vec<to_float<typename vec_t::value_type>, vec_t::size()> res{};
	auto sz = std::distance(begin, end);
	for (; begin != end; ++begin)
		res += *begin;
	res /= sz;
	return res;
}

template <typename InputIt, typename T>
constexpr void transform_pts(InputIt begin, InputIt end, const mat<T, 4, 4> &mat)
{
	using vec_t = typename std::iterator_traits<InputIt>::value_type;
    static_assert(sgl::is_vec<vec_t>::value, "InputIt value_type must be a vec");
	
	for (; begin != end; ++begin)
	{
		if constexpr (vec_t::size() == 1)
			*begin = mat * vec4(*begin, 0, 0, 1);
		else if constexpr (vec_t::size() == 2)
			*begin = mat * vec4(*begin, 0, 1);
		else if constexpr (vec_t::size() == 3)
			*begin = mat * vec4(*begin, 1);
		else if constexpr (vec_t::size() == 4)
			*begin = mat * *begin;
	}
}

SGL_END