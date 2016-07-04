#ifndef ZZKEYPAD_H
#define ZZKEYPAD_H
#include <windows.h>
#include <QString>
#include <QDebug>
#include <QLibrary>
#define SUCCESS 0
#define FAIL -1

class zzkeypad
{
public:
    zzkeypad();

    /*
     *功能:加载keypad对应的动态库
     * 返回值: 0：成功。 -1：失败
     *
     */

    int Qt_loadKeypadDll();


    /*
     * 功能:初始化keypad
     * 返回值: 0：成功。 -1：失败
     */
    int Qt_initAndOpenKeyPad(int comNo);

    /*
     *功能：获取按键值
     * 参数:ch 读到的按键值
     * 返回值: 0：成功。 -1：失败
     */
    int Qt_getInputKey( char *ch);

    /*
     * 功能:关闭keypad
     *返回值: 0：成功。 -1：失败
     */
    int Qt_CloseKeyPad();




public:

//    static zzkeypad * getInstance()
//     {
//        if(zkeypad == NULL)
//        {
//           zkeypad = new zzkeypad();
//        }

//        return zkeypad;
//     }

private:
    int initFuncs();

public:
    typedef int (__stdcall * SUNSON_SetAlgorithmParameter)(unsigned char ucPPara,unsigned char ucFPara,unsigned char *ReturnInfo);

   //1.3.1 打开串口，关闭串口，复位密码键盘
   typedef int (__stdcall *SUNSON_OpenCom)(int nPort, long nBaud);
  typedef int (__stdcall *SUNSON_ResetEPP)(unsigned char *result);

   //1.3.2 打开密码键盘,获取按下的键值
   typedef int(__stdcall *SUNSON_UsePlainKeyboard)(unsigned char para,unsigned char *result);
   typedef int (__stdcall *SUNSON_ScanKeyPress)(unsigned char *ucValue);

   //1.3.3 下载主密钥，工作密钥
   typedef int (__stdcall *SUNSON_LoadMasterKey)(unsigned char MasterKeyId,unsigned char len,unsigned char* MasterKey,unsigned char *result);

   typedef int (__stdcall * SUNSON_LoadWorkKey)( unsigned char  MasterKeyId, unsigned char WorkKeyId,unsigned char len,unsigned char* WorkKeyCiphertext,unsigned char *result);
   //设定下载密钥的密码算法
   typedef int (__stdcall *SUNSON_SetEncryMode)(unsigned char mode,unsigned char *result);
   //激活工作密钥
   typedef int (__stdcall * SUNSON_ActiveKey)( unsigned char MasterKeyId, unsigned char WorkKeyId,unsigned char *result);
    //1.3.4 键盘算法参数设置
    //设置加密密码算法
    typedef int  (__stdcall *SUNSON_SetEncryAlgorithm)(unsigned char SuanFa,unsigned char *result);
    //设置PIN右补偿方式
    typedef int (__stdcall * SUNSON_FillPinMode)(unsigned char fillpara,unsigned char *result);
    typedef int (__stdcall * SUNSON_SetFunctionKeys)(unsigned char para,unsigned char *result);
    //设置PIN处理方式
    typedef int (__stdcall * SUNSON_SetPinFormat)(unsigned char para,unsigned char *result);


    //1.3.5使用密码键盘完成PINBLOCK运算
    //下载卡号/账号
    typedef int (__stdcall * SUNSON_LoadCardNumber)(unsigned char* CardNo,unsigned char *result);
    //启动密码键盘加密
    typedef int (__stdcall * SUNSON_StartEpp)(unsigned char PinLen,unsigned char JM_mode,unsigned char time_out,unsigned char *result);
    typedef int (__stdcall * SUNSON_ReadCypherPin)(unsigned char *ReturnInfo);
    //取键盘中PIN运算后的密文结果
    typedef int (__stdcall * SUNSON_GetPin)(unsigned char *result);


    //1.3.7关闭串口
    typedef int (__stdcall *SUNSON_CloseCom)(void);

