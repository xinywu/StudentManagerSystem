#include "attendancerecorddialog.h"
#include "ui_attendancerecorddialog.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidgetItem>

AttendanceRecordDialog::AttendanceRecordDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AttendanceRecordDialog)
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle(QStringLiteral("考勤记录查询"));

    // 设置表格为整行选择
    ui->attendanceTableWidget->setSelectionBehavior(
        QAbstractItemView::SelectRows);

    // 设置表格一次只能选择一行
    ui->attendanceTableWidget->setSelectionMode(
        QAbstractItemView::SingleSelection);

    // 禁止直接修改表格内容
    ui->attendanceTableWidget->setEditTriggers(
        QAbstractItemView::NoEditTriggers);

    // 让六列表头自动铺满表格
    ui->attendanceTableWidget->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

    // 打开窗口时自动加载考勤记录
    loadAttendanceRecords();


}

AttendanceRecordDialog::~AttendanceRecordDialog()
{
    delete ui;
}

// 从数据库加载考勤记录
void AttendanceRecordDialog::loadAttendanceRecords()
{
    // 清空表格中的旧数据
    ui->attendanceTableWidget->setRowCount(0);

    QSqlQuery query;

    // 按日期和签到时间倒序查询考勤记录
    if (!query.exec(
            "SELECT student_no, student_name, attendance_date, "
            "check_in_time, status, similarity "
            "FROM attendance_records "
            "ORDER BY attendance_date DESC, check_in_time DESC")) {
        QMessageBox::warning(
            this,
            QStringLiteral("数据库错误"),
            QStringLiteral("读取考勤记录失败：\n%1")
                .arg(query.lastError().text()));
        return;
    }

    // 将每一条考勤记录添加到表格
    while (query.next()) {
        const int row = ui->attendanceTableWidget->rowCount();
        ui->attendanceTableWidget->insertRow(row);

        ui->attendanceTableWidget->setItem(
            row, 0, new QTableWidgetItem(query.value(0).toString()));

        ui->attendanceTableWidget->setItem(
            row, 1, new QTableWidgetItem(query.value(1).toString()));

        ui->attendanceTableWidget->setItem(
            row, 2, new QTableWidgetItem(query.value(2).toString()));

        ui->attendanceTableWidget->setItem(
            row, 3, new QTableWidgetItem(query.value(3).toString()));

        ui->attendanceTableWidget->setItem(
            row, 4, new QTableWidgetItem(query.value(4).toString()));

        ui->attendanceTableWidget->setItem(
            row, 5, new QTableWidgetItem(
                QString::number(query.value(5).toDouble(), 'f', 3)));
    }
}