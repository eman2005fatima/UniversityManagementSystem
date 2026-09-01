#include "RegistrationPanel.h"
#include "../ToastSystem.h"
#include "../Style.h"

extern ToastSystem g_Toast;

void ShowRegistrationPanel(AppState& state, Database& db) {
    if (state.currentUser.role != "student") {
        state.sidebarSelection = 0;
        return;
    }

    ImGui::TextColored(ImVec4(0.5f,0.5f,0.6f,1), "Dashboard");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f,0.6f,0.7f,1), "/");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0,0.78f,0.60f,1), "Registration");
    ImGui::SameLine();
    if (ImGui::Button("Back", ImVec2(80, 30))) { state.sidebarSelection = 0; }
    ImGui::Dummy(ImVec2(0, 10));
    if (fontHeading) ImGui::PushFont(fontHeading);
    ImGui::TextColored(ImVec4(0.0f, 0.678f, 0.71f, 1.0f), "  COURSE REGISTRATION");
    if (fontHeading) ImGui::PopFont();
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 10));

    if (state.studentId == 0) {
        std::string q = "SELECT student_id, name, department, semester FROM students WHERE user_id = " + std::to_string(state.currentUser.userId);
        MYSQL_RES* sr = db.execQuery(q);
        if (sr) {
            MYSQL_ROW r = mysql_fetch_row(sr);
            if (r) {
                state.studentId = atoi(r[0]);
                state.myName = r[1] ? r[1] : "";
                state.myDept = r[2] ? r[2] : "";
                state.mySem = r[3] ? atoi(r[3]) : 1;
            }
            mysql_free_result(sr);
        }
    }

    ImGui::Text("Welcome, %s (Semester %d)", state.myName.c_str(), state.mySem);
    ImGui::Dummy(ImVec2(0, 8));

    // Available courses
    ImGui::TextColored(ImVec4(0.6f,0.6f,0.7f,1), "Available Courses for Semester %d", state.mySem);
    static int selectedCourseId = -1;
    std::string q = "SELECT c.course_id, c.course_code, c.course_name, c.credit_hours, "
                   "COALESCE(f.name, 'TBA') FROM courses c "
                   "LEFT JOIN faculty f ON c.faculty_id = f.faculty_id "
                   "WHERE c.semester = " + std::to_string(state.mySem) + " "
                   "AND c.course_id NOT IN (SELECT course_id FROM enrollments WHERE student_id = " + std::to_string(state.studentId) + ")";
    if (ImGui::BeginTable("avail_courses", 5,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
        ImVec2(0, 200))) {
        ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Credits", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Faculty", ImGuiTableColumnFlags_WidthFixed, 180.0f);
        ImGui::TableHeadersRow();

        MYSQL_RES* acr = db.execQuery(q);
        if (acr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(acr))) {
                int cid = atoi(row[0]);
                ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                ImGui::TableNextColumn();
                if (ImGui::RadioButton(("##sel" + std::to_string(cid)).c_str(), &selectedCourseId, cid)) {}
                ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[2]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[3]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[4]);
            }
            mysql_free_result(acr);
        }
        ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0, 5));
    if (ImGui::Button("REGISTER SELECTED COURSE", ImVec2(280, 35))) {
        if (selectedCourseId > 0) {
            std::string q = "INSERT INTO enrollments (student_id, course_id, semester, year) VALUES ("
                          + std::to_string(state.studentId) + ", " + std::to_string(selectedCourseId)
                          + ", 'Spring', YEAR(CURDATE()))";
            if (db.query(q)) {
                g_Toast.push("Course registered successfully!", 0);
                selectedCourseId = -1;
            } else {
                g_Toast.push("Registration failed. Possibly already registered.", 1);
            }
        } else {
            g_Toast.push("Select a course using the radio button first.", 2);
        }
    }

    ImGui::Dummy(ImVec2(0, 10));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 5));
    ImGui::TextColored(ImVec4(0.6f,0.6f,0.7f,1), "My Registered Courses");
    ImGui::Dummy(ImVec2(0, 5));

    if (ImGui::BeginTable("my_courses", 6,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
        ImVec2(0, ImGui::GetContentRegionAvail().y - 10))) {
        ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f);
        ImGui::TableSetupColumn("Credits", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Grade", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Faculty", ImGuiTableColumnFlags_WidthFixed, 180.0f);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableHeadersRow();

        q = "SELECT c.course_id, c.course_code, c.course_name, c.credit_hours, e.grade, "
            "COALESCE(f.name, 'TBA') FROM enrollments e JOIN courses c ON e.course_id = c.course_id "
            "LEFT JOIN faculty f ON c.faculty_id = f.faculty_id "
            "WHERE e.student_id = " + std::to_string(state.studentId);
        MYSQL_RES* ecr = db.execQuery(q);
        if (ecr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(ecr))) {
                ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[2]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[3]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[4] ? row[4] : "-");
                ImGui::TableNextColumn(); ImGui::Text("%s", row[5]);
                ImGui::TableNextColumn();
                if (ImGui::SmallButton(("Drop##" + std::to_string(atoi(row[0]))).c_str())) {
                    db.query("DELETE FROM enrollments WHERE student_id = " + std::to_string(state.studentId) +
                             " AND course_id = " + std::string(row[0]));
                    g_Toast.push("Course dropped.", 2);
                }
            }
            mysql_free_result(ecr);
        }
        ImGui::EndTable();
    }
}
