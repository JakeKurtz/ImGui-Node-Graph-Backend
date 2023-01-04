#pragma once
#include "globals.h"

#include <string>
#include <memory>
#include <any>
#include <iostream>
#include <imnodes.h>
#include <imgui.h>
#include "GLCommon.h"
#include <map>

class INode;
struct IPortLink;
class PortLink;

enum IO_t {
    Input, Output, GlobalOutput
};

enum Val_t {
    Bool,
    Int,
    Float,
    Vec2,
    Vec3,
    Vec4,
    Color,
    String,
    GLSLCode_t
};

class INodePort
{
public:
    INodePort() 
    {
        logger.log(debug, "Port(" + std::to_string(id) + ") was created");
    };
    ~INodePort()
    {
        logger.log(debug, "Port(" + std::to_string(id) + ") : " + name + " was destroyed");
    };

    uint32_t get_id() const;
    Val_t get_type() const;
    IO_t get_io_type() const;

    std::weak_ptr<INode> get_parent() const;
    void set_parent(const std::weak_ptr<INode>& parent);

    std::string get_name() const;
    void set_name(const std::string& name);

    std::any get_value() const;
    void set_value(const std::any value) { set_value_impl(value); };

    void set_index(const uint32_t index);
    uint32_t get_index() const;

    void enable();
    void disable();

    bool is_altered();
    bool is_enabled() const;

    uint32_t get_color() const;
    uint32_t get_color_hover() const;
    uint32_t get_color_select() const;

    void add_link(const std::shared_ptr<PortLink> link);
    void remove_link(const uint32_t link_id);

    std::map<uint32_t, std::weak_ptr<PortLink>> get_links() const;

    void render();

protected:
    uint32_t id = gen_id();
    std::string name;
    Val_t type;
    IO_t io_type;
    std::any value;

    std::weak_ptr<INode> parent;

    uint32_t index;

    bool enabled = false;
    bool altered = false;

    uint32_t nmb_links;

    ImColor color = ImColor(255, 255, 0, 255);
    ImColor color_hover;
    ImColor color_select;

    float v_speed = 1.f;
    float v_min = 0.f;
    float v_max = 0.f;

    std::map<uint32_t, std::weak_ptr<PortLink>> link_map;

    void init_colors();

private:
    virtual void render_impl() = 0;
    virtual void set_value_impl(const std::any value) = 0;
};
