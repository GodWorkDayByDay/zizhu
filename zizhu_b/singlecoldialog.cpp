#include "singlecoldialog.h"
#include "ui_singlecoldialog.h"

singleColDialog::singleColDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::singleColDialog)
{
    ui->setupUi(this);
    showFullScreen();
}

singleColDialog::~singleColDialog()
{
    delete ui;
}


void singleColDialog::initsingleDialg()
{
    btnCount = btnCount+1;
    switch(btnCount)
    {
        case 1:
            ui->pushButton_1->hide();
        case 2:
            ui->pushButton_2->hide();
        case 3:
            ui->pushButton_3->hide();
        case 4:
            ui->pushButton_4->hide();
        case 5:
            ui->pushButton_5->hide();
        case 6:
            ui->pushButton_6->hide();
        case 7:
            ui->pushButton_7->hide();
        case 8:
            ui->pushButton_8->hide();
        case 9:
            ui->pushButton_9->hide();
        case 10:
            ui->pushButton_10->hide();

    default:
        break;
    }

    for(int i = btnsText.count() ; i<10;i++)
    {
        btnsText.append("");
    }

    ui->pushButton_1->setText(btnsText.at(0));
    ui->pushButton_2->setText(btnsText.at(1));
    ui->pushButton_3->setText(btnsText.at(2));
    ui->pushButton_4->setText(btnsText.at(3));
    ui->pushButton_5->setText(btnsText.at(4));
    ui->pushButton_6->setText(btnsText.at(5));
    ui->pushButton_7->setText(btnsText.at(6));
    ui->pushButton_8->setText(btnsText.at(7));
    ui->pushButton_9->setText(btnsText.at(8));
    ui->pushButton_10->setText(btnsText.at(9));

    initConnections();

}


void singleColDialog::initConnections()
{
    switch(btnCount)
    {
        case 10:
             connect(ui->pushButton_10,&QPushButton::clicked,this,funs[9]);
        case 9:
             connect(ui->pushButton_9,&QPushButton::clicked,this,funs[8]);
        case 8:
             connect(ui->pushButton_8,&QPushButton::clicked,this,funs[7]);
        case 7:
             connect(ui->pushButton_7,&QPushButton::clicked,this,funs[6]);
        case 6:
             connect(ui->pushButton_6,&QPushButton::clicked,this,funs[5]);
        case 5:
             connect(ui->pushButton_5,&QPushButton::clicked,this,funs[4]);
        case 4:
             connect(ui->pushButton_4,&QPushButton::clicked,this,funs[3]);
        case 3:
             connect(ui->pushButton_3,&QPushButton::clicked,this,funs[2]);
        case 2:
             connect(ui->pushButton_2,&QPushButton::clicked,this,funs[1]);
        case 1:
             connect(ui->pushButton_1,&QPushButton::clicked,this,funs[0]);
    }
}

void singleColDialog::on_pushButton_clicked()
{
    this->close();
}
