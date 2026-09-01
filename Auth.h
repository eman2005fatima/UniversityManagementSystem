#pragma once
#include <string>
#include "Database.h"
#include "Models.h"
#include "utils/Validators.h"
#include "utils/Crypto.h"

struct LoginResult {
    bool success = false;
    std::string message;
    UserSession session;
};

class Auth {
public:
    static LoginResult login(Database& db, const std::string& email,
                             const std::string& password, const std::string& role);
    static std::string signupStudent(Database& db, const std::string& name,
                                     const std::string& email, const std::string& password,
                                     const std::string& dept, int enrollYear,
                                     const std::string& phone,
                                     const std::string& secQuestion,
                                     const std::string& secAnswer);
    static std::string signupFaculty(Database& db, const std::string& name,
                                     const std::string& email, const std::string& password,
                                     const std::string& dept, const std::string& designation,
                                     const std::string& qualification,
                                     const std::string& secQuestion,
                                     const std::string& secAnswer);
    static std::string forgotPassword(Database& db, const std::string& email,
                                      const std::string& role, const std::string& answer,
                                      const std::string& newPass);
    static std::string getSecurityQuestion(Database& db, const std::string& email,
                                            const std::string& role);
    static std::string changePassword(Database& db, int userId,
                                       const std::string& oldPass,
                                       const std::string& newPass);
};
