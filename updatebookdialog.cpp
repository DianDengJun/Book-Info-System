#include "updatebookdialog.h"
#include "ui_updatebookdialog.h"
#include <QMessageBox>

updateBookDialog::updateBookDialog(QWidget *parent, Book *oldBook) :
    QDialog(parent),
    ui(new Ui::updateBookDialog),
    oldBook(oldBook)
{
    ui->setupUi(this);
    ui->infoLabel->setText("更新图书" + QString::number(oldBook->getId()) + "的信息");
    ui->oldNameLabel->setText(oldBook->getName());
    ui->oldAuthorLabel->setText(oldBook->getAuthor());
    ui->oldTypeLabel->setText(oldBook->getType());
    ui->oldPressLabel->setText(oldBook->getPress());
    ui->oldPubTimeLabel->setText(oldBook->getPubDate().toString("yyyy-MM-dd"));
    ui->oldPriceLabel->setText(QString::number(oldBook->getPrice()));
    ui->oldNumbersLabel->setText(QString::number(oldBook->getNumbers()));
}

updateBookDialog::~updateBookDialog()
{
    delete ui;
}

void updateBookDialog::on_buttonBox_accepted()
{
    // 参数获取
    QString name = ui->newNameLineEdit->text();
    QString author = ui->newAuthorLineEdit->text();
    QString type = ui->newTypeLineEdit->text();
    QString press = ui->newPressLineEdit->text();
    QDate pubTime = ui->newPubTimeDateEdit->date();
    double price = ui->newPriceDoubleSpinBox->value();
    int numbers = ui->newNumbersSpinBox->value();

    // 参数启用获取
    bool isSelectName = ui->nameCheckBox->isChecked();
    bool isSelectAuthor = ui->authorCheckBox->isChecked();
    bool isSelectType = ui->typeCheckBox->isChecked();
    bool isSelectPress = ui->pressCheckBox->isChecked();
    bool isSelectPubTime = ui->pubTimeCheckBox->isChecked();
    bool isSelectPrice = ui->priceCheckBox->isChecked();
    bool isSelectNumbers = ui->numbersCheckBox->isChecked();

    // 参数为空判断
    if (isSelectName && name.isEmpty()) {
        QMessageBox::warning(this,"输入错误","姓名不得为空");
        return;
    }
    if (isSelectAuthor && author.isEmpty()) {
        QMessageBox::warning(this,"输入错误","作者不得为空");
        return;
    }
    if (isSelectType && type.isEmpty()) {
        QMessageBox::warning(this,"输入错误","分类不得为空");
        return;
    }
    if (isSelectPress && press.isEmpty()) {
        QMessageBox::warning(this,"输入错误","分类不得为空");
        return;
    }
    if (!isSelectName && !isSelectAuthor && !isSelectType && !isSelectPress
            && !isSelectPubTime && !isSelectPrice && !isSelectNumbers) {
        QMessageBox::information(this,"操作提示","未更新任何字段！");
        return reject();
    }

    // 新书
    Book newBook(
                oldBook->getId(),
                isSelectName ? name : "",
                isSelectAuthor ? author : "",
                isSelectType ? type : "",
                isSelectPress ? press : "",
                isSelectPubTime ? pubTime : QDate{0,0,0},
                isSelectPrice ? price : -1,
                isSelectNumbers ? numbers : -1);
    // 执行更新操作
    try {
        manage.updateBookByID(oldBook->getId(), newBook);
        return accept();
    } catch (std::invalid_argument &e) {
        QMessageBox::warning(this,"操作失败","这本书已经不存在了！");
    }
}
