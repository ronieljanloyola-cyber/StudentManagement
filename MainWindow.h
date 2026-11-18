#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
#include <QCloseEvent>

#include "Models.h"
#include "FileManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Part 5.11: Automatically save updated records before exiting
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Student CRUD & Sorting (Parts 1, 2, 7)
    void onAddStudent();
    void onUpdateStudent();
    void onDeleteStudent();
    void onSearchStudent();
    void onSortByID();
    void onSortByName();
    void onStudentTableClicked(int row, int column);
    void onViewStudentProfile();

    // Subject Management (Part 3)
    void onAddSubject();
    void onUpdateSubject();
    void onDeleteSubject();
    void onSearchSubject();
    void onAssignSubjectToStudent();
    void onSubjectTableClicked(int row, int column);

    // Grade Management & GWA (Part 4)
    void onSaveOrUpdateGrade();
    void onDeleteGrade();
    void onGradeTableClicked(int row, int column);
    void onFilterGradesByStudent();

    // Report Card (Part 6)
    void onGenerateReportCard();
    void onExportReportCard();

private:
    // Core Data Structures
    StudentLinkedList studentList;
    std::vector<Subject> subjects;
    std::vector<Grade> grades;
    FileManager fileManager;

    QTabWidget *tabWidget;

    // Tab 1: Student Management Widgets
    QLineEdit *stuIdEdit, *stuFirstEdit, *stuLastEdit, *stuCourseEdit, *stuSectionEdit, *stuSearchEdit;
    QSpinBox *stuAgeSpin, *stuYearSpin;
    QComboBox *stuGenderCombo;
    QTableWidget *studentTable;

    // Tab 2: Subject Management Widgets
    QLineEdit *subCodeEdit, *subNameEdit, *subDescEdit, *subSearchEdit, *assignStuIdEdit, *assignSubCodeEdit;
    QSpinBox *subUnitsSpin;
    QTableWidget *subjectTable;

    // Tab 3: Grade Management & GWA Widgets
    QLineEdit *gradeStuIdEdit, *gradeSubCodeEdit, *gradeFilterStuIdEdit;
    QDoubleSpinBox *gradeValueSpin;
    QLabel *gwaDisplayLabel, *statusDisplayLabel;
    QTableWidget *gradeTable;

    // Tab 4: Report Card Widgets
    QLineEdit *reportStuIdEdit;
    QTextEdit *reportCardDisplay;

    // Setup & Helpers
    void setupUI();
    QWidget* createStudentTab();
    QWidget* createSubjectTab();
    QWidget* createGradeTab();
    QWidget* createReportTab();

    void refreshStudentTable(const std::vector<Student>& data);
    void refreshSubjectTable(const std::vector<Subject>& data);
    void refreshGradeTable(const std::string& filterStudentID = "");
    void saveAllData();

    Subject* findSubjectByCode(const std::string& code);
    double computeGWA(const std::string& studentID, int& totalUnits, int& gradedSubjects);
};

#endif // MAINWINDOW_H