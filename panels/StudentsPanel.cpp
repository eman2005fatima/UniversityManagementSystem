#include "StudentsPanel.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/Export.h"
#include "../Auth.h"
#include <cstring>
#include <vector>
#include <algorithm>

extern ToastSystem g_Toast;

void ShowStudentsPanel(AppState& state, Database& db, StudentManager& studentMgr) {
    if (state.currentUser.role == "student") {
        state.sidebarSelection = 0;
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::TextUnformatted(state.currentUser.role == "faculty" ? "MY STUDENTS" : "MANAGE STUDENTS");
    ImGui::PopStyleColor();
    DrawAccentLine(40.0f, 3.0f);
    ImGui::Dummy(ImVec2(0, 8));

    if (state.currentUser.role == "faculty") {
        if (state.facultyId == 0) {
            MYSQL_RES* fr = db.execQuery("SELECT faculty_id FROM faculty WHERE user_id = " + std::to_string(state.currentUser.userId));
            if (fr) { MYSQL_ROW r = mysql_fetch_row(fr); if (r) state.facultyId = atoi(r[0]); mysql_free_result(fr); }
        }

        BeginTableStyled();
        if (ImGui::BeginTable("faculty_students", 6,
            ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg |
            ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX,
            ImVec2(0, ImGui::GetContentRegionAvail().y))) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 180.0f);
            ImGui::TableSetupColumn("Email", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Department", ImGuiTableColumnFlags_WidthFixed, 140.0f);
            ImGui::TableSetupColumn("Semester", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableHeadersRow();

            std::string q = "SELECT DISTINCT s.student_id, s.name, s.email, s.department, s.semester, s.enrollment_year FROM students s JOIN enrollments e ON s.student_id = e.student_id JOIN courses c ON e.course_id = c.course_id WHERE c.faculty_id = " + std::to_string(state.facultyId) + " ORDER BY s.name";
            MYSQL_RES* sr = db.execQuery(q);
            if (sr) {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(sr))) {
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, 38.0f);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[0]);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[2] ? row[2] : "");
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[3] ? row[3] : "");
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[4] ? row[4] : "");
                    ImGui::TableNextColumn(); ImGui::Text("%s", row[5] ? row[5] : "");
                }
                mysql_free_result(sr);
            }
            ImGui::EndTable();
        }
        EndTableStyled();
        return;
    }

    // ── Admin view ──
    static char searchBuf[128] = "";
    static int deptFilter = 0;
    static std::vector<std::string> deptList;
    static bool deptsLoaded = false;

    if (!deptsLoaded) {
        deptList.clear();
        deptList.push_back("All Departments");
        MYSQL_RES* dr = db.execQuery("SELECT DISTINCT department FROM students ORDER BY department");
        if (dr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(dr)))
                deptList.push_back(row[0] ? row[0] : "");
            mysql_free_result(dr);
        }
        deptsLoaded = true;
    }

    // ── Top bar: search + filters ──
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::SetNextItemWidth(280.0f);
    ImGui::InputTextWithHint("##searchStudents", "Search by name or ID...", searchBuf, 128);
    ImGui::SameLine(0, 10);
    std::vector<const char*> deptItems;
    for (auto& d : deptList) deptItems.push_back(d.c_str());
    ImGui::SetNextItemWidth(170.0f);
    ImGui::Combo("##deptFilter", &deptFilter, deptItems.data(), (int)deptItems.size());
    ImGui::SameLine(0, 10);
    if (ImGui::Button("Refresh", ImVec2(100, 34))) {
        studentMgr.loadFromDB(db);
        deptsLoaded = false;
    }
    ImGui::SameLine(0, 10);
    if (ImGui::Button("Export CSV", ImVec2(120, 34))) {
        auto all = studentMgr.getAll();
        std::vector<std::string> headers = {"ID","Name","Email","Department","Semester","Year","Credits","Phone"};
        bool ok = exportCSV<Student>(all, "students", headers, [](const Student& s) {
            return std::to_string(s.studentId) + ",\"" + s.name + "\",\"" + s.email + "\",\"" +
                   s.department + "\"," + std::to_string(s.semester) + "," +
                   std::to_string(s.enrollmentYear) + "," + std::to_string(s.totalCredits) + ",\"" + s.phone + "\"";
        });
        if (ok) g_Toast.push("CSV exported: students.csv", 0);
        else g_Toast.push("CSV export failed.", 1);
    }
    ImGui::SameLine(0, 10);
    static bool showAddForm = false;
    if (ImGui::Button("Add Student", ImVec2(120, 34))) {
        showAddForm = !showAddForm;
    }
    ImGui::PopStyleVar();

    if (showAddForm) {
        ImGui::Dummy(ImVec2(0, 12));
        BeginFormCard();
        DrawSectionHeader("Personal Details");
        
        static char name[128] = "";
        static char email[128] = "";
        static char password[128] = "";
        static char phone[64] = "";
        static int yearIdx = 2; // Default 2024
        
        float labelWidth = 160.0f;
        DrawLabeledInput("Full Name *", "##add_name", name, 128, 0, labelWidth);
        DrawLabeledInput("Email *", "##add_email", email, 128, 0, labelWidth);
        DrawLabeledInput("Password *", "##add_pass", password, 128, ImGuiInputTextFlags_Password, labelWidth);
        DrawLabeledInput("Phone", "##add_phone", phone, 64, 0, labelWidth);
        
        std::vector<const char*> addDeptItems;
        for (auto& d : deptList) {
            if (d != "All Departments") addDeptItems.push_back(d.c_str());
        }
        if (addDeptItems.empty()) {
            addDeptItems = {"Computer Science", "Electrical Engineering", "Business Administration", "Mathematics", "Physics"};
        }
        static int selDeptIdx = 0;
        DrawLabeledCombo("Department *", "##add_dept", &selDeptIdx, addDeptItems.data(), (int)addDeptItems.size(), labelWidth);
        
        static const char* years[] = {"2022", "2023", "2024", "2025", "2026"};
        DrawLabeledCombo("Enroll Year *", "##add_year", &yearIdx, years, 5, labelWidth);
        
        ImGui::Dummy(ImVec2(0, 8));
        
        ImGui::SetCursorPosX(labelWidth + 28.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.000f, 0.749f, 0.647f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.000f, 0.85f, 0.75f, 1.0f));
        if (ImGui::Button("Submit Student", ImVec2(150, 38))) {
            std::string err = Auth::signupStudent(db, trim(name), trim(email), password,
                                                 addDeptItems[selDeptIdx], atoi(years[yearIdx]), trim(phone),
                                                 "What is your mother's maiden name?", "admin");
            if (err.empty()) {
                g_Toast.push("Student added successfully!", 0);
                studentMgr.loadFromDB(db);
                showAddForm = false;
                name[0] = '\0';
                email[0] = '\0';
                password[0] = '\0';
                phone[0] = '\0';
            } else {
                g_Toast.push(err, 1);
            }
        }
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine(0, 12);
        if (ImGui::Button("Cancel", ImVec2(100, 38))) {
            showAddForm = false;
        }
        EndFormCard();
        ImGui::Dummy(ImVec2(0, 16));
    }

    ImGui::Dummy(ImVec2(0, 8));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::Text("Total Students: %d", studentMgr.count());
    ImGui::PopStyleColor();
    ImGui::Dummy(ImVec2(0, 8));

    // ── Pagination ──
    static int currentPage = 0;
    const int PAGE_SIZE = 15;
    auto allStudents = studentMgr.getFiltered(searchBuf, deptFilter > 0 ? deptList[deptFilter] : "");
    int totalPages = std::max(1, ((int)allStudents.size() + PAGE_SIZE - 1) / PAGE_SIZE);
    if (currentPage >= totalPages) currentPage = totalPages - 1;
    int pageStart = currentPage * PAGE_SIZE;
    int pageEnd = std::min(pageStart + PAGE_SIZE, (int)allStudents.size());

    // ── Table ──
    BeginTableStyled();
    if (ImGui::BeginTable("students_table", 8,
        ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX,
        ImVec2(0, ImGui::GetContentRegionAvail().y - 50))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 180.0f);
        ImGui::TableSetupColumn("Student ID", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Department", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Semester", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        for (int i = pageStart; i < pageEnd; i++) {
            auto& s = allStudents[i];
            ImGui::TableNextRow(ImGuiTableRowFlags_None, 38.0f);
            ImGui::TableNextColumn(); ImGui::Text("%d", i - pageStart + 1);
            ImGui::TableNextColumn(); ImGui::Text("%s", s.name.c_str());
            ImGui::TableNextColumn(); ImGui::Text("%d", s.studentId);
            ImGui::TableNextColumn(); ImGui::Text("%s", s.department.c_str());
            ImGui::TableNextColumn(); ImGui::Text("%d", s.enrollmentYear);
            ImGui::TableNextColumn(); ImGui::Text("%d", s.semester);
            ImGui::TableNextColumn();
            DrawPill("Active", ImVec4(0.247f, 0.725f, 0.314f, 1));
            ImGui::TableNextColumn();
            if (ImGui::SmallButton(("Del##" + std::to_string(s.studentId)).c_str())) {
                if (studentMgr.deleteStudent(db, s.studentId))
                    g_Toast.push("Student deleted.", 0);
                else
                    g_Toast.push("Cannot delete: student has enrollments.", 2);
            }
        }
        ImGui::EndTable();
    }
    EndTableStyled();

    // ── Pagination bar ──
    ImGui::Dummy(ImVec2(0, 4));
    if (currentPage > 0) {
        if (ImGui::Button("< Prev", ImVec2(80, 30))) currentPage--;
        ImGui::SameLine();
    }
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::Text("  Showing %d-%d of %d  ", pageStart + 1, pageEnd, (int)allStudents.size());
    ImGui::PopStyleColor();
    ImGui::SameLine();
    if (currentPage < totalPages - 1) {
        if (ImGui::Button("Next >", ImVec2(80, 30))) currentPage++;
    }
}
