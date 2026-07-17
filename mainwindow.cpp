#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "studentdialog.h"
#include "scoremanagementdialog.h"
#include "attendancemanagementdialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDialog>
#include <QAbstractItemView>
#include <QAction>

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->studentTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->studentTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->studentTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->studentTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);


    connect(ui->addButton, &QPushButton::clicked,
            this, &MainWindow::onAddButtonClicked);

    connect(ui->editButton, &QPushButton::clicked,
            this, &MainWindow::onEditButtonClicked);

    connect(ui->deleteButton, &QPushButton::clicked,
            this, &MainWindow::onDeleteButtonClicked);

    connect(ui->searchButton, &QPushButton::clicked,
            this, &MainWindow::onSearchButtonClicked);

    // 点击“成绩管理”按钮时，执行 onScoreManagementClicked()
    connect(ui->scoreManagementButton, &QPushButton::clicked,
            this, &MainWindow::onScoreManagementClicked);

    // 点击菜单中的“打开成绩管理”时，也执行同一个函数
    connect(ui->actionOpenScoreManagement, &QAction::triggered,
            this, &MainWindow::onScoreManagementClicked);


    if (initDatabase()) {
        loadStudentsFromDatabase();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddButtonClicked()
{
    StudentDialog dialog(this);
    dialog.setWindowTitle("新增学生");

    if (dialog.exec() == QDialog::Accepted) {
        QString studentNo = dialog.studentNo();
        QString name = dialog.studentName();
        QString gender = dialog.gender();
        QString college = dialog.college();
        QString major = dialog.major();

        if (studentNo.isEmpty()) {
            QMessageBox::warning(this, "提示", "学号不能为空！");
            return;
        }

        if (name.isEmpty()) {
            QMessageBox::warning(this, "提示", "姓名不能为空！");
            return;
        }

        if (gender == "请选择") {
            QMessageBox::warning(this, "提示", "请选择性别！");
            return;
        }

        if (college == "请选择") {
            QMessageBox::warning(this, "提示", "请选择学院！");
            return;
        }

        if (major.isEmpty()) {
            QMessageBox::warning(this, "提示", "专业不能为空！");
            return;
        }

        for (int i = 0; i < ui->studentTableWidget->rowCount(); ++i) {
            QTableWidgetItem *item = ui->studentTableWidget->item(i, 0);
            if (item && item->text() == studentNo) {
                QMessageBox::warning(this, "提示", "该学号已存在！");
                return;
            }
        }

        int row = -1;

        for (int i = 0; i < ui->studentTableWidget->rowCount(); ++i) {
            QTableWidgetItem *item = ui->studentTableWidget->item(i, 0);
            if (item == nullptr || item->text().trimmed().isEmpty()) {
                row = i;
                break;
            }
        }

        if (row == -1) {
            row = ui->studentTableWidget->rowCount();
            ui->studentTableWidget->insertRow(row);
        }

        QSqlQuery query;
        query.prepare("INSERT INTO students(student_no, name, gender, college, major) "
                      "VALUES(?, ?, ?, ?, ?)");

        query.addBindValue(studentNo);
        query.addBindValue(name);
        query.addBindValue(gender);
        query.addBindValue(college);
        query.addBindValue(major);

        if (!query.exec()) {
            QMessageBox::warning(this,
                                 "数据库错误",
                                 "新增学生失败：\n" + query.lastError().text());
            return;
        }

        addStudentToTable(studentNo, name, gender, college, major);
    }
}

void MainWindow::onEditButtonClicked()
{
    int row = ui->studentTableWidget->currentRow();

    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要编辑的学生！");
        return;
    }

    QTableWidgetItem *studentNoItem = ui->studentTableWidget->item(row, 0);
    QTableWidgetItem *nameItem = ui->studentTableWidget->item(row, 1);
    QTableWidgetItem *genderItem = ui->studentTableWidget->item(row, 2);
    QTableWidgetItem *collegeItem = ui->studentTableWidget->item(row, 3);
    QTableWidgetItem *majorItem = ui->studentTableWidget->item(row, 4);

    if (!studentNoItem || studentNoItem->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择一条有效的学生记录！");
        return;
    }

    QString oldStudentNo = studentNoItem->text().trimmed();
    QString oldName = nameItem ? nameItem->text().trimmed() : "";
    QString oldGender = genderItem ? genderItem->text().trimmed() : "";
    QString oldCollege = collegeItem ? collegeItem->text().trimmed() : "";
    QString oldMajor = majorItem ? majorItem->text().trimmed() : "";

    StudentDialog dialog(this);
    dialog.setWindowTitle("编辑学生");

    dialog.setStudentInfo(oldStudentNo, oldName, oldGender, oldCollege, oldMajor);

    if (dialog.exec() == QDialog::Accepted) {
        QString studentNo = dialog.studentNo();
        QString name = dialog.studentName();
        QString gender = dialog.gender();
        QString college = dialog.college();
        QString major = dialog.major();

        if (studentNo.isEmpty()) {
            QMessageBox::warning(this, "提示", "学号不能为空！");
            return;
        }

        if (name.isEmpty()) {
            QMessageBox::warning(this, "提示", "姓名不能为空！");
            return;
        }

        if (gender == "请选择") {
            QMessageBox::warning(this, "提示", "请选择性别！");
            return;
        }

        if (college == "请选择") {
            QMessageBox::warning(this, "提示", "请选择学院！");
            return;
        }

        if (major.isEmpty()) {
            QMessageBox::warning(this, "提示", "专业不能为空！");
            return;
        }

        for (int i = 0; i < ui->studentTableWidget->rowCount(); ++i) {
            if (i == row) {
                continue;
            }

            QTableWidgetItem *item = ui->studentTableWidget->item(i, 0);
            if (item && item->text().trimmed() == studentNo) {
                QMessageBox::warning(this, "提示", "该学号已存在！");
                return;
            }
        }

        QSqlQuery query;
        query.prepare("UPDATE students "
                      "SET student_no = ?, name = ?, gender = ?, college = ?, major = ? "
                      "WHERE student_no = ?");

        query.addBindValue(studentNo);
        query.addBindValue(name);
        query.addBindValue(gender);
        query.addBindValue(college);
        query.addBindValue(major);
        query.addBindValue(oldStudentNo);

        if (!query.exec()) {
            QMessageBox::warning(this,
                                 "数据库错误",
                                 "修改学生失败：\n" + query.lastError().text());
            return;
        }

        ui->studentTableWidget->setItem(row, 0, new QTableWidgetItem(studentNo));
        ui->studentTableWidget->setItem(row, 1, new QTableWidgetItem(name));
        ui->studentTableWidget->setItem(row, 2, new QTableWidgetItem(gender));
        ui->studentTableWidget->setItem(row, 3, new QTableWidgetItem(college));
        ui->studentTableWidget->setItem(row, 4, new QTableWidgetItem(major));

        QMessageBox::information(this, "提示", "修改成功！");
    }
}

