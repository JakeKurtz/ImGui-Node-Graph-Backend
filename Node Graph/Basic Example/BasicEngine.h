#pragma once
#include "../IEngine.h"

namespace Basic {
    class Engine : public IEngine
    {
    public:
        Engine() { build_conversion_map(); };
    private:
        void build_conversion_map();
        void pre_eval();
        void post_eval();
        void eval_node(const std::shared_ptr<INode>& node);
    };
}
