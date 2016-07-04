#include "zzprinter.h"

#include <QDebug>
#include <QTextCodec>

zzprinter * zzprinter::zprinter = NULL;

zzprinter::zzprinter()
{

}


int zzprinter::Qt_loadPrinterDll()
{
    dllPath = "printerlib\\TKIOSKDLL.dll";

    /*加载动态库*/
    loadlib = new QLibrary(dllPath);

    if(loadlib->load()) {
          qDebug() << "load printer dll ok!";
      } else {
          qDebug() << "load error!";
          return FAIL;
      }

    return initFuncs();
}

int zzprinter::initFuncs()
{
    //open serial port
    Qt_TKIOSK_OpenCom = (TKIOSK_OpenCom)loadlib->resolve("TKIOSK_OpenCom");
    if (Qt_TKIOSK_OpenCom == NULL) {
        qDebug() << "TKIOSK_OpenCom eror!";
        return FAIL;
    }

    //Set the timeouts of serial port.
    Qt_TKIOSK_SetComTimeOuts = (TKIOSK_SetComTimeOuts)loadlib->resolve("TKIOSK_SetComTimeOuts");
    if (Qt_TKIOSK_SetComTimeOuts == NULL) {
        qDebug() << "TKIOSK_SetComTimeOuts error!";
        return FAIL;
    }

    //Write data to port or file
    Qt_TKIOSK_WriteData = (TKIOSK_WriteData)loadlib->resolve("TKIOSK_WriteData");
    if (Qt_TKIOSK_WriteData == NULL) {
        qDebug() << "TKIOSK_WriteData error!";
        return FAIL;
    }

    Qt_TKIOSK_ReadData= (TKIOSK_ReadData)loadlib->resolve("TKIOSK_ReadData");
    if (Qt_TKIOSK_ReadData == NULL) {
        qDebug() << "TKIOSK_WriteData error!";
        return FAIL;
    }

    //close serial port
    Qt_TKIOSK_CloseCom = (TKIOSK_CloseCom)loadlib->resolve("TKIOSK_CloseCom");
    if (Qt_TKIOSK_CloseCom == NULL) {
        qDebug() << "TKIOSK_CloseCom error!";
        return FAIL;
    }

    Qt_TKIOSK_StartDoc = (TKIOSK_StartDoc)loadlib->resolve("TKIOSK_StartDoc");
    if (Qt_TKIOSK_StartDoc == NULL) {
        qDebug() << "TKIOSK_CloseCom error!";
        return FAIL;
    }

    Qt_TKIOSK_BeginSaveToFile = (TKIOSK_BeginSaveToFile)loadlib->resolve("TKIOSK_BeginSaveToFile");
    if (Qt_TKIOSK_BeginSaveToFile == NULL) {
        qDebug() << "TKIOSK_CloseCom error!";
        return FAIL;
    }

    Qt_TKIOSK_Reset = (TKIOSK_Reset)loadlib->resolve("TKIOSK_Reset");
    if (Qt_TKIOSK_Reset == NULL) {
        qDebug() << "Qt_TKIOSK_Reset error!";
        return FAIL;
    }

    Qt_TKIOSK_SetMode = (TKIOSK_SetMode)loadlib->resolve("TKIOSK_SetMode");
    if (Qt_TKIOSK_SetMode == NULL) {
        qDebug() << "Qt_TKIOSK_SetMode error!";
        return FAIL;
    }

    Qt_TKIOSK_SetLineSpacing = (TKIOSK_SetLineSpacing)loadlib->resolve("TKIOSK_SetLineSpacing");
    if (Qt_TKIOSK_SetLineSpacing == NULL) {
        qDebug() << "Qt_TKIOSK_SetMode error!";
        return FAIL;
    }

    Qt_TKIOSK_S_SetLeftMarginAndAreaWidth = (TKIOSK_S_SetLeftMarginAndAreaWidth)loadlib->resolve("TKIOSK_S_SetLeftMarginAndAreaWidth");
    if (Qt_TKIOSK_S_SetLeftMarginAndAreaWidth == NULL) {
        qDebug() << "Qt_TKIOSK_S_SetLeftMarginAndAreaWidth error!";
        return FAIL;
    }

    Qt_TKIOSK_SetRightSpacing = (TKIOSK_SetRightSpacing)loadlib->resolve("TKIOSK_SetRightSpacing");
    if (Qt_TKIOSK_SetRightSpacing == NULL) {
        qDebug() << "TKIOSK_SetRightSpacing error!";
        return FAIL;
    }

    Qt_TKIOSK_S_SetAlignMode = (TKIOSK_S_SetAlignMode)loadlib->resolve("TKIOSK_S_SetAlignMode");
    if (Qt_TKIOSK_S_SetAlignMode == NULL) {
        qDebug() << "Qt_TKIOSK_S_SetAlignMode error!";
        return FAIL;
    }

    Qt_TKIOSK_S_Textout = (TKIOSK_S_Textout)loadlib->resolve("TKIOSK_S_Textout");
    if (Qt_TKIOSK_S_Textout == NULL) {
        qDebug() << "Qt_TKIOSK_S_Textout error!";
        return FAIL;
    }

    Qt_TKIOSK_CutPaper = (TKIOSK_CutPaper)loadlib->resolve("TKIOSK_CutPaper");
    if (Qt_TKIOSK_CutPaper == NULL) {
        qDebug() << "TKIOSK_CutPaper error!";
        return FAIL;
    }

    Qt_TKIOSK_EndSaveToFile = (TKIOSK_EndSaveToFile)loadlib->resolve("TKIOSK_EndSaveToFile");
    if (Qt_TKIOSK_EndSaveToFile == NULL) {
        qDebug() << "TKIOSK_EndSaveToFile error!";
        return FAIL;
    }

    Qt_TKIOSK_EndDoc = (TKIOSK_EndDoc)loadlib->resolve("TKIOSK_EndDoc");
    if (Qt_TKIOSK_EndDoc == NULL) {
        qDebug() << "Qt_TKIOSK_EndDoc error!";
        return FAIL;
    }

    Qt_TKIOSK_OpenDrv = (TKIOSK_OpenDrv)loadlib->resolve("TKIOSK_OpenDrv");
    if (Qt_TKIOSK_OpenDrv == NULL) {
        qDebug() << "Qt_TKIOSK_OpenDrv error!";
        return FAIL;
    }

    Qt_TKIOSK_SetChineseFont = (TKIOSK_SetChineseFont)loadlib->resolve("TKIOSK_SetChineseFont");
    if (Qt_TKIOSK_SetChineseFont == NULL) {
        qDebug() << "Qt_TKIOSK_SetChineseFont error!";
        return FAIL;
    }
    Qt_TKIOSK_S_TestPrint = (TKIOSK_S_TestPrint)loadlib->resolve("TKIOSK_S_TestPrint");
    if (Qt_TKIOSK_S_TestPrint == NULL) {
        qDebug() << "Qt_TKIOSK_S_TestPrint error!";
        return FAIL;
    }

    Qt_TKIOSK_FeedLine =(TKIOSK_FeedLine)loadlib->resolve("TKIOSK_FeedLine");
    if (Qt_TKIOSK_FeedLine == NULL) {
        qDebug() << "Qt_TKIOSK_FeedLine error!";
        return FAIL;
    }

    Qt_TKIOSK_S_SetAlignMode = (TKIOSK_S_SetAlignMode)loadlib->resolve("TKIOSK_S_SetAlignMode");
    if (Qt_TKIOSK_S_SetAlignMode == NULL) {
        qDebug() << "Qt_TKIOSK_S_SetAlignMode error!";
        return FAIL;
    }

    Qt_TKIOSK_S_PrintBarcode = (TKIOSK_S_PrintBarcode)loadlib->resolve("TKIOSK_S_PrintBarcode");
    if (Qt_TKIOSK_S_PrintBarcode == NULL) {
        qDebug() << "Qt_TKIOSK_S_PrintBarcode error!";
        return FAIL;
    }

    Qt_TKIOSK_PreDownloadBmpToRAM = (TKIOSK_PreDownloadBmpToRAM)loadlib->resolve("TKIOSK_PreDownloadBmpToRAM");
    if (Qt_TKIOSK_PreDownloadBmpToRAM == NULL) {
        qDebug() << "Qt_TKIOSK_PreDownloadBmpToRAM error!";
        return FAIL;
    }

    Qt_TKIOSK_S_PrintBmpInRAM = (TKIOSK_S_PrintBmpInRAM)loadlib->resolve("TKIOSK_S_PrintBmpInRAM");
    if (Qt_TKIOSK_S_PrintBmpInRAM == NULL) {
        qDebug() << "Qt_TKIOSK_S_PrintBmpInRAM error!";
        return FAIL;
    }

    Qt_TKIOSK_FeedLines = (TKIOSK_FeedLines)loadlib->resolve("TKIOSK_FeedLines");
    if (Qt_TKIOSK_FeedLines == NULL) {
        qDebug() << "Qt_TKIOSK_FeedLines error!";
        return FAIL;
    }
    return SUCCESS;
}

