#include "zzkeypad.h"
#include <QTime>
#include <QCoreApplication>


//zzkeypad * zzkeypad::zkeypad = NULL;

zzkeypad::zzkeypad()
{
    //一直是false的
    exitWaitFlag = false;
}


int zzkeypad::Qt_loadKeypadDll()
{
    dllPath = "keypadlib\\XZ_F10_API.dll";

    /*加载动态库*/
    loadlib = new QLibrary(dllPath);

    if(loadlib->load()) {
          qDebug() << "load  KeyPad dll ok!";
      } else {
          qDebug() << "load error!";
          return FAIL;
      }

    return initFuncs();
}


//   Qt_SUNSON_FillPinMode = (SUNSON_FillPinMode)loadlib->resolve("SUNSON_FillPinMode");
//   if (Qt_SUNSON_FillPinMode == NULL) {
//       qDebug() << "Qt_SUNSON_FillPinMode eror!";
//       return FAIL;
//   }

//   Qt_SUNSON_SetFunctionKeys = (SUNSON_SetFunctionKeys)loadlib->resolve("SUNSON_SetFunctionKeys");
//   if (Qt_SUNSON_SetFunctionKeys == NULL) {
//       qDebug() << "Qt_SUNSON_SetFunctionKeys eror!";
//       return FAIL;
//   }

//    Qt_SUNSON_SetPinFormat = (SUNSON_SetPinFormat)loadlib->resolve("SUNSON_SetPinFormat");
//    if (Qt_SUNSON_SetPinFormat == NULL) {
//        qDebug() << "Qt_SUNSON_SetPinFormat eror!";
//        return FAIL;
//    }
//    Qt_SUNSON_GetPin =(SUNSON_GetPin)loadlib->resolve("SUNSON_GetPin");
//    if (Qt_SUNSON_GetPin == NULL) {
//        qDebug() << "Qt_SUNSON_GetPin eror!";
//        return FAIL;
//    }



int zzkeypad::initFuncs()
{


    //1 打开串口
    Qt_SUNSON_OpenCom = (SUNSON_OpenCom)loadlib->resolve("SUNSON_OpenCom");
    if (Qt_SUNSON_OpenCom == NULL) {
        qDebug() << "Qt_SUNSON_OpenCom eror!";
        return FAIL;
    }

    //2.获取按下的按键键值
    Qt_SUNSON_ScanKeyPress = (SUNSON_ScanKeyPress)loadlib->resolve("SUNSON_ScanKeyPress");
    if (Qt_SUNSON_ScanKeyPress == NULL) {
        qDebug() << "Qt_SUNSON_ScanKeyPress eror!";
        return FAIL;
    }

     //3.关闭串口
    Qt_SUNSON_CloseCom = (SUNSON_CloseCom)loadlib->resolve("SUNSON_CloseCom");
    if (Qt_SUNSON_CloseCom == NULL) {
        qDebug() << "Qt_SUNSON_CloseCom eror!";
        return FAIL;
    }

  //4.键盘复位自检，所有密钥复位
   Qt_SUNSON_ResetEPP = (SUNSON_ResetEPP)loadlib->resolve("SUNSON_ResetEPP");
   if (Qt_SUNSON_ResetEPP == NULL) {
       qDebug() << "Qt_SUNSON_ResetEPP eror!";
       return FAIL;
   }

   //5.读取版本号
   Qt_SUNSON_GetVersion = (SUNSON_GetVersion)loadlib->resolve("SUNSON_GetVersion");
   if (Qt_SUNSON_GetVersion == NULL) {
       qDebug() << "Qt_SUNSON_GetVersion eror!";
       return FAIL;
   }

   //6.回送字符
   Qt_SUNSON_SendAscII = (SUNSON_SendAscII)loadlib->resolve("SUNSON_SendAscII");
   if (Qt_SUNSON_SendAscII == NULL) {
       qDebug() << "Qt_SUNSON_SendAscII eror!";
       return FAIL;
   }

   //7.下载工作密钥
   Qt_SUNSON_LoadWorkKey = (SUNSON_LoadWorkKey)loadlib->resolve("SUNSON_LoadWorkKey");
   if (Qt_SUNSON_LoadWorkKey == NULL) {
       qDebug() << "Qt_SUNSON_LoadWorkKey eror!";
       return FAIL;
   }

    //8.下载主密钥
   Qt_SUNSON_LoadMasterKey = (SUNSON_LoadMasterKey)loadlib->resolve("SUNSON_LoadMasterKey");
   if (Qt_SUNSON_LoadMasterKey == NULL) {
       qDebug() << "Qt_SUNSON_LoadMasterKey eror!";
       return FAIL;
   }

   //9.取键盘序列号
   Qt_SUNSON_GetSerialNumber = (SUNSON_GetSerialNumber)loadlib->resolve("SUNSON_GetSerialNumber");
   if (Qt_SUNSON_GetSerialNumber == NULL) {
       qDebug() << "Qt_SUNSON_SendAscII eror!";
       return FAIL;
   }

   //10.激活工作密钥
   Qt_SUNSON_ActiveKey = (SUNSON_ActiveKey)loadlib->resolve("SUNSON_ActiveKey");
   if (Qt_SUNSON_ActiveKey == NULL) {
       qDebug() << "Qt_SUNSON_ActiveKey eror!";
       return FAIL;
   }

   //11.加密
    Qt_SUNSON_DataEncrypt = (SUNSON_DataEncrypt)loadlib->resolve("SUNSON_DataEncrypt");
    if (Qt_SUNSON_DataEncrypt == NULL) {
        qDebug() << "Qt_SUNSON_DataEncrypt eror!";
        return FAIL;
    }

   //12.解密
   Qt_SUNSON_DataDecrypt = (SUNSON_DataDecrypt)loadlib->resolve("SUNSON_DataDecrypt");
   if (Qt_SUNSON_DataDecrypt == NULL) {
       qDebug() << "Qt_SUNSON_DataDecrypt eror!";
       return FAIL;
   }

      //13.下载卡号账号
   Qt_SUNSON_LoadCardNumber = (SUNSON_LoadCardNumber)loadlib->resolve("SUNSON_LoadCardNumber");
   if (Qt_SUNSON_LoadCardNumber == NULL) {
       qDebug() << "Qt_SUNSON_LoadCardNumber eror!";
       return FAIL;
   }



  //14.启动密码键盘加密
    Qt_SUNSON_StartEpp = (SUNSON_StartEpp)loadlib->resolve("SUNSON_StartEpp");
    if (Qt_SUNSON_StartEpp == NULL) {
        qDebug() << "Qt_SUNSON_StartEpp eror!";
        return FAIL;
    }

    //获取加密结果
    Qt_SUNSON_ReadCypherPin = (SUNSON_ReadCypherPin)loadlib->resolve("SUNSON_ReadCypherPin");
    if (Qt_SUNSON_ReadCypherPin == NULL) {
        qDebug() << "SUNSON_ReadCypherPin eror!";
        return FAIL;
    }

    //15.设置算法处理参数
    Qt_SUNSON_SetAlgorithmParameter = (SUNSON_SetAlgorithmParameter)loadlib->resolve("SUNSON_SetAlgorithmParameter");
    if (Qt_SUNSON_SetAlgorithmParameter == NULL) {
        qDebug() << "Qt_SUNSON_SetAlgorithmParameter eror!";
        return FAIL;
    }

     //16.开关键盘和按键声音
    Qt_SUNSON_UseEppPlainTextMode = (SUNSON_UseEppPlainTextMode)loadlib->resolve("SUNSON_UseEppPlainTextMode");
    if (Qt_SUNSON_UseEppPlainTextMode == NULL) {
        qDebug() << "Qt_SUNSON_UseEppPlainTextMode eror!";
        return FAIL;
    }

    //计算mac
    Qt_SUNSON_MakeBaseMac = (SUNSON_MakeBaseMac)loadlib->resolve("SUNSON_MakeBaseMac");
    if (Qt_SUNSON_MakeBaseMac == NULL) {
        qDebug() << "Qt_SUNSON_MakeBaseMac eror!";
        return FAIL;
    }

    return SUCCESS;
}


