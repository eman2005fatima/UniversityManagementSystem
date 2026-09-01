#include "AttendancePanel.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/DatePicker.h"
#include "../utils/Export.h"
#include <cstring>
#include <vector>
#include <unordered_map>

extern ToastSystem g_Toast;

void ShowAttendancePanel(AppState& state, Database& db) {
    ImGui::TextColored(ImVec4(0.5f,0.5f,0.6f,1), "Dashboard");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f,0.6f,0.7f,1), "/");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0,0.78f,0.60f,1), "%s", state.currentUser.role == "student" ? "My Attendance" : "Attendance");
    ImGui::SameLine();
    if (ImGui::Button("Back", ImVec2(80, 30))) { state.sidebarSelection = 0; }
    ImGui::Dummy(ImVec2(0, 10));
    if (fontHeading) ImGui::PushFont(fontHeading);
    ImGui::TextColored(ImVec4(0.0f, 0.678f, 0.71f, 1.0f), "  ATTENDANCE");
    if (fontHeading) ImGui::PopFont();
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 10));

    if (state.currentUser.role == "admin") {
        static int adminAttCourseId = 0;
        static std::vector<std::string> adminAttCourseItems;
        static std::vector<int> adminAttCourseIds;
        static bool adminAttLoaded = false;

        if (!adminAttLoaded) {
            adminAttCourseItems.clear(); adminAttCourseIds.clear();
            MYSQL_RES* cr = db.execQuery("SELECT course_id, course_code, course_name FROM courses ORDER BY course_code");
            if (cr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(cr))) {
                    adminAttCourseItems.push_back(std::string(row[1]) + " - " + std::string(row[2]));
                    adminAttCourseIds.push_back(atoi(row[0]));
                }
                mysql_free_result(cr);
            }
            adminAttLoaded = true;
        }
        if (ImGui::Button("Refresh##adminatt", ImVec2(120,25))) adminAttLoaded = false;
        ImGui::SameLine();
        if (!adminAttCourseItems.empty()) {
            std::vector<const char*> items;
            for (auto& s : adminAttCourseItems) items.push_back(s.c_str());
            ImGui::SetNextItemWidth(300);
            ImGui::Combo("Course##adminatt", &adminAttCourseId, items.data(), (int)items.size());
        }

        if (!adminAttCourseIds.empty()) {
            int cid = adminAttCourseIds[adminAttCourseId];
            ImGui::Separator();
            if (ImGui::BeginTable("admin_att_table", 6,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
                ImVec2(0, ImGui::GetContentRegionAvail().y - 10))) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Student Name", ImGuiTableColumnFlags_WidthStretch, 1.0f);
                ImGui::TableSetupColumn("Present", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Absent", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Late", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Percentage", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableHeadersRow();

                std::string q = "SELECT s.student_id, s.name, "
                    "SUM(CASE WHEN a.status='present' THEN 1 ELSE 0 END) as p, "
                    "SUM(CASE WHEN a.status='absent' THEN 1 ELSE 0 END) as a, "
                    "SUM(CASE WHEN a.status='late' THEN 1 ELSE 0 END) as l, "
                    "COUNT(*) as t "
                    "FROM students s "
                    "JOIN enrollments e ON s.student_id = e.student_id AND e.course_id = " + std::to_string(cid) + " "
                    "LEFT JOIN attendance a ON s.student_id = a.student_id AND a.course_id = " + std::to_string(cid) + " "
                    "GROUP BY s.student_id, s.name ORDER BY s.name";
                MYSQL_RES* ar = db.execQuery(q);
                if (ar) {
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(ar))) {
                        ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                        ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                        ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                        ImGui::TableNextColumn(); ImGui::Text("%s", row[2] ? row[2] : "0");
                        ImGui::TableNextColumn(); ImGui::Text("%s", row[3] ? row[3] : "0");
                        ImGui::TableNextColumn(); ImGui::Text("%s", row[4] ? row[4] : "0");
                        ImGui::TableNextColumn();
                        int total = row[5] ? atoi(row[5]) : 0;
                        int attended = (row[2] ? atoi(row[2]) : 0) + (row[4] ? atoi(row[4]) : 0);
                        float pct = total > 0 ? (attended * 100.0f / total) : 0;
                        ImVec4 pcol = pct >= 75 ? ImVec4(0.18f,0.8f,0.44f,1) :
                                    (pct >= 60 ? ImVec4(0.95f,0.61f,0.07f,1) : ImVec4(0.91f,0.3f,0.24f,1));
                        ImGui::TextColored(pcol, "%.1f%%", pct);
                    }
                    mysql_free_result(ar);
                }
                ImGui::EndTable();
            }
        }
    } else if (state.currentUser.role == "student") {
        if (state.studentId == 0) {
            MYSQL_RES* sr = db.execQuery("SELECT student_id FROM students WHERE user_id = " + std::to_string(state.currentUser.userId));
            if (sr) { MYSQL_ROW r = mysql_fetch_row(sr); if (r) state.studentId = atoi(r[0]); mysql_free_result(sr); }
        }

        if (ImGui::Button("Export CSV", ImVec2(120, 30))) {
            std::string q = "SELECT c.course_name, "
                "SUM(CASE WHEN a.status='present' OR a.status='late' THEN 1 ELSE 0 END) as attended, "
                "COUNT(*) as total "
                "FROM enrollments e JOIN courses c ON e.course_id = c.course_id "
                "LEFT JOIN attendance a ON e.student_id = a.student_id AND e.course_id = a.course_id "
                "WHERE e.student_id = " + std::to_string(state.studentId) + " "
                "GROUP BY c.course_name";
            MYSQL_RES* atr = db.execQuery(q);
            if (atr) {
                std::ofstream file("attendance_" + getCurrentDateStr() + ".csv");
                if (file.is_open()) {
                    file << "\"Course\",\"Attended\",\"Total\",\"Percentage\",\"Status\"\n";
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(atr))) {
                        int total = row[2] ? atoi(row[2]) : 0;
                        int attended = row[1] ? atoi(row[1]) : 0;
                        float pct = total > 0 ? (attended * 100.0f / total) : 0;
                        file << "\"" << (row[0]?row[0]:"") << "\"," << attended << "," << total
                             << "," << pct << "%\n";
                    }
                    file.close();
                    g_Toast.push("CSV exported: attendance.csv", 0);
                } else g_Toast.push("CSV export failed.", 1);
                mysql_free_result(atr);
            }
        }

        if (ImGui::BeginTable("att_table", 5,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
            ImVec2(0, ImGui::GetContentRegionAvail().y - 10))) {
            ImGui::TableSetupColumn("Course", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Attended", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Percentage", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableHeadersRow();

            std::string q = "SELECT c.course_name, "
                "SUM(CASE WHEN a.status='present' OR a.status='late' THEN 1 ELSE 0 END) as attended, "
                "COUNT(*) as total "
                "FROM enrollments e JOIN courses c ON e.course_id = c.course_id "
                "LEFT JOIN attendance a ON e.student_id = a.student_id AND e.course_id = a.course_id "
                "WHERE e.student_id = " + std::to_string(state.studentId) + " "
                "GROUP BY c.course_name";
            MYSQL_RES* atr = db.execQuery(q);
            if (atr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(atr))) {
                    int total = row[2] ? atoi(row[2]) : 0;
                    int attended = row[1] ? atoi(row[1]) : 0;
                    float pct = total > 0 ? (attended * 100.0f / total) : 0;
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                    ImGui::TableNextColumn(); ImGui::Text("%d", attended);
                    ImGui::TableNextColumn(); ImGui::Text("%d", total);
                    ImGui::TableNextColumn();
                    ImVec4 pcol = pct >= 75 ? ImVec4(0.18f,0.8f,0.44f,1) :
                                (pct >= 60 ? ImVec4(0.95f,0.61f,0.07f,1) : ImVec4(0.91f,0.3f,0.24f,1));
                    ImGui::TextColored(pcol, "%.1f%%", pct);
                    ImGui::TableNextColumn();
                    if (pct >= 75) ImGui::TextColored(ImVec4(0.18f,0.8f,0.44f,1), "Good");
                    else ImGui::TextColored(ImVec4(0.91f,0.3f,0.24f,1), "At Risk");
                }
                mysql_free_result(atr);
            }
            ImGui::EndTable();
        }
    } else if (state.currentUser.role == "faculty") {
        if (state.facultyId == 0) {
            MYSQL_RES* fr = db.execQuery("SELECT faculty_id FROM faculty WHERE user_id = " + std::to_string(state.currentUser.userId));
            if (fr) { MYSQL_ROW r = mysql_fetch_row(fr); if (r) state.facultyId = atoi(r[0]); mysql_free_result(fr); }
        }

        static int selCourseAtt = 0;
        static std::vector<std::string> attCourseItems;
        static std::vector<int> attCourseIds;
        static bool attCoursesLoaded = false;

        if (!attCoursesLoaded) {
            attCourseItems.clear(); attCourseIds.clear();
            std::string q = "SELECT course_id, course_code, course_name FROM courses WHERE faculty_id = " + std::to_string(state.facultyId);
            MYSQL_RES* cr = db.execQuery(q);
            if (cr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(cr))) {
                    attCourseItems.push_back(std::string(row[1]) + " - " + std::string(row[2]));
                    attCourseIds.push_back(atoi(row[0]));
                }
                mysql_free_result(cr);
            }
            attCoursesLoaded = true;
        }
        if (ImGui::Button("Refresh##att", ImVec2(120,25))) attCoursesLoaded = false;
        ImGui::SameLine();

        if (!attCourseItems.empty()) {
            std::vector<const char*> items;
            for (auto& s : attCourseItems) items.push_back(s.c_str());
            ImGui::SetNextItemWidth(300);
            ImGui::Combo("Course##att", &selCourseAtt, items.data(), (int)items.size());

            static int attYear = 2025, attMonth = 6, attDay = 6;
            ImGui::SameLine();
            if (DatePickerPopup("##datepick", &attYear, &attMonth, &attDay)) {}
            char dateStr[64];
            snprintf(dateStr, 64, "%04d-%02d-%02d", attYear, attMonth, attDay);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5f,0.5f,0.6f,1), "  Date: %s", dateStr);

            if (attCourseIds.size() > 0) {
                int cid = attCourseIds[selCourseAtt];

                // Bulk action buttons
                if (ImGui::Button("Mark All Present", ImVec2(160, 28))) {
                    std::string q = "SELECT s.student_id FROM enrollments e "
                                   "JOIN students s ON e.student_id = s.student_id WHERE e.course_id = " + std::to_string(cid);
                    MYSQL_RES* sr = db.execQuery(q);
                    if (sr) {
                        db.beginTransaction();
                        MYSQL_ROW row;
                        while ((row = mysql_fetch_row(sr))) {
                            db.query("INSERT INTO attendance (student_id, course_id, date, status) VALUES ("
                                    + std::string(row[0]) + ", " + std::to_string(cid) + ", '"
                                    + dateStr + "', 'present') ON DUPLICATE KEY UPDATE status = 'present'");
                        }
                        db.commit();
                        mysql_free_result(sr);
                        g_Toast.push("All marked present!", 0);
                    }
                }
                ImGui::SameLine(0, 10);
                if (ImGui::Button("Mark All Absent", ImVec2(160, 28))) {
                    std::string q = "SELECT s.student_id FROM enrollments e "
                                   "JOIN students s ON e.student_id = s.student_id WHERE e.course_id = " + std::to_string(cid);
                    MYSQL_RES* sr = db.execQuery(q);
                    if (sr) {
                        db.beginTransaction();
                        MYSQL_ROW row;
                        while ((row = mysql_fetch_row(sr))) {
                            db.query("INSERT INTO attendance (student_id, course_id, date, status) VALUES ("
                                    + std::string(row[0]) + ", " + std::to_string(cid) + ", '"
                                    + dateStr + "', 'absent') ON DUPLICATE KEY UPDATE status = 'absent'");
                        }
                        db.commit();
                        mysql_free_result(sr);
                        g_Toast.push("All marked absent!", 0);
                    }
                }
                ImGui::SameLine(0, 10);
                if (ImGui::Button("Batch Save", ImVec2(140, 28))) {
                    g_Toast.push("Batch saved!", 0);
                }

                if (ImGui::BeginTable("att_mark", 4,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
                    ImVec2(0, ImGui::GetContentRegionAvail().y - 60))) {
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f);
                    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                    ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                    ImGui::TableHeadersRow();

                    std::string q = "SELECT s.student_id, s.name FROM enrollments e "
                        "JOIN students s ON e.student_id = s.student_id WHERE e.course_id = " + std::to_string(cid);
                    MYSQL_RES* sr = db.execQuery(q);
                    if (sr) {
                        static std::unordered_map<int,int> attStatus;
                        MYSQL_ROW row;
                        while ((row = mysql_fetch_row(sr))) {
                            ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                            ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                            ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                            ImGui::TableNextColumn();
                            int sid = atoi(row[0]);
                            ImGui::PushID(sid);
                            ImGui::SetNextItemWidth(110);
                            ImGui::Combo("##s", &attStatus[sid], "Present\0Absent\0Late\0");
                            ImGui::PopID();
                            ImGui::TableNextColumn();
                            if (ImGui::SmallButton(("Mark##" + std::to_string(sid)).c_str())) {
                                std::string fullStatus = attStatus[sid] == 0 ? "present" :
                                                       (attStatus[sid] == 1 ? "absent" : "late");
                                db.query("INSERT INTO attendance (student_id, course_id, date, status) VALUES ("
                                         + std::to_string(sid) + ", " + std::to_string(cid) + ", '"
                                         + dateStr + "', '" + fullStatus
                                         + "') ON DUPLICATE KEY UPDATE status = '" + fullStatus + "'");
                                g_Toast.push("Attendance marked!", 0);
                            }
                        }
                        mysql_free_result(sr);
                    }
                    ImGui::EndTable();
                }
            }
        }
    }
}
