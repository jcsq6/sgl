#include "window/window.h"

#include "utils/error.h"

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

SGL_BEG

struct sgl_glfw
{
	sgl_glfw()
	{
		res = glfwInit();
		if (res != GLFW_TRUE)
			detail::log_error(error("GLFW initializatino failed with code " + std::to_string(res) + '.', error_code::glfw_initialization_failure));
	}
	~sgl_glfw()
	{
		glfwTerminate();
	}

	int res;
};

sgl_glfw &get_glfwlib()
{
	static sgl_glfw lib;
	return lib;
}

struct callbacks
{
	std::function<void()> close;
	std::function<void(int, int)> windowsize;
	std::function<void(int, int)> framebuffer;
	std::function<void(float, float)> contentscale;
	std::function<void(int, int)> windowpos;
	std::function<void(int)> windowminimize;
	std::function<void(int)> windowmaximize;
	std::function<void(int)> windowfocus;
	std::function<void(key_code, int, action_code, int)> key;
	std::function<void(unsigned int)> character;
	std::function<void(double, double)> cursor;
	std::function<void(int)> enter;
	std::function<void(mouse_code, action_code, int)> mouse_button;
	std::function<void(double, double)> scroll;
	window *parent;
};

std::map<GLFWwindow *, callbacks> window_callbacks;

void framebuffer_callback(GLFWwindow *window, int width, int height) { window_callbacks[window].framebuffer(width, height); }

window::window(int width, int height, std::string_view name, window::creation_hints hints) : m_window{}, logical_size{-1, -1}
{
	if (get_glfwlib().res != GLFW_TRUE)
		return;

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, hints.opengl_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, hints.opengl_minor);

	glfwWindowHint(GLFW_RESIZABLE, !(hints.window_flags & creation_code::is_not_resizable));
	glfwWindowHint(GLFW_VISIBLE, !(hints.window_flags & creation_code::is_invisible));
	glfwWindowHint(GLFW_DECORATED, !(hints.window_flags & creation_code::is_plain));
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, !!(hints.window_flags & creation_code::should_scale_to_monitor));
	glfwWindowHint(GLFW_AUTO_ICONIFY, !!(hints.window_flags & creation_code::should_auto_minimize));
	glfwWindowHint(GLFW_FLOATING, !!(hints.window_flags & creation_code::is_always_top));
	glfwWindowHint(GLFW_MAXIMIZED, !!(hints.window_flags & creation_code::is_maximized));
	glfwWindowHint(GLFW_CENTER_CURSOR, !!(hints.window_flags & creation_code::cursor_is_centered));
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, !!(hints.window_flags & creation_code::framebuffer_is_transparent));

	m_window = glfwCreateWindow(width, height, name.data(), nullptr, nullptr);

	view.pos = {0, 0};
	view.size = get_framebuffer_size();

	window_callbacks[m_window].parent = this;

	glfwSetFramebufferSizeCallback(m_window, framebuffer_callback);
	set_framebuffer_callback();

	if (m_window)
	{
		glfwMakeContextCurrent(m_window);
		if (glewInit() != GLEW_OK)
			detail::log_error(error("Couldn't load glew.", error_code::glew_initialization_failure));
	}
	else
		detail::log_error(error("Couldn't create window.", error_code::window_creation_failure));
}

window::~window()
{
	glfwDestroyWindow(m_window);
}

bool window::should_close() const
{
	return glfwWindowShouldClose(m_window);
}

void window::swap_buffers() const
{
	glfwSwapBuffers(m_window);
}

void mouse_pos_interp(sgl::dvec2 window_size, sgl::dvec2 target_size, sgl::dvec2 &mouse_pos)
{
	mouse_pos.x = target_size.x / window_size.x * mouse_pos.x;
	mouse_pos.y = target_size.y / -window_size.y * (mouse_pos.y - window_size.y);
}

void window::poll_events() const
{
	glfwPollEvents();

	for (auto &k : keys)
	{
		k.second.was_pressed = k.second.pressed;
		k.second.pressed = glfwGetKey(m_window, static_cast<int>(k.first)) == GLFW_PRESS;
	}

	for (auto &b : buttons)
	{
		b.second.was_pressed = b.second.pressed;
		b.second.pressed = glfwGetMouseButton(m_window, static_cast<int>(b.first)) == GLFW_PRESS;
	}

	glfwGetCursorPos(m_window, &mouse_pos.x, &mouse_pos.y);

	mouse_pos_interp(get_window_size(), drawable_size(), mouse_pos);
}

