#include "LoginPanel.h"
#include "../Auth.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/Validators.h"
#include <GLFW/glfw3.h>
#include <cmath>

extern ToastSystem g_Toast;

void ShowLoginPanel(AppState& state, Database& db) {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 ws = io.DisplaySize;
    float winW = 480.0f;
    float winH = 580.0f;

    ImGui::SetNextWindowPos(ImVec2(ws.x * 0.5f, ws.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(winW, winH), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(40, 36));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.086f, 0.106f, 0.133f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.188f, 0.212f, 0.239f, 1.0f));

    ImGui::Begin("##loginPanel", &state.showLogin,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoScrollbar);

    float formW = 480.0f - 80.0f;
    float cx = formW * 0.5f;

    // ── Title ──
    auto centerText = [&](const char* text, ImVec4 color, float y) {
        ImGui::SetCursorPosY(y);
        float tw = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((formW - tw) * 0.5f + 40);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(text);
        ImGui::PopStyleColor();
    };
    centerText("UNIVERSITY", ImVec4(0, 0.749f, 0.647f, 1), 30);
    centerText("MANAGEMENT SYSTEM", ImVec4(0.902f, 0.929f, 0.953f, 0.85f), 62);

    // ── Accent divider ──
    float t = (float)glfwGetTime();
    float pulse = 0.5f + 0.5f * sinf(t * 1.8f);
    float barW = 50.0f + 16.0f * pulse;
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 wpos = ImGui::GetWindowPos();
    float accentY = wpos.y + 85;
    dl->AddRectFilled(ImVec2(wpos.x + 240 - barW * 0.5f, accentY),
                      ImVec2(wpos.x + 240 + barW * 0.5f, accentY + 3),
                      IM_COL32(0, 191, 165, 255), 2.0f);

    ImGui::SetCursorPosY(100);
    ImGui::SetCursorPosX(cx - ImGui::CalcTextSize("Sign in to your account").x * 0.5f + 40);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::TextUnformatted("Sign in to your account");
    ImGui::PopStyleColor();

    ImGui::SetCursorPosY(135);

    // ── Role toggle buttons ──
    static const char* roleLabels[] = {"Student", "Faculty", "Admin"};
    static int selectedRole = 0;
    float roleBtnW = (formW - 16) / 3.0f;
    for (int r = 0; r < 3; r++) {
        if (r > 0) ImGui::SameLine(0, 8);
        if (selectedRole == r) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.749f, 0.647f, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.85f, 0.75f, 1));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.129f, 0.149f, 0.177f, 1));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.169f, 0.192f, 0.224f, 1));
        }
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        if (ImGui::Button(roleLabels[r], ImVec2(roleBtnW, 38))) selectedRole = r;
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }

    ImGui::SetCursorPosY(201);

    // ── Email field ──
    static char loginEmail[128] = "";
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::TextUnformatted("Email");
    ImGui::PopStyleColor();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14, 11));
    ImGui::SetNextItemWidth(formW);
    ImGui::InputText("##email", loginEmail, 128);
    ImGui::PopStyleVar(2);
    DrawFieldGlow();

    ImGui::SetCursorPosY(265);

    // ── Password field ──
    static char loginPassword[128] = "";
    static bool showPass = false;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::TextUnformatted("Password");
    ImGui::PopStyleColor();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14, 11));
    ImGui::SetNextItemWidth(formW - 44);
    ImGui::InputText("##pass", loginPassword, 128,
                     showPass ? 0 : ImGuiInputTextFlags_Password);
    ImGui::PopStyleVar(2);
    DrawFieldGlow();
    ImGui::SameLine(0, 4);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (ImGui::Button(showPass ? "Hide" : "Show", ImVec2(40, 40))) showPass = !showPass;
    ImGui::PopStyleVar();

    // ── Error message ──
    static std::string loginMsg;
    static double loginErrorTime = 0.0;
    if (!loginMsg.empty()) {
        double elapsed = glfwGetTime() - loginErrorTime;
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.973f, 0.318f, 0.286f, 1));
        ImGui::SetCursorPosX(cx - ImGui::CalcTextSize(loginMsg.c_str()).x * 0.5f + 40);
        ImGui::TextUnformatted(loginMsg.c_str());
        ImGui::PopStyleColor();
        if (elapsed < 0.3) {
            float shake = sinf((float)elapsed * 30.0f) * 4.0f * (1.0f - (float)elapsed * 3.3f);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + shake);
        }
    }

    ImGui::SetCursorPosY(375);

    // ── SIGN IN button ──
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.85f, 0.75f, 1));
    if (ImGui::Button("SIGN IN", ImVec2(formW, 48))) {
        std::string roleStr = selectedRole == 0 ? "student" : (selectedRole == 1 ? "faculty" : "admin");
        auto result = Auth::login(db, trim(loginEmail), trim(loginPassword), roleStr);
        if (result.success) {
            state.currentUser = result.session;
            state.showLogin = false;
            state.showMainApp = true;
            state.dbStatsDirty = true;
            memset(loginEmail, 0, sizeof(loginEmail));
            memset(loginPassword, 0, sizeof(loginPassword));
            loginMsg.clear();
            g_Toast.push("Welcome, " + result.session.name + "!", 0);
        } else {
            loginMsg = result.message;
            loginErrorTime = glfwGetTime();
        }
    }
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    // ── Forgot password ──
    ImGui::SetCursorPosY(436);
    ImGui::SetCursorPosX(cx - ImGui::CalcTextSize("Forgot Password?").x * 0.5f + 40);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    if (ImGui::Button("Forgot Password?")) {
        state.showLogin = false;
        state.showForgotPassword = true;
    }
    ImGui::PopStyleColor(2);

    // ── OR divider ──
    ImGui::SetCursorPosY(470);
    ImGui::SetCursorPosX(cx - 10 + 40);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::TextUnformatted("OR");
    ImGui::PopStyleColor();

    // ── CREATE ACCOUNT button ──
    ImGui::SetCursorPosY(496);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.129f, 0.149f, 0.177f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.169f, 0.192f, 0.224f, 1));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.188f, 0.212f, 0.239f, 1));
    if (ImGui::Button("CREATE NEW ACCOUNT", ImVec2(formW, 44))) {
        state.showLogin = false;
        state.showSignup = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);

    ImGui::End();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
}
