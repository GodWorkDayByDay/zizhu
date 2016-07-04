#include "widget.h"
#include <QApplication>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include "zzkeypad.h"
#include "cardreader.h"
#include "zzprinter.h"
#include "singleton.h"

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QFile file("log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
//    w.show();


    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    if(PrmMngr_InitFile() != BANK_OK)
         return a.exec();

    //注册MessageHandler,qDebug信息保存到文件
    qInstallMessageHandler(outputMessage);



    //初始化ic/磁条卡读卡器的动态库,并打开设备
    CardReader::getInstance()->Qt_loadCardReaderDll();
    CardReader::getInstance()->Qt_initAndOpenCardReader("Com4");


    //初始化打印机
    zzprinter::getInstance()->Qt_loadPrinterDll();
    zzprinter::getInstance()->Qt_initAndOpenPrinter("COM1");

    //初始化keypad的动态库
    Singleton<zzkeypad>::getInstance()->Qt_loadKeypadDll();
    Singleton<zzkeypad>::getInstance()->Qt_initAndOpenKeyPad(3);

    w.showFullScreen();
    return a.exec();
}
