#ifndef ZZPRINTER_H
#define ZZPRINTER_H
#include <windows.h>
#include <QString>
#include <QLibrary>


#define SUCCESS 0
#define FAIL -1
#define TKIOSK_SUCCESS 1001


class zzprinter
{
public:
    zzprinter();

    /*
     * 初始化打印机动态库
    *返回值：0:成功, -1:失败
    */
    int Qt_loadPrinterDll();


    /*
     * 功能：打印字符串
     * 参数
     * printBufUtf8:要打印的字符串
     * attrib:1：正常显示, 2:2倍大小, 3:长1倍 宽2倍, 4:长2倍 宽1倍
     * align: 0：左对齐, 1：居中对齐, 2：右对齐
     * fontStyle:0x00 正常 0x08黑体
     * 返回值:0:成功, -1:失败
     */
    int Qt_Print_Str(QString printBufUtf8 , int attrib = 1, int align =0,int fontStyle = 0x00);


    /*
     * 功能：打印图片
     * 参数：
     * filePathAndName:文件的绝对或者相对路径+名字
     * leftOffset：图片距离左边距多少个点
     * 返回值： 0:成功, -1失败。
    */
    int Qt_Print_Bmp(char * filePathAndName, int leftOffset = 0);


    /*
    * 功能：打开打印机
    * 返回值:0:成功, -1:失败
    */
    int Qt_initAndOpenPrinter(char * com);

    /*
     * 功能:关闭打印机
     * 返回值: 0:成功, -1:失败
    */
    int Qt_ClosePrinter();

    /*
     *功能：打印空行
     *返回值: 0:成功, -1:失败
    */
    int Qt_Printer_FeedLines(int lines = 1);

    /*功能:切纸
     * 返回值: 0:成功, -1:失败
    */
    int Qt_Printer_CutPaper();

    /*
     * 功能:打印一维码
    */
    int Qt_Print_BarCode(char * printstr);


    static zzprinter * getInstance()
    {
       if(zprinter == NULL)
       {
          zprinter = new zzprinter();
       }

       return zprinter;
    }


private:
    int initFuncs();
    void printtest();


private:

    typedef HANDLE(__stdcall *TKIOSK_OpenCom)(char* lpName,int nComBaudrate,int nComDataBits,int nComStopBits,int nComParity,int nFlowControl);
    typedef int(__stdcall *TKIOSK_SetComTimeOuts)(HANDLE hPort, int nWriteTimeoutMul, int nWriteTimeoutCon, int nReadTimeoutMul, int nReadTimeoutCon);
    typedef int(__stdcall *TKIOSK_WriteData) (HANDLE hPort,int nPortType, char *pszData, int nBytesToWrite);
    typedef int (__stdcall *TKIOSK_ReadData)(HANDLE hPort, int nPortType, int nStatus, char *pszBuffer, int nBytesToRead);
    typedef int(__stdcall *TKIOSK_CloseCom) (HANDLE hPort);
    typedef int(__stdcall *TKIOSK_StartDoc) (HANDLE hPort);
    typedef int (__stdcall *TKIOSK_BeginSaveToFile)(HANDLE hPort, char * lpFileName, BOOL bToPrinter);
    typedef  int (__stdcall *TKIOSK_Reset)(HANDLE hPort, int nPortType);
    typedef int (__stdcall *TKIOSK_SetMode)(HANDLE hPort, int nPortType, int nPrintMode);
    typedef int (__stdcall *TKIOSK_S_SetLeftMarginAndAreaWidth)(HANDLE hPort, int nPortType, int nDistance, int nWidth);
    typedef int (__stdcall *TKIOSK_SetLineSpacing)(HANDLE hPort, int nPortType, int nDistance);
    typedef int (__stdcall *TKIOSK_SetRightSpacing) (HANDLE hPort, int nPortType, int nDistance);
    typedef int(__stdcall *TKIOSK_S_SetAlignMode) (HANDLE hPort, int nPortType, int nMode);
    typedef int (__stdcall *TKIOSK_S_Textout)(HANDLE hPort, int nPortType, char * pszData, int nOrgx, int nWidthTimes, int nHeightTimes, int nFontType, int nFontStyle);
    typedef int (__stdcall * TKIOSK_CutPaper)(HANDLE hPort, int nPortType, int nMode, int nDistance);
    typedef int (__stdcall * TKIOSK_EndSaveToFile)(HANDLE hPort);
    typedef int (__stdcall *TKIOSK_EndDoc)(HANDLE hPort);
    typedef HANDLE (__stdcall *TKIOSK_OpenDrv)(char *Drivername);
    typedef int (__stdcall *TKIOSK_SetChineseFont)(HANDLE hPort, int nPortType, char * pszBuffer, int nEnable, int nBigger, int nLSpacing, int nRSpacing, int nUnderLine);
    typedef int (__stdcall *TKIOSK_S_TestPrint)(HANDLE hPort, int nPortType);
    typedef int (__stdcall *TKIOSK_FeedLine)(HANDLE hPort, int nPortType);
    typedef int(__stdcall * TKIOSK_FeedLines)(HANDLE hPort, int nPortType, int nLines);

