#include "Auth.h"
#include <sstream>

LoginResult Auth::login(Database& db, const std::string& email,
                        const std::string& password, const std::string& role) {
    LoginResult result;
    std::string escapedEmail = db.escape(email);
    std::string escapedRole = db.escape(role);

    std::string q = "SELECT u.user_id, u.email, u.password_hash, u.password_salt, u.role, "
                    "u.failed_attempts, u.is_locked, u.lock_time, u.status, "
                    "COALESCE(a.name,'') as name "
                    "FROM users u "
                    "LEFT JOIN admins a ON u.user_id = a.user_id AND u.role='admin' "
                    "WHERE u.email = '" + escapedEmail + "' AND u.role = '" + escapedRole + "'";
    MYSQL_RES* res = db.execQuery(q);
    if (!res) {
        result.message = "Database error occurred.";
        return result;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        result.message = "No account found with this email and role.";
        return result;
    }

    int userId = atoi(row[0]);
    std::string dbHash = row[2] ? row[2] : "";
    std::string dbSalt = row[3] ? row[3] : "";
    int failedAttempts = row[5] ? atoi(row[5]) : 0;
    int isLocked = row[6] ? atoi(row[6]) : 0;
    std::string lockTime = row[7] ? row[7] : "";
    std::string status = row[8] ? row[8] : "active";
    std::string userName = row[9] ? row[9] : "User";

    if (status != "active") {
        mysql_free_result(res);
        if (status == "disabled")
            result.message = "Account disabled by admin.";
        else
            result.message = "Account is not active.";
        return result;
    }

    if (isLocked) {
        if (!lockTime.empty()) {
            std::string unlockQ = "SELECT TIMESTAMPDIFF(MINUTE, '" + lockTime + "', NOW())";
            MYSQL_RES* lr = db.execQuery(unlockQ);
            if (lr) {
                MYSQL_ROW lrow = mysql_fetch_row(lr);
                int minsPassed = lrow ? atoi(lrow[0]) : 0;
                mysql_free_result(lr);
                if (minsPassed >= 15) {
                    db.query("UPDATE users SET is_locked = 0, failed_attempts = 0, lock_time = NULL WHERE user_id = " + std::to_string(userId));
                    isLocked = 0;
                } else {
                    mysql_free_result(res);
                    int remaining = 15 - minsPassed;
                    result.message = "Account locked — try again in " + std::to_string(remaining) + " minutes.";
                    return result;
                }
            }
        }
        if (isLocked) {
            mysql_free_result(res);
            result.message = "Account locked — try again later.";
            return result;
        }
    }

    std::string verifyQ = "SELECT SHA2(CONCAT('" + db.escape(password) + "', '" + dbSalt + "'), 256)";
    MYSQL_RES* vr = db.execQuery(verifyQ);
    if (!vr) {
        mysql_free_result(res);
        result.message = "Database error occurred.";
        return result;
    }
    MYSQL_ROW vrow = mysql_fetch_row(vr);
    std::string inputHash = vrow ? vrow[0] : "";
    mysql_free_result(vr);

    if (inputHash != dbHash) {
        failedAttempts++;
        if (failedAttempts >= 3) {
            db.query("UPDATE users SET is_locked = 1, lock_time = NOW(), failed_attempts = " +
                     std::to_string(failedAttempts) + " WHERE user_id = " + std::to_string(userId));
            mysql_free_result(res);
            result.message = "Wrong password. Account locked — try again in 15 minutes.";
        } else {
            db.query("UPDATE users SET failed_attempts = " + std::to_string(failedAttempts) +
                     " WHERE user_id = " + std::to_string(userId));
            mysql_free_result(res);
            result.message = "Wrong password.";
        }
        return result;
    }

    db.query("UPDATE users SET failed_attempts = 0, is_locked = 0, lock_time = NULL, last_login = NOW() WHERE user_id = " +
             std::to_string(userId));

    result.success = true;
    result.session.userId = userId;
    result.session.email = row[1] ? row[1] : "";
    result.session.role = role;
    result.session.name = userName;
    result.session.loggedIn = true;
    mysql_free_result(res);
    return result;
}

