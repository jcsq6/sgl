#pragma once
#include "codes.h"

#include "object/render_target.h"

#include <string>
#include <map>
#include <functional>

struct GLFWwindow;

SGL_BEG

class key
{
	bool was_pressed;
	bool pressed;

	friend class window;

public:
	inline key() : was_pressed{false}, pressed{false} {}

	inline bool is_pressed() const
	{
		return pressed;
	}

	inline bool is_repeated() const
	{
		return was_pressed && pressed;
	}

	inline bool is_initialPress() const
	{
		return pressed && !was_pressed;
	}
};

class window : public render_target
{
public:
	struct creation_hints
	{
		inline creation_hints(int major = 4, int minor = 1, int flags = creation_code::defaults) : window_flags{flags}, opengl_major{major}, opengl_minor{minor} {}
		int window_flags;
		int opengl_major;
		int opengl_minor;
	};

	window(int width, int height, std::string_view name, creation_hints hints = {});
	~window();

	bool should_close() const;
	void swap_buffers() const;
	void poll_events() const;

	key *get_key(key_code code) const;
	key *get_mouse_button(mouse_code code) const;

	ivec2 drawable_size() const override;
	ivec2 get_window_size() const;
	ivec2 get_framebuffer_size() const;

	/// @brief overriden method that is used with viewport
	/// @return framebuffer size
	ivec2 actual_size() const override;

	ivec2 get_window_pos() const;

	inline dvec2 get_mouse_pos() const { return mouse_pos; }

	/// @brief sets the number of screen updates that the window will wait for between swapping buffers
	/// @param interval number of screen updates
	static void set_swap_interval(int interval);

	void set_window_title(std::string_view name);
	void set_window_size(int width, int height);
	void set_logical_size(int width, int height);

	/// @brief set window minimum size and/or window maximum size (to specify no limit, set the component to -1)
	/// @param min_size minimum size
	/// @param max_size maximum size
	void set_window_size_limits(ivec2 min_size, ivec2 max_size);

	void set_aspect_ratio(int numerator, int denominator);

	void set_window_pos(ivec2 screen_pos);

	void set_should_close(bool should_close);

	void set_cursor_mode(state_code state);

	/// @brief set callback for when the user tries to close the window
	/// @param callback callable with no parameters
	void set_close_callback(std::function<void()> callback);

	/// @brief set callback for when the size of the window changes
	/// @param callback callable with the signature void(width, height), where width and height represent the new size of the window
	void set_windowsize_callback(std::function<void(int, int)> callback);

	/// @brief set callback for when the framebuffer size changes
	/// @param callback callable with the signature void(width, height), where width and height repressent the new size of the framebuffer
	/// @note There is no need to set a viewport callback, that is handled by the window
	void set_framebuffer_callback(std::function<void(int, int)> callback);

	/// @brief set callback for when the content scale of the window changes
	/// @param callback callable with the signature void(xscale, yscale), where x and yscale represent the new scale the current DPI and the platforms default DPI
	void set_contentscale_callback(std::function<void(float, float)> callback);

	/// @brief set callback for when the screen position of the window changes
	/// @param callback callable with the signature void(x, y), where x and y represent the new position of the window
	void set_windowpos_callback(std::function<void(int, int)> callback);

	/// @brief set callback for when the window is minimized
	/// @param callback callable with the signature void(minimized), where minimized is 1 if the window is minimized, 0 otherwise
	void set_windowminimize_callback(std::function<void(int)> callback);

	/// @brief set callback for when the window is maximized
	/// @param callback callable with the signature void(maximized), where maximized is 1 if the window is maximized, 0 otherwise
	void set_windowmaximize_callback(std::function<void(int)> callback);

	/// @brief set callback for when the window gains/loses focus
	/// @param callback callable with the signature void(focused), where focused is 1 if the window is focused, 0 otherwise
	void set_windowfocus_callback(std::function<void(int)> callback);

	/// @brief set callback for when a key is pressed or released
	/// @param callback callable with the signature (key, scancode, action, mods), where key is the key_code,
	/// 				scancode is the platform-dependant scancode, action is the state of the key, and mods is an int with relevant modifier bits set (defined in modifier_code enum)
	void set_key_callback(std::function<void(key_code, int, action_code, int)> callback);

	/// @brief set callback for text input
	/// @param callback callable with the signature void(codepoint), where codepoint is a character in UTF-32
	void set_character_callback(std::function<void(unsigned int)> callback);

	/// @brief set callback for cursor position
	/// @param callback callable with the signature void(x, y), where x and y is the position of the cursor
	void set_cursor_callback(std::function<void(double, double)> callback);

	/// @brief set callback for cursor entering/exiting
	/// @param callback callable with the signature void(entered), where entered is 1 if the mouse entered the window, and 0 otherwise
	void set_enterexit_callback(std::function<void(int)> callback);

	/// @brief set callback for when a mouse button is pressed or released
	/// @param callback callable with the signature void(button, action, mods), where button is the mouse_code, action is the action_code, and
	///					mods is an int with relevant modifier bits set (defined in modifier_code enum)
	void set_mousebutton_callback(std::function<void(mouse_code, action_code, int)> callback);

	/// @brief set callback for when the user scrolls the mouse
	/// @param callback callable with the signature void(xoff, yoff), where x and yoff are the horizontal and vertical offsets from the wheel, respectively
	void set_scroll_callback(std::function<void(double, double)> callback);

	void remove_windowsize_callback();
	void remove_framebuffer_callback();
	void remove_contentscale_callback();
	void remove_windowpos_callback();
	void remove_windowminimize_callback();
	void remove_windowmaximize_callback();
	void remove_windowfocus_callback();
	void remove_key_callback();
	void remove_character_callback();
	void remove_cursor_callback();
	void remove_enterexit_callback();
	void remove_mousebutton_callback();
	void remove_scroll_callback();

private:
	GLFWwindow *m_window;

	mutable std::map<key_code, key> keys;
	mutable std::map<mouse_code, key> buttons;

	mutable dvec2 mouse_pos;

	ivec2 logical_size;

	void bind_framebuffer() override;

	void set_framebuffer_callback();
};

SGL_END