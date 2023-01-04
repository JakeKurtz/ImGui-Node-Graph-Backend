#include "ShaderView.h"

ShaderView::ShaderView()
{
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    name = "Shader View";

    start_time = std::chrono::system_clock::now();

    init_fbos();
}

void ShaderView::set_engine(std::shared_ptr<Shader::Engine> engine)
{
    (this)->engine = engine;
}

void ShaderView::draw()
{
    frame_time();

    ImGui::Begin(name.c_str(), &p_open, window_flags);

    auto _window_size_x = window_size.x;
    auto _window_size_y = window_size.y;

    update_context_interaction_state();
    update_window_properties();

    if (_window_size_x != window_size.x || _window_size_y != window_size.y)
    {
        fbo->set_attachment_size(window_size.x, window_size.y);
    }

    //menu_bar();

    render_shader();

    draw_to_window();

    if (ImGui::Button("Compile Shader Code"))
    {
        engine->compile();
    }

    ImGui::End();
}

void ShaderView::draw_to_window()
{
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImVec2 mp = ImGui::GetIO().MousePos;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->AddImage((void*)fbo->get_color_tex_id(),
        pos,
        ImVec2(pos.x + window_size.x, pos.y + window_size.y),
        ImVec2(0, 1),
        ImVec2(1, 0));
}

void ShaderView::draw_screen()
{
    if (screen_vao == 0)
    {
        glGenVertexArrays(1, &screen_vao);
        glGenBuffers(1, &screen_vbo);
        glBindVertexArray(screen_vao);
        glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screen_verts), &screen_verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(screen_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void ShaderView::render_shader()
{
    auto shader = engine->get_shader();
    auto uniforms = engine->get_uniforms();

    auto end_time = std::chrono::system_clock::now();
    float time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    fbo->bind();
        shader->use();
        glUniform1f(glGetUniformLocation(shader->get_id(), "time"), (float)time);
        for (const auto& uni_ports : uniforms) {
            uni_ports->pass_to_shader(shader->get_id());
        }
        draw_screen();
    fbo->unbind();
}

void ShaderView::menu_bar()
{
}

void ShaderView::window_size_callback(GLFWwindow* window, int width, int height)
{
}

void ShaderView::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void ShaderView::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

void ShaderView::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void ShaderView::key_callback()
{
}

void ShaderView::process_input(GLFWwindow* window)
{
}

void ShaderView::init_fbos()
{
    fbo = std::make_shared<FrameBuffer>(1, 1);
    fbo->attach(GL_COLOR, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
    fbo->attach(GL_DEPTH, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
    fbo->construct();
}