    typedef int (__stdcall *TKIOSK_S_PrintBarcode)(HANDLE hPort, int nPortType, char *pszBuffer, int nOrgx, int nType, int nWidth,int nHeight, int nHriFontType, int nHriFontPosition, int nBytesOfBuffer);

    typedef int (__stdcall *TKIOSK_PreDownloadBmpToRAM)(HANDLE hPort, int nPortType, char *pszPaths, int nTranslateMode, int nID);
    typedef int (__stdcall *TKIOSK_S_PrintBmpInRAM)(HANDLE hPort, int nPortType, int nID, int nOrgx, int nMode);

private:

    /*
    lpName：[in] 串口名称；如'COM1'，COM2'等等。
    nComBaudrate：[in] 波特率；如2400，9600，19200，38400，56000，57600，115200。目前仅支持上述值。
    nComDataBits：[in] 数据位；7或 8。
    nComStopBits：[in] 停止位；可以取如下值之一：
    */
    TKIOSK_OpenCom Qt_TKIOSK_OpenCom;

    /*
    设置超时
    nWriteTimeoutMul：[in] 写超时系数，单位：毫秒，nWriteTimeoutMul >= 0。
    nWriteTimeoutCon：[in] 写超时常数，单位：毫秒，nWriteTimeoutCon >= 0。
    nReadTimeoutMul：[in] 读超时系数，单位：毫秒，nReadTimeoutMul >= 0。
    nReadTimeoutCon：[in] 读超时常数， 单位：毫秒，nReadTimeoutCon >= 0。
    */
    TKIOSK_SetComTimeOuts Qt_TKIOSK_SetComTimeOuts;


    /*
     * 向端口或者文件些数据
    */
    TKIOSK_WriteData Qt_TKIOSK_WriteData;


    /*
     *  读打印机数据
    */
    TKIOSK_ReadData Qt_TKIOSK_ReadData;


    /*
     * 关闭打印机
    */
    TKIOSK_CloseCom Qt_TKIOSK_CloseCom;

    /*
     * 开启新的打印作业
    */
    TKIOSK_StartDoc Qt_TKIOSK_StartDoc;


    TKIOSK_BeginSaveToFile Qt_TKIOSK_BeginSaveToFile;


    /*
    * 复位打印机
    */
    TKIOSK_Reset Qt_TKIOSK_Reset;

    /*设置打印模式*/
    TKIOSK_SetMode Qt_TKIOSK_SetMode;

    /*设置字符行高*/
    TKIOSK_SetLineSpacing Qt_TKIOSK_SetLineSpacing;

    /*设置左边距*/
    TKIOSK_S_SetLeftMarginAndAreaWidth Qt_TKIOSK_S_SetLeftMarginAndAreaWidth;

    /*设置右边距*/
    TKIOSK_SetRightSpacing Qt_TKIOSK_SetRightSpacing;

    /*在标准模式下，设置字符对齐模式*/
    TKIOSK_S_SetAlignMode Qt_TKIOSK_S_SetAlignMode;

    /*打印文本*/
    TKIOSK_S_Textout Qt_TKIOSK_S_Textout;

    /*切纸*/
    TKIOSK_CutPaper Qt_TKIOSK_CutPaper;

    /*结束保存数据到指定文件*/
    TKIOSK_EndSaveToFile Qt_TKIOSK_EndSaveToFile;

    /*结束打印作业*/
    TKIOSK_EndDoc Qt_TKIOSK_EndDoc;

    /*打开驱动接口*/
    TKIOSK_OpenDrv Qt_TKIOSK_OpenDrv;

    //设置汉字模式
    TKIOSK_SetChineseFont Qt_TKIOSK_SetChineseFont;

    /*测试页*/
    TKIOSK_S_TestPrint Qt_TKIOSK_S_TestPrint;

    /*打印空行*/
    TKIOSK_FeedLine Qt_TKIOSK_FeedLine;
    TKIOSK_FeedLines Qt_TKIOSK_FeedLines;
    TKIOSK_S_PrintBarcode Qt_TKIOSK_S_PrintBarcode;


    TKIOSK_PreDownloadBmpToRAM Qt_TKIOSK_PreDownloadBmpToRAM;

    TKIOSK_S_PrintBmpInRAM Qt_TKIOSK_S_PrintBmpInRAM;


private:
    QString dllPath;
    QLibrary * loadlib;
    HANDLE  hPort;

      static zzprinter * zprinter;

};

#endif // ZZPRINTER_H
