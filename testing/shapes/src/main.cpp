#include <sgl.h>
#include <object/shapes.h>
#include <object/camera.h>
#include <utils/timer.h>

#ifdef _WIN32
#define main WinMain
#endif

void draw_grid(sgl::render_target &target, sgl::vec2 min, sgl::vec2 max, float grid_size);

int main()
{
	sgl::window window(500, 500, "shapes");
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

	sgl::mat4 ortho = sgl::ortho_mat(0, window.drawable_size().x, 0, window.drawable_size().y, -1, 1);

	sgl::cube_obj<true> cube({}, { 2, 2, 2 });
	cube.set_rot_axis({ 0, 1, 0 });
	cube.set_rot_origin({ 1, 1, 1 });

	sgl::rectangle_obj rect({ 3, 2, 0 }, { 2, 1 }, { 0, 0, 1 });

	sgl::point_obj<2> pt_2d({ 0, 3 }, .1f);
	sgl::point_obj<3> pt_3d({ 0, 4, 0 }, .1f);

	sgl::point_obj<3> line_beg({0, 0, 0}, .1f);
	sgl::point_obj<3> line_end{{1, 1, 1}, .1f};

	sgl::line_obj<false, false> line(line_beg.get_center(), line_end.get_center(), .1f);

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

	auto framebuffer_callback = [&persp, &ortho](int width, int height)
	{
		persp = sgl::perspective<float>(45.f, (float)width / height, 0.1f, 100);
		ortho = sgl::ortho_mat(0, width, 0, height, -1, 1);
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

		static constexpr float speed = 1;

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

		sgl::vec3 new_end = sgl::rot((float)dt.seconds() / 4, {0, 1, 0}) * sgl::vec4(line_end.get_center(), 1.0);
		line_end.set_center(new_end);
		line.set_endpoint(new_end);

		if (cam_changed)
			view = cam.view();

		cube.set_angle(cube.get_angle() + (float)dt.seconds() * sgl::pi<float>());

		window.clear({ 1, 1, 1, 1 });

		// draw_grid(window, { -10, -10 }, { 10, 10 }, 1);

		// window.draw(cube, { sgl::vec4(1, 0, 0, 1) });
		// window.draw(rect, { sgl::vec4(0, 1, 0, 1) });
		// window.draw(pt_2d, { sgl::vec4(1, 1, 0, 1) });
		// window.draw(pt_3d, { sgl::vec4(0, 1, 1, 1) });

		window.draw(line_beg, { sgl::vec4(1, 0, 0, 1) });
		window.draw(line_end, { sgl::vec4(1, 0, 0, 1) });
		window.draw(line, { sgl::vec4(0, 0, 0, 1) });

		window.swap_buffers();

		sgl::error err;
		while (sgl::get_error(err))
		{
			std::ofstream out("error.log", std::ofstream::app);
			out << err.message() << '\n';
			out << '\n';
		}

		dt.stop();
	}
}

void draw_grid(sgl::render_target &target, sgl::vec2 min, sgl::vec2 max, float grid_size)
{
	for (float x = min.x; x <= max.x; x += grid_size)
	{
		sgl::line_obj<false, false> line(sgl::vec3(x, 0, min.y), sgl::vec3(x, 0, max.y));
		target.draw(line, { sgl::vec4(0, 0, 0, .2) });
	}

	for (float y = min.y; y <= max.y; y += grid_size)
	{
		sgl::line_obj<false, false> line(sgl::vec3(min.x, 0, y), sgl::vec3(max.x, 0, y));
		target.draw(line, { sgl::vec4(0, 0, 0, .2) });
	}
}