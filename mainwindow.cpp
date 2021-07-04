#include "mainwindow.h"
#include "LoginDialog.h"
#include "QDebug"
#include "aboutdialog.h"
#include "veritydialog.h"
#include "common.h"
#include "ui_mainwindow.h"
#include "updatebookdialog.h"
#include "updateuserdialog.h"
#include "userinfodialog.h"
#include <QAbstractItemView>
#include <QByteArray>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->userShowTableWidget->hideColumn(6); // 永远隐藏密码列
    uiInit();                               // UI初始化
}

MainWindow::~MainWindow() {
    delete ui;
}

// 2
void MainWindow::on_searchAllBooksCheckBox_stateChanged(int arg1) {
    bool status = ui->searchAllBooksCheckBox->isChecked();
    ui->searchBookNameCheckBox->setEnabled(!status);
    ui->searchBookNameLineEdit->setEnabled(!status);
    ui->searchBookAuthorCheckBox->setEnabled(!status);
    ui->searchBookAuthorLineEdit->setEnabled(!status);
    ui->searchBookTypeCheckBox->setEnabled(!status);
    ui->searchBookTypeLineEdit->setEnabled(!status);
    ui->searchBookPressCheckBox->setEnabled(!status);
    ui->searchBookPressLineEdit->setEnabled(!status);
    ui->searchBookPubTimeCheckBox->setEnabled(!status);
    ui->searchBookPubTimeDateEdit->setEnabled(!status);
    ui->searchBookPriceCheckBox->setEnabled(!status);
    ui->searchBookPriceDoubleSpinBox->setEnabled(!status);
    ui->vagueBookCheckBox->setEnabled(!status);
}

// 1
void MainWindow::on_clearInputSearchBookPushButton_clicked() {
    ui->searchBookNameLineEdit->clear();
    ui->searchBookAuthorLineEdit->clear();
    ui->searchBookTypeLineEdit->clear();
    ui->searchBookPressLineEdit->clear();
    ui->searchBookPubTimeDateEdit->setDate(QDate(2000, 1, 1));
    ui->searchBookPriceDoubleSpinBox->setValue(0.00);
}

// 22
void MainWindow::on_clearUserShowPushButton_clicked() {
    ui->userShowTableWidget->clearContents();
    ui->userShowTableWidget->setRowCount(0);
}

// 19
void MainWindow::on_clearInputSearchUserPushButton_clicked() {
    ui->searchUserNameLineEdit->clear();
    ui->searchUserTelLineEdit->clear();
    ui->searchUserMaxBorrowNumbersSpinBox->setValue(0);
    ui->searchUserAgeSpinBox->setValue(0);
}

// 20
void MainWindow::on_searchAllUsersCheckBox_stateChanged(int arg1) {
    bool status = ui->searchAllUsersCheckBox->isChecked();
    ui->searchUserNameCheckBox->setEnabled(!status);
    ui->searchUserNameLineEdit->setEnabled(!status);
    ui->searchUserTelCheckBox->setEnabled(!status);
    ui->searchUserTelLineEdit->setEnabled(!status);
    ui->searchUserMaxBorrowNumbersCheckBox->setEnabled(!status);
    ui->searchUserMaxBorrowNumbersSpinBox->setEnabled(!status);
    ui->searchUserAgeCheckBox->setEnabled(!status);
    ui->searchUserAgeSpinBox->setEnabled(!status);
    ui->searchUserTypeCheckBox->setEnabled(!status);
    ui->searchUserTypeComboBox->setEnabled(!status);
    ui->vagueUserCheckBox->setEnabled(!status);
}

