#include "CoursesPanel.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/Export.h"
#include <cstring>
#include <vector>

extern ToastSystem g_Toast;

void ShowCoursesPanel(AppState& state, Database& db) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::TextUnformatted(state.currentUser.role == "faculty" ? "MY COURSES" : "MANAGE COURSES");
    ImGui::PopStyleColor();
    DrawAccentLine(40.0f, 3.0f);
    ImGui::Dummy(ImVec2(0, 8));

    if (state.currentUser.role == "admin") {
        // ── Add New Course ──
        static char newCourseCode[64] = "", newCourseName[128] = "", newCourseDept[64] = "";
        static int newCourseCredits = 3, newCourseSem = 1;
        BeginFormCard();
        DrawSectionHeader("ADD NEW COURSE");
        DrawLabeledInput("Course Code", "##cc", newCourseCode, 64);
        DrawLabeledInput("Course Name", "##cn", newCourseName, 128);
        DrawLabeledInputInt("Credit Hours", "##cch", &newCourseCredits);
        DrawLabeledInput("Department", "##cd", newCourseDept, 64);
        DrawLabeledInputInt("Semester", "##csem", &newCourseSem);

        ImGui::SetCursorPosX(180);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.247f, 0.725f, 0.314f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.247f, 0.725f, 0.314f, 1));
        if (ImGui::Button("Add Course", ImVec2(180, 38))) {
            std::string q = "INSERT INTO courses (course_code, course_name, credit_hours, department, semester) VALUES ('"
                           + db.escape(newCourseCode) + "', '" + db.escape(newCourseName) + "', "
                           + std::to_string(newCourseCredits) + ", '" + db.escape(newCourseDept) + "', " + std::to_string(newCourseSem) + ")";
            if (db.query(q)) {
                g_Toast.push("Course added!", 0);
                memset(newCourseCode,0,sizeof(newCourseCode));
                memset(newCourseName,0,sizeof(newCourseName));
                memset(newCourseDept,0,sizeof(newCourseDept));
            } else {
                g_Toast.push("Could not add course.", 1);
            }
        }
        ImGui::PopStyleColor(2);
        EndFormCard();

        ImGui::Dummy(ImVec2(0, 12));

        // ── Assign Faculty ──
        BeginFormCard();
        DrawSectionHeader("ASSIGN FACULTY");
        static int assignCourseIdx = 0, assignFacultyIdx = 0;
        static std::vector<std::string> courseList, facultyList;
        static std::vector<int> courseIdList, facultyIdList;
        static bool listsLoaded = false;

        if (!listsLoaded) {
            courseList.clear(); courseIdList.clear();
            facultyList.clear(); facultyIdList.clear();
            MYSQL_RES* cr = db.execQuery("SELECT course_id, course_code, course_name FROM courses ORDER BY course_code");
            if (cr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(cr))) {
                    courseList.push_back(std::string(row[1]) + " - " + std::string(row[2]));
                    courseIdList.push_back(atoi(row[0]));
                }
                mysql_free_result(cr);
            }
            MYSQL_RES* fr = db.execQuery("SELECT faculty_id, name FROM faculty ORDER BY name");
            if (fr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(fr))) {
                    facultyList.push_back(row[1] ? row[1] : "");
                    facultyIdList.push_back(atoi(row[0]));
                }
                mysql_free_result(fr);
            }
            listsLoaded = true;
        }

        float avail = ImGui::GetContentRegionAvail().x;
        std::vector<const char*> citems, fitems;
        for (auto& s : courseList) citems.push_back(s.c_str());
        for (auto& s : facultyList) fitems.push_back(s.c_str());

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
        ImGui::SetCursorPosX(20);
        ImGui::TextUnformatted("Course");
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 8);
        ImGui::SetCursorPosX(180);
        ImGui::SetNextItemWidth(avail - 400);
        if (!courseList.empty()) ImGui::Combo("##ac", &assignCourseIdx, citems.data(), (int)citems.size());
        ImGui::Dummy(ImVec2(0, 6));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
        ImGui::SetCursorPosX(20);
        ImGui::TextUnformatted("Faculty");
        ImGui::PopStyleColor();
        ImGui::SameLine(0, 8);
        ImGui::SetCursorPosX(180);
        ImGui::SetNextItemWidth(avail - 400);
        if (!facultyList.empty()) ImGui::Combo("##af", &assignFacultyIdx, fitems.data(), (int)fitems.size());
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::SetCursorPosX(180);
        if (ImGui::Button("Refresh", ImVec2(110, 30))) listsLoaded = false;
        ImGui::SameLine(0, 10);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.122f, 0.435f, 0.922f, 0.8f));
        if (ImGui::Button("Assign", ImVec2(110, 30)) && !courseIdList.empty() && !facultyIdList.empty()) {
            if (db.query("UPDATE courses SET faculty_id = " + std::to_string(facultyIdList[assignFacultyIdx]) +
                         " WHERE course_id = " + std::to_string(courseIdList[assignCourseIdx]))) {
                g_Toast.push("Faculty assigned!", 0);
            } else g_Toast.push("Assignment failed.", 1);
            listsLoaded = false;
        }
        ImGui::PopStyleColor();
        EndFormCard();

        ImGui::Dummy(ImVec2(0, 12));

        // ── Export ──
        BeginFormCard();
        DrawSectionHeader("EXPORT");
        ImGui::SetCursorPosX(180);
        if (ImGui::Button("Export CSV", ImVec2(140, 34))) {
            MYSQL_RES* cr = db.execQuery("SELECT c.course_id, c.course_code, c.course_name, c.credit_hours, c.department, c.semester, COALESCE(f.name, 'Not Assigned') FROM courses c LEFT JOIN faculty f ON c.faculty_id = f.faculty_id ORDER BY c.course_code");
            if (cr) {
                std::ofstream file("courses_" + getCurrentDateStr() + ".csv");
                if (file.is_open()) {
                    file << "\"ID\",\"Code\",\"Name\",\"Credits\",\"Department\",\"Semester\",\"Faculty\"\n";
                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(cr))) {
                        file << row[0] << ",\"" << (row[1]?row[1]:"") << "\",\"" << (row[2]?row[2]:"") << "\"," << (row[3]?row[3]:"0") << ",\"" << (row[4]?row[4]:"") << "\"," << (row[5]?row[5]:"0") << ",\"" << (row[6]?row[6]:"") << "\"\n";
                    }
                    file.close();
                    g_Toast.push("CSV exported.", 0);
                } else g_Toast.push("CSV export failed.", 1);
                mysql_free_result(cr);
            }
        }
        EndFormCard();
    }

    // ── Courses Table ──
    ImGui::Dummy(ImVec2(0, 12));

    int colCount = state.currentUser.role == "student" ? 4 : 5;
    float widths[] = {90.0f, 200.0f, 100.0f, 130.0f, 160.0f};
    const char* headers[] = {"Code", "Name", "Credits", "Department", "Faculty"};

    BeginTableStyled();
    if (ImGui::BeginTable("courses_table", colCount,
        ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX,
        ImVec2(0, ImGui::GetContentRegionAvail().y))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        for (int c = 0; c < colCount; c++)
            ImGui::TableSetupColumn(headers[c], ImGuiTableColumnFlags_WidthFixed, widths[c]);
        ImGui::TableHeadersRow();

        std::string q;
        if (state.currentUser.role == "faculty") {
            if (state.facultyId == 0) {
                MYSQL_RES* fr = db.execQuery("SELECT faculty_id FROM faculty WHERE user_id = " + std::to_string(state.currentUser.userId));
                if (fr) { MYSQL_ROW r = mysql_fetch_row(fr); if (r) state.facultyId = atoi(r[0]); mysql_free_result(fr); }
            }
            q = "SELECT c.course_code, c.course_name, c.credit_hours, c.department, COALESCE(f.name, 'Not Assigned') FROM courses c LEFT JOIN faculty f ON c.faculty_id = f.faculty_id WHERE c.faculty_id = " + std::to_string(state.facultyId) + " ORDER BY c.course_code";
        } else if (state.currentUser.role == "student") {
            q = "SELECT c.course_code, c.course_name, c.credit_hours, c.department FROM courses c ORDER BY c.course_code";
        } else {
            q = "SELECT c.course_code, c.course_name, c.credit_hours, c.department, COALESCE(f.name, 'Not Assigned') FROM courses c LEFT JOIN faculty f ON c.faculty_id = f.faculty_id ORDER BY c.course_code";
        }

        MYSQL_RES* cr = db.execQuery(q);
        if (cr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(cr))) {
                ImGui::TableNextRow(ImGuiTableRowFlags_None, 38.0f);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[2]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[3]);
                if (colCount > 4) {
                    ImGui::TableNextColumn();
                    std::string fn = row[4] ? row[4] : "";
                    if (fn == "Not Assigned")
                        ImGui::TextColored(ImVec4(0.973f, 0.318f, 0.286f, 1), "%s", fn.c_str());
                    else
                        ImGui::TextColored(ImVec4(0.247f, 0.725f, 0.314f, 1), "%s", fn.c_str());
                }
            }
            mysql_free_result(cr);
        }
        ImGui::EndTable();
    }
    EndTableStyled();
}
