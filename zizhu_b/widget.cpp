#include "widget.h"
#include "ui_widget.h"
#include <QPropertyAnimation>
#include "functions.h"
#include "pos/trans.h"
#include <QTime>
#include <QDebug>
#include "cardreader.h"
#include "singleton.h"

//extern Term_Config gTermCfg;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

    ui->setupUi(this);

    ui->titlelabel->setText("消费菜单");
    ui->pushButton_btn_1->setText("1.消费");
    ui->pushButton_btn_2->setText("2.撤销");
    ui->pushButton_btn_3->setText("3.退货");
    ui->pushButton_btn_4->setText("4.预授权");
    ui->pushButton_btn_5->setText("5.离线");
    ui->pushButton_btn_6->setText("6.打印");
    ui->pushButton_btn_7->setText("7.管理");
    ui->pushButton_btn_8->setText("8.设置主秘钥");

    DetailDialog::getInstance()->hide();
}


Widget::~Widget()
{
    delete ui;
}

void Widget::on_exitButton_clicked()
{
    //界面动画，改变透明度的方式消失1 - 0渐变
    zzprinter::getInstance()->Qt_ClosePrinter();
    CardReader::getInstance()->Qt_CardReader_Close();


    Singleton<zzkeypad>::getInstance()->exitWaitFlag = true;
    while(true)
    {
        QTime dieTime = QTime::currentTime().addMSecs(100);
        while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        if(Singleton<zzkeypad>::getInstance()->exitWaitFlag == true)
        {
            break;
        }
    }
    Singleton<zzkeypad>::getInstance()->Qt_CloseKeyPad();


    DetailDialog::getInstance()->close();


    this->close();
}


void Widget::on_pushButton_btn_1_clicked()
{

    detaildialog  = DetailDialog::getInstance();

    detaildialog->show();
    //消费函数
    Trans_Sale((void *)99);
    CardReader::getInstance()->Qt_pushOutCard();
    DetailDialog::getInstance()->hide();
}

void Widget::on_pushButton_btn_8_clicked()
{
    detaildialog  = DetailDialog::getInstance();

    detaildialog->show();

    KeyMngr_ManualLoadKey((void *) 0);
    DetailDialog::getInstance()->hide();
}

void Widget::on_pushButton_btn_6_clicked()
{

}

void Widget::on_pushButton_btn_2_clicked()
{

    detaildialog  = DetailDialog::getInstance();
    detaildialog->show();

    //消费函数
    Trans_Void(0);
    CardReader::getInstance()->Qt_pushOutCard();
    DetailDialog::getInstance()->hide();

}

void Widget::on_pushButton_btn_7_clicked()
{

    transdialog = new transDialog();

    //设置按钮的的个数
    transdialog->btnCount = 9;

    transdialog->btnsText.append("管理");
    transdialog->btnsText.append("1.签到");
    transdialog->btnsText.append("2.签退");
    transdialog->btnsText.append("3.交易查询");
    transdialog->btnsText.append("4.柜员");
    transdialog->btnsText.append("5.外线号码");
    transdialog->btnsText.append("6.结算");
    transdialog->btnsText.append("7.锁定终端");
    transdialog->btnsText.append("8.版本");
    transdialog->btnsText.append("9.通讯类型");


    transdialog->funs[0] = fun1;
    transdialog->funs[1] = fun2;
    transdialog->funs[2] = fun3;
    transdialog->funs[3] = fun4;
    transdialog->funs[4] = fun5;
    transdialog->funs[5] = fun6;
    transdialog->funs[6] = fun7;
    transdialog->funs[7] = fun8;
    transdialog->funs[8] = fun9;

    transdialog->inittransDialg();
    transdialog->show();

}

void Widget::on_pushButton_btn_3_clicked()
{
    detaildialog  = DetailDialog::getInstance();
    detaildialog->show();
    Trans_Refund((void *)0);
    CardReader::getInstance()->Qt_pushOutCard();
    DetailDialog::getInstance()->hide();
}

void Widget::on_pushButton_btn_4_clicked()
{
    transdialog = new transDialog();

    //设置按钮的的个数
    transdialog->btnCount = 5;

    transdialog->btnsText.append("预授权");
    transdialog->btnsText.append("1.预授权");
    transdialog->btnsText.append("2.预授权完成请求");
    transdialog->btnsText.append("3.预授权完成通知");
    transdialog->btnsText.append("4.预授权撤销");
    transdialog->btnsText.append("5.预授权完成撤销");


    transdialog->funs[0] = funAuth;
    transdialog->funs[1] = funAuthReq;
    transdialog->funs[2] = funAuth;
    transdialog->funs[3] = funAuth;
    transdialog->funs[4] = funAuth;

    transdialog->inittransDialg();
    transdialog->show();
}

void Widget::on_pushButton_btn_5_clicked()
{

}
