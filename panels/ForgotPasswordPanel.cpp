#include "ForgotPasswordPanel.h"
#include "../Auth.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/Validators.h"
#include <cstring>

extern ToastSystem g_Toast;

void ShowForgotPasswordPanel(AppState& state, Database& db) {
    ImVec2 ws = ImGui::GetIO().DisplaySize;
    float cx = ws.x * 0.5f;
    float cy = ws.y * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(cx - 225, cy - 210), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(450, 460), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.16f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.05f, 0.05f, 0.12f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.05f, 0.05f, 0.12f, 1.0f));

    ImGui::Begin("Reset Password", &state.showForgotPassword,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    static int fpRole = 0;
    static char fpEmail[128] = "", fpAnswer[128] = "";
    static char fpNewPass[128] = "", fpConfirmPass[128] = "";
    static std::string fpQuestion;
    static bool questionFetched = false;

    ImGui::SetCursorPosY(55);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.65f, 0.75f, 1.0f));
    ImGui::Text("Select Role:");
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 10);
    ImGui::RadioButton("Student", &fpRole, 0); ImGui::SameLine(0, 20);
    ImGui::RadioButton("Faculty", &fpRole, 1);

    ImGui::Dummy(ImVec2(0, 8));
    DrawLabeledInput("Email Address", "##fpe", fpEmail, 128, 0, 160.0f);

    ImGui::Dummy(ImVec2(0, 8));
    ImGui::SetCursorPosX(180);
    if (ImGui::Button("Fetch Security Question", ImVec2(210, 32))) {
        fpQuestion = Auth::getSecurityQuestion(db, trim(fpEmail),
                                                fpRole == 0 ? "student" : "faculty");
        if (fpQuestion.empty()) {
            g_Toast.push("No account found with this email.", 1);
            questionFetched = false;
        } else {
            questionFetched = true;
        }
    }
    ImGui::SameLine(0, 10);
    ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x - 20);
    ImGui::TextColored(ImVec4(0,0.7f,0.7f,1), "%s", fpQuestion.c_str());
    ImGui::PopTextWrapPos();

    if (questionFetched) {
        ImGui::Dummy(ImVec2(0, 8));
        DrawLabeledInput("Security Answer", "##fpa", fpAnswer, 128, 0, 160.0f);
        DrawLabeledInput("New Password", "##fpn", fpNewPass, 128, ImGuiInputTextFlags_Password, 160.0f);

        std::string strLabel = passwordStrengthLabel(fpNewPass);
        float strVal = passwordStrengthValue(fpNewPass);
        if (strVal > 0) {
            ImVec4 col = strVal <= 0.33f ? ImVec4(0.8f, 0.2f, 0.2f, 1) :
                         (strVal <= 0.66f ? ImVec4(0.8f, 0.7f, 0.2f, 1) : ImVec4(0.2f, 0.8f, 0.3f, 1));
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
            ImGui::SetCursorPosX(180);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 200);
            ImGui::ProgressBar(strVal, ImVec2(ImGui::GetContentRegionAvail().x - 200, 6), strLabel.c_str());
            ImGui::PopStyleColor();
        }

        DrawLabeledInput("Confirm Password", "##fpc", fpConfirmPass, 128, ImGuiInputTextFlags_Password, 160.0f);

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::SetCursorPosX(180);
        if (ImGui::Button("RESET PASSWORD", ImVec2(250, 42))) {
            if (strcmp(fpNewPass, fpConfirmPass) != 0) {
                g_Toast.push("Passwords do not match.", 1);
            } else {
                std::string err = Auth::forgotPassword(db, trim(fpEmail),
                    fpRole == 0 ? "student" : "faculty", trim(fpAnswer), fpNewPass);
                if (err.empty()) {
                    g_Toast.push("Password reset successfully!", 0);
                    state.showForgotPassword = false;
                    state.showLogin = true;
                    questionFetched = false;
                    fpQuestion.clear();
                    memset(fpEmail, 0, sizeof(fpEmail));
                    memset(fpAnswer, 0, sizeof(fpAnswer));
                    memset(fpNewPass, 0, sizeof(fpNewPass));
                    memset(fpConfirmPass, 0, sizeof(fpConfirmPass));
                } else {
                    g_Toast.push(err, 1);
                }
            }
        }
    }

    ImGui::SameLine(0, 10);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.22f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.19f, 0.19f, 0.28f, 1.0f));
    if (ImGui::Button("CANCEL", ImVec2(150, 42))) {
        state.showForgotPassword = false;
        state.showLogin = true;
        questionFetched = false;
        fpQuestion.clear();
    }
    ImGui::PopStyleColor(2);

    ImGui::End();
    ImGui::PopStyleColor(3);
}
