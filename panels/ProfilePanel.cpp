#include "ProfilePanel.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/Validators.h"
#include <cstring>
#include <cmath>

extern ToastSystem g_Toast;

void ShowProfilePanel(AppState& state, Database& db) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::TextUnformatted("MY PROFILE");
    ImGui::PopStyleColor();
    DrawAccentLine(40.0f, 3.0f);
    ImGui::Dummy(ImVec2(0, 12));

    // Avatar with initials
    std::string initials;
    if (!state.currentUser.name.empty()) {
        initials += toupper(state.currentUser.name[0]);
        size_t sp = state.currentUser.name.find(' ');
        if (sp != std::string::npos && sp + 1 < state.currentUser.name.length())
            initials += toupper(state.currentUser.name[sp + 1]);
    }

    ImDrawList* dl = ImGui::GetWindowDrawList();
    float avail = ImGui::GetContentRegionAvail().x;
    float avatarSize = 80.0f;
    float avatarX = (avail - avatarSize) * 0.5f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    pos.x += avatarX;
    ImVec2 avatarCenter(pos.x + avatarSize * 0.5f, pos.y + avatarSize * 0.5f);
    dl->AddCircleFilled(avatarCenter, avatarSize * 0.5f, IM_COL32(0, 191, 165, 255), 32);
    dl->AddText(ImVec2(avatarCenter.x - ImGui::CalcTextSize(initials.c_str()).x * 0.5f,
                       avatarCenter.y - ImGui::CalcTextSize(initials.c_str()).y * 0.5f),
                IM_COL32(255, 255, 255, 255), initials.c_str());
    ImGui::SetCursorPosX(avatarX);
    ImGui::Dummy(ImVec2(avatarSize, avatarSize * 0.7f));

    // Profile info
    static bool editing = false;
    static char editName[128] = "", editPhone[64] = "";

    if (!editing) {
        BeginFormCard();
        DrawSectionHeader("PROFILE INFO");
        ImGui::Dummy(ImVec2(0, 6));
        auto fieldLabel = [](const char* label) {
            ImGui::SetCursorPosX(20);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
            ImGui::TextUnformatted(label);
            ImGui::PopStyleColor();
            ImGui::SameLine(); ImGui::SetCursorPosX(130);
        };
        fieldLabel("Name:");
        ImGui::Text("%s", state.currentUser.name.c_str());
        fieldLabel("Email:");
        ImGui::Text("%s", state.currentUser.email.c_str());
        fieldLabel("Role:");
        std::string r = state.currentUser.role;
        if (!r.empty()) r[0] = toupper(r[0]);
        ImGui::Text("%s", r.c_str());

        if (state.currentUser.role == "student") {
            std::string q = "SELECT department, enrollment_year, phone, semester, total_credits FROM students WHERE user_id = " + std::to_string(state.currentUser.userId);
            MYSQL_RES* sr = db.execQuery(q);
            if (sr) {
                MYSQL_ROW row = mysql_fetch_row(sr);
                if (row) {
                    fieldLabel("Department:");
                    ImGui::Text("%s", row[0] ? row[0] : "");
                    fieldLabel("Semester:");
                    ImGui::Text("%s", row[3] ? row[3] : "");

                    // CGPA
                    std::string cgQ = "SELECT SUM(c.credit_hours * e.grade_points) / SUM(c.credit_hours) FROM enrollments e JOIN courses c ON e.course_id = c.course_id WHERE e.student_id = (SELECT student_id FROM students WHERE user_id = " + std::to_string(state.currentUser.userId) + ") AND e.grade IS NOT NULL";
                    MYSQL_RES* cgr = db.execQuery(cgQ);
                    if (cgr) {
                        MYSQL_ROW cgRow = mysql_fetch_row(cgr);
                        if (cgRow && cgRow[0]) {
                            float cgpa = atof(cgRow[0]);
                            ImGui::Dummy(ImVec2(0, 8));
                            fieldLabel("CGPA:");
                            ImGui::PushStyleColor(ImGuiCol_Text,
                                cgpa >= 3.0f ? ImVec4(0.247f,0.725f,0.314f,1) :
                                (cgpa >= 2.0f ? ImVec4(0.824f,0.573f,0.133f,1) : ImVec4(0.973f,0.318f,0.286f,1)));
                            ImGui::Text("%.2f / 4.0", cgpa);
                            ImGui::PopStyleColor();
                        }
                        mysql_free_result(cgr);
                    }
                }
                mysql_free_result(sr);
            }
        } else if (state.currentUser.role == "faculty") {
            std::string q = "SELECT department, designation, qualification FROM faculty WHERE user_id = " + std::to_string(state.currentUser.userId);
            MYSQL_RES* fr = db.execQuery(q);
            if (fr) {
                MYSQL_ROW row = mysql_fetch_row(fr);
                if (row) {
                    fieldLabel("Department:");
                    ImGui::Text("%s", row[0] ? row[0] : "");
                    fieldLabel("Designation:");
                    ImGui::Text("%s", row[1] ? row[1] : "");
                }
                mysql_free_result(fr);
            }
        }

        ImGui::Dummy(ImVec2(0, 12));
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.749f, 0.647f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.85f, 0.75f, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        if (ImGui::Button("Edit Profile", ImVec2(160, 38))) {
            editing = true;
            strncpy(editName, state.currentUser.name.c_str(), 127);
            editName[127] = '\0';
            editPhone[0] = '\0';
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
        EndFormCard();
    } else {
        BeginFormCard();
        DrawSectionHeader("EDIT PROFILE");
        DrawLabeledInput("Full Name", "##editName", editName, 128);
        DrawLabeledInput("Phone Number", "##editPhone", editPhone, 64);
        ImGui::SetCursorPosX(180);
        if (ImGui::Button("Save", ImVec2(140, 38))) {
            std::string table = state.currentUser.role == "student" ? "students" : "faculty";
            std::string q = "UPDATE " + table + " SET name = '" + db.escape(editName) + "'";
            if (strlen(editPhone) > 0) q += ", phone = '" + db.escape(editPhone) + "'";
            q += " WHERE user_id = " + std::to_string(state.currentUser.userId);
            if (db.query(q)) {
                state.currentUser.name = editName;
                g_Toast.push("Profile updated!", 0);
                editing = false;
            } else g_Toast.push("Failed to update profile.", 1);
        }
        ImGui::SameLine(0, 10);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.129f, 0.149f, 0.177f, 1));
        if (ImGui::Button("Cancel", ImVec2(140, 38))) editing = false;
        ImGui::PopStyleColor();
        EndFormCard();
    }
}
