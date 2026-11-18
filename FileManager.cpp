#include "FileManager.h"
#include <fstream>
#include <sstream>

std::vector<std::string> FileManager::split(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream tokenStream(line);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// ==========================================
// Students File Handling
// ==========================================
void FileManager::saveStudents(const StudentLinkedList& list) {
    std::ofstream file(studentFile);
    if (!file.is_open()) return;

    std::vector<Student> students = list.getAllStudents();
    for (const auto& s : students) {
        file << s.studentID << "|"
             << s.firstName << "|"
             << s.lastName << "|"
             << s.age << "|"
             << s.gender << "|"
             << s.course << "|"
             << s.yearLevel << "|"
             << s.section << "\n";
    }
    file.close();
}

void FileManager::loadStudents(StudentLinkedList& list) {
    list.clear();
    std::ifstream file(studentFile);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = split(line, '|');
        if (parts.size() >= 8) {
            Student s(parts[0], parts[1], parts[2], std::stoi(parts[3]),
                      parts[4], parts[5], std::stoi(parts[6]), parts[7]);
            list.insertStudent(s);
        }
    }
    file.close();
}

// ==========================================
// Subjects File Handling
// ==========================================
void FileManager::saveSubjects(const std::vector<Subject>& subjects) {
    std::ofstream file(subjectFile);
    if (!file.is_open()) return;

    for (const auto& sub : subjects) {
        file << sub.subjectCode << "|"
             << sub.subjectName << "|"
             << sub.description << "|"
             << sub.units << "\n";
    }
    file.close();
}

void FileManager::loadSubjects(std::vector<Subject>& subjects) {
    subjects.clear();
    std::ifstream file(subjectFile);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = split(line, '|');
        if (parts.size() >= 4) {
            Subject sub(parts[0], parts[1], parts[2], std::stoi(parts[3]));
            subjects.push_back(sub);
        }
    }
    file.close();
}

// ==========================================
// Grades File Handling
// ==========================================
void FileManager::saveGrades(const std::vector<Grade>& grades) {
    std::ofstream file(gradeFile);
    if (!file.is_open()) return;

    for (const auto& g : grades) {
        file << g.studentID << "|"
             << g.subjectCode << "|"
             << g.gradeValue << "|"
             << g.units << "\n";
    }
    file.close();
}

void FileManager::loadGrades(std::vector<Grade>& grades) {
    grades.clear();
    std::ifstream file(gradeFile);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = split(line, '|');
        if (parts.size() >= 4) {
            Grade g(parts[0], parts[1], std::stod(parts[2]), std::stoi(parts[3]));
            grades.push_back(g);
        }
    }
    file.close();
}

// ==========================================
// Report Card Export
// ==========================================
bool FileManager::exportReportCard(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << content;
    file.close();
    return true;
}