#pragma once
#include "math/mat.h"
#include "buffers.h"
#include "texture.h"

SGL_BEG

class render_type;
class render_obj;
struct render_settings;

struct viewport
{
	ivec2 pos;
	ivec2 size;

	inline void apply() const
	{
		glViewport(pos.x, pos.y, size.x, size.y);
	}
};

/// @brief set projection (orthographic/perspective) for following rendering operations
/// @param projection pointer to projection. If nullptr, then a default orthographic projection will be used
void set_projection(const mat4 *projection);
/// @brief get projection currently used for rendering operations
/// @return pointer to projection matrix
const mat4 *get_projection();

/// @brief set view matrix (view space/camera) for following rendering operations
/// @param view pointer to view. If nullptr, then no view matrix will be used
void set_view(const mat4 *view);

/// @brief get view currently used for rendering operations
/// @return pointer to view matrix
const mat4 *get_view();

class render_target
{
public:
	void clear(vec4 color, GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	void draw(const render_obj &obj, const render_settings &settings);
	void draw(const render_obj &obj);

	inline void set_viewport(viewport view)
	{
		this->view = view;
	}
	inline void set_logical_viewport()
	{
		set_viewport({{0, 0}, actual_size()});
	}

	viewport get_viewport() const
	{
		return view;
	}

	virtual ivec2 drawable_size() const = 0;
	
	/// @brief used to set viewport size
	/// @return dimensions of target
	virtual ivec2 actual_size() const = 0;

	virtual ~render_target() = default;

protected:
	viewport view;

	friend render_obj;
	friend render_type;

	virtual void bind_framebuffer() = 0;
};

class texture_target : public render_target
{
public:
	inline texture_target() : framebuffer{}, text{} {}
	inline texture_target(texture &texture) : framebuffer{}, text{&texture}
	{
		framebuffer.generate();
		framebuffer.attach_data(texture, GL_COLOR_ATTACHMENT0);

		set_logical_viewport();
	}

	inline void attach_texture(texture &texture)
	{
		text = &texture;
		if (!framebuffer.index())
			framebuffer.generate();
		framebuffer.attach_data(texture, GL_COLOR_ATTACHMENT0);
	
		set_logical_viewport();
	}

	ivec2 drawable_size() const override;
	ivec2 actual_size() const override;

private:
	fbo framebuffer;
	texture *text;

	void bind_framebuffer() override;
};

SGL_END