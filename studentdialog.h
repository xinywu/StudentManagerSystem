#ifndef STUDENTDIALOG_H
#define STUDENTDIALOG_H

#include <QDialog>

namespace Ui {
class StudentDialog;
}

class StudentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StudentDialog(QWidget *parent = nullptr);
    ~StudentDialog();

    QString studentNo() const;
    QString studentName() const;
    QString gender() const;
    QString college() const;
    QString major() const;

    void setStudentInfo(const QString &studentNo,
                        const QString &name,
                        const QString &gender,
                        const QString &college,
                        const QString &major);


private:
    Ui::StudentDialog *ui;
};

#endif // STUDENTDIALOG_H
