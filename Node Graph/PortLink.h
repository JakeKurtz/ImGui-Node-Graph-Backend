#pragma once
#include "globals.h"

#include <memory>
#include "INodePort.h"

class PortLink : public std::enable_shared_from_this<PortLink>
{
public:
    ~PortLink();

    std::shared_ptr<PortLink> get_ptr();

    [[nodiscard]] static std::shared_ptr<PortLink> create(const std::shared_ptr<INodePort>& u, const std::shared_ptr<INodePort>& v);

    uint32_t get_id() const;

    std::shared_ptr<INodePort> get_u() const;
    std::shared_ptr<INodePort> get_v() const;

    void set_u(const std::shared_ptr<INodePort>& u);
    void set_v(const std::shared_ptr<INodePort>& v);

    void render() const;

private:
    PortLink(const std::shared_ptr<INodePort>& u, const std::shared_ptr<INodePort>& v) : u(u), v(v) {};

    uint32_t id = gen_id();
    std::shared_ptr<INodePort> u;
    std::shared_ptr<INodePort> v;

    void build();
};
