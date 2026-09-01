#pragma once
#include "imgui.h"
#include <string>

extern ImFont* fontRegular;
extern ImFont* fontHeading;
extern ImFont* fontMono;
extern bool darkMode;

void SetupImGuiStyle();
void LoadFonts();
void ToggleTheme();

// ── Card decoration ──
void DrawCardBorder(ImVec4 color);
void DrawAccentLine(float width = 36.0f, float height = 3.0f, ImU32 color = 0);
void DrawFieldGlow();

// ── Form helpers ──
void BeginFormCard();
void EndFormCard();
void DrawSectionHeader(const char* title);

bool DrawLabeledInput(const char* label, const char* inputId, char* buf, size_t bufSize,
                      ImGuiInputTextFlags flags = 0, float labelWidth = 160.0f);
bool DrawLabeledInputInt(const char* label, const char* inputId, int* value,
                         float labelWidth = 160.0f);
bool DrawLabeledCombo(const char* label, const char* inputId, int* currentItem,
                      const char* const items[], int itemsCount, float labelWidth = 160.0f);

// ── Dashboard helpers ──
void DrawStatCard(const char* title, const char* value, ImVec4 borderColor);
void DrawPill(const char* text, ImVec4 color);

// ── Table helpers ──
void BeginTableStyled();
void EndTableStyled();
