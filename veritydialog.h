#ifndef VERITYDIALOG_H
#define VERITYDIALOG_H

#include <QDialog>
#include "common.h"

namespace Ui {
class VerityDialog;
}

class VerityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VerityDialog(QWidget *parent = nullptr, User *user = nullptr, const QString &operation = QString{});
    ~VerityDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::VerityDialog *ui;
    User *user;
};

#endif // VERITYDIALOG_H
