#include "attendancemanagementdialog.h"
#include "ui_attendancemanagementdialog.h"
// 引入考勤记录查询窗口
#include "attendancerecorddialog.h"

// 引入已经移植好的 SeetaFace 窗口
#include "facecollectwindow.h"

#include <QMessageBox>

AttendanceManagementDialog::AttendanceManagementDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AttendanceManagementDialog)
{
    ui->setupUi(this);

    // 设置考勤管理窗口标题
    setWindowTitle(QStringLiteral("考勤管理"));
}

AttendanceManagementDialog::~AttendanceManagementDialog()
{
    delete ui;
}

// 打开学生人脸信息录入窗口
void AttendanceManagementDialog::on_faceRegisterButton_clicked()
{
    auto *faceWindow = new FaceCollectWindow(this);

    // 窗口关闭后自动释放内存
    faceWindow->setAttribute(Qt::WA_DeleteOnClose);

    // 设置当前功能标题
    faceWindow->setWindowTitle(QStringLiteral("学生人脸信息录入"));

    faceWindow->show();
}

// 打开人脸识别考勤窗口
void AttendanceManagementDialog::on_startAttendanceButton_clicked()
{
    auto *attendanceWindow = new FaceCollectWindow(this);

    // 窗口关闭后自动释放内存
    attendanceWindow->setAttribute(Qt::WA_DeleteOnClose);

    // 设置当前功能标题
    attendanceWindow->setWindowTitle(QStringLiteral("开始人脸考勤"));

    attendanceWindow->show();
}


// 打开考勤记录查询窗口
void AttendanceManagementDialog::on_attendanceRecordButton_clicked()
{
    AttendanceRecordDialog dialog(this);
    dialog.exec();
}

// 关闭当前窗口
void AttendanceManagementDialog::on_closeButton_clicked()
{
    close();
}
