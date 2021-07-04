#ifndef USERINFODIALOG_H
#define USERINFODIALOG_H

#include <QDialog>
#include "common.h"

namespace Ui {
class userInfoDialog;
}

class userInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit userInfoDialog(QWidget *parent = nullptr, User *user = nullptr);
    ~userInfoDialog();

private slots:
    void on_updateNamePushButton_clicked();

    void on_updateAgePushButton_clicked();

    void on_updateTelPushButton_clicked();

    void on_updatePwdPushButton_clicked();

    void on_logOutPushButton_clicked();

signals:
    void logout();

private:
    Ui::userInfoDialog *ui;
    User *user;
};

#endif // USERINFODIALOG_H
