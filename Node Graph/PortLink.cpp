#include "PortLink.h"

PortLink::~PortLink()
{
    u->remove_link(id);
    v->remove_link(id);
    logger.log(debug, "PortLink(" + std::to_string(id) + ") was destroyed.");
}

std::shared_ptr<PortLink> PortLink::get_ptr()
{
    return shared_from_this();
}

std::shared_ptr<PortLink> PortLink::create(const std::shared_ptr<INodePort>& u, const std::shared_ptr<INodePort>& v)
{
    auto new_portlink = std::shared_ptr<PortLink>(new PortLink(u, v));
    new_portlink->build();
    return new_portlink;
}

uint32_t PortLink::get_id() const
{
    return id;
}

std::shared_ptr<INodePort> PortLink::get_u() const
{
    return u;
}

std::shared_ptr<INodePort> PortLink::get_v() const
{
    return v;
}

void PortLink::set_u(const std::shared_ptr<INodePort>& u)
{
    (this)->u->remove_link(id);
    (this)->u = u;
    logger.log(debug, "PortLink(" + std::to_string(id) + ") u was set to Port(" + std::to_string(u->get_id()) + "): " + u->get_name() + ".");
    (this)->u->add_link(get_ptr());
}

void PortLink::set_v(const std::shared_ptr<INodePort>& v)
{
    (this)->v->remove_link(id);
    (this)->v = v;
    logger.log(debug, "PortLink(" + std::to_string(id) + ") v was set to Port(" + std::to_string(u->get_id()) + "): " + u->get_name() + ".");
    (this)->v->add_link(get_ptr());
}

void PortLink::render() const
{
    if (u->is_enabled() && v->is_enabled()) {
        ImNodes::PushColorStyle(ImNodesCol_Link, u->get_color());
        ImNodes::PushColorStyle(ImNodesCol_LinkHovered, u->get_color_hover());
        ImNodes::PushColorStyle(ImNodesCol_LinkSelected, u->get_color_select());
        ImNodes::Link(id, u->get_id(), v->get_id());
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }
}

void PortLink::build()
{
    u->add_link(shared_from_this());
    v->add_link(shared_from_this());
};