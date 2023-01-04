#pragma once
#include "../INodePort.h"

#include <boost/algorithm/string.hpp>

namespace Shader 
{
    static std::string get_identifier(const std::string& base)
    {
        std::string out = base;
        boost::algorithm::to_lower(out);
        boost::replace_all(out, " ", "_");
        return out;
    }
    static bool valid_identifier(const std::string& str)
    {
        if (str.length() == 0)
            return false;

        if (!((str[0] >= 'a' && str[0] <= 'z') || (str[0] >= 'A' && str[0] <= 'Z') || str[0] == '_'))
            return false;

        for (int i = 1; i < str.length(); i++) {
            if (!((str[i] >= 'a' && str[i] <= 'z')
                || (str[i] >= 'A' && str[i] <= 'Z')
                || (str[i] >= '0' && str[i] <= '9')
                || str[i] == '_'))
                return false;
        }
    }

    class Port : public INodePort
    {
    public:
        Port(const IO_t io, const std::string& name, const std::string& variable_name, const ImColor color, const float v_speed, const float v_min, const float v_max)
        {
            (io == GlobalOutput) ? set_variable_name(variable_name) : set_variable_name(variable_name + "_" + std::to_string(this->id));

            this->name = name;
            this->io_type = (io == GlobalOutput) ? Output : io;
            this->color = color;
            init_colors();
        };

        void pass_to_shader(const uint32_t shader_id)
        {
            pass_to_shader_impl(shader_id);
        }
        void set_uniform(const std::any value) 
        {
            set_uniform_impl(value);
        };
        void reset() 
        {
            value = default_value;
        };

    protected:
        void set_variable_name(const std::any value)
        {
            if (const std::string* v = std::any_cast<std::string>(&value)) {
                if (true) {//(valid_identifier(*v)) {
                    (this)->value = value;
                    (this)->default_value = value;
                }
                else {
                    std::cerr << "Warning: \"" + name + "\" " << std::endl;
                }
            }
            else {
                std::cerr << "Warning: \"" + name + "\" " << std::endl;
            }
        };

    private:
        void set_value_impl(const std::any value)
        {
            if (const std::string* v = std::any_cast<std::string>(&value)) {
                if (true) {//(valid_identifier(*v)) {
                    (this)->value = value;
                }
                else {
                    std::cerr << "Warning: \"" + name + "\" " << std::endl;
                }
            }
            else {
                std::cerr << "Warning: \"" + name + "\" " << std::endl;
            }
        };

        virtual void pass_to_shader_impl(const uint32_t shader_id) const = 0;
        virtual void set_uniform_impl(const std::any value) = 0;

    protected:
        std::any uniform;
        std::any default_value;
    };

    class BoolPort : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<BoolPort> create(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            return std::shared_ptr<BoolPort>(new BoolPort(io, name, variable_name, v_speed, v_min, v_max));
        };
    private:
        BoolPort(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed, const float v_min, const float v_max) 
            : Port(io, name, variable_name, IM_COL32(204, 255, 153, 255), v_speed, v_min, v_max)
        {
            this->uniform = false;
            this->type = Bool;
        }

