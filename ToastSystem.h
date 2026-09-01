#pragma once
#include <string>
#include <deque>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "Models.h"

class ToastSystem {
public:
    void push(const std::string& msg, int type);
    void render();
private:
    std::deque<Toast> queue_;
    static const int MAX_VISIBLE = 4;
    static const double DISPLAY_TIME;
};
