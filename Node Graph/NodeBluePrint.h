#pragma once
#include "INodePort.h"
#include "INodeOptn.h"
#include <vector>

class NodeBluePrint 
{
public:
    NodeBluePrint() 
    {
        logger.log(debug, "NodeBluePrint was created");
    };
    NodeBluePrint(const std::string& name, std::weak_ptr<INode> parent) : name(name), parent(parent) 
    {
        logger.log(debug, "NodeBluePrint: " + name + " was created");
    };
    ~NodeBluePrint()
    {
        logger.log(debug, "NodeBluePrint: " + name + " was destroyed");
    };

    void enable();
    void disable();

    void add_port(const std::shared_ptr<INodePort>& port);
    void add_optn(const std::shared_ptr<INodeOptn>& optn);

    std::string get_name() const;
    std::map<std::string, std::shared_ptr<INodePort>> get_port_map() const;
    std::vector<std::string> get_port_keys() const;    
    
    std::map<std::string, std::shared_ptr<INodeOptn>> get_optn_map() const;
    std::vector<std::string> get_optn_keys() const;

    bool is_port(const std::string& name);
    bool is_optn(const std::string& name);

private:
    uint32_t in_cnt;
    uint32_t out_cnt;

    std::string name;

    std::weak_ptr<INode> parent;

    std::map<std::string, std::shared_ptr<INodePort>> port_map;
    std::vector<std::string> port_keys;    
    
    std::map<std::string, std::shared_ptr<INodeOptn>> optn_map;
    std::vector<std::string> optn_keys;

    bool name_exists(const std::string& name);
};