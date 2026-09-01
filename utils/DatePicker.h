#pragma once
#include "imgui.h"
#include <ctime>
#include <string>

inline int getDaysInMonth(int year, int month) {
    static const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
        return 29;
    return days[month - 1];
}

inline int getDayOfWeek(int year, int month, int day) {
    struct tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    mktime(&t);
    return t.tm_wday;
}

inline bool DatePickerPopup(const char* id, int* year, int* month, int* day) {
    bool changed = false;
    ImGui::PushID(id);

    static int selYear = *year, selMonth = *month, selDay = *day;

    char preview[32];
    snprintf(preview, 32, "%04d-%02d-%02d", *year, *month, *day);
    ImGui::SetNextItemWidth(160);
    if (ImGui::BeginCombo("##datepicker", preview, ImGuiComboFlags_NoArrowButton)) {
        ImGui::Text("%s", "Date Picker");
        ImGui::Separator();

        // Navigation
        if (ImGui::Button("<")) {
            selMonth--;
            if (selMonth < 1) { selMonth = 12; selYear--; }
        }
        ImGui::SameLine();
        char monthYear[64];
        snprintf(monthYear, 64, "%s %04d",
                 ImGui::GetIO().Fonts->Fonts.Size > 0 ? "Month" : "",
                 selYear);
        static const char* monthNames[] = {"Jan","Feb","Mar","Apr","May","Jun",
                                           "Jul","Aug","Sep","Oct","Nov","Dec"};
        snprintf(monthYear, 64, "%s %04d", monthNames[selMonth - 1], selYear);
        ImGui::TextUnformatted(monthYear);
        ImGui::SameLine();
        if (ImGui::Button(">")) {
            selMonth++;
            if (selMonth > 12) { selMonth = 1; selYear++; }
        }

        // Day grid
        int daysInMonth = getDaysInMonth(selYear, selMonth);
        int startDay = getDayOfWeek(selYear, selMonth, 1);

        ImGui::Dummy(ImVec2(0, 4));
        static const char* dayHeaders[] = {"Su","Mo","Tu","We","Th","Fr","Sa"};
        for (int i = 0; i < 7; i++) {
            if (i > 0) ImGui::SameLine(0, 2);
            ImGui::TextUnformatted(dayHeaders[i]);
        }
        ImGui::Dummy(ImVec2(0, 2));

        for (int row = 0; row < 6; row++) {
            bool hasRow = false;
            for (int col = 0; col < 7; col++) {
                int dayNum = row * 7 + col - startDay + 1;
                if (col > 0) ImGui::SameLine(0, 2);
                if (dayNum < 1 || dayNum > daysInMonth) {
                    ImGui::TextUnformatted(" ");
                    // Reserve space
                    ImGui::Dummy(ImVec2(30, 24));
                } else {
                    hasRow = true;
                    ImGui::PushID(dayNum);
                    bool isSelected = (dayNum == selDay && selMonth == *month && selYear == *year);
                    if (isSelected) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.68f, 0.71f, 0.8f));
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,1,1));
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f,0.9f,0.94f,1));
                    }
                    char dStr[4];
                    snprintf(dStr, 4, "%2d", dayNum);
                    if (ImGui::Button(dStr, ImVec2(30, 24))) {
                        *year = selYear;
                        *month = selMonth;
                        *day = dayNum;
                        changed = true;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopStyleColor(2);
                    ImGui::PopID();
                }
            }
            if (!hasRow) break;
        }

        ImGui::EndCombo();

        // Reset button
        ImGui::SameLine();
        if (ImGui::Button("Today")) {
            time_t nowt = time(nullptr);
            struct tm nowTm;
            localtime_s(&nowTm, &nowt);
            *year = nowTm.tm_year + 1900;
            *month = nowTm.tm_mon + 1;
            *day = nowTm.tm_mday;
            selYear = *year;
            selMonth = *month;
            selDay = *day;
            changed = true;
        }
    }

    ImGui::PopID();
    return changed;
}
