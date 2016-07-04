#include <QDebug>
#include <QString>
#include "functions.h"
#include "detaildialog.h"
#include "cardreader.h"
#include "pos/cupms.h"
#include "pos/trans.h"

int funAuth(void)
{

    //{ NULL, 2, "预授权完成请求",    Trans_PreAuthCompleteRequest, (void *) 0 },
   //{ NULL, 3, "预授权完成通知",    Trans_PreAuthCompleteNote, (void *) 0 },
   // { NULL, 4, "预授权撤销",        Trans_PreAuthVoid, (void *) 0 },
    //{ NULL, 5, "预授权完成撤销",    Trans_PreAuthCompleteVoid, (void *) 0 },
    DetailDialog::getInstance()->show();

    //预售权
    Trans_PreAuth((void *)0);
    CardReader::getInstance()->Qt_pushOutCard();
    DetailDialog::getInstance()->hide();
   return 0;

}
int funAuthReq(void)
{

    //{ NULL, 2, "预授权完成请求",    Trans_PreAuthCompleteRequest, (void *) 0 },
    DetailDialog::getInstance()->show();

    //预售权
    Trans_PreAuthCompleteRequest((void *)0);
        CardReader::getInstance()->Qt_pushOutCard();
    DetailDialog::getInstance()->hide();
   return 0;

}
int funAuthNotice(void)
{

   //{ NULL, 3, "预授权完成通知",    Trans_PreAuthCompleteNote, (void *) 0 },
    DetailDialog::getInstance()->show();

    //预售权
    Trans_PreAuthCompleteNote((void *)0);
        CardReader::getInstance()->Qt_pushOutCard();
    DetailDialog::getInstance()->hide();
   return 0;

}
int funAuthvoid(void)
{

   // { NULL, 4, "预授权撤销",        Trans_PreAuthVoid, (void *) 0 },
    DetailDialog::getInstance()->show();

    //预售权
    Trans_PreAuthVoid((void *)0);
        CardReader::getInstance()->Qt_pushOutCard();
    DetailDialog::getInstance()->hide();
   return 0;

}
int funAuthresVoid(void)
{

    //{ NULL, 5, "预授权完成撤销",    Trans_PreAuthCompleteVoid, (void *) 0 },
    DetailDialog::getInstance()->show();

    //预售权
    Trans_PreAuthCompleteVoid((void *)0);
        CardReader::getInstance()->Qt_pushOutCard();
    DetailDialog::getInstance()->hide();
   return 0;

}
int fun1(void)
{
    DetailDialog::getInstance()->show();

    //签到
    Trans_Logon((void *)1);
    DetailDialog::getInstance()->hide();
   return 0;
}

int fun2(void)
{
    //DetailDialog::getInstance()->show();
    //Trans_Void((void *)0);
    //DetailDialog::getInstance()->hide();
    return 0;
}

int fun3(void)
{
   qDebug()<<"hello fun3";
   return 0;
}

int fun4(void)
{
   qDebug()<<"hello fun4";
   return 0;
}

int fun5(void)
{
   qDebug()<<"hello fun5";
   return 0;
}

int fun6(void)
{
   qDebug()<<"hello fun6";
   return 0;
}
int fun7(void)
{
   qDebug()<<"hello fun7";
   return 0;
}
int fun8(void)
{
   qDebug()<<"hello fun8";
   return 0;
}
int fun9(void)
{
   qDebug()<<"hello fun9";
   return 0;
}
int fun10(void)
{
   qDebug()<<"hello fun10";
   return 0;
}
int fun11(void)
{
   qDebug()<<"hello fun11";
   return 0;
}
int fun12(void)
{
   qDebug()<<"hello fun12";
   return 0;
}
int fun13(void)
{
   qDebug()<<"hello fun13";
   return 0;
}
int fun14(void)
{
   qDebug()<<"hello fun14";
   return 0;
}
int fun15(void)
{
   qDebug()<<"hello fun15";
   return 0;
}

int fun16(void)
{
   qDebug()<<"hello fun16";
   return 0;
}
