//#define USE_SHADER

#include "GLCommon.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Window.h"
#include "NodeEditWindow.h"
#include "Shader Example/ShaderView.h"
#include "Shader Example/ShaderEngine.h"
#include "Shader Example/ShaderEngine.h"
#include "Basic Example/BasicEngine.h"
#include "Basic Example/BasicNode.h"

const char* glsl_version = "#version 330 core";
GLuint image_texture;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

std::vector<std::shared_ptr<Window>> contexts;
std::shared_ptr<Window> active_context;

static void glfw_error_callback(int error, const char* description);
static void glfw_window_size_callback(GLFWwindow* window, int width, int height);
static void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void glfw_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void glfw_process_input(GLFWwindow* window);

static void glfw_init(GLFWwindow** window, const int width, const int height)
{
    //
    // INITIALIZE GLFW/GLAD
    //

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef PXL_FULLSCREEN
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    *window = glfwCreateWindow(mode->width, mode->height, "GLFW / CUDA Interop", monitor, NULL);
#else
    * window = glfwCreateWindow(width, height, "GLFW / CUDA Interop", NULL, NULL);
#endif

    if (*window == NULL)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(*window);

    glfwSetErrorCallback(glfw_error_callback);
    glfwSetKeyCallback(*window, glfw_key_callback);
    glfwSetFramebufferSizeCallback(*window, glfw_window_size_callback);
    glfwSetCursorPosCallback(*window, glfw_mouse_callback);
    glfwSetMouseButtonCallback(*window, glfw_mouse_button_callback);
    glfwSetScrollCallback(*window, glfw_mouse_scroll_callback);

    // set up GLAD
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR);

    // ignore vsync for now
    glfwSwapInterval(0);

    // only copy r/g/b
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}
static void imgui_init(GLFWwindow** window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(*window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    //show_demo_window = true;
    //show_another_window = false;
    ImVec4 clear_color;

    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

}
void render_gui(GLFWwindow* window)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    for (auto c : contexts) {
        if (c->focused()) {
            active_context = c;
        }
        c->draw();
    }

    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    ImGuiIO io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

int main()
{
    GLFWwindow* window;
    glfw_init(&window, 250, 250);
    imgui_init(&window);

#ifdef USE_SHADER
    auto shader_engine = std::make_shared<Shader::Engine>();
    std::shared_ptr<NodeEditWindow> node_edit_window = std::make_shared<NodeEditWindow>(shader_engine);
    
    node_edit_window->add_node<Shader::NoiseNode>("Noise");
    node_edit_window->add_node<Shader::MathNode>("Math");
    node_edit_window->add_node<Shader::VecMathNode>("Vector Math");
    node_edit_window->add_node<Shader::MapRangeNode>("Map Range");
    node_edit_window->add_node<Shader::VecMapRangeNode>("Vec Map Range");
    node_edit_window->add_node<Shader::RGBSplitNode>("RGB Split");
    node_edit_window->add_node<Shader::RGBCombineNode>("RGB Combine");
    node_edit_window->add_node<Shader::RGBMixNode>("RGB Mix");
    node_edit_window->add_node<Shader::HSVCombineNode>("HSV Combine");
    node_edit_window->add_node<Shader::VecSplitNode>("Vector Split");
    node_edit_window->add_node<Shader::TimeNode>("Time");
    node_edit_window->add_node<Shader::ScrnPosNode>("Screen Pos");
    node_edit_window->add_node<Shader::ColorOutNode>("Color Out");
    
    std::shared_ptr<ShaderView> shader_view_window = std::make_shared<ShaderView>();
    shader_view_window->set_engine(shader_engine);

    contexts.push_back(node_edit_window);
    contexts.push_back(shader_view_window);

#else
    auto basic_engine = std::make_shared<Basic::Engine>();
    std::shared_ptr<NodeEditWindow> node_edit_window = std::make_shared<NodeEditWindow>(basic_engine);
    node_edit_window->add_node<Basic::TestNode>("Tester");
    node_edit_window->add_node<Basic::MathNode>("Math");
    node_edit_window->add_node<Basic::MapRangeNode>("Map Range");
    node_edit_window->add_node<Basic::RGBSplitNode>("RGB Split");
    node_edit_window->add_node<Basic::RGBCombineNode>("RGB Combine");
    node_edit_window->add_node<Basic::HSVCombineNode>("HSV Combine");
    node_edit_window->add_node<Basic::TimeNode>("Time");
    node_edit_window->add_node<Basic::ColorOutNode>("Color Out");

    contexts.push_back(node_edit_window);
#endif

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_gui(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0; 
}

static void glfw_error_callback(int error, const char* description)
{
    //fputs(description, stderr);
}
static void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
    //glViewport(0, 0, width, height);
    //buffer_reset = true;
    if (active_context != nullptr) active_context->window_size_callback(window, width, height);
}
static void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (active_context != nullptr) active_context->mouse_callback(window, xpos, ypos);
}
static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (active_context != nullptr) active_context->mouse_button_callback(window, button, action, mods);
}
static void glfw_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (active_context != nullptr) active_context->mouse_scroll_callback(window, xoffset, yoffset);
}
static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (active_context != nullptr) active_context->key_callback();
}
static void glfw_process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (active_context != nullptr) active_context->process_input(window);
}