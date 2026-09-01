#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <functional>

inline std::string getCurrentDateStr() {
    time_t t = time(nullptr);
    struct tm buf;
    localtime_s(&buf, &t);
    std::ostringstream oss;
    oss << (buf.tm_year + 1900) << "-"
        << std::setw(2) << std::setfill('0') << (buf.tm_mon + 1) << "-"
        << std::setw(2) << std::setfill('0') << buf.tm_mday;
    return oss.str();
}

template<typename Row>
bool exportCSV(const std::vector<Row>& rows, const std::string& tableName,
               const std::vector<std::string>& headers,
               std::function<std::string(const Row&)> rowToString) {
    std::string filename = tableName + "_" + getCurrentDateStr() + ".csv";
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    // Write headers
    for (size_t i = 0; i < headers.size(); i++) {
        if (i > 0) file << ",";
        file << "\"" << headers[i] << "\"";
    }
    file << "\n";

    // Write rows
    for (const auto& row : rows) {
        file << rowToString(row) << "\n";
    }

    file.close();
    return true;
}
