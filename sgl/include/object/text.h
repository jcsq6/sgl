#pragma once
#include "render_obj.h"
#include "math/vec.h"
#include "math/bound.h"
#include "texture.h"

#include <string>
#include <map>

struct FT_FaceRec_;

SGL_BEG

DETAIL_BEG
struct library_handle;
DETAIL_END

class font
{
public:
	static constexpr unsigned int default_height = 48;

	font() = default;

	inline font(const std::string &file_name, unsigned int height) : font()
	{
		load(file_name, height);
	}
	inline font(const void *data, std::size_t size, unsigned int height) : font()
	{
		load(data, size, height);
	}

	void load(const std::string &file_name, unsigned int height);
	void load(const void *data, std::size_t size, unsigned int height);

	inline unsigned int get_character_height() const { return face.size; }

private:
	friend class text;

	struct face_handle
	{
		FT_FaceRec_ *face;
		unsigned int size;

		face_handle();
		~face_handle();

		void load(detail::library_handle &lib, const std::string &file_name);
		void load(detail::library_handle &lib, const void *data, std::size_t size);

		face_handle(face_handle &&other) noexcept;
		face_handle &operator=(face_handle &&other) noexcept;

		void resize();
	};

	face_handle face;

	struct character
	{
		inline character() noexcept : text{}, offset{}, advance{}, height{} {}

		character(const font *_font, uint32_t c);
		~character() = default;

		// dummy functions to enable use with std::map
		character(const character &other);
		character &operator=(const character &other);

		character(character &&other) noexcept;
		character &operator=(character &&other) noexcept;

		void load(const font *_font, uint32_t c);

		texture text;

		ivec2 offset;
		unsigned int advance;

		unsigned int height;
	};

	inline character const *at(uint32_t c) const
	{
		if (!m_chars.count(c))
			return &m_chars.emplace(c, character{this, c}).first->second;
		return &m_chars.at(c);
	}

	// mutable to allow potential addition of new characters in draw function
	mutable std::map<uint32_t, character> m_chars;
};

class text : public render_obj
{
public:
	inline text() : render_obj(),
					m_origin{},
					m_dir{1, 0, 0},
					m_up{0, 1, 0},
					m_scale{1, 1},
					m_rot_origin{},
					m_axis{0, 0, 1},
					m_font{},
					m_angle{},
					m_data{}
	{
	}
	inline text(font &_font) : render_obj(),
							   m_origin{},
							   m_dir{1, 0, 0},
							   m_up{0, 1, 0},
							   m_scale{1, 1},
							   m_rot_origin{},
							   m_axis{0, 0, 1},
							   m_font{&_font},
							   m_angle{},
							   m_data{}
	{
	}
	inline text(std::basic_string_view<char> txt, font &_font) : render_obj(),
																 m_origin{},
																 m_dir{1, 0, 0},
																 m_up{0, 1, 0},
																 m_scale{1, 1},
																 m_rot_origin{},
																 m_axis{0, 0, 1},
																 m_font{&_font},
																 m_angle{},
																 m_data{txt.begin(), txt.end()}
	{
	}

	inline text(std::basic_string_view<wchar_t> txt, font &_font) : render_obj(),
																	m_origin{},
																	m_dir{1, 0, 0},
																	m_up{0, 1, 0},
																	m_scale{1, 1},
																	m_rot_origin{},
																	m_axis{0, 0, 1},
																	m_font{ &_font },
																	m_angle{},
																	m_data{txt.begin(), txt.end()}
	{
	}

	inline text(std::basic_string_view<uint32_t> txt, font &_font) : render_obj(),
																	 m_origin{},
																	 m_dir{1, 0, 0},
																	 m_up{0, 1, 0},
																	 m_scale{1, 1},
																	 m_rot_origin{},
																	 m_axis{0, 0, 1},
																	 m_font{ &_font },
																	 m_angle{},
																	 m_data{txt.begin(), txt.end()}
	{
	}

	inline void set_string(std::basic_string_view<char> txt)
	{
		m_data.assign(txt.begin(), txt.end());
	}

	inline void set_string(std::basic_string_view<wchar_t> txt)
	{
		m_data.assign(txt.begin(), txt.end());
	}

	inline void set_string(std::basic_string_view<uint32_t> txt)
	{
		m_data.assign(txt.begin(), txt.end());
	}

	inline const std::basic_string<uint32_t> &get_string() const
	{
		return m_data;
	}

	inline void clear()
	{
		m_data.clear();
	}

	template <typename... Ts>
	inline void insert(Ts &&...args)
	{
		m_data.insert(std::forward<Ts>(args)...);
	}

	template <typename... Ts>
	inline void erase(Ts &&...args)
	{
		m_data.erase(std::forward<Ts>(args)...);
	}

	inline void push_back(uint32_t c)
	{
		m_data.push_back(c);
	}

	inline void pop_back()
	{
		m_data.pop_back();
	}

	template <typename... Ts>
	inline void append(Ts &&...args)
	{
		m_data.append(std::forward<Ts>(args)...);
	}

	template <typename... Ts>
	inline void replace(Ts &&...args)
	{
		m_data.replace(std::forward<Ts>(args)...);
	}

	template <typename... Ts>
	inline void resize(Ts... args)
	{
		m_data.resize(args...);
	}

	inline void set_text_origin(vec3 origin) { m_origin = origin; }
	inline vec3 get_text_origin() const { return m_origin; }

	inline void set_dir(vec3 dir) { m_dir = normalize(dir); }
	inline vec3 get_dir() const { return m_dir; }

	inline void set_up(vec3 up) { m_up = normalize(up); }
	inline vec3 get_up() const { return m_up; }

	inline void set_scale(vec2 scale) { m_scale = scale; }
	inline vec2 get_scale() const { return m_scale; }

	// rot_origin is local to the object, not the world
	inline void set_rot_origin(vec3 origin) { m_rot_origin = origin; }
	inline vec3 get_rot_origin() const { return m_rot_origin; }

	inline void set_rot_axis(vec3 axis) { m_axis = normalize(axis); }
	inline vec3 get_rot_axis() const { return m_axis; }

	inline void set_angle(float angle) { m_angle = angle; }
	inline float get_angle() const { return m_angle; }

	inline void set_font(font& _font) noexcept { m_font = &_font; }
	inline font const* get_font() const noexcept { return m_font;  }

	/// @brief get's rect with local bounds of the text with the origin as (0,0). The minimum of the returned rect is not neccessarily (0,0). This function doesn't take into account direction
	/// @return rect including the min of the text bounds, and the dimensions
	rect get_local_rect() const;

	/// @brief get's bound with local bounds of text. The minimum is not neccessarily (0,0,0). This function takes direction into account. Doesn't take rotation into account
	/// @return bound including min of 3d text bounds, and dimensions
	bound get_local_bound() const;

	void draw(render_target &target, const render_settings &settings) const override;
	void draw(render_target &target) const override;

private:
	std::basic_string<uint32_t> m_data;
	vec3 m_origin;
	vec3 m_dir;
	vec3 m_up;
	vec3 m_rot_origin;
	vec3 m_axis;
	vec2 m_scale;
	font* m_font;
	float m_angle;
};
SGL_END