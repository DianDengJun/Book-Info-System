#include "LoginDialog.h"
#include "common.h"
#include "mainwindow.h"
#include "registerdialog.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTranslator>

char Password::key = 0x10;   // 用户密码密钥
User Anonymous{0,"匿名用户"}; // 匿名用户

// 管理中心初始化
Manage manage;


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setFont(QFont("PingFang SC", 13));
    QString appPath = QCoreApplication::applicationDirPath();
    // 载入读者信息
    manage.initUsers(appPath + "/res/users.json");
    // 载入图书信息
    manage.initBooks(appPath + "/res/books.json");
    // 载入借阅记录
    manage.initRecords(appPath + "/res/records.json");

    // 主界面
    MainWindow w;
    // 登录界面
    LoginDialog logDia;
    logDia.show();

    // 信号与槽连接
    QObject::connect(&logDia, SIGNAL(showMain()), &w, SLOT(receiveLogin()));
    QObject::connect(&w, SIGNAL(loginShow()), &logDia, SLOT(receiveShow()));
    QObject::connect(&w, SIGNAL(quit()), &app, SLOT(quit()));
    QObject::connect(&logDia, SIGNAL(quit()), &app, SLOT(quit()));

    return app.exec();

}
