#pragma once
#include "imgui.h"
#include "../Models.h"
#include "../AnimState.h"
#include "../Database.h"
#include "../StudentManager.h"

void ShowStudentsPanel(AppState& state, Database& db, StudentManager& studentMgr);
