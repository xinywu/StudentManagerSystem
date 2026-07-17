#include "scoremanagementdialog.h"
#include "ui_scoremanagementdialog.h"
#include "scoredialog.h"

#include <QComboBox>
#include <QStringList>
#include <QPushButton>
#include <QAbstractItemView>
#include <QLineEdit>

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDialog>
#include <QVariant>

ScoreManagementDialog::ScoreManagementDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ScoreManagementDialog)
{
    ui->setupUi(this);

    // 点击任意单元格时选中整行
    ui->scoreTableWidget->setSelectionBehavior(
        QAbstractItemView::SelectRows);

    // 每次只能选择一行
    ui->scoreTableWidget->setSelectionMode(
        QAbstractItemView::SingleSelection);

    // 禁止直接双击修改表格内容，修改必须通过“编辑成绩”
    ui->scoreTableWidget->setEditTriggers(
        QAbstractItemView::NoEditTriggers);

    // 窗口第一次打开时，根据当前学期初始化课程列表
    updateSearchCourseComboBox(
        ui->searchSemesterComboBox->currentText()
        );

    // 创建成绩表，创建成功后加载已有成绩
    if (initScoreTable()) {
        loadScoresFromDatabase();
    }

    // 点击“新增成绩”按钮时，执行 onAddScoreButtonClicked()
    connect(ui->addScoreButton,
            &QPushButton::clicked,
            this,
            &ScoreManagementDialog::onAddScoreButtonClicked);

    // 点击“编辑成绩”按钮时，执行编辑函数
    connect(ui->editScoreButton,
            &QPushButton::clicked,
            this,
            &ScoreManagementDialog::onEditScoreButtonClicked);

    // 点击“删除成绩”按钮时，执行删除函数
    connect(ui->deleteScoreButton,
            &QPushButton::clicked,
            this,
            &ScoreManagementDialog::onDeleteScoreButtonClicked);

    // 学期选项变化时，自动更新课程下拉框
    connect(ui->searchSemesterComboBox,
            &QComboBox::currentTextChanged,
            this,
            &ScoreManagementDialog::updateSearchCourseComboBox);

    // 点击“搜索”按钮时筛选成绩
    connect(ui->searchScoreButton,
            &QPushButton::clicked,
            this,
            &ScoreManagementDialog::onSearchScoreButtonClicked);

    // 点击“重置”按钮时恢复全部成绩
    connect(ui->resetSearchButton,
            &QPushButton::clicked,
            this,
            &ScoreManagementDialog::onResetSearchButtonClicked);

    // 在学号输入框按回车，直接执行搜索
    connect(ui->searchStudentNoEdit,
            &QLineEdit::returnPressed,
            this,
            &ScoreManagementDialog::onSearchScoreButtonClicked);

    // 在姓名输入框按回车，直接执行搜索
    connect(ui->searchNameEdit,
            &QLineEdit::returnPressed,
            this,
            &ScoreManagementDialog::onSearchScoreButtonClicked);
}

ScoreManagementDialog::~ScoreManagementDialog()
{
    delete ui;
}

// 根据所选学期更新课程下拉框
void ScoreManagementDialog::updateSearchCourseComboBox(
    const QString &semester)
{
    // 先清空原来的课程，避免重复添加
    ui->searchCourseComboBox->clear();

    // 搜索课程时，“全部”表示不限制具体课程
    ui->searchCourseComboBox->addItem("全部");

    QStringList courses;

    if (semester == "2025-2026第一学期") {
        courses << "微积分"
                << "大学英语"
                << "程序设计与算法基础"
                << "思想道德与法治";
    }
    else if (semester == "2025-2026第二学期") {
        courses << "线性代数"
                << "大学物理"
                << "数据结构"
                << "离散数学";
    }
    else if (semester == "2026-2027第一学期") {
        courses << "概率论与数理统计"
                << "计算机组成原理"
                << "操作系统"
                << "Java程序设计";
    }
    else if (semester == "2026-2027第二学期") {
        courses << "计算机网络"
                << "数据库原理"
                << "软件工程"
                << "人工智能导论";
    }
    else if (semester == "全部") {
        // 选择“全部”时，课程下拉框显示所有学期的课程
        courses << "微积分"
                << "大学英语"
                << "程序设计与算法基础"
                   ""
                << "思想道德与法治"
                << "线性代数"
                << "大学物理"
                << "数据结构"
                << "离散数学"
                << "概率论与数理统计"
                << "计算机组成原理"
                << "操作系统"
                << "Java程序设计"
                << "计算机网络"
                << "数据库原理"
                << "软件工程"
                << "人工智能导论";
    }

    // 一次性把课程添加到下拉框中
    ui->searchCourseComboBox->addItems(courses);
}

