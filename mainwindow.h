#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void uiAuto(); // UI自动匹配
    void uiRestore(); // UI复原
    void uiInit();      // UI初始化

private slots:
    void receiveLogin(); // 接受登录

    void receiveLogout(); // 请求注销

    void on_searchAllBooksCheckBox_stateChanged(int arg1);

    void on_clearInputSearchBookPushButton_clicked();

    void on_clearUserShowPushButton_clicked();

    void on_clearInputSearchUserPushButton_clicked();

    void on_searchAllUsersCheckBox_stateChanged(int arg1);

    void on_searchUserPushButton_clicked();

    void on_deleteUserPushButton_clicked();

    void on_modifyUserPushButton_clicked();

    void on_clearInputNewUserPushButton_clicked();

    void on_newUserIsAdminCheckBox_stateChanged(int arg1);

    void on_doNewUserPushButton_clicked();

    void on_newUserTelLineEdit_editingFinished();

    void on_exportResultUserPushButton_clicked();

    void on_importNewUsersPushButton_clicked();

    void on_quearyBookPushButton_clicked();

    void on_clearBookShowPushButton_clicked();

    void on_deleteBookPushButton_clicked();

    void on_modifyBookPushButton_clicked();

    void on_doNewBookPushButton_clicked();

    void on_clearInputNewBookPushButton_clicked();

    void on_exportResultBookPushButton_clicked();

    void on_importNewBooksPushButton_clicked();

    void on_clearInputSearchRecordPushButton_clicked();

    void on_searchAllRecordsCheckBox_stateChanged(int arg1);

    void on_searchRecordPushButton_clicked();

    void on_clearRecordShowPushButton_clicked();

    void on_updateBorrowPushButton_clicked();

    void on_borrowBookPushButton_clicked();

    void on_actionExitLogin_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_returnBookPushButton_clicked();

    void on_exportResultRecordPushButton_clicked();

    void on_actionUserInfomartion_triggered();

signals:
    void loginShow(); // 显示登录对话框
    void quit();      // 退出

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
