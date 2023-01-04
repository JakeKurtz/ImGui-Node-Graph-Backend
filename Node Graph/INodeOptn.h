#pragma once
#include "INodePort.h"

class INodeOptn
{
public:
    INodeOptn() 
    {
        logger.log(debug, "Option(" + std::to_string(id) + ") : " + name + " was created");
    };
    ~INodeOptn()
    {
        logger.log(debug, "Option(" + std::to_string(id) + ") : " + name + " was destroyed");
    };

    uint32_t get_id() const;
    Val_t get_type() const;
    std::string get_name() const;
    std::any get_value() const;

    void render();

protected:
    uint32_t id = gen_id();
    std::string name;
    Val_t type;
    std::any value;

    void set_default_value(const std::any value) { set_default_value_impl(value); };

private:
    virtual void render_impl() = 0;
    virtual void set_default_value_impl(const std::any value) = 0;
};

