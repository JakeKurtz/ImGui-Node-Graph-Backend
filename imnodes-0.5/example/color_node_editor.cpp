/*
#include "node_editor.h"
//#include "graph.h"

#include <imnodes.h>
#include <imgui.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <vector>

#include "../../CUDA-RayTracer/GraphNode.h"

#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/directed_graph.hpp>
#include <queue>

namespace example
{
namespace
{

template<class T>
T clamp(T x, T a, T b)
{
    return std::min(b, std::max(x, a));
}

static float current_time_seconds = 0.f;
static bool  emulate_three_button_mouse = false;

 // create a typedef for the Graph type

typedef boost::adjacency_list<
    boost::listS,
    boost::listS,
    boost::bidirectionalS,
    std::shared_ptr<GraphVertex>,
    std::shared_ptr<GraphEdge>>
    Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor vertex_desc;
typedef boost::graph_traits<Graph>::edge_descriptor   edge_desc;

//typedef boost::property_map<Graph, boost::vertex_color_t>::type color_map_t;

typedef std::pair<std::size_t, std::shared_ptr<GraphEdge>> edge_pair;
typedef std::pair<std::size_t, vertex_desc>                    vertex_pair;
typedef std::pair<std::size_t, std::shared_ptr<GraphVertexUI>> ui_node_pair;

class dfs_visitor : public boost::default_dfs_visitor
{
public:
    dfs_visitor() : vv(new std::stack<std::shared_ptr<GraphVertex>>()) {}

    void discover_vertex(vertex_desc v, const Graph& g)
    {
        vv->push(g[v]);
    }
    std::stack<std::shared_ptr<GraphVertex>>& GetVector() const { return *vv; }

private:
    boost::shared_ptr<std::stack<std::shared_ptr<GraphVertex>>> vv;
};

class ColorNodeEditor
{
public:
    ColorNodeEditor() : g(), root_node_id_(-1),
        minimap_location_(ImNodesMiniMapLocation_BottomRight) { }
    
    ImU32 evaluate(const int root_node)
    {
        // INDEX MAP CREATION //
        std::map<vertex_desc, size_t> i_map;
        for (auto v : boost::make_iterator_range(vertices(g)))
        {
            i_map.emplace(v, i_map.size());
        }

        auto ipmap = boost::make_assoc_property_map(i_map);
        auto color_map = boost::make_vector_property_map<boost::default_color_type>(ipmap);
        
        dfs_visitor vis;
        boost::depth_first_visit(g, vertex_map[root_node], vis, color_map);
        
        std::stack<std::shared_ptr<GraphVertex>> post_order = vis.GetVector();
        std::stack<std::shared_ptr<GraphVertex>> value_stack;

        while (!post_order.empty())
        {
            auto node = post_order.top();
            post_order.pop();

            switch (node->type)
            {
            case GraphVertexType::Input:
            {
                // If the node has no out going edges, then it is not connected.
                // Just get its value
                if (node->nmb_out_edges == 0) 
                {
                    value_stack.push(node);
                }
                else
                {
                    auto vd = vertex_map[node->id];
                    for (const auto& ei : make_iterator_range(boost::out_edges(vd, g)))
                    {
                        const auto& edge = g[ei];
                        unsigned int u = edge->u;
                        unsigned int v = edge->v;

                        value_stack.push(g[vertex_map[v]]);
                    }
                }
            }
            break;
            case GraphVertexType::Operator:
            {
                if (auto op = std::static_pointer_cast<GraphVertexUI>(node))
                {
                    // load input values from stack
                    std::vector<std::shared_ptr<ShaderProp>> inputs;
                    for (int i = 0; i < op->get_input_attributes().size(); i++)
                    {
                        if (auto input = std::static_pointer_cast<ShaderProp>(value_stack.top()))
                        {
                            inputs.push_back(input);
                            value_stack.pop();
                        }
                    }
                    op->evaluate(inputs);
                }
            }
            break;
            case GraphVertexType::Output:
            {

            }
            default:
                break;
            }
        }
        
        glm::vec3 out_color = glm::vec3(0.f);

        auto vd = vertex_map[root_node];
        for (const auto& ei : make_iterator_range(boost::out_edges(vd, g)))
        {
            const auto&  edge = g[ei];
            unsigned int u = edge->u;
            unsigned int v = edge->v;

            if (auto out = std::static_pointer_cast<ShaderProp>(g[vertex_map[u]]))
            {
                out_color = out->get_color();
            }
        }

        return IM_COL32(out_color.x * 255, out_color.y * 255, out_color.z * 255, 255);
    }
    
    void add_ui_node(std::shared_ptr<GraphVertexUI> node)
    {
        ui_nodes.insert(ui_node_pair(node->id, node));

        auto node_vd = boost::add_vertex(node, g);
        vertex_map.insert(vertex_pair(node->id, node_vd));

        for (auto i : node->get_input_attributes())
        {
            auto input_vd = boost::add_vertex(i, g);
            vertex_map.insert(vertex_pair(i->get_id(), input_vd));

            auto foo = std::make_shared<GraphEdge>(i->get_id(), node->get_id(), GraphEdgeType::Interal);
            boost::add_edge(node_vd, input_vd, foo, g);

            g[node_vd]->nmb_out_edges++;
            g[input_vd]->nmb_in_edges++;
        }

        for (auto i : node->get_out_attributes())
        {
            auto output_vd = boost::add_vertex(i, g);
            vertex_map.insert(vertex_pair(i->get_id(), output_vd));

            auto foo = std::make_shared<GraphEdge>(node->get_id(), i->get_id(), GraphEdgeType::Interal);
            boost::add_edge(output_vd, node_vd, foo, g);

            g[node_vd]->nmb_in_edges++;
            g[output_vd]->nmb_out_edges++;
        }
    };

    void show()
    {
        // Update timer context
        current_time_seconds = 0.001f;// * SDL_GetTicks();

        auto flags = ImGuiWindowFlags_MenuBar;

        // The node editor window
        ImGui::Begin("color node editor", NULL, flags);

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
                    bool selected = minimap_location_ == locations[i];
                    if (ImGui::MenuItem(names[i], NULL, &selected))
                        minimap_location_ = locations[i];
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
        if (ImGui::Checkbox("emulate_three_button_mouse", &emulate_three_button_mouse))
        {
            ImNodes::GetIO().EmulateThreeButtonMouse.Modifier =
                emulate_three_button_mouse ? &ImGui::GetIO().KeyAlt : NULL;
        }
        ImGui::Columns(1);

        ImNodes::BeginNodeEditor();

        // Handle new nodes
        // These are driven by the user, so we place this code before rendering the nodes
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

                if (ImGui::MenuItem("Output"))
                {
                    auto output_node = std::make_shared<SurfaceOutput>(click_pos);
                    add_ui_node(output_node);
                    ui_output_nodes.push_back(output_node);
                }

                if (ImGui::MenuItem("Test"))
                {
                    auto test_node = std::make_shared<TestNode>(click_pos);
                    add_ui_node(test_node);
                }

                if (ImGui::MenuItem("Math"))
                {
                    auto math_node = std::make_shared<MathNode>(click_pos);
                    add_ui_node(math_node);
                }

                if (ImGui::MenuItem("Color Mix"))
                {
                    auto node = std::make_shared<MixColorNode>(click_pos);
                    add_ui_node(node);
                }

                if (ImGui::MenuItem("Color Combine"))
                {
                    auto node = std::make_shared<ColorCombineNode>(click_pos);
                    add_ui_node(node);
                }

                if (ImGui::MenuItem("Color Split"))
                {
                    auto node = std::make_shared<ColorSplitNode>(click_pos);
                    add_ui_node(node);
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();
        }

        for (auto node : ui_nodes)
        {
            node.second->render();
        }

        boost::graph_traits<Graph>::edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
        {
            const auto& edge = g[*ei];
            unsigned int u = edge->u;
            unsigned int v = edge->v;

            if (edge->type == GraphEdgeType::Interal) 
                continue;

            ImNodes::Link(edge->id, u, v);
        }

        ImNodes::MiniMap(0.2f, minimap_location_);
        ImNodes::EndNodeEditor();

        // Handle new links
        // These are driven by Imnodes, so we place the code after EndNodeEditor().
        {
            int start_attr, end_attr;
            if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
            {
                auto u_vd = vertex_map[start_attr];
                auto v_vd = vertex_map[end_attr]; 

                auto u = g[u_vd]; 
                auto v = g[v_vd]; 

                const GraphVertexType start_type = u->type;
                const GraphVertexType end_type = v->type;

                const bool valid_link = start_type != end_type;
                if (valid_link)
                {
                    // Ensure the edge is always directed from the value to
                    // whatever produces the value
                    if (start_type != GraphVertexType::Input)
                    {
                        std::swap(start_attr, end_attr);

                        u_vd = vertex_map[start_attr];
                        v_vd = vertex_map[end_attr];

                        u = g[u_vd];
                        v = g[v_vd]; 
                    }

                    auto nmb_out_edges = boost::out_degree(u_vd, g);
                    if (nmb_out_edges >= 1) // kill old edges
                    {
                        for (const auto& ei : make_iterator_range(boost::out_edges(u_vd, g)))
                        {
                            const auto& edge = g[ei];
                            edge_murder_q.push(g[ei]->id);
                        }
                    }
                    auto foo = std::make_shared<GraphEdge>(start_attr, end_attr, GraphEdgeType::External);
                    boost::add_edge(u_vd, v_vd, foo, g);
                    edge_map.insert(edge_pair(foo->id, foo));

                    u->nmb_out_edges++;
                    v->nmb_in_edges++;
                }
            }
        }

        // Handle deleted links
        {
            int link_id;
            if (ImNodes::IsLinkDestroyed(&link_id))
            {
                edge_murder_q.push(link_id);
            }
        }

        {
            const int num_selected = ImNodes::NumSelectedLinks();
            if (num_selected > 0 && ImGui::IsKeyReleased(88))
            {
                static std::vector<int> selected_links;
                selected_links.resize(static_cast<size_t>(num_selected));
                ImNodes::GetSelectedLinks(selected_links.data());
                for (const int edge_id : selected_links)
                {
                    edge_murder_q.push(edge_id);
                }
            }
        }

        {
            const int num_selected = ImNodes::NumSelectedNodes();
            if (num_selected > 0 && ImGui::IsKeyReleased(88))
            {
                static std::vector<int> selected_nodes;
                selected_nodes.resize(static_cast<size_t>(num_selected));
                ImNodes::GetSelectedNodes(selected_nodes.data());
                for (const int v_id : selected_nodes)
                {
                    auto vd_i = vertex_map[v_id];
                    for (const auto& ei : make_iterator_range(boost::out_edges(vd_i, g)))
                    {
                        vertex_murder_q.push(g[ei]->u);
                        edge_murder_q.push(g[ei]->id);

                        auto vd_j = vertex_map[v_id];
                        for (const auto& ej : make_iterator_range(boost::out_edges(vd_j, g)))
                            edge_murder_q.push(g[ej]->id);

                    }

                    for (const auto& ei : make_iterator_range(boost::in_edges(vd_i, g)))
                    {
                        vertex_murder_q.push(g[ei]->v);
                        edge_murder_q.push(g[ei]->id);

                        auto vd_j = vertex_map[v_id];
                        for (const auto& ej : make_iterator_range(boost::in_edges(vd_j, g)))
                            edge_murder_q.push(g[ej]->id);
                    }
                    
                    vertex_murder_q.push(v_id);
                    ui_nodes.erase(v_id);
                }
            }
        }
        
        ImGui::End();

        while (!edge_murder_q.empty())
        {
            int e_id = edge_murder_q.front();
            if (edge_map.find(e_id) != edge_map.end())
            {
                const auto& edge = edge_map[e_id];

                auto u_vd = vertex_map[edge->u];
                auto v_vd = vertex_map[edge->v];

                auto u = g[u_vd];
                auto v = g[v_vd]; 

                boost::remove_edge(u_vd, v_vd, g);

                u->nmb_out_edges--;
                v->nmb_in_edges--;
            }
            edge_map.erase(e_id);
            edge_murder_q.pop();
        }

        while (!vertex_murder_q.empty())
        {
            int v_id = vertex_murder_q.front();
            if (vertex_map.find(v_id) != vertex_map.end())
            {
                auto vd = vertex_map[v_id];
                boost::clear_vertex(vd, g);
                boost::remove_vertex(vd, g);
            }
            vertex_map.erase(v_id);
            vertex_murder_q.pop();
        }

        // The color output window
        
        if (ui_output_nodes.size() != 0)
        {
            root_node_id_ = ui_output_nodes[0]->id;
        }
        else
        {
            root_node_id_ = -1;
        }
        
        const ImU32 color = root_node_id_ != -1 ? evaluate(root_node_id_) : IM_COL32(255, 20, 147, 255);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, color);
        ImGui::Begin("output color");
        ImGui::End();
        ImGui::PopStyleColor();
    }

private:
    int                    root_node_id_;
    ImNodesMiniMapLocation minimap_location_;
    Graph g;

    std::unordered_map<std::size_t, std::shared_ptr<GraphEdge>> edge_map;
    std::unordered_map<std::size_t, vertex_desc> vertex_map;

    std::unordered_map<std::size_t, std::shared_ptr<GraphVertexUI>> ui_nodes;
    std::vector<std::shared_ptr<GraphVertexUI>> ui_output_nodes;
    std::queue<std::size_t> edge_murder_q;
    std::queue<std::size_t> vertex_murder_q;
};

static ColorNodeEditor color_editor;
} // namespace

void NodeEditorInitialize()
{
    ImNodesIO& io = ImNodes::GetIO();
    io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
}

void NodeEditorShow() { color_editor.show(); }

void NodeEditorShutdown() {}
} // namespace example
*/