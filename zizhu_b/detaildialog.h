#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class DetailDialog;
}



class DetailDialog : public QDialog
{
    Q_OBJECT

public:
    static DetailDialog *getInstance();

    void startTrans();

    /*设置显示输入框和输入框模式，是密码还是普通文本*/
    void isShowInputText(bool isHide,bool isPassword,bool isMoney);

    /*获取输入框的字符串*/
    QString getInputText();

    /*清除输入框的字符串*/
    void clearInputText();


    /*清屏*/
    void clearScreen();

    /*清行*/
    void clearLine(int line);

    /*清理几行*/
    void clearStartLines(int lineno,int linecount);

    /*设置行的内容*/
    void setText(int lineno,QString str,int align);

    void keyPressEvent(QKeyEvent * event);

    /*获取q按键*/
    void getKeyConfirm();


    /*向输入框输入字符串*/
    int inputTextKeys();

private:
    explicit DetailDialog(QWidget *parent = 0);
    ~DetailDialog();
private slots:
    void on_pushButton_clicked();

private:
    Ui::DetailDialog *ui;
    static  DetailDialog * instance;
    bool isConfirm;
    QString inputTextString;
};

#endif // DETAILDIALOG_H
