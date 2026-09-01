#pragma once
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>

inline std::string generateSalt() {
    static std::mt19937_64 rng(std::random_device{}());
    static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string salt;
    for (int i = 0; i < 16; i++) {
        salt += chars[rng() % (sizeof(chars) - 1)];
    }
    return salt;
}

inline std::string hexEncode(const unsigned char* data, size_t len) {
    std::ostringstream oss;
    for (size_t i = 0; i < len; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return oss.str();
}

inline std::string sha256Hex(const std::string& input) {
    unsigned char hash[32];
    memset(hash, 0, sizeof(hash));
    // Use MySQL SHA2 via query — this is just a placeholder for the actual
    // MySQL-side SHA2 call. All hashing is done server-side.
    (void)input;
    return "";
}

// Compute password hash that matches MySQL SHA2(CONCAT(pass, salt), 256)
// We use MySQL's SHA2 function via queries — this is a wrapper for the
// hash computation that happens on the MySQL server.
inline std::string getPasswordHashQuery(const std::string& password, const std::string& salt) {
    std::string escapedPass = password;
    std::string escapedSalt = salt;
    // Simple escape for single quotes
    auto replaceAll = [](std::string& s, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != std::string::npos) {
            s.replace(pos, from.length(), to);
            pos += to.length();
        }
    };
    replaceAll(escapedPass, "'", "\\'");
    replaceAll(escapedSalt, "'", "\\'");
    return "SELECT SHA2(CONCAT('" + escapedPass + "', '" + escapedSalt + "'), 256)";
}
