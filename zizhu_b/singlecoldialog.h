#ifndef SINGLECOLDIALOG_H
#define SINGLECOLDIALOG_H

#include <QDialog>

namespace Ui {
class singleColDialog;
}

class singleColDialog : public QDialog
{
    Q_OBJECT

public:
    explicit singleColDialog(QWidget *parent = 0);
    ~singleColDialog();

private:
    void initConnections();

private slots:


    void on_pushButton_clicked();

private:
    Ui::singleColDialog *ui;

public:
    void initsingleDialg();
    int btnCount;
    QStringList  btnsText;
    typedef int (*FUN[256])(void);
    FUN funs;
};

#endif // SINGLECOLDIALOG_H