void zzprinter::printtest()
{
    int result;
    hPort = Qt_TKIOSK_OpenCom("COM1",19200,8,1,0x00,0x00);
    if(hPort == INVALID_HANDLE_VALUE)
    {
        qDebug()<<"open com fail";
        return;
    }
    qDebug()<<"hport "<<hPort;



    result =  Qt_TKIOSK_SetComTimeOuts(hPort, 3000, 3000, 90000, 90000);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"TKIOSK_SetComTimeOuts fail"<< result;
        return;
    }

    result = Qt_TKIOSK_SetMode(hPort, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_SetMode fail"<< result;
        return;
    }

//    result = Qt_TKIOSK_S_SetLeftMarginAndAreaWidth(hPort, 0x00, 50, 500);
//    if(result != TKIOSK_SUCCESS)
//    {
//        qDebug()<<"Qt_TKIOSK_S_SetLeftMarginAndAreaWidth fail"<< result;
//        return;
//    }

    Qt_TKIOSK_S_SetAlignMode(hPort,0x00,0x00);

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "abcdefghigklmnopqrstuvwxyz\n", 20, 1, 1, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }
    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "abcdefghigklmnopqrstuvwxyz\n", 20, 1, 1, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    Qt_TKIOSK_S_SetAlignMode(hPort,0x00,0x01);

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20, 1, 1, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }
    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20, 1, 1, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    char printBuf[1024];
    QString printbuf_utf9 = "春风吹碧，春云映绿，晓梦入芳裀。\n";
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    memset(printBuf,0x00,sizeof(printBuf));
    sprintf(printBuf,"%s",printbuf_utf9.toLocal8Bit().data());


    Qt_TKIOSK_S_SetAlignMode(hPort,0x00,0x02);

    result = Qt_TKIOSK_S_Textout(hPort, 0x00,printBuf, 20, 1, 1, 0x03, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    result = Qt_TKIOSK_S_Textout(hPort, 0x00,printBuf, 20, 1, 1, 0x03, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));


    result = Qt_TKIOSK_S_PrintBarcode(hPort,0x00,"123456789012",10,0x41,5,162,0x00,0x01,12);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_PrintBarcode fail"<< result;
        return;
    }



    Qt_TKIOSK_S_SetAlignMode(hPort,0x00,0x00);

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20, 1, 1, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20, 2, 2, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20, 3, 3, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20,4, 4, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }
    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20,5, 5, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20,6, 6, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20,1, 2, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    result = Qt_TKIOSK_S_Textout(hPort, 0x00, "1234567890\n", 20,2, 1, 0x00, 0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return;
    }

    result = Qt_TKIOSK_PreDownloadBmpToRAM(hPort,0x00,"1.bmp",1,1);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_PreDownloadBmpToRAM fail"<< result;
        return;
    }

    result = Qt_TKIOSK_S_PrintBmpInRAM(hPort,0x00,1,10,0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_PrintBmpInRAM fail"<< result;
        return;
    }

    for(int i = 0 ; i< 30;i++){
        result = Qt_TKIOSK_FeedLine(hPort, 0x00);
        if(result != TKIOSK_SUCCESS)
        {
            qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
            return;
        }
    }


    result = Qt_TKIOSK_CutPaper(hPort, 0x00, 0, 100);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_CutPaper fail"<< result;
        return;
    }


    result = Qt_TKIOSK_CloseCom(hPort);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_CloseCom fail"<< result;
        return;
    }
}