// 21
void MainWindow::on_searchUserPushButton_clicked() {
    ui->userShowTableWidget->clearContents();
    // 参数获取
    QString name = ui->searchUserNameLineEdit->text();
    QString tel = ui->searchUserTelLineEdit->text();
    int maxBorrowNumbers = ui->searchUserMaxBorrowNumbersSpinBox->value();
    int age = ui->searchUserAgeSpinBox->value();
    User::Type type;
    switch (ui->searchUserTypeComboBox->currentIndex()) {
        case 0:
            type = User::Type::Reader;
            break;
        case 1:
            type = User::Type::Admin;
            break;
        default:
            type = User::Type::Undefined;
            break;
    }
    // 参数启用判断
    bool isSelectName = ui->searchUserNameCheckBox->isChecked();
    bool isSelectTel = ui->searchUserTelCheckBox->isChecked();
    bool isSelectMax = ui->searchUserMaxBorrowNumbersCheckBox->isChecked();
    bool isSelectAge = ui->searchUserAgeCheckBox->isChecked();
    bool isSelectType = ui->searchUserTypeCheckBox->isChecked();
    bool isSearchAllUsers = ui->searchAllUsersCheckBox->isChecked();
    bool isVagueSearch = ui->vagueUserCheckBox->isChecked();

    if (isSelectTel && !Telephone::isTel(tel) && !tel.isEmpty()) {
        QMessageBox::warning(this,"输入错误","不是正确的电话号码！");
        return;
    }

    if (tel.isEmpty()) {
        tel = "18000000000";
    }

    // 清空表
    QTableWidget *userShowTable = ui->userShowTableWidget;
    userShowTable->clearContents();

    // 要展示的结果
    UserTable::ReaderTable showReaders{};
    UserTable::AdminTable showAdmins{};

    // 完成搜寻工作
    if (isSearchAllUsers) { // 显示所有人
        showReaders = manage.getAllReaders();
        showAdmins = manage.getAllAdmins();
    } else {
        // 实例化读者样例
        Reader readerExample(
                -1,
                isSelectName ? name : "",
                isSelectAge ? age : -1,
                isSelectTel ? Telephone{tel} : Telephone{},
                Password{},
                isSelectMax ? maxBorrowNumbers : -1);
        // 实例化管理员样例
        Administrator adminExample(
                -1,
                isSelectName ? name : "",
                isSelectAge ? age : -1,
                isSelectTel ? Telephone{tel} : Telephone{},
                Password{});
        if (isVagueSearch && isSelectType) {  // 模糊搜索指定类别用户
            if (type == User::Type::Reader) { // 模糊搜索读者
                // 匹配读者样例
                showReaders = manage.vagueSelectReaders(readerExample);
            } else { // 模糊搜索管理员
                // 匹配管理员样例
                showAdmins = manage.vagueSelectAdmins(adminExample);
            }
        } else if (isVagueSearch && !isSelectType) { // 模糊搜索全体
            // 匹配读者样例
            showReaders = manage.vagueSelectReaders(readerExample);
            // 匹配管理员样例
            showAdmins = manage.vagueSelectAdmins(adminExample);
        } else if (!isVagueSearch && isSelectType) { // 精确搜索指定类别用户
            if (type == User::Type::Reader) {        // 精确搜索读者
                // 匹配读者样例
                showReaders = manage.selectReaders(readerExample);
            } else { // 精确搜索管理员
                // 匹配管理员样例
                showAdmins = manage.selectAdmins(adminExample);
            }
        } else if (!isVagueSearch && !isSelectType) { // 精确搜索全体
            // 匹配读者样例
            showReaders = manage.selectReaders(readerExample);
            // 匹配管理员样例
            showAdmins = manage.selectAdmins(adminExample);
        }
    }

    // 展示到表中
    userShowTable->setRowCount(showReaders.size() + showAdmins.size());
    if (showReaders.size() + showAdmins.size() == 0) {
        QMessageBox::information(this, "操作提示", "没有查询到相关内容！");
        return;
    }
    for (size_t i = 0; i < showReaders.size(); i++) {
        auto reader = showReaders[i];
        userShowTable->setItem(i, 0, new QTableWidgetItem(QString::number(reader.getId())));
        userShowTable->setItem(i, 1, new QTableWidgetItem(reader.getName()));
        userShowTable->setItem(i, 2, new QTableWidgetItem(reader.getTel().toString()));
        userShowTable->setItem(i, 3, new QTableWidgetItem(QString::number(reader.getAge())));
        userShowTable->setItem(i, 4, new QTableWidgetItem(QString::number(reader.getMaxBorrowNumbers())));
        userShowTable->setItem(i, 5, new QTableWidgetItem("读者"));
        userShowTable->setItem(i, 6, new QTableWidgetItem(reader.getPwd().decrypt()));
    }
    for (size_t i = 0; i < showAdmins.size(); i++) {
        auto admin = showAdmins[i];
        userShowTable->setItem(i + showReaders.size(), 0, new QTableWidgetItem(QString::number(admin.getId())));
        userShowTable->setItem(i + showReaders.size(), 1, new QTableWidgetItem(admin.getName()));
        userShowTable->setItem(i + showReaders.size(), 2, new QTableWidgetItem(admin.getTel().toString()));
        userShowTable->setItem(i + showReaders.size(), 3, new QTableWidgetItem(QString::number(admin.getAge())));
        userShowTable->setItem(i + showReaders.size(), 4, new QTableWidgetItem(""));
        userShowTable->setItem(i + showReaders.size(), 5, new QTableWidgetItem("管理员"));
        userShowTable->setItem(i + showReaders.size(), 6, new QTableWidgetItem(admin.getPwd().decrypt()));
    }
}

// 24
void MainWindow::on_deleteUserPushButton_clicked() {
    // 是否有选中行
    if (!ui->userShowTableWidget->selectedItems().count())
        return;

    // 获取管理员许可
    VerityDialog dia(this, manage.getCurrentAdmin(), "删除用户操作");
    if (dia.exec() == QDialog::Rejected) {
        return;
    }

    // 获取选中行的信息
    int row = ui->userShowTableWidget->currentRow();
    int id = ui->userShowTableWidget->item(row, 0)->text().toInt();

    if (ui->userShowTableWidget->item(row, 5)->text() == "读者") {
        try {
            manage.removeReader(id);
            QMessageBox::information(this, "操作成功", QString{"成功删除了读者"} + QString::number(id));
            ui->userShowTableWidget->removeRow(row);
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(this, "操作失败", e.what());
        }
    } else {
        try {
            manage.removeAdmin(id);
            QMessageBox::information(this, "操作成功", QString{"成功删除了管理员"} + QString::number(id));
            ui->userShowTableWidget->removeRow(row);
        } catch (std::invalid_argument &e) {
            QMessageBox::warning(this, "操作失败", e.what());
        }
    }
}

// 25
void MainWindow::on_modifyUserPushButton_clicked() {
    // 是否有选中行
    if (!ui->userShowTableWidget->selectedItems().count())
        return;

    // 获取管理员许可
    VerityDialog dia(this, manage.getCurrentAdmin(), "修改用户操作");
    if (dia.exec() == QDialog::Rejected) {
        return;
    }

    // 获取选中行的信息
    int row = ui->userShowTableWidget->currentRow();
    int id = ui->userShowTableWidget->item(row, 0)->text().toInt();
    QString name = ui->userShowTableWidget->item(row, 1)->text();
    Telephone tel{ui->userShowTableWidget->item(row, 2)->text()};
    int age = ui->userShowTableWidget->item(row, 3)->text().toInt();
    int maxBorrowNumbers = ui->userShowTableWidget->item(row, 4)->text().toInt();
    if (ui->userShowTableWidget->item(row, 5)->text() == "读者") {
        Reader toModify = Reader{id, name, age, tel, Password{}, maxBorrowNumbers};
        updateUserDialog dia(this, &toModify);
        if (dia.exec() == QDialog::Accepted) {
            // 更新了数据，需要同步到表中
            ui->userShowTableWidget->setItem(row, 1, new QTableWidgetItem(toModify.getName()));
            ui->userShowTableWidget->setItem(row, 2, new QTableWidgetItem(toModify.getTel().toString()));
            ui->userShowTableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(toModify.getAge())));
            ui->userShowTableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(toModify.getMaxBorrowNumbers())));
        }
    } else {
        // 不能在此界面中修改自己
        if (id == manage.getCurrentAdmin()->getId()) {
            QMessageBox::warning(this, "操作提示", "不能在此界面中修改自己!");
            return;
        }
        Administrator toModify = Administrator{id, name, age, tel};
        updateUserDialog dia(this, &toModify);
        if (dia.exec() == QDialog::Accepted) {
            // 更新了数据，需要同步到表中
            ui->userShowTableWidget->setItem(row, 1, new QTableWidgetItem(toModify.getName()));
            ui->userShowTableWidget->setItem(row, 2, new QTableWidgetItem(toModify.getTel().toString()));
            ui->userShowTableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(toModify.getAge())));
        }
    }
}

