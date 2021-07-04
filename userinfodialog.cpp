#include "userinfodialog.h"
#include "ui_userinfodialog.h"
#include "veritydialog.h"

userInfoDialog::userInfoDialog(QWidget *parent, User *user) :
    QDialog(parent),
    ui(new Ui::userInfoDialog),
    user(user)
{
    ui->setupUi(this);
    ui->idLineEdit->setText(QString::number(user->getId()));
    ui->nameLineEdit->setText(user->getName());
    ui->telLineEdit->setText(user->getTel().toString());
    ui->ageSpinBox->setValue(user->getAge());
    ui->pwdLineEdit->setText(user->getPwd().decrypt());

    QObject::connect(this, SIGNAL(logout()), parent, SLOT(receiveLogout()));
}

userInfoDialog::~userInfoDialog()
{
    delete ui;
}

void userInfoDialog::on_updateNamePushButton_clicked()
{
    if (ui->updateNamePushButton->text() == "修改") {
        ui->nameLineEdit->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        ui->nameLineEdit->setFocus();
        ui->updateNamePushButton->setText("完成");
    } else {
        if (!ui->nameLineEdit->text().isEmpty() && ui->nameLineEdit->text() != user->getName()) {
            user->setName(ui->nameLineEdit->text());
            QMessageBox::information(this, "操作提示", "修改成功！");
        } else {
            QMessageBox::information(this, "操作提示", "未更新！");
        }
        ui->nameLineEdit->setText(user->getName());
        ui->nameLineEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        ui->nameLineEdit->clearFocus();
        ui->updateNamePushButton->setText("修改");
    }
}

void userInfoDialog::on_updateAgePushButton_clicked()
{
    if (ui->updateAgePushButton->text() == "修改") {
        ui->ageSpinBox->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        ui->ageSpinBox->setFocus();
        ui->updateAgePushButton->setText("完成");
    } else {
        if (ui->ageSpinBox->value() != 0 && !ui->ageSpinBox->text().isEmpty()
                && ui->ageSpinBox->value() != user->getAge()) {
            user->setAge(ui->ageSpinBox->value());
            QMessageBox::information(this, "操作提示", "修改成功！");
        } else {
            QMessageBox::information(this, "操作提示", "未更新！");
        }
        ui->ageSpinBox->setValue(user->getAge());
        ui->ageSpinBox->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        ui->ageSpinBox->clearFocus();
        ui->updateAgePushButton->setText("修改");
    }
}

void userInfoDialog::on_updateTelPushButton_clicked()
{
    if (ui->updateTelPushButton->text() == "修改") {
        ui->telLineEdit->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        ui->telLineEdit->setFocus();
        ui->updateTelPushButton->setText("完成");
    } else {
        ui->telLineEdit->editingFinished();
        if (!ui->telLineEdit->text().isEmpty() && ui->telLineEdit->text() != user->getTel().toString()) {
            try {
                user->setTel(Telephone{ui->telLineEdit->text()});
                QMessageBox::information(this, "操作提示", "修改成功！");
            } catch (std::invalid_argument &e) {
                QMessageBox::warning(this, "操作失败", "不是正确的电话号码！");
            }
        } else {
            QMessageBox::information(this, "操作提示", "未更新！");
        }
        ui->telLineEdit->setText(user->getTel().toString());
        ui->telLineEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        ui->telLineEdit->clearFocus();
        ui->updateTelPushButton->setText("修改");
    }
}

void userInfoDialog::on_updatePwdPushButton_clicked()
{
    if (ui->updatePwdPushButton->text() == "修改") {
        // 先确认旧密码
        VerityDialog dia(this, manage.getCurrentUser(), "修改密码，请确认这是你");
        if (dia.exec() == QDialog::Rejected) {
            return;
        }

        ui->pwdLineEdit->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        ui->pwdLineEdit->setFocus();
        ui->updatePwdPushButton->setText("完成");
    } else {
        if (!ui->pwdLineEdit->text().isEmpty() && ui->pwdLineEdit->text() != user->getPwd().decrypt()) {
            user->setPwd(Password{ui->pwdLineEdit->text()});
            QMessageBox::information(this, "操作提示", "修改成功！");
        } else {
            QMessageBox::information(this, "操作提示", "未更新！");
        }
        ui->pwdLineEdit->setText(user->getPwd().decrypt());
        ui->pwdLineEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        ui->pwdLineEdit->clearFocus();
        ui->updatePwdPushButton->setText("修改");
    }
}

void userInfoDialog::on_logOutPushButton_clicked()
{
    // 先确认旧密码
    VerityDialog dia(this, manage.getCurrentUser(), "注销账户，请确认这是你");
    if (dia.exec() == QDialog::Rejected) {
        return;
    }

    try {
        manage.logoutUser(*manage.getCurrentUser());
        QMessageBox::information(this,"操作成功","您已经注销账户，无法再次登录");
        emit logout();
        return accept();
    } catch (std::invalid_argument &e) {
        QMessageBox::warning(this,"操作失败",e.what());
    }
}
