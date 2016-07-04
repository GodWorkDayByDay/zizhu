#ifndef TRANSDIALOG_H
#define TRANSDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class transDialog;
}

class transDialog : public QDialog
{
    Q_OBJECT

public:
    typedef int (*FUN[256])(void);


public:
    explicit transDialog(QWidget *parent = 0);
    ~transDialog();
    void inittransDialg();
    void initConnections();


private slots:
    void on_exitpushButton_clicked();

    //void on_pushButton_btn_2_clicked();

    void on_pushButton_btn_2_clicked();

private:
    Ui::transDialog *ui;

public:
    int btnCount;
    QStringList  btnsText;

    FUN funs;
};

#endif // TRANSDIALOG_H
