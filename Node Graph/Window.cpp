#include "Window.h"

void Window::frame_time()
{
    current_frame_time = glfwGetTime();
    delta_time = current_frame_time - last_frame_time;
    last_frame_time = current_frame_time;
}

void Window::update_window_properties()
{
    window_size = ImGui::GetWindowSize();

    window_min_pos = ImGui::GetWindowContentRegionMin();
    window_max_pos = ImGui::GetWindowContentRegionMax();

    window_min_pos.x += ImGui::GetWindowPos().x;
    window_min_pos.y += ImGui::GetWindowPos().y;
    window_max_pos.x += ImGui::GetWindowPos().x;
    window_max_pos.y += ImGui::GetWindowPos().y;

    window_center.x = (window_max_pos.x + window_min_pos.x) / 2.f;
    window_center.y = (window_max_pos.y + window_min_pos.y) / 2.f;
}

void Window::update_context_interaction_state()
{
    bool title_bar_hovered = ImGui::IsItemHovered();
    currently_focused = (ImGui::IsWindowFocused() && !title_bar_hovered) ? true : false;
}
