#ifndef UPDATEBOOKDIALOG_H
#define UPDATEBOOKDIALOG_H

#include <QDialog>
#include "common.h"

namespace Ui {
class updateBookDialog;
}

class updateBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit updateBookDialog(QWidget *parent = nullptr, Book *oldBook = nullptr);
    ~updateBookDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::updateBookDialog *ui;
    Book *oldBook;
};

#endif // UPDATEBOOKDIALOG_H
