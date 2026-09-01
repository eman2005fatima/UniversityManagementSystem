-- Sample Data for University Management System
-- CE/CS Department Courses, Students, Faculty, and Assignments
-- All passwords are '...123'

USE university_db;

-- ============================================================
-- Helper: generate password hash using MySQL SHA2
-- ============================================================
-- All new accounts use password '...123'
-- We'll compute hashes inline using SELECT SHA2(CONCAT('...123', salt), 256)

-- ============================================================
-- SAMPLE FACULTY (Pakistani/Muslim names)
-- ============================================================
INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'dr.ahmed@university.edu', SHA2(CONCAT('...123', 'facsalt1'), 256), 'facsalt1', 'faculty', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'dr.ahmed@university.edu');
SET @u1 = (SELECT user_id FROM users WHERE email = 'dr.ahmed@university.edu');
INSERT IGNORE INTO faculty (user_id, name, email, department, designation, qualification)
VALUES (@u1, 'Dr. Ahmed Hussain', 'dr.ahmed@university.edu', 'Computer Science', 'Professor', 'PhD Computer Science');

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'dr.fatima@university.edu', SHA2(CONCAT('...123', 'facsalt2'), 256), 'facsalt2', 'faculty', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'dr.fatima@university.edu');
SET @u2 = (SELECT user_id FROM users WHERE email = 'dr.fatima@university.edu');
INSERT IGNORE INTO faculty (user_id, name, email, department, designation, qualification)
VALUES (@u2, 'Dr. Fatima Khan', 'dr.fatima@university.edu', 'Computer Engineering', 'Associate Professor', 'PhD Computer Engineering');

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'muhammad.ali@university.edu', SHA2(CONCAT('...123', 'facsalt3'), 256), 'facsalt3', 'faculty', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'muhammad.ali@university.edu');
SET @u3 = (SELECT user_id FROM users WHERE email = 'muhammad.ali@university.edu');
INSERT IGNORE INTO faculty (user_id, name, email, department, designation, qualification)
VALUES (@u3, 'Muhammad Ali', 'muhammad.ali@university.edu', 'Computer Science', 'Assistant Professor', 'MS Computer Science');

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'sana.malik@university.edu', SHA2(CONCAT('...123', 'facsalt4'), 256), 'facsalt4', 'faculty', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'sana.malik@university.edu');
SET @u4 = (SELECT user_id FROM users WHERE email = 'sana.malik@university.edu');
INSERT IGNORE INTO faculty (user_id, name, email, department, designation, qualification)
VALUES (@u4, 'Sana Malik', 'sana.malik@university.edu', 'Computer Science', 'Lecturer', 'BS Computer Science');

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'usman.raza@university.edu', SHA2(CONCAT('...123', 'facsalt5'), 256), 'facsalt5', 'faculty', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'usman.raza@university.edu');
SET @u5 = (SELECT user_id FROM users WHERE email = 'usman.raza@university.edu');
INSERT IGNORE INTO faculty (user_id, name, email, department, designation, qualification)
VALUES (@u5, 'Usman Raza', 'usman.raza@university.edu', 'Mathematics', 'Associate Professor', 'PhD Mathematics');

-- ============================================================
-- SAMPLE STUDENTS (Pakistani/Muslim names)
-- ============================================================
INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'abdullah.khan@university.edu', SHA2(CONCAT('...123', 'studsalt1'), 256), 'studsalt1', 'student', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'abdullah.khan@university.edu');
SET @s1 = (SELECT user_id FROM users WHERE email = 'abdullah.khan@university.edu');
INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester)
VALUES (@s1, 'Abdullah Khan', 'abdullah.khan@university.edu', 'Computer Science', 2023, '03001234567', 5);

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'aisha.ahmed@university.edu', SHA2(CONCAT('...123', 'studsalt2'), 256), 'studsalt2', 'student', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'aisha.ahmed@university.edu');
SET @s2 = (SELECT user_id FROM users WHERE email = 'aisha.ahmed@university.edu');
INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester)
VALUES (@s2, 'Aisha Ahmed', 'aisha.ahmed@university.edu', 'Computer Science', 2024, '03002345678', 3);

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'bilal.hassan@university.edu', SHA2(CONCAT('...123', 'studsalt3'), 256), 'studsalt3', 'student', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'bilal.hassan@university.edu');
SET @s3 = (SELECT user_id FROM users WHERE email = 'bilal.hassan@university.edu');
INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester)
VALUES (@s3, 'Bilal Hassan', 'bilal.hassan@university.edu', 'Computer Engineering', 2024, '03003456789', 3);

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'fatima.zahra@university.edu', SHA2(CONCAT('...123', 'studsalt4'), 256), 'studsalt4', 'student', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'fatima.zahra@university.edu');
SET @s4 = (SELECT user_id FROM users WHERE email = 'fatima.zahra@university.edu');
INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester)
VALUES (@s4, 'Fatima Zahra', 'fatima.zahra@university.edu', 'Computer Science', 2024, '03004567890', 3);

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'hassan.abbas@university.edu', SHA2(CONCAT('...123', 'studsalt5'), 256), 'studsalt5', 'student', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'hassan.abbas@university.edu');
SET @s5 = (SELECT user_id FROM users WHERE email = 'hassan.abbas@university.edu');
INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester)
VALUES (@s5, 'Hassan Abbas', 'hassan.abbas@university.edu', 'Computer Engineering', 2023, '03005678901', 5);

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'maryam.noor@university.edu', SHA2(CONCAT('...123', 'studsalt6'), 256), 'studsalt6', 'student', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'maryam.noor@university.edu');
SET @s6 = (SELECT user_id FROM users WHERE email = 'maryam.noor@university.edu');
INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester)
VALUES (@s6, 'Maryam Noor', 'maryam.noor@university.edu', 'Computer Science', 2024, '03006789012', 3);

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'omar.farooq@university.edu', SHA2(CONCAT('...123', 'studsalt7'), 256), 'studsalt7', 'student', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'omar.farooq@university.edu');
SET @s7 = (SELECT user_id FROM users WHERE email = 'omar.farooq@university.edu');
INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester)
VALUES (@s7, 'Omar Farooq', 'omar.farooq@university.edu', 'Computer Science', 2024, '03007890123', 1);

