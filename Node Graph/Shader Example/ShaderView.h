#pragma once
#include "../Window.h"
#include "FrameBuffer.h"
#include "GlslShader.h"
#include "ShaderEngine.h"

class ShaderView : public Window
{
public:
    ShaderView();

    void set_engine(std::shared_ptr<Shader::Engine> engine);

	void draw();

    void menu_bar();

    void window_size_callback(GLFWwindow* window, int width, int height);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void key_callback();
    void process_input(GLFWwindow* window);
private:
    void draw_to_window();
    void draw_screen();
    void render_shader();

    void init_fbos();

    std::shared_ptr<FrameBuffer> fbo;

    std::shared_ptr<Shader::Engine> engine;

    std::chrono::system_clock::time_point start_time;

    unsigned int screen_vao = 0;
    unsigned int screen_vbo;
    float screen_verts[20] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
};

