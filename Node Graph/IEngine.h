#pragma once
#include <queue>
#include <map>
#include <memory>
#include <functional>
#include <stack>

#include "INode.h"

typedef std::pair<Val_t, Val_t> type_pair_t;
typedef std::function<std::any(std::any)> lambda_t;

typedef std::pair<uint32_t, uint32_t> Edge;

struct Vertex {
	uint32_t id;
    std::map<Edge, std::shared_ptr<Vertex>> adj;
	Vertex(const uint32_t id) : id(id) {};
};

class IEngine
{
public:
    IEngine() {};

    bool is_conversion(const Val_t from, const Val_t to);
    std::any convert(const Val_t from, const Val_t to, const std::any& value);
    void eval(std::map<uint32_t, std::shared_ptr<INode>>& node_map);

protected:
    template<typename T>
    void add_conversion(const Val_t from, const Val_t to, T lambda)
    {
        transform_map.emplace(type_pair_t(from, to), lambda);
    }
    std::shared_ptr<Vertex> root;
    bool altered = false;
private:
    virtual void build_conversion_map() = 0;
    virtual void pre_eval() = 0;
    virtual void post_eval() = 0;
    virtual void eval_node(const std::shared_ptr<INode>& node) = 0;

    void transfer_data(const std::shared_ptr<PortLink> link);
    void build_graph(const std::map<uint32_t, std::shared_ptr<INode>>& node_map);
    void add_vertex(const uint32_t id);
    void add_edge(const uint32_t u, const uint32_t v);

    std::stack<uint32_t> get_eval_ord();
    void topo_sort(const std::shared_ptr<Vertex>& vertex, std::map<uint32_t, bool>& visited, std::stack<uint32_t>& stack);
    std::stack<uint32_t> inverse_stack(std::stack<uint32_t>& stack);

    std::map<type_pair_t, lambda_t> transform_map;
    std::map<uint32_t, std::shared_ptr<Vertex>> vert_map;
};

