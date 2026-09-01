#pragma once
#include <cmath>
#include <unordered_map>
#include <string>

struct Anim {
    float val = 0.0f;
    void tick(float target, float speed = 0.15f) {
        val += (target - val) * speed;
    }
    void reset() { val = 0.0f; }
};

struct AnimManager {
    std::unordered_map<std::string, Anim> anims;

    Anim& get(const std::string& key) {
        return anims[key];
    }

    void tick(const std::string& key, float target, float speed = 0.15f) {
        anims[key].tick(target, speed);
    }

    float getVal(const std::string& key) {
        return anims[key].val;
    }

    void reset(const std::string& key) {
        anims[key].reset();
    }

    void resetAll() {
        for (auto& [k, v] : anims) v.reset();
    }
};
