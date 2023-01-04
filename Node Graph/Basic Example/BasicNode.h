#pragma once
#include "../INode.h"
#include "BasicPorts.h"
#include "../NodeOptns.h"

#include <chrono>
#include <ctime>
#include <algorithm>

namespace Basic {
	class TestNode : public INode
	{
	public:
		[[nodiscard]] static std::shared_ptr<TestNode> create()
		{
			auto _node = std::shared_ptr<TestNode>(new TestNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cc = NodeBluePrint("Test 0", get_ptr());
			cc.add_port(BoolPort::create(Input, "A"));
			cc.add_port(IntPort::create(Input, "B"));
			cc.add_port(FloatPort::create(Input, "C"));
			cc.add_port(Vec2Port::create(Input, "D"));
			cc.add_port(Vec3Port::create(Input, "E"));
			cc.add_port(Vec4Port::create(Input, "F"));
			cc.add_port(ColorPort::create(Input, "G"));

			cc.add_port(BoolPort::create(Output, "H"));
			cc.add_port(IntPort::create(Output, "I"));
			cc.add_port(FloatPort::create(Output, "J"));
			cc.add_port(Vec2Port::create(Output, "K"));
			cc.add_port(Vec3Port::create(Output, "L"));
			cc.add_port(Vec4Port::create(Output, "M"));
			cc.add_port(ColorPort::create(Output, "N"));
			add_blueprint(cc);

			auto t1 = NodeBluePrint("Test 1", get_ptr());

			t1.add_optn(BoolOptn::create("Bool", false));
			t1.add_optn(ComboOptn::create("Combo", { "optn 0", "optn 3" , "optn 2" }, "optn 0"));
			t1.add_optn(ComboOptn::create("Combo2", { "optn 0", "optn 3" , "optn 2" }, "optn 0"));

			t1.add_port(BoolPort::create(Input, "A"));
			t1.add_port(IntPort::create(Input, "B"));
			t1.add_port(FloatPort::create(Input, "C"));

			t1.add_port(BoolPort::create(Output, "G"));
			t1.add_port(IntPort::create(Output, "H"));
			t1.add_port(FloatPort::create(Output, "I"));
			add_blueprint(t1);
		}
		void evaluate_impl()
		{
			auto r = get_value<float>("R");
			auto g = get_value<float>("G");
			auto b = get_value<float>("B");

			auto x = get_value<float>("Option value");

			set_value("Color", glm::vec3(r, g, b));
		};
	};

	class MathNode : public INode
	{
	public:
		[[nodiscard]] static std::shared_ptr<MathNode> create()
		{
			auto _node = std::shared_ptr<MathNode>(new MathNode()); _node->build(); return _node;
		};

	private:
		void build_impl()
		{
			auto add = NodeBluePrint("Add", get_ptr());
			add.add_port(FloatPort::create(Input, "X"));
			add.add_port(FloatPort::create(Input, "Y"));
			add.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(add);

			auto sub = NodeBluePrint("Subtract", get_ptr());
			sub.add_port(FloatPort::create(Input, "X"));
			sub.add_port(FloatPort::create(Input, "Y"));
			sub.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(sub);

			auto mul = NodeBluePrint("Multiply", get_ptr());
			mul.add_port(FloatPort::create(Input, "X"));
			mul.add_port(FloatPort::create(Input, "Y"));
			mul.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(mul);

			auto div = NodeBluePrint("Divide", get_ptr());
			div.add_port(FloatPort::create(Input, "X"));
			div.add_port(FloatPort::create(Input, "Y"));
			div.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(div);

			auto sin = NodeBluePrint("Sin", get_ptr());
			sin.add_optn(BoolOptn::create("Clamp", false));
			sin.add_port(FloatPort::create(Input, "Value"));
			sin.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(sin);

			auto cos = NodeBluePrint("Cos", get_ptr());
			cos.add_optn(BoolOptn::create("Clamp", false));
			cos.add_port(FloatPort::create(Input, "Value"));
			cos.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(cos);

			auto pow = NodeBluePrint("Power", get_ptr());
			pow.add_optn(BoolOptn::create("Clamp", false));
			pow.add_port(FloatPort::create(Input, "Base"));
			pow.add_port(FloatPort::create(Input, "Exp"));
			pow.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(pow);
		};
		void evaluate_impl()
		{
			if
				(!sel_bp.compare("Add"))
			{
				auto x = get_value<float>("X");
				auto y = get_value<float>("Y");

				set_value("Out", x + y);
			}
			else if
				(!sel_bp.compare("Subtract"))
			{
				auto x = get_value<float>("X");
				auto y = get_value<float>("Y");
				set_value("Out", x - y);
			}
			else if
				(!sel_bp.compare("Multiply"))
			{
				auto x = get_value<float>("X");
				auto y = get_value<float>("Y");
				set_value("Out", x * y);
			}
			else if
				(!sel_bp.compare("Divide"))
			{
				auto x = get_value<float>("X");
				auto y = get_value<float>("Y");
				set_value("Out", x / y);
			}
			else if
				(!sel_bp.compare("Sin"))
			{
				auto val = get_value<float>("Value");
				auto c = get_value<bool>("Clamp");
				auto out = sin(val);
				if (c) std::clamp(out, 0.f, 1.f);
				set_value("Out", out);
			}
			else if
				(!sel_bp.compare("Cos"))
			{
				auto val = get_value<float>("Value");
				auto c = get_value<bool>("Clamp");
				auto out = cos(val);
				if (c) std::clamp(out, 0.f, 1.f);
				set_value("Out", out);
			}
			else if
				(!sel_bp.compare("Power"))
			{
				auto base = get_value<float>("Base");
				auto exp = get_value<float>("Exp");
				set_value("Out", pow(base, exp));
			}
		};
	};

