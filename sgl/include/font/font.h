#pragma once
#include "math/vec.h"
#include <string>

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
	
	inline font(const std::string &file_name, unsigned int height) : font() { face.size = height; load(file_name); }
	inline font(void const *data, std::size_t size, unsigned int height) : font() { face.size = height; load(data, size); }

	void load(const std::string &file_name);
	void load(void const *data, std::size_t size);

	void set_character_size(unsigned int char_size);
	inline unsigned int get_character_size() const { return face.size; }

private:
	friend class text;

	struct face_handle
	{
		FT_FaceRec_* face;
		unsigned int size;

		face_handle();
		~face_handle();

		void load(detail::library_handle& lib, const std::string &file_name);
		void load(detail::library_handle& lib, void const *data, std::size_t size);

		face_handle(face_handle&& other);
		face_handle& operator=(face_handle&& other);

		void resize();
	};

	face_handle face;
};

SGL_END