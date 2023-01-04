#pragma once
#include "Window.h"
#include <imnodes.h>
#include <vector>
#include <memory>
#include <functional>

#include "IEngine.h"

class NodeEditWindow : public Window
{
public:
	NodeEditWindow(const std::shared_ptr<IEngine>& engine);

	void draw();

	template<class t_Node>
	void add_node(const std::string& node_name)
	{
		node_create_dic.emplace(node_name, []() { return t_Node::create(); });
		node_create_keys.push_back(node_name);
	}

	void window_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypo);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void key_callback();
	void process_input(GLFWwindow* window);

private:

	typedef std::function<std::shared_ptr<INode>()> node_create;

	ImNodesMiniMapLocation minimap_pos;
	std::map<std::string, node_create> node_create_dic;
	std::vector<std::string> node_create_keys;

	std::map<uint32_t, std::shared_ptr<INode>> node_map;
	std::map<uint32_t, std::shared_ptr<INodePort>> port_map;
	std::map<uint32_t, std::shared_ptr<PortLink>> link_map;

	bool valid_link;
	bool dragging = false;

	int start_port_id = -1;
	int end_port_id = -1;

	std::shared_ptr<IEngine> engine;

	void menu_bar();

	void render_nodes();
	void render_links();

	void node_create_handler();
	void node_delete_handler();

	void link_create_handler();
	void link_delete_handler();
};
