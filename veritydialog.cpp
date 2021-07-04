#include "veritydialog.h"
#include "ui_veritydialog.h"
#include <QMessageBox>

VerityDialog::VerityDialog(QWidget *parent, User *user, const QString &operation) :
    QDialog(parent),
    ui(new Ui::VerityDialog),
    user(user)
{
    ui->setupUi(this);
    ui->infoLabel->setText("正在尝试" + operation + "。");
    ui->id->setText(QString::number(user->getId()));
    if (user->getType() == User::Type::Admin)
        ui->idLabel->setText("管理员ID：");
    else
        ui->idLabel->setText("读者ID：");
    ui->pwdLineEdit->setFocus();
    QPixmap image(":/img/lock.png");
    QPixmap fitpixmap = image.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->img->setPixmap(fitpixmap);
    ui->img->setAlignment(Qt::AlignCenter);
}

VerityDialog::~VerityDialog()
{
    delete ui;
}

void VerityDialog::on_buttonBox_accepted()
{
    if (ui->pwdLineEdit->text() == user->getPwd().decrypt()) {
        return accept();
    } else {
        QMessageBox::warning(this,"密码错误","输入的密码不正确");
    }
}