// 27
void MainWindow::on_clearInputNewUserPushButton_clicked() {
    ui->newUserNameLineEdit->clear();
    ui->newUserTelLineEdit->clear();
    ui->newUserPwdLineEdit->clear();
    ui->newUserMaxBorrowNumbersSpinBox->setValue(0);
    ui->newUserAgeSpinBox->setValue(0);
}

void MainWindow::on_newUserIsAdminCheckBox_stateChanged(int arg1) {
    bool status = ui->newUserIsAdminCheckBox->isChecked();
    ui->newUserMaxBorrowNumbersLabel->setEnabled(!status);
    ui->newUserMaxBorrowNumbersSpinBox->setEnabled(!status);
}

// 29
void MainWindow::on_doNewUserPushButton_clicked() {
    // 参数获取
    QString name = ui->newUserNameLineEdit->text();
    QString strTel = ui->newUserTelLineEdit->text();
    QString strPwd = ui->newUserPwdLineEdit->text();
    int maxBorrowNumbers = ui->newUserMaxBorrowNumbersSpinBox->value();
    int age = ui->newUserAgeSpinBox->value();
    bool isAdmin = ui->newUserIsAdminCheckBox->isChecked();

    // 参数为空检测
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "姓名不能为空!");
        return;
    }
    if (strTel.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "电话号码不能为空!");
        return;
    }
    if (strPwd.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "密码不能为空!");
        return;
    }


    // 执行用户创建
    if (!isAdmin) {
        int id = manage.addNewReader(name, age, Telephone{strTel}, Password{strPwd}, maxBorrowNumbers);
        QMessageBox::information(this, "操作成功", QString{"该读者的ID是"} + QString::number(id));
        // 表增加一行
        int rows = ui->newUserLogTableWidget->rowCount();
        ui->newUserLogTableWidget->insertRow(rows);
        // 添加数据
        ui->newUserLogTableWidget->setItem(rows, 0, new QTableWidgetItem(QString::number(id)));
        ui->newUserLogTableWidget->setItem(rows, 1, new QTableWidgetItem(name));
        ui->newUserLogTableWidget->setItem(rows, 2, new QTableWidgetItem(strTel));
        ui->newUserLogTableWidget->setItem(rows, 3, new QTableWidgetItem(QString::number(age)));
        ui->newUserLogTableWidget->setItem(rows, 4, new QTableWidgetItem(QString::number(maxBorrowNumbers)));
        ui->newUserLogTableWidget->setItem(rows, 5, new QTableWidgetItem("读者"));
    } else {
        int id = manage.addNewAdmin(name, age, Telephone{strTel}, Password{strPwd});
        QMessageBox::information(this, "操作成功", QString{"该管理员的ID是"} + QString::number(id));
        // 表增加一行
        int rows = ui->newUserLogTableWidget->rowCount();
        ui->newUserLogTableWidget->insertRow(rows);
        // 添加数据
        ui->newUserLogTableWidget->setItem(rows, 0, new QTableWidgetItem(QString::number(id)));
        ui->newUserLogTableWidget->setItem(rows, 1, new QTableWidgetItem(name));
        ui->newUserLogTableWidget->setItem(rows, 2, new QTableWidgetItem(strTel));
        ui->newUserLogTableWidget->setItem(rows, 3, new QTableWidgetItem(QString::number(age)));
        ui->newUserLogTableWidget->setItem(rows, 4, new QTableWidgetItem(""));
        ui->newUserLogTableWidget->setItem(rows, 5, new QTableWidgetItem("管理员"));
    }
}

// 28
void MainWindow::on_newUserTelLineEdit_editingFinished() {
    QString tel = ui->newUserTelLineEdit->text();
    if (!Telephone::isTel(tel) && !tel.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "不是正确的电话号码！");
        ui->newUserTelLineEdit->clear();
    }
}

// 23
void MainWindow::on_exportResultUserPushButton_clicked() {
    // 检测表是否为空
    if (ui->userShowTableWidget->rowCount() == 0) {
        QMessageBox::information(this, "操作提示", "表中没有数据");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName( this, tr("保存用户数据配置文件"), "/", tr("数据交换文件(*json)"));

    if (fileName.isEmpty())
        return;

    // 构建JSON数组 - readers
    QJsonArray jsonReaders;
    // 构建JSON数组 - admins
    QJsonArray jsonAdmins;

    for (int i = 0; i < ui->userShowTableWidget->rowCount(); i++) {
        // 获取用户类型
        QString type = ui->userShowTableWidget->item(i, 5)->text();
        if (type == "读者") {
            // 构建单个JSON对象 - reader
            QJsonObject jsonReader;
            jsonReader.insert("ID", ui->userShowTableWidget->item(i, 0)->text().toInt());
            jsonReader.insert("name", ui->userShowTableWidget->item(i, 1)->text());
            jsonReader.insert("telephone", ui->userShowTableWidget->item(i, 2)->text());
            jsonReader.insert("age", ui->userShowTableWidget->item(i, 3)->text().toInt());
            jsonReader.insert("maxBorrowNumbers", ui->userShowTableWidget->item(i, 4)->text().toInt());
            QString pwdResult = Password::x_or(ui->userShowTableWidget->item(i, 6)->text());
            jsonReader.insert("password", pwdResult);
            // 追加到readers中
            jsonReaders.append(jsonReader);
        } else {
            // 构建单个JSON对象 - admin
            QJsonObject jsonAdmin;
            jsonAdmin.insert("ID", ui->userShowTableWidget->item(i, 0)->text().toInt());
            jsonAdmin.insert("name", ui->userShowTableWidget->item(i, 1)->text());
            jsonAdmin.insert("telephone", ui->userShowTableWidget->item(i, 2)->text());
            jsonAdmin.insert("age", ui->userShowTableWidget->item(i, 3)->text().toInt());
            QString pwdResult = Password::x_or(ui->userShowTableWidget->item(i, 6)->text());
            jsonAdmin.insert("password", pwdResult);
            // 追加到admins中
            jsonAdmins.append(jsonAdmin);
        }
    }
    // 构建JSON对象 - 根
    QJsonObject rootObj;
    rootObj.insert("readers", jsonReaders);
    rootObj.insert("admins", jsonAdmins);

    // 构建json文档
    QJsonDocument jsonDoc;
    jsonDoc.setObject(rootObj);

    // 将json文档字符串化
    QString jsonDocStr{jsonDoc.toJson()};

    // 打开指定文件
    QFile file{fileName + ".json"};
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::warning(this, "System Error", "Can not open such file.");
        return;
    }
    // 从文件构造输出流
    QTextStream out(&file);
    // 输出到输出流中
    out << jsonDocStr;
    // 关闭文件
    file.close();
    // 提示操作成功
    QMessageBox::information(this, "操作成功", "已导出到文件" + fileName + ".json");
}

