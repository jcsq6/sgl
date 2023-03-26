#pragma once
#include "buffers.h"
#include "math/mat.h"
#include "math/numerics.h"
#include <vector>
#include <iterator>
#include <concepts>

SGL_BEG

// use this to create general shapes at the origin.
// shape_data is to be used with render_obj's
template <vec_len dim>
class shape_data
{
public:
	virtual const vec<float, dim> *points() const = 0;
	virtual std::size_t size() const = 0;

	inline vbo get_buffer(GLenum usage = GL_STATIC_DRAW) const
	{
		vbo res;
		res.generate();
		res.attach_data(size() * sizeof(vec<float, dim>), points(), usage);

		return res;
	}
};

DETAIL_BEG

template <vec_len dim>
class array_iterator
{
public:
   	using iterator_category = std::random_access_iterator_tag;
   	using value_type        = vec<float, dim>;
   	using difference_type   = std::ptrdiff_t;
   	using pointer           = value_type*;
   	using reference         = value_type&;

	constexpr array_iterator(const array_iterator&) = default;
	constexpr array_iterator &operator=(const array_iterator&) = default;

	constexpr array_iterator() : val{} {}
	constexpr array_iterator(pointer p, difference_type off = 0) : val{p + off} {}

	constexpr reference operator*() const { return *val; }
	constexpr pointer operator->() const { return val; }

	constexpr array_iterator &operator++() { ++val; return *this; }
	constexpr array_iterator operator++(int) { array_iterator old{*this}; ++val; return old; }

	constexpr array_iterator &operator--() { --val; return *this; }
	constexpr array_iterator operator--(int) { array_iterator old{*this}; --val; return old; }

	constexpr array_iterator &operator+=(difference_type i) { val += i; return *this; }
	constexpr array_iterator &operator-=(difference_type i) { val -= i; return *this; }

	constexpr array_iterator operator+(difference_type i) { return array_iterator{*this} += i; }
	constexpr array_iterator operator-(difference_type i) { return array_iterator{*this} -= i; }

	constexpr reference operator[](difference_type i) { return val[i]; }
private:
	pointer val;
};

template <vec_len dim>
class array_const_iterator
{
public:
   	using iterator_category = std::random_access_iterator_tag;
   	using value_type        = vec<float, dim>;
   	using difference_type   = std::ptrdiff_t;
   	using pointer           = const value_type*;
   	using reference         = const value_type&;
	
	constexpr array_const_iterator(const array_const_iterator&) = default;
	constexpr array_const_iterator &operator=(const array_const_iterator&) = default;
	
	constexpr array_const_iterator() : val{} {}
	constexpr array_const_iterator(array_iterator<dim> it) : val{it.operator->()} {}
	constexpr array_const_iterator(pointer p, difference_type off = 0) : val{p + off} {}

	constexpr reference operator*() const { return *val; }
	constexpr pointer operator->() const { return val; }

	constexpr array_const_iterator &operator++() { ++val; return *this; }
	constexpr array_const_iterator operator++(int) { array_const_iterator old{*this}; ++val; return old; }

	constexpr array_const_iterator &operator--() { --val; return *this; }
	constexpr array_const_iterator operator--(int) { array_const_iterator old{*this}; --val; return old; }

	constexpr array_const_iterator &operator+=(difference_type i) { val += i; return *this; }
	constexpr array_const_iterator &operator-=(difference_type i) { val -= i; return *this; }

	constexpr array_const_iterator operator+(difference_type i) { return array_const_iterator{*this} += i; }
	constexpr array_const_iterator operator-(difference_type i) { return array_const_iterator{*this} -= i; }

	constexpr reference operator[](difference_type i) { return val[i]; }
private:
	pointer val;
};

template <vec_len dim>
constexpr bool operator==(array_const_iterator<dim> a, array_const_iterator<dim> b) { return a.operator->() == b.operator->(); }
template <vec_len dim>
constexpr bool operator!=(array_const_iterator<dim> a, array_const_iterator<dim> b) { return a.operator->() != b.operator->(); }

template <vec_len dim>
constexpr array_iterator<dim> operator+(typename array_iterator<dim>::difference_type i, array_iterator<dim> it) { return it += i; }

template <vec_len dim>
constexpr array_const_iterator<dim> operator+(typename array_iterator<dim>::difference_type i, array_const_iterator<dim> it) { return it += i; }

template <vec_len dim>
constexpr typename array_const_iterator<dim>::difference_type operator-(array_const_iterator<dim> a, array_const_iterator<dim> b) { return a.operator->() - b.operator->(); }

DETAIL_END

