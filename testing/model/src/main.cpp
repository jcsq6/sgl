#include <sgl.h>
#include <object/shapes.h>
#include <object/camera.h>
#include <shaders/render_shader.h>
#include <utils/timer.h>
#include <utils/error.h>
#include <model/model.h>

#include <iostream>

#ifdef _WIN32
#define main WinMain
#endif

int main()
{
	sgl::window window(500, 500, "work");
	window.set_logical_size(500, 500);
	window.set_swap_interval(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	sgl::mat4 persp = sgl::perspective<float>(45.f, 1, 0.1f, 1000);
	sgl::set_projection(&persp);

	sgl::camera cam({ 0, 0, -1 }, { 0, 0, 0 });
	bool cam_changed = false;
	sgl::mat4 view = cam.view();
	sgl::set_view(&view);

	auto lighting_shader = sgl::phong_shader(0, 0, 1, sgl::variables::sgl_Material);

	sgl::lighting_engine engine;
	//engine.add_positional_light(sgl::positional_light({ .1, .1, .1 }, { 1, 1, 1 }, { .75, .75, .75 }, { 10, 10, 10 }, 1, .09f, .032f));
	//engine.add_directional_light(sgl::directional_light({ .1, .1, .1 }, { 1, 1, 1 }, { .75, .75, .75 }, { 1, 0, 0 }));
	engine.add_spotlight(sgl::spotlight({ .1, .1, .1 }, { 1, 1, 1 }, { .75, .75, .75 }, cam.get_dir(), cam.pos, sgl::radians(10.f), sgl::radians(40.f), 1, 0.09f, 0.032f));
	auto spotlight = engine.spotlights_end() - 1;

	// https://free3d.com/3d-model/toy-truck-481161.html
	auto model_data = sgl::get_model("truck.obj");
	sgl::model_type model_type(model_data);
	sgl::model_obj<true, false> model(model_type);
	for (auto &mesh : model.meshes)
		mesh.set_scale({ 20, 20, 20 });

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

		static constexpr float speed = 10;

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
		{
			view = cam.view();
			spotlight->position = cam.pos;
			spotlight->direction = -cam.get_dir();
		}

		window.clear({ 184.f / 255 , 184.f / 255, 184.f / 255, 1 });
		sgl::render_settings settings({ 0, 0, 0, 1 }, &lighting_shader, &engine, nullptr);
		window.draw(model, settings);

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