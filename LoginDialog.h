#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <registerdialog.h>
#include <mainwindow.h>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();


private slots:
    void on_nextButton_clicked();

    void on_anonymousBox_stateChanged(int arg1);

    void on_ReaderRegisterPushButton_clicked();

    void receiveShow();

    void on_exitButton_clicked();

signals:
    void showMain(); // 显示主窗口
    void quit();

private:
    Ui::LoginDialog *ui;
    RegisterDialog *registerDialog = new RegisterDialog(this);
};

#endif // LOGINDIALOG_H
