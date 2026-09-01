CXX      = g++
STD      = -std=c++17
WARN     = -Wall -Wextra -Wno-unused-parameter
OPT      = -O2
INCLUDES = -I. -Ipanels -Iutils \
           -I"C:/Program Files/MySQL/MySQL Server 8.0/include" \
           -I"C:/msys64/ucrt64/include"
LIBS     = -L"C:/Program Files/MySQL/MySQL Server 8.0/lib" -llibmysql \
           -L"C:/msys64/ucrt64/lib" -lglfw3 -lopengl32 -lgdi32 -limm32
IMGUI    = imgui.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp \
           backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp
SRCS     = main.cpp Config.cpp Database.cpp Auth.cpp Style.cpp \
           ToastSystem.cpp StudentManager.cpp \
           panels/LoginPanel.cpp panels/SignupPanel.cpp \
           panels/ForgotPasswordPanel.cpp panels/DashboardPanel.cpp \
           panels/StudentsPanel.cpp panels/FacultyPanel.cpp \
           panels/CoursesPanel.cpp panels/RegistrationPanel.cpp \
           panels/GradesPanel.cpp panels/AttendancePanel.cpp \
           panels/ReportsPanel.cpp panels/SettingsPanel.cpp \
           panels/ProfilePanel.cpp \
           utils/DatePicker.cpp utils/Export.cpp
TARGET   = university_system.exe

all: $(TARGET)
$(TARGET): $(SRCS) $(IMGUI)
	$(CXX) $(STD) $(WARN) $(OPT) $(INCLUDES) $^ -o $@ $(LIBS)

run: $(TARGET)
	@copy /Y "C:\msys64\ucrt64\bin\glfw3.dll" . >nul 2>&1 || true
	@copy /Y "C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.dll" . >nul 2>&1 || true
	./$(TARGET)

db:
	mysql -u root -p < schema.sql && mysql -u root -p university_db < sample_data.sql

clean:
	del /Q $(TARGET) 2>nul || true
