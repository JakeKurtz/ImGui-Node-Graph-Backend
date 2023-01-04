#include "INode.h"
#include <imgui.h>
#include <imnodes.h>

uint32_t INode::get_id() const
{
    return id;
};

void INode::add_blueprint(const NodeBluePrint& blueprint)
{
    if (bp_map.find(blueprint.get_name()) == bp_map.end()) 
    {
        for (const auto& p_pair : blueprint.get_port_map())
        {
            auto p = p_pair.second;
            auto p_id = p->get_id();
            port_map[p_id] = p;
        }

        bp_map[blueprint.get_name()] = blueprint;
        bp_keys.push_back(blueprint.get_name());

        if (bp_map.size() == 1) 
        {
            bp_map[blueprint.get_name()].enable();
            sel_bp = blueprint.get_name();
            pre_bp = sel_bp;
        }
    } 
    else
    {
        logger.log(info, "class: INode\tmethod: add_blueprint");
        logger.log(warn, "Failed to add blueprint. A blueprint with the name \"" + blueprint.get_name() + "\" has already been added to this node.");
    }
}

std::shared_ptr<INodePort> INode::get_port(const std::string& port_key)
{
    auto blueprint = bp_map[sel_bp];
    std::any value;
    if (blueprint.is_port(port_key)) {
        return blueprint.get_port_map()[port_key];
    }
    else {
        logger.log(info, "class: INode\tmethod: get_port");
        logger.log(warn, "Failed to get port. The port \"" + port_key + "\" does not exist.");
        logger.log(notice, "Returned a null pointer.");
        return nullptr;
    }
}

void INode::set_value(const std::string& port_key, const std::any& value)
{
    auto blueprint = bp_map[sel_bp];
    if (blueprint.is_port(port_key)) {
        auto port = blueprint.get_port_map()[port_key];
        if (port->get_io_type() == Output) {
            blueprint.get_port_map()[port_key]->set_value(value);
        } else {
            logger.log(info, "class: INode\tmethod: set_value");
            logger.log(warn, "Failed to set value. The value \"" + port_key + "\" is an input. Inputs can not be set.");
        }
    }
    else if (blueprint.is_optn(port_key)) {
        logger.log(info, "class: INode\tmethod: set_value");
        logger.log(warn, "Failed to set value. The value \"" + port_key + "\" is an option. Options can not be set.");
    } else {
        logger.log(info, "class: INode\tmethod: set_value");
        logger.log(warn, "Failed to set value. The value \"" + port_key + "\" does not exist.");
    }
}

void INode::build()
{
    build_impl();

    is_root_node = true;
    for (const auto& p_pair : port_map)
    {
        const auto& port = p_pair.second;
        if (port->get_io_type() == Output) is_root_node = false;
    }

    is_input_node = true;
    for (const auto& p_pair : port_map)
    {
        const auto& port = p_pair.second;
        if (port->get_io_type() == Input) is_input_node = false;
    }
}

int INode::get_width() const
{
    return width;
}

void INode::set_width(const int width)
{
    (this)->width = width;
}

bool INode::is_root() const
{
    return is_root_node;
}

bool INode::is_input() const
{
    return is_input_node;
}

bool INode::is_altered()
{
    if (sel_bp.compare(pre_bp)) {
        pre_bp = sel_bp;
        return true;
    }
    return false;
}

void INode::reconnect_links(const std::string& old_bp, const std::string& new_bp)
{
    bp_map[old_bp].disable();
    bp_map[new_bp].enable();

    auto ports = bp_map[new_bp].get_port_map();
    std::map<uint32_t, std::weak_ptr<PortLink>> links;

    // Gather all the links associated with our node.
    for (const auto& bp_pair : bp_map)
    {
        auto port_map = bp_pair.second.get_port_map();
        for (const auto& p_pair : port_map)
        {
            auto port = p_pair.second;
            for (const auto& l_pair : port->get_links())
            {
                links[port->get_id()] = l_pair.second;
            }
        }
    }

    // For each link, we try and find a port in our blueprint, such that, it has the following:
    //  1. same input type
    //  2. same value type
    //  3. same port index
    for (auto it = links.cbegin(), next_it = it; it != links.cend(); it = next_it)
    {
        ++next_it;
        const auto& link = (*it).second.lock();

        auto tgt_port = link->get_u();
        if (port_map.find(tgt_port->get_id()) == port_map.end())
        {
            tgt_port = link->get_v();
        }

        auto tgt_io = tgt_port->get_io_type();
        auto tgt_type = tgt_port->get_type();
        auto tgt_idx = tgt_port->get_index();

        for (auto jt = ports.cbegin(), next_jt = jt; jt != ports.cend(); jt = next_jt)
        {
            ++next_jt;
            const auto& src_port = (*jt).second;
            auto src_io = src_port->get_io_type();
            auto src_type = src_port->get_type();
            auto src_idx = src_port->get_index();

            if (tgt_io == src_io && tgt_type == src_type && tgt_idx == src_idx)
            {
                switch (tgt_io)
                {
                case Input:
                    link->set_v(src_port);
                    break;
                case Output:
                    for (const auto& l : tgt_port->get_links())
                        l.second.lock()->set_u(src_port);
                    break;
                }
                ports.erase(jt);
                break;
            }
        }
    }
}

std::map<uint32_t, std::shared_ptr<INodePort>> INode::get_ports()
{
    return port_map;
}

void INode::render()
{
    ImNodes::BeginNode(id);
        draw_title();
        draw_blueprint_combo();
        draw_optns();
        draw_ports();
    ImNodes::EndNode();
}

void INode::draw_title()
{
    ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(sel_bp.c_str());
    ImNodes::EndNodeTitleBar();
}

void INode::draw_blueprint_combo()
{
    if (bp_map.size() > 1) {
        const float label_width = ImGui::CalcTextSize(sel_bp.c_str()).x;
        ImGui::PushItemWidth(width - label_width);
        if (ImGui::BeginCombo("##blueprint combo", sel_bp.c_str()))
        {
            for (const auto& bp : bp_keys)
            {
                bool is_selected = (sel_bp == bp);
                if (ImGui::Selectable(bp.c_str(), is_selected)) {
                    reconnect_links(sel_bp, bp);
                    sel_bp = bp;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
    }
}

void INode::draw_optns()
{
    auto optn_keys = bp_map[sel_bp].get_optn_keys();
    auto optn_map = bp_map[sel_bp].get_optn_map();

    for (const auto& ok : optn_keys)
    {
        auto o = optn_map[ok];
        const float label_width = ImGui::CalcTextSize(o->get_name().c_str()).x;

        ImGui::PushItemWidth(width - label_width);
            o->render();
        ImGui::PopItemWidth();
    }
}

void INode::draw_ports()
{
    auto port_keys = bp_map[sel_bp].get_port_keys();
    auto port_map = bp_map[sel_bp].get_port_map();

    for (const auto& pk : port_keys)
    {
        auto p = port_map[pk];
        const float label_width = ImGui::CalcTextSize(p->get_name().c_str()).x;

        switch (p->get_io_type()) 
        {
        case Input:
            ImGui::PushItemWidth(width - label_width);
                p->render();
            ImGui::PopItemWidth();
            break;
        case Output:
            ImGui::Indent(width - label_width);
                p->render();
            ImGui::Unindent(width - label_width);
            break;
        }
    }
}
