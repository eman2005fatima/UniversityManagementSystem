#include "ToastSystem.h"
#include <algorithm>

const double ToastSystem::DISPLAY_TIME = 3.0;

void ToastSystem::push(const std::string& msg, int type) {
    Toast t;
    t.message = msg;
    t.type = type;
    t.spawnTime = glfwGetTime();
    t.slideX = 300.0f;
    t.alpha = 1.0f;
    queue_.push_back(t);
    if ((int)queue_.size() > MAX_VISIBLE)
        queue_.pop_front();
}

void ToastSystem::render() {
    double now = glfwGetTime();
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    auto it = queue_.begin();
    while (it != queue_.end()) {
        double age = now - it->spawnTime;

        if (age > DISPLAY_TIME + 1.0) {
            it = queue_.erase(it);
            continue;
        }

        it->slideX = 300.0f * (1.0f - std::min((float)age * 8.0f, 1.0f));
        it->alpha = std::min((float)age * 5.0f, 1.0f);
        if (age > DISPLAY_TIME)
            it->alpha = std::max(1.0f - (float)(age - DISPLAY_TIME) * 4.0f, 0.0f);

        ImVec4 bgColor;
        switch (it->type) {
            case 0: bgColor = ImVec4(0.18f, 0.80f, 0.44f, 0.95f); break; // success green
            case 1: bgColor = ImVec4(0.91f, 0.30f, 0.24f, 0.95f); break; // danger red
            case 2: bgColor = ImVec4(0.95f, 0.61f, 0.07f, 0.95f); break; // warning amber
            default: bgColor = ImVec4(0.61f, 0.35f, 0.71f, 0.95f); break; // info purple
        }
        bgColor.w *= it->alpha;

        float toastW = 380.0f;
        float toastH = 56.0f;
        float yOffset = 90.0f + (std::distance(queue_.begin(), it) * (toastH + 10.0f));
        float xPos = displaySize.x - toastW - 20.0f + it->slideX;

        ImGui::SetNextWindowPos(ImVec2(xPos, yOffset), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(toastW, toastH), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, it->alpha));

        ImGui::Begin(("##toast" + std::to_string((int)(it->spawnTime * 1000))).c_str(),
                     nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings);

        // Icon + message
        const char* icon = it->type == 0 ? "[OK]" : (it->type == 1 ? "[X]" : (it->type == 2 ? "[!]" : "[i]"));
        ImGui::Text("%s  %s", icon, it->message.c_str());

        // Countdown progress bar
        float progress = (float)(age / DISPLAY_TIME);
        ImGui::Dummy(ImVec2(0, 6));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1, 1, 1, 0.4f * it->alpha));
        ImGui::ProgressBar(1.0f - progress, ImVec2(toastW - 20, 3), "");
        ImGui::PopStyleColor();

        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);

        ++it;
    }
}