	class MapRangeNode : public INode
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
			cc.add_port(FloatPort::create(Input, "Value"));
			cc.add_port(FloatPort::create(Input, "From Min"));
			cc.add_port(FloatPort::create(Input, "From Max"));
			cc.add_port(FloatPort::create(Input, "To Min"));
			cc.add_port(FloatPort::create(Input, "To Max"));
			cc.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
			auto value = get_value<float>("Value");
			auto min_0 = get_value<float>("From Min");
			auto max_0 = get_value<float>("From Max");
			auto min_1 = get_value<float>("To Min");
			auto max_1 = get_value<float>("To Max");

			set_value("Out", min_1 + (value - min_0) * (max_1 - min_1) / (max_0 - min_0));
		};
	};

	class RGBSplitNode : public INode
	{
	public:
		[[nodiscard]] static std::shared_ptr<RGBSplitNode> create()
		{
			auto _node = std::shared_ptr<RGBSplitNode>(new RGBSplitNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto cs = NodeBluePrint("Color Split", get_ptr());
			cs.add_port(ColorPort::create(Input, "Color"));
			cs.add_port(FloatPort::create(Output, "R"));
			cs.add_port(FloatPort::create(Output, "G"));
			cs.add_port(FloatPort::create(Output, "B"));
			add_blueprint(cs);
		}
		void evaluate_impl()
		{
			auto color = get_value<glm::vec3>("Color");
			set_value("R", color.r);
			set_value("G", color.g);
			set_value("B", color.b);
		};
	};

	class RGBCombineNode : public INode
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
			auto cc = NodeBluePrint("Color Combine", get_ptr());
			cc.add_port(FloatPort::create(Input, "R", 0.01f, 0.f, 1.f));
			cc.add_port(FloatPort::create(Input, "G", 0.01f, 0.f, 1.f));
			cc.add_port(FloatPort::create(Input, "B", 0.01f, 0.f, 1.f));
			cc.add_port(ColorPort::create(Output, "Color"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
			auto r = get_value<float>("R");
			auto g = get_value<float>("G");
			auto b = get_value<float>("B");
			set_value("Color", glm::vec3(r, g, b));
		};
	};

	class HSVCombineNode : public INode
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
			auto cc = NodeBluePrint("HSV", get_ptr());
			cc.add_port(FloatPort::create(Input, "Hue", 0.01f, 0.f, 1.f));
			cc.add_port(FloatPort::create(Input, "Sat", 0.01f, 0.f, 1.f));
			cc.add_port(FloatPort::create(Input, "Val", 0.01f, 0.f, 1.f));
			cc.add_port(ColorPort::create(Output, "Color"));
			add_blueprint(cc);
		}
		void evaluate_impl()
		{
			auto h = get_value<float>("Hue");
			auto s = get_value<float>("Sat");
			auto v = get_value<float>("Val");
			float r, g, b;
			ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
			set_value("Color", glm::vec3(r, g, b));
		};
	};

	class TimeNode : public INode
	{
	public:
		[[nodiscard]] static std::shared_ptr<TimeNode> create()
		{
			auto _node = std::shared_ptr<TimeNode>(new TimeNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			start_time = std::chrono::system_clock::now();

			auto tn = NodeBluePrint("Time", get_ptr());
			tn.add_optn(ComboOptn::create("OOM", { "(s)", "(ms)" , "(ns)" }, "(ms)"));
			tn.add_port(FloatPort::create(Output, "Out"));
			add_blueprint(tn);
		}
		void evaluate_impl()
		{
			auto end_time = std::chrono::system_clock::now();
			auto elapsed_time = std::chrono::duration<float>(end_time - start_time);

			float time;
			std::string order_of_mag = get_value<std::string>("OOM");
			if (!order_of_mag.compare("(s)")) {
				time = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
			}
			else if (!order_of_mag.compare("(ms)")) {
				time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
			}
			else if (!order_of_mag.compare("(ns)")) {
				time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
			}

			set_value("Out", time);
		};
		std::chrono::system_clock::time_point start_time;
	};

	class ColorOutNode : public INode
	{
	public:
		[[nodiscard]] static std::shared_ptr<ColorOutNode> create()
		{
			auto _node = std::shared_ptr<ColorOutNode>(new ColorOutNode()); _node->build(); return _node;
		};
	private:
		void build_impl()
		{
			auto co = NodeBluePrint("Color Out", get_ptr());
			co.add_port(ColorPort::create(Input, "Color"));
			add_blueprint(co);
		}
		void evaluate_impl()
		{

		};
	};
}