INSERT INTO users (email, password_hash, password_salt, role, status)
SELECT 'zainab.ali@university.edu', SHA2(CONCAT('...123', 'studsalt8'), 256), 'studsalt8', 'student', 'active'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE email = 'zainab.ali@university.edu');
SET @s8 = (SELECT user_id FROM users WHERE email = 'zainab.ali@university.edu');
INSERT IGNORE INTO students (user_id, name, email, department, enrollment_year, phone, semester)
VALUES (@s8, 'Zainab Ali', 'zainab.ali@university.edu', 'Computer Engineering', 2024, '03008901234', 1);

-- ============================================================
-- PASSWORD HISTORY for new users (all with '...123')
-- ============================================================
INSERT IGNORE INTO password_history (user_id, password_hash, password_salt)
SELECT user_id, password_hash, password_salt FROM users
WHERE email IN ('dr.ahmed@university.edu','dr.fatima@university.edu','muhammad.ali@university.edu',
                'sana.malik@university.edu','usman.raza@university.edu',
                'abdullah.khan@university.edu','aisha.ahmed@university.edu','bilal.hassan@university.edu',
                'fatima.zahra@university.edu','hassan.abbas@university.edu','maryam.noor@university.edu',
                'omar.farooq@university.edu','zainab.ali@university.edu');

-- ============================================================
-- COURSES (skip if code already exists)
-- ============================================================
INSERT IGNORE INTO courses (course_code, course_name, credit_hours, department, semester) VALUES
('CS101', 'Programming Fundamentals', 3, 'Computer Science', 1),
('CS102', 'Object Oriented Programming', 3, 'Computer Science', 2),
('CS103', 'Discrete Mathematics', 3, 'Computer Science', 1),
('CS201', 'Data Structures & Algorithms', 3, 'Computer Science', 3),
('CS202', 'Database Systems', 3, 'Computer Science', 3),
('CS203', 'Digital Logic Design', 3, 'Computer Science', 3),
('CS301', 'Computer Networks', 3, 'Computer Science', 4),
('CS302', 'Operating Systems', 3, 'Computer Science', 4),
('CS303', 'Theory of Automata', 3, 'Computer Science', 4),
('CS401', 'Software Engineering', 3, 'Computer Science', 5),
('CS402', 'Web Development', 3, 'Computer Science', 5),
('CS403', 'Compiler Construction', 3, 'Computer Science', 5),
('CS501', 'Artificial Intelligence', 3, 'Computer Science', 6),
('CS502', 'Machine Learning', 3, 'Computer Science', 6),
('CS503', 'Parallel Computing', 3, 'Computer Science', 6),
('CS601', 'Cyber Security', 3, 'Computer Science', 7),
('CS602', 'Cloud Computing', 3, 'Computer Science', 7),
('CS603', 'Big Data Analytics', 3, 'Computer Science', 7),
('CE101', 'Circuit Analysis', 3, 'Computer Engineering', 1),
('CE102', 'Programming for Engineers', 3, 'Computer Engineering', 1),
('CE201', 'Microprocessor & Interfacing', 3, 'Computer Engineering', 3),
('CE202', 'Embedded Systems', 3, 'Computer Engineering', 4),
('CE301', 'Computer Architecture', 3, 'Computer Engineering', 5),
('CE302', 'VLSI Design', 3, 'Computer Engineering', 6),
('MATH101', 'Calculus I', 3, 'Mathematics', 1),
('MATH102', 'Linear Algebra', 3, 'Mathematics', 2),
('MATH201', 'Probability & Statistics', 3, 'Mathematics', 3),
('ENG101', 'English Composition', 2, 'General', 1),
('ENG201', 'Technical Writing', 2, 'General', 3),
('ISL101', 'Islamic Studies', 2, 'General', 2),
('PSY101', 'Introduction to Psychology', 2, 'General', 4);

