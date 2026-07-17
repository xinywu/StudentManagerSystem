#ifndef SCOREMANAGEMENTDIALOG_H
#define SCOREMANAGEMENTDIALOG_H

#include <QDialog>

namespace Ui {
class ScoreManagementDialog;
}

class ScoreManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScoreManagementDialog(QWidget *parent = nullptr);
    ~ScoreManagementDialog();

private slots:
    // 当搜索区域中的学期发生变化时，
    // 根据所选学期更新课程下拉框中的课程
    void updateSearchCourseComboBox(const QString &semester);

    // 点击“新增成绩”按钮后，打开新增成绩对话框
    void onAddScoreButtonClicked();

    // 点击“编辑成绩”按钮后的响应函数
    void onEditScoreButtonClicked();

    // 点击“删除成绩”按钮后的响应函数
    void onDeleteScoreButtonClicked();

    // 点击“搜索”按钮
    void onSearchScoreButtonClicked();

    // 点击“重置”按钮
    void onResetSearchButtonClicked();

private:
    Ui::ScoreManagementDialog *ui;

    // 创建 scores 成绩表
    bool initScoreTable();

    // 从数据库读取全部成绩，并显示到成绩表格
    void loadScoresFromDatabase();
};

#endif // SCOREMANAGEMENTDIALOG_H