// 点击“新增成绩”按钮后，打开对话框并保存成绩
void ScoreManagementDialog::onAddScoreButtonClicked()
{
    ScoreDialog dialog(this);
    dialog.setWindowTitle("新增成绩");

    // 用户点击取消，直接结束
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    // 从 ScoreDialog 获取用户填写的数据
    QString studentNo = dialog.studentNo();
    QString semester = dialog.semester();
    QString course = dialog.course();
    double score = dialog.score();

    // 检查是否选择了学生
    if (studentNo.isEmpty()) {
        QMessageBox::warning(
            this,
            "提示",
            "请选择学生！"
            );
        return;
    }

    // 检查是否选择了具体学期
    if (semester.isEmpty() ||
        semester == "请选择学期") {

        QMessageBox::warning(
            this,
            "提示",
            "请选择学期！"
            );
        return;
    }

    // 检查是否选择了具体课程
    if (course.isEmpty() ||
        course == "请选择课程" ||
        course == "请先选择学期") {

        QMessageBox::warning(
            this,
            "提示",
            "请选择课程！"
            );
        return;
    }

    // 检查该成绩是否已经存在
    QSqlQuery checkQuery;

    checkQuery.prepare(
        "SELECT COUNT(*) "
        "FROM scores "
        "WHERE student_no = ? "
        "AND semester = ? "
        "AND course = ?"
        );

    checkQuery.addBindValue(studentNo);
    checkQuery.addBindValue(semester);
    checkQuery.addBindValue(course);

    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::warning(
            this,
            "数据库错误",
            "检查成绩是否重复时失败：\n"
                + checkQuery.lastError().text()
            );
        return;
    }

    if (checkQuery.value(0).toInt() > 0) {
        QMessageBox::warning(
            this,
            "提示",
            "该学生在当前学期的这门课程成绩已经存在！"
            );
        return;
    }

    // 把成绩写入 scores 表
    QSqlQuery insertQuery;

    insertQuery.prepare(
        "INSERT INTO scores "
        "(student_no, semester, course, score) "
        "VALUES (?, ?, ?, ?)"
        );

    insertQuery.addBindValue(studentNo);
    insertQuery.addBindValue(semester);
    insertQuery.addBindValue(course);
    insertQuery.addBindValue(score);

    if (!insertQuery.exec()) {
        QMessageBox::warning(
            this,
            "数据库错误",
            "新增成绩失败：\n"
                + insertQuery.lastError().text()
            );
        return;
    }

    // 重新从数据库读取成绩并显示
    loadScoresFromDatabase();

    QMessageBox::information(
        this,
        "提示",
        "成绩添加成功！"
        );
}

// 创建 scores 成绩表
bool ScoreManagementDialog::initScoreTable()
{
    QSqlQuery query;

    QString sql =
        "CREATE TABLE IF NOT EXISTS scores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "student_no TEXT NOT NULL,"
        "semester TEXT NOT NULL,"
        "course TEXT NOT NULL,"
        "score REAL NOT NULL CHECK(score >= 0 AND score <= 100),"
        "UNIQUE(student_no, semester, course),"
        "FOREIGN KEY(student_no) REFERENCES students(student_no) "
        "ON UPDATE CASCADE "
        "ON DELETE CASCADE"
        ")";

    if (!query.exec(sql)) {
        QMessageBox::critical(
            this,
            "数据库错误",
            "创建成绩表失败：\n" + query.lastError().text()
            );

        return false;
    }

    return true;
}

