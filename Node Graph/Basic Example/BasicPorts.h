#pragma once
#include "../INodePort.h"
#include <glm/glm.hpp>
#include <imgui.h>

namespace Basic {
    class Port : public INodePort
    {
    public:
        Port(const IO_t io, const std::string& name, const Val_t type, const ImColor color, const float v_speed, const float v_min, const float v_max)
        {
            this->name = name;
            this->io_type = io;
            this->color = color;
            init_colors();
        };
    };

    class BoolPort : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<BoolPort> create(const IO_t io, const std::string& name, const std::any& default_value = false, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            auto port = std::shared_ptr<BoolPort>(new BoolPort(io, name, v_speed, v_min, v_max));
            port->set_value(default_value);
            return port;
        };
    private:
        BoolPort(const IO_t io, const std::string& name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, Bool, IM_COL32(204, 255, 153, 255), v_speed, v_min, v_max) {}

        void set_value_impl(const std::any value)
        {
            if (const bool* v = std::any_cast<bool>(&value)) {
                (this)->value = value;
            }
            else {
                std::cerr << "Warning: Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };

        void render_impl()
        {
           ImGui::Checkbox(("##"+name).c_str(), (bool*)&value);
        };
    };

    class IntPort : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<IntPort> create(const IO_t io, const std::string& name, const std::any& default_value = 0, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            auto port = std::shared_ptr<IntPort>(new IntPort(io, name, v_speed, v_min, v_max));
            port->set_value(default_value);
            return port;
        };
    private:
        IntPort(const IO_t io, const std::string& name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, Int, IM_COL32(255, 255, 153, 255), v_speed, v_min, v_max) {}

        void set_value_impl(const std::any value)
        {
            if (const int* v = std::any_cast<int>(&value)) {
                (this)->value = value;
            }
            else {
                std::cerr << "Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };

        void render_impl()
        {
            ImGui::DragInt(("##"+name).c_str(), (int*)&value);
        };
    };

    class FloatPort : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<FloatPort> create(const IO_t io, const std::string& name, const std::any& default_value = 0.f, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            auto port = std::shared_ptr<FloatPort>(new FloatPort(io, name, v_speed, v_min, v_max));
            port->set_value(default_value);
            return port;
        };
    private:
        FloatPort(const IO_t io, const std::string& name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, Float, IM_COL32(255, 195, 77, 255), v_speed, v_min, v_max) {}

        void set_value_impl(const std::any value)
        {
            if (const float* v = std::any_cast<float>(&value)) {
                (this)->value = value;
            }
            else {
                std::cerr << "Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };

        void render_impl()
        {
            ImGui::DragFloat(("##"+name).c_str(), (float*)&value, v_speed, v_min, v_max);
        };
    };

    class Vec2Port : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<Vec2Port> create(const IO_t io, const std::string& name, const std::any& default_value = glm::vec2(0.f), const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            auto port = std::shared_ptr<Vec2Port>(new Vec2Port(io, name, v_speed, v_min, v_max));
            port->set_value(default_value);
            return port;
        };
    private:
        Vec2Port(const IO_t io, const std::string& name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, Vec2, IM_COL32(153, 204, 255, 255), v_speed, v_min, v_max) {}

        void set_value_impl(const std::any value)
        {
            if (const glm::vec2* v = std::any_cast<glm::vec2>(&value)) {
                (this)->value = value;
            }
            else {
                std::cerr << "Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };

        void render_impl()
        {
            ImGui::DragFloat2(("##"+name).c_str(), (float*)&value, v_speed, v_min, v_max);
        };
    };

    class Vec3Port : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<Vec3Port> create(const IO_t io, const std::string& name, const std::any& default_value = glm::vec3(0.f), const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            auto port = std::shared_ptr<Vec3Port>(new Vec3Port(io, name, v_speed, v_min, v_max));
            port->set_value(default_value);
            return port;
        };
    private:
        Vec3Port(const IO_t io, const std::string& name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, Vec3, IM_COL32(153, 153, 255, 255), v_speed, v_min, v_max) {}

        void set_value_impl(const std::any value)
        {
            if (const glm::vec3* v = std::any_cast<glm::vec3>(&value)) {
                (this)->value = value;
            }
            else {
                std::cerr << "Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };

        void render_impl() 
        {
            ImGui::DragFloat3(("##"+name).c_str(), (float*)&value, v_speed, v_min, v_max);
        };
    };

    class Vec4Port : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<Vec4Port> create(const IO_t io, const std::string& name, const std::any& default_value = glm::vec4(0.f), const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            auto port = std::shared_ptr<Vec4Port>(new Vec4Port(io, name, v_speed, v_min, v_max));
            port->set_value(default_value);
            return port;
        };
    private:
        Vec4Port(const IO_t io, const std::string& name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, Vec4, IM_COL32(153, 102, 255, 255), v_speed, v_min, v_max) {}

        void set_value_impl(const std::any value)
        {
            if (const glm::vec4* v = std::any_cast<glm::vec4>(&value)) {
                (this)->value = value;
            }
            else {
                std::cerr << "Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };

        void render_impl()
        {
            ImGui::DragFloat4(("##"+name).c_str(), (float*)&value);// , v_speed, v_min, v_max);
        };
    };

    class ColorPort : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<ColorPort> create(const IO_t io, const std::string& name, const std::any& default_value = glm::vec3(0.f), const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            auto port = std::shared_ptr<ColorPort>(new ColorPort(io, name, v_speed, v_min, v_max));
            port->set_value(default_value);
            return port;
        };
    private:
        ColorPort(const IO_t io, const std::string& name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, Color, IM_COL32(255, 102, 255, 255), v_speed, v_min, v_max) {}

        void set_value_impl(const std::any value)
        {
            if (const glm::vec3* v = std::any_cast<glm::vec3>(&value)) {
                (this)->value = value;
            }
            else {
                std::cerr << "Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };

        void render_impl()
        {
            ImGui::ColorEdit3(("##"+name).c_str(), (float*)&value, ImGuiColorEditFlags_NoInputs);
        };
    };
}