// 29
void MainWindow::on_importNewUsersPushButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName( this, tr("打开用户数据配置文件"), "/", tr("数据交换文件(*json)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(nullptr, "System Error", "Can not load such file!");
        return;
    }

    try {
        auto users = manage.addUsers(file);
        auto all = users.getAllUsers();
        for (auto user: all) {
            int rows = ui->newUserLogTableWidget->rowCount();
            ui->newUserLogTableWidget->insertRow(rows);
            ui->newUserLogTableWidget->setItem(rows, 0, new QTableWidgetItem(QString::number(user->getId())));
            ui->newUserLogTableWidget->setItem(rows, 1, new QTableWidgetItem(user->getName()));
            ui->newUserLogTableWidget->setItem(rows, 2, new QTableWidgetItem(user->getTel().toString()));
            ui->newUserLogTableWidget->setItem(rows, 3, new QTableWidgetItem(QString::number(user->getAge())));
            if (user->getType() == User::Type::Reader) {
                auto reader = dynamic_cast<const Reader*>(user);
                ui->newUserLogTableWidget->setItem(rows, 4, new QTableWidgetItem(QString::number(reader->getMaxBorrowNumbers())));
                ui->newUserLogTableWidget->setItem(rows, 5, new QTableWidgetItem("读者"));
            } else {
                ui->newUserLogTableWidget->setItem(rows, 4, new QTableWidgetItem(""));
                ui->newUserLogTableWidget->setItem(rows, 5, new QTableWidgetItem("管理员"));
            }
        }
        file.close();
        QMessageBox::information(this, "导入用户成功", "已成功导入用户，但请注意重分配了ID。");
    } catch (std::invalid_argument &e) {
        QMessageBox::warning(this,"操作失败","JSON文件解析异常！");
    }
}

// 3
void MainWindow::on_quearyBookPushButton_clicked() {
    // 表别名
    auto &bookShowTable = *(ui->bookShowTableWidget);
    // 参数获取
    QString name = ui->searchBookNameLineEdit->text();
    QString author = ui->searchBookAuthorLineEdit->text();
    QString type = ui->searchBookTypeLineEdit->text();
    QString press = ui->searchBookPressLineEdit->text();
    QDate pubDate = ui->searchBookPubTimeDateEdit->date();
    double price = ui->searchBookPriceDoubleSpinBox->value();
    // 参数启用判断
    bool isSelectBookName = ui->searchBookNameCheckBox->isChecked();
    bool isSelectBookAuthor = ui->searchBookAuthorCheckBox->isChecked();
    bool isSelectBookType = ui->searchBookTypeCheckBox->isChecked();
    bool isSelectBookPress = ui->searchBookPressCheckBox->isChecked();
    bool isSelectBookPubTime = ui->searchBookPubTimeCheckBox->isChecked();
    bool isSelectBookPrice = ui->searchBookPriceCheckBox->isChecked();
    bool isSelectSearchAllBook = ui->searchAllBooksCheckBox->isChecked();
    bool isSelectVagueSerch = ui->vagueBookCheckBox->isChecked();

    // 先清空表
    bookShowTable.clearContents();

    // 要展示的结果
    BookTable::Table showBooks{};
    std::vector<int> borrowedNumbers;

    // 完成搜寻工作
    if (isSelectSearchAllBook) { // 显示所有书
        showBooks = manage.getAllBooks();
    } else {
        // 实例化图书样例
        Book example(
                -1,
                isSelectBookName ? name : "",
                isSelectBookAuthor ? author : "",
                isSelectBookType ? type : "",
                isSelectBookPress ? press : "",
                isSelectBookPubTime ? pubDate : QDate{0, 0, 0},
                isSelectBookPrice ? price : -1);
        if (isSelectVagueSerch) { // 模糊搜索
            showBooks = manage.vagueSelectBooks(example);
        } else { // 精确搜索
            showBooks = manage.selectBooks(example);
        }
    }

    // 查询图书已借数量
    for (const auto &book : showBooks) {
        borrowedNumbers.push_back(manage.getBorrowingNumbers(book.getId()));
    }

    // 展示到表中
    bookShowTable.setRowCount(showBooks.size());
    if (showBooks.empty()) {
        QMessageBox::information(this, "操作提示", "没有查询到相关内容！");
    } else {
        for (size_t i = 0; i < showBooks.size(); i++) {
            auto book = showBooks.at(i);
            bookShowTable.setItem(i, 0, new QTableWidgetItem(QString::number(book.getId())));
            bookShowTable.setItem(i, 1, new QTableWidgetItem(book.getName()));
            bookShowTable.setItem(i, 2, new QTableWidgetItem(book.getAuthor()));
            bookShowTable.setItem(i, 3, new QTableWidgetItem(book.getType()));
            bookShowTable.setItem(i, 4, new QTableWidgetItem(book.getPress()));
            bookShowTable.setItem(i, 5, new QTableWidgetItem(book.getPubDate().toString("yyyy-MM-dd")));
            bookShowTable.setItem(i, 6, new QTableWidgetItem(QString::number(book.getPrice())));
            bookShowTable.setItem(i, 7, new QTableWidgetItem(QString::number(book.getNumbers())));
            bookShowTable.setItem(i, 8, new QTableWidgetItem(QString::number(borrowedNumbers[i])));
        }
    }
}

