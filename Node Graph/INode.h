#pragma once
#include "globals.h"

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <boost/type_index.hpp>
#include "NodeBluePrint.h"
#include "PortLink.h"

class INode : public std::enable_shared_from_this<INode>
{
public:
    INode() 
    {
        logger.log(debug, "Node(" + std::to_string(id) + ") : " + name + " was created");
    };
    ~INode()
    {
        logger.log(debug, "Node(" + std::to_string(id) + ") : " + name + " was destroyed");
    };

    std::shared_ptr<INode> get_ptr() 
    {
        return shared_from_this();
    }

    uint32_t get_id() const;

    std::map<uint32_t, std::shared_ptr<INodePort>> get_ports();

    int get_width() const;
    void set_width(const int width);

    bool is_root() const;
    bool is_input() const;

    bool is_altered();

    void render();

    void evaluate() { pre_evaluation(); evaluate_impl(); post_evaluation(); };

protected:
    uint32_t id = gen_id();
    std::string name;

    std::string sel_bp;
    std::string pre_bp;

    bool is_root_node = false;
    bool is_input_node = false;

    int width = 120;

    std::map<std::string, NodeBluePrint> bp_map;
    std::vector<std::string> bp_keys;

    std::map<uint32_t, std::shared_ptr<INodePort>> port_map;

    void add_blueprint(const NodeBluePrint& blueprint);

    template <typename T>
    T get_value(const std::string& val_key);

    std::shared_ptr<INodePort> get_port(const std::string& port_key);

    void set_value(const std::string& port_key, const std::any& value);

    void build();
private:
    void reconnect_links(const std::string& old_op, const std::string& new_op);

    virtual void build_impl() = 0;

    virtual void pre_evaluation() {};
    virtual void evaluate_impl() = 0;
    virtual void post_evaluation() {};

    void draw_title();
    void draw_blueprint_combo();
    void draw_optns();
    void draw_ports();
};

template<typename T>
inline T INode::get_value(const std::string& val_key)
{
    auto blueprint = bp_map[sel_bp];
    std::any value;
    if (blueprint.is_port(val_key)) {
        value = blueprint.get_port_map()[val_key]->get_value();
    } else if (blueprint.is_optn(val_key)) {
        value = blueprint.get_optn_map()[val_key]->get_value();
    } else {
        logger.log(info, "class: INode\tmethod: get_value");
        logger.log(warn, "Failed to get value. The value \"" + val_key + "\" does not exist.");
        logger.log(notice, "Returned T().");
        return T();
    }

    if (const T* _v = std::any_cast<T>(&value)) {
       return *_v;
    }
    else {
        logger.log(info, "class: INode\tmethod: get_value");
        logger.log(warn, "Any cast failed. The port \"" + val_key + "\" is not a " + boost::typeindex::type_id<T>().pretty_name() +".");
        logger.log(notice, "Returned T().");
        return T();
    }
}
