#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Student Information, Grading & GWA Management System");
    resize(1050, 680);

    // Part 5.10: Automatically load saved data when the program starts
    fileManager.loadStudents(studentList);
    fileManager.loadSubjects(subjects);
    fileManager.loadGrades(grades);

    setupUI();

    refreshStudentTable(studentList.getAllStudents());
    refreshSubjectTable(subjects);
    refreshGradeTable();

    statusBar()->showMessage("Loaded records from students.txt, subjects.txt, and grades.txt.");
}

MainWindow::~MainWindow() {}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Part 5.11: Save updated records before exiting the program
    saveAllData();
    event->accept();
}

void MainWindow::saveAllData() {
    fileManager.saveStudents(studentList);
    fileManager.saveSubjects(subjects);
    fileManager.saveGrades(grades);
}

// ==========================================
// PART 7.10 - 7.16: Main Menu Integration
// ==========================================
void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Top Navigation / Quick Save & Exit Bar
    QHBoxLayout *topBar = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("<h2>Academic Records & GWA System</h2>");
    QPushButton *saveAllBtn = new QPushButton("Save All to Disk");
    QPushButton *exitBtn = new QPushButton("Save & Exit");
    exitBtn->setStyleSheet("background-color: #c0392b; color: white; font-weight: bold; padding: 6px 14px;");

    connect(saveAllBtn, &QPushButton::clicked, this, [this]() {
        saveAllData();
        QMessageBox::information(this, "Saved", "All student, subject, and grade records have been saved.");
    });
    connect(exitBtn, &QPushButton::clicked, this, &QWidget::close);

    topBar->addWidget(titleLabel);
    topBar->addStretch();
    topBar->addWidget(saveAllBtn);
    topBar->addWidget(exitBtn);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(createStudentTab(), "1. Student Registration & CRUD");
    tabWidget->addTab(createSubjectTab(), "2. Subject Management & Enrollment");
    tabWidget->addTab(createGradeTab(),   "3. Grade Management & GWA Calculator");
    tabWidget->addTab(createReportTab(),  "4. Report Card Generation");

    mainLayout->addLayout(topBar);
    mainLayout->addWidget(tabWidget);
    setCentralWidget(central);
}

// ==========================================
// TAB 1: Student Registration, CRUD & Sorting
// ==========================================
QWidget* MainWindow::createStudentTab() {
    QWidget *tab = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(tab);

    // Left Form: Registration & Update
    QGroupBox *formGroup = new QGroupBox("Student Profile Form");
    formGroup->setFixedWidth(330);
    QFormLayout *form = new QFormLayout(formGroup);

    stuIdEdit = new QLineEdit();
    stuFirstEdit = new QLineEdit();
    stuLastEdit = new QLineEdit();
    stuAgeSpin = new QSpinBox(); stuAgeSpin->setRange(15, 100); stuAgeSpin->setValue(18);
    stuGenderCombo = new QComboBox(); stuGenderCombo->addItems({"Male", "Female", "Non-Binary", "Prefer not to say"});
    stuCourseEdit = new QLineEdit();
    stuYearSpin = new QSpinBox(); stuYearSpin->setRange(1, 6);
    stuSectionEdit = new QLineEdit();

    form->addRow("Student ID:", stuIdEdit);
    form->addRow("First Name:", stuFirstEdit);
    form->addRow("Last Name:", stuLastEdit);
    form->addRow("Age:", stuAgeSpin);
    form->addRow("Gender:", stuGenderCombo);
    form->addRow("Course/Program:", stuCourseEdit);
    form->addRow("Year Level:", stuYearSpin);
    form->addRow("Section:", stuSectionEdit);

    QPushButton *addBtn = new QPushButton("Register Student");
    QPushButton *updBtn = new QPushButton("Update Student");
    QPushButton *delBtn = new QPushButton("Delete Student");
    QPushButton *profBtn = new QPushButton("View Individual Profile");

    addBtn->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold;");
    delBtn->setStyleSheet("background-color: #e74c3c; color: white;");

    form->addRow(addBtn);
    form->addRow(updBtn);
    form->addRow(delBtn);
    form->addRow(profBtn);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddStudent);
    connect(updBtn, &QPushButton::clicked, this, &MainWindow::onUpdateStudent);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::onDeleteStudent);
    connect(profBtn, &QPushButton::clicked, this, &MainWindow::onViewStudentProfile);

    // Right Panel: Search, Sort & Table
    QVBoxLayout *rightLayout = new QVBoxLayout();
    QHBoxLayout *toolsLayout = new QHBoxLayout();

    stuSearchEdit = new QLineEdit();
    stuSearchEdit->setPlaceholderText("Search by ID or Name...");
    QPushButton *searchBtn = new QPushButton("Search");
    QPushButton *resetBtn = new QPushButton("Show All");
    QPushButton *sortIdBtn = new QPushButton("Sort by ID");
    QPushButton *sortNameBtn = new QPushButton("Sort by Name");

    toolsLayout->addWidget(stuSearchEdit);
    toolsLayout->addWidget(searchBtn);
    toolsLayout->addWidget(resetBtn);
    toolsLayout->addWidget(sortIdBtn);
    toolsLayout->addWidget(sortNameBtn);

    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchStudent);
    connect(resetBtn, &QPushButton::clicked, this, [this]() {
        stuSearchEdit->clear();
        refreshStudentTable(studentList.getAllStudents());
    });
    connect(sortIdBtn, &QPushButton::clicked, this, &MainWindow::onSortByID);
    connect(sortNameBtn, &QPushButton::clicked, this, &MainWindow::onSortByName);

    studentTable = new QTableWidget(0, 7);
    studentTable->setHorizontalHeaderLabels({"Student ID", "First Name", "Last Name", "Age", "Gender", "Course", "Year & Sec"});
    studentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    studentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    studentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(studentTable, &QTableWidget::cellClicked, this, &MainWindow::onStudentTableClicked);

    rightLayout->addLayout(toolsLayout);
    rightLayout->addWidget(studentTable);

    layout->addWidget(formGroup);
    layout->addLayout(rightLayout);
    return tab;
}