void MainWindow::onDeleteButtonClicked()
{
    int row = ui->studentTableWidget->currentRow();

    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的学生！");
        return;
    }

    QTableWidgetItem *studentNoItem = ui->studentTableWidget->item(row, 0);

    if (!studentNoItem || studentNoItem->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择一条有效的学生记录！");
        return;
    }

    QString studentNo = studentNoItem->text().trimmed();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                  "确认删除",
                                  "确定要删除学号为 " + studentNo + " 的学生吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM students WHERE student_no = ?");
        query.addBindValue(studentNo);

        if (!query.exec()) {
            QMessageBox::warning(this,
                                 "数据库错误",
                                 "删除学生失败：\n" + query.lastError().text());
            return;
        }

        ui->studentTableWidget->removeRow(row);

        if (ui->studentTableWidget->rowCount() < 10) {
            ui->studentTableWidget->insertRow(ui->studentTableWidget->rowCount());
        }

        QMessageBox::information(this, "提示", "删除成功！");
    }
}

void MainWindow::onSearchButtonClicked()
{
    QString searchStudentNo = ui->searchStudentNoEdit->text().trimmed();
    QString searchName = ui->searchNameEdit->text().trimmed();
    QString searchCollege = ui->searchCollegeComboBox->currentText();

    bool hasCollegeCondition = (searchCollege != "全部" &&
                                searchCollege != "请选择" &&
                                !searchCollege.isEmpty());

    bool hasCondition = !searchStudentNo.isEmpty()
                        || !searchName.isEmpty()
                        || hasCollegeCondition;

    int matchCount = 0;

    for (int row = 0; row < ui->studentTableWidget->rowCount(); ++row) {
        QTableWidgetItem *studentNoItem = ui->studentTableWidget->item(row, 0);
        QTableWidgetItem *nameItem = ui->studentTableWidget->item(row, 1);
        QTableWidgetItem *collegeItem = ui->studentTableWidget->item(row, 3);

        bool emptyRow = (!studentNoItem || studentNoItem->text().trimmed().isEmpty());

        // 如果没有输入任何搜索条件，就显示全部行
        if (!hasCondition) {
            ui->studentTableWidget->setRowHidden(row, false);
            continue;
        }

        // 有搜索条件时，空白行隐藏
        if (emptyRow) {
            ui->studentTableWidget->setRowHidden(row, true);
            continue;
        }

        QString studentNo = studentNoItem ? studentNoItem->text().trimmed() : "";
        QString name = nameItem ? nameItem->text().trimmed() : "";
        QString college = collegeItem ? collegeItem->text().trimmed() : "";

        bool matched = true;

        if (!searchStudentNo.isEmpty() &&
            !studentNo.contains(searchStudentNo, Qt::CaseInsensitive)) {
            matched = false;
        }

        if (!searchName.isEmpty() &&
            !name.contains(searchName, Qt::CaseInsensitive)) {
            matched = false;
        }

        if (hasCollegeCondition && college != searchCollege) {
            matched = false;
        }

        ui->studentTableWidget->setRowHidden(row, !matched);

        if (matched) {
            matchCount++;
        }
    }

    if (hasCondition && matchCount == 0) {
        QMessageBox::information(this, "搜索结果", "没有找到符合条件的学生。");
    }
}

