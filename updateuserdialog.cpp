#include "updateuserdialog.h"
#include "ui_updateuserdialog.h"
#include "QMessageBox"

updateUserDialog::updateUserDialog(QWidget *parent, User *oldUser) :
    QDialog(parent),
    ui(new Ui::updateUserDialog),
    oldUser(oldUser)
{
    ui->setupUi(this);
    if (oldUser->getType() == User::Type::Reader) {
        ui->infoLabel->setText(QString{"更新读者"}+QString::number(oldUser->getId())+QString("的信息："));
        ui->oldNameLabel->setText(oldUser->getName());
        ui->oldTelLabel->setText(oldUser->getTel().toString());
        ui->oldAgeLabel->setText(QString::number(oldUser->getAge()));
        ui->oldNumLabel->setText(QString::number(dynamic_cast<Reader*>(oldUser)->getMaxBorrowNumbers()));
    } else {
        ui->infoLabel->setText(QString{"更新管理员"}+QString::number(oldUser->getId())+QString("的信息："));
        ui->oldNameLabel->setText(oldUser->getName());
        ui->oldTelLabel->setText(oldUser->getTel().toString());
        ui->oldAgeLabel->setText(QString::number(oldUser->getAge()));
        ui->maxBorrowNumbersCheckBox->setHidden(true);
        ui->oldNumLabel->setHidden(true);
        ui->newMaxBorrowNumbersSpinBox->setHidden(true);
    }
}

updateUserDialog::~updateUserDialog()
{
    delete ui;
}

void updateUserDialog::on_buttonBox_accepted()
{
    // 参数获取
    QString name = ui->newNameLineEdit->text();
    QString strTel = ui->newTelLineEdit->text();
    int age = ui->newAgeSpinBox->value();
    int maxBorrowNumbers = ui->newMaxBorrowNumbersSpinBox->value();
    // 参数启用获取
    bool isSelectName = ui->nameCheckBox->isChecked();
    bool isSelectTel = ui->telCheckBox->isChecked();
    bool isSelectAge = ui->ageCheckBox->isChecked();
    bool isSelectMaxBorrowNumbers = ui->maxBorrowNumbersCheckBox->isChecked();

    // 参数为空判断
    if (isSelectName && name.isEmpty()) {
        QMessageBox::warning(this,"输入错误","姓名不得为空！");
        return;
    }
    if (isSelectTel && strTel.isEmpty()) {
        QMessageBox::warning(this,"输入错误","电话号码不得为空！");
        return;
    }
    if (!isSelectName && !isSelectTel && !isSelectAge && !isSelectMaxBorrowNumbers) {
        QMessageBox::information(this,"操作提示","未更新任何字段！");
        return reject();
    }

    // 加载一个用户管理模块
    if (oldUser->getType() == User::Type::Reader) { // 更新读者信息
        // 新读者
        Reader newReader = Reader(
                    oldUser->getId(),
                    isSelectName ? name : "",
                    isSelectAge ? age : -1,
                    isSelectTel ? Telephone{strTel} : Telephone{},
                    Password{},
                    isSelectMaxBorrowNumbers ? maxBorrowNumbers : -1);
        // 执行更新操作
        try {
            manage.updateReader(oldUser->getId(),newReader);
            QMessageBox::information(this,"操作成功","成功更改了读者信息！");
            // 用于主界面更新
            dynamic_cast<Reader*>(oldUser)->update(newReader);
            return accept();
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(this,"System Error","No such reader.");
        }

    } else { // 更新管理员信息
        // 新管理员
        Administrator newAdmin = Administrator(
                    oldUser->getId(),
                    isSelectName ? name : "",
                    isSelectAge ? age : -1,
                    isSelectTel ? Telephone{strTel} : Telephone{});
        // 执行更新操作
        try {
            manage.updateAdmin(oldUser->getId(), newAdmin);
            QMessageBox::information(this,"操作成功","成功更改了管理员信息！");
            // 用于主界面更新
            dynamic_cast<Administrator*>(oldUser)->update(newAdmin);
            return accept();
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(this,"System Error","No such admin.");
        }
    }
}


void updateUserDialog::on_newTelLineEdit_editingFinished()
{
    QString tel = ui->newTelLineEdit->text();
    if (!Telephone::isTel(tel) && !tel.isEmpty()) {
        QMessageBox::warning(this,"输入错误","不是正确的电话号码！");
        ui->newTelLineEdit->setText("");
    }
}
