#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAddButtonClicked();
    void onEditButtonClicked();
    void onDeleteButtonClicked();
    void onSearchButtonClicked();

    // 响应“成绩管理”按钮和菜单项，后面用于打开成绩管理窗口
    void onScoreManagementClicked();

    void on_attendanceButton_clicked();

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;

    bool initDatabase();

    void loadStudentsFromDatabase();

    void addStudentToTable(const QString &studentNo,
                           const QString &name,
                           const QString &gender,
                           const QString &college,
                           const QString &major);
};
#endif // MAINWINDOW_H