// 初始化 SQLite 数据库：打开 student.db，并创建 students 学生表
bool MainWindow::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("student.db");

    if (!db.open()) {
        QMessageBox::critical(this,
                              "数据库错误",
                              "数据库打开失败：\n" + db.lastError().text());
        return false;
    }

    // 启用 SQLite 外键约束
    QSqlQuery pragmaQuery(db);

    if (!pragmaQuery.exec("PRAGMA foreign_keys = ON")) {
        QMessageBox::critical(this,
                              "数据库错误",
                              "启用数据库外键失败：\n"
                                  + pragmaQuery.lastError().text());
        return false;
    }

    QSqlQuery query;

    QString sql =
        "CREATE TABLE IF NOT EXISTS students ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "student_no TEXT UNIQUE NOT NULL,"
        "name TEXT NOT NULL,"
        "gender TEXT NOT NULL,"
        "college TEXT NOT NULL,"
        "major TEXT NOT NULL"
        ")";

    if (!query.exec(sql)) {
        QMessageBox::critical(this,
                              "数据库错误",
                              "学生表创建失败：\n" + query.lastError().text());
        return false;
    }

    // 创建考勤记录表
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS attendance_records ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "student_no TEXT NOT NULL, "
            "student_name TEXT NOT NULL, "
            "attendance_date TEXT NOT NULL, "
            "check_in_time TEXT NOT NULL, "
            "status TEXT NOT NULL DEFAULT '正常', "
            "similarity REAL DEFAULT 0, "
            "UNIQUE(student_no, attendance_date)"
            ")")) {
        QMessageBox::warning(
            this,
            QStringLiteral("数据库错误"),
            QStringLiteral("创建考勤记录表失败：\n%1")
                .arg(query.lastError().text())
            );
    }

    // 使用学生数据库连接创建考勤记录表
    QSqlQuery attendanceQuery(db);

    QString attendanceSql =
        "CREATE TABLE IF NOT EXISTS attendance_records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "student_no TEXT NOT NULL, "
        "student_name TEXT NOT NULL, "
        "attendance_date TEXT NOT NULL, "
        "check_in_time TEXT NOT NULL, "
        "status TEXT NOT NULL DEFAULT '正常', "
        "similarity REAL NOT NULL DEFAULT 0, "
        "UNIQUE(student_no, attendance_date)"
        ")";

    // 执行考勤记录表建表语句
    if (!attendanceQuery.exec(attendanceSql)) {
        QMessageBox::critical(
            this,
            QStringLiteral("数据库错误"),
            QStringLiteral("考勤记录表创建失败：\n%1")
                .arg(attendanceQuery.lastError().text())
            );
        return false;
    }

    return true;

    return true;
}

// 从数据库读取所有学生信息，并显示到主界面的表格中
void MainWindow::addStudentToTable(const QString &studentNo,
                                   const QString &name,
                                   const QString &gender,
                                   const QString &college,
                                   const QString &major)
{
    int row = -1;

    for (int i = 0; i < ui->studentTableWidget->rowCount(); ++i) {
        QTableWidgetItem *item = ui->studentTableWidget->item(i, 0);

        if (item == nullptr || item->text().trimmed().isEmpty()) {
            row = i;
            break;
        }
    }

    if (row == -1) {
        row = ui->studentTableWidget->rowCount();
        ui->studentTableWidget->insertRow(row);
    }

    ui->studentTableWidget->setItem(row, 0, new QTableWidgetItem(studentNo));
    ui->studentTableWidget->setItem(row, 1, new QTableWidgetItem(name));
    ui->studentTableWidget->setItem(row, 2, new QTableWidgetItem(gender));
    ui->studentTableWidget->setItem(row, 3, new QTableWidgetItem(college));
    ui->studentTableWidget->setItem(row, 4, new QTableWidgetItem(major));
}

// 把一条学生信息添加到表格中
void MainWindow::loadStudentsFromDatabase()
{
    ui->studentTableWidget->setRowCount(0);

    QSqlQuery query;

    if (!query.exec("SELECT student_no, name, gender, college, major FROM students ORDER BY student_no")) {
        QMessageBox::warning(this,
                             "数据库错误",
                             "读取学生信息失败：\n" + query.lastError().text());
        return;
    }

    while (query.next()) {
        QString studentNo = query.value(0).toString();
        QString name = query.value(1).toString();
        QString gender = query.value(2).toString();
        QString college = query.value(3).toString();
        QString major = query.value(4).toString();

        addStudentToTable(studentNo, name, gender, college, major);
    }

    while (ui->studentTableWidget->rowCount() < 10) {
        ui->studentTableWidget->insertRow(ui->studentTableWidget->rowCount());
    }
}

// “成绩管理”按钮和菜单项共用的响应函数
void MainWindow::onScoreManagementClicked()
{
    ScoreManagementDialog dialog(this);
    dialog.exec();
}

// 点击按钮后打开考勤管理窗口
void MainWindow::on_attendanceButton_clicked()
{
    AttendanceManagementDialog dialog(this);
    dialog.exec();
}