// 4
void MainWindow::on_clearBookShowPushButton_clicked() {
    ui->bookShowTableWidget->clearContents();
    ui->bookShowTableWidget->setRowCount(0);
}

// 7
void MainWindow::on_deleteBookPushButton_clicked() {
    // 引用图书展示表
    auto &bookShowTable = *(ui->bookShowTableWidget);

    // 是否有选中行
    if (!bookShowTable.selectedItems().count())
        return;

    // 获取管理员许可
    VerityDialog dia(this, manage.getCurrentAdmin(), "删除图书操作");
    if (dia.exec() == QDialog::Rejected) {
        return;
    }

    // 获取选中行的信息
    int row = bookShowTable.currentRow();
    int id = bookShowTable.item(row, 0)->text().toInt();


    // 删除对应书
    try {
        manage.removeBook(id);
        QMessageBox::information(this, "操作成功", "成功删除了书" + QString::number(id));
        bookShowTable.removeRow(row);
    } catch (std::invalid_argument &e) {
        QMessageBox::warning(this, "操作失败", e.what());
    }
}

// 8
void MainWindow::on_modifyBookPushButton_clicked() {
    // 引用图书展示表
    auto &bookShowTable = *(ui->bookShowTableWidget);

    // 是否有选中行
    if (!bookShowTable.selectedItems().count())
        return;

    // 获取管理员许可
    VerityDialog dia(this, manage.getCurrentAdmin(), "修改图书操作");
    if (dia.exec() == QDialog::Rejected) {
        return;
    }

    // 获取选中行的信息
    int row = bookShowTable.currentRow();
    int id = bookShowTable.item(row, 0)->text().toInt();

    // 更改图书信息
    try {
        auto &toModify = manage.selectBookByID(id);
        updateBookDialog dia{this, &toModify};
        if (dia.exec() == QDialog::Accepted) {
            // 更新了数据，需要同步到表中
            bookShowTable.setItem(row, 1, new QTableWidgetItem(toModify.getName()));
            bookShowTable.setItem(row, 2, new QTableWidgetItem(toModify.getAuthor()));
            bookShowTable.setItem(row, 3, new QTableWidgetItem(toModify.getType()));
            bookShowTable.setItem(row, 4, new QTableWidgetItem(toModify.getPress()));
            bookShowTable.setItem(row, 5, new QTableWidgetItem(toModify.getPubDate().toString("yyyy-MM-dd")));
            bookShowTable.setItem(row, 6, new QTableWidgetItem(QString::number(toModify.getPrice())));
            bookShowTable.setItem(row, 7, new QTableWidgetItem(QString::number(toModify.getNumbers())));
        }
    } catch (std::invalid_argument &e) {
        QMessageBox::warning(this, "操作失败", "抱歉，没有这本书！请先重新查询！");
    }
}

// 12
void MainWindow::on_doNewBookPushButton_clicked() {
    // 参数获取
    QString name = ui->newBookNameLineEdit->text();
    QString author = ui->newBookAuthorLineEdit->text();
    QString type = ui->newBookTypeLineEdit->text();
    QString press = ui->newBookPressLineEdit->text();
    QDate pubTime = ui->newBookPubTimeDateEdit->date();
    double price = ui->newBookPriceDoubleSpinBox->value();
    int numbers = ui->newBookNumbersSpinBox->value();

    // 参数为空检测
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入书名！");
        return;
    }
    if (author.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入作者！");
        return;
    }
    if (type.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入分类！");
        return;
    }
    if (press.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入出版社！");
        return;
    }


    // 执行添加书籍操作
    int id = manage.addNewBook(name, author, type, press, pubTime, price, numbers);
    QMessageBox::information(this, "操作成功", QString{"系统为该书分配了ID:"} + QString::number(id));

    // 表增加一行
    int rows = ui->newBookLogTableWidget->rowCount();
    ui->newBookLogTableWidget->insertRow(rows);
    // 添加数据
    ui->newBookLogTableWidget->setItem(rows, 0, new QTableWidgetItem(QString::number(id)));
    ui->newBookLogTableWidget->setItem(rows, 1, new QTableWidgetItem(name));
    ui->newBookLogTableWidget->setItem(rows, 2, new QTableWidgetItem(author));
    ui->newBookLogTableWidget->setItem(rows, 3, new QTableWidgetItem(type));
    ui->newBookLogTableWidget->setItem(rows, 4, new QTableWidgetItem(press));
    ui->newBookLogTableWidget->setItem(rows, 5, new QTableWidgetItem(pubTime.toString("yyyy-MM-dd")));
    ui->newBookLogTableWidget->setItem(rows, 6, new QTableWidgetItem(QString::number(price)));
    ui->newBookLogTableWidget->setItem(rows, 7, new QTableWidgetItem(QString::number(numbers)));
}

// 11
void MainWindow::on_clearInputNewBookPushButton_clicked() {
    ui->newBookNameLineEdit->clear();
    ui->newBookAuthorLineEdit->clear();
    ui->newBookTypeLineEdit->clear();
    ui->newBookPressLineEdit->clear();
    ui->newBookPubTimeDateEdit->setDate(QDate{2000, 1, 1});
    ui->newBookPriceDoubleSpinBox->setValue(0.00);
    ui->newBookNumbersSpinBox->setValue(0);
}

