#include "object/render_target.h"
#include "object/render_obj.h"
#include "context_lock/context_lock.h"

SGL_BEG

static const mat4 *projection_value = nullptr;
static const mat4 *view_value = nullptr;

void set_projection(const mat4 *projection)
{
	projection_value = projection;
}
const mat4 *get_projection()
{
	return projection_value;
}

void set_view(const mat4 *view)
{
	view_value = view;
}
const mat4 *get_view()
{
	return view_value;
}

void render_target::clear(vec4 color, GLbitfield mask)
{
	detail::fbo_lock flock;
	detail::viewport_lock vlock;

	bind_framebuffer();
	view.apply();

	glClearColor(color.x, color.y, color.z, color.w);
	glClear(mask);
}

void render_target::draw(const render_obj &obj, const render_settings &settings)
{
	// trusting that proper context locks are used
	obj.draw(*this, settings);
}

void render_target::draw(const render_obj &obj)
{
	// trusting that proper context locks are used
	obj.draw(*this);
}

ivec2 texture_target::drawable_size() const
{
	return { text->get_width(), text->get_height() };
}

ivec2 texture_target::actual_size() const
{
	return drawable_size();
}

void texture_target::bind_framebuffer()
{
	framebuffer.use();
}

// call this in overrides of render_target::draw, after using proper context locks
void render_type::bind_target(render_target &target) const
{
	target.view.apply();
	target.bind_framebuffer();
}

// call this in overrides of render_target::draw, after using proper context locks
void render_obj::bind_target(render_target &target) const
{
	target.view.apply();
	target.bind_framebuffer();
}

void render_obj::draw(render_target &target) const
{
	render_settings settings{ vec4{0, 0, 0, 1}, nullptr, nullptr };
	draw(target, settings);
}

void movable_obj::apply_transform() const
{
	model *= translate(m_loc);
}
void scalable_obj::apply_transform() const
{
	if (m_origin != vec3{})
	{
		model *= translate(m_origin);
		model *= scale(m_scale);
		model *= translate(-m_origin);
	}
	else
		model *= scale(m_scale);
}

void rotatable_obj::apply_transform() const
{
	if (m_angle != 0)
	{
		if (m_origin != vec3{})
		{
			model *= translate(m_origin);
			model *= rot(m_angle, m_axis);
			model *= translate(-m_origin);
		}
		else
			model *= rot(m_angle, m_axis);
	}
}

void transformable_obj<true, true, true>::apply_transform() const
{
	movable_obj::apply_transform();
	rotatable_obj::apply_transform();
	scalable_obj::apply_transform();
}

void transformable_obj<true, true, false>::apply_transform() const
{
	movable_obj::apply_transform();
	scalable_obj::apply_transform();
}

void transformable_obj<true, false, true>::apply_transform() const
{
	movable_obj::apply_transform();
	rotatable_obj::apply_transform();
}

void transformable_obj<false, true, true>::apply_transform() const
{
	rotatable_obj::apply_transform();
	scalable_obj::apply_transform();
}
SGL_END