// 从数据库读取所有成绩，并显示到成绩表格
void ScoreManagementDialog::loadScoresFromDatabase()
{
    // 清空原来的表格，避免重复显示
    ui->scoreTableWidget->setRowCount(0);

    QSqlQuery query;

    QString sql =
        "SELECT scores.student_no, "
        "students.name, "
        "scores.semester, "
        "scores.course, "
        "scores.score "
        "FROM scores "
        "INNER JOIN students "
        "ON scores.student_no = students.student_no "
        "ORDER BY scores.student_no, "
        "scores.semester, "
        "scores.course";

    if (!query.exec(sql)) {
        QMessageBox::warning(
            this,
            "数据库错误",
            "读取成绩失败：\n" + query.lastError().text()
            );

        return;
    }

    while (query.next()) {
        int row = ui->scoreTableWidget->rowCount();

        // 增加一行
        ui->scoreTableWidget->insertRow(row);

        QString studentNo = query.value(0).toString();
        QString name = query.value(1).toString();
        QString semester = query.value(2).toString();
        QString course = query.value(3).toString();
        double score = query.value(4).toDouble();

        // 第0列：学号
        ui->scoreTableWidget->setItem(
            row, 0, new QTableWidgetItem(studentNo));

        // 第1列：姓名
        ui->scoreTableWidget->setItem(
            row, 1, new QTableWidgetItem(name));

        // 第2列：学期
        ui->scoreTableWidget->setItem(
            row, 2, new QTableWidgetItem(semester));

        // 第3列：课程
        ui->scoreTableWidget->setItem(
            row, 3, new QTableWidgetItem(course));

        // 第4列：成绩
        ui->scoreTableWidget->setItem(
            row, 4,
            new QTableWidgetItem(QString::number(score, 'f', 1)));
    }

    // 没有足够数据时补充空行，使界面保持整齐
    while (ui->scoreTableWidget->rowCount() < 10) {
        ui->scoreTableWidget->insertRow(
            ui->scoreTableWidget->rowCount());
    }
}

// 编辑当前选中的成绩
void ScoreManagementDialog::onEditScoreButtonClicked()
{
    // 获取当前选中的行号
    int row = ui->scoreTableWidget->currentRow();

    // 没有选中任何行
    if (row < 0) {
        QMessageBox::warning(
            this,
            "提示",
            "请先选择要编辑的成绩！");
        return;
    }

    // 读取当前行的五个单元格
    QTableWidgetItem *studentNoItem =
        ui->scoreTableWidget->item(row, 0);

    QTableWidgetItem *nameItem =
        ui->scoreTableWidget->item(row, 1);

    QTableWidgetItem *semesterItem =
        ui->scoreTableWidget->item(row, 2);

    QTableWidgetItem *courseItem =
        ui->scoreTableWidget->item(row, 3);

    QTableWidgetItem *scoreItem =
        ui->scoreTableWidget->item(row, 4);

    // 防止用户选中表格中的空白行
    if (!studentNoItem ||
        studentNoItem->text().trimmed().isEmpty()) {

        QMessageBox::warning(
            this,
            "提示",
            "请选择一条有效的成绩记录！");
        return;
    }

    // 保存修改前的数据
    // 后面 UPDATE 的 WHERE 条件要用这些原数据
    QString oldStudentNo =
        studentNoItem->text().trimmed();

    QString oldSemester =
        semesterItem ? semesterItem->text().trimmed() : "";

    QString oldCourse =
        courseItem ? courseItem->text().trimmed() : "";

    double oldScore =
        scoreItem ? scoreItem->text().toDouble() : 0.0;

    // 创建编辑成绩对话框
    ScoreDialog dialog(this);
    dialog.setWindowTitle("编辑成绩");

    // 把原成绩信息填入对话框
    dialog.setScoreInfo(oldStudentNo,
                        oldSemester,
                        oldCourse,
                        oldScore);

    // 用户点击取消时，不进行任何修改
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    // 获取用户修改后的数据
    QString newStudentNo = dialog.studentNo();
    QString newSemester = dialog.semester();
    QString newCourse = dialog.course();
    double newScore = dialog.score();

    // 检查学生
    if (newStudentNo.isEmpty()) {
        QMessageBox::warning(
            this,
            "提示",
            "请选择学生！");
        return;
    }

    // 检查学期
    if (newSemester.isEmpty() ||
        newSemester == "请选择学期") {

        QMessageBox::warning(
            this,
            "提示",
            "请选择学期！");
        return;
    }

    // 检查课程
    if (newCourse.isEmpty() ||
        newCourse == "请选择课程" ||
        newCourse == "请先选择学期") {

        QMessageBox::warning(
            this,
            "提示",
            "请选择课程！");
        return;
    }

    /*
     * 如果学生、学期或课程发生变化，
     * 需要检查新的组合是否已经存在。
     *
     * 例如数据库中已经有：
     * 张三 + 第一学期 + 高等数学
     *
     * 就不能把另一条成绩也修改成同一个组合。
     */
    bool keyChanged =
        newStudentNo != oldStudentNo ||
        newSemester != oldSemester ||
        newCourse != oldCourse;

    if (keyChanged) {
        QSqlQuery checkQuery;

        checkQuery.prepare(
            "SELECT COUNT(*) "
            "FROM scores "
            "WHERE student_no = ? "
            "AND semester = ? "
            "AND course = ?"
            );

        checkQuery.addBindValue(newStudentNo);
        checkQuery.addBindValue(newSemester);
        checkQuery.addBindValue(newCourse);

        if (!checkQuery.exec() || !checkQuery.next()) {
            QMessageBox::warning(
                this,
                "数据库错误",
                "检查成绩是否重复时失败：\n"
                    + checkQuery.lastError().text());
            return;
        }

        if (checkQuery.value(0).toInt() > 0) {
            QMessageBox::warning(
                this,
                "提示",
                "该学生在当前学期的这门课程成绩已经存在！");
            return;
        }
    }

    // 更新数据库
    QSqlQuery updateQuery;

    updateQuery.prepare(
        "UPDATE scores "
        "SET student_no = ?, "
        "semester = ?, "
        "course = ?, "
        "score = ? "
        "WHERE student_no = ? "
        "AND semester = ? "
        "AND course = ?"
        );

    // SET 后的新数据
    updateQuery.addBindValue(newStudentNo);
    updateQuery.addBindValue(newSemester);
    updateQuery.addBindValue(newCourse);
    updateQuery.addBindValue(newScore);

    // WHERE 后的原数据
    updateQuery.addBindValue(oldStudentNo);
    updateQuery.addBindValue(oldSemester);
    updateQuery.addBindValue(oldCourse);

    if (!updateQuery.exec()) {
        QMessageBox::warning(
            this,
            "数据库错误",
            "修改成绩失败：\n"
                + updateQuery.lastError().text());
        return;
    }

    // 没有找到需要更新的记录
    if (updateQuery.numRowsAffected() == 0) {
        QMessageBox::warning(
            this,
            "提示",
            "没有找到需要修改的成绩记录！");
        return;
    }

    // 重新加载数据库中的成绩
    loadScoresFromDatabase();

    QMessageBox::information(
        this,
        "提示",
        "成绩修改成功！");
}