        void render_impl()
        {
            ImGui::Checkbox(("##" + name).c_str(), (bool*)&uniform);
        };
        void pass_to_shader_impl(const uint32_t shader_id) const
        {
            auto val = std::any_cast<bool>(uniform);;
            auto var_name = std::any_cast<std::string>(value);
            glUniform1i(glGetUniformLocation(shader_id, var_name.c_str()), (int)val);
        };
        void set_uniform_impl(const std::any uniform)
        {
            if (const bool* v = std::any_cast<bool>(&uniform)) {
                (this)->uniform = uniform;
            }
            else {
                std::cerr << "Warning: Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };
    };

    class IntPort : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<IntPort> create(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            return std::shared_ptr<IntPort>(new IntPort(io, name, variable_name, v_speed, v_min, v_max));
        };
    private:
        IntPort(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, variable_name, IM_COL32(204, 255, 153, 255), v_speed, v_min, v_max)
        {
            this->uniform = 0;
            this->type = Int;
        }

        void render_impl()
        {
            ImGui::DragInt(("##" + name).c_str(), (int*)&uniform);
        };
        void pass_to_shader_impl(const uint32_t shader_id) const
        {
            auto val = std::any_cast<int>(uniform);;
            auto var_name = std::any_cast<std::string>(value);
            glUniform1i(glGetUniformLocation(shader_id, var_name.c_str()), (int)val);
        };
        void set_uniform_impl(const std::any uniform)
        {
            if (const int* v = std::any_cast<int>(&uniform)) {
                (this)->uniform = uniform;
            }
            else {
                std::cerr << "Warning: Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };
    };

    class FloatPort : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<FloatPort> create(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            return std::shared_ptr<FloatPort>(new FloatPort(io, name, variable_name, v_speed, v_min, v_max));
        };
    private:
        FloatPort(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, variable_name, IM_COL32(255, 255, 153, 255), v_speed, v_min, v_max)
        {
            this->uniform = 0.f;
            this->type = Float;
        }
        void render_impl()
        {
            ImGui::DragFloat(("##" + name).c_str(), (float*)&uniform, v_speed, v_min, v_max);
        };
        void pass_to_shader_impl(const uint32_t shader_id) const
        {
            auto val = std::any_cast<float>(uniform);;
            auto var_name = std::any_cast<std::string>(value);
            glUniform1f(glGetUniformLocation(shader_id, var_name.c_str()), (float)val);
        };
        void set_uniform_impl(const std::any uniform)
        {
            if (const float* v = std::any_cast<float>(&uniform)) {
                (this)->uniform = uniform;
            }
            else {
                std::cerr << "Warning: Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };
    };

    class Vec2Port : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<Vec2Port> create(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            return std::shared_ptr<Vec2Port>(new Vec2Port(io, name, variable_name, v_speed, v_min, v_max));
        };
    private:
        Vec2Port(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, variable_name, IM_COL32(255, 195, 77, 255), v_speed, v_min, v_max)
        {
            this->uniform = glm::vec2(0.f);
            this->type = Vec2;
        }
        void render_impl()
        {
            ImGui::DragFloat2(("##" + name).c_str(), (float*)&uniform, v_speed, v_min, v_max);
        };
        void pass_to_shader_impl(const uint32_t shader_id) const
        {
            auto val = std::any_cast<glm::vec2>(uniform);;
            auto var_name = std::any_cast<std::string>(value);
            glUniform2fv(glGetUniformLocation(shader_id, var_name.c_str()), 1, &val[0]);
        };
        void set_uniform_impl(const std::any uniform)
        {
            if (const glm::vec2* v = std::any_cast<glm::vec2>(&uniform)) {
                (this)->uniform = uniform;
            }
            else {
                std::cerr << "Warning: Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };
    };

    class Vec3Port : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<Vec3Port> create(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            return std::shared_ptr<Vec3Port>(new Vec3Port(io, name, variable_name, v_speed, v_min, v_max));
        };
    private:
        Vec3Port(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, variable_name, IM_COL32(153, 153, 255, 255), v_speed, v_min, v_max)
        {
            this->uniform = glm::vec3(0.f);
            this->type = Vec3;
        }
        void render_impl()
        {
            ImGui::DragFloat3(("##" + name).c_str(), (float*)&uniform, v_speed, v_min, v_max);
        };
        void pass_to_shader_impl(const uint32_t shader_id) const
        {
            auto val = std::any_cast<glm::vec3>(uniform);;
            auto var_name = std::any_cast<std::string>(value);
            glUniform3fv(glGetUniformLocation(shader_id, var_name.c_str()), 1, &val[0]);
        };
        void set_uniform_impl(const std::any uniform)
        {
            if (const glm::vec3* v = std::any_cast<glm::vec3>(&uniform)) {
                (this)->uniform = uniform;
            }
            else {
                std::cerr << "Warning: Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };
    };

    class Vec4Port : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<Vec4Port> create(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            return std::shared_ptr<Vec4Port>(new Vec4Port(io, name, variable_name, v_speed, v_min, v_max));
        };
    private:
        Vec4Port(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, variable_name, IM_COL32(153, 102, 255, 255), v_speed, v_min, v_max)
        {
            this->uniform = glm::vec4(0.f);
            this->type = Vec4;
        }
        void render_impl()
        {
            ImGui::DragFloat4(("##" + name).c_str(), (float*)&uniform, v_speed, v_min, v_max);
        };
        void pass_to_shader_impl(const uint32_t shader_id) const
        {
            auto val = std::any_cast<glm::vec4>(uniform);;
            auto var_name = std::any_cast<std::string>(value);
            glUniform4fv(glGetUniformLocation(shader_id, var_name.c_str()), 1, &val[0]);
        };
        void set_uniform_impl(const std::any uniform)
        {
            if (const glm::vec4* v = std::any_cast<glm::vec4>(&uniform)) {
                (this)->uniform = uniform;
            }
            else {
                std::cerr << "Warning: Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };
    };

    class ColorPort : public Port
    {
    public:
        [[nodiscard]] static std::shared_ptr<ColorPort> create(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed = 1.f, const float v_min = 0.f, const float v_max = 0.f)
        {
            return std::shared_ptr<ColorPort>(new ColorPort(io, name, variable_name, v_speed, v_min, v_max));
        };
    private:
        ColorPort(const IO_t io, const std::string& name, const std::string& variable_name, const float v_speed, const float v_min, const float v_max)
            : Port(io, name, variable_name, IM_COL32(255, 102, 255, 255), v_speed, v_min, v_max)
        {
            this->uniform = glm::vec3(0.f);
            this->type = Color;
        }
        void render_impl()
        {
            ImGui::ColorEdit3("##hidelabel", (float*)&uniform, ImGuiColorEditFlags_NoInputs);
        };
        void pass_to_shader_impl(const uint32_t shader_id) const
        {
            auto val = std::any_cast<glm::vec3>(uniform);;
            auto var_name = std::any_cast<std::string>(value);
            glUniform3fv(glGetUniformLocation(shader_id, var_name.c_str()), 1, &val[0]);
        };
        void set_uniform_impl(const std::any uniform)
        {
            if (const glm::vec3* v = std::any_cast<glm::vec3>(&uniform)) {
                (this)->uniform = uniform;
            }
            else {
                std::cerr << "Warning: Any cast failed. The port \"" + name + "\" is not a " << std::endl;
            }
        };
    };
}