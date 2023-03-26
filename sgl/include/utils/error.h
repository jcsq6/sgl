#pragma once
#include "macro.h"

#include <fstream>
#include <iostream>

SGL_BEG

enum class error_code
{
	no_error,
	shader_compilation_failure,
	shader_program_compilation_failure,
	glew_initialization_failure,
	glfw_initialization_failure,
	window_creation_failure,
	file_open_failure,
	unrecognized_file_format,
	freetype_initialization_failure,
	freetype_invalid_character,
	freetype_font_failure,
	invalid_argument,
	uknown_error,
};

class error
{
public:
	inline error() : m_message{}, m_code{error_code::no_error} {}
	inline error(const char *message, error_code code) : m_message{message}, m_code{code} {}
	inline error(std::string message, error_code code) : m_message{ std::move(message ) }, m_code{ code } {}

	inline const std::string &message() const { return m_message; }
	inline error_code code() const { return m_code; }
private:
	std::string m_message;
	error_code m_code;
};

// returns true if there is an error in queue
bool get_error(error &err);

DETAIL_BEG
void log_error(error err);
DETAIL_END

SGL_END