std::string Auth::signupStudent(Database& db, const std::string& name,
                                 const std::string& email, const std::string& password,
                                 const std::string& dept, int enrollYear,
                                 const std::string& phone,
                                 const std::string& secQuestion,
                                 const std::string& secAnswer) {
    if (!isStrongPassword(password))
        return "Password must be 8+ characters with uppercase, lowercase, digit, and special character (!@#$%^&*).";

    if (!isValidEmail(email))
        return "Invalid email format.";

    std::string q = "SELECT user_id FROM users WHERE email = '" + db.escape(email) + "'";
    MYSQL_RES* cr = db.execQuery(q);
    if (cr) {
        if (mysql_fetch_row(cr)) {
            mysql_free_result(cr);
            return "Email already exists.";
        }
        mysql_free_result(cr);
    }

    std::string salt = generateSalt();
    std::string hashQ = "SELECT SHA2(CONCAT('" + db.escape(password) + "', '" + salt + "'), 256)";
    MYSQL_RES* hr = db.execQuery(hashQ);
    if (!hr) return "Database error occurred.";
    MYSQL_ROW hrow = mysql_fetch_row(hr);
    std::string pwdHash = hrow ? hrow[0] : "";
    mysql_free_result(hr);
    if (pwdHash.empty()) return "Database error occurred.";

    std::string secHashQ = "SELECT SHA2('" + db.escape(secAnswer) + "', 256)";
    MYSQL_RES* shr = db.execQuery(secHashQ);
    if (!shr) return "Database error occurred.";
    MYSQL_ROW srow = mysql_fetch_row(shr);
    std::string ansHash = srow ? srow[0] : "";
    mysql_free_result(shr);

    db.beginTransaction();

    q = "INSERT INTO users (email, password_hash, password_salt, role, security_question, security_answer_hash, status) VALUES ('"
        + db.escape(email) + "', '" + pwdHash + "', '" + salt + "', 'student', '"
        + db.escape(secQuestion) + "', '" + ansHash + "', 'active')";
    if (!db.query(q)) {
        db.rollback();
        return "Registration failed. Database error.";
    }

    int userId = db.getLastInsertId();
    q = "INSERT INTO students (user_id, name, email, department, enrollment_year, phone, semester) VALUES ("
        + std::to_string(userId) + ", '" + db.escape(name) + "', '" + db.escape(email) + "', '"
        + db.escape(dept) + "', " + std::to_string(enrollYear) + ", '" + db.escape(phone) + "', 1)";
    if (!db.query(q)) {
        db.rollback();
        return "Registration failed. Database error.";
    }

    db.commit();
    return "";
}

std::string Auth::signupFaculty(Database& db, const std::string& name,
                                 const std::string& email, const std::string& password,
                                 const std::string& dept, const std::string& designation,
                                 const std::string& qualification,
                                 const std::string& secQuestion,
                                 const std::string& secAnswer) {
    if (!isStrongPassword(password))
        return "Password must be 8+ characters with uppercase, lowercase, digit, and special character (!@#$%^&*).";

    if (!isValidEmail(email))
        return "Invalid email format.";

    std::string q = "SELECT user_id FROM users WHERE email = '" + db.escape(email) + "'";
    MYSQL_RES* cr = db.execQuery(q);
    if (cr) {
        if (mysql_fetch_row(cr)) {
            mysql_free_result(cr);
            return "Email already exists.";
        }
        mysql_free_result(cr);
    }

    std::string salt = generateSalt();
    std::string hashQ = "SELECT SHA2(CONCAT('" + db.escape(password) + "', '" + salt + "'), 256)";
    MYSQL_RES* hr = db.execQuery(hashQ);
    if (!hr) return "Database error occurred.";
    MYSQL_ROW hrow = mysql_fetch_row(hr);
    std::string pwdHash = hrow ? hrow[0] : "";
    mysql_free_result(hr);
    if (pwdHash.empty()) return "Database error occurred.";

    std::string secHashQ = "SELECT SHA2('" + db.escape(secAnswer) + "', 256)";
    MYSQL_RES* shr = db.execQuery(secHashQ);
    if (!shr) return "Database error occurred.";
    MYSQL_ROW srow = mysql_fetch_row(shr);
    std::string ansHash = srow ? srow[0] : "";
    mysql_free_result(shr);

    db.beginTransaction();

    q = "INSERT INTO users (email, password_hash, password_salt, role, security_question, security_answer_hash, status) VALUES ('"
        + db.escape(email) + "', '" + pwdHash + "', '" + salt + "', 'faculty', '"
        + db.escape(secQuestion) + "', '" + ansHash + "', 'active')";
    if (!db.query(q)) {
        db.rollback();
        return "Registration failed. Database error.";
    }

    int userId = db.getLastInsertId();
    q = "INSERT INTO faculty (user_id, name, email, department, designation, qualification) VALUES ("
        + std::to_string(userId) + ", '" + db.escape(name) + "', '" + db.escape(email) + "', '"
        + db.escape(dept) + "', '" + db.escape(designation) + "', '" + db.escape(qualification) + "')";
    if (!db.query(q)) {
        db.rollback();
        return "Registration failed. Database error.";
    }

    db.commit();
    return "";
}

