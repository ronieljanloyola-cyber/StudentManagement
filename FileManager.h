#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Models.h"
#include <string>
#include <vector>

class FileManager {
private:
    std::string studentFile = "students.txt";
    std::string subjectFile = "subjects.txt";
    std::string gradeFile   = "grades.txt";

    std::vector<std::string> split(const std::string& line, char delimiter);

public:
    // Students
    void saveStudents(const StudentLinkedList& list);
    void loadStudents(StudentLinkedList& list);

    // Subjects
    void saveSubjects(const std::vector<Subject>& subjects);
    void loadSubjects(std::vector<Subject>& subjects);

    // Grades
    void saveGrades(const std::vector<Grade>& grades);
    void loadGrades(std::vector<Grade>& grades);

    // Report Card Export (Part 6.11)
    bool exportReportCard(const std::string& filename, const std::string& content);
};

#endif // FILEMANAGER_H