void MainWindow::onAddStudent() {
    std::string id = stuIdEdit->text().trimmed().toStdString();
    std::string fName = stuFirstEdit->text().trimmed().toStdString();
    std::string lName = stuLastEdit->text().trimmed().toStdString();
    std::string course = stuCourseEdit->text().trimmed().toStdString();
    std::string sec = stuSectionEdit->text().trimmed().toStdString();

    if (id.empty() || fName.empty() || lName.empty() || course.empty()) {
        QMessageBox::warning(this, "Validation Error", "Please fill in Student ID, First Name, Last Name, and Course.");
        return;
    }

    Student s(id, fName, lName, stuAgeSpin->value(),
              stuGenderCombo->currentText().toStdString(), course,
              stuYearSpin->value(), sec);

    // Part 1.10: Prevent duplicate Student IDs
    if (!studentList.insertStudent(s)) {
        QMessageBox::critical(this, "Duplicate ID", "A student with ID '" + QString::fromStdString(id) + "' already exists!");
        return;
    }

    fileManager.saveStudents(studentList);
    refreshStudentTable(studentList.getAllStudents());
    QMessageBox::information(this, "Success", "Student registered successfully!");
}

void MainWindow::onUpdateStudent() {
    std::string id = stuIdEdit->text().trimmed().toStdString();
    Student* s = studentList.searchByID(id);

    // Part 2.7: Display error if Student ID does not exist
    if (!s) {
        QMessageBox::critical(this, "Not Found", "Student ID '" + QString::fromStdString(id) + "' does not exist.");
        return;
    }

    // Part 2.4: Edit name, age, gender, course, year level, and section
    s->firstName = stuFirstEdit->text().trimmed().toStdString();
    s->lastName  = stuLastEdit->text().trimmed().toStdString();
    s->age       = stuAgeSpin->value();
    s->gender    = stuGenderCombo->currentText().toStdString();
    s->course    = stuCourseEdit->text().trimmed().toStdString();
    s->yearLevel = stuYearSpin->value();
    s->section   = stuSectionEdit->text().trimmed().toStdString();

    fileManager.saveStudents(studentList);
    refreshStudentTable(studentList.getAllStudents());

    // Part 2.8: Display updated student information
    QString updatedInfo = QString("Updated Student Record:\n\nID: %1\nName: %2 %3\nAge: %4 | Gender: %5\nCourse: %6 (%7-%8)")
            .arg(QString::fromStdString(s->studentID), QString::fromStdString(s->firstName),
                 QString::fromStdString(s->lastName)).arg(s->age)
            .arg(QString::fromStdString(s->gender), QString::fromStdString(s->course))
            .arg(s->yearLevel).arg(QString::fromStdString(s->section));
    QMessageBox::information(this, "Record Updated", updatedInfo);
}

