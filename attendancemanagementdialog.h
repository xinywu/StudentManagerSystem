#ifndef ATTENDANCEMANAGEMENTDIALOG_H
#define ATTENDANCEMANAGEMENTDIALOG_H

#include <QDialog>

namespace Ui {
class AttendanceManagementDialog;
}

class AttendanceManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AttendanceManagementDialog(QWidget *parent = nullptr);
    ~AttendanceManagementDialog();

private slots:
    // 打开人脸信息录入窗口
    void on_faceRegisterButton_clicked();

    // 打开人脸考勤窗口
    void on_startAttendanceButton_clicked();

    // 查看考勤记录
    void on_attendanceRecordButton_clicked();

    // 关闭考勤管理窗口
    void on_closeButton_clicked();

private:
    Ui::AttendanceManagementDialog *ui;
};

#endif // ATTENDANCEMANAGEMENTDIALOG_H