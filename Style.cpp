#include "Style.h"
#include <cmath>
#include <cstring>
#include <GLFW/glfw3.h>

ImFont* fontRegular = nullptr;
ImFont* fontHeading = nullptr;
ImFont* fontMono = nullptr;
bool darkMode = true;

// ── Color palette (dark only) ──
namespace Color {
    ImVec4 bg            = ImVec4(0.051f, 0.067f, 0.090f, 1.0f); // #0D1117
    ImVec4 panel         = ImVec4(0.086f, 0.106f, 0.133f, 1.0f); // #161B22
    ImVec4 sidebar       = ImVec4(0.059f, 0.075f, 0.094f, 1.0f); // #0F1318
    ImVec4 accent        = ImVec4(0.000f, 0.749f, 0.647f, 1.0f); // #00BFA5
    ImVec4 hoverBlue     = ImVec4(0.122f, 0.435f, 0.922f, 1.0f); // #1F6FEB
    ImVec4 text          = ImVec4(0.902f, 0.929f, 0.953f, 1.0f); // #E6EDF3
    ImVec4 textMuted     = ImVec4(0.545f, 0.580f, 0.620f, 1.0f); // #8B949E
    ImVec4 inputBg       = ImVec4(0.129f, 0.149f, 0.177f, 1.0f); // #21262D
    ImVec4 border        = ImVec4(0.188f, 0.212f, 0.239f, 1.0f); // #30363D
    ImVec4 danger        = ImVec4(0.973f, 0.318f, 0.286f, 1.0f); // #F85149
    ImVec4 success       = ImVec4(0.247f, 0.725f, 0.314f, 1.0f); // #3FB950
    ImVec4 warning       = ImVec4(0.824f, 0.573f, 0.133f, 1.0f); // #D29922
    ImVec4 cardBlue      = ImVec4(0.122f, 0.435f, 0.922f, 1.0f); // #1F6FEB
    ImVec4 cardGreen     = ImVec4(0.247f, 0.725f, 0.314f, 1.0f); // #3FB950
    ImVec4 cardYellow    = ImVec4(0.824f, 0.573f, 0.133f, 1.0f); // #D29922
    ImVec4 cardPink      = ImVec4(0.969f, 0.471f, 0.729f, 1.0f); // #F778BA
}

void LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();
    fontRegular = io.Fonts->AddFontDefault();
    fontHeading = fontRegular;
    fontMono = fontRegular;
}

