#include "ShaderNode.h"

namespace Shader
{
    GLSLCode Node::get_glsl_code() const
    {
        return glsl_code;
    }

    void Node::construct_decl_strings()
    {
        auto blueprint = bp_map[sel_bp];
        auto port_keys = blueprint.get_port_keys();
        auto port_map = blueprint.get_port_map();
        uniform_ports.clear();
        for (const auto& p_key : port_keys)
        {
            if (auto port = std::dynamic_pointer_cast<Port>(port_map[p_key]))
            {
                auto is_input = (port->get_io_type() == Input);
                auto is_connected = (port->get_links().size() != 0);

                auto var_type = get_glsl_type(port->get_type());
                auto var_name = get_value<std::string>(port->get_name());

                if (port->get_links().size() == 0) 
                    port->reset();

                if (is_input && !is_connected)
                {
                    glsl_code.unif_decl += "uniform " + var_type + " " + var_name + ";\n";
                    uniform_ports.push_back(port);
                }
                else if (!is_input && !is_input_node) {
                    glsl_code.locl_decl += "\t" + var_type + " " + var_name + ";\n";
                }
            }
            else {
                std::cout << "Warning: port " << port->get_name() << " failed to cast to ShaderPort. The ports in Nodes MUST be shader ports!" << std::endl;
            }
        }
    }

    std::vector<std::shared_ptr<Port>> Node::get_uniform_ports() const
    {
        return uniform_ports;
    }
}