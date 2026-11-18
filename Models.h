#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

// ==========================================
// PART 1: Student Class
// ==========================================
class Student {
public:
    std::string studentID;
    std::string firstName;
    std::string lastName;
    int age;
    std::string gender;
    std::string course;
    int yearLevel;
    std::string section;

    Student() : age(0), yearLevel(1) {}
    Student(std::string id, std::string fName, std::string lName, int a,
            std::string g, std::string c, int yLevel, std::string sec)
        : studentID(id), firstName(fName), lastName(lName), age(a),
          gender(g), course(c), yearLevel(yLevel), section(sec) {}

    std::string getFullName() const {
        return firstName + " " + lastName;
    }
};

// ==========================================
// PART 7: Linked List for Student Records
// ==========================================
struct StudentNode {
    Student data;
    StudentNode* next;
    StudentNode(const Student& s) : data(s), next(nullptr) {}
};

class StudentLinkedList {
private:
    StudentNode* head;

    static std::string toLower(const std::string& str) {
        std::string res = str;
        std::transform(res.begin(), res.end(), res.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return res;
    }

public:
    StudentLinkedList() : head(nullptr) {}

    ~StudentLinkedList() {
        clear();
    }

    void clear() {
        StudentNode* current = head;
        while (current != nullptr) {
            StudentNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
    }

    // Part 1.10: Prevent duplicate IDs & Part 7.2: Insertion
    bool insertStudent(const Student& s) {
        if (searchByID(s.studentID) != nullptr) {
            return false; // Duplicate ID found
        }
        StudentNode* newNode = new StudentNode(s);
        if (!head) {
            head = newNode;
        } else {
            StudentNode* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        return true;
    }

    // Part 2.1 & Part 7.6: Linear Search by Student ID
    Student* searchByID(const std::string& id) const {
        StudentNode* temp = head;
        while (temp) {
            if (temp->data.studentID == id) {
                return &(temp->data);
            }
            temp = temp->next;
        }
        return nullptr;
    }

    // Part 2.2 & Part 7.6: Linear Search by Student Name (partial or full, case-insensitive)
    std::vector<Student> searchByName(const std::string& query) const {
        std::vector<Student> results;
        std::string q = toLower(query);
        StudentNode* temp = head;
        while (temp) {
            std::string full = toLower(temp->data.getFullName());
            if (full.find(q) != std::string::npos) {
                results.push_back(temp->data);
            }
            temp = temp->next;
        }
        return results;
    }

    // Part 2.5 & Part 7.4: Deletion of student node
    bool deleteByID(const std::string& id) {
        if (!head) return false;

        if (head->data.studentID == id) {
            StudentNode* toDelete = head;
            head = head->next;
            delete toDelete;
            return true;
        }

        StudentNode* current = head;
        while (current->next && current->next->data.studentID != id) {
            current = current->next;
        }

        if (current->next) {
            StudentNode* toDelete = current->next;
            current->next = current->next->next;
            delete toDelete;
            return true;
        }
        return false;
    }

    // Part 7.7: Sort Linked List by Student ID (Bubble Sort)
    void sortByID() {
        if (!head || !head->next) return;
        bool swapped;
        do {
            swapped = false;
            StudentNode* ptr = head;
            while (ptr->next) {
                if (ptr->data.studentID > ptr->next->data.studentID) {
                    std::swap(ptr->data, ptr->next->data);
                    swapped = true;
                }
                ptr = ptr->next;
            }
        } while (swapped);
    }

    // Part 7.8: Sort Linked List by Student Name (Bubble Sort)
    void sortByName() {
        if (!head || !head->next) return;
        bool swapped;
        do {
            swapped = false;
            StudentNode* ptr = head;
            while (ptr->next) {
                if (toLower(ptr->data.lastName + ptr->data.firstName) >
                    toLower(ptr->next->data.lastName + ptr->next->data.firstName)) {
                    std::swap(ptr->data, ptr->next->data);
                    swapped = true;
                }
                ptr = ptr->next;
            }
        } while (swapped);
    }

    // Part 7.3: Traversal to retrieve all records
    std::vector<Student> getAllStudents() const {
        std::vector<Student> list;
        StudentNode* temp = head;
        while (temp) {
            list.push_back(temp->data);
            temp = temp->next;
        }
        return list;
    }
};

// ==========================================
// PART 3: Subject Class
// ==========================================
class Subject {
public:
    std::string subjectCode;
    std::string subjectName;
    std::string description;
    int units;

    Subject() : units(0) {}
    Subject(std::string code, std::string name, std::string desc, int u)
        : subjectCode(code), subjectName(name), description(desc), units(u) {}
};

// ==========================================
// PART 4: Grade Class
// ==========================================
class Grade {
public:
    std::string studentID;
    std::string subjectCode;
    double gradeValue; // e.g., 1.00 (Highest) to 5.00 (Failed); 0.00 = Enrolled / Ungraded
    int units;

    Grade() : gradeValue(0.0), units(0) {}
    Grade(std::string sID, std::string subCode, double gVal, int u)
        : studentID(sID), subjectCode(subCode), gradeValue(gVal), units(u) {}
};

#endif // MODELS_H