void MainWindow::onDeleteStudent() {
    std::string id = stuIdEdit->text().trimmed().toStdString();
    if (!studentList.searchByID(id)) {
        QMessageBox::critical(this, "Not Found", "Student ID does not exist.");
        return;
    }

    // Part 2.6: Ask for confirmation before deleting
    auto reply = QMessageBox::question(this, "Confirm Deletion",
                                       "Are you sure you want to delete Student ID '" + QString::fromStdString(id) + "' and all associated grades?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        studentList.deleteByID(id);
        // Remove associated grades
        grades.erase(std::remove_if(grades.begin(), grades.end(),
                     [&](const Grade& g) { return g.studentID == id; }), grades.end());

        saveAllData();
        refreshStudentTable(studentList.getAllStudents());
        refreshGradeTable();
        QMessageBox::information(this, "Deleted", "Student record deleted.");
    }
}

void MainWindow::onSearchStudent() {
    std::string q = stuSearchEdit->text().trimmed().toStdString();
    if (q.empty()) {
        refreshStudentTable(studentList.getAllStudents());
        return;
    }

    // Part 2.1 & 2.2: Search by ID or Name
    Student* exact = studentList.searchByID(q);
    if (exact) {
        refreshStudentTable({*exact});
        return;
    }

    auto matches = studentList.searchByName(q);
    if (matches.empty()) {
        QMessageBox::information(this, "No Results", "No matching student records found.");
    }
    refreshStudentTable(matches);
}

void MainWindow::onSortByID() {
    studentList.sortByID();
    refreshStudentTable(studentList.getAllStudents());
    statusBar()->showMessage("Students sorted by Student ID.");
}

void MainWindow::onSortByName() {
    studentList.sortByName();
    refreshStudentTable(studentList.getAllStudents());
    statusBar()->showMessage("Students sorted alphabetically by Last Name.");
}

void MainWindow::onStudentTableClicked(int row, int) {
    std::string id = studentTable->item(row, 0)->text().toStdString();
    Student* s = studentList.searchByID(id);
    if (s) {
        stuIdEdit->setText(QString::fromStdString(s->studentID));
        stuFirstEdit->setText(QString::fromStdString(s->firstName));
        stuLastEdit->setText(QString::fromStdString(s->lastName));
        stuAgeSpin->setValue(s->age);
        stuGenderCombo->setCurrentText(QString::fromStdString(s->gender));
        stuCourseEdit->setText(QString::fromStdString(s->course));
        stuYearSpin->setValue(s->yearLevel);
        stuSectionEdit->setText(QString::fromStdString(s->section));
    }
}

// Part 1.8: Display individual student profile
void MainWindow::onViewStudentProfile() {
    std::string id = stuIdEdit->text().trimmed().toStdString();
    Student* s = studentList.searchByID(id);
    if (!s) {
        QMessageBox::warning(this, "Not Found", "Select or enter a valid Student ID first.");
        return;
    }

    QString profile = QString(
        "====================================\n"
        "         STUDENT PROFILE            \n"
        "====================================\n"
        "Student ID : %1\n"
        "Full Name  : %2 %3\n"
        "Age        : %4\n"
        "Gender     : %5\n"
        "Course     : %6\n"
        "Year & Sec : Year %7 - %8\n"
        "===================================="
    ).arg(QString::fromStdString(s->studentID), QString::fromStdString(s->firstName),
          QString::fromStdString(s->lastName)).arg(s->age)
     .arg(QString::fromStdString(s->gender), QString::fromStdString(s->course))
     .arg(s->yearLevel).arg(QString::fromStdString(s->section));

    QMessageBox::information(this, "Student Profile", profile);
}

void MainWindow::refreshStudentTable(const std::vector<Student>& data) {
    studentTable->setRowCount(0);
    for (const auto& s : data) {
        int r = studentTable->rowCount();
        studentTable->insertRow(r);
        studentTable->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(s.studentID)));
        studentTable->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(s.firstName)));
        studentTable->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(s.lastName)));
        studentTable->setItem(r, 3, new QTableWidgetItem(QString::number(s.age)));
        studentTable->setItem(r, 4, new QTableWidgetItem(QString::fromStdString(s.gender)));
        studentTable->setItem(r, 5, new QTableWidgetItem(QString::fromStdString(s.course)));
        studentTable->setItem(r, 6, new QTableWidgetItem(QString("%1-%2").arg(s.yearLevel).arg(QString::fromStdString(s.section))));
    }
}

