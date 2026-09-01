#include "SettingsPanel.h"
#include "../Auth.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include <cstring>

extern ToastSystem g_Toast;
extern bool darkMode;

void ShowSettingsPanel(AppState& state, Database& db) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::TextUnformatted("SETTINGS");
    ImGui::PopStyleColor();
    DrawAccentLine(40.0f, 3.0f);
    ImGui::Dummy(ImVec2(0, 12));

    BeginFormCard();
    DrawSectionHeader("APPEARANCE");
    ImGui::Dummy(ImVec2(0, 8));
    ImGui::SetCursorPosX(180);
    if (ImGui::Button(darkMode ? "Switch to Light Mode" : "Switch to Dark Mode", ImVec2(240, 38))) {
        ToggleTheme();
        g_Toast.push(darkMode ? "Dark mode enabled." : "Light mode enabled.", 0);
    }
    EndFormCard();

    ImGui::Dummy(ImVec2(0, 12));

    BeginFormCard();
    DrawSectionHeader("CHANGE PASSWORD");

    static char oldPass[128] = "", newPass1[128] = "", newPass2[128] = "";
    DrawLabeledInput("Current Password", "##oldpass", oldPass, 128, ImGuiInputTextFlags_Password);
    DrawLabeledInput("New Password", "##newpass1", newPass1, 128, ImGuiInputTextFlags_Password);
    DrawLabeledInput("Confirm Password", "##newpass2", newPass2, 128, ImGuiInputTextFlags_Password);

    ImGui::SetCursorPosX(180);
    if (ImGui::Button("Update Password", ImVec2(220, 38))) {
        if (strcmp(newPass1, newPass2) != 0) {
            g_Toast.push("Passwords do not match.", 1);
        } else if (strlen(newPass1) < 8) {
            g_Toast.push("Password must be 8+ characters.", 1);
        } else {
            std::string err = Auth::changePassword(db, state.currentUser.userId, oldPass, newPass1);
            if (err.empty()) {
                g_Toast.push("Password changed!", 0);
                memset(oldPass, 0, sizeof(oldPass));
                memset(newPass1, 0, sizeof(newPass1));
                memset(newPass2, 0, sizeof(newPass2));
            } else {
                g_Toast.push(err, 1);
            }
        }
    }
    EndFormCard();

    ImGui::Dummy(ImVec2(0, 12));

    BeginFormCard();
    DrawSectionHeader("ACCOUNT INFO");
    ImGui::Dummy(ImVec2(0, 8));
    ImGui::SetCursorPosX(20);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::Text("Name:");
    ImGui::PopStyleColor();
    ImGui::SameLine(); ImGui::SetCursorPosX(120);
    ImGui::TextUnformatted(state.currentUser.name.c_str());
    ImGui::SetCursorPosX(20);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::Text("Email:");
    ImGui::PopStyleColor();
    ImGui::SameLine(); ImGui::SetCursorPosX(120);
    ImGui::TextUnformatted(state.currentUser.email.c_str());
    ImGui::SetCursorPosX(20);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::Text("Role:");
    ImGui::PopStyleColor();
    ImGui::SameLine(); ImGui::SetCursorPosX(120);
    ImGui::TextUnformatted(state.currentUser.role.c_str());
    EndFormCard();
}
