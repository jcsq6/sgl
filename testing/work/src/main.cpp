#include <sgl.h>
#include <object/shapes.h>
#include <object/camera.h>
#include <shaders/shaders.h>
#include <utils/timer.h>
#include <object/sprite.h>
#include <object/text.h>
#include <iostream>

#ifdef _WIN32
#define main WinMain
#endif

void draw_grid(sgl::render_target &target, sgl::vec2 min, sgl::vec2 max, float grid_size);

int main()
{
	sgl::window window(500, 500, "work");
	window.set_logical_size(500, 500);
	window.set_swap_interval(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	sgl::mat4 persp = sgl::perspective<float>(45.f, 1, 0.1f, 1000);
	sgl::set_projection(&persp);

	sgl::camera cam({ 0, 0, -1 }, { 0, 0, 0 });
	bool cam_changed = false;
	sgl::mat4 view = cam.view();
	sgl::set_view(&view);

	sgl::cube_obj<false> cube({}, { 2, 2, 2 });

	auto cursor_callback = [&cam_changed, &cam, &window](double x, double y)
	{
		static sgl::dvec2 old_pos(x, y);

		static auto left = window.get_mouse_button(sgl::mouse_code::left);
		if (left->is_pressed())
		{
			sgl::vec2 angle = (sgl::dvec2(x, y) - old_pos) / window.drawable_size() * sgl::pi<float>();

			cam.rotate_right(angle.x);
			cam.rotate_up(angle.y);

			cam_changed = true;
		}

		old_pos.x = x;
		old_pos.y = y;
	};

	window.set_cursor_callback(cursor_callback);

	auto framebuffer_callback = [&persp/*, &ortho */](int width, int height)
	{
		persp = sgl::perspective<float>(45.f, (float)width / height, 0.1f, 100);
		//ortho = sgl::ortho_mat(0, width, 0, height, -1, 1);
	};

	window.set_framebuffer_callback(framebuffer_callback);

	sgl::timer dt;

	while (!window.should_close())
	{
		dt.start();

		cam_changed = false;

		window.poll_events();

		if (window.get_key(sgl::key_code::escape)->is_pressed())
			window.set_should_close(true);

		static constexpr float speed = 8;

		if (window.get_key(sgl::key_code::d)->is_pressed())
		{
			cam.move_right((float)dt.seconds() * speed);
			cam_changed = true;
		}
		if (window.get_key(sgl::key_code::a)->is_pressed())
		{
			cam.move_right((float)dt.seconds() * -speed);
			cam_changed = true;
		}
		if (window.get_key(sgl::key_code::w)->is_pressed())
		{
			cam.move_forward((float)dt.seconds() * speed);
			cam_changed = true;
		}
		if (window.get_key(sgl::key_code::s)->is_pressed())
		{
			cam.move_forward((float)dt.seconds() * -speed);
			cam_changed = true;
		}
		if (window.get_key(sgl::key_code::space)->is_pressed())
		{
			cam.move_up((float)dt.seconds() * speed);
			cam_changed = true;
		}
		if (window.get_key(sgl::key_code::left_shift)->is_pressed() || window.get_key(sgl::key_code::right_shift)->is_pressed())
		{
			cam.move_up((float)dt.seconds() * -speed);
			cam_changed = true;
		}

		if (cam_changed)
			view = cam.view();

		window.clear({ 1, 1, 1, 1 });

		sgl::set_draw_color({ 0, 0, 0, .2 });
		draw_grid(window, { -10, -10 }, { 10, 10 }, 1);

		sgl::set_draw_color({ 1, 0, 0, 1 });
		window.draw(cube);

		window.swap_buffers();

		dt.stop();
	}
}

void draw_grid(sgl::render_target &target, sgl::vec2 min, sgl::vec2 max, float grid_size)
{
	for (float x = min.x; x <= max.x; x += grid_size)
	{
		sgl::line_obj<false, false> line(sgl::vec3(x, 0, min.y), sgl::vec3(x, 0, max.y));
		target.draw(line);
	}

	for (float y = min.y; y <= max.y; y += grid_size)
	{
		sgl::line_obj<false, false> line(sgl::vec3(min.x, 0, y), sgl::vec3(max.x, 0, y));
		target.draw(line);
	}
}