#include "INodePort.h"
#include "INode.h"

uint32_t INodePort::get_id() const
{
    return id;
}

Val_t INodePort::get_type() const
{
    return type;
}

IO_t INodePort::get_io_type() const
{
    return io_type;
}

std::weak_ptr<INode> INodePort::get_parent() const
{
    return parent;
}

void INodePort::set_parent(const std::weak_ptr<INode>& parent)
{
    (this)->parent = parent;
}

void INodePort::set_name(const std::string& name)
{
    (this)->name = name;
};

std::string INodePort::get_name() const
{
    return name;
};

std::any INodePort::get_value() const
{
    return value;
}

void INodePort::set_index(const uint32_t index)
{
    (this)->index = index;
}

uint32_t INodePort::get_index() const
{
    return index;
}

void INodePort::enable()
{
    logger.log(debug, "Port(" + std::to_string(id) + ") : " + name + " is enabled");
    enabled = true;
}

void INodePort::disable()
{
    logger.log(debug, "Port(" + std::to_string(id) + ") : " + name + " is disabled");
    enabled = false;
}

bool INodePort::is_altered()
{
    if (altered) {
        altered = false;
        return true;
    }
    return false;
}

bool INodePort::is_enabled() const
{
    return enabled;
}

uint32_t INodePort::get_color() const
{
    return color;
}

uint32_t INodePort::get_color_select() const
{
    return color_select;
}

uint32_t INodePort::get_color_hover() const
{
    return color_hover;
}

void INodePort::add_link(const std::shared_ptr<PortLink> link)
{
    link_map[link->get_id()] = link;
    logger.log(debug, "Port("+std::to_string(id)+") : " + name + " added Link(" + std::to_string(link->get_id())+")");
    altered = true;
}

void INodePort::remove_link(const uint32_t link_id)
{
    link_map.erase(link_id);
    logger.log(debug, "Port(" + std::to_string(id) + ") : " + name + " removed Link(" + std::to_string(link_id) + ")");
    altered = true;
}

std::map<uint32_t, std::weak_ptr<PortLink>> INodePort::get_links() const
{
    return link_map;
}

void INodePort::render()
{
    const float label_width = ImGui::CalcTextSize(name.c_str()).x;

    switch (io_type) {
    case Input:
        ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
        ImNodes::PushColorStyle(ImNodesCol_Pin, color);
        ImNodes::PushColorStyle(ImNodesCol_PinHovered, color_hover);
            ImNodes::BeginInputAttribute(id);
                ImGui::TextUnformatted(name.c_str());
                ImGui::SameLine();
                if (io_type == Input && link_map.size() == 0 || parent.lock()->is_root()) render_impl();
            ImNodes::EndInputAttribute();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopAttributeFlag();
        break;
    case Output:
        ImNodes::PushColorStyle(ImNodesCol_Pin, color);
        ImNodes::PushColorStyle(ImNodesCol_PinHovered, color_hover);
            ImNodes::BeginOutputAttribute(id);
                ImGui::TextUnformatted(name.c_str());
            ImNodes::EndOutputAttribute();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        break;
    }

}

void INodePort::init_colors()
{
    ImColor col_tmp;
    ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, col_tmp.Value.x, col_tmp.Value.y, col_tmp.Value.z);
    ImGui::ColorConvertHSVtoRGB(col_tmp.Value.x, col_tmp.Value.y, col_tmp.Value.z - 0.2f, color_hover.Value.x, color_hover.Value.y, color_hover.Value.z);
    color_hover.Value.w = 1.f;

    ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, col_tmp.Value.x, col_tmp.Value.y, col_tmp.Value.z);
    ImGui::ColorConvertHSVtoRGB(col_tmp.Value.x, col_tmp.Value.y, col_tmp.Value.z + 0.2f, color_select.Value.x, color_select.Value.y, color_select.Value.z);
    color_select.Value.w = 1.f;
}