key *window::get_key(key_code code) const
{
	if (!keys.count(code))
		keys[code].pressed = glfwGetKey(m_window, static_cast<int>(code));
	return &keys[code];
}

key *window::get_mouse_button(mouse_code code) const
{
	if (!buttons.count(code))
		buttons[code].pressed = glfwGetMouseButton(m_window, static_cast<int>(code));
	return &buttons[code];
}

ivec2 window::drawable_size() const
{
	if (logical_size != ivec2{-1, -1})
		return logical_size;
	return get_framebuffer_size();
}

ivec2 window::get_window_size() const
{
	ivec2 res;
	glfwGetWindowSize(m_window, &res.x, &res.y);
	return res;
}

ivec2 window::get_framebuffer_size() const
{
	ivec2 res;
	glfwGetFramebufferSize(m_window, &res.x, &res.y);
	return res;
}

ivec2 window::actual_size() const
{
	return get_framebuffer_size();
}

ivec2 window::get_window_pos() const
{
	ivec2 res;
	glfwGetWindowPos(m_window, &res.x, &res.y);
	return res;
}

void window::set_swap_interval(int interval)
{
	glfwSwapInterval(interval);
}

void window::set_window_title(std::string_view name)
{
	glfwSetWindowTitle(m_window, name.data());
}

void window::set_window_size(int width, int height)
{
	glfwSetWindowSize(m_window, width, height);
}

void window::set_logical_size(int width, int height)
{
	logical_size.x = width;
	logical_size.y = height;
}

void window::set_window_size_limits(ivec2 min_size, ivec2 max_size)
{
	glfwSetWindowSizeLimits(m_window, min_size.x, min_size.y, max_size.x, max_size.y);
}

void window::set_aspect_ratio(int numerator, int denominator)
{
	glfwSetWindowAspectRatio(m_window, numerator, denominator);
}

void window::set_window_pos(ivec2 screen_pos)
{
	glfwSetWindowPos(m_window, screen_pos.x, screen_pos.y);
}

void window::set_should_close(bool should_close)
{
	glfwSetWindowShouldClose(m_window, should_close);
}

void window::set_cursor_mode(state_code state)
{
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + static_cast<int>(state));
}

void close_callback(GLFWwindow *window) { window_callbacks[window].close(); }
void window::set_close_callback(std::function<void()> callback)
{
	window_callbacks[m_window].close = std::move(callback);
	glfwSetWindowCloseCallback(m_window, close_callback);
}

void windowsize_callback(GLFWwindow *window, int width, int height) { window_callbacks[window].windowsize(width, height); }
void window::set_windowsize_callback(std::function<void(int, int)> callback)
{
	window_callbacks[m_window].windowsize = std::move(callback);
	glfwSetWindowSizeCallback(m_window, windowsize_callback);
}

void window::set_framebuffer_callback(std::function<void(int, int)> callback)
{
	window_callbacks[m_window].framebuffer = [this, callback = std::move(callback)](int width, int height)
	{
		view.size.x = width;
		view.size.y = height;
		callback(width, height);
	};
}

void window::set_framebuffer_callback()
{
	window_callbacks[m_window].framebuffer = [this](int width, int height)
	{
		view.size.x = width;
		view.size.y = height;
	};
}

void contentscale_callback(GLFWwindow *window, float xscale, float yscale) { window_callbacks[window].contentscale(xscale, yscale); }
void window::set_contentscale_callback(std::function<void(float, float)> callback)
{
	window_callbacks[m_window].contentscale = std::move(callback);
	glfwSetWindowContentScaleCallback(m_window, contentscale_callback);
}

void windowpos_callback(GLFWwindow *window, int x, int y) { window_callbacks[window].windowpos(x, y); }
void window::set_windowpos_callback(std::function<void(int, int)> callback)
{
	window_callbacks[m_window].windowpos = std::move(callback);
	glfwSetWindowPosCallback(m_window, windowpos_callback);
}

void windowminimize_callback(GLFWwindow *window, int minimized) { window_callbacks[window].windowminimize(minimized); }
void window::set_windowminimize_callback(std::function<void(int)> callback)
{
	window_callbacks[m_window].windowminimize = std::move(callback);
	glfwSetWindowIconifyCallback(m_window, windowminimize_callback);
}

