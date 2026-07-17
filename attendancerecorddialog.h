#ifndef ATTENDANCERECORDDIALOG_H
#define ATTENDANCERECORDDIALOG_H

#include <QDialog>

namespace Ui {
class AttendanceRecordDialog;
}

class AttendanceRecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AttendanceRecordDialog(QWidget *parent = nullptr);
    ~AttendanceRecordDialog();

private:
    // 从数据库加载考勤记录
    void loadAttendanceRecords();

    Ui::AttendanceRecordDialog *ui;
};

#endif // ATTENDANCERECORDDIALOG_H