// 5
void MainWindow::on_exportResultBookPushButton_clicked() {
    // 引用表
    auto &table = *(ui->bookShowTableWidget);

    // 检测表是否为空
    if (table.rowCount() == 0) {
        QMessageBox::information(this, "操作提示", "表中没有数据");
        return;
    }

    // 打开一个文件选择窗口
    QString fileName = QFileDialog::getSaveFileName( this, tr("保存图书信息配置文件"), "/", tr("数据交换文件(*json)"));

    // 未获取到文件名
    if (fileName.isEmpty())
        return;

    // 构建JSON数组 - books
    QJsonArray jsonBooks;

    for (int i = 0; i < table.rowCount(); i++) {
        // 构建单个JSON对象 - book
        QJsonObject jsonBook;
        jsonBook.insert("ID", table.item(i, 0)->text().toInt());
        jsonBook.insert("name", table.item(i, 1)->text());
        jsonBook.insert("author", table.item(i, 2)->text());
        jsonBook.insert("type", table.item(i, 3)->text());
        jsonBook.insert("press", table.item(i, 4)->text());
        jsonBook.insert("publication time", table.item(i, 5)->text());
        jsonBook.insert("price", table.item(i, 6)->text().toDouble());
        jsonBook.insert("numbers", table.item(i, 7)->text().toInt());

        // 追加到books中
        jsonBooks.append(jsonBook);
    }
    // 构建json根对象
    QJsonObject jsonRoot;
    jsonRoot.insert("books", jsonBooks);

    // 构建json文档
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonRoot);

    // 将json文档字符串化
    QString jsonDocStr{jsonDoc.toJson()};

    // 打开指定文件
    QFile file{fileName + ".json"};
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::warning(this, "System Error", "Can not open such file.");
        return;
    }
    // 从文件构造输出流
    QTextStream out(&file);
    // 输出到输出流中
    out << jsonDocStr;
    // 关闭文件
    file.close();
    // 提示操作成功
    QMessageBox::information(this, "操作成功", "已导出到文件" + fileName + ".json");
}

// 13
void MainWindow::on_importNewBooksPushButton_clicked() {
    // 打开一个文件选择窗口
    QString fileName = QFileDialog::getOpenFileName( this, tr("打开图书信息配置文件"), "/", tr("数据交换文件(*json)"));

    // 未获取到文件名
    if (fileName.isEmpty())
        return;

    // 实例化文件
    QFile file(fileName);

    // 打开文件
    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::warning(this, "System Error", "Can not load such file!");
        return;
    }

    try {
        auto books = manage.addBooks(file);
        // 展示到表中
        for (const auto& book: books) {
            int rows = ui->newUserLogTableWidget->rowCount();
            ui->newBookLogTableWidget->insertRow(rows);
            ui->newBookLogTableWidget->setItem(rows, 0, new QTableWidgetItem(QString::number(book.getId())));
            ui->newBookLogTableWidget->setItem(rows, 1, new QTableWidgetItem(book.getName()));
            ui->newBookLogTableWidget->setItem(rows, 2, new QTableWidgetItem(book.getAuthor()));
            ui->newBookLogTableWidget->setItem(rows, 3, new QTableWidgetItem(book.getType()));
            ui->newBookLogTableWidget->setItem(rows, 4, new QTableWidgetItem(book.getPress()));
            ui->newBookLogTableWidget->setItem(rows, 5, new QTableWidgetItem(book.getPubDate().toString("yyyy-MM-dd")));
            ui->newBookLogTableWidget->setItem(rows, 6, new QTableWidgetItem(QString::number(book.getPrice())));
            ui->newBookLogTableWidget->setItem(rows, 7, new QTableWidgetItem(QString::number(book.getNumbers())));
        }
        // 关闭文件
        file.close();
        QMessageBox::information(this, "导入图书成功", "已成功导入图书，但请注意重分配了ID。");
    } catch (std::invalid_argument &e) {
        QMessageBox::warning(this, "操作失败", "JSON文件解析异常！");
    }
}

// 14
void MainWindow::on_clearInputSearchRecordPushButton_clicked() {
    ui->searchRecordReaderIDSpinBox->setValue(0);
    ui->searchRecordBookIDSpinBox->setValue(0);
    ui->searchRecordBorrowDateDateEdit->setDate({2000, 1, 1});
    ui->searchRecordReturnDateDateEdit->setDate({2000, 1, 1});
}

// 15
void MainWindow::on_searchAllRecordsCheckBox_stateChanged(int arg1) {
    bool status = ui->searchAllRecordsCheckBox->isChecked();
    ui->searchRecordReaderIDCheckBox->setEnabled(!status);
    ui->searchRecordReaderIDSpinBox->setEnabled(!status);
    ui->searchRecordBookIDCheckBox->setEnabled(!status);
    ui->searchRecordBookIDSpinBox->setEnabled(!status);
    ui->searchRecordBorrowDateCheckBox->setEnabled(!status);
    ui->searchRecordBorrowDateDateEdit->setEnabled(!status);
    ui->searchRecordReturnDateCheckBox->setEnabled(!status);
    ui->searchRecordReturnDateDateEdit->setEnabled(!status);
    ui->searchRecordBookStatusCheckBox->setEnabled(!status);
    ui->searchRecordBookStatusComboBox->setEnabled(!status);
}

// 16
void MainWindow::on_searchRecordPushButton_clicked() {
    // 表别名
    auto &recordShowTable = *(ui->recordShowTableWidget);
    // 参数获取
    int readerID = ui->searchRecordReaderIDSpinBox->value();
    int bookID = ui->searchRecordBookIDSpinBox->value();
    QDate borrowDate = ui->searchRecordBorrowDateDateEdit->date();
    QDate returnDate = ui->searchRecordReturnDateDateEdit->date();
    BorrowRecord::Status status;
    switch (ui->searchRecordBookStatusComboBox->currentIndex()) {
        case 0:
            status = BorrowRecord::Status::Borrowing;
            break;
        case 1:
            status = BorrowRecord::Status::Returned;
            break;
    }

    // 参数启用判断
    bool isSelectReaderID = ui->searchRecordReaderIDCheckBox->isChecked();
    bool isSelectBookID = ui->searchRecordBookIDCheckBox->isChecked();
    bool isSelectBorrowDate = ui->searchRecordBorrowDateCheckBox->isChecked();
    bool isSelectReturnDate = ui->searchRecordReturnDateCheckBox->isChecked();
    bool isSelectBookStatus = ui->searchRecordBookStatusCheckBox->isChecked();
    bool isSelectSearchAllRecords = ui->searchAllRecordsCheckBox->isChecked();
    bool isSelectVagueSerch = ui->vagueRecordCheckBox->isChecked();

    // 先清空表
    recordShowTable.clearContents();

    // 要展示的结果
    BorrowTable::Table showBorrows{};

    // 完成搜寻工作
    if (isSelectSearchAllRecords) { // 显示所有记录
        showBorrows = manage.getAllRecords();
    } else {
        // 实例化记录样例
        BorrowRecord example(
                isSelectReaderID ? readerID : -1,
                isSelectBookID ? bookID : -1,
                isSelectBorrowDate ? borrowDate : QDate{0, 0, 0},
                isSelectReturnDate ? returnDate : QDate{0, 0, 0},
                isSelectBookStatus ? status : BorrowRecord::Status::Undefined);
        if (isSelectVagueSerch) { // 模糊搜索
            showBorrows = manage.vagueSelectRecords(example);
        } else { // 精确搜索
            showBorrows = manage.selectRecords(example);
        }
    }

    // 展示到表中
    recordShowTable.setRowCount(showBorrows.size());
    if (showBorrows.empty()) {
        QMessageBox::information(this, "操作提示", "没有查询到相关内容！");
    } else {
        for (size_t i = 0; i < showBorrows.size(); i++) {
            auto record = showBorrows.at(i);
            recordShowTable.setItem(i, 0, new QTableWidgetItem(QString::number(record.getReaderId())));
            recordShowTable.setItem(i, 1, new QTableWidgetItem(QString::number(record.getBookId())));
            recordShowTable.setItem(i, 2, new QTableWidgetItem(record.getBorrowDate().toString("yyyy-MM-dd")));
            recordShowTable.setItem(i, 3, new QTableWidgetItem(record.getReturnDate().toString("yyyy-MM-dd")));
            if (record.getStatus() == BorrowRecord::Status::Borrowing) {
                recordShowTable.setItem(i, 4, new QTableWidgetItem("借阅中"));
            } else
                recordShowTable.setItem(i, 4, new QTableWidgetItem("已归还"));
        }
    }
}

