#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    QPixmap image(":/img/logo.png");
    QPixmap fitpixmap = image.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label->setPixmap(fitpixmap);
    ui->label->setAlignment(Qt::AlignCenter);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
