#include "SignupPanel.h"
#include "../Auth.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/Validators.h"
#include "../utils/Crypto.h"
#include <vector>
#include <cstring>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <cmath>

extern ToastSystem g_Toast;

void ShowSignupPanel(AppState& state, Database& db) {
    ImVec2 ws = ImGui::GetIO().DisplaySize;
    float winW = 580.0f;

    ImGui::SetNextWindowPos(ImVec2(ws.x * 0.5f, ws.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(winW, 700.0f), ImGuiCond_Always);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.086f, 0.106f, 0.133f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.086f, 0.106f, 0.133f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.086f, 0.106f, 0.133f, 1.0f));

    ImGui::Begin("Create Account", &state.showSignup,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoScrollbar);

    float labelW = 160.0f;
    float inputW = 320.0f;
    float formX = (winW - 40.0f - labelW - inputW) * 0.5f + 20.0f;

    ImGui::SetCursorPosY(55);

    // ── Role toggle ──
    static int signupRole = 0;
    ImGui::SetCursorPosX(formX);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::TextUnformatted("Role");
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 8);
    ImGui::SetCursorPosX(formX + labelW + 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (signupRole == 0) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.749f, 0.647f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.85f, 0.75f, 1));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.129f, 0.149f, 0.177f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.169f, 0.192f, 0.224f, 1));
    }
    if (ImGui::Button("Student", ImVec2(100, 34))) signupRole = 0;
    ImGui::PopStyleColor(2);
    ImGui::SameLine(0, 8);
    if (signupRole == 1) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.749f, 0.647f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.85f, 0.75f, 1));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.129f, 0.149f, 0.177f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.169f, 0.192f, 0.224f, 1));
    }
    if (ImGui::Button("Faculty", ImVec2(100, 34))) signupRole = 1;
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
    ImGui::SetCursorPosY(95);

    // ── Form fields ──
    static char signupName[128] = "", signupEmail[128] = "";
    static char signupPass[128] = "", signupConfirm[128] = "";
    static char signupPhone[64] = "", signupSecA[128] = "";
    static int signupYearIdx = 0, signupDeptIdx = 0;
    static int signupDesignationIdx = 0, signupQualIdx = 0;
    static int signupSecQIdx = 0;

    static std::vector<std::string> departments;
    static bool deptsLoaded = false;
    if (!deptsLoaded) {
        departments.clear();
        MYSQL_RES* dr = db.execQuery("SELECT DISTINCT department FROM courses ORDER BY department");
        if (dr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(dr)))
                departments.push_back(row[0] ? row[0] : "");
            mysql_free_result(dr);
        }
        if (departments.empty()) {
            departments = {"Computer Science", "Electrical Engineering", "Business Administration", "Mathematics", "Physics"};
        }
        deptsLoaded = true;
    }

    static const char* years[] = {"2022", "2023", "2024", "2025", "2026"};
    static const char* designations[] = {"Professor", "Associate Professor", "Assistant Professor", "Lecturer"};
    static const char* quals[] = {"PhD", "MS", "BS"};
    static const char* secQuestions[] = {
        "What is your favorite book?",
        "What is your mother's maiden name?",
        "What is the name of your first pet?"
    };
    std::vector<const char*> deptItems;
    for (auto& d : departments) deptItems.push_back(d.c_str());

    float rowY = 95.0f;
    auto row = [&](float y) { ImGui::SetCursorPosY(y); };

    // Full Name
    row(rowY); DrawLabeledInput("Full Name", "##sn", signupName, 128, 0, 160.0f); rowY += 52;
    row(rowY); DrawLabeledInput("Email", "##se", signupEmail, 128, 0, 160.0f); rowY += 52;
    row(rowY); DrawLabeledInput("Password", "##sp", signupPass, 128, ImGuiInputTextFlags_Password, 160.0f); rowY += 52;
    row(rowY); DrawLabeledInput("Confirm", "##sc", signupConfirm, 128, ImGuiInputTextFlags_Password, 160.0f); rowY += 52;
    row(rowY); DrawLabeledCombo("Department", "##sd", &signupDeptIdx, deptItems.data(), (int)deptItems.size(), 160.0f); rowY += 52;

    if (signupRole == 0) {
        row(rowY); DrawLabeledCombo("Year", "##sy", &signupYearIdx, years, 5, 160.0f); rowY += 52;
        row(rowY); DrawLabeledInput("Phone", "##sph", signupPhone, 64, 0, 160.0f); rowY += 52;
    } else {
        row(rowY); DrawLabeledCombo("Designation", "##sdes", &signupDesignationIdx, designations, 4, 160.0f); rowY += 52;
        row(rowY); DrawLabeledCombo("Qualification", "##squal", &signupQualIdx, quals, 3, 160.0f); rowY += 52;
    }

    row(rowY); DrawLabeledCombo("Security Q", "##ssq", &signupSecQIdx, secQuestions, 3, 160.0f); rowY += 52;
    row(rowY); DrawLabeledInput("Answer", "##ssa", signupSecA, 128, 0, 160.0f); rowY += 52;

    // ── Error message ──
    static std::string signupError;
    if (!signupError.empty()) {
        ImGui::SetCursorPosY(rowY);
        ImGui::SetCursorPosX(formX + labelW + 8.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.973f, 0.318f, 0.286f, 1));
        ImGui::TextUnformatted(signupError.c_str());
        ImGui::PopStyleColor();
        rowY += 28;
    }

    // ── Buttons ──
    ImGui::SetCursorPosY(rowY + 12.0f);
    float btnW = 150.0f;
    float btnGap = 12.0f;
    float btnsTotal = btnW * 2 + btnGap;
    ImGui::SetCursorPosX((winW - btnsTotal) * 0.5f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.85f, 0.75f, 1));
    if (ImGui::Button("REGISTER", ImVec2(btnW, 42))) {
        signupError.clear();
        if (strcmp(signupPass, signupConfirm) != 0) {
            signupError = "Passwords do not match.";
            g_Toast.push("Passwords do not match.", 1);
        } else {
            std::string err;
            if (signupRole == 0) {
                err = Auth::signupStudent(db, trim(signupName), trim(signupEmail), signupPass,
                    departments.empty() ? "Computer Science" : departments[signupDeptIdx],
                    atoi(years[signupYearIdx]), trim(signupPhone),
                    secQuestions[signupSecQIdx], trim(signupSecA));
            } else {
                err = Auth::signupFaculty(db, trim(signupName), trim(signupEmail), signupPass,
                    departments.empty() ? "Computer Science" : departments[signupDeptIdx],
                    designations[signupDesignationIdx], quals[signupQualIdx],
                    secQuestions[signupSecQIdx], trim(signupSecA));
            }
            if (err.empty()) {
                g_Toast.push("Account created! Please login.", 0);
                state.showSignup = false;
                state.showLogin = true;
                signupError.clear();
                deptsLoaded = false;
                memset(signupName, 0, sizeof(signupName));
                memset(signupEmail, 0, sizeof(signupEmail));
                memset(signupPass, 0, sizeof(signupPass));
                memset(signupConfirm, 0, sizeof(signupConfirm));
                memset(signupSecA, 0, sizeof(signupSecA));
            } else {
                signupError = err;
                g_Toast.push(err, 1);
            }
        }
    }
    ImGui::PopStyleColor(2);
    ImGui::SameLine(0, 12);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.129f, 0.149f, 0.177f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.169f, 0.192f, 0.224f, 1));
    if (ImGui::Button("CANCEL", ImVec2(btnW, 42))) {
        state.showSignup = false;
        state.showLogin = true;
        signupError.clear();
        deptsLoaded = false;
    }
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    ImGui::End();
    ImGui::PopStyleColor(3);
}
