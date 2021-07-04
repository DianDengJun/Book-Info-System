#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "common.h"
#include <QMessageBox>
#include <QDebug>

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_okButton_clicked()
{
    // 获取参数
    QString name = ui->nameLineEdit->text();
    int age = ui->ageSpinBox->value();
    QString strTelephone = ui->telLineEdit->text();
    QString strEnterPwd = ui->enterPwdLineEdit->text();
    QString strCheckPwd = ui->checkPwdLineEdit->text();
    // 参数检测
    if (name.isEmpty()) {
        QMessageBox::warning(this,"输入错误","姓名不能为空！");
        return;
    }
    if (strTelephone.isEmpty()) {
        QMessageBox::warning(this,"输入错误","电话不能为空！");
        return;
    }
    Telephone tel{};
    try {
        tel = strTelephone;
    } catch (std::invalid_argument &e) {
        QMessageBox::warning(this,"电话号码错误","这不是一个正确的电话号码！");
        return;
    }
    if (strEnterPwd.isEmpty()) {
        QMessageBox::warning(this,"输入错误","密码不能为空！");
        return;
    }
    if (strEnterPwd != strCheckPwd) {
        QMessageBox::warning(this,"输入错误","两次密码不一致！");
        return;
    }
    Password pwd{strEnterPwd};
    // 添加读者
    int readerID = manage.registerReader(name,age,tel,pwd);
    QString info = QString{"您好，"} + ui->nameLineEdit->text() + "\n系统为您分配的ID是" + QString::number(readerID);
    QMessageBox::information(this,"注册成功",info);
    return accept();
}
