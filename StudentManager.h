#pragma once
#include <map>
#include <vector>
#include <string>
#include "Database.h"
#include "Models.h"

class StudentManager {
public:
    bool loadFromDB(Database& db);
    Student* findById(int id);
    std::vector<Student> getAll();
    std::vector<Student> getFiltered(const std::string& search, const std::string& dept);
    int count();
    bool addStudent(Database& db, const Student& s);
    bool deleteStudent(Database& db, int id);
private:
    std::map<int, Student> cache_;
};
