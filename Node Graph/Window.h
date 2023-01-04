#pragma once
#include <imgui.h>
#include "GLCommon.h"
#include <string>

class Window
{
public:
	Window() {};

	virtual void draw() = 0;
	virtual void menu_bar() = 0;

	virtual void window_size_callback(GLFWwindow* window, int width, int height) = 0;
	virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos) = 0;
	virtual void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) = 0;
	virtual void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) = 0;
	virtual void key_callback() = 0;
	virtual void process_input(GLFWwindow* window) = 0;

	bool focused() { return currently_focused; };

protected:
	ImGuiWindowFlags window_flags = 0;

	std::string name;

	//int width = 1;
	//int height = 1;

	bool p_open = false;

	ImVec2 window_size;
	ImVec2 window_min_pos;
	ImVec2 window_max_pos;
	ImVec2 window_center;

	float delta_time = 0.f;
	float last_frame_time = 0.f;
	float current_frame_time = 0.f;

	float image_scale = 1.f;
	float image_pan_x = 0;
	float image_pan_y = 0;

	float last_x = 0.f;
	float last_y = 0.f;

	float raw_mouse_scroll = 1;

	bool first_mouse = true;

	bool visible = true;

	bool mouse_button_l_down = false;
	bool mouse_button_r_down = false;
	bool mouse_button_3_down = false;
	bool mouse_drag = false;
	bool mouse_down = false;

	bool shift_down = false;
	bool currently_focused = false;

	void frame_time();
	void update_window_properties();
	void update_context_interaction_state();
};

