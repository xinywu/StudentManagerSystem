#ifndef SCOREDIALOG_H
#define SCOREDIALOG_H

#include <QDialog>

namespace Ui {
class ScoreDialog;
}

class ScoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScoreDialog(QWidget *parent = nullptr);
    ~ScoreDialog();

    // 获取当前选择学生的真实学号
    QString studentNo() const;

    // 获取当前选择的学期
    QString semester() const;

    // 获取当前选择的课程
    QString course() const;

    // 获取输入的成绩
    double score() const;

    // 编辑成绩时，把原来的学生、学期、课程和成绩填入对话框
    void setScoreInfo(const QString &studentNo,
                      const QString &semester,
                      const QString &course,
                      double score);

private slots:
    // 学期发生变化时，更新课程下拉框
    void updateCourseComboBox(const QString &semester);


private:
    Ui::ScoreDialog *ui;

    // 从 students 数据表读取所有学生，
    // 并将“学号 - 姓名”加入学生下拉框
    void loadStudents();
};

#endif // SCOREDIALOG_H
