#include "DashboardPanel.h"
#include "../Style.h"
#include "../ToastSystem.h"
#include <cmath>
#include <cstring>

extern ToastSystem g_Toast;

void ShowDashboardPanel(AppState& state, Database& db) {
    float avail = ImGui::GetContentRegionAvail().x;

    // ── Page title ──
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::TextUnformatted("DASHBOARD");
    ImGui::PopStyleColor();
    DrawAccentLine(40.0f, 3.0f);
    ImGui::Dummy(ImVec2(0, 20));

    // ── Refresh stats ──
    if (state.dbStatsDirty) {
        MYSQL_RES* sq = db.execQuery("SELECT COUNT(*) FROM students");
        if (sq) { MYSQL_ROW r = mysql_fetch_row(sq); if (r) state.cachedStudentCount = atoi(r[0]); mysql_free_result(sq); }
        MYSQL_RES* fq = db.execQuery("SELECT COUNT(*) FROM faculty");
        if (fq) { MYSQL_ROW r = mysql_fetch_row(fq); if (r) state.cachedFacultyCount = atoi(r[0]); mysql_free_result(fq); }
        MYSQL_RES* cq = db.execQuery("SELECT COUNT(*) FROM courses");
        if (cq) { MYSQL_ROW r = mysql_fetch_row(cq); if (r) state.cachedCourseCount = atoi(r[0]); mysql_free_result(cq); }
        MYSQL_RES* eq = db.execQuery("SELECT COUNT(*) FROM enrollments");
        if (eq) { MYSQL_ROW r = mysql_fetch_row(eq); if (r) state.cachedEnrollmentCount = atoi(r[0]); mysql_free_result(eq); }
        state.dbStatsDirty = false;
    }

    // ── Stat cards row ──
    float cardGap = 16.0f;
    float cardW = (avail - cardGap * 3) / 4.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(cardGap, 12));
    ImGui::Columns(4, "statCards", false);
    ImGui::SetColumnWidth(0, cardW);
    ImGui::SetColumnWidth(1, cardW);
    ImGui::SetColumnWidth(2, cardW);
    ImGui::SetColumnWidth(3, cardW);

    if (state.currentUser.role == "admin") {
        DrawStatCard("Total Students", std::to_string(state.cachedStudentCount).c_str(), ImVec4(0.122f, 0.435f, 0.922f, 1));
        ImGui::NextColumn();
        DrawStatCard("Total Faculty", std::to_string(state.cachedFacultyCount).c_str(), ImVec4(0.247f, 0.725f, 0.314f, 1));
        ImGui::NextColumn();
        DrawStatCard("Total Courses", std::to_string(state.cachedCourseCount).c_str(), ImVec4(0.824f, 0.573f, 0.133f, 1));
        ImGui::NextColumn();
        DrawStatCard("Enrollments", std::to_string(state.cachedEnrollmentCount).c_str(), ImVec4(0.969f, 0.471f, 0.729f, 1));
    } else if (state.currentUser.role == "faculty") {
        if (state.facultyId == 0) {
            MYSQL_RES* fr = db.execQuery("SELECT faculty_id FROM faculty WHERE user_id = " + std::to_string(state.currentUser.userId));
            if (fr) { MYSQL_ROW r = mysql_fetch_row(fr); if (r) state.facultyId = atoi(r[0]); mysql_free_result(fr); }
        }
        int myCourses = 0, totalStudents = 0, pendingGrades = 0;
        MYSQL_RES* mcq = db.execQuery("SELECT COUNT(*) FROM courses WHERE faculty_id = " + std::to_string(state.facultyId));
        if (mcq) { MYSQL_ROW r = mysql_fetch_row(mcq); if (r) myCourses = atoi(r[0]); mysql_free_result(mcq); }
        MYSQL_RES* tsq = db.execQuery("SELECT COUNT(DISTINCT e.student_id) FROM enrollments e JOIN courses c ON e.course_id = c.course_id WHERE c.faculty_id = " + std::to_string(state.facultyId));
        if (tsq) { MYSQL_ROW r = mysql_fetch_row(tsq); if (r) totalStudents = atoi(r[0]); mysql_free_result(tsq); }
        MYSQL_RES* pgq = db.execQuery("SELECT COUNT(*) FROM enrollments e JOIN courses c ON e.course_id = c.course_id WHERE c.faculty_id = " + std::to_string(state.facultyId) + " AND e.grade IS NULL");
        if (pgq) { MYSQL_ROW r = mysql_fetch_row(pgq); if (r) pendingGrades = atoi(r[0]); mysql_free_result(pgq); }
        DrawStatCard("My Courses", std::to_string(myCourses).c_str(), ImVec4(0.122f, 0.435f, 0.922f, 1));
        ImGui::NextColumn();
        DrawStatCard("Students", std::to_string(totalStudents).c_str(), ImVec4(0.247f, 0.725f, 0.314f, 1));
        ImGui::NextColumn();
        DrawStatCard("Pending Grades", std::to_string(pendingGrades).c_str(), ImVec4(0.824f, 0.573f, 0.133f, 1));
        ImGui::NextColumn();
        DrawStatCard("Avg Attendance", "85%", ImVec4(0.969f, 0.471f, 0.729f, 1));
    } else {
        if (state.studentId == 0) {
            MYSQL_RES* sr = db.execQuery("SELECT student_id FROM students WHERE user_id = " + std::to_string(state.currentUser.userId));
            if (sr) { MYSQL_ROW r = mysql_fetch_row(sr); if (r) state.studentId = atoi(r[0]); mysql_free_result(sr); }
        }
        std::string cgpaStr = "N/A";
        int enrolledCredits = 0, attendancePct = 0, coursesCount = 0;
        MYSQL_RES* cgpaq = db.execQuery("SELECT SUM(c.credit_hours * e.grade_points) / SUM(c.credit_hours) FROM enrollments e JOIN courses c ON e.course_id = c.course_id WHERE e.student_id = " + std::to_string(state.studentId) + " AND e.grade IS NOT NULL");
        if (cgpaq) { MYSQL_ROW r = mysql_fetch_row(cgpaq); if (r && r[0]) { char buf[32]; snprintf(buf, 32, "%.2f", atof(r[0])); cgpaStr = buf; } mysql_free_result(cgpaq); }
        MYSQL_RES* ecq = db.execQuery("SELECT SUM(c.credit_hours) FROM enrollments e JOIN courses c ON e.course_id = c.course_id WHERE e.student_id = " + std::to_string(state.studentId));
        if (ecq) { MYSQL_ROW r = mysql_fetch_row(ecq); if (r && r[0]) enrolledCredits = atoi(r[0]); mysql_free_result(ecq); }
        MYSQL_RES* atq = db.execQuery("SELECT (SUM(CASE WHEN a.status='present' OR a.status='late' THEN 1 ELSE 0 END) * 100.0 / COUNT(*)) FROM attendance a WHERE a.student_id = " + std::to_string(state.studentId));
        if (atq) { MYSQL_ROW r = mysql_fetch_row(atq); if (r && r[0]) attendancePct = (int)atof(r[0]); mysql_free_result(atq); }
        MYSQL_RES* ccq = db.execQuery("SELECT COUNT(*) FROM enrollments WHERE student_id = " + std::to_string(state.studentId));
        if (ccq) { MYSQL_ROW r = mysql_fetch_row(ccq); if (r) coursesCount = atoi(r[0]); mysql_free_result(ccq); }
        DrawStatCard("CGPA", cgpaStr.c_str(), ImVec4(0.122f, 0.435f, 0.922f, 1));
        ImGui::NextColumn();
        DrawStatCard("Credits", std::to_string(enrolledCredits).c_str(), ImVec4(0.247f, 0.725f, 0.314f, 1));
        ImGui::NextColumn();
        DrawStatCard("Attendance", (std::to_string(attendancePct) + "%").c_str(), ImVec4(0.824f, 0.573f, 0.133f, 1));
        ImGui::NextColumn();
        DrawStatCard("Courses", std::to_string(coursesCount).c_str(), ImVec4(0.969f, 0.471f, 0.729f, 1));
    }

    ImGui::Columns(1);
    ImGui::PopStyleVar();
    ImGui::Dummy(ImVec2(0, 16));

    // ── Welcome block ──
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.086f, 0.106f, 0.133f, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.188f, 0.212f, 0.239f, 0.5f));
    ImGui::BeginChild("welcome", ImVec2(avail, 80), true);
    ImDrawList* dlw = ImGui::GetWindowDrawList();
    ImVec2 wpos = ImGui::GetWindowPos();
    dlw->AddRectFilledMultiColor(wpos, ImVec2(wpos.x + avail, wpos.y + 80),
        IM_COL32(0, 60, 55, 30), IM_COL32(0, 60, 55, 10),
        IM_COL32(10, 10, 20, 10), IM_COL32(10, 10, 20, 10));
    ImGui::SetCursorPosY(18); ImGui::SetCursorPosX(20);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::Text("Welcome, %s", state.currentUser.name.c_str());
    ImGui::PopStyleColor();
    ImGui::SetCursorPosX(20);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    std::string roleCap = state.currentUser.role;
    if (!roleCap.empty()) roleCap[0] = toupper(roleCap[0]);
    ImGui::Text("Role: %s", roleCap.c_str());
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);

    ImGui::Dummy(ImVec2(0, 16));

    // ── Alerts ──
    if (state.currentUser.role == "student") {
        MYSQL_RES* atq = db.execQuery("SELECT (SUM(CASE WHEN a.status='present' OR a.status='late' THEN 1 ELSE 0 END) * 100.0 / COUNT(*)) FROM attendance a WHERE a.student_id = " + std::to_string(state.studentId));
        if (atq) {
            MYSQL_ROW r = mysql_fetch_row(atq);
            if (r && r[0]) {
                float attPct = atof(r[0]);
                if (attPct < 75.0f) {
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.973f, 0.318f, 0.286f, 0.1f));
                    ImGui::BeginChild("attAlert", ImVec2(avail, 38), true);
                    ImGui::SetCursorPosY(8); ImGui::SetCursorPosX(14);
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.973f, 0.318f, 0.286f, 1));
                    ImGui::Text("Warning: Attendance is %.1f%% — below 75%% threshold!", attPct);
                    ImGui::PopStyleColor();
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                    ImGui::Dummy(ImVec2(0, 8));
                }
            }
            mysql_free_result(atq);
        }
    }
    if (state.currentUser.role == "faculty") {
        MYSQL_RES* pgq = db.execQuery("SELECT COUNT(*) FROM enrollments e JOIN courses c ON e.course_id = c.course_id WHERE c.faculty_id = " + std::to_string(state.facultyId) + " AND e.grade IS NULL");
        if (pgq) {
            MYSQL_ROW r = mysql_fetch_row(pgq);
            if (r && atoi(r[0]) > 0) {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.824f, 0.573f, 0.133f, 0.1f));
                ImGui::BeginChild("gradeAlert", ImVec2(avail, 38), true);
                ImGui::SetCursorPosY(8); ImGui::SetCursorPosX(14);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.824f, 0.573f, 0.133f, 1));
                ImGui::Text("Reminder: You have %s ungraded enrollments.", r[0]);
                ImGui::PopStyleColor();
                ImGui::EndChild();
                ImGui::PopStyleColor();
            }
            mysql_free_result(pgq);
        }
    }

    // ── Quick Actions ──
    ImGui::Dummy(ImVec2(0, 8));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.902f, 0.929f, 0.953f, 0.8f));
    ImGui::TextUnformatted("QUICK ACTIONS");
    ImGui::PopStyleColor();
    DrawAccentLine(36.0f, 2.0f);
    ImGui::Dummy(ImVec2(0, 8));

    auto& sel = state.sidebarSelection;
    float btnW = 170.0f;
    float btnGap = 12.0f;
    float totalBtnW = (state.currentUser.role == "admin" ? 4 : 3) * btnW + (state.currentUser.role == "admin" ? 3 : 2) * btnGap;
    ImGui::SetCursorPosX((avail - totalBtnW) * 0.5f);

    if (state.currentUser.role == "admin") {
        if (ImGui::Button("View Students", ImVec2(btnW, 40))) sel = 1;
        ImGui::SameLine(0, btnGap);
        if (ImGui::Button("View Courses", ImVec2(btnW, 40))) sel = 2;
        ImGui::SameLine(0, btnGap);
        if (ImGui::Button("Reports", ImVec2(btnW, 40))) sel = 8;
        ImGui::SameLine(0, btnGap);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.749f, 0.647f, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.749f, 0.647f, 0.35f));
        if (ImGui::Button("Refresh", ImVec2(btnW, 40))) state.dbStatsDirty = true;
        ImGui::PopStyleColor(2);
    } else if (state.currentUser.role == "student") {
        if (ImGui::Button("Register Courses", ImVec2(btnW, 40))) sel = 3;
        ImGui::SameLine(0, btnGap);
        if (ImGui::Button("View Grades", ImVec2(btnW, 40))) sel = 4;
        ImGui::SameLine(0, btnGap);
        if (ImGui::Button("Attendance", ImVec2(btnW, 40))) sel = 5;
    } else {
        if (ImGui::Button("My Courses", ImVec2(btnW, 40))) sel = 2;
        ImGui::SameLine(0, btnGap);
        if (ImGui::Button("Grade Entry", ImVec2(btnW, 40))) sel = 4;
        ImGui::SameLine(0, btnGap);
        if (ImGui::Button("Attendance", ImVec2(btnW, 40))) sel = 5;
    }
}
