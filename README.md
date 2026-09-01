# 🎓 University Management System

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://isocpp.org/)
[![GUI Framework](https://img.shields.io/badge/GUI-Dear%20ImGui-orange.svg)](https://github.com/ocornut/imgui)
[![Graphics](https://img.shields.io/badge/Graphics-OpenGL3%20%2B%20GLFW3-green.svg)](https://www.glfw.org/)
[![Database](https://img.shields.io/badge/Database-MySQL%208.0-blue.svg)](https://www.mysql.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D6.svg)](https://microsoft.com/windows)
[![License](https://img.shields.io/badge/License-MIT-brightgreen.svg)](LICENSE.txt)

A high-performance, professional desktop GUI application designed for managing university administrative operations, including student records, faculty management, course registrations, attendance tracking, grade entries, and analytical reporting.

Built with **C++17**, **Dear ImGui**, **OpenGL3**, **GLFW3**, and **MySQL 8.0** using a modern component-based UI architecture with smooth animations, custom dark/light themes, and interactive dashboard analytics.

---

## 🌟 Key Features

- **Multi-Role Access Control**: Tailored dashboards for **Admin**, **Faculty**, and **Student** roles.
- **Student & Faculty Management**: Full CRUD operations with search, filtering, and pagination (50 rows/page).
- **Course & Registration System**: Real-time course enrollment, prerequisite checking, and drop handlers.
- **Interactive Grade Management**: Grade entry with automatic GPA calculation and distribution bar charts.
- **Attendance Tracker & Date Picker**: Custom popup calendar widget for single-date or bulk attendance logging (Mark All Present/Absent).
- **Interactive Dashboard**: Real-time stat cards with animated progress indicators and operational alerts (low CGPA < 2.0, low attendance < 75%, pending grades).
- **Data Export & Reporting**: One-click export of data tables to CSV format.
- **Security & User Auth**: Hashed password storage with salt generation, interactive password strength meter, account lockout after 3 failed attempts, and password reset panel.
- **Modern UI & UX**: Animated UI transitions, slide-in toast notifications, customizable dark/light themes, and keyboard shortcuts.

---

## 🏗 System Architecture

```
UniversityManagementSystem/
├── main.cpp                     # Application entry point & main render loop
├── Config.h / Config.cpp        # INI configuration loader (config.ini)
├── Database.h / Database.cpp    # RAII MySQL database connection wrapper
├── Auth.h / Auth.cpp            # Authentication, session & security handlers
├── Models.h                     # Data Transfer Objects (DTOs) & structs
├── AnimState.h                  # Widget-level dynamic animation states
├── Style.h / Style.cpp          # Visual theme design system, colors & fonts
├── ToastSystem.h / .cpp         # Slide-in toast alert notification engine
├── StudentManager.h / .cpp      # Student state & caching manager
├── panels/                      # UI Panels & Modular Views
│   ├── LoginPanel.h/.cpp
│   ├── SignupPanel.h/.cpp
│   ├── ForgotPasswordPanel.h/.cpp
│   ├── DashboardPanel.h/.cpp
│   ├── StudentsPanel.h/.cpp
│   ├── FacultyPanel.h/.cpp
│   ├── CoursesPanel.h/.cpp
│   ├── RegistrationPanel.h/.cpp
│   ├── GradesPanel.h/.cpp
│   ├── AttendancePanel.h/.cpp
│   ├── ReportsPanel.h/.cpp
│   ├── SettingsPanel.h/.cpp
│   └── ProfilePanel.h/.cpp
├── utils/                       # Core Utilities & Helper Widgets
│   ├── Validators.h             # Input validation (email, phone, password)
│   ├── Crypto.h                 # Security salt & hashing utilities
│   ├── DatePicker.h/.cpp        # Custom calendar picker popup widget
│   └── Export.h/.cpp            # CSV export helper functions
├── schema.sql                   # Full MySQL database DDL schema
├── sample_data.sql              # Seed data for testing & demo
├── config.ini.example           # Database configuration template
└── Makefile                     # Build automation setup
```

---

## 🛠 Prerequisites & Dependencies

1. **C++ Compiler**: MinGW-w64 (GCC 11+ with C++17 support).
2. **Graphics Libraries**: GLFW 3.x and OpenGL 3.3+.
3. **Database**: MySQL Server 8.0 installed locally or accessible remotely.
4. **MySQL C API**: `libmysql.dll` / `libmysqlclient`.

---

## 🚀 Getting Started

### 1. Database Setup

Import the database schema and initial sample dataset into your MySQL instance:

```bash
mysql -u root -p < schema.sql
mysql -u root -p university_db < sample_data.sql
```

### 2. Configuration

Copy `config.ini.example` to `config.ini` and update it with your MySQL credentials:

```ini
[database]
host = localhost
user = root
password = your_password_here
dbname = university_db
port = 3306
```

> ⚠️ **Note**: `config.ini` contains sensitive database credentials and is automatically ignored by `.gitignore`.

### 3. Build & Run

To compile and launch the application using the included `Makefile`:

```bash
# Build binary
make

# Run application
make run
```

Alternatively, compile and link manually using GCC:

```bash
g++ -std=c++17 main.cpp Auth.cpp Config.cpp Database.cpp StudentManager.cpp Style.cpp ToastSystem.cpp imgui*.cpp utils/*.cpp panels/*.cpp -I. -lglfw3 -lmysql -lopengl32 -o university_system.exe
```

---

## 🔑 Default Credentials

| Role | Email | Password | Access Level |
| :--- | :--- | :--- | :--- |
| **Admin** | `admin@university.edu` | `Admin@123` | Full system administration & reporting |
| **Faculty** | `faculty@university.edu` | `Faculty@123` | Course grade entry, student attendance |
| **Student** | `student@university.edu` | `Student@123` | Profile, course enrollment, grade view |

---

## ⌨️ Keyboard Shortcuts

| Shortcut | Description |
| :--- | :--- |
| <kbd>Ctrl</kbd> + <kbd>F</kbd> | Focus global search bar in active panel |
| <kbd>Ctrl</kbd> + <kbd>E</kbd> | Export current table view to CSV file |
| <kbd>Ctrl</kbd> + <kbd>N</kbd> | Open creation modal (Admin panels) |
| <kbd>Ctrl</kbd> + <kbd>S</kbd> | Save changes / Submit current active form |
| <kbd>Esc</kbd> | Dismiss active popup modal or navigate to main dashboard |

---

## 👤 Author

Developed by **Eman Fatima**  
📧 Email: [emanmubashir2005@gmail.com](mailto:emanmubashir2005@gmail.com)  
🌐 GitHub: [@eman2005fatima](https://github.com/eman2005fatima)

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details.
