#pragma once
#include <string>
#include <fstream>
#include <unordered_map>
#include <algorithm>

class Config {
public:
    std::string host = "localhost";
    std::string user = "root";
    std::string password = "";
    std::string dbname = "university_db";
    int port = 3306;

    bool load(const std::string& path = "config.ini") {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        std::string line;
        std::string section;
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r"));
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;
            if (line[0] == '[') {
                size_t end = line.find(']');
                if (end != std::string::npos) {
                    section = line.substr(1, end - 1);
                }
                continue;
            }
            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            auto trim = [](std::string& s) {
                s.erase(0, s.find_first_not_of(" \t\r"));
                s.erase(s.find_last_not_of(" \t\r") + 1);
            };
            trim(key);
            trim(value);

            if (section == "database") {
                if (key == "host") host = value;
                else if (key == "user") user = value;
                else if (key == "password") password = value;
                else if (key == "dbname") dbname = value;
                else if (key == "port") port = std::stoi(value);
            }
        }
        return true;
    }
};
