#include "scoredialog.h"
#include "ui_scoredialog.h"

#include <QComboBox>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

ScoreDialog::ScoreDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ScoreDialog)
{
    ui->setupUi(this);

    // 从 students 数据表加载学生
    loadStudents();

    // 当学期下拉框的文字发生变化时，更新课程下拉框
    connect(ui->semesterComboBox,
            &QComboBox::currentTextChanged,
            this,
            &ScoreDialog::updateCourseComboBox);

    // 窗口第一次打开时，初始化课程下拉框
    updateCourseComboBox(ui->semesterComboBox->currentText());
}


ScoreDialog::~ScoreDialog()
{
    delete ui;
}

// 根据所选学期更新课程下拉框
void ScoreDialog::updateCourseComboBox(const QString &semester)
{
    // 先清空原来的课程，避免课程重复出现
    ui->courseComboBox->clear();

    // 尚未选择具体学期
    if (semester == "请选择学期" || semester.isEmpty()) {
        ui->courseComboBox->addItem("请先选择学期");
        ui->courseComboBox->setEnabled(false);
        return;
    }

    // 已经选择具体学期，允许选择课程
    ui->courseComboBox->setEnabled(true);
    ui->courseComboBox->addItem("请选择课程");

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

    // 将对应学期的全部课程加入下拉框
    ui->courseComboBox->addItems(courses);
}

// 从 students 数据表读取学生，并填入学生下拉框
void ScoreDialog::loadStudents()
{
    // 先清空设计器中原有的内容，防止重复添加
    ui->studentComboBox->clear();

    // 第一项作为提示文字
    // 第二个参数 "" 是这一项内部保存的数据
    ui->studentComboBox->addItem("请选择学生", "");

    QSqlQuery query;

    // 按学号顺序读取学生的学号和姓名
    if (!query.exec(
            "SELECT student_no, name "
            "FROM students "
            "ORDER BY student_no")) {

        QMessageBox::warning(
            this,
            "数据库错误",
            "读取学生信息失败：\n" + query.lastError().text()
            );

        return;
    }

    // 一条一条读取查询结果
    while (query.next()) {
        QString studentNo = query.value(0).toString();
        QString name = query.value(1).toString();

        // 下拉框界面上显示的文字
        QString displayText = studentNo + " - " + name;

        // 第一个参数是显示文字
        // 第二个参数是这一项内部保存的真实学号
        ui->studentComboBox->addItem(displayText, studentNo);
    }
}

// 获取当前选择学生的真实学号
QString ScoreDialog::studentNo() const
{
    return ui->studentComboBox->currentData().toString();
}

// 获取当前选择的学期
QString ScoreDialog::semester() const
{
    return ui->semesterComboBox->currentText();
}

// 获取当前选择的课程
QString ScoreDialog::course() const
{
    return ui->courseComboBox->currentText();
}

// 获取用户输入的成绩
double ScoreDialog::score() const
{
    return ui->scoreSpinBox->value();
}

// 编辑成绩时，把原来的成绩信息填入各个控件
void ScoreDialog::setScoreInfo(const QString &studentNo,
                               const QString &semester,
                               const QString &course,
                               double score)
{
    // 根据学号，在学生下拉框中找到对应学生
    int studentIndex =
        ui->studentComboBox->findData(studentNo);

    if (studentIndex >= 0) {
        ui->studentComboBox->setCurrentIndex(studentIndex);
    }

    // 根据学期文字，找到对应学期
    int semesterIndex =
        ui->semesterComboBox->findText(semester);

    if (semesterIndex >= 0) {
        ui->semesterComboBox->setCurrentIndex(semesterIndex);
    }

    // 根据学期重新生成课程列表
    updateCourseComboBox(semester);

    // 在新的课程列表中找到原课程
    int courseIndex =
        ui->courseComboBox->findText(course);

    if (courseIndex >= 0) {
        ui->courseComboBox->setCurrentIndex(courseIndex);
    }

    // 填入原成绩
    ui->scoreSpinBox->setValue(score);
}