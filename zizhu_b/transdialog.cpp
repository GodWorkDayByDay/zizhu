#include "transdialog.h"
#include "ui_transdialog.h"
#include <Qpainter>
#include <QPropertyAnimation>
#include <QDebug>
transDialog::transDialog(QWidget *parent) :
    QDialog(NULL),
    ui(new Ui::transDialog)
{

    ui->setupUi(this);
    showFullScreen();

//    setWindowFlags(Qt::Dialog);
//    setWindowModality(Qt::ApplicationModal);

//    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
//    animation->setDuration(300);
//    animation->setStartValue(0);
//    animation->setEndValue(1);
//    animation->start();
}


void transDialog::inittransDialg()
{
    btnCount = btnCount+1;
    switch(btnCount)
    {
        case 1:
            ui->pushButton_btn_1->hide();
        case 2:
            ui->pushButton_btn_2->hide();
        case 3:
            ui->pushButton_btn_3->hide();
        case 4:
            ui->pushButton_btn_4->hide();
        case 5:
            ui->pushButton_btn_5->hide();
        case 6:
            ui->pushButton_btn_6->hide();
        case 7:
            ui->pushButton_btn_7->hide();
        case 8:
            ui->pushButton_btn_8->hide();
        case 9:
            ui->pushButton_btn_9->hide();
        case 10:
            ui->pushButton_btn_10->hide();
        case 11:
            ui->pushButton_btn_11->hide();
        case 12:
            ui->pushButton_btn_12->hide();
        case 13:
            ui->pushButton_btn_13->hide();
        case 14:
            ui->pushButton_btn_14->hide();
        case 15:
            ui->pushButton_btn_15->hide();
        case 16:
            ui->pushButton_btn_16->hide();

    default:
        break;
    }

    for(int i = btnsText.count() ; i<17;i++)
    {
        btnsText.append("");
    }

    ui->titlelabel->setText(btnsText.at(0));
    ui->pushButton_btn_1->setText(btnsText.at(1));
    ui->pushButton_btn_2->setText(btnsText.at(2));
    ui->pushButton_btn_3->setText(btnsText.at(3));
    ui->pushButton_btn_4->setText(btnsText.at(4));
    ui->pushButton_btn_5->setText(btnsText.at(5));
    ui->pushButton_btn_6->setText(btnsText.at(6));
    ui->pushButton_btn_7->setText(btnsText.at(7));
    ui->pushButton_btn_8->setText(btnsText.at(8));
    ui->pushButton_btn_9->setText(btnsText.at(9));
    ui->pushButton_btn_10->setText(btnsText.at(10));
    ui->pushButton_btn_11->setText(btnsText.at(11));
    ui->pushButton_btn_12->setText(btnsText.at(12));
    ui->pushButton_btn_13->setText(btnsText.at(13));
    ui->pushButton_btn_14->setText(btnsText.at(14));
    ui->pushButton_btn_15->setText(btnsText.at(15));
    ui->pushButton_btn_16->setText(btnsText.at(16));


    initConnections();
}


transDialog::~transDialog()
{
    delete ui;
}


void transDialog::on_exitpushButton_clicked()
{
    this->close();
}





void transDialog::initConnections()
{

    switch(btnCount)
    {
        case 16:
            connect(ui->pushButton_btn_16,&QPushButton::clicked,this,funs[15]);
        case 15:
             connect(ui->pushButton_btn_15,&QPushButton::clicked,this,funs[14]);
        case 14:
             connect(ui->pushButton_btn_14,&QPushButton::clicked,this,funs[13]);
        case 13:
             connect(ui->pushButton_btn_13,&QPushButton::clicked,this,funs[12]);
        case 12:
             connect(ui->pushButton_btn_12,&QPushButton::clicked,this,funs[11]);
        case 11:
             connect(ui->pushButton_btn_11,&QPushButton::clicked,this,funs[10]);
        case 10:
             connect(ui->pushButton_btn_10,&QPushButton::clicked,this,funs[9]);
        case 9:
             connect(ui->pushButton_btn_9,&QPushButton::clicked,this,funs[8]);
        case 8:
             connect(ui->pushButton_btn_8,&QPushButton::clicked,this,funs[7]);
        case 7:
             connect(ui->pushButton_btn_7,&QPushButton::clicked,this,funs[6]);
        case 6:
             connect(ui->pushButton_btn_6,&QPushButton::clicked,this,funs[5]);
        case 5:
             connect(ui->pushButton_btn_5,&QPushButton::clicked,this,funs[4]);
        case 4:
             connect(ui->pushButton_btn_4,&QPushButton::clicked,this,funs[3]);
        case 3:
             connect(ui->pushButton_btn_3,&QPushButton::clicked,this,funs[2]);
        case 2:
             connect(ui->pushButton_btn_2,&QPushButton::clicked,this,funs[1]);
        case 1:
             connect(ui->pushButton_btn_1,&QPushButton::clicked,this,funs[0]);
    }
}


void transDialog::on_pushButton_btn_2_clicked()
{

}
