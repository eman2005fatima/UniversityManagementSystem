#include "GradesPanel.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/Export.h"
#include <cmath>
#include <cstring>
#include <vector>

extern ToastSystem g_Toast;

void ShowGradesPanel(AppState& state, Database& db) {
    ImGui::TextColored(ImVec4(0.5f,0.5f,0.6f,1), "Dashboard");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f,0.6f,0.7f,1), "/");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0,0.78f,0.60f,1), "%s", state.currentUser.role == "student" ? "My Grades" : (state.currentUser.role == "faculty" ? "Grade Entry" : "Grades"));
    ImGui::SameLine();
    if (ImGui::Button("Back", ImVec2(80, 30))) { state.sidebarSelection = 0; }
    ImGui::Dummy(ImVec2(0, 10));

    if (state.currentUser.role == "student") {
        if (fontHeading) ImGui::PushFont(fontHeading);
        ImGui::TextColored(ImVec4(0.0f, 0.678f, 0.71f, 1.0f), "  MY GRADES");
        if (fontHeading) ImGui::PopFont();
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));
        if (state.studentId == 0) {
            MYSQL_RES* sr = db.execQuery("SELECT student_id FROM students WHERE user_id = " + std::to_string(state.currentUser.userId));
            if (sr) { MYSQL_ROW r = mysql_fetch_row(sr); if (r) state.studentId = atoi(r[0]); mysql_free_result(sr); }
        }

        // CGPA
        std::string q = "SELECT SUM(c.credit_hours * e.grade_points) / SUM(c.credit_hours) as cgpa "
                       "FROM enrollments e JOIN courses c ON e.course_id = c.course_id "
                       "WHERE e.student_id = " + std::to_string(state.studentId) + " AND e.grade IS NOT NULL";
        MYSQL_RES* cgr = db.execQuery(q);
        if (cgr) {
            MYSQL_ROW r = mysql_fetch_row(cgr);
            if (r && r[0]) {
                float cgpa = atof(r[0]);
                ImGui::TextColored(ImVec4(0,0.68f,0.71f,1), "CGPA: %.2f / 4.0", cgpa);
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, cgpa >= 3.0f ? ImVec4(0.2f,0.8f,0.3f,1) :
                                     (cgpa >= 2.0f ? ImVec4(0.8f,0.7f,0.2f,1) : ImVec4(0.8f,0.2f,0.2f,1)));
                ImGui::ProgressBar(cgpa / 4.0f, ImVec2(ImGui::GetContentRegionAvail().x, 20), "");
                ImGui::PopStyleColor();
            } else ImGui::TextUnformatted("No grades available yet.");
            mysql_free_result(cgr);
        }

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 5));

        if (ImGui::Button("Export CSV", ImVec2(120, 30))) {
            std::string q2 = "SELECT c.course_code, c.course_name, c.credit_hours, e.grade, e.grade_points "
                           "FROM enrollments e JOIN courses c ON e.course_id = c.course_id "
                           "WHERE e.student_id = " + std::to_string(state.studentId) + " AND e.grade IS NOT NULL ORDER BY c.course_code";
            MYSQL_RES* gr = db.execQuery(q2);
            if (gr) {
                std::ofstream file("grades_" + getCurrentDateStr() + ".csv");
                if (file.is_open()) {
                    file << "\"Code\",\"Name\",\"Credits\",\"Grade\",\"Points\"\n";
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(gr))) {
                        file << "\"" << (row[0]?row[0]:"") << "\",\"" << (row[1]?row[1]:"") << "\","
                             << (row[2]?row[2]:"0") << ",\"" << (row[3]?row[3]:"") << "\","
                             << (row[4]?row[4]:"0") << "\n";
                    }
                    file.close();
                    g_Toast.push("CSV exported: grades.csv", 0);
                } else g_Toast.push("CSV export failed.", 1);
                mysql_free_result(gr);
            }
        }

        if (ImGui::BeginTable("grades_table", 5,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
            ImVec2(0, ImGui::GetContentRegionAvail().y - 10))) {
            ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableSetupColumn("Credits", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Grade", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Points", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableHeadersRow();

            q = "SELECT c.course_code, c.course_name, c.credit_hours, e.grade, e.grade_points "
                "FROM enrollments e JOIN courses c ON e.course_id = c.course_id "
                "WHERE e.student_id = " + std::to_string(state.studentId) + " AND e.grade IS NOT NULL ORDER BY c.course_code";
            MYSQL_RES* gr = db.execQuery(q);
            if (gr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(gr))) {
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[2]);
                    ImGui::TableNextColumn();
                    std::string grade = row[3] ? row[3] : "-";
                    ImVec4 gcol = ImVec4(0.9f,0.9f,0.94f,1);
                    if (grade == "A") gcol = ImVec4(0.18f,0.8f,0.44f,1);
                    else if (grade == "B") gcol = ImVec4(0.0f,0.68f,0.71f,1);
                    else if (grade == "C") gcol = ImVec4(0.95f,0.61f,0.07f,1);
                    else if (grade == "D") gcol = ImVec4(0.95f,0.5f,0.2f,1);
                    else if (grade == "F") gcol = ImVec4(0.91f,0.3f,0.24f,1);
                    ImGui::TextColored(gcol, "%s", grade.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[4] ? row[4] : "0");
                }
                mysql_free_result(gr);
            }
            ImGui::EndTable();
        }
    } else if (state.currentUser.role == "faculty") {
        if (fontHeading) ImGui::PushFont(fontHeading);
        ImGui::TextColored(ImVec4(0.0f, 0.678f, 0.71f, 1.0f), "  GRADE ENTRY");
        if (fontHeading) ImGui::PopFont();
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));

        if (state.facultyId == 0) {
            MYSQL_RES* fr = db.execQuery("SELECT faculty_id FROM faculty WHERE user_id = " + std::to_string(state.currentUser.userId));
            if (fr) { MYSQL_ROW r = mysql_fetch_row(fr); if (r) state.facultyId = atoi(r[0]); mysql_free_result(fr); }
        }

        static int selectedCourseForGrade = 0;
        static std::vector<std::string> courseItems;
        static std::vector<int> courseIds;
        static bool coursesLoaded = false;

        if (!coursesLoaded) {
            courseItems.clear(); courseIds.clear();
            std::string q = "SELECT course_id, course_code, course_name FROM courses WHERE faculty_id = " + std::to_string(state.facultyId);
            MYSQL_RES* cr = db.execQuery(q);
            if (cr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(cr))) {
                    courseItems.push_back(std::string(row[1]) + " - " + std::string(row[2]));
                    courseIds.push_back(atoi(row[0]));
                }
                mysql_free_result(cr);
            }
            coursesLoaded = true;
        }
        if (ImGui::Button("Refresh Courses", ImVec2(150,25))) coursesLoaded = false;

        if (!courseItems.empty()) {
            std::vector<const char*> cItems;
            for (auto& s : courseItems) cItems.push_back(s.c_str());
            ImGui::Combo("Course", &selectedCourseForGrade, cItems.data(), (int)cItems.size());
            int cid = courseIds[selectedCourseForGrade];

            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 5));

            // Grade summary bar chart
            {
                std::string q = "SELECT e.grade, COUNT(*) FROM enrollments e WHERE e.course_id = " + std::to_string(cid) + " AND e.grade IS NOT NULL GROUP BY e.grade ORDER BY e.grade";
                MYSQL_RES* gsr = db.execQuery(q);
                if (gsr) {
                    ImGui::Text("Grade Distribution");
                    ImDrawList* dl = ImGui::GetWindowDrawList();
                    ImVec2 pos = ImGui::GetCursorScreenPos();
                    float barW = 40.0f;
                    float maxH = 80.0f;
                    float maxCount = 1;
                    std::vector<std::pair<std::string,int>> gradeCounts;
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(gsr))) {
                        int cnt = row[1] ? atoi(row[1]) : 0;
                        gradeCounts.push_back({row[0] ? row[0] : "", cnt});
                        if (cnt > maxCount) maxCount = cnt;
                    }
                    mysql_free_result(gsr);
                    float startX = pos.x + 20;
                    ImU32 gradeColors[] = {IM_COL32(0,173,181,255), IM_COL32(46,204,113,255),
                                          IM_COL32(243,156,18,255), IM_COL32(231,76,60,255), IM_COL32(155,89,182,255)};
                    for (size_t i = 0; i < gradeCounts.size(); i++) {
                        float h = (gradeCounts[i].second / maxCount) * maxH;
                        float x = startX + i * (barW + 10);
                        dl->AddRectFilled(ImVec2(x, pos.y + maxH - h),
                                          ImVec2(x + barW, pos.y + maxH),
                                          gradeColors[i % 5], 4.0f);
                        dl->AddText(ImVec2(x + 5, pos.y + maxH + 4),
                                   IM_COL32(200,200,200,255), gradeCounts[i].first.c_str());
                        dl->AddText(ImVec2(x + 5, pos.y + maxH - h - 14),
                                   IM_COL32(200,200,200,255), std::to_string(gradeCounts[i].second).c_str());
                    }
                    ImGui::Dummy(ImVec2(0, maxH + 24));
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(0, 5));
                }
            }

            if (ImGui::BeginTable("grade_assign", 5,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
                ImVec2(0, ImGui::GetContentRegionAvail().y - 60))) {
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 1.0f);
                ImGui::TableSetupColumn("Current Grade", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("New Grade", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Submit", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                ImGui::TableHeadersRow();

                std::string q = "SELECT s.student_id, s.name, e.grade, e.enrollment_id FROM enrollments e "
                    "JOIN students s ON e.student_id = s.student_id WHERE e.course_id = " + std::to_string(cid);
                MYSQL_RES* sr = db.execQuery(q);
                if (sr) {
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(sr))) {
                        ImGui::TableNextRow(ImGuiTableRowFlags_None, 34.0f);
                        ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                        ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                        ImGui::TableNextColumn();
                        std::string cg = row[2] ? row[2] : "N/A";
                        ImGui::Text("%s", cg.c_str());
                        ImGui::TableNextColumn();
                        static std::unordered_map<int,int> gradeChoice;
                        int eid = atoi(row[3]);
                        ImGui::PushID(eid);
                        ImGui::SetNextItemWidth(80);
                        ImGui::Combo("##g", &gradeChoice[eid], "F\0D\0C\0B\0A\0");
                        ImGui::PopID();
                        ImGui::TableNextColumn();
                        if (ImGui::Button("Submit Grade", ImVec2(100, 28))) {
                            const char* grades[] = {"F","D","C","B","A"};
                            float gps[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
                            int idx = gradeChoice[eid];
                            if (idx >= 0 && idx < 5) {
                                db.query("UPDATE enrollments SET grade = '" + std::string(grades[idx]) + "', grade_points = "
                                         + std::to_string(gps[idx]) + " WHERE enrollment_id = " + std::to_string(eid));
                                g_Toast.push("Grade assigned!", 0);
                            }
                        }
                    }
                    mysql_free_result(sr);
                }
                ImGui::EndTable();
            }
        } else {
            ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "No courses assigned to you.");
        }
    }
}
