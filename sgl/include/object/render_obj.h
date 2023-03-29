#pragma once
#include "macro.h"
#include "math/mat.h"
#include "buffers.h"

SGL_BEG

class render_target;
class render_obj;

class render_shader;
class lighting_engine;

struct render_settings
{
	inline render_settings() : color{ 0, 0, 0, 1 }, shader{}, engine{} {}
	inline render_settings(vec4 col) : color{ col }, shader{}, engine{} {}
	inline render_settings(render_shader *shader_program) : color{ 0, 0, 0, 1 }, shader{ shader_program }, engine{} {}
	inline render_settings(const lighting_engine *light_engine) : color{ 0, 0, 0, 1 }, shader{}, engine{ light_engine } {}
	inline render_settings(render_shader *shader_program, const lighting_engine *light_engine) : color{ 0, 0, 0, 1 }, shader{ shader_program }, engine{ light_engine } {}
	inline render_settings(vec4 col, render_shader *shader_program, const lighting_engine *light_engine) : color{ col }, shader{ shader_program }, engine{ light_engine } {}
	
	vec4 color;
	render_shader *shader;
	const lighting_engine *engine;
};

// render_types are generic types for rendering
// they don't represent instances
// ex: cube, rectangle, triangle
class render_type
{
public:
	render_type() = default;
	inline render_type(vao &&_vao) : m_vao{std::move(_vao)} {}
	virtual ~render_type() = default;

	// draws without shader setup
	virtual void draw(render_target &target) const = 0;

protected:
	vao m_vao;

	// call before drawing operations
	void bind_target(render_target &target) const;

	friend render_obj;
};

// render_objs are specific instances of render_types
class render_obj
{
public:
	virtual void draw(render_target &target, const render_settings &settings) const = 0;
	virtual void draw(render_target &target) const;

	virtual ~render_obj() = default;
protected:
	inline render_obj(const render_type &rtype) : type{&rtype} {}
	inline render_obj() : type{} {}

	// call before drawing operations
	void bind_target(render_target &target) const;

	inline const vao &get_vao() const { return type->m_vao; }

	const render_type *type;
};

//class render_settings_obj : public render_obj
//{
//public:
//	void draw(render_target &target) const override;
//	render_settings settings;
//	render_obj *obj;
//};

/// @brief transformable render_obj
/// @tparam movable set to true if the type should include translation transforms
/// @tparam scalable set to true if the type should include scaling transforms
/// @tparam rotatable set to true if the type should include rotation transforms
template <bool movable, bool scalable, bool rotatable>
class transformable_obj;

template<>
class transformable_obj<false, false, false> : public virtual render_obj
{
public:
	inline transformable_obj() : render_obj(), model{ identity() }, changed{ true } {}
	inline transformable_obj(const render_type &rtype) : render_obj(rtype), model{ identity() }, changed{ true } {}

	inline const mat4 &get_model() const { return model; }

	// call before draw operations
	inline void update_model() const
	{
		if (changed)
		{
			model = identity();
			apply_transform();
			changed = false;
		}
	}

	virtual ~transformable_obj() = default;

protected:
	// mutable is neccessary because it may need to be updated when draw is called
	mutable mat4 model;

	// call when model needs to be updated
	inline void data_changed() { changed = true; }
	inline bool has_changed() const { return changed;  }

	virtual void apply_transform() const = 0;

private:
	// mutable is neccessary because it may need to be updated when draw is called
	mutable bool changed;
};

/// @brief transformable render_obj
/// @tparam movable set to true if the type should include translation transforms
/// @tparam scalable set to true if the type should include scaling transforms
/// @tparam rotatable set to true if the type should include rotation transforms
template <bool movable, bool scalable, bool rotatable>
class transformable_obj;

template <>
class transformable_obj<true, false, false> : public virtual transformable_obj<false, false, false>
{
public:
	inline transformable_obj() = default;
	inline transformable_obj(vec3 loc) : m_loc{loc} {}

	inline void set_loc(vec3 loc) { m_loc = loc; data_changed(); }
	inline vec3 get_loc() const	{ return m_loc; }

	virtual ~transformable_obj() = default;

	static constexpr bool is_movable = true;
	static constexpr bool is_scalable = false;
	static constexpr bool is_rotatable = false;

protected:
	vec3 m_loc;

