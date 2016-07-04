#ifndef CARDREADER_H
#define CARDREADER_H
#include <QString>
#include <QLibrary>
#include <windows.h>
#include <QLibrary>
#define ERR		-1
#define OK		0

#define ERRCARD	2
#define ERRRDCARD	3
#define ERRWRCARD	4
#define ERRCARDSEC	5
#define ERRCARDKEY	6
#define ERRCARDLOCKED 7
#define ERRMSG 8
#define ERRRFCARD	9
#define ERRFORMAT	10
#define ERROVERFLOW	11

#define NOCARD	1
#define UNKNOWCARD	12
#define ERRCARDPOSITION	14


#define PAC_ADDRESS	1021

#define ENQ  0x05//请求连接通信线路(询问).
#define ACK  0x06//确认(握手).
#define NAK  0x15//通信忙.
#define EOT  0x04//通信结束(传送结束).
#define CAN  0x18//解除通信(取消).
#define STX  0x02//数据包起始符(起始字符).
#define ETX  0x03//数据包结束符(终结符).
#define US   0x1F//数据分隔符.


#define SUCCESS 0
#define FAIL -1

class CardReader
{

public:
    CardReader();

    /*
     * 功能:加载动态库并初始化设备
     * 返回值：0：成功， -1:失败
     */
    int Qt_loadCardReaderDll();

    /*
     *功能：弹出卡片
     *返回值: 0：成功， -1:失败
     */
    int Qt_pushOutCard();


    /*
     * 功能：读取卡片信息
     * 参数：
     * track1:第一磁道数据
     * track2:第二磁道数据
     * trakc3:第三磁道数据
     * cardNo:卡号
     *返回值: 0：成功， -1:失败
     */
    int Qt_GetTrack(unsigned char * track1,unsigned char * track2,unsigned char * track3,unsigned char * cardNo);

    /*
     * 功能：初始化并打开读卡器
     *
     * 返回值： 0：成功， -1:失败
     */
    int Qt_initAndOpenCardReader(char *deviceName);


    int Qt_CardReader_Close();


    static CardReader * getInstance()
    {
       if(cardreader == NULL)
       {
          cardreader = new CardReader();
       }

       return cardreader;
    }


private:
    int getCardStatusIn();
    int initFuncs();


public:
    typedef HANDLE  (__stdcall *CommOpen)(char *Port);
    typedef HANDLE (__stdcall *CommOpenWithBaut)(char *Port, unsigned int _data);
    typedef int (__stdcall * CommClose)(HANDLE ComHandle);
    typedef int (__stdcall *  CommSetting)(HANDLE ComHandle,char *ComSeting);
    typedef int (__stdcall * CRT_R_SetComm)(HANDLE ComHandle, unsigned int _data);

    typedef int (__stdcall * MC_ReadTrack)(HANDLE ComHandle, BYTE _mode, BYTE _track, BYTE _BlockData[]);
     typedef int (__stdcall  * CRT310_ReadSnr)(HANDLE ComHandle, BYTE _SNData[], BYTE *_dataLen);
    typedef int APIENTRY (__stdcall * CRT310_WriteSnr)(HANDLE ComHandle, BYTE _SNData[], BYTE _dataLen);

    typedef int (__stdcall * CRT310_Reset)(HANDLE ComHandle, BYTE _Eject);
    typedef int (__stdcall * CRT_R_SetCommForVB)(HANDLE ComHandle, BYTE _Bauddata);
    typedef int (__stdcall * CRT310_CardSetting)(HANDLE ComHandle,BYTE _CardIn, BYTE _EnableBackIn);
    typedef int (__stdcall * CRT310_CardPosition)(HANDLE ComHandle, BYTE _Position);
    typedef int (__stdcall * CRT310_GetStatus)(HANDLE ComHandle, BYTE *_atPosition, BYTE *_frontSetting, BYTE *_rearSetting);
    typedef  int (__stdcall * CRT310_MovePosition)(HANDLE ComHandle, BYTE _Position);
    typedef int (__stdcall * CRT310_SensorStatusV3)(HANDLE ComHandle, BYTE *_PSS0,BYTE *_PSS1, BYTE *_PSS2, BYTE *_PSS3, BYTE *_PSS4, BYTE *_PSS5, BYTE *_CTSW, BYTE *_KSW);

private:

    CommOpen Qt_CommOpen;
    CommOpenWithBaut Qt_CommOpenWithBaut;
    CommClose Qt_CommClose;
    CommSetting Qt_CommSetting;
    CRT_R_SetComm  Qt_CRT_R_SetComm;
    CRT310_WriteSnr  Qt_CRT310_WriteSnr;
    CRT310_ReadSnr  Qt_CRT310_ReadSnr;

    CRT310_Reset Qt_CRT310_Reset;//复位读卡机
    CRT310_CardSetting Qt_CRT310_CardSetting;//进卡控制设置
    CRT310_CardPosition Qt_CRT310_CardPosition; //停卡位置设置
    CRT310_GetStatus Qt_CRT310_GetStatus;//从读卡机读取状态信息:卡机内有长卡,卡机内有短卡
    CRT310_MovePosition Qt_CRT310_MovePosition; //走卡位置设置
    CRT310_SensorStatusV3 Qt_CRT310_SensorStatusV3;
    MC_ReadTrack  Qt_MC_ReadTrack;
    CRT_R_SetCommForVB Qt_CRT_R_SetCommForVB;

private:
    QString dllPath;
    QLibrary *loadlib;

    HANDLE ComHandle; //读卡器的句柄

    static CardReader * cardreader;

    bool DllInitFlag; //动态库初始化标志位
};

#endif // CARDREADER_H
