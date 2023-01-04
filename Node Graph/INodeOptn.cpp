#include "INodeOptn.h"

uint32_t INodeOptn::get_id() const
{
    return id;
}

Val_t INodeOptn::get_type() const
{
    return type;
}

std::string INodeOptn::get_name() const
{
    return name;
}

std::any INodeOptn::get_value() const
{
    return value;
}

void INodeOptn::render()
{
    ImGui::TextUnformatted(name.c_str());
    ImGui::SameLine();
    render_impl();
}
