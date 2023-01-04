#pragma once
#include "../INode.h"
#include "ShaderPort.h"
#include "../NodeOptns.h"

#include <chrono>
#include <ctime>
#include <algorithm>

namespace Shader
{
	static std::string call_glsl_op(const std::string& op_sym, const std::string& assign, const std::string& lhs, const std::string& rhs)
	{
		return "\t" + assign + " = " + lhs + " " + op_sym + " " + rhs + ";\n";
	};

	template<class ... Args>
	std::string call_glsl_func(const std::string& func_name, const std::string& assign, Args&&...args) {
		std::string func_args;
		int temp[] = { (func_args += std::string(args) + ",",0)... };
		static_cast<void>(temp);
		func_args.pop_back();
		return "\t" + assign + " = " + func_name + "(" + func_args + ");\n";
	};

	static std::string get_glsl_type(const Val_t type)
	{
		switch (type)
		{
		case Bool:
			return "bool";
		case Int:
			return "int";
		case Float:
			return "float";
		case Vec2:
			return "vec2";
		case Vec3:
			return "vec3";
		case Vec4:
			return "vec4";
		case Color:
			return "vec3";
		}
	};

	struct GLSLCode
	{
		std::string unif_decl;
		std::string locl_decl;
		std::string logic;

		void clear() {
			unif_decl.clear();
			locl_decl.clear();
			logic.clear();
		}
	};

	static GLSLCode& operator+=(GLSLCode& a, const GLSLCode& b)
	{
		a.unif_decl = a.unif_decl + b.unif_decl;
		a.locl_decl = a.locl_decl + b.locl_decl;
		a.logic = a.logic + b.logic;

		return a;
	}

	class Node : public INode
	{
	public:
		virtual ~Node() {};
		GLSLCode get_glsl_code() const;
		std::vector<std::shared_ptr<Port>> get_uniform_ports() const;
	protected:

		GLSLCode glsl_code;
		void construct_decl_strings();
		std::vector<std::shared_ptr<Port>> uniform_ports;
	private:
		void pre_evaluation() {
			glsl_code.clear();
		};
		void post_evaluation() {
			construct_decl_strings();
		};
	};

	class MathNode : public Node
	{
	public:
		~MathNode() {};

