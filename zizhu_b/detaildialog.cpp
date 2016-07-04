#include "detaildialog.h"
#include "ui_detaildialog.h"
#include <QPropertyAnimation>
#include <QDebug>
#include <QTime>
#include "zzkeypad.h"
#include "singleton.h"

DetailDialog * DetailDialog::instance = NULL;

DetailDialog::DetailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailDialog)
{
    ui->setupUi(this);
    showFullScreen();
    isConfirm = false;
}


DetailDialog::~DetailDialog()
{
    delete ui;
}

DetailDialog* DetailDialog::getInstance()
{
    if(instance == NULL)
        instance = new DetailDialog();

    return instance;
}


void DetailDialog::clearLine(int line)
{
   setText(line,"",0);
}

void DetailDialog::clearScreen()
{
    for(int i = 1; i <=10 ;i++)
        clearLine(i);
}

void DetailDialog::startTrans()
{
    clearScreen();
}

void DetailDialog::setText(int lineno, QString str,int align)
{
    Qt::AlignmentFlag alignflag;
    if(align == 0)
        alignflag = Qt::AlignLeft;
    else if(align == 1)
        alignflag = Qt::AlignCenter;
    else if(align == 2)
        alignflag = Qt::AlignRight;

    switch (lineno) {
    case 1:
        ui->label_1->setText(str);
        ui->label_1->setAlignment(alignflag);
        break;
    case 2:
        ui->label_2->setText(str);
        ui->label_2->setAlignment(alignflag);
        break;
    case 3:
        ui->label_3->setText(str);
        ui->label_3->setAlignment(alignflag);
        break;
    case 4:
        ui->label_4->setText(str);
        ui->label_4->setAlignment(alignflag);
        break;
    case 5:
        ui->label_5->setText(str);
        ui->label_5->setAlignment(alignflag);
        break;
    case 6:
        ui->label_6->setText(str);
        ui->label_6->setAlignment(alignflag);
        break;
    case 7:
        ui->label_7->setText(str);
        ui->label_7->setAlignment(alignflag);
        break;
    case 8:
        ui->label_8->setText(str);
        ui->label_8->setAlignment(alignflag);
        break;
    case 9:
        ui->label_9->setText(str);
        ui->label_9->setAlignment(alignflag);
        break;
    case 10:
        ui->label_10->setText(str);
        ui->label_10->setAlignment(alignflag);
        break;
    default:
        break;
    }
}

void DetailDialog::isShowInputText(bool isHide, bool isPassword, bool isMoney)
{
    if(isHide == false)
    {
        if(isPassword == true)
        {
            qDebug()<<"is password";
            ui->password_lineEdit->setEchoMode(QLineEdit::Password);
        }
        else
        {
            qDebug()<<"is normal";
            ui->password_lineEdit->setEchoMode(QLineEdit::Normal);
        }

        if(isMoney == true)
        {
            //设置字符
            ui->password_lineEdit->setAlignment(Qt::AlignRight);
        }
        else
        {
            ui->password_lineEdit->setAlignment(Qt::AlignLeft);
        }

        ui->password_lineEdit->show();
        ui->password_lineEdit->clear();
        /*隐藏第5行*/
        ui->label_5->hide();

        /*设置输入框获取焦点*/
        ui->password_lineEdit->setFocus();
    }
    else
    {
        ui->password_lineEdit->hide();
        ui->label_5->show();
    }


}

void DetailDialog::on_pushButton_clicked()
{
//    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
//    animation->setDuration(300);
//    animation->setStartValue(1);
//    animation->setEndValue(0);
//    animation->start();
//    connect(animation, SIGNAL(finished()), this, SLOT(close()));
    this->hide();
}

QString DetailDialog::getInputText()
{
    return ui->password_lineEdit->text();
}

void DetailDialog::keyPressEvent(QKeyEvent *event)
{
     if(event->key()==Qt::Key_Q)
     {
         isConfirm = true;
     }
}



void DetailDialog::getKeyConfirm()
{
    while(true)
    {
        QTime dieTime = QTime::currentTime().addMSecs(1000);
        while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

        if(isConfirm == true)
        {
            qDebug()<<"isConfirm";
            isConfirm = false;
            return ;
        }
    }
}

int DetailDialog::inputTextKeys()
{
     char ch ;
    inputTextString.clear();

    while(true){
    Singleton<zzkeypad>::getInstance()->Qt_getInputKey(&ch);
    qDebug("ch = %02X",ch);
    if(ch == 0x0D)
        return SUCCESS;
    if(ch == 0x08)
        inputTextString.remove(inputTextString.length()-1,1);
    else if(ch == 0x1b)
        return FAIL;
    else
        inputTextString.append(ch);

    ui->password_lineEdit->setText(inputTextString);
   }
}


void DetailDialog::clearInputText()
{
    ui->password_lineEdit->clear();
}

void DetailDialog::clearStartLines(int lineno, int linecount)
{
    for(int i = lineno;i < lineno+linecount;i++)
    {
        clearLine(i);
    }
}


