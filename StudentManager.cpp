#include "StudentManager.h"

bool StudentManager::loadFromDB(Database& db) {
    cache_.clear();
    MYSQL_RES* res = db.execQuery("SELECT student_id, user_id, name, email, department, enrollment_year, phone, semester, total_credits FROM students");
    if (!res) return false;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Student s;
        s.studentId = row[0] ? atoi(row[0]) : 0;
        s.userId = row[1] ? atoi(row[1]) : 0;
        s.name = row[2] ? row[2] : "";
        s.email = row[3] ? row[3] : "";
        s.department = row[4] ? row[4] : "";
        s.enrollmentYear = row[5] ? atoi(row[5]) : 2024;
        s.phone = row[6] ? row[6] : "";
        s.semester = row[7] ? atoi(row[7]) : 1;
        s.totalCredits = row[8] ? atoi(row[8]) : 0;
        cache_[s.studentId] = s;
    }
    mysql_free_result(res);
    return true;
}

Student* StudentManager::findById(int id) {
    auto it = cache_.find(id);
    if (it != cache_.end()) return &it->second;
    return nullptr;
}

std::vector<Student> StudentManager::getAll() {
    std::vector<Student> result;
    for (const auto& [id, s] : cache_)
        result.push_back(s);
    return result;
}

std::vector<Student> StudentManager::getFiltered(const std::string& search, const std::string& dept) {
    std::vector<Student> result;
    std::string searchLower;
    for (char c : search) searchLower += (c >= 'A' && c <= 'Z') ? (c + 32) : c;

    for (const auto& [id, s] : cache_) {
        if (!dept.empty() && s.department != dept) continue;
        if (!search.empty()) {
            std::string nameLower;
            for (char c : s.name) nameLower += (c >= 'A' && c <= 'Z') ? (c + 32) : c;
            if (nameLower.find(searchLower) == std::string::npos) continue;
        }
        result.push_back(s);
    }
    return result;
}

int StudentManager::count() {
    return (int)cache_.size();
}

bool StudentManager::addStudent(Database& db, const Student& s) {
    std::string q = "INSERT INTO students (user_id, name, email, department, enrollment_year, phone, semester) VALUES ("
                    + std::to_string(s.userId) + ", '" + db.escape(s.name) + "', '"
                    + db.escape(s.email) + "', '" + db.escape(s.department) + "', "
                    + std::to_string(s.enrollmentYear) + ", '" + db.escape(s.phone) + "', "
                    + std::to_string(s.semester) + ")";
    if (!db.query(q)) return false;
    cache_[s.studentId] = s;
    return true;
}

bool StudentManager::deleteStudent(Database& db, int id) {
    MYSQL_RES* er = db.execQuery("SELECT student_id FROM enrollments WHERE student_id = " + std::to_string(id) + " LIMIT 1");
    if (er) {
        if (mysql_fetch_row(er)) { mysql_free_result(er); return false; }
        mysql_free_result(er);
    }
    if (!db.query("DELETE FROM students WHERE student_id = " + std::to_string(id))) return false;
    cache_.erase(id);
    return true;
}
