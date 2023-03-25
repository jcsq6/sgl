#include "object/text.h"

#include "object/render_target.h"

#include "object/buffers.h"
#include "shaders/shaders.h"

#include "object/sprite.h"

#include "defaults.h"

#include <stdexcept>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

SGL_BEG

DETAIL_BEG
struct library_handle
{
	FT_Library library;

	library_handle() : library{}
	{
		if (FT_Init_FreeType(&library))
			throw std::runtime_error("could not initialize freetype");
	}
	~library_handle()
	{
		FT_Done_FreeType(library);
	}
};
DETAIL_END

detail::library_handle &get_library()
{
	static detail::library_handle lib;
	return lib;
}

font::face_handle::face_handle() : face{}, size{default_height} {}

void font::face_handle::load(detail::library_handle &lib, const std::string &file_name)
{
	if (FT_New_Face(lib.library, file_name.data(), 0, &face))
		throw std::runtime_error("could not load font");
}
void font::face_handle::load(detail::library_handle &lib, const void *data, std::size_t size)
{
	if (FT_New_Memory_Face(lib.library, reinterpret_cast<const FT_Byte *>(data), static_cast<FT_Long>(size), 0, &face))
		throw std::runtime_error("could not load font");
}

font::face_handle::~face_handle()
{
	FT_Done_Face(face);
}

font::face_handle::face_handle(face_handle &&other) noexcept : face{other.face}, size{other.size}
{
	other.face = nullptr;
	other.size = default_height;
}
font::face_handle &font::face_handle::operator=(face_handle &&other) noexcept
{
	FT_Done_Face(face);
	face = other.face;
	size = other.size;
	other.face = nullptr;
	other.size = default_height;

	return *this;
}

void font::face_handle::resize()
{
	FT_Set_Pixel_Sizes(face, 0, size);
}

void font::load(const std::string &file_name, unsigned int height)
{
	m_chars.clear();
	
	face.load(get_library(), file_name);

	face.size = height;
	face.resize();
}

void font::load(const void *data, std::size_t size, unsigned int height)
{
	m_chars.clear();

	face.load(get_library(), data, size);
	
	face.size = height;
	face.resize();
}

font::character::character(const font *_font, uint32_t c) : text{}, offset{}, advance{}, height{}
{
	load(_font, c);
}

font::character::character(character &&other) noexcept : text{std::move(other.text)}, offset{other.offset}, advance{other.advance}, height{other.height}
{
	other.offset = {};
	other.height = {};
	other.advance = {};
}

font::character &font::character::operator=(font::character &&other) noexcept
{
	text.destroy();

	text = std::move(other.text);
	offset = other.offset;
	advance = other.advance;
	height = other.height;

	other.offset = {};
	other.height = {};
	other.advance = {};

	return *this;
}

void font::character::load(const font *_font, uint32_t c)
{
	height = _font->get_character_height();

	FT_Face face = _font->face.face;
	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		throw std::runtime_error("couldn't load character");
	text.load(GL_RGBA, face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 1);
	offset.x = face->glyph->bitmap_left;
	offset.y = face->glyph->bitmap_top;
	advance = face->glyph->advance.x;

	text.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	text.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	text.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	text.set_parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	static vec4 transparent{0, 0, 0, 0};

	text.set_parameter(GL_TEXTURE_BORDER_COLOR, value(transparent));
}

font::character::character(const font::character &other)
{
	throw std::runtime_error("You shouldn't be able to get here... For some reason the copy constructor was called for character");
}
font::character &font::character::operator=(const font::character &other)
{
	throw std::runtime_error("You shouldn't be able to get here... For some reason the copy assignment was called for character");
}

namespace text_detail
{
	render_shader &get_shader()
	{
		using namespace variables;
		static std::string vertex_source = "void main() { gl_Position = sgl_ModelViewProj * vec4(sgl_Pos, 1.0); sgl_VertTextPos = sgl_TextPos; }";
		static std::string fragment_source = "void main() { sgl_OutColor = vec4(sgl_Color.xyz, sgl_Color.w * texture(sgl_Texture, sgl_VertTextPos).r); }";
		static render_shader shader(vertex_source, fragment_source, sgl_ModelViewProj | sgl_Pos | sgl_TextPos | sgl_VertTextPos | sgl_Color | sgl_Texture);
		return shader;
	}

	inline void setup_shader(const gtexture &text, const mat4 &m, render_shader &program)
	{
		program.set_texture_uniform(text);
		program.set_color_uniform(get_draw_color());

		auto *v = get_view();
		auto *p = get_projection();

		if (!p)
			p = &global_defaults::identity_ref();
		if (!v)
			v = &global_defaults::identity_ref();

		if (program.has_modelViewProj_uniform())
			program.set_modelViewProj_uniform((*p) * (*v) * m);
		if (program.has_model_uniform())
			program.set_model_uniform(m);
		if (program.has_view_uniform())
			program.set_view_uniform(*v);
		if (program.has_proj_uniform())
			program.set_proj_uniform(*p);
	
		program.bind();
	}