int zzkeypad::Qt_initAndOpenKeyPad(int comNo)
{
    int ret;
    unsigned char result[1024];
    //打开设备
    //配置是com3 9600
    ret = Qt_SUNSON_OpenCom(comNo,9600);
    if(ret == -1)
    {
        qDebug()<<"打开 keypad com失败";
        return FAIL;
    }
    qDebug()<<"打开 keypad 成功";

    memset(result,0x00,sizeof(result));
    ret = Qt_SUNSON_UseEppPlainTextMode(0x03,result);
        if(ret == -1)
        {
            qDebug()<<"设置键盘失败";
            return FAIL;
        }

        qDebug()<<"设置键盘成功 ";
//    ret = Qt_SUNSON_UsePlainKeyboard(0x03,result);
//    if(ret == -1)
//    {
//        qDebug()<<"设置键盘失败";
//        return FAIL;
//    }

    return SUCCESS;
}

int zzkeypad::Qt_getInputKey( char * ch)
{
    int result;
    unsigned char ucbuf[3];
    int count = 0;


    if(exitWaitFlag == true)
    {
        return -2;
    }


    while(true){

        QTime dieTime = QTime::currentTime().addMSecs(100);
        while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

        memset(ucbuf,0x00,sizeof(ucbuf));

        qDebug()<<"exitWaitFlag:"<<exitWaitFlag;

        if(exitWaitFlag == true)
        {
            return -2;
        }

        result = Qt_SUNSON_ScanKeyPress(ucbuf);
        if(result == -1)
        {
            count++;
            *ch = 0x00;
            if(count == 100)
            {

               return FAIL;
            }
        }
        else
        {
            *ch = ucbuf[0];
            break;
        }
    }

    return SUCCESS;
}


int zzkeypad::Qt_CloseKeyPad()
{
    int result;



    result = Qt_SUNSON_CloseCom();
    if(result ==-1)
    {
        qDebug()<<"关闭密码键盘失败";
        return FAIL;
    }

    qDebug()<<"unload"<< loadlib->unload();
    loadlib->deleteLater();
    return SUCCESS;
}

