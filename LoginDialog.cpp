#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include <QMessageBox>
#include "common.h"
#include <QDebug>
#include <QPushButton>
#include <QValidator>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    // 限制ID只能为数字
    ui->userLineEdit->setValidator(new QIntValidator(1,99999999,this));
}

LoginDialog::~LoginDialog()
{
    delete ui;
    delete registerDialog;
}

void LoginDialog::on_nextButton_clicked()
{
    // 获取参数
    QString id = ui->userLineEdit->text();
    QString pwd = ui->pwdLineEdit->text();
    bool isAnonymous = ui->anonymousBox->isChecked();
    bool isAdmin = ui->adminCheckBox->isChecked();

    // 是否匿名登录
    if (isAnonymous) {
        manage.setCurrentUser(Anonymous);
        this->hide();
        emit showMain();
        return;
    }

    // 检查参数不为空
    if (id.isEmpty()) {
        QMessageBox::warning(this,"警告","用户名为空！",QMessageBox::Yes);
        return;
    } else if (pwd.isEmpty()){
        QMessageBox::warning(this,"警告","密码为空！",QMessageBox::Yes);
        return;
    }

    // 执行不同登录方式
    if (isAdmin) { // 是否管理员登录
        try {
            Administrator &admin = manage.selectAdminByID(id.toInt());
            // 检测密码是否匹配
            if (pwd == admin.getPwd().decrypt()) {
                manage.setCurrentUser(admin);
                this->hide();
                emit showMain();
                return;
            } else
                QMessageBox::warning(this,"警告","密码错误！",QMessageBox::Yes);
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(this,"警告","没有找到该管理员！",QMessageBox::Yes);
        }
    } else {
        try {
            Reader &reader = manage.selectReaderByID(id.toInt());
            // 检测密码是否匹配
            if (pwd == reader.getPwd().decrypt()) {
                manage.setCurrentUser(reader);
                this->hide();
                emit showMain();
                return;
            } else
                QMessageBox::warning(this,"警告","密码错误！",QMessageBox::Yes);
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(this,"警告","没有找到该读者！",QMessageBox::Yes);
        }
    }

}


void LoginDialog::on_anonymousBox_stateChanged(int arg1)
{
    bool status = ui->anonymousBox->isChecked();
    ui->adminCheckBox->setEnabled(!status);
    ui->userLineEdit->setEnabled(!status);
    ui->pwdLineEdit->setEnabled(!status);
}

void LoginDialog::on_ReaderRegisterPushButton_clicked()
{
    RegisterDialog regDialog;
    regDialog.exec();
}

void LoginDialog::receiveShow()
{
    this->show();
    ui->userLineEdit->clear();
    ui->pwdLineEdit->clear();
    ui->anonymousBox->setChecked(false);
    ui->adminCheckBox->setChecked(false);
    ui->userLineEdit->setFocus();
}
void LoginDialog::on_exitButton_clicked()
{
    emit quit();
}
