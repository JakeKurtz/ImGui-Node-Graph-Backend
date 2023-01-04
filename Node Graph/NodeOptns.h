#pragma once
#include "INodeOptn.h"

class BoolOptn : public INodeOptn
{
public:
    [[nodiscard]] static std::shared_ptr<BoolOptn> create(const std::string& name, const bool& default_value = false)
    {
        auto _port = std::shared_ptr<BoolOptn>(new BoolOptn());
        _port->set_default_value(default_value);
        _port->name = name;
        _port->type = Bool;
        return _port;
    };
private:
    void set_default_value_impl(const std::any value)
    {
        if (const bool* v = std::any_cast<bool>(&value)) {
            (this)->value = value;
        }
        else {
            logger.log(warn, "Any cast failed. The option \"" + name + "\" is a bool type.");
        }
    };
    void render_impl()
    {
        ImGui::Checkbox(("##" + name).c_str(), (bool*)&value);
    };
};

class ComboOptn : public INodeOptn
{
public:
    [[nodiscard]] static std::shared_ptr<ComboOptn> create(const std::string& name, std::vector<std::string> optns, const std::string& default_value)
    {
        auto _port = std::shared_ptr<ComboOptn>(new ComboOptn());
        _port->set_default_value(default_value);
        _port->name = name;
        _port->optns = optns;
        _port->type = String;
        return _port;
    };
private:
    void set_default_value_impl(const std::any value)
    {
        if (const std::string* v = std::any_cast<std::string>(&value)) {
            (this)->value = value;
        }
        else {
            logger.log(warn, "Any cast failed. The option \"" + name + "\" is a std::vector<std::string> type.");
        }
    };
    void render_impl()
    {
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(name.c_str()).x);
        const std::string sel_optn = std::any_cast<std::string>(value);
        if (ImGui::BeginCombo(("##"+name).c_str(), sel_optn.c_str()))
        {
            for (const auto& optn : optns)
            {
                bool is_selected = (sel_optn == optn);
                if (ImGui::Selectable(optn.c_str(), is_selected)) {
                    value = optn;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    };
    std::vector<std::string> optns;
};