// ==========================================
// TAB 2: Subject Management & Assignment
// ==========================================
QWidget* MainWindow::createSubjectTab() {
    QWidget *tab = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(tab);

    QVBoxLayout *leftCol = new QVBoxLayout();

    // Subject CRUD Form
    QGroupBox *subGroup = new QGroupBox("Subject Details");
    subGroup->setFixedWidth(330);
    QFormLayout *subForm = new QFormLayout(subGroup);

    subCodeEdit = new QLineEdit();
    subNameEdit = new QLineEdit();
    subDescEdit = new QLineEdit();
    subUnitsSpin = new QSpinBox(); subUnitsSpin->setRange(1, 10); subUnitsSpin->setValue(3);

    subForm->addRow("Subject Code:", subCodeEdit);
    subForm->addRow("Subject Name:", subNameEdit);
    subForm->addRow("Description:", subDescEdit);
    subForm->addRow("Units:", subUnitsSpin);

    QPushButton *addSubBtn = new QPushButton("Add Subject");
    QPushButton *updSubBtn = new QPushButton("Update Subject");
    QPushButton *delSubBtn = new QPushButton("Delete Subject");
    addSubBtn->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold;");

    subForm->addRow(addSubBtn);
    subForm->addRow(updSubBtn);
    subForm->addRow(delSubBtn);

    connect(addSubBtn, &QPushButton::clicked, this, &MainWindow::onAddSubject);
    connect(updSubBtn, &QPushButton::clicked, this, &MainWindow::onUpdateSubject);
    connect(delSubBtn, &QPushButton::clicked, this, &MainWindow::onDeleteSubject);

    // Part 3.11: Assign Subject to Student
    QGroupBox *assignGroup = new QGroupBox("Enroll / Assign Subject to Student");
    assignGroup->setFixedWidth(330);
    QFormLayout *assignForm = new QFormLayout(assignGroup);

    assignStuIdEdit = new QLineEdit();
    assignSubCodeEdit = new QLineEdit();
    QPushButton *assignBtn = new QPushButton("Assign Subject to Student");
    assignBtn->setStyleSheet("background-color: #2980b9; color: white; font-weight: bold;");

    assignForm->addRow("Student ID:", assignStuIdEdit);
    assignForm->addRow("Subject Code:", assignSubCodeEdit);
    assignForm->addRow(assignBtn);

    connect(assignBtn, &QPushButton::clicked, this, &MainWindow::onAssignSubjectToStudent);

    leftCol->addWidget(subGroup);
    leftCol->addWidget(assignGroup);
    leftCol->addStretch();

    // Right Table & Search
    QVBoxLayout *rightCol = new QVBoxLayout();
    QHBoxLayout *searchRow = new QHBoxLayout();
    subSearchEdit = new QLineEdit();
    subSearchEdit->setPlaceholderText("Search by Subject Code...");
    QPushButton *searchSubBtn = new QPushButton("Search Subject");
    QPushButton *showAllSubBtn = new QPushButton("Show All");

    searchRow->addWidget(subSearchEdit);
    searchRow->addWidget(searchSubBtn);
    searchRow->addWidget(showAllSubBtn);

    connect(searchSubBtn, &QPushButton::clicked, this, &MainWindow::onSearchSubject);
    connect(showAllSubBtn, &QPushButton::clicked, this, [this]() {
        subSearchEdit->clear();
        refreshSubjectTable(subjects);
    });

    subjectTable = new QTableWidget(0, 4);
    subjectTable->setHorizontalHeaderLabels({"Subject Code", "Subject Name", "Description", "Units"});
    subjectTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    subjectTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    subjectTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(subjectTable, &QTableWidget::cellClicked, this, &MainWindow::onSubjectTableClicked);

    rightCol->addLayout(searchRow);
    rightCol->addWidget(subjectTable);

    layout->addLayout(leftCol);
    layout->addLayout(rightCol);
    return tab;
}

Subject* MainWindow::findSubjectByCode(const std::string& code) {
    for (auto& sub : subjects) {
        if (sub.subjectCode == code) return &sub;
    }
    return nullptr;
}

void MainWindow::onAddSubject() {
    std::string code = subCodeEdit->text().trimmed().toStdString();
    std::string name = subNameEdit->text().trimmed().toStdString();
    std::string desc = subDescEdit->text().trimmed().toStdString();
    int units = subUnitsSpin->value();

    if (code.empty() || name.empty()) {
        QMessageBox::warning(this, "Validation", "Subject Code and Name are required.");
        return;
    }
    if (findSubjectByCode(code) != nullptr) {
        QMessageBox::critical(this, "Duplicate", "Subject Code already exists!");
        return;
    }

    subjects.emplace_back(code, name, desc, units);
    fileManager.saveSubjects(subjects);
    refreshSubjectTable(subjects);
    QMessageBox::information(this, "Success", "Subject added.");
}

