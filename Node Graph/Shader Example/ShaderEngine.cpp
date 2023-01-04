#include "ShaderEngine.h"

namespace Shader {
    std::shared_ptr<GlslShader> Engine::get_shader() const
    {
        return shader;
    }
    std::vector<std::shared_ptr<Port>> Engine::get_uniforms() const
    {
        return uniforms;
    }
    void Engine::compile()
    {
        compile_code = true;
    }
    void Engine::build_conversion_map()
    {
        add_conversion(Bool, Int, [](std::any x) { return "int(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Bool, Float, [](std::any x) { return "float(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Bool, Vec2, [](std::any x) { return "vec2(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Bool, Vec3, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Bool, Vec4, [](std::any x) { return "vec4(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Bool, Color, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ")"; });

        add_conversion(Int, Bool, [](std::any x) { return "bool(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Int, Float, [](std::any x) { return "float(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Int, Vec2, [](std::any x) { return "vec2(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Int, Vec3, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Int, Vec4, [](std::any x) { return "vec4(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Int, Color, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ")"; });

        add_conversion(Float, Bool, [](std::any x) { return "bool(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Float, Int, [](std::any x) { return "int(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Float, Vec2, [](std::any x) { return "vec2(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Float, Vec3, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Float, Vec4, [](std::any x) { return "vec4(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Float, Color, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ")"; });

        add_conversion(Vec2, Bool, [](std::any x) { return "bool(lum(" + std::any_cast<std::string>(x) + "))"; });
        add_conversion(Vec2, Int, [](std::any x) { return "int(lum(" + std::any_cast<std::string>(x) + "))"; });
        add_conversion(Vec2, Float, [](std::any x) { return "lum(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Vec2, Vec3, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ", 0.f)"; });
        add_conversion(Vec2, Vec4, [](std::any x) { return "vec4(" + std::any_cast<std::string>(x) + ", 0.f, 0.f)"; });
        add_conversion(Vec2, Color, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ", 0.f)"; });

        add_conversion(Vec3, Bool, [](std::any x) { return "bool(lum(" + std::any_cast<std::string>(x) + "))"; });
        add_conversion(Vec3, Int, [](std::any x) { return "int(lum(" + std::any_cast<std::string>(x) + "))"; });
        add_conversion(Vec3, Float, [](std::any x) { return "lum(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Vec3, Vec2, [](std::any x) { return "vec2(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Vec3, Vec4, [](std::any x) { return "vec4(" + std::any_cast<std::string>(x) + ", 0.f)"; });
        add_conversion(Vec3, Color, [](std::any x) { return std::any_cast<std::string>(x); });

        add_conversion(Vec4, Bool, [](std::any x) { return "bool(lum(" + std::any_cast<std::string>(x) + "))"; });
        add_conversion(Vec4, Int, [](std::any x) { return "int(lum(" + std::any_cast<std::string>(x) + "))"; });
        add_conversion(Vec4, Float, [](std::any x) { return "lum(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Vec4, Vec2, [](std::any x) { return "vec2(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Vec4, Vec3, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Vec4, Color, [](std::any x) { return "vec3(" + std::any_cast<std::string>(x) + ")"; });

        add_conversion(Color, Bool, [](std::any x) { return "bool(lum(" + std::any_cast<std::string>(x) + "))"; });
        add_conversion(Color, Int, [](std::any x) { return "int(lum(" + std::any_cast<std::string>(x) + "))"; });
        add_conversion(Color, Float, [](std::any x) { return "lum(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Color, Vec2, [](std::any x) { return "vec2(" + std::any_cast<std::string>(x) + ")"; });
        add_conversion(Color, Vec4, [](std::any x) { return "vec4(" + std::any_cast<std::string>(x) + ", 0.f)"; });
        add_conversion(Color, Vec3, [](std::any x) { return std::any_cast<std::string>(x); });
    }

    void Engine::pre_eval()
    {
        shader_code.clear();
        uniforms.clear();
    }

    void Engine::post_eval()
    {
        if (root != nullptr) {
            std::string shdr_code = R"(
#version 330 core
out vec4 frag_color;
in vec2 texcoord;

)" + shader_code.unif_decl + glsl_functions + noies_functions + R"(
void main() {
)" + shader_code.locl_decl +
                "\n" +
                shader_code.logic +
                "\n}";

            shader->set_code(GL_FRAGMENT_SHADER, shdr_code);
        }

        if (altered) {
            shader->compile();
        }
    }

    void Engine::eval_node(const std::shared_ptr<INode>& node)
    {
        if (auto s_node = std::dynamic_pointer_cast<Node>(node)) {
            s_node->evaluate();
            shader_code += s_node->get_glsl_code();
            auto node_uni = s_node->get_uniform_ports();
            uniforms.insert(uniforms.end(), node_uni.begin(), node_uni.end());
        }
        else {
            std::cout << "Warning: node " << node->get_id() << " failed to cast to ShaderNode. The nodes in the ShaderEngine MUST be ShaderNodes!" << std::endl;
        }
    }
}