// 删除当前选中的成绩
void ScoreManagementDialog::onDeleteScoreButtonClicked()
{
    // 获取当前选中的表格行号
    int row = ui->scoreTableWidget->currentRow();

    // 没有选中任何行
    if (row < 0) {
        QMessageBox::warning(
            this,
            "提示",
            "请先选择要删除的成绩！"
            );
        return;
    }

    // 读取当前行中的学号、姓名、学期和课程
    QTableWidgetItem *studentNoItem =
        ui->scoreTableWidget->item(row, 0);

    QTableWidgetItem *nameItem =
        ui->scoreTableWidget->item(row, 1);

    QTableWidgetItem *semesterItem =
        ui->scoreTableWidget->item(row, 2);

    QTableWidgetItem *courseItem =
        ui->scoreTableWidget->item(row, 3);

    // 防止用户选中空白行
    if (!studentNoItem ||
        studentNoItem->text().trimmed().isEmpty()) {

        QMessageBox::warning(
            this,
            "提示",
            "请选择一条有效的成绩记录！"
            );
        return;
    }

    // 取出这一条成绩的信息
    QString studentNo =
        studentNoItem->text().trimmed();

    QString name =
        nameItem ? nameItem->text().trimmed() : "";

    QString semester =
        semesterItem ? semesterItem->text().trimmed() : "";

    QString course =
        courseItem ? courseItem->text().trimmed() : "";

    // 弹出确认框，防止误删
    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(
        this,
        "确认删除",
        QString("确定要删除以下成绩吗？\n\n"
                "学生：%1（%2）\n"
                "学期：%3\n"
                "课程：%4")
            .arg(name, studentNo, semester, course),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    // 用户点击“否”，不删除
    if (reply != QMessageBox::Yes) {
        return;
    }

    // 根据“学号 + 学期 + 课程”删除数据库记录
    QSqlQuery deleteQuery;

    deleteQuery.prepare(
        "DELETE FROM scores "
        "WHERE student_no = ? "
        "AND semester = ? "
        "AND course = ?"
        );

    deleteQuery.addBindValue(studentNo);
    deleteQuery.addBindValue(semester);
    deleteQuery.addBindValue(course);

    if (!deleteQuery.exec()) {
        QMessageBox::warning(
            this,
            "数据库错误",
            "删除成绩失败：\n"
                + deleteQuery.lastError().text()
            );
        return;
    }

    // 判断数据库中是否真的删除了一条记录
    if (deleteQuery.numRowsAffected() == 0) {
        QMessageBox::warning(
            this,
            "提示",
            "没有找到需要删除的成绩记录！"
            );
        return;
    }

    // 删除成功后，重新从数据库加载成绩
    loadScoresFromDatabase();

    QMessageBox::information(
        this,
        "提示",
        "成绩删除成功！"
        );
}

// 根据学号、姓名、学期和课程筛选成绩
void ScoreManagementDialog::onSearchScoreButtonClicked()
{
    // 获取搜索条件，并去除前后空格
    QString studentNo =
        ui->searchStudentNoEdit->text().trimmed();

    QString name =
        ui->searchNameEdit->text().trimmed();

    QString semester =
        ui->searchSemesterComboBox->currentText();

    QString course =
        ui->searchCourseComboBox->currentText();

    int matchCount = 0;

    // 遍历成绩表格中的所有行
    for (int row = 0;
         row < ui->scoreTableWidget->rowCount();
         ++row) {

        QTableWidgetItem *studentNoItem =
            ui->scoreTableWidget->item(row, 0);

        QTableWidgetItem *nameItem =
            ui->scoreTableWidget->item(row, 1);

        QTableWidgetItem *semesterItem =
            ui->scoreTableWidget->item(row, 2);

        QTableWidgetItem *courseItem =
            ui->scoreTableWidget->item(row, 3);

        // 空白行不是有效成绩，搜索时直接隐藏
        if (!studentNoItem ||
            studentNoItem->text().trimmed().isEmpty()) {

            ui->scoreTableWidget->setRowHidden(row, true);
            continue;
        }

        QString rowStudentNo =
            studentNoItem->text().trimmed();

        QString rowName =
            nameItem ? nameItem->text().trimmed() : "";

        QString rowSemester =
            semesterItem ? semesterItem->text().trimmed() : "";

        QString rowCourse =
            courseItem ? courseItem->text().trimmed() : "";

        /*
         * 学号和姓名使用“包含匹配”。
         *
         * 例如输入学号 2024，
         * 可以匹配 20240001、20240002 等。
         */
        bool studentNoMatched =
            studentNo.isEmpty() ||
            rowStudentNo.contains(
                studentNo,
                Qt::CaseInsensitive);

        bool nameMatched =
            name.isEmpty() ||
            rowName.contains(
                name,
                Qt::CaseInsensitive);

        /*
         * 学期和课程使用“完全匹配”。
         * 选择“全部”时不限制该条件。
         */
        bool semesterMatched =
            semester.isEmpty() ||
            semester == "全部" ||
            rowSemester == semester;

        bool courseMatched =
            course.isEmpty() ||
            course == "全部" ||
            rowCourse == course;

        // 所有条件必须同时满足
        bool matched =
            studentNoMatched &&
            nameMatched &&
            semesterMatched &&
            courseMatched;

        // 不匹配就隐藏，匹配就显示
        ui->scoreTableWidget->setRowHidden(
            row,
            !matched);

        if (matched) {
            ++matchCount;
        }
    }

    // 没有匹配结果时给出提示
    if (matchCount == 0) {
        QMessageBox::information(
            this,
            "搜索结果",
            "没有找到符合条件的成绩记录！");
    }
}

// 清空搜索条件，并恢复显示全部成绩
void ScoreManagementDialog::onResetSearchButtonClicked()
{
    // 清空学号和姓名
    ui->searchStudentNoEdit->clear();
    ui->searchNameEdit->clear();

    // 学期恢复为“全部”
    int semesterIndex =
        ui->searchSemesterComboBox->findText("全部");

    if (semesterIndex >= 0) {
        ui->searchSemesterComboBox->setCurrentIndex(
            semesterIndex);
    } else {
        ui->searchSemesterComboBox->setCurrentIndex(0);
    }

    /*
     * 学期变成“全部”后，
     * updateSearchCourseComboBox() 会重新生成课程列表。
     */

    // 课程恢复为“全部”
    int courseIndex =
        ui->searchCourseComboBox->findText("全部");

    if (courseIndex >= 0) {
        ui->searchCourseComboBox->setCurrentIndex(
            courseIndex);
    } else {
        ui->searchCourseComboBox->setCurrentIndex(0);
    }

    // 恢复显示表格中的所有行
    for (int row = 0;
         row < ui->scoreTableWidget->rowCount();
         ++row) {

        ui->scoreTableWidget->setRowHidden(row, false);
    }

    // 清除原来的行选择
    ui->scoreTableWidget->clearSelection();
    ui->scoreTableWidget->setCurrentCell(-1, -1);
}