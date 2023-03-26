#pragma once
#include "render_obj.h"
#include "math/vec.h"
#include "math/bound.h"
#include "shape_data.h"

SGL_BEG

template <bool rotatable = false>
class cube_obj : public virtual transformable_obj<true, true, rotatable>
{
public:
	cube_obj();
	cube_obj(vec3 min, vec3 size);

	void draw(render_target& target) const override;
};

template <bool rotatable = false>
class rectangle_obj : public virtual transformable_obj<true, false, rotatable>
{
public:
	rectangle_obj();

	/// @brief construct rectangle object
	/// @param min minimum of rect in 3d space
	/// @param size 2d size of rectangle (size.x/y do not neccessarily correspond to actual x/y axis)
	/// @param right direction corresponding to x axis in parameter size
	/// @param up direction corresponding to y axis in parameter size
	rectangle_obj(vec3 min, vec2 size, vec3 right = {1, 0, 0}, vec3 up = {0, 1, 0});

	inline void set_up(vec3 up_dir) { m_up = normalize(up_dir); }
	inline vec3 get_up() const { return m_up; }

	inline void set_right(vec3 right_dir) { m_right = normalize(right_dir); }
	inline vec3 get_right() const { return m_right; }

	inline void set_size(vec2 size) { m_sz = size; }
	inline vec2 get_size() const { return m_sz; }

	void draw(render_target& target) const override;
protected:
	vec3 m_right;
	vec3 m_up;
	vec2 m_sz;

	void setup_buffer() const;
};

template <vec_len dim>
class point_obj : public virtual base_transformable_obj
{
public:
	static_assert(dim == 2 || dim == 3, "Dimension must be two or three dimensional");

	point_obj();
	point_obj(vec<float, dim> center, float size);

	inline void set_size(float size) { m_size = size; data_changed(); }
	inline float get_size() const { return m_size; }

	inline void set_center(vec<float, dim> center) { m_center = center; data_changed(); }
	inline vec<float, dim> get_center() const { return m_center; }

	void draw(render_target& target) const override;
protected:
	vec<float, dim> m_center;
	float m_size;

	void apply_transform() const override;
};

template <bool scalable = false, bool rotatable = false>
class line_obj : public virtual transformable_obj<true, scalable, rotatable>
{
public:
	line_obj();
	line_obj(vec3 beg, vec3 end, float line_width = .05);

	inline void set_endpoint(vec3 end) { m_end = end; }

	inline vec3 get_endpoint() const { return m_end; }

	inline void set_line_width(float line_width) { m_width = line_width; }

	inline float get_line_width() const { return m_width; }

	void draw(render_target& target) const override;

protected:
	vec3 m_end;
	float m_width;

	void setup_buffer() const;
};

// class polygon_obj : public movable_obj, public rotatable_obj, public colorable_obj
// {
// public:
// 	template <vec_len dim>
// 	polygon_obj(vec3 loc, vec3 scale, const shape_data<dim> &vertices) : movable_obj(loc), rotatable_obj(), colorable_obj(),
// 																		 m_buffer{vertices.get_buffer()}, m_vao{get_vao(m_buffer, dim)},
// 																		 m_scale{scale}, m_count(vertices.size())
// 	{
// 	}

// 	template <vec_len dim>
// 	inline void set_shape(const shape_data<dim> &vertices)
// 	{
// 		m_buffer = vertices.get_buffer();
// 		m_vao = get_vao(m_buffer, dim);
// 		m_count = vertices.size();
// 	}

// 	inline virtual void set_scale(vec3 scale)
// 	{
// 		m_scale = scale;
// 	}

// 	inline vec3 get_scale() const
// 	{
// 		return m_scale;
// 	}

// 	int count() const
// 	{
// 		return m_count;
// 	}

// 	void draw(render_target &target) const override;

// private:
// 	vbo m_buffer;
// 	vao m_vao;

// 	vec3 m_scale;

// 	int m_count;

// 	static vao get_vao(const vbo &buffer, vec_len dim);
// };

SGL_END