std::string Auth::forgotPassword(Database& db, const std::string& email,
                                  const std::string& role, const std::string& answer,
                                  const std::string& newPass) {
    std::string q = "SELECT user_id, security_question, security_answer_hash, password_salt FROM users WHERE email = '"
                    + db.escape(email) + "' AND role = '" + db.escape(role) + "'";
    MYSQL_RES* res = db.execQuery(q);
    if (!res) return "Database error occurred.";
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) { mysql_free_result(res); return "No account found with this email and role."; }
    int userId = atoi(row[0]);
    std::string answerHash = row[2] ? row[2] : "";
    mysql_free_result(res);

    std::string checkQ = "SELECT SHA2('" + db.escape(trim(answer)) + "', 256)";
    MYSQL_RES* cr = db.execQuery(checkQ);
    if (!cr) return "Database error occurred.";
    MYSQL_ROW crow = mysql_fetch_row(cr);
    std::string inputHash = crow ? crow[0] : "";
    mysql_free_result(cr);
    if (inputHash != answerHash) return "Security answer is incorrect.";

    if (!isStrongPassword(newPass))
        return "Password must be 8+ characters with uppercase, lowercase, digit, and special character.";

    // Check password history
    q = "SELECT password_hash, password_salt FROM password_history WHERE user_id = "
        + std::to_string(userId) + " ORDER BY changed_at DESC LIMIT 3";
    MYSQL_RES* phr = db.execQuery(q);
    if (phr) {
        MYSQL_ROW prow;
        while ((prow = mysql_fetch_row(phr))) {
            std::string oldHashQ = "SELECT SHA2(CONCAT('" + db.escape(newPass) + "', '" + (prow[1] ? prow[1] : "") + "'), 256)";
            MYSQL_RES* ohr = db.execQuery(oldHashQ);
            if (ohr) {
                MYSQL_ROW ohrow = mysql_fetch_row(ohr);
                if (ohrow && prow[0] && ohrow[0] == prow[0]) {
                    mysql_free_result(ohr);
                    mysql_free_result(phr);
                    return "Cannot reuse any of your last 3 passwords.";
                }
                mysql_free_result(ohr);
            }
        }
        mysql_free_result(phr);
    }

    std::string newSalt = generateSalt();
    std::string newHashQ = "SELECT SHA2(CONCAT('" + db.escape(newPass) + "', '" + newSalt + "'), 256)";
    MYSQL_RES* nhr = db.execQuery(newHashQ);
    if (!nhr) return "Database error occurred.";
    MYSQL_ROW nhrow = mysql_fetch_row(nhr);
    std::string newHash = nhrow ? nhrow[0] : "";
    mysql_free_result(nhr);

    db.query("UPDATE users SET password_hash = '" + newHash + "', password_salt = '" + newSalt +
             "', failed_attempts = 0, is_locked = 0, lock_time = NULL WHERE user_id = " + std::to_string(userId));
    db.query("INSERT INTO password_history (user_id, password_hash, password_salt) VALUES (" +
             std::to_string(userId) + ", '" + newHash + "', '" + newSalt + "')");
    db.query("DELETE FROM password_history WHERE user_id = " + std::to_string(userId) +
             " AND id NOT IN (SELECT id FROM (SELECT id FROM password_history WHERE user_id = " +
             std::to_string(userId) + " ORDER BY changed_at DESC LIMIT 3) AS t)");
    return "";
}

std::string Auth::getSecurityQuestion(Database& db, const std::string& email,
                                       const std::string& role) {
    std::string q = "SELECT security_question FROM users WHERE email = '"
                    + db.escape(email) + "' AND role = '" + db.escape(role) + "'";
    MYSQL_RES* res = db.execQuery(q);
    if (!res) return "";
    MYSQL_ROW row = mysql_fetch_row(res);
    std::string question = (row && row[0]) ? row[0] : "";
    mysql_free_result(res);
    return question;
}

std::string Auth::changePassword(Database& db, int userId,
                                  const std::string& oldPass,
                                  const std::string& newPass) {
    std::string q = "SELECT password_hash, password_salt FROM users WHERE user_id = "
                    + std::to_string(userId);
    MYSQL_RES* sr = db.execQuery(q);
    if (!sr) return "Database error occurred.";
    MYSQL_ROW r = mysql_fetch_row(sr);
    if (!r) { mysql_free_result(sr); return "User not found."; }
    std::string dbHash = r[0] ? r[0] : "";
    std::string dbSalt = r[1] ? r[1] : "";
    mysql_free_result(sr);

    std::string verifyQ = "SELECT SHA2(CONCAT('" + db.escape(oldPass) + "', '" + dbSalt + "'), 256)";
    MYSQL_RES* vr = db.execQuery(verifyQ);
    if (!vr) return "Database error occurred.";
    MYSQL_ROW vrow = mysql_fetch_row(vr);
    std::string inputHash = vrow ? vrow[0] : "";
    mysql_free_result(vr);
    if (inputHash != dbHash) return "Current password is incorrect.";

    if (!isStrongPassword(newPass))
        return "New password must be 8+ characters with uppercase, lowercase, digit, and special character.";

    std::string newSalt = generateSalt();
    std::string newHashQ = "SELECT SHA2(CONCAT('" + db.escape(newPass) + "', '" + newSalt + "'), 256)";
    MYSQL_RES* nhr = db.execQuery(newHashQ);
    if (!nhr) return "Database error occurred.";
    MYSQL_ROW nhrow = mysql_fetch_row(nhr);
    std::string newHash2 = nhrow ? nhrow[0] : "";
    mysql_free_result(nhr);

    db.query("UPDATE users SET password_hash = '" + newHash2 + "', password_salt = '" + newSalt +
             "' WHERE user_id = " + std::to_string(userId));
    db.query("INSERT INTO password_history (user_id, password_hash, password_salt) VALUES (" +
             std::to_string(userId) + ", '" + newHash2 + "', '" + newSalt + "')");
    return "";
}
