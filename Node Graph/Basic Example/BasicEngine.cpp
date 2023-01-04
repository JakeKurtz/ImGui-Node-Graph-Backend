#include "BasicEngine.h"
namespace Basic {
    void Engine::build_conversion_map()
    {
        add_conversion(Bool, Int, [](std::any x) { return (int)std::any_cast<bool>(x); });
        add_conversion(Bool, Float, [](std::any x) { return (float)std::any_cast<bool>(x); });
        add_conversion(Bool, Vec2, [](std::any x) { return glm::vec2(std::any_cast<bool>(x)); });
        add_conversion(Bool, Vec3, [](std::any x) { return glm::vec3(std::any_cast<bool>(x)); });
        add_conversion(Bool, Vec4, [](std::any x) { return glm::vec4(std::any_cast<bool>(x)); });
        add_conversion(Bool, Color, [](std::any x) { return glm::vec3(std::any_cast<bool>(x)); });

        add_conversion(Int, Bool, [](std::any x) { return (bool)std::any_cast<int>(x); });
        add_conversion(Int, Float, [](std::any x) { return (float)std::any_cast<int>(x); });
        add_conversion(Int, Vec2, [](std::any x) { return glm::vec2(std::any_cast<int>(x)); });
        add_conversion(Int, Vec3, [](std::any x) { return glm::vec3(std::any_cast<int>(x)); });
        add_conversion(Int, Vec4, [](std::any x) { return glm::vec4(std::any_cast<int>(x)); });
        add_conversion(Int, Color, [](std::any x) { return glm::vec3(std::any_cast<int>(x)); });

        add_conversion(Float, Bool, [](std::any x) { return (bool)std::any_cast<float>(x); });
        add_conversion(Float, Int, [](std::any x) { return (int)std::any_cast<float>(x); });
        add_conversion(Float, Vec2, [](std::any x) { return glm::vec2(std::any_cast<float>(x)); });
        add_conversion(Float, Vec3, [](std::any x) { return glm::vec3(std::any_cast<float>(x)); });
        add_conversion(Float, Vec4, [](std::any x) { return glm::vec4(std::any_cast<float>(x)); });
        add_conversion(Float, Color, [](std::any x) { return glm::vec3(std::any_cast<float>(x)); });

        add_conversion(Vec2, Bool, [](std::any x) { return (bool)std::any_cast<glm::vec2>(x).x; });
        add_conversion(Vec2, Int, [](std::any x) { return (int)std::any_cast<glm::vec2>(x).x; });
        add_conversion(Vec2, Float, [](std::any x) { return (float)std::any_cast<glm::vec2>(x).x; });
        add_conversion(Vec2, Vec3, [](std::any x) { return glm::vec3(std::any_cast<glm::vec2>(x), 0.f); });
        add_conversion(Vec2, Vec4, [](std::any x) { return glm::vec4(std::any_cast<glm::vec2>(x), 0.f, 0.f); });
        add_conversion(Vec2, Color, [](std::any x) { return glm::vec3(std::any_cast<glm::vec2>(x), 0.f); });

        add_conversion(Vec3, Bool, [](std::any x) { return (bool)std::any_cast<glm::vec3>(x).x; });
        add_conversion(Vec3, Int, [](std::any x) { return (int)std::any_cast<glm::vec3>(x).x; });
        add_conversion(Vec3, Float, [](std::any x) { return (float)std::any_cast<glm::vec3>(x).x; });
        add_conversion(Vec3, Vec2, [](std::any x) { return glm::vec2(std::any_cast<glm::vec3>(x)); });
        add_conversion(Vec3, Vec4, [](std::any x) { return glm::vec4(std::any_cast<glm::vec3>(x), 0.f); });
        add_conversion(Vec3, Color, [](std::any x) { return std::any_cast<glm::vec3>(x); });

        add_conversion(Vec4, Bool, [](std::any x) { return (bool)std::any_cast<glm::vec4>(x).x; });
        add_conversion(Vec4, Int, [](std::any x) { return (int)std::any_cast<glm::vec4>(x).x; });
        add_conversion(Vec4, Float, [](std::any x) { return (float)std::any_cast<glm::vec4>(x).x; });
        add_conversion(Vec4, Vec2, [](std::any x) { return glm::vec2(std::any_cast<glm::vec4>(x)); });
        add_conversion(Vec4, Vec3, [](std::any x) { return glm::vec3(std::any_cast<glm::vec4>(x)); });
        add_conversion(Vec4, Color, [](std::any x) { return std::any_cast<glm::vec4>(x); });

        add_conversion(Color, Bool, [](std::any x) { return (bool)std::any_cast<glm::vec3>(x).x; });
        add_conversion(Color, Int, [](std::any x) { return (int)std::any_cast<glm::vec3>(x).x; });
        add_conversion(Color, Float, [](std::any x) { return (float)std::any_cast<glm::vec3>(x).x; });
        add_conversion(Color, Vec2, [](std::any x) { return glm::vec2(std::any_cast<glm::vec3>(x)); });
        add_conversion(Color, Vec4, [](std::any x) { return glm::vec4(std::any_cast<glm::vec3>(x), 0.f); });
        add_conversion(Color, Vec3, [](std::any x) { return std::any_cast<glm::vec3>(x); });
    }

    void Engine::pre_eval()
    {
    }

    void Engine::post_eval()
    {
    }

    void Engine::eval_node(const std::shared_ptr<INode>& node)
    {
        node->evaluate();
    }
}