// 17
void MainWindow::on_clearRecordShowPushButton_clicked() {
    ui->recordShowTableWidget->setRowCount(0);
    ui->recordShowTableWidget->clearContents();
}

// 9
void MainWindow::on_updateBorrowPushButton_clicked() {
    BorrowTable::Table records;
    // 获取与该用户有关的所有记录
    if (ui->showBorrowingCheckBox->isChecked()) {
        records = manage.getBorrowingRecords();
    } else
        records = manage.getAllRecordsByCurrentReader();

    // 表别名
    auto &recordShowTable = *(ui->myBorrowTableWidget);

    // 展示到表中
    recordShowTable.setRowCount(records.size());
    for (size_t i = 0; i < records.size(); i++) {
        auto record = records.at(i);
        recordShowTable.setItem(i, 0, new QTableWidgetItem(QString::number(record.getBookId())));
        recordShowTable.setItem(i, 1, new QTableWidgetItem(record.getBorrowDate().toString("yyyy-MM-dd")));
        recordShowTable.setItem(i, 2, new QTableWidgetItem(record.getReturnDate().toString("yyyy-MM-dd")));
        if (record.getStatus() == BorrowRecord::Status::Borrowing) {
            recordShowTable.setItem(i, 3, new QTableWidgetItem("借阅中"));
        } else
            recordShowTable.setItem(i, 3, new QTableWidgetItem("已归还"));
    }
}

// 6
void MainWindow::on_borrowBookPushButton_clicked() {
    // 引用图书展示表
    auto &bookShowTable = *(ui->bookShowTableWidget);

    // 是否有选中行
    if (!bookShowTable.selectedItems().count())
        return;

    // 获取选中行的信息
    int row = bookShowTable.currentRow();
    int id = bookShowTable.item(row, 0)->text().toInt();

    // 借书
    try {
        manage.borrowBook(id);
        QMessageBox::information(this, "操作成功", "借书成功！记得按期归还");
        auto update = manage.selectBookByID(id);
        bookShowTable.setItem(row, 7, new QTableWidgetItem(QString::number(update.getNumbers())));
        bookShowTable.setItem(row, 8, new QTableWidgetItem(QString::number(manage.getBorrowingNumbers(id))));
    } catch (std::invalid_argument &e) {
        QMessageBox::warning(this, "操作失败", e.what());
    }
}

void MainWindow::on_actionExitLogin_triggered() {
    // 恢复UI
    uiRestore();
    // 暂时禁用菜单项
    ui->menuAccount->setEnabled(false);
    this->hide();
    emit loginShow();
}

void MainWindow::on_actionExit_triggered() {
    emit quit();
}

void MainWindow::receiveLogin() {
    this->show();
    // 启用菜单项
    ui->menuAccount->setEnabled(true);
    uiAuto();
}

void MainWindow::receiveLogout() {
    ui->actionExitLogin->triggered();
}

