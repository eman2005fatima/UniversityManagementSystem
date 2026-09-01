#pragma once
#include <string>
#include <cctype>
#include <regex>

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

inline std::string toLower(const std::string& s) {
    std::string r = s;
    for (char& c : r) if (c >= 'A' && c <= 'Z') c += 32;
    return r;
}

inline bool isValidEmail(const std::string& email) {
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

inline bool isValidPhone(const std::string& phone) {
    if (phone.empty()) return true;
    const std::regex pattern(R"(^\+?[\d\s\-\(\)]{7,20}$)");
    return std::regex_match(phone, pattern);
}

inline bool isStrongPassword(const std::string& p) {
    if (p.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    const std::string special = "!@#$%^&*";
    for (char c : p) {
        if (c >= 'A' && c <= 'Z') hasUpper = true;
        else if (c >= 'a' && c <= 'z') hasLower = true;
        else if (c >= '0' && c <= '9') hasDigit = true;
        else if (special.find(c) != std::string::npos) hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

inline std::string passwordStrengthLabel(const std::string& p) {
    if (p.empty()) return "";
    int score = 0;
    if (p.length() >= 8) score++;
    if (p.length() >= 12) score++;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    const std::string special = "!@#$%^&*";
    for (char c : p) {
        if (c >= 'A' && c <= 'Z') hasUpper = true;
        else if (c >= 'a' && c <= 'z') hasLower = true;
        else if (c >= '0' && c <= '9') hasDigit = true;
        else if (special.find(c) != std::string::npos) hasSpecial = true;
    }
    if (hasUpper) score++;
    if (hasLower) score++;
    if (hasDigit) score++;
    if (hasSpecial) score += 2;
    if (score <= 2) return "Weak";
    if (score <= 4) return "Fair";
    if (score <= 6) return "Strong";
    return "Very Strong";
}

inline float passwordStrengthValue(const std::string& p) {
    if (p.empty()) return 0.0f;
    std::string label = passwordStrengthLabel(p);
    if (label == "Weak") return 0.25f;
    if (label == "Fair") return 0.50f;
    if (label == "Strong") return 0.75f;
    return 1.0f;
}