	virtual void apply_transform() const override;
};

template <>
class transformable_obj<false, true, false> : public virtual transformable_obj<false, false, false>
{
public:
	inline transformable_obj() : m_scale{1, 1, 1}, m_origin{} {}
	inline transformable_obj(vec3 scale) : m_scale{scale}, m_origin{} {}

	inline void set_scale(vec3 scale) { m_scale = scale; data_changed(); }
	inline vec3 get_scale() const	{ return m_scale; }

	// scale_origin is local to the object, not the world
	inline void set_scale_origin(vec3 origin) { m_origin = origin; data_changed(); }
	inline vec3 get_scale_origin() const { return m_origin; }

	virtual ~transformable_obj() = default;

	static constexpr bool is_movable = false;
	static constexpr bool is_scalable = true;
	static constexpr bool is_rotatable = false;

protected:
	vec3 m_scale;
	vec3 m_origin;

	virtual void apply_transform() const override;
};

template <>
class transformable_obj<false, false, true> : public virtual transformable_obj<false, false, false>
{
public:
	inline transformable_obj() : m_origin{}, m_axis{0, 0, 1}, m_angle{} {}
	inline transformable_obj(vec3 origin, vec3 axis, float angle) : m_origin{origin}, m_axis{normalize(axis)}, m_angle{angle} {}

	// rot_origin is local to the object, not the world
	inline void set_rot_origin(vec3 origin) { m_origin = origin; data_changed(); }
	inline vec3 get_rot_origin() const { return m_origin; }

	inline void set_rot_axis(vec3 axis) { m_axis = normalize(axis); data_changed(); }
	inline vec3 get_rot_axis() const { return m_axis; }

	inline void set_angle(float angle) { m_angle = angle; data_changed(); }
	inline float get_angle() const { return m_angle; }

	virtual ~transformable_obj() = default;

	static constexpr bool is_movable = false;
	static constexpr bool is_scalable = false;
	static constexpr bool is_rotatable = true;
protected:
	vec3 m_origin;
	vec3 m_axis;
	float m_angle;

	virtual void apply_transform() const override;
};

using base_transformable_obj = transformable_obj<false, false, false>;
using movable_obj = transformable_obj<true, false, false>;
using scalable_obj = transformable_obj<false, true, false>;
using rotatable_obj = transformable_obj<false, false, true>;

template <>
class transformable_obj<true, true, true> : public virtual movable_obj, public virtual scalable_obj, public virtual rotatable_obj
{
public:
	inline transformable_obj() = default;
	inline transformable_obj(const render_type &rtype) : render_obj(rtype) {}

	virtual ~transformable_obj() = default;

	static constexpr bool is_movable = true;
	static constexpr bool is_scalable = true;
	static constexpr bool is_rotatable = true;
protected:

	virtual void apply_transform() const override;
};

template <>
class transformable_obj<true, true, false> : public virtual movable_obj, public virtual scalable_obj
{
public:
	inline transformable_obj() = default;
	inline transformable_obj(const render_type &rtype) : render_obj(rtype) {}

	virtual ~transformable_obj() = default;

	static constexpr bool is_movable = true;
	static constexpr bool is_scalable = true;
	static constexpr bool is_rotatable = false;
protected:
	virtual void apply_transform() const override;
};

template <>
class transformable_obj<true, false, true> : public virtual movable_obj, public virtual rotatable_obj
{
public:
	inline transformable_obj() = default;
	inline transformable_obj(const render_type &rtype) : render_obj(rtype) {}

	virtual ~transformable_obj() = default;

	static constexpr bool is_movable = true;
	static constexpr bool is_scalable = false;
	static constexpr bool is_rotatable = true;
protected:
	virtual void apply_transform() const override;
};

template <>
class transformable_obj<false, true, true> : public virtual scalable_obj, public virtual rotatable_obj
{
public:
	inline transformable_obj() = default;
	inline transformable_obj(const render_type &rtype) : render_obj(rtype) {}

	virtual ~transformable_obj() = default;

	static constexpr bool is_movable = false;
	static constexpr bool is_scalable = true;
	static constexpr bool is_rotatable = true;
protected:
	virtual void apply_transform() const override;
};

SGL_END