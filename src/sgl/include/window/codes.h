#pragma once
#include "macro.h"

SGL_BEG

enum class state_code
{
	enabled = 0,
	hidden = 1,
	disabled = 2,
};

enum class key_code : int
{
	uknown = -1,
	space = 32,
	apostrophe = 39,
	comma = 44,
	minus = 45,
	period = 46,
	slash = 47,
	key_0 = 48,
	key_1 = 49,
	key_2 = 50,
	key_3 = 51,
	key_4 = 52,
	key_5 = 53,
	key_6 = 54,
	key_7 = 55,
	key_8 = 56,
	key_9 = 57,
	semicolon = 59,
	equal = 61,
	a = 65,
	b = 66,
	c = 67,
	d = 68,
	e = 69,
	f = 70,
	g = 71,
	h = 72,
	i = 73,
	j = 74,
	k = 75,
	l = 76,
	m = 77,
	n = 78,
	o = 79,
	p = 80,
	q = 81,
	r = 82,
	s = 83,
	t = 84,
	u = 85,
	v = 86,
	w = 87,
	x = 88,
	y = 89,
	z = 90,
	left_bracket = 91,
	backslash = 92,
	right_bracket = 93,
	grave_accent = 96,
	escape = 256,
	enter = 257,
	tab = 258,
	backspace = 259,
	insert = 260,
	key_delete = 261,
	right = 262,
	left = 263,
	down = 264,
	up = 265,
	page_up = 266,
	page_down = 267,
	home = 268,
	end = 269,
	caps_lock = 280,
	scroll_lock = 281,
	num_lock = 282,
	print_screen = 283,
	pause = 284,
	f1 = 290,
	f2 = 291,
	f3 = 292,
	f4 = 293,
	f5 = 294,
	f6 = 295,
	f7 = 296,
	f8 = 297,
	f9 = 298,
	f10 = 299,
	f11 = 300,
	f12 = 301,
	f13 = 302,
	f14 = 303,
	f15 = 304,
	f16 = 305,
	f17 = 306,
	f18 = 307,
	f19 = 308,
	f20 = 309,
	f21 = 310,
	f22 = 311,
	f23 = 312,
	f24 = 313,
	f25 = 314,
	keypad_0 = 320,
	keypad_1 = 321,
	keypad_2 = 322,
	keypad_3 = 323,
	keypad_4 = 324,
	keypad_5 = 325,
	keypad_6 = 326,
	keypad_7 = 327,
	keypad_8 = 328,
	keypad_9 = 329,
	keypad_decimal = 330,
	keypad_divide = 331,
	keypad_multiply = 332,
	keypad_subtract = 333,
	keypad_add = 334,
	keypad_enter = 335,
	keypad_equal = 336,
	left_shift = 340,
	left_control = 341,
	left_alt = 342,
	left_super = 343,
	right_shift = 344,
	right_control = 345,
	right_alt = 346,
	right_super = 347,
	menu = 348,
};

enum class mouse_code : int
{
	button_1 = 0,
	button_2 = 1,
	button_3 = 2,
	button_4 = 3,
	button_5 = 4,
	button_6 = 5,
	button_7 = 6,
	button_8 = 7,
	left = button_1,
	right = button_2,
	middle = button_3,
};

enum class action_code : int
{
	release = 0,
	press = 1,
	repeat = 2,
};

enum class modifier_code : int
{
	shift = 0x01,
	control = 0x02,
	alt = 0x04,
	super = 0x08,
	caps_lock = 0x10,
	num_lock = 0x20,
};

inline int operator&(int value, modifier_code code)
{
	return value & static_cast<int>(code);
}
inline int operator|(int value, modifier_code code)
{
	return value | static_cast<int>(code);
}
inline int operator^(int value, modifier_code code)
{
	return value ^ static_cast<int>(code);
}

namespace creation_code
{
	enum code : int
	{
		is_not_resizable = 0x0001,            // bit to set if the window should not be resizable
		is_invisible = 0x0002,                // bit to set if the window should not be visible
		is_plain = 0x0004,                    // bit to set if the window should have no decorations
		is_focused = 0x0008,                  // bit to set if the window should be focused
		should_auto_minimize = 0x0010,        // bit to set if the window should automatically minimize if lost focus
		is_always_top = 0x0020,               // bit to set if the window should remain above all other windows
		is_maximized = 0x0040,                // bit to set if the window should be maximized
		cursor_is_centered = 0x0080,          // bit to set if the cursor should be set to the center of full screen windows
		framebuffer_is_transparent = 0x0100,  // bit to set if the framebuffer should be transparent (if supported)
		should_scale_to_monitor = 0x0200, // bit to set if the window should not be scaled to whatever monitor it is placed on
		defaults = 0,
	};
}

SGL_END