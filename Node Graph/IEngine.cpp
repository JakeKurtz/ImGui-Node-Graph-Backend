#include "IEngine.h"

bool IEngine::is_conversion(const Val_t from, const Val_t to)
{
    return !(transform_map.find(type_pair_t(from, to)) == transform_map.end());
}

std::any IEngine::convert(const Val_t from, const Val_t to, const std::any& value)
{
    return transform_map[type_pair_t(from, to)](value);
}

void IEngine::eval(std::map<uint32_t, std::shared_ptr<INode>>& node_map)
{
    altered = false;
    pre_eval();
    build_graph(node_map);
    auto eval_ord = get_eval_ord();
    while (!eval_ord.empty())
    {
        auto n_id = eval_ord.top(); eval_ord.pop();
        const auto& node = node_map[n_id];
        if (node->is_altered()) altered = true;
        for (const auto& p_pair : node->get_ports())
        {
            auto& port = p_pair.second;
            if (port->is_altered()) altered = true;
            for (const auto& l_pair : port->get_links())
            {
                transfer_data(l_pair.second.lock());
            }
        }
        eval_node(node);
    }
    post_eval();
};

void IEngine::transfer_data(const std::shared_ptr<PortLink> link)
{
    const auto& u = link->get_u();
    const auto& v = link->get_v();

    if (u->get_type() == v->get_type()) {
        v->set_value(u->get_value());
    }
    else {
        v->set_value(convert(u->get_type(), v->get_type(), u->get_value()));
    }
};

void IEngine::build_graph(const std::map<uint32_t, std::shared_ptr<INode>>& node_map)
{
    root = nullptr;
    vert_map.clear();

    for (const auto& n_pair : node_map) {
        auto v_id = n_pair.first;
        auto node = n_pair.second;

        add_vertex(v_id);
    }

    for (const auto& n_pair : node_map) {
        auto node = n_pair.second;
        int nmb_links = 0;
        for (const auto& p_pair : node->get_ports())
        {
            auto port = p_pair.second;
            auto port_map = port->get_links();
            nmb_links += port_map.size();
            if (port_map.size() != 0)
            {
                auto link = (*port->get_links().begin()).second.lock();

                if (link->get_v()->get_id() == port->get_id())
                {
                    auto u = link->get_u()->get_parent().lock();
                    auto v = link->get_v()->get_parent().lock();
                    add_edge(v->get_id(), u->get_id());
                }
            }
        }
        if (node->is_root() && nmb_links != 0) {
            root = vert_map[node->get_id()];
        }
    }
};

void IEngine::add_vertex(const uint32_t id)
{
    auto itr = vert_map.find(id);
    if (itr == vert_map.end())
    {
        auto v = std::make_shared<Vertex>(id);
        vert_map[id] = v;
    }
    else {
        logger.log(info, "class: IEngine\tmethod: add_vertex");
        logger.log(warn, "Failed to add vertex("+std::to_string(id)+") to graph. It already exists.");
    }
}

void IEngine::add_edge(const uint32_t u, const uint32_t v)
{
    auto vert_u = (vert_map.find(u)->second);
    auto vert_v = (vert_map.find(v)->second);
    vert_u->adj[Edge(u, v)] = vert_v;
}

std::stack<uint32_t> IEngine::get_eval_ord() {
    if (root != nullptr)
    {
        std::stack<uint32_t> stack;
        std::map<uint32_t, bool> visited;

        topo_sort(root, visited, stack);

        return inverse_stack(stack);
    }
}

void IEngine::topo_sort(const std::shared_ptr<Vertex>& vertex, std::map<uint32_t, bool>& visited, std::stack<uint32_t>& stack)
{
    visited[vertex->id] = true;

    for (const auto& v_pair : vertex->adj)
    {
        const auto& v = v_pair.second;
        if (!visited[v->id])
        {
            topo_sort(v, visited, stack);
        }
    }
    stack.push(vertex->id);
};

std::stack<uint32_t> IEngine::inverse_stack(std::stack<uint32_t>& stack)
{
    std::stack<uint32_t> inv_stack;
    while (!stack.empty())
    {
        auto v = stack.top();
        stack.pop();
        inv_stack.push(v);
    }
    return inv_stack;
}