    //1.3.8
    //设置mac算法
    typedef int  (__stdcall *SUNSON_SetMacAlgorithm)(unsigned char Macmode,unsigned char *result);
    //mac运算
    typedef int (__stdcall * SUNSON_MacOperation)(unsigned char len,unsigned char* macdata,unsigned char *result);

    //1.3.9加解密运算
    //加密运算
    typedef int (__stdcall * SUNSON_DataEncrypt)(unsigned char len,unsigned char* sourcedata,unsigned char *result);
    //数据解密运算
    typedef int (__stdcall * SUNSON_DataDecrypt)(unsigned  char len,unsigned char* sourcedata,unsigned char *result);

    typedef int (__stdcall * SUNSON_MakeBaseMac)(int nMacDataLen,unsigned char* ucMacData,unsigned char *hexReturnInfo);
    //其他
    typedef int (__stdcall *SUNSON_SendAscII)(unsigned char Ascii,unsigned char *result);
   typedef int (__stdcall *SUNSON_GetVersion)(unsigned char *version);
   typedef int (__stdcall *SUNSON_GetSerialNumber)(unsigned char *SerialNumber);


    typedef int (__stdcall * SUNSON_UseEppPlainTextMode)(
            unsigned char ucTextModeFormat,
            unsigned char *ReturnInfo);

public:

    // 1.打开串口
    SUNSON_OpenCom Qt_SUNSON_OpenCom;

    //2.获取按下的按键键值
    SUNSON_ScanKeyPress Qt_SUNSON_ScanKeyPress;

    //3.关闭串口
    SUNSON_CloseCom Qt_SUNSON_CloseCom;


    //4.键盘复位自检，所有密钥复位
    SUNSON_ResetEPP Qt_SUNSON_ResetEPP;

    //5.读取版本号
    SUNSON_GetVersion Qt_SUNSON_GetVersion;


    //6.回送字符
    SUNSON_SendAscII Qt_SUNSON_SendAscII;

    //7.下载工作密钥
    SUNSON_LoadWorkKey Qt_SUNSON_LoadWorkKey;

    //8.下载主密钥
    SUNSON_LoadMasterKey Qt_SUNSON_LoadMasterKey;

    //9.取键盘序列号
    SUNSON_GetSerialNumber Qt_SUNSON_GetSerialNumber;


    //10.激活工作密钥
    SUNSON_ActiveKey Qt_SUNSON_ActiveKey;

    //11.加密
    SUNSON_DataEncrypt Qt_SUNSON_DataEncrypt;

    //12.解密
    SUNSON_DataDecrypt Qt_SUNSON_DataDecrypt;

    //13.下载卡号账号
    SUNSON_LoadCardNumber  Qt_SUNSON_LoadCardNumber;

    //14.启动密码键盘加密
    SUNSON_StartEpp Qt_SUNSON_StartEpp;

    //获取加密结果
    SUNSON_ReadCypherPin Qt_SUNSON_ReadCypherPin;

    SUNSON_MakeBaseMac Qt_SUNSON_MakeBaseMac;


    //15.设置算法处理参数
    SUNSON_SetAlgorithmParameter  Qt_SUNSON_SetAlgorithmParameter;

    //16.开关键盘和按键声音
    SUNSON_UseEppPlainTextMode  Qt_SUNSON_UseEppPlainTextMode;


    QString dllPath;
    QLibrary *loadlib;

private:

    //设置PIN右补偿方式
//    SUNSON_FillPinMode Qt_SUNSON_FillPinMode;

    //设置回车键自动推送 lyx  2016.06.12
//    SUNSON_SetFunctionKeys Qt_SUNSON_SetFunctionKeys;

    //设置PIN处理方式
//    SUNSON_SetPinFormat Qt_SUNSON_SetPinFormat;

//    取键盘中密码(PIN加密后的结果)
//    SUNSON_GetPin Qt_SUNSON_GetPin;



private:
//    static zzkeypad * zkeypad;

public:
    bool exitWaitFlag;
    bool isExitLoop;
};

#endif // ZZKEYPAD_H
