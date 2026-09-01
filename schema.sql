-- University Management System - Database Schema
-- Run this script to set up the database:
-- mysql -u root -p < schema.sql

CREATE DATABASE IF NOT EXISTS university_db;
USE university_db;

-- ============================================================
-- USERS TABLE (unified for authentication)
-- ============================================================
CREATE TABLE IF NOT EXISTS users (
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(64) NOT NULL,
    password_salt VARCHAR(32) NOT NULL,
    role ENUM('admin', 'student', 'faculty') NOT NULL,
    security_question VARCHAR(200) DEFAULT NULL,
    security_answer_hash VARCHAR(64) DEFAULT NULL,
    failed_attempts INT DEFAULT 0,
    is_locked TINYINT(1) DEFAULT 0,
    lock_time DATETIME DEFAULT NULL,
    last_login DATETIME DEFAULT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    status ENUM('active', 'pending', 'disabled') DEFAULT 'active'
);

-- ============================================================
-- PASSWORD HISTORY (last 3 passwords)
-- ============================================================
CREATE TABLE IF NOT EXISTS password_history (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    password_hash VARCHAR(64) NOT NULL,
    password_salt VARCHAR(32) NOT NULL,
    changed_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- ============================================================
-- STUDENTS
-- ============================================================
CREATE TABLE IF NOT EXISTS students (
    student_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    department VARCHAR(100) NOT NULL,
    enrollment_year INT NOT NULL,
    phone VARCHAR(20) DEFAULT NULL,
    semester INT DEFAULT 1,
    total_credits INT DEFAULT 0,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- ============================================================
-- FACULTY
-- ============================================================
CREATE TABLE IF NOT EXISTS faculty (
    faculty_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    department VARCHAR(100) NOT NULL,
    designation VARCHAR(100) NOT NULL,
    qualification VARCHAR(200) DEFAULT NULL,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- ============================================================
-- ADMINS
-- ============================================================
CREATE TABLE IF NOT EXISTS admins (
    admin_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE
);

-- ============================================================
-- COURSES
-- ============================================================
CREATE TABLE IF NOT EXISTS courses (
    course_id INT AUTO_INCREMENT PRIMARY KEY,
    course_code VARCHAR(20) UNIQUE NOT NULL,
    course_name VARCHAR(100) NOT NULL,
    credit_hours INT NOT NULL,
    department VARCHAR(100) NOT NULL,
    faculty_id INT DEFAULT NULL,
    semester INT NOT NULL,
    max_students INT DEFAULT 50,
    FOREIGN KEY (faculty_id) REFERENCES faculty(faculty_id) ON DELETE SET NULL
);

-- ============================================================
-- ENROLLMENTS
-- ============================================================
CREATE TABLE IF NOT EXISTS enrollments (
    enrollment_id INT AUTO_INCREMENT PRIMARY KEY,
    student_id INT NOT NULL,
    course_id INT NOT NULL,
    semester VARCHAR(10) NOT NULL,
    year INT NOT NULL,
    grade VARCHAR(2) DEFAULT NULL,
    grade_points FLOAT DEFAULT 0,
    FOREIGN KEY (student_id) REFERENCES students(student_id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(course_id) ON DELETE CASCADE,
    UNIQUE KEY (student_id, course_id, semester, year)
);

-- ============================================================
-- ATTENDANCE
-- ============================================================
CREATE TABLE IF NOT EXISTS attendance (
    attendance_id INT AUTO_INCREMENT PRIMARY KEY,
    student_id INT NOT NULL,
    course_id INT NOT NULL,
    date DATE NOT NULL,
    status ENUM('present', 'absent', 'late') NOT NULL,
    FOREIGN KEY (student_id) REFERENCES students(student_id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(course_id) ON DELETE CASCADE,
    UNIQUE KEY (student_id, course_id, date)
);

-- ============================================================
-- TIMETABLE (NEW)
-- ============================================================
CREATE TABLE IF NOT EXISTS timetable (
    slot_id INT AUTO_INCREMENT PRIMARY KEY,
    course_id INT NOT NULL,
    day_of_week ENUM('Mon','Tue','Wed','Thu','Fri','Sat') NOT NULL,
    start_time TIME NOT NULL,
    end_time TIME NOT NULL,
    room VARCHAR(50) DEFAULT NULL,
    FOREIGN KEY (course_id) REFERENCES courses(course_id) ON DELETE CASCADE,
    UNIQUE KEY (course_id, day_of_week, start_time)
);

-- ============================================================
-- ANNOUNCEMENTS (NEW)
-- ============================================================
CREATE TABLE IF NOT EXISTS announcements (
    id INT AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(200) NOT NULL,
    body TEXT NOT NULL,
    posted_by INT NOT NULL,
    posted_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    expires_at DATE DEFAULT NULL,
    FOREIGN KEY (posted_by) REFERENCES users(user_id) ON DELETE CASCADE
);

-- ============================================================
-- INDEXES FOR PERFORMANCE
-- ============================================================
CREATE INDEX IF NOT EXISTS idx_enrollments_student ON enrollments(student_id);
CREATE INDEX IF NOT EXISTS idx_enrollments_course ON enrollments(course_id);
CREATE INDEX IF NOT EXISTS idx_attendance_student ON attendance(student_id);
CREATE INDEX IF NOT EXISTS idx_attendance_course ON attendance(course_id);
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
CREATE INDEX IF NOT EXISTS idx_announcements_expires ON announcements(expires_at);

-- ============================================================
-- TRIGGER: auto-update students.total_credits
-- ============================================================
DELIMITER $$
CREATE TRIGGER IF NOT EXISTS trg_update_credits
AFTER UPDATE ON enrollments
FOR EACH ROW
BEGIN
    IF NEW.grade IS NOT NULL THEN
        UPDATE students s
        SET s.total_credits = (
            SELECT COALESCE(SUM(c.credit_hours), 0)
            FROM enrollments e
            JOIN courses c ON e.course_id = c.course_id
            WHERE e.student_id = NEW.student_id
              AND e.grade IS NOT NULL
        )
        WHERE s.student_id = NEW.student_id;
    END IF;
END$$
DELIMITER ;

-- ============================================================
-- DEFAULT DATA - Admin account
-- Password: ...123 (SHA-256 with salt 'adminsalt123')
-- ============================================================
INSERT IGNORE INTO users (email, password_hash, password_salt, role, status) VALUES
('admin@university.edu',
 '8480af683e5721074eb27488b98506ba3f9ac9ae26332bccc1230f2b0ade74c2',
 'adminsalt123', 'admin', 'active');

INSERT IGNORE INTO admins (user_id, name, email) VALUES
(1, 'System Admin', 'admin@university.edu');

-- Default Student account
INSERT IGNORE INTO users (email, password_hash, password_salt, role, status) VALUES
('student@university.edu',
 '9d18df678b1fafe28f1b40b739e5cbcdd6a07bf7cf71448a43a235e54fe763ad',
 'studentsalt1', 'student', 'active');

INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester) VALUES
(2, 'Default Student', 'student@university.edu', 'Computer Science', 2024, '03000000000', 3);

-- Default Faculty account
INSERT IGNORE INTO users (email, password_hash, password_salt, role, status) VALUES
('faculty@university.edu',
 'b96c8177ceebb79ee2a21b39e3b3793c510f4296444118cc8e5db6fa0927a338',
 'facultysalt1', 'faculty', 'active');

INSERT IGNORE INTO faculty (user_id, name, email, department, designation, qualification) VALUES
(3, 'Default Faculty', 'faculty@university.edu', 'Computer Science', 'Professor', 'PhD Computer Science');