template <std::size_t n, vec_len dim>
class shape_data_array : public shape_data<dim>
{
public:
	using value_type = vec<float, dim>;
	using reference = value_type&;
	using const_reference = const value_type&;
	using difference_type = std::ptrdiff_t;
	using size_type = std::size_t;

	using iterator = detail::array_iterator<dim>;
	using const_iterator = detail::array_const_iterator<dim>;

	value_type m_data[n];

	constexpr shape_data_array(const shape_data_array&) = default;
	constexpr shape_data_array() : m_data{} {}
	constexpr shape_data_array(std::initializer_list<value_type> vals) : m_data{}
	{
		*this = vals;
	}
	
	constexpr shape_data_array& operator=(const shape_data_array&) = default;
	constexpr shape_data_array& operator=(std::initializer_list<value_type> vals)
	{
		const value_type* vali = vals.begin();
		for (size_type i = 0; i < n; ++i, ++vali)
			m_data[i] = *vali;
		return *this;
	}

	constexpr reference operator[](size_type i)
	{
		return m_data[i];
	}

	constexpr value_type operator[](size_type i) const
	{
		return m_data[i];
	}

	constexpr const value_type *data() const
	{
		return m_data;
	}

	constexpr value_type *data()
	{
		return m_data;
	}

	constexpr iterator begin()
	{
		return {m_data};
	}

	constexpr const_iterator begin() const
	{
		return {m_data};
	}

	constexpr iterator end()
	{
		return {m_data + n};
	}

	constexpr const_iterator end() const
	{
		return {m_data + n};
	}

	constexpr const value_type *points() const override
	{
		return m_data;
	}

	constexpr std::size_t size() const override
	{
		return n;
	}
};

template <vec_len dim>
class shape_data_vector : public shape_data<dim>, public std::vector<vec<float, dim>>
{
public:
	using std::vector<vec<float, dim>>::vector;

	inline const vec<float, dim> *points() const override
	{
		return std::vector<vec<float, dim>>::data();
	}

	inline std::size_t size() const override
	{
		return std::vector<vec<float, dim>>::size();
	}
};

namespace detail
{
	template <typename T>
	concept Contiguous_Container = requires(const T a)
	{
		typename T::value_type;
		{a.size()} -> std::convertible_to<std::size_t>;
		{a.data()} -> std::same_as<const typename T::value_type *>;
	};
}

template <vec_len dim>
class shape_data_view : public shape_data<dim>
{
public:
	template <typename It>
	shape_data_view(It beg, It end) : m_beg{&(*beg)}, m_size{end - beg}
	{
		using category = typename std::iterator_traits<It>::iterator_category;
		using type = typename std::iterator_traits<It>::value_type;
		static_assert(std::is_same_v<category, std::random_access_iterator_tag>, "Must be a contiguous range");
		static_assert(std::is_same_v<type, vec<float, dim>>, "Must be array of type vec2");
	}

	template <detail::Contiguous_Container C>
	shape_data_view(const C &container) : m_beg(container.data()), m_size{container.size()} {}

	template <std::size_t N>
	shape_data_view(const vec<float, dim> (&container)[N]) : m_beg(container), m_size{N} {}

	inline const vec<float, dim> *points() const override
	{
		return m_beg;
	}

	inline std::size_t size() const override
	{
		return m_size;
	}
private:
	const vec<float, dim> *m_beg;
	std::size_t m_size;
};

// returns a regular polygon with n points and radius 1 around the origin
template <vec_len n>
constexpr shape_data_array<n, 2> regular_polygon()
{
	float angle = 2 * pi<float>() / n;
	mat3 rot_mat = rot2d(angle);

	shape_data_array<n, 2> res;
	res[0] = {0, 1};
	
	// if even, rotate by half of the angle to make the bottom straight
	if (!(n & 1))
		res[0] = rot2d(angle / 2) * vec3(res[0]);
	
	for (vec_len i = 1; i < n; ++i)
		res[i] = rot_mat * vec3(res[i - 1]);

	return res;
}

// returns a regular polygon with n points and radius 1 around the origin
inline shape_data_vector<2> regular_polygon(vec_len n)
{
	float angle = 2 * pi<float>() / n;
	mat3 rot_mat = rot2d(angle);

	shape_data_vector<2> res(n);
	res[0] = {0, 1};
	
	// if even, rotate by half of the angle to make the bottom straight
	if (!(n & 1))
		res[0] = rot2d(angle / 2) * vec3(res[0]);
	
	for (vec_len i = 1; i < n; ++i)
		res[i] = rot_mat * vec3(res[i - 1]);

	return res;
}

SGL_END