#pragma once
#include <any>
#include <string>
#include <boost/type_index.hpp>
/*
namespace Shader
{
	class IProp
	{
    public:
        void pass_to_shader(const uint32_t shader_id)
        {
            pass_to_shader_impl(shader_id);
        };
        
        template <typename T>
        T get_value();
        void set_value(const std::any value)
        {
            set_value_impl(value);
        };
    private:
        virtual void set_value_impl(const std::any value) = 0;
        virtual void pass_to_shader_impl(const uint32_t shader_id) const = 0;

		std::any value;
	};

    template<typename T>
    inline T IProp::get_value()
    {
        if (const T* _v = std::any_cast<T>(&value)) {
            return *_v;
        }
        else {
            std::cerr << "Warning: Any cast failed. The shader property \"" + val_key + "\" is not a " + boost::typeindex::type_id<T>().pretty_name() << std::endl;
            return T();
        }
    }

    class BoolProp : public IProp
    {

    };
}
*/

