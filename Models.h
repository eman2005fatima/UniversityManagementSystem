#pragma once
#include <string>
#include <vector>

struct UserSession {
    int userId = 0;
    std::string email;
    std::string role;
    std::string name;
    bool loggedIn = false;
};

struct Student {
    int studentId = 0;
    int userId = 0;
    std::string name;
    std::string email;
    std::string department;
    int enrollmentYear = 2024;
    std::string phone;
    int semester = 1;
    int totalCredits = 0;
};

struct Faculty {
    int facultyId = 0;
    int userId = 0;
    std::string name;
    std::string email;
    std::string department;
    std::string designation;
    std::string qualification;
};

struct Course {
    int courseId = 0;
    std::string courseCode;
    std::string courseName;
    int creditHours = 3;
    std::string department;
    int facultyId = 0;
    std::string facultyName;
    int semester = 1;
    int maxStudents = 50;
};

struct Enrollment {
    int enrollmentId = 0;
    int studentId = 0;
    int courseId = 0;
    std::string semester;
    int year = 2025;
    std::string grade;
    float gradePoints = 0.0f;
};

struct Attendance {
    int attendanceId = 0;
    int studentId = 0;
    int courseId = 0;
    std::string date;
    std::string status;
};

struct Toast {
    std::string message;
    int type = 0;
    double spawnTime = 0.0;
    float slideX = 300.0f;
    float alpha = 1.0f;
};

struct Announcement {
    int id = 0;
    std::string title;
    std::string body;
    int postedBy = 0;
    std::string postedAt;
    std::string expiresAt;
    bool read = false;
};

struct TimetableSlot {
    int slotId = 0;
    int courseId = 0;
    std::string dayOfWeek;
    std::string startTime;
    std::string endTime;
    std::string room;
    std::string courseCode;
    std::string courseName;
};

struct AppState {
    UserSession currentUser;
    int sidebarSelection = 0;
    int studentId = 0;
    int facultyId = 0;
    std::string myName;
    std::string myDept;
    int mySem = 1;

    bool showLogin = true;
    bool showSignup = false;
    bool showForgotPassword = false;
    bool showMainApp = false;
    bool showChangePassword = false;
    bool showLogoutConfirm = false;

    float darkenBg = 0.0f;

    bool dbStatsDirty = true;
    int cachedStudentCount = 0;
    int cachedFacultyCount = 0;
    int cachedCourseCount = 0;
    int cachedEnrollmentCount = 0;
};
