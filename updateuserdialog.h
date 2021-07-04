#ifndef UPDATEUSERDIALOG_H
#define UPDATEUSERDIALOG_H

#include <QDialog>
#include "common.h"

namespace Ui {
class updateUserDialog;
}

class updateUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit updateUserDialog(QWidget *parent = nullptr, User *oldUser = nullptr);
    ~updateUserDialog();

private slots:

    void on_buttonBox_accepted();

    void on_newTelLineEdit_editingFinished();

private:
    Ui::updateUserDialog *ui;
    User *oldUser;
};

#endif // UPDATEUSERDIALOG_H
