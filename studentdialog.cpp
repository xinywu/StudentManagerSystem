#include "studentdialog.h"
#include "ui_studentdialog.h"

StudentDialog::StudentDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StudentDialog)
{
    ui->setupUi(this);
}

StudentDialog::~StudentDialog()
{
    delete ui;
}

QString StudentDialog::studentNo() const
{
    return ui->studentNoEdit->text().trimmed();
}

QString StudentDialog::studentName() const
{
    return ui->nameEdit->text().trimmed();
}

QString StudentDialog::gender() const
{
    return ui->genderComboBox->currentText();
}

QString StudentDialog::college() const
{
    return ui->collegeComboBox->currentText();
}

QString StudentDialog::major() const
{
    return ui->majorEdit->text().trimmed();
}

void StudentDialog::setStudentInfo(const QString &studentNo,
                                   const QString &name,
                                   const QString &gender,
                                   const QString &college,
                                   const QString &major)
{
    ui->studentNoEdit->setText(studentNo);
    ui->nameEdit->setText(name);
    ui->genderComboBox->setCurrentText(gender);
    ui->collegeComboBox->setCurrentText(college);
    ui->majorEdit->setText(major);
}