/*
* printBufUtf8:要打印的字符串
* attrib:1：正常显示, 2:2倍大小, 3:长1倍 宽2倍, 4:长2倍 宽1倍
* align: 0：左对齐, 1：居中对齐, 2：右对齐
* fontStyle:0x00 正常 0x08黑体
*/
int zzprinter::Qt_Print_Str(QString printBufUtf8, int attrib, int align, int fontStyle)
{
    int result;
    char printBuf[2048];
    int width;
    int height;

    if(attrib == 1)
    {
        width = 1;
        height = 1;
    }
    else if(attrib == 2)
    {
        width = 2;
        height = 2;
    }
    else if(attrib == 3)
    {
        width = 1;
        height = 2;
    }
    else if(attrib == 4)
    {
        width = 2;
        height = 1;
    }


    /*设置对齐方式*/
    if(align == 0)
        Qt_TKIOSK_S_SetAlignMode(hPort,0x00,0x00);
    else if(align == 1)
        Qt_TKIOSK_S_SetAlignMode(hPort,0x00,0x01);
    else if(align == 2)
        Qt_TKIOSK_S_SetAlignMode(hPort,0x00,0x02);



    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    memset(printBuf,0x00,sizeof(printBuf));
    sprintf(printBuf,"%s\n",printBufUtf8.toLocal8Bit().data());


    result = Qt_TKIOSK_S_Textout(hPort, 0x00,printBuf, 60, width, height, 0x03, fontStyle);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return FAIL;
    }
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    return SUCCESS;
}


