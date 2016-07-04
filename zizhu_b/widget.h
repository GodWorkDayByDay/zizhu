#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "transdialog.h"
#include "detaildialog.h"
#include "zzprinter.h"
#include "zzkeypad.h"
#include "pos/cupms.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:


public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_exitButton_clicked();

    void on_pushButton_btn_1_clicked();
    void on_pushButton_btn_8_clicked();
    void on_pushButton_btn_6_clicked();

    void on_pushButton_btn_2_clicked();

    void on_pushButton_btn_7_clicked();

    void on_pushButton_btn_3_clicked();

    void on_pushButton_btn_4_clicked();

    void on_pushButton_btn_5_clicked();

private:
    Ui::Widget *ui;
    transDialog * transdialog;
    DetailDialog * detaildialog;
    zzprinter * zprinter;
    zzkeypad * zkeypad;

};

#endif // WIDGET_H