-- ============================================================
-- ASSIGN FACULTY TO COURSES
-- ============================================================
-- Dr. Ahmed Hussain (CS) -> CS upper-level courses
UPDATE courses SET faculty_id = (SELECT faculty_id FROM faculty WHERE email='dr.ahmed@university.edu')
WHERE course_code IN ('CS301','CS302','CS303','CS401','CS402');

-- Muhammad Ali (CS) -> CS courses
UPDATE courses SET faculty_id = (SELECT faculty_id FROM faculty WHERE email='muhammad.ali@university.edu')
WHERE course_code IN ('CS403','CS501','CS502','CS503','CS601','CS602','CS603');

-- Sana Malik (CS) -> CS foundation courses
UPDATE courses SET faculty_id = (SELECT faculty_id FROM faculty WHERE email='sana.malik@university.edu')
WHERE course_code IN ('CS101','CS102','CS103','CS201','CS202','CS203');

-- Dr. Fatima Khan (CE) -> CE courses
UPDATE courses SET faculty_id = (SELECT faculty_id FROM faculty WHERE email='dr.fatima@university.edu')
WHERE department = 'Computer Engineering';

-- Usman Raza (Math) -> Math courses
UPDATE courses SET faculty_id = (SELECT faculty_id FROM faculty WHERE email='usman.raza@university.edu')
WHERE department = 'Mathematics';

-- ============================================================
-- ENROLL STUDENTS IN COURSES
-- ============================================================
-- Abdullah Khan (CS, sem 5) -> CS301, CS302, CS303, CS401, MATH201
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT (SELECT student_id FROM students WHERE email='abdullah.khan@university.edu'), course_id, 'Spring', 2025
FROM courses WHERE course_code IN ('CS301','CS302','CS303','CS401','MATH201');

-- Aisha Ahmed (CS, sem 3) -> CS201, CS202, CS203, MATH102, ENG201
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT (SELECT student_id FROM students WHERE email='aisha.ahmed@university.edu'), course_id, 'Spring', 2025
FROM courses WHERE course_code IN ('CS201','CS202','CS203','MATH102','ENG201','ISL101');

-- Bilal Hassan (CE, sem 3) -> CE201, MATH101, ENG101, CE102
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT (SELECT student_id FROM students WHERE email='bilal.hassan@university.edu'), course_id, 'Spring', 2025
FROM courses WHERE course_code IN ('CE201','MATH101','ENG101','CE102');

-- Fatima Zahra (CS, sem 3) -> CS201, CS202, CS203, MATH102, ISL101
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT (SELECT student_id FROM students WHERE email='fatima.zahra@university.edu'), course_id, 'Spring', 2025
FROM courses WHERE course_code IN ('CS201','CS202','CS203','MATH102','ISL101','ENG201');

-- Hassan Abbas (CE, sem 5) -> CE301, MATH201, ENG201, CE202, CS301
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT (SELECT student_id FROM students WHERE email='hassan.abbas@university.edu'), course_id, 'Spring', 2025
FROM courses WHERE course_code IN ('CE301','MATH201','ENG201','CE202','CS301');

-- Maryam Noor (CS, sem 3) -> CS201, CS202, CS203, MATH102, ENG201
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT (SELECT student_id FROM students WHERE email='maryam.noor@university.edu'), course_id, 'Spring', 2025
FROM courses WHERE course_code IN ('CS201','MATH102','ENG201','ISL101');

-- Omar Farooq (CS, sem 1) -> CS101, CS103, MATH101, ENG101, ISL101
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT (SELECT student_id FROM students WHERE email='omar.farooq@university.edu'), course_id, 'Spring', 2025
FROM courses WHERE course_code IN ('CS101','CS103','MATH101','ENG101','ISL101');

-- Zainab Ali (CE, sem 1) -> CE101, CE102, MATH101, ENG101
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT (SELECT student_id FROM students WHERE email='zainab.ali@university.edu'), course_id, 'Spring', 2025
FROM courses WHERE course_code IN ('CE101','CE102','MATH101','ENG101');

-- ============================================================
-- Enroll default student in more courses
-- ============================================================
SET @def_student = (SELECT student_id FROM students WHERE user_id = 2 LIMIT 1);
INSERT IGNORE INTO enrollments (student_id, course_id, semester, year)
SELECT @def_student, course_id, 'Spring', 2025 FROM courses
WHERE course_code IN ('CS101', 'CS102', 'MATH101', 'ENG101', 'ISL101', 'CS103', 'MATH102');