void windowmaximize_callback(GLFWwindow *window, int maximzed) { window_callbacks[window].windowmaximize(maximzed); }
void window::set_windowmaximize_callback(std::function<void(int)> callback)
{
	window_callbacks[m_window].windowmaximize = std::move(callback);
	glfwSetWindowMaximizeCallback(m_window, windowmaximize_callback);
}

void windowfocus_callback(GLFWwindow *window, int focused) { window_callbacks[window].windowfocus(focused); }
void window::set_windowfocus_callback(std::function<void(int)> callback)
{
	window_callbacks[m_window].windowfocus = std::move(callback);
	glfwSetWindowFocusCallback(m_window, windowfocus_callback);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) { window_callbacks[window].key((key_code)key, scancode, (action_code)action, mods); }
void window::set_key_callback(std::function<void(key_code, int, action_code, int)> callback)
{
	window_callbacks[m_window].key = std::move(callback);
	glfwSetKeyCallback(m_window, key_callback);
}

void character_callback(GLFWwindow *window, unsigned int codepoint) { window_callbacks[window].character(codepoint); }
void window::set_character_callback(std::function<void(unsigned int)> callback)
{
	window_callbacks[m_window].character = std::move(callback);
	glfwSetCharCallback(m_window, character_callback);
}

void cursor_callback(GLFWwindow *window, double x, double y)
{
	auto *parent = window_callbacks[window].parent;
	sgl::dvec2 mouse_pos(x, y);
	mouse_pos_interp(parent->get_window_size(), parent->drawable_size(), mouse_pos);
	window_callbacks[window].cursor(mouse_pos.x, mouse_pos.y);
}
void window::set_cursor_callback(std::function<void(double, double)> callback)
{
	window_callbacks[m_window].cursor = std::move(callback);
	glfwSetCursorPosCallback(m_window, cursor_callback);
}

void enterexit_callback(GLFWwindow *window, int entered) { window_callbacks[window].enter(entered); }
void window::set_enterexit_callback(std::function<void(int)> callback)
{
	window_callbacks[m_window].enter = std::move(callback);
	glfwSetCursorEnterCallback(m_window, enterexit_callback);
}

void mousebutton_callback(GLFWwindow *window, int button, int action, int mods) { window_callbacks[window].mouse_button((mouse_code)button, (action_code)action, mods); }
void window::set_mousebutton_callback(std::function<void(mouse_code, action_code, int)> callback)
{
	window_callbacks[m_window].mouse_button = std::move(callback);
	glfwSetMouseButtonCallback(m_window, mousebutton_callback);
}

void scroll_callback(GLFWwindow *window, double xoff, double yoff) { window_callbacks[window].scroll(xoff, yoff); }
void window::set_scroll_callback(std::function<void(double, double)> callback)
{
	window_callbacks[m_window].scroll = std::move(callback);
	glfwSetScrollCallback(m_window, scroll_callback);
}

void window::remove_windowsize_callback() { glfwSetWindowSizeCallback(m_window, nullptr); }
void window::remove_framebuffer_callback() { set_framebuffer_callback(); }
void window::remove_contentscale_callback() { glfwSetWindowContentScaleCallback(m_window, nullptr); }
void window::remove_windowpos_callback() { glfwSetWindowPosCallback(m_window, nullptr); }
void window::remove_windowminimize_callback() { glfwSetWindowIconifyCallback(m_window, nullptr); }
void window::remove_windowmaximize_callback() { glfwSetWindowMaximizeCallback(m_window, nullptr); }
void window::remove_windowfocus_callback() { glfwSetWindowFocusCallback(m_window, nullptr); }
void window::remove_key_callback() { glfwSetKeyCallback(m_window, nullptr); }
void window::remove_character_callback() { glfwSetCharCallback(m_window, nullptr); }
void window::remove_cursor_callback() { glfwSetCursorPosCallback(m_window, nullptr); }
void window::remove_enterexit_callback() { glfwSetCursorEnterCallback(m_window, nullptr); }
void window::remove_mousebutton_callback() { glfwSetMouseButtonCallback(m_window, nullptr); }
void window::remove_scroll_callback() { glfwSetScrollCallback(m_window, nullptr); }

void window::bind_framebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SGL_END