void MainWindow::onUpdateSubject() {
    std::string code = subCodeEdit->text().trimmed().toStdString();
    Subject* sub = findSubjectByCode(code);
    if (!sub) {
        QMessageBox::critical(this, "Not Found", "Subject Code does not exist.");
        return;
    }
    sub->subjectName = subNameEdit->text().trimmed().toStdString();
    sub->description = subDescEdit->text().trimmed().toStdString();
    sub->units = subUnitsSpin->value();

    // Update units in existing grade records if changed
    for (auto& g : grades) {
        if (g.subjectCode == code) g.units = sub->units;
    }

    fileManager.saveSubjects(subjects);
    fileManager.saveGrades(grades);
    refreshSubjectTable(subjects);
    refreshGradeTable();
    QMessageBox::information(this, "Updated", "Subject updated.");
}

void MainWindow::onDeleteSubject() {
    std::string code = subCodeEdit->text().trimmed().toStdString();
    auto it = std::find_if(subjects.begin(), subjects.end(), [&](const Subject& s) { return s.subjectCode == code; });
    if (it == subjects.end()) {
        QMessageBox::critical(this, "Not Found", "Subject Code does not exist.");
        return;
    }
    subjects.erase(it);
    fileManager.saveSubjects(subjects);
    refreshSubjectTable(subjects);
    QMessageBox::information(this, "Deleted", "Subject deleted.");
}

void MainWindow::onSearchSubject() {
    std::string code = subSearchEdit->text().trimmed().toStdString();
    Subject* sub = findSubjectByCode(code);
    if (!sub) {
        QMessageBox::information(this, "Not Found", "No subject found with that code.");
        return;
    }
    refreshSubjectTable({*sub});
}

void MainWindow::onAssignSubjectToStudent() {
    std::string stuID = assignStuIdEdit->text().trimmed().toStdString();
    std::string subCode = assignSubCodeEdit->text().trimmed().toStdString();

    if (!studentList.searchByID(stuID)) {
        QMessageBox::critical(this, "Error", "Student ID does not exist.");
        return;
    }
    Subject* sub = findSubjectByCode(subCode);
    if (!sub) {
        QMessageBox::critical(this, "Error", "Subject Code does not exist.");
        return;
    }

    for (const auto& g : grades) {
        if (g.studentID == stuID && g.subjectCode == subCode) {
            QMessageBox::warning(this, "Already Enrolled", "Student is already enrolled in this subject.");
            return;
        }
    }

    // Enroll student with initial ungraded value (0.00)
    grades.emplace_back(stuID, subCode, 0.0, sub->units);
    fileManager.saveGrades(grades);
    refreshGradeTable();
    QMessageBox::information(this, "Enrolled", "Subject assigned to student! You can now encode their grade in Tab 3.");
}

void MainWindow::onSubjectTableClicked(int row, int) {
    subCodeEdit->setText(subjectTable->item(row, 0)->text());
    subNameEdit->setText(subjectTable->item(row, 1)->text());
    subDescEdit->setText(subjectTable->item(row, 2)->text());
    subUnitsSpin->setValue(subjectTable->item(row, 3)->text().toInt());
    assignSubCodeEdit->setText(subjectTable->item(row, 0)->text());
}

void MainWindow::refreshSubjectTable(const std::vector<Subject>& data) {
    subjectTable->setRowCount(0);
    for (const auto& s : data) {
        int r = subjectTable->rowCount();
        subjectTable->insertRow(r);
        subjectTable->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(s.subjectCode)));
        subjectTable->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(s.subjectName)));
        subjectTable->setItem(r, 2, new QTableWidgetItem(QString::fromStdString(s.description)));
        subjectTable->setItem(r, 3, new QTableWidgetItem(QString::number(s.units)));
    }
}

