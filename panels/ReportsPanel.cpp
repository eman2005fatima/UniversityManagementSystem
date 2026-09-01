#include "ReportsPanel.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include <vector>
#include <cstring>
#include <cmath>

extern ToastSystem g_Toast;

void ShowReportsPanel(AppState& state, Database& db) {
    if (state.currentUser.role != "admin") {
        state.sidebarSelection = 0;
        return;
    }

    ImGui::TextColored(ImVec4(0.5f,0.5f,0.6f,1), "Dashboard");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f,0.6f,0.7f,1), "/");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0,0.78f,0.60f,1), "Reports");
    ImGui::SameLine();
    if (ImGui::Button("Back", ImVec2(80, 30))) { state.sidebarSelection = 0; }
    ImGui::Dummy(ImVec2(0, 10));
    if (fontHeading) ImGui::PushFont(fontHeading);
    ImGui::TextColored(ImVec4(0.0f, 0.678f, 0.71f, 1.0f), "  REPORTS");
    if (fontHeading) ImGui::PopFont();
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 10));

    // Course selector dropdown
    static int reportCourseIdx = 0;
    static std::vector<std::string> reportCourseItems;
    static std::vector<int> reportCourseIds;
    static bool reportLoaded = false;

    if (!reportLoaded) {
        reportCourseItems.clear(); reportCourseIds.clear();
        MYSQL_RES* cr = db.execQuery("SELECT course_id, course_code, course_name FROM courses ORDER BY course_code");
        if (cr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(cr))) {
                reportCourseItems.push_back(std::string(row[1]) + " - " + std::string(row[2]));
                reportCourseIds.push_back(atoi(row[0]));
            }
            mysql_free_result(cr);
        }
        reportLoaded = true;
    }

    if (ImGui::Button("Refresh Courses", ImVec2(140, 28))) reportLoaded = false;
    ImGui::SameLine(0, 10);
    if (!reportCourseItems.empty()) {
        std::vector<const char*> items;
        for (auto& s : reportCourseItems) items.push_back(s.c_str());
        ImGui::SetNextItemWidth(350);
        ImGui::Combo("Course", &reportCourseIdx, items.data(), (int)items.size());
    }

    if (!reportCourseIds.empty()) {
        int courseId = reportCourseIds[reportCourseIdx];

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));

        // Attendance report
        ImGui::TextColored(ImVec4(0.0f, 0.678f, 0.71f, 1.0f), "Attendance Report");
        ImGui::Separator();

        if (ImGui::BeginTable("att_report", 5,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
            ImVec2(0, 250))) {
            ImGui::TableSetupColumn("Student ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Present", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Percentage", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableHeadersRow();

            MYSQL_RES* ar = db.execQuery(
                "SELECT s.student_id, s.name, "
                "SUM(CASE WHEN a.status='present' OR a.status='late' THEN 1 ELSE 0 END) as present, "
                "COUNT(*) as total "
                "FROM students s "
                "JOIN enrollments e ON s.student_id = e.student_id AND e.course_id = " + std::to_string(courseId) + " "
                "LEFT JOIN attendance a ON s.student_id = a.student_id AND a.course_id = " + std::to_string(courseId) + " "
                "GROUP BY s.student_id, s.name");
            if (ar) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(ar))) {
                    int total = row[3] ? atoi(row[3]) : 0;
                    int present = row[2] ? atoi(row[2]) : 0;
                    float pct = total > 0 ? (present * 100.0f / total) : 0;
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                    ImGui::TableNextColumn(); ImGui::Text("%d", present);
                    ImGui::TableNextColumn(); ImGui::Text("%d", total);
                    ImGui::TableNextColumn();
                    ImVec4 pcol = pct >= 75 ? ImVec4(0.18f,0.8f,0.44f,1) :
                                (pct >= 60 ? ImVec4(0.95f,0.61f,0.07f,1) : ImVec4(0.91f,0.3f,0.24f,1));
                    ImGui::TextColored(pcol, "%.1f%%", pct);
                }
                mysql_free_result(ar);
            }
            ImGui::EndTable();
        }

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));

        // Grade report
        ImGui::TextColored(ImVec4(0.0f, 0.678f, 0.71f, 1.0f), "Grade Report");
        ImGui::Separator();

        if (ImGui::BeginTable("grade_report", 4,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
            ImVec2(0, 250))) {
            ImGui::TableSetupColumn("Student ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Grade", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Points", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableHeadersRow();

            MYSQL_RES* gr = db.execQuery(
                "SELECT s.student_id, s.name, e.grade, e.grade_points "
                "FROM students s "
                "JOIN enrollments e ON s.student_id = e.student_id AND e.course_id = " + std::to_string(courseId) + " "
                "ORDER BY s.student_id");
            if (gr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(gr))) {
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                    ImGui::TableNextColumn();
                    std::string grade = row[2] ? row[2] : "N/A";
                    ImVec4 gcol = ImVec4(0.9f,0.9f,0.94f,1);
                    if (grade == "A") gcol = ImVec4(0.18f,0.8f,0.44f,1);
                    else if (grade == "B") gcol = ImVec4(0.0f,0.68f,0.71f,1);
                    else if (grade == "C") gcol = ImVec4(0.95f,0.61f,0.07f,1);
                    else if (grade == "D") gcol = ImVec4(0.95f,0.5f,0.2f,1);
                    else if (grade == "F") gcol = ImVec4(0.91f,0.3f,0.24f,1);
                    ImGui::TextColored(gcol, "%s", grade.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[3] ? row[3] : "0");
                }
                mysql_free_result(gr);
            }
            ImGui::EndTable();
        }
    }
}