		[[nodiscard]] static std::shared_ptr<MathNode> create()
		{
			auto _node = std::shared_ptr<MathNode>(new MathNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto add = NodeBluePrint("Add", get_ptr());
			add.add_port(FloatPort::create(Input, "X", "x"));
			add.add_port(FloatPort::create(Input, "Y", "y"));
			add.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(add);

			auto sub = NodeBluePrint("Sub", get_ptr());
			sub.add_port(FloatPort::create(Input, "X", "x"));
			sub.add_port(FloatPort::create(Input, "Y", "y"));
			sub.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(sub);

			auto mul = NodeBluePrint("Mul", get_ptr());
			mul.add_port(FloatPort::create(Input, "X", "x"));
			mul.add_port(FloatPort::create(Input, "Y", "y"));
			mul.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(mul);

			auto div = NodeBluePrint("Div", get_ptr());
			div.add_port(FloatPort::create(Input, "X", "x"));
			div.add_port(FloatPort::create(Input, "Y", "y"));
			div.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(div);

			auto sin = NodeBluePrint("Sin", get_ptr());
			sin.add_optn(BoolOptn::create("Clamp", false));
			sin.add_port(FloatPort::create(Input, "Value", "val"));
			sin.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(sin);

			auto cos = NodeBluePrint("Cos", get_ptr());
			cos.add_optn(BoolOptn::create("Clamp", false));
			cos.add_port(FloatPort::create(Input, "Value", "val"));
			cos.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(cos);

			auto pow = NodeBluePrint("Power", get_ptr());
			pow.add_optn(BoolOptn::create("Clamp", false));
			pow.add_port(FloatPort::create(Input, "Base", "base"));
			pow.add_port(FloatPort::create(Input, "Exp", "exp"));
			pow.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(pow);
		};
		void evaluate_impl()
		{
			if
				(!sel_bp.compare("Add"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_op("+", out, x, y);
			}
			else if
				(!sel_bp.compare("Sub"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_op("-", out, x, y);
			}
			else if
				(!sel_bp.compare("Mul"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_op("*", out, x, y);
			}
			else if
				(!sel_bp.compare("Div"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_op("/", out, x, y);
			}
			else if
				(!sel_bp.compare("Sin"))
			{
				auto val = get_value<std::string>("Value");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_func("sin", out, val);
			}
			else if
				(!sel_bp.compare("Cos"))
			{
				auto val = get_value<std::string>("Value");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_func("cos", out, val);
			}
			else if
				(!sel_bp.compare("Power"))
			{
				auto base = get_value<std::string>("Base");
				auto exp = get_value<std::string>("Exp");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_func("pow", out, base, exp);
			}
		};
	};

	class VecMathNode : public Node
	{
	public:
		~VecMathNode() {};

		[[nodiscard]] static std::shared_ptr<VecMathNode> create()
		{
			auto _node = std::shared_ptr<VecMathNode>(new VecMathNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto add = NodeBluePrint("Add", get_ptr());
			add.add_port(Vec3Port::create(Input, "X", "x"));
			add.add_port(Vec3Port::create(Input, "Y", "y"));
			add.add_port(Vec3Port::create(Output, "Out", "out"));
			add_blueprint(add);

			auto sub = NodeBluePrint("Sub", get_ptr());
			sub.add_port(Vec3Port::create(Input, "X", "x"));
			sub.add_port(Vec3Port::create(Input, "Y", "y"));
			sub.add_port(Vec3Port::create(Output, "Out", "out"));
			add_blueprint(sub);

			auto mul = NodeBluePrint("Mul", get_ptr());
			mul.add_port(Vec3Port::create(Input, "X", "x"));
			mul.add_port(Vec3Port::create(Input, "Y", "y"));
			mul.add_port(Vec3Port::create(Output, "Out", "out"));
			add_blueprint(mul);

			auto div = NodeBluePrint("Div", get_ptr());
			div.add_port(Vec3Port::create(Input, "X", "x"));
			div.add_port(Vec3Port::create(Input, "Y", "y"));
			div.add_port(Vec3Port::create(Output, "Out", "out"));
			add_blueprint(div);

			auto sin = NodeBluePrint("Sin", get_ptr());
			sin.add_optn(BoolOptn::create("Clamp", false));
			sin.add_port(Vec3Port::create(Input, "Value", "val"));
			sin.add_port(Vec3Port::create(Output, "Out", "out"));
			add_blueprint(sin);

			auto cos = NodeBluePrint("Cos", get_ptr());
			cos.add_optn(BoolOptn::create("Clamp", false));
			cos.add_port(Vec3Port::create(Input, "Value", "val"));
			cos.add_port(Vec3Port::create(Output, "Out", "out"));
			add_blueprint(cos);

			auto pow = NodeBluePrint("Power", get_ptr());
			pow.add_optn(BoolOptn::create("Clamp", false));
			pow.add_port(Vec3Port::create(Input, "Base", "base"));
			pow.add_port(Vec3Port::create(Input, "Exp", "exp"));
			pow.add_port(Vec3Port::create(Output, "Out", "out"));
			add_blueprint(pow);
		};
		void evaluate_impl()
		{
			if
				(!sel_bp.compare("Add"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_op("+", out, x, y);
			}
			else if
				(!sel_bp.compare("Sub"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_op("-", out, x, y);
			}
			else if
				(!sel_bp.compare("Mul"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_op("*", out, x, y);
			}
			else if
				(!sel_bp.compare("Div"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_op("/", out, x, y);
			}
			else if
				(!sel_bp.compare("Sin"))
			{
				auto val = get_value<std::string>("Value");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_func("sin", out, val);
			}
			else if
				(!sel_bp.compare("Cos"))
			{
				auto val = get_value<std::string>("Value");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_func("cos", out, val);
			}
			else if
				(!sel_bp.compare("Power"))
			{
				auto base = get_value<std::string>("Base");
				auto exp = get_value<std::string>("Exp");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_func("pow", out, base, exp);
			}
		};
	};

	class VecMapRangeNode : public Node
	{
	public:
	public:
		[[nodiscard]] static std::shared_ptr<VecMapRangeNode> create()
		{
			auto _node = std::shared_ptr<VecMapRangeNode>(new VecMapRangeNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cc = NodeBluePrint("Map Range", get_ptr());
			cc.add_port(Vec3Port::create(Input, "Value", "value"));
			cc.add_port(Vec3Port::create(Input, "From Min", "min_0"));
			cc.add_port(Vec3Port::create(Input, "From Max", "max_0"));
			cc.add_port(Vec3Port::create(Input, "To Min", "min_1"));
			cc.add_port(Vec3Port::create(Input, "To Max", "max_1"));
			cc.add_port(Vec3Port::create(Output, "Out", "out"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
			auto value = get_value<std::string>("Value");
			auto min_0 = get_value<std::string>("From Min");
			auto max_0 = get_value<std::string>("From Max");
			auto min_1 = get_value<std::string>("To Min");
			auto max_1 = get_value<std::string>("To Max");
			auto out = get_value<std::string>("Out");

			glsl_code.logic = call_glsl_func("map", out, value, min_0, max_0, min_1, max_1);
		};
	};

	class MapRangeNode : public Node
	{
	public:
	public:
		[[nodiscard]] static std::shared_ptr<MapRangeNode> create()
		{
			auto _node = std::shared_ptr<MapRangeNode>(new MapRangeNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cc = NodeBluePrint("Map Range", get_ptr());
			cc.add_port(FloatPort::create(Input, "Value", "value"));
			cc.add_port(FloatPort::create(Input, "From Min", "min_0"));
			cc.add_port(FloatPort::create(Input, "From Max", "max_0"));
			cc.add_port(FloatPort::create(Input, "To Min", "min_1"));
			cc.add_port(FloatPort::create(Input, "To Max", "max_1"));
			cc.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
			auto value = get_value<std::string>("Value");
			auto min_0 = get_value<std::string>("From Min");
			auto max_0 = get_value<std::string>("From Max");
			auto min_1 = get_value<std::string>("To Min");
			auto max_1 = get_value<std::string>("To Max");
			auto out = get_value<std::string>("Out");

			glsl_code.logic = call_glsl_func("map", out, value, min_0, max_0, min_1, max_1);
		};
	};

	class RGBSplitNode : public Node
	{
	public:
		[[nodiscard]] static std::shared_ptr<RGBSplitNode> create()
		{
			auto _node = std::shared_ptr<RGBSplitNode>(new RGBSplitNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cs = NodeBluePrint("RGB Split", get_ptr());
			cs.add_port(ColorPort::create(Input, "Color", "color"));
			cs.add_port(FloatPort::create(Output, "R", "r"));
			cs.add_port(FloatPort::create(Output, "G", "g"));
			cs.add_port(FloatPort::create(Output, "B", "b"));
			add_blueprint(cs);
		}
		void evaluate_impl()
		{
			auto color = get_value<std::string>("Color");

			auto r = get_value<std::string>("R");
			auto g = get_value<std::string>("G");
			auto b = get_value<std::string>("B");

			glsl_code.logic = "\t" + r + "=" + color + ".r;\n\t" + g + "=" + color + ".g;\n\t" + b + "=" + color + ".b;\n";
		};
	};

	class RGBCombineNode : public Node
	{
	public:
	public:
		[[nodiscard]] static std::shared_ptr<RGBCombineNode> create()
		{
			auto _node = std::shared_ptr<RGBCombineNode>(new RGBCombineNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cc = NodeBluePrint("RGB Combine", get_ptr());
			cc.add_port(FloatPort::create(Input, "R", "r", 0.01f, 0.f, 1.f));
			cc.add_port(FloatPort::create(Input, "G", "g", 0.01f, 0.f, 1.f));
			cc.add_port(FloatPort::create(Input, "B", "b", 0.01f, 0.f, 1.f));
			cc.add_port(ColorPort::create(Output, "Color", "color"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
			auto r = get_value<std::string>("R");
			auto g = get_value<std::string>("G");
			auto b = get_value<std::string>("B");
			auto col = get_value<std::string>("Color");

			glsl_code.logic = call_glsl_func("vec3", col, r, g, b);
		};
	};

	class RGBMixNode : public Node
	{
	public:
	public:
		[[nodiscard]] static std::shared_ptr<RGBMixNode> create()
		{
			auto _node = std::shared_ptr<RGBMixNode>(new RGBMixNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cc = NodeBluePrint("RGB Mix", get_ptr());
			cc.add_port(ColorPort::create(Input, "Color 1", "col1"));
			cc.add_port(ColorPort::create(Input, "Color 2", "col2"));
			cc.add_port(FloatPort::create(Input, "Factor", "f", 0.01f, 0.f, 1.f));
			cc.add_port(ColorPort::create(Output, "Color", "color"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
			auto col1 = get_value<std::string>("Color 1");
			auto col2 = get_value<std::string>("Color 2");
			auto f = get_value<std::string>("Factor");
			auto col = get_value<std::string>("Color");

			glsl_code.logic = call_glsl_func("mix", col, col1, col2, f);
		};
	};

	class HSVCombineNode : public Node
	{
	public:
	public:
		[[nodiscard]] static std::shared_ptr<HSVCombineNode> create()
		{
			auto _node = std::shared_ptr<HSVCombineNode>(new HSVCombineNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cc = NodeBluePrint("HSV Combine", get_ptr());
			cc.add_port(FloatPort::create(Input, "H", "h", 0.01f, 0.f, 1.f));
			cc.add_port(FloatPort::create(Input, "S", "s", 0.01f, 0.f, 1.f));
			cc.add_port(FloatPort::create(Input, "V", "v", 0.01f, 0.f, 1.f));
			cc.add_port(ColorPort::create(Output, "Color", "color"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
			auto h = get_value<std::string>("H");
			auto s = get_value<std::string>("S");
			auto v = get_value<std::string>("V");
			auto col = get_value<std::string>("Color");

			glsl_code.logic = call_glsl_func("hsv_to_rgb", col, h, s, v);
		};
	};

	class ColorOutNode : public Node
	{
	public:

		[[nodiscard]] static std::shared_ptr<ColorOutNode> create()
		{
			auto _node = std::shared_ptr<ColorOutNode>(new ColorOutNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto add = NodeBluePrint("Color Out", get_ptr());
			add.add_port(ColorPort::create(Input, "In", "in"));
			add_blueprint(add);
		};
		void evaluate_impl()
		{
			auto in = get_value<std::string>("In");
			glsl_code.logic = "\tfrag_color = vec4(" + in + ", 1.f);";
		};
	protected:

	};

	class TimeNode : public Node
	{
	public:
	public:
		[[nodiscard]] static std::shared_ptr<TimeNode> create()
		{
			auto _node = std::shared_ptr<TimeNode>(new TimeNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cc = NodeBluePrint("Time", get_ptr());
			cc.add_port(FloatPort::create(GlobalOutput, "Time", "time"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
		};
	};

	class ScrnPosNode : public Node
	{
	public:
	public:
		[[nodiscard]] static std::shared_ptr<ScrnPosNode> create()
		{
			auto _node = std::shared_ptr<ScrnPosNode>(new ScrnPosNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cc = NodeBluePrint("Screen Position", get_ptr());
			cc.add_port(Vec2Port::create(GlobalOutput, "Tex Coords", "texcoord"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
		};
	};

	class NoiseNode : public Node
	{
	public:
		[[nodiscard]] static std::shared_ptr<NoiseNode> create()
		{
			auto _node = std::shared_ptr<NoiseNode>(new NoiseNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto v2 = NodeBluePrint("2D", get_ptr());
			v2.add_port(Vec2Port::create(Input, "Vector", "vec"));
			v2.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(v2);

			auto v3 = NodeBluePrint("3D", get_ptr());
			v3.add_port(Vec3Port::create(Input, "Vector", "vec"));
			v3.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(v3);

			auto v4 = NodeBluePrint("4D", get_ptr());
			v4.add_port(Vec2Port::create(Input, "X", "x"));
			v4.add_port(FloatPort::create(Input, "Y", "y"));
			v4.add_port(FloatPort::create(Input, "Z", "z"));
			v4.add_port(FloatPort::create(Input, "W", "w"));
			v4.add_port(FloatPort::create(Output, "Out", "out"));
			add_blueprint(v4);
		}
		void evaluate_impl()
		{
			if
				(!sel_bp.compare("2D"))
			{
				auto vec = get_value<std::string>("Vector");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_func("snoise", out, vec);
			}
			else if
				(!sel_bp.compare("3D"))
			{
				auto vec = get_value<std::string>("Vector");
				auto out = get_value<std::string>("Out");

				glsl_code.logic = call_glsl_func("snoise", out, vec);
			}
			else if
				(!sel_bp.compare("4D"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto z = get_value<std::string>("Z");
				auto w = get_value<std::string>("W");
				auto vec = get_value<std::string>("Vector");
			}
		};
	};

	class VecSplitNode : public Node
	{
	public:
		[[nodiscard]] static std::shared_ptr<VecSplitNode> create()
		{
			auto _node = std::shared_ptr<VecSplitNode>(new VecSplitNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto v2 = NodeBluePrint("Vec 2", get_ptr());
			v2.add_port(Vec2Port::create(Input, "Vector", "vec"));
			v2.add_port(FloatPort::create(Output, "X", "x"));
			v2.add_port(FloatPort::create(Output, "Y", "y"));
			add_blueprint(v2);

			auto v3 = NodeBluePrint("Vec 3", get_ptr());
			v3.add_port(Vec3Port::create(Input, "Vector", "vec"));
			v3.add_port(FloatPort::create(Output, "X", "x"));
			v3.add_port(FloatPort::create(Output, "Y", "y"));
			v3.add_port(FloatPort::create(Output, "Z", "z"));
			add_blueprint(v3);

			auto v4 = NodeBluePrint("Vec 4", get_ptr());
			v4.add_port(Vec4Port::create(Input, "Vector", "vec"));
			v4.add_port(FloatPort::create(Output, "X", "x"));
			v4.add_port(FloatPort::create(Output, "Y", "y"));
			v4.add_port(FloatPort::create(Output, "Z", "z"));
			v4.add_port(FloatPort::create(Output, "W", "w"));
			add_blueprint(v4);
		}
		void evaluate_impl()
		{
			if
			(!sel_bp.compare("Vec 2"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto vec = get_value<std::string>("Vector");

				glsl_code.logic = "\t" + x + "=" + vec + ".x;\n\t" + y + "=" + vec + ".y;\n";
			}
			else if
			(!sel_bp.compare("Vec 3"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto z = get_value<std::string>("Z");
				auto vec = get_value<std::string>("Vector");

				glsl_code.logic = "\t" + x + "=" + vec + ".x;\n\t" + y + "=" + vec + ".y;\n\t" + z + " = " + vec + ".z; \n";
			}
			else if
			(!sel_bp.compare("Vec 4"))
			{
				auto x = get_value<std::string>("X");
				auto y = get_value<std::string>("Y");
				auto z = get_value<std::string>("Z");
				auto w = get_value<std::string>("W");
				auto vec = get_value<std::string>("Vector");

				glsl_code.logic = "\t" + x + "=" + vec + ".x;\n\t" + y + "=" + vec + ".y;\n\t" + z + " = " + vec + ".z; \n\t" + w + " = " + vec + ".w; \n";
			}
		};
	};
}