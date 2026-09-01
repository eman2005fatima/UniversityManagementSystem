#define NOMINMAX
#include <iostream>
#include <cstdlib>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Database.h"
#include "Models.h"
#include "Config.h"
#include "Style.h"
#include "ToastSystem.h"
#include "Auth.h"
#include "StudentManager.h"

#include "panels/LoginPanel.h"
#include "panels/SignupPanel.h"
#include "panels/ForgotPasswordPanel.h"
#include "panels/DashboardPanel.h"
#include "panels/StudentsPanel.h"
#include "panels/FacultyPanel.h"
#include "panels/CoursesPanel.h"
#include "panels/RegistrationPanel.h"
#include "panels/GradesPanel.h"
#include "panels/AttendancePanel.h"
#include "panels/ReportsPanel.h"
#include "panels/SettingsPanel.h"
#include "panels/ProfilePanel.h"

#include "utils/Validators.h"
#include "utils/Crypto.h"

Database g_DB;
ToastSystem g_Toast;
AppState g_State;
StudentManager g_StudentMgr;

void SetupMainStyle() {
    LoadFonts();
    SetupImGuiStyle();
}

void ShowMainApplicationWindow() {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 ws = io.DisplaySize;
    float mainW = (ws.x < 1280.0f) ? 1280.0f : ws.x;
    float mainH = (ws.y < 800.0f) ? 800.0f : ws.y;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(mainW, mainH), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.051f, 0.067f, 0.090f, 1.0f));

    ImGui::Begin("University Management System", &g_State.showMainApp,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // ── Sidebar (220px fixed) ──
    float sidebarW = 220.0f;
    ImGui::BeginChild("Sidebar", ImVec2(sidebarW, 0), false);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.059f, 0.075f, 0.094f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

    ImDrawList* sdl = ImGui::GetWindowDrawList();
    ImVec2 swpos = ImGui::GetWindowPos();

    // Header area
    sdl->AddRectFilledMultiColor(swpos, ImVec2(swpos.x + sidebarW, swpos.y + 70),
        IM_COL32(0, 60, 55, 60), IM_COL32(0, 60, 55, 60),
        IM_COL32(0, 20, 25, 20), IM_COL32(0, 20, 25, 20));
    sdl->AddRectFilled(ImVec2(swpos.x + 50, swpos.y + 66), ImVec2(swpos.x + sidebarW - 50, swpos.y + 68),
        IM_COL32(0, 191, 165, 80));

    ImGui::SetCursorPosY(16); ImGui::SetCursorPosX(18);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::TextUnformatted("UNIVERSITY");
    ImGui::PopStyleColor();
    ImGui::SetCursorPosX(18);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.902f, 0.929f, 0.953f, 0.7f));
    ImGui::TextUnformatted("Management System");
    ImGui::PopStyleColor();

    // Navigation items
    struct NavItem { const char* label; int index; };
    std::vector<NavItem> items;

    if (g_State.currentUser.role == "admin") {
        items = {{"Dashboard",0},{"Manage Students",1},{"Manage Faculty",7},
                 {"Manage Courses",2},{"Attendance",5},{"Reports",8},
                 {"Settings",9},{"Profile",10}};
    } else if (g_State.currentUser.role == "faculty") {
        items = {{"Dashboard",0},{"My Courses",2},{"Student Grades",4},
                 {"Attendance",5},{"Profile",10},{"Settings",9}};
    } else {
        items = {{"Dashboard",0},{"My Courses",2},{"Registration",3},
                 {"My Grades",4},{"My Attendance",5},{"Profile",10},{"Settings",9}};
    }

    ImGui::SetCursorPosY(80);
    ImGui::BeginChild("SidebarMenu", ImVec2(sidebarW, ImGui::GetContentRegionAvail().y - 85), false);

    for (int i = 0; i < (int)items.size(); i++) {
        bool sel = (g_State.sidebarSelection == items[i].index);
        ImVec2 cpos = ImGui::GetCursorScreenPos();

        if (sel) {
            sdl->AddRectFilled(ImVec2(cpos.x, cpos.y), ImVec2(cpos.x + 4, cpos.y + 44), IM_COL32(0, 191, 165, 255));
            sdl->AddRectFilled(ImVec2(cpos.x + 4, cpos.y), ImVec2(cpos.x + sidebarW, cpos.y + 44), IM_COL32(26, 33, 40, 180));
        }

        if (sel) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.102f, 0.129f, 0.157f, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.102f, 0.129f, 0.157f, 0));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.059f, 0.075f, 0.094f, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.100f, 0.130f, 0.180f, 1));
        }
        ImGui::PushStyleColor(ImGuiCol_Text, sel ? ImVec4(0, 0.749f, 0.647f, 1) : ImVec4(0.545f, 0.580f, 0.620f, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::SetCursorPosX(0);
        if (ImGui::Button(items[i].label, ImVec2(sidebarW, 44))) {
            g_State.sidebarSelection = items[i].index;
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();

    // Footer
    ImGui::BeginChild("SidebarFooter", ImVec2(sidebarW, 75), false);
    ImDrawList* fdl = ImGui::GetWindowDrawList();
    ImVec2 fpos = ImGui::GetWindowPos();
    fdl->AddRectFilled(fpos, ImVec2(fpos.x + sidebarW, fpos.y + 75), IM_COL32(0, 0, 0, 30));
    fdl->AddRectFilled(fpos, ImVec2(fpos.x + sidebarW, fpos.y + 1), IM_COL32(0, 191, 165, 25));
    ImGui::SetCursorPosY(12); ImGui::SetCursorPosX(14);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.902f, 0.929f, 0.953f, 0.9f));
    ImGui::TextUnformatted(g_State.currentUser.name.c_str());
    ImGui::PopStyleColor();
    ImGui::SetCursorPosX(14);
    std::string roleLabel = g_State.currentUser.role;
    if (!roleLabel.empty()) roleLabel[0] = toupper(roleLabel[0]);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
    ImGui::TextUnformatted(roleLabel.c_str());
    ImGui::PopStyleColor();
    ImGui::SetCursorPos(ImVec2(sidebarW - 80, 22));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.129f, 0.149f, 0.177f, 1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.973f, 0.318f, 0.286f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.973f, 0.318f, 0.286f, 0.3f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if (ImGui::Button("Logout", ImVec2(66, 32))) {
        g_State.showLogoutConfirm = true;
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    ImGui::EndChild();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::SameLine();

    // ── Content area ──
    ImGui::BeginChild("Content", ImVec2(0, 0), false);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.051f, 0.067f, 0.090f, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 20));

    switch (g_State.sidebarSelection) {
    case 0: ShowDashboardPanel(g_State, g_DB); break;
    case 1: ShowStudentsPanel(g_State, g_DB, g_StudentMgr); break;
    case 2: ShowCoursesPanel(g_State, g_DB); break;
    case 3: ShowRegistrationPanel(g_State, g_DB); break;
    case 4: ShowGradesPanel(g_State, g_DB); break;
    case 5: ShowAttendancePanel(g_State, g_DB); break;
    case 7: ShowFacultyPanel(g_State, g_DB); break;
    case 8: ShowReportsPanel(g_State, g_DB); break;
    case 9: ShowSettingsPanel(g_State, g_DB); break;
    case 10: ShowProfilePanel(g_State, g_DB); break;
    default: ShowDashboardPanel(g_State, g_DB); break;
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    // ── Logout modal ──
    if (g_State.showLogoutConfirm) {
        ImGui::OpenPopup("Confirm Logout");
    }
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(380.0f, 185.0f), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24.0f, 20.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    bool showPopup = ImGui::BeginPopupModal("Confirm Logout", &g_State.showLogoutConfirm,
                                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::PopStyleVar(2);
    if (showPopup)
    {
        ImGui::Dummy(ImVec2(0, 4));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.902f, 0.929f, 0.953f, 1));
        float tw = ImGui::CalcTextSize("Leave?").x;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - tw) * 0.5f);
        ImGui::TextUnformatted("Leave?");
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 8));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.580f, 0.620f, 1));
        tw = ImGui::CalcTextSize("Are you sure you want to logout?").x;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - tw) * 0.5f);
        ImGui::TextUnformatted("Are you sure you want to logout?");
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 20));
        float bw = 130.0f, gap = 16.0f, btnsW = bw * 2 + gap;
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - btnsW) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.749f, 0.647f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0.85f, 0.75f, 1));
        if (ImGui::Button("Yes, Logout", ImVec2(bw, 38))) {
            g_State.currentUser.loggedIn = false;
            g_State.studentId = 0; g_State.facultyId = 0;
            g_State.sidebarSelection = 0;
            g_State.showMainApp = false;
            g_State.showLogin = true;
            g_State.showLogoutConfirm = false;
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine(0, gap);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.129f, 0.149f, 0.177f, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.169f, 0.192f, 0.224f, 1));
        if (ImGui::Button("Cancel", ImVec2(bw, 38))) {
            g_State.showLogoutConfirm = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(2);
        ImGui::EndPopup();
    }
}

int main() {
    Config config;
    if (!config.load("config.ini")) {
        std::cerr << "Warning: config.ini not found. Using defaults.\n";
    }

    if (!g_DB.connect(config)) {
        std::cerr << "Failed to connect to database.\n";
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    g_StudentMgr.loadFromDB(g_DB);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 800, "University Management System", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    SetupMainStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    g_State.showLogin = true;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (g_State.showLogin) ShowLoginPanel(g_State, g_DB);
        if (g_State.showSignup) ShowSignupPanel(g_State, g_DB);
        if (g_State.showForgotPassword) ShowForgotPasswordPanel(g_State, g_DB);
        if (g_State.showMainApp) ShowMainApplicationWindow();

        g_Toast.render();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.051f, 0.067f, 0.090f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