	class char_type : public render_type
	{
	public:
		char_type() : render_type()
		{
			detail::vao_lock lvao;
			detail::vbo_lock lvbo;

			m_vao.generate();
			m_vao.use();

			static sgl::shape_data_array<4, 2> text_pts_data = {
			{
				{0, 0},
				{1, 0},
				{1, 1},
				{0, 1},
			}};

			static auto text_pts_buffer = text_pts_data.get_buffer();

			global_defaults::rect_obj_vbo().use();
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			text_pts_buffer.use();
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
		}

		void draw(render_target &target) const override
		{
			detail::vao_lock vlock;
			detail::fbo_lock flock;

			bind_target(target);

			m_vao.use();
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		static char_type& get_instance()
		{
			static char_type res;
			return res;
		}
	};

	class char_obj : public rectangle_obj<true>
	{
	public:
		char_obj(vec3 min, vec2 size, vec3 right, vec3 up) : render_obj(char_type::get_instance()),
															 movable_obj(min),
												   			 rectangle_obj<true>::rectangle_obj(min, size, right, up),
															 m_texture{}
		{
		}

		void draw(render_target& target) const override
		{
			base_transformable_obj::update_model();

			detail::cull_face_lock clock;
			detail::shader_lock slock;

			rectangle_obj<true>::setup_buffer();

			glDisable(GL_CULL_FACE);

			text_detail::setup_shader(*m_texture, base_transformable_obj::model, text_detail::get_shader());

			render_obj::type->draw(target);
		}

		const gtexture* m_texture;
	};
}

void text::draw(render_target &target) const
{
	detail::blend_lock lock;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto* cur = m_font->at(m_data.front());

	// remove first character's horizontal offset
	auto origin = m_origin - cur->offset.x * m_dir * m_scale.x;
	text_detail::char_obj cspr({}, {}, m_dir, m_up);

	for (uint32_t c : m_data)
	{
		cur = m_font->at(c);

		cspr.m_texture = &cur->text;

		vec2 sz(cur->text.get_width(), cur->text.get_height());
		vec3 cur_loc = origin + (cur->offset.x * m_scale.x * m_dir) + ((cur->offset.y - sz.y) * m_scale.y * m_up);
		origin += (cur->advance >> 6) * m_scale.x * m_dir;

		cspr.set_loc(cur_loc);
		cspr.set_size(sz * m_scale);

		cspr.set_angle(m_angle);
		cspr.set_rot_origin(m_rot_origin - cur_loc);
		cspr.set_rot_axis(m_axis);

		cspr.draw(target);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

rect text::get_local_rect() const
{
	rect res{{0, 0}, {0, 0}};

	if (m_data.empty())
		return res;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	vec2 max{-m_font->at(m_data.front())->offset.x, 0};

	auto end = m_data.end() - 1;

	font::character const *cur;

	for (auto it = m_data.begin(); it != end; ++it)
	{
		cur = m_font->at(*it);
		max.x += cur->advance >> 6;

		if (float pot = (float)cur->offset.y - cur->text.get_height(); pot < res.min.y)
			res.min.y = pot;
		if (cur->offset.y > max.y)
			max.y = (float)cur->offset.y;
	}

	cur = m_font->at(*end);
	max.x += cur->offset.x + cur->text.get_width();

	if (float pot = (float)cur->offset.y - cur->text.get_height(); pot < res.min.y)
		res.min.y = pot;
	if (cur->offset.y > max.y)
		max.y = (float)cur->offset.y;

	res.dims = max - res.min;

	res.dims *= m_scale;
	res.min *= m_scale;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	return res;
}

bound text::get_local_bound() const
{
	font::character const* cur = m_font->at(m_data.front());

	// remove first character's horizontal offset
	auto max = -cur->offset.x * m_dir;

	auto end = m_data.end() - 1;

	float min_y = 0;
	float max_y = 0;

	for (auto it = m_data.begin(); it != end; ++it)
	{
		cur = m_font->at(*it);
		max += (cur->advance >> 6) * m_dir;

		if (float pot = (float)cur->offset.y - cur->text.get_height(); pot < min_y)
			min_y = pot;
		if (cur->offset.y > max_y)
			max_y = (float)cur->offset.y;
	}

	cur = m_font->at(*end);
	max += (cur->offset.x + cur->text.get_width()) * m_dir;

	if (float pot = (float)cur->offset.y - cur->text.get_height(); pot < min_y)
		min_y = pot;
	if (cur->offset.y > max.y)
		max_y = (float)cur->offset.y;


	bound res{};
	max += m_up * max_y;
	res.min = m_up * min_y;
	res.dims = max - res.min;

	vec3 scale = m_dir * m_scale.x + m_up * m_scale.y;

	res.dims *= scale;
	res.min *= scale;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	return res;
}

SGL_END