// ==========================================
// TAB 3: Grade Management & GWA Calculator
// ==========================================
QWidget* MainWindow::createGradeTab() {
    QWidget *tab = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(tab);

    QGroupBox *gradeFormGroup = new QGroupBox("Record / Modify Student Grade");
    gradeFormGroup->setFixedWidth(330);
    QFormLayout *form = new QFormLayout(gradeFormGroup);

    gradeStuIdEdit = new QLineEdit();
    gradeSubCodeEdit = new QLineEdit();
    gradeValueSpin = new QDoubleSpinBox();
    gradeValueSpin->setRange(1.00, 5.00);
    gradeValueSpin->setSingleStep(0.25);
    gradeValueSpin->setValue(1.50);

    QLabel *policyNote = new QLabel("<i>Grading Scale: 1.00 (Highest) to 3.00 (Passing).<br>Above 3.00 (up to 5.00) = Failed.</i>");
    policyNote->setWordWrap(true);

    form->addRow("Student ID:", gradeStuIdEdit);
    form->addRow("Subject Code:", gradeSubCodeEdit);
    form->addRow("Final Grade:", gradeValueSpin);
    form->addRow(policyNote);

    QPushButton *saveGradeBtn = new QPushButton("Save / Update Grade");
    QPushButton *delGradeBtn = new QPushButton("Delete Grade Record");
    saveGradeBtn->setStyleSheet("background-color: #27ae60; color: white; font-weight: bold;");

    form->addRow(saveGradeBtn);
    form->addRow(delGradeBtn);

    connect(saveGradeBtn, &QPushButton::clicked, this, &MainWindow::onSaveOrUpdateGrade);
    connect(delGradeBtn, &QPushButton::clicked, this, &MainWindow::onDeleteGrade);

    // GWA Summary Box
    QGroupBox *gwaBox = new QGroupBox("Computed GWA & Academic Status");
    QVBoxLayout *gwaLayout = new QVBoxLayout(gwaBox);
    gwaDisplayLabel = new QLabel("GWA: N/A");
    gwaDisplayLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    statusDisplayLabel = new QLabel("Status: Filter by Student ID to compute");
    statusDisplayLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    gwaLayout->addWidget(gwaDisplayLabel);
    gwaLayout->addWidget(statusDisplayLabel);

    QVBoxLayout *leftPanel = new QVBoxLayout();
    leftPanel->addWidget(gradeFormGroup);
    leftPanel->addWidget(gwaBox);
    leftPanel->addStretch();

    // Right Panel: Filter & Grades Table
    QVBoxLayout *rightPanel = new QVBoxLayout();
    QHBoxLayout *filterBar = new QHBoxLayout();
    gradeFilterStuIdEdit = new QLineEdit();
    gradeFilterStuIdEdit->setPlaceholderText("Enter Student ID to view grades & compute GWA...");
    QPushButton *filterBtn = new QPushButton("Compute Student GWA");
    QPushButton *allGradesBtn = new QPushButton("Show All Grades");

    filterBar->addWidget(gradeFilterStuIdEdit);
    filterBar->addWidget(filterBtn);
    filterBar->addWidget(allGradesBtn);

    connect(filterBtn, &QPushButton::clicked, this, &MainWindow::onFilterGradesByStudent);
    connect(allGradesBtn, &QPushButton::clicked, this, [this]() {
        gradeFilterStuIdEdit->clear();
        refreshGradeTable();
        gwaDisplayLabel->setText("GWA: N/A");
        statusDisplayLabel->setText("Status: Filter by Student ID to compute");
    });

    gradeTable = new QTableWidget(0, 5);
    gradeTable->setHorizontalHeaderLabels({"Student ID", "Subject Code", "Units", "Grade", "Remarks"});
    gradeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    gradeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    gradeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(gradeTable, &QTableWidget::cellClicked, this, &MainWindow::onGradeTableClicked);

    rightPanel->addLayout(filterBar);
    rightPanel->addWidget(gradeTable);

    layout->addLayout(leftPanel);
    layout->addLayout(rightPanel);
    return tab;
}

void MainWindow::onSaveOrUpdateGrade() {
    std::string stuID = gradeStuIdEdit->text().trimmed().toStdString();
    std::string subCode = gradeSubCodeEdit->text().trimmed().toStdString();
    double val = gradeValueSpin->value();

    if (!studentList.searchByID(stuID)) {
        QMessageBox::critical(this, "Error", "Student ID does not exist.");
        return;
    }
    Subject* sub = findSubjectByCode(subCode);
    if (!sub) {
        QMessageBox::critical(this, "Error", "Subject Code does not exist.");
        return;
    }

    bool updated = false;
    for (auto& g : grades) {
        if (g.studentID == stuID && g.subjectCode == subCode) {
            g.gradeValue = val;
            g.units = sub->units;
            updated = true;
            break;
        }
    }

    if (!updated) {
        grades.emplace_back(stuID, subCode, val, sub->units);
    }

    fileManager.saveGrades(grades);
    gradeFilterStuIdEdit->setText(QString::fromStdString(stuID));
    onFilterGradesByStudent();
    QMessageBox::information(this, "Saved", "Grade recorded and GWA recalculated.");
}