/*
 * 功能：打印图片
 * 参数：
 * filePathAndName:文件的绝对或者相对路径+名字
 * leftOffset：图片距离左边距多少个点
 * 返回值： 0:成功, -1失败。
*/
int zzprinter::Qt_Print_Bmp(char *filePathAndName, int leftOffset)
{
    int result;
    int fileID = 1;

    result = Qt_TKIOSK_Reset(hPort,0x00);

    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_PreDownloadBmpToRAM fail"<< result;
        return FAIL;
    }

    result = Qt_TKIOSK_PreDownloadBmpToRAM(hPort,0x00,filePathAndName,1,fileID);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_PreDownloadBmpToRAM fail"<< result;
        return FAIL;
    }

    result = Qt_TKIOSK_S_PrintBmpInRAM(hPort,0x00,fileID,leftOffset,0x00);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_PrintBmpInRAM fail"<< result;
        return FAIL;
    }

    return SUCCESS;
}

int zzprinter::Qt_initAndOpenPrinter(char * com)
{
    int result;
    hPort = Qt_TKIOSK_OpenCom(com,19200,8,1,0x00,0x00);
    if(hPort == INVALID_HANDLE_VALUE)
    {
        qDebug()<<"open com fail";
        return FAIL;
    }

    result =  Qt_TKIOSK_SetComTimeOuts(hPort, 3000, 3000, 90000, 90000);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"TKIOSK_SetComTimeOuts fail"<< result;
        return FAIL;
    }

    return SUCCESS;
}

int zzprinter::Qt_ClosePrinter()
{
    int result;
    result = Qt_TKIOSK_CloseCom(hPort);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_CloseCom fail"<< result;
        return FAIL;
    }

    return SUCCESS;
}

int zzprinter::Qt_Printer_FeedLines(int lines)
{
    int result;
    result = Qt_TKIOSK_FeedLines(hPort, 0x00, lines);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_FeedLines fail"<< result;
        return FAIL;
    }

    return SUCCESS;
}


int zzprinter::Qt_Printer_CutPaper()
{
    int result;
    result = Qt_TKIOSK_CutPaper(hPort, 0x00, 0, 100);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_CutPaper fail"<< result;
        return FAIL;
    }
    return SUCCESS;
}

int zzprinter::Qt_Print_BarCode(char *printstr)
{
    int result;
    char printBuf[1024];

    memset(printBuf,0x00,sizeof(printBuf));
    sprintf(printBuf,"%s\n",printstr);

    result = Qt_TKIOSK_S_PrintBarcode(hPort,0x00,printBuf,10,0x41,5,162,0x00,0x01,12);
    if(result != TKIOSK_SUCCESS)
    {
        qDebug()<<"Qt_TKIOSK_S_Textout fail"<< result;
        return FAIL;
    }

    return SUCCESS;
}
