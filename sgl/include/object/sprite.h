#pragma once
#include "shapes.h"
#include "gtexture.h"

SGL_BEG

template <bool rotatable = false>
class sprite : public rectangle_obj<rotatable>
{
public:
	/// @brief construct sprite object
	/// @param min minimum of rect in 3d space
	/// @param size 2d size of rectangle (size.x/y do not neccessarily correspond to actual x/y axis)
	/// @param right direction corresponding to x axis in parameter size
	/// @param up direction corresponding to y axis in parameter size
	sprite(const gtexture &texture, vec3 min, vec2 size, vec3 right = {1, 0, 0}, vec3 up = {0, 1, 0});

	inline sprite(const sprite &) = default;
	inline sprite &operator=(const sprite &) = default;

	inline const gtexture& get_texture() const
	{
		return *m_texture;
	}

	inline void set_texture(const gtexture& texture)
	{
		m_texture = &texture;
	}

	void draw(render_target& target) const override;
private:
	const gtexture* m_texture;
};

SGL_END