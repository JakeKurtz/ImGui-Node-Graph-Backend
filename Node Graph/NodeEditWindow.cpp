#include "NodeEditWindow.h"

NodeEditWindow::NodeEditWindow(const std::shared_ptr<IEngine>& engine) : minimap_pos(ImNodesMiniMapLocation_BottomRight)
{
    ImNodes::CreateContext();

    // Setup style
    ImNodes::StyleColorsDark();

    ImNodesIO& io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;

    (this)->engine = engine;
}

void NodeEditWindow::draw()
{
    auto flags = ImGuiWindowFlags_MenuBar;

    ImGui::Begin("Node Editor", &p_open, flags);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Mini-map"))
        {
            const char* names[] = {
                "Top Left",
                "Top Right",
                "Bottom Left",
                "Bottom Right",
            };
            int locations[] = {
                ImNodesMiniMapLocation_TopLeft,
                ImNodesMiniMapLocation_TopRight,
                ImNodesMiniMapLocation_BottomLeft,
                ImNodesMiniMapLocation_BottomRight,
            };

            for (int i = 0; i < 4; i++)
            {
                bool selected = minimap_pos == locations[i];
                if (ImGui::MenuItem(names[i], NULL, &selected))
                    minimap_pos = locations[i];
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Style"))
        {
            if (ImGui::MenuItem("Classic"))
            {
                ImGui::StyleColorsClassic();
                ImNodes::StyleColorsClassic();
            }
            if (ImGui::MenuItem("Dark"))
            {
                ImGui::StyleColorsDark();
                ImNodes::StyleColorsDark();
            }
            if (ImGui::MenuItem("Light"))
            {
                ImGui::StyleColorsLight();
                ImNodes::StyleColorsLight();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::TextUnformatted("Edit the color of the output color window using nodes.");
    ImGui::Columns(2);
    ImGui::TextUnformatted("A -- add node");
    ImGui::TextUnformatted("X -- delete selected node or link");
    ImGui::NextColumn();
    ImGui::Columns(1);

    ImNodes::BeginNodeEditor();
        node_create_handler();
        render_nodes();
        render_links();
        ImNodes::MiniMap(0.2f, minimap_pos);
    ImNodes::EndNodeEditor();

    link_create_handler();
    link_delete_handler();
    node_delete_handler();

    ImGui::End();

    engine->eval(node_map);
}

void NodeEditWindow::menu_bar()
{
}

void NodeEditWindow::window_size_callback(GLFWwindow* window, int width, int height)
{
}

void NodeEditWindow::mouse_callback(GLFWwindow* window, double xpos, double ypo)
{
}

void NodeEditWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

void NodeEditWindow::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void NodeEditWindow::key_callback()
{
}

void NodeEditWindow::process_input(GLFWwindow* window)
{
}

void NodeEditWindow::render_nodes()
{
    for (const auto& node : node_map)
    {
        node.second->render();
    }
}

void NodeEditWindow::render_links()
{
    for (const auto& link : link_map)
    {
        link.second->render();
    }
}

void NodeEditWindow::node_create_handler()
{
    const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
        ImNodes::IsEditorHovered() &&
        ImGui::IsKeyReleased(65);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
    if (!ImGui::IsAnyItemHovered() && open_popup)
    {
        ImGui::OpenPopup("add node");
    }

    if (ImGui::BeginPopup("add node"))
    {
        const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
        for (const auto& nk : node_create_keys)
        {
            auto node_name = nk.c_str();
            if (ImGui::MenuItem(node_name))
            {
                auto node = node_create_dic[nk]();

                ImNodes::SetNodeScreenSpacePos(node->get_id(), click_pos);

                node_map[node->get_id()] = node;

                auto node_ports = node->get_ports();
                port_map.insert(node_ports.begin(), node_ports.end());
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void NodeEditWindow::node_delete_handler()
{
    int num_nodes_selected = ImNodes::NumSelectedNodes();
    if (num_nodes_selected > 0 && ImGui::IsKeyReleased(88))
    {
        static std::vector<int> selected_nodes;
        selected_nodes.resize(static_cast<size_t>(num_nodes_selected));
        ImNodes::GetSelectedNodes(selected_nodes.data());
        for (const int node_id : selected_nodes) 
        {
            auto it = node_map.find(node_id);
            if (it != node_map.end()) {
                const auto& node = (*it).second;
                for (const auto& p_pair : node->get_ports()) 
                {
                    const auto& port = p_pair.second;
                    for (const auto& l_pair : port->get_links()) 
                    {
                        link_map.erase(l_pair.first);
                    }
                    port_map.erase(port->get_id());
                }
                node_map.erase(node_id);
            }
        }
    }
}

void NodeEditWindow::link_create_handler()
{

    int port_id;
    if (ImNodes::IsLinkStarted(&port_id)) {
        dragging = true;
        start_port_id = port_id;
    }

    if (ImNodes::IsLinkDropped(&port_id)) {
        dragging = false;
        start_port_id = -1;
    }

    if (ImNodes::IsPinHovered(&port_id) && dragging) {
        end_port_id = port_id;
    } else {
        end_port_id = -1;
    }

    // TODO: lots of duplicate code here. Not only is this function huge, there's probably a better way to do this...
    if (start_port_id != -1 && end_port_id != -1)
    {
        const auto& start_port = port_map[start_port_id];
        const auto& end_port = port_map[end_port_id];

        std::shared_ptr<INodePort> u;
        std::shared_ptr<INodePort> v;

        if (start_port->get_io_type() != Output) {
            u = end_port; v = start_port;
        }
        else {
            u = start_port; v = end_port;
        }

        // TODO: refactor this. I'm being lazy as hell right now. Maybe replace temp with query? 
        valid_link = (u->get_type() == v->get_type() || engine->is_conversion(u->get_type(), v->get_type())) && (u->get_io_type() == Output && v->get_io_type() == Input) && (u->get_parent().lock()->get_id() != v->get_parent().lock()->get_id());

        if (valid_link) 
            ImNodes::PushColorStyle(ImNodesCol_Link, port_map[u->get_id()]->get_color_hover());
        else 
            ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(255, 0, 0, 255));
    }
    else if (start_port_id != -1) {
        const auto& start_port = port_map[start_port_id];
        ImNodes::PushColorStyle(ImNodesCol_Link, port_map[start_port->get_id()]->get_color_hover());
    }

    int start_attr, end_attr;
    if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
    {
        const auto& start_port = port_map[start_attr];
        const auto& end_port = port_map[end_attr];

        std::shared_ptr<INodePort> u;
        std::shared_ptr<INodePort> v;

        if (start_port->get_io_type() != Output) {
            u = end_port; v = start_port;
        }
        else {
            u = start_port; v = end_port;
        }

        valid_link = u->get_type() == v->get_type() || engine->is_conversion(u->get_type(), v->get_type());

        if (valid_link) {
            for (const auto& link : v->get_links())
            {
                link_map.erase(link.first);
            }
            auto link = PortLink::create(u, v); link_map[link->get_id()] = link;
        }

        start_port_id = -1;
        end_port_id = -1;
    }
}

void NodeEditWindow::link_delete_handler()
{
    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id))
    {
        const auto& link = link_map[link_id];
        ImNodes::PushColorStyle(ImNodesCol_Link, link->get_u()->get_color_hover());
        link_map.erase(link_id);
    }

    int num_links_selected = ImNodes::NumSelectedLinks();
    if (num_links_selected > 0 && ImGui::IsKeyReleased(88))
    {
        static std::vector<int> selected_links;
        selected_links.resize(static_cast<size_t>(num_links_selected));
        ImNodes::GetSelectedLinks(selected_links.data());
        for (const int link_id : selected_links)
        {
            link_map.erase(link_id);
        }
    }
}