void MainWindow::onDeleteGrade() {
    std::string stuID = gradeStuIdEdit->text().trimmed().toStdString();
    std::string subCode = gradeSubCodeEdit->text().trimmed().toStdString();

    auto it = std::find_if(grades.begin(), grades.end(), [&](const Grade& g) {
        return g.studentID == stuID && g.subjectCode == subCode;
    });

    if (it == grades.end()) {
        QMessageBox::warning(this, "Not Found", "Grade record not found.");
        return;
    }

    grades.erase(it);
    fileManager.saveGrades(grades);
    refreshGradeTable(gradeFilterStuIdEdit->text().trimmed().toStdString());
    QMessageBox::information(this, "Deleted", "Grade record removed.");
}

void MainWindow::onGradeTableClicked(int row, int) {
    gradeStuIdEdit->setText(gradeTable->item(row, 0)->text());
    gradeSubCodeEdit->setText(gradeTable->item(row, 1)->text());
    double g = gradeTable->item(row, 3)->text().toDouble();
    if (g >= 1.0) gradeValueSpin->setValue(g);
}

// Part 4.8: GWA Formula = Sum(Grade * Units) / Sum(Units)
double MainWindow::computeGWA(const std::string& studentID, int& totalUnits, int& gradedSubjects) {
    double totalWeightedPoints = 0.0;
    totalUnits = 0;
    gradedSubjects = 0;

    for (const auto& g : grades) {
        if (g.studentID == studentID && g.gradeValue > 0.0) {
            totalWeightedPoints += (g.gradeValue * g.units);
            totalUnits += g.units;
            gradedSubjects++;
        }
    }

    if (totalUnits == 0) return 0.0;
    return totalWeightedPoints / totalUnits;
}

void MainWindow::onFilterGradesByStudent() {
    std::string stuID = gradeFilterStuIdEdit->text().trimmed().toStdString();
    refreshGradeTable(stuID);

    int totalUnits = 0, gradedCount = 0;
    double gwa = computeGWA(stuID, totalUnits, gradedCount);

    if (gradedCount == 0) {
        gwaDisplayLabel->setText("GWA: N/A (No graded subjects)");
        statusDisplayLabel->setText("Status: Pending");
        statusDisplayLabel->setStyleSheet("color: #7f8c8d; font-weight: bold;");
    } else {
        gwaDisplayLabel->setText(QString("GWA: %1 (Total Units: %2)").arg(gwa, 0, 'f', 2).arg(totalUnits));
        // Part 4.10: Determine academic status based on grading policy (<= 3.00 Passed)
        if (gwa <= 3.00) {
            statusDisplayLabel->setText("Academic Status: PASSED");
            statusDisplayLabel->setStyleSheet("color: #27ae60; font-size: 15px; font-weight: bold;");
        } else {
            statusDisplayLabel->setText("Academic Status: FAILED");
            statusDisplayLabel->setStyleSheet("color: #c0392b; font-size: 15px; font-weight: bold;");
        }
    }
}

void MainWindow::refreshGradeTable(const std::string& filterStudentID) {
    gradeTable->setRowCount(0);
    for (const auto& g : grades) {
        if (!filterStudentID.empty() && g.studentID != filterStudentID) continue;

        int r = gradeTable->rowCount();
        gradeTable->insertRow(r);
        gradeTable->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(g.studentID)));
        gradeTable->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(g.subjectCode)));
        gradeTable->setItem(r, 2, new QTableWidgetItem(QString::number(g.units)));

        QString gradeStr = (g.gradeValue <= 0.0) ? "Pending" : QString::number(g.gradeValue, 'f', 2);
        QString remarkStr = (g.gradeValue <= 0.0) ? "Enrolled" : (g.gradeValue <= 3.00 ? "Passed" : "Failed");

        gradeTable->setItem(r, 3, new QTableWidgetItem(gradeStr));
        gradeTable->setItem(r, 4, new QTableWidgetItem(remarkStr));
    }
}