void MainWindow::uiAuto() {
    switch (manage.userMode()) {
        case Manage::UserType::Undefined: {
            ui->welcomeLabel->setText(tr("您好，匿名用户"));
            ui->tabWidget->removeTab(5);
            ui->tabWidget->removeTab(4);
            ui->tabWidget->removeTab(3);
            ui->tabWidget->removeTab(2);
            ui->tabWidget->removeTab(1);
            ui->deleteBookPushButton->setHidden(true);
            ui->modifyBookPushButton->setHidden(true);
            ui->borrowBookPushButton->setHidden(true);
            ui->actionUserInfomartion->setEnabled(false);
        } break;
        case Manage::UserType::Reader: {
            ui->welcomeLabel->setText(tr("您好，") + manage.getCurrentUser()->getName());
            ui->tabWidget->removeTab(5);
            ui->tabWidget->removeTab(4);
            ui->tabWidget->removeTab(3);
            ui->tabWidget->removeTab(2);
            ui->deleteBookPushButton->setHidden(true);
            ui->modifyBookPushButton->setHidden(true);
            ui->borrowBookPushButton->setHidden(false);
            ui->actionUserInfomartion->setEnabled(true);
            ui->updateBorrowPushButton->click();
        } break;
        case Manage::UserType::Admin: {
            ui->welcomeLabel->setText(tr("您好，") + manage.getCurrentUser()->getName());
            ui->tabWidget->removeTab(1);
            ui->deleteBookPushButton->setHidden(false);
            ui->modifyBookPushButton->setHidden(false);
            ui->borrowBookPushButton->setHidden(true);
            ui->actionUserInfomartion->setEnabled(true);
        } break;
        default:
            break;
    }

    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::uiRestore() {
    auto &tabWidget = *(ui->tabWidget);
    tabWidget.clear();
    tabWidget.addTab(ui->searchBookTab, "书目检索");
    tabWidget.addTab(ui->borrowRecordTab, "我的借书记录");
    tabWidget.addTab(ui->importBookTab, "新书入库");
    tabWidget.addTab(ui->borrowManageTab, "借阅管理");
    tabWidget.addTab(ui->userManageTab, "用户检索");
    tabWidget.addTab(ui->newUserTab, "创建用户");
    uiInit();
}

void MainWindow::uiInit() {
    // 书目检索页
    ui->searchBookNameCheckBox->setChecked(false);
    ui->searchBookAuthorCheckBox->setChecked(false);
    ui->searchBookTypeCheckBox->setChecked(false);
    ui->searchBookPressCheckBox->setChecked(false);
    ui->searchBookPubTimeCheckBox->setChecked(false);
    ui->searchBookPriceCheckBox->setChecked(false);
    ui->clearInputSearchBookPushButton->click();
    ui->searchAllBooksCheckBox->setChecked(false);
    ui->vagueBookCheckBox->setChecked(false);
    ui->clearBookShowPushButton->click();
    // 我的借书记录页
    ui->myBorrowTableWidget->setRowCount(0);
    ui->myBorrowTableWidget->clearContents();
    ui->showBorrowingCheckBox->setChecked(true);
    // 新书入库页
    ui->clearInputNewBookPushButton->click();
    ui->newBookLogTableWidget->setRowCount(0);
    ui->newBookLogTableWidget->clearContents();
    // 借阅管理页
    ui->searchRecordReaderIDCheckBox->setChecked(false);
    ui->searchRecordBookIDCheckBox->setChecked(false);
    ui->searchRecordBorrowDateCheckBox->setChecked(false);
    ui->searchRecordReturnDateCheckBox->setChecked(false);
    ui->searchRecordBookStatusCheckBox->setChecked(false);
    ui->searchAllRecordsCheckBox->setChecked(false);
    ui->vagueRecordCheckBox->setChecked(false);
    ui->clearInputSearchRecordPushButton->click();
    ui->clearRecordShowPushButton->click();
    // 用户检索页
    ui->searchUserNameCheckBox->setChecked(false);
    ui->searchUserTelCheckBox->setChecked(false);
    ui->searchUserMaxBorrowNumbersCheckBox->setChecked(false);
    ui->searchUserAgeCheckBox->setChecked(false);
    ui->searchUserTypeCheckBox->setChecked(false);
    ui->searchAllUsersCheckBox->setChecked(false);
    ui->vagueUserCheckBox->setChecked(false);
    ui->clearInputSearchUserPushButton->click();
    ui->clearUserShowPushButton->click();
    // 创建用户页
    ui->clearInputNewUserPushButton->click();
    ui->newUserLogTableWidget->setRowCount(0);
    ui->newUserLogTableWidget->clearContents();
}

void MainWindow::on_actionAbout_triggered() {
    AboutDialog aboutDia;
    aboutDia.exec();
}

// 10
void MainWindow::on_returnBookPushButton_clicked() {
    // 引用借书记录表
    auto &borrowingShow = *(ui->myBorrowTableWidget);

    // 是否有选中行
    if (!borrowingShow.selectedItems().count())
        return;

    // 获取选中行的信息
    int row = borrowingShow.currentRow();
    int id = borrowingShow.item(row, 0)->text().toInt();
    bool isReturned = borrowingShow.item(row, 3)->text() == "已归还";
    if (isReturned) {
        QMessageBox::information(this, "操作提示", "该书已归还");
        return;
    }

    // 还书
    try {
        double money = manage.backBook(id);
        if (money > 0) {
            QMessageBox::information(this, "操作成功", QString{"还书成功，超期费用："} + QString::number(money));
        } else {
            QMessageBox::information(this, "操作成功", QString{"还书成功"});
        }
    } catch(std::invalid_argument &e) {
        QMessageBox::warning(this, "操作失败", "抱歉，没有这条记录！");
    }
}

// 18
void MainWindow::on_exportResultRecordPushButton_clicked()
{
    // 引用表
    auto &table = *(ui->recordShowTableWidget);

    // 检测表是否为空
    if (table.rowCount() == 0) {
        QMessageBox::information(this, "操作提示", "表中没有数据");
        return;
    }

    // 打开一个文件选择窗口
    QString fileName = QFileDialog::getSaveFileName( this, tr("保存图书信息配置文件"), "/", tr("数据交换文件(*json)"));

    // 未获取到文件名
    if (fileName.isEmpty())
        return;

    // 构建JSON数组 - records
    QJsonArray jsonRecords;

    for (int i = 0; i < table.rowCount(); i++) {
        // 构建单个JSON对象 - record
        QJsonObject jsonRecord;
        jsonRecord.insert("reader ID", table.item(i, 0)->text().toInt());
        jsonRecord.insert("book ID", table.item(i, 1)->text().toInt());
        jsonRecord.insert("borrow date", table.item(i, 2)->text());
        jsonRecord.insert("return date", table.item(i, 3)->text());
        if (table.item(i, 4)->text() == "借阅中")
            jsonRecord.insert("status", "Borrowing");
        else
            jsonRecord.insert("status", "Returned");
        // 追加到books中
        jsonRecords.append(jsonRecord);
    }
    // 构建json根对象
    QJsonObject jsonRoot;
    jsonRoot.insert("books", jsonRecords);

    // 构建json文档
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonRoot);

    // 将json文档字符串化
    QString jsonDocStr{jsonDoc.toJson()};

    // 打开指定文件
    QFile file{fileName + ".json"};
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::warning(this, "System Error", "Can not open such file.");
        return;
    }
    // 从文件构造输出流
    QTextStream out(&file);
    // 输出到输出流中
    out << jsonDocStr;
    // 关闭文件
    file.close();
    // 提示操作成功
    QMessageBox::information(this, "操作成功", "已导出到文件" + fileName + ".json");
}

void MainWindow::on_actionUserInfomartion_triggered()
{
    userInfoDialog dia(this, manage.getCurrentUser());
    dia.exec();
    // 更新姓名
    ui->welcomeLabel->setText("您好，"+manage.getCurrentUser()->getName());
}
