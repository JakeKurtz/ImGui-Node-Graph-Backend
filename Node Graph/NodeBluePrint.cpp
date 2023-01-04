#include "NodeBluePrint.h"

void NodeBluePrint::enable()
{
    logger.log(debug, "NodeBluePrint: " + name + " is enabled");
    for (const auto p : port_map)
        p.second->enable();
};
void NodeBluePrint::disable()
{
    logger.log(debug, "NodeBluePrint: " + name + " is disabled");
    for (const auto p : port_map)
        p.second->disable();
};

void NodeBluePrint::add_port(const std::shared_ptr<INodePort>& port)
{
    auto port_name = port->get_name();
    if (port_map.find(port_name) == port_map.end()) 
    {
        switch (port->get_io_type()) 
        {
        case Input:
            port->set_index(in_cnt);
            in_cnt++;
            break;
        case Output:
            port->set_index(out_cnt);
            out_cnt++;
            break;
        }

        port->set_parent(parent);

        port_map[port_name] = port;
        port_keys.push_back(port_name);
    } 
    else 
    {
        logger.log(warn, "Failed to add port to the NodeBluePrint "+ name +". A port with the name \"" + port_name + "\" is already in the blueprint.");
    }
}

void NodeBluePrint::add_optn(const std::shared_ptr<INodeOptn>& optn)
{
    auto optn_name = optn->get_name();
    if (optn_map.find(optn_name) == optn_map.end())
    {
        optn_map[optn_name] = optn;
        optn_keys.push_back(optn_name);
    }
    else
    {
        logger.log(warn, "Failed to add option to the NodeBluePrint " + name + ". A option with the name \"" + optn_name + "\" is already in the blueprint.");
    }
}

std::string NodeBluePrint::get_name() const
{
    return name;
}

std::map<std::string, std::shared_ptr<INodePort>> NodeBluePrint::get_port_map() const
{
    return port_map;
}

std::vector<std::string> NodeBluePrint::get_port_keys() const
{
    return port_keys;
}

std::map<std::string, std::shared_ptr<INodeOptn>> NodeBluePrint::get_optn_map() const
{
    return optn_map;
}

std::vector<std::string> NodeBluePrint::get_optn_keys() const
{
    return optn_keys;
}

bool NodeBluePrint::is_port(const std::string& name)
{
    return !(port_map.find(name) == port_map.end());;
}

bool NodeBluePrint::is_optn(const std::string& name)
{
    return !(optn_map.find(name) == optn_map.end());
}

bool NodeBluePrint::name_exists(const std::string& name)
{
    bool in_optns = !(optn_map.find(name) == optn_map.end());
    bool in_ports = !(port_map.find(name) == port_map.end());

    return in_optns || in_ports;
}