// ==========================================
// TAB 4: Report Card Generation (Part 6)
// ==========================================
QWidget* MainWindow::createReportTab() {
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QHBoxLayout *topRow = new QHBoxLayout();
    reportStuIdEdit = new QLineEdit();
    reportStuIdEdit->setPlaceholderText("Enter Student ID to generate Report Card...");
    QPushButton *genBtn = new QPushButton("Generate Report Card");
    QPushButton *exportBtn = new QPushButton("Export Report Card to .txt");

    genBtn->setStyleSheet("background-color: #2980b9; color: white; font-weight: bold;");
    exportBtn->setStyleSheet("background-color: #16a085; color: white; font-weight: bold;");

    topRow->addWidget(reportStuIdEdit);
    topRow->addWidget(genBtn);
    topRow->addWidget(exportBtn);

    connect(genBtn, &QPushButton::clicked, this, &MainWindow::onGenerateReportCard);
    connect(exportBtn, &QPushButton::clicked, this, &MainWindow::onExportReportCard);

    reportCardDisplay = new QTextEdit();
    reportCardDisplay->setReadOnly(true);
    reportCardDisplay->setFontFamily("Courier New");
    reportCardDisplay->setFontPointSize(11);

    layout->addLayout(topRow);
    layout->addWidget(reportCardDisplay);
    return tab;
}

void MainWindow::onGenerateReportCard() {
    std::string stuID = reportStuIdEdit->text().trimmed().toStdString();
    Student* s = studentList.searchByID(stuID);
    if (!s) {
        QMessageBox::critical(this, "Error", "Student ID not found.");
        return;
    }

    std::ostringstream out;
    out << "====================================================================\n";
    out << "                     OFFICIAL STUDENT REPORT CARD                   \n";
    out << "====================================================================\n";
    out << "Student ID   : " << s->studentID << "\n";
    out << "Student Name : " << s->lastName << ", " << s->firstName << "\n";
    out << "Age / Gender : " << s->age << " / " << s->gender << "\n";
    out << "Program      : " << s->course << " (Year " << s->yearLevel << " - Sec " << s->section << ")\n";
    out << "--------------------------------------------------------------------\n";
    out << std::left << std::setw(12) << "CODE"
        << std::setw(28) << "SUBJECT TITLE"
        << std::setw(8)  << "UNITS"
        << std::setw(10) << "GRADE"
        << "STATUS\n";
    out << "--------------------------------------------------------------------\n";

    bool hasSubjects = false;
    for (const auto& g : grades) {
        if (g.studentID == stuID) {
            hasSubjects = true;
            Subject* sub = findSubjectByCode(g.subjectCode);
            std::string subTitle = sub ? sub->subjectName : "Unknown Subject";
            if (subTitle.length() > 26) subTitle = subTitle.substr(0, 23) + "...";

            std::string gradeText = (g.gradeValue <= 0.0) ? "N/A" : [&]() {
                std::ostringstream gs;
                gs << std::fixed << std::setprecision(2) << g.gradeValue;
                return gs.str();
            }();

            std::string statusText = (g.gradeValue <= 0.0) ? "ENROLLED" : (g.gradeValue <= 3.00 ? "PASSED" : "FAILED");

            out << std::left << std::setw(12) << g.subjectCode
                << std::setw(28) << subTitle
                << std::setw(8)  << g.units
                << std::setw(10) << gradeText
                << statusText << "\n";
        }
    }

    if (!hasSubjects) {
        out << "No enrolled subjects recorded for this student.\n";
    }

    int totalUnits = 0, gradedCount = 0;
    double gwa = computeGWA(stuID, totalUnits, gradedCount);

    out << "--------------------------------------------------------------------\n";
    if (gradedCount > 0) {
        out << "Total Credited Units : " << totalUnits << "\n";
        out << "General Weighted Avg : " << std::fixed << std::setprecision(2) << gwa << "\n";
        out << "Academic Standing    : " << (gwa <= 3.00 ? "PASSED (Good Standing)" : "FAILED") << "\n";
    } else {
        out << "General Weighted Avg : Pending (No graded subjects)\n";
    }
    out << "====================================================================\n";

    reportCardDisplay->setPlainText(QString::fromStdString(out.str()));
}

void MainWindow::onExportReportCard() {
    if (reportCardDisplay->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Empty", "Generate a report card first before exporting.");
        return;
    }

    QString defaultName = "ReportCard_" + reportStuIdEdit->text().trimmed() + ".txt";
    QString fileName = QFileDialog::getSaveFileName(this, "Export Report Card", defaultName, "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    if (fileManager.exportReportCard(fileName.toStdString(), reportCardDisplay->toPlainText().toStdString())) {
        QMessageBox::information(this, "Exported", "Report card saved successfully to:\n" + fileName);
    } else {
        QMessageBox::critical(this, "Error", "Failed to write report card file.");
    }
}