void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text]              = Color::text;
    colors[ImGuiCol_TextDisabled]      = Color::textMuted;
    colors[ImGuiCol_WindowBg]          = Color::bg;
    colors[ImGuiCol_ChildBg]           = Color::panel;
    colors[ImGuiCol_PopupBg]           = ImVec4(0.129f, 0.149f, 0.177f, 0.95f);
    colors[ImGuiCol_Border]            = Color::border;
    colors[ImGuiCol_BorderShadow]      = ImVec4(0, 0, 0, 0);
    colors[ImGuiCol_FrameBg]           = Color::inputBg;
    colors[ImGuiCol_FrameBgHovered]    = ImVec4(0.169f, 0.192f, 0.224f, 1.0f);
    colors[ImGuiCol_FrameBgActive]     = ImVec4(0.200f, 0.224f, 0.259f, 1.0f);
    colors[ImGuiCol_TitleBg]           = Color::panel;
    colors[ImGuiCol_TitleBgActive]     = Color::panel;
    colors[ImGuiCol_TitleBgCollapsed]  = Color::bg;
    colors[ImGuiCol_MenuBarBg]         = Color::sidebar;
    colors[ImGuiCol_ScrollbarBg]       = Color::panel;
    colors[ImGuiCol_ScrollbarGrab]     = Color::border;
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.350f, 0.400f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.400f, 0.450f, 0.500f, 1.0f);
    colors[ImGuiCol_CheckMark]         = Color::accent;
    colors[ImGuiCol_SliderGrab]        = Color::accent;
    colors[ImGuiCol_SliderGrabActive]  = ImVec4(0, 0.6f, 0.5f, 1);
    colors[ImGuiCol_Button]            = Color::inputBg;
    colors[ImGuiCol_ButtonHovered]     = Color::hoverBlue;
    colors[ImGuiCol_ButtonActive]      = ImVec4(0.09f, 0.35f, 0.75f, 1);
    colors[ImGuiCol_Header]            = ImVec4(0, 0.749f, 0.647f, 0.2f);
    colors[ImGuiCol_HeaderHovered]     = ImVec4(0, 0.749f, 0.647f, 0.35f);
    colors[ImGuiCol_HeaderActive]      = ImVec4(0, 0.6f, 0.5f, 0.4f);
    colors[ImGuiCol_Separator]         = Color::border;
    colors[ImGuiCol_SeparatorHovered]  = Color::accent;
    colors[ImGuiCol_SeparatorActive]   = Color::accent;
    colors[ImGuiCol_ResizeGrip]        = Color::border;
    colors[ImGuiCol_ResizeGripHovered] = Color::accent;
    colors[ImGuiCol_ResizeGripActive]  = Color::accent;
    colors[ImGuiCol_Tab]               = Color::panel;
    colors[ImGuiCol_TabHovered]        = Color::accent;
    colors[ImGuiCol_TabActive]         = Color::accent;
    colors[ImGuiCol_TabUnfocused]      = Color::sidebar;
    colors[ImGuiCol_TabUnfocusedActive]= Color::panel;
    colors[ImGuiCol_TableHeaderBg]     = ImVec4(0.122f, 0.161f, 0.216f, 1.0f); // #1F2937
    colors[ImGuiCol_TableRowBg]        = Color::panel;
    colors[ImGuiCol_TableRowBgAlt]     = Color::bg;
    colors[ImGuiCol_TextSelectedBg]    = ImVec4(0, 0.749f, 0.647f, 0.25f);
    colors[ImGuiCol_NavHighlight]      = Color::accent;
    colors[ImGuiCol_ModalWindowDimBg]  = ImVec4(0, 0, 0, 0.65f);

    style.WindowRounding    = 6.0f;
    style.ChildRounding     = 5.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 5.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding      = 5.0f;
    style.TabRounding       = 4.0f;
    style.FramePadding      = ImVec2(12, 10);
    style.ItemSpacing       = ImVec2(12, 14);
    style.ItemInnerSpacing  = ImVec2(8, 6);
    style.WindowPadding     = ImVec2(24, 24);
    style.CellPadding       = ImVec2(12, 8);
    style.IndentSpacing     = 20.0f;
    style.ScrollbarSize     = 0.0f;
    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.WindowTitleAlign  = ImVec2(0.5f, 0.5f);
    style.ButtonTextAlign   = ImVec2(0.5f, 0.5f);
}

void ToggleTheme() {
    darkMode = !darkMode;
    SetupImGuiStyle();
}

// ── Draw a colored 3px top border on a child window ──
void DrawCardBorder(ImVec4 color) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 min = ImGui::GetWindowPos();
    ImVec2 max = ImGui::GetWindowSize();
    dl->AddRectFilled(min, ImVec2(min.x + max.x, min.y + 3), ImColor(color));
}

// ── Section divider with accent line ──
void DrawAccentLine(float width, float height, ImU32 color) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    if (color == 0) color = IM_COL32(0, 191, 165, 255);
    dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), color, height * 0.5f);
    ImGui::Dummy(ImVec2(0, height + 6));
}

// ── Focus glow effect ──
void DrawFieldGlow() {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 min = ImGui::GetItemRectMin();
    ImVec2 max = ImGui::GetItemRectMax();
    if (ImGui::IsItemActive()) {
        float t = (float)glfwGetTime();
        float pulse = 0.6f + 0.4f * (sinf(t * 3.0f) * 0.5f + 0.5f);
        dl->AddRect(min, max, IM_COL32(0, 191, 165, (int)(80 * pulse)), 6.0f, 0, 2.0f);
    } else if (ImGui::IsItemHovered()) {
        dl->AddRect(min, max, IM_COL32(0, 191, 165, 35), 6.0f, 0, 1.0f);
    }
}

void BeginFormCard() {
    float avail = ImGui::GetContentRegionAvail().x;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Color::panel);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, Color::border);
    ImGui::BeginChild("##formCard", ImVec2(avail, 0), true);
    ImGui::PopStyleColor();
}

void EndFormCard() {
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void DrawSectionHeader(const char* title) {
    ImGui::Dummy(ImVec2(0, 8));
    if (fontHeading) ImGui::PushFont(fontHeading);
    ImGui::PushStyleColor(ImGuiCol_Text, Color::accent);
    ImGui::TextUnformatted(title);
    ImGui::PopStyleColor();
    if (fontHeading) ImGui::PopFont();
    DrawAccentLine(36.0f, 3.0f);
}

bool DrawLabeledInput(const char* label, const char* inputId, char* buf, size_t bufSize, ImGuiInputTextFlags flags, float labelWidth) {
    float avail = ImGui::GetContentRegionAvail().x;
    float inputW = (avail > labelWidth + 50.0f) ? avail - labelWidth - 40.0f : 300.0f;
    ImGui::PushStyleColor(ImGuiCol_Text, Color::textMuted);
    ImGui::SetCursorPosX(labelWidth - ImGui::CalcTextSize(label).x + 20.0f);
    ImGui::TextUnformatted(label);
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 8);
    ImGui::SetCursorPosX(labelWidth + 28.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::SetNextItemWidth(inputW);
    bool changed = ImGui::InputText(inputId, buf, bufSize, flags);
    ImGui::PopStyleVar();
    DrawFieldGlow();
    ImGui::Dummy(ImVec2(0, 8));
    return changed;
}

bool DrawLabeledInputInt(const char* label, const char* inputId, int* value, float labelWidth) {
    float avail = ImGui::GetContentRegionAvail().x;
    float inputW = (avail > labelWidth + 50.0f) ? avail - labelWidth - 40.0f : 300.0f;
    ImGui::PushStyleColor(ImGuiCol_Text, Color::textMuted);
    ImGui::SetCursorPosX(labelWidth - ImGui::CalcTextSize(label).x + 20.0f);
    ImGui::TextUnformatted(label);
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 8);
    ImGui::SetCursorPosX(labelWidth + 28.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::SetNextItemWidth(inputW);
    bool changed = ImGui::InputInt(inputId, value);
    ImGui::PopStyleVar();
    ImGui::Dummy(ImVec2(0, 8));
    return changed;
}

bool DrawLabeledCombo(const char* label, const char* inputId, int* currentItem, const char* const items[], int itemsCount, float labelWidth) {
    float avail = ImGui::GetContentRegionAvail().x;
    float inputW = (avail > labelWidth + 50.0f) ? avail - labelWidth - 40.0f : 300.0f;
    ImGui::PushStyleColor(ImGuiCol_Text, Color::textMuted);
    ImGui::SetCursorPosX(labelWidth - ImGui::CalcTextSize(label).x + 20.0f);
    ImGui::TextUnformatted(label);
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 8);
    ImGui::SetCursorPosX(labelWidth + 28.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::SetNextItemWidth(inputW);
    bool changed = ImGui::Combo(inputId, currentItem, items, itemsCount);
    ImGui::PopStyleVar();
    ImGui::Dummy(ImVec2(0, 8));
    return changed;
}

void DrawStatCard(const char* title, const char* value, ImVec4 borderColor) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, Color::panel);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::BeginChild(title, ImVec2(0, 100), true);
    DrawCardBorder(borderColor);
    ImGui::SetCursorPos(ImVec2(16, 16));
    ImGui::PushStyleColor(ImGuiCol_Text, Color::textMuted);
    ImGui::TextUnformatted(title);
    ImGui::PopStyleColor();
    ImGui::SetCursorPos(ImVec2(16, 44));
    ImGui::PushStyleColor(ImGuiCol_Text, Color::text);
    if (fontHeading) ImGui::PushFont(fontHeading);
    ImGui::TextUnformatted(value);
    if (fontHeading) ImGui::PopFont();
    ImGui::PopStyleColor();
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void DrawPill(const char* text, ImVec4 color) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float w = textSize.x + 16.0f;
    float h = 22.0f;
    dl->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + w, pos.y + h),
                      ImColor(color), h * 0.5f);
    float textX = pos.x + (w - textSize.x) * 0.5f;
    float textY = pos.y + (h - textSize.y) * 0.5f;
    dl->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), text);
    ImGui::Dummy(ImVec2(w, h));
}

void BeginTableStyled() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0.122f, 0.161f, 0.216f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, Color::panel);
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, Color::bg);
    ImGui::PushStyleColor(ImGuiCol_Border, Color::border);
}

void EndTableStyled() {
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
}
