#include "FacultyPanel.h"
#include "../ToastSystem.h"
#include "../Style.h"
#include "../utils/Export.h"

extern ToastSystem g_Toast;

void ShowFacultyPanel(AppState& state, Database& db) {
    if (state.currentUser.role != "admin") {
        state.sidebarSelection = 0;
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0.749f, 0.647f, 1));
    ImGui::TextUnformatted("MANAGE FACULTY");
    ImGui::PopStyleColor();
    DrawAccentLine(40.0f, 3.0f);
    ImGui::Dummy(ImVec2(0, 8));

    ImGui::SameLine();
    if (ImGui::Button("Export CSV", ImVec2(120, 30))) {
        MYSQL_RES* fr = db.execQuery("SELECT faculty_id, name, designation, department, qualification, email FROM faculty");
        if (fr) {
            std::vector<std::string> rows;
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(fr))) {
                rows.push_back(std::string(row[0]) + ",\"" + (row[1]?row[1]:"") + "\",\"" +
                               (row[2]?row[2]:"") + "\",\"" + (row[3]?row[3]:"") + "\",\"" +
                               (row[4]?row[4]:"") + "\",\"" + (row[5]?row[5]:"") + "\"");
            }
            mysql_free_result(fr);
            std::ofstream file("faculty_" + getCurrentDateStr() + ".csv");
            if (file.is_open()) {
                file << "\"ID\",\"Name\",\"Designation\",\"Department\",\"Qualification\",\"Email\"\n";
                for (auto& r : rows) file << r << "\n";
                file.close();
                g_Toast.push("CSV exported: faculty.csv", 0);
            } else g_Toast.push("CSV export failed.", 1);
        }
    }

    ImGui::Dummy(ImVec2(0, 12));

    BeginTableStyled();
    if (ImGui::BeginTable("faculty_table", 6,
        ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg |
        ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX,
        ImVec2(0, ImGui::GetContentRegionAvail().y))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 180.0f);
        ImGui::TableSetupColumn("Designation", ImGuiTableColumnFlags_WidthFixed, 160.0f);
        ImGui::TableSetupColumn("Department", ImGuiTableColumnFlags_WidthFixed, 160.0f);
        ImGui::TableSetupColumn("Qualification", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        MYSQL_RES* fr = db.execQuery("SELECT faculty_id, name, designation, department, qualification FROM faculty ORDER BY name");
        if (fr) {
            MYSQL_ROW row;
            int idx = 0;
            while ((row = mysql_fetch_row(fr))) {
                ImGui::TableNextRow(ImGuiTableRowFlags_None, 38.0f);
                ImGui::TableNextColumn(); ImGui::Text("%d", ++idx);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[1]);
                ImGui::TableNextColumn(); ImGui::Text("%s", row[2] ? row[2] : "");
                ImGui::TableNextColumn(); ImGui::Text("%s", row[3] ? row[3] : "");
                ImGui::TableNextColumn(); ImGui::Text("%s", row[4] ? row[4] : "");
                ImGui::TableNextColumn();
                DrawPill("Active", ImVec4(0.247f, 0.725f, 0.314f, 1));
            }
            mysql_free_result(fr);
        }
        ImGui::EndTable();
    }
    EndTableStyled();
}
