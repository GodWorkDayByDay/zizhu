#include "cardreader.h"
#include <QDebug>
#include <QTime>
#include <QApplication>

CardReader * CardReader::cardreader = NULL;
CardReader::CardReader()
{
    DllInitFlag = false;
}

int CardReader::Qt_loadCardReaderDll()
{
    if(DllInitFlag == false){

        dllPath = "cardreader\\CRT_310.dll";

        /*加载动态库*/
        loadlib = new QLibrary(dllPath);

        if(loadlib->load()) {
            qDebug() << "load cardreader  dll ok!";
        } else {
            qDebug() << "load error!";
            return FAIL;
        }

        //获取函数
        if(initFuncs() == FAIL)
            return FAIL;

        DllInitFlag = true;
    }

    return SUCCESS;
}

/*
    CRT310_SensorStatusV3 Qt_CRT310_SensorStatusV3;
*/
int CardReader::initFuncs()
{
    Qt_CommOpen = (CommOpen)loadlib->resolve("CommOpen");
    if (Qt_CommOpen == NULL) {
        qDebug() << "Qt_CommOpen eror!";
        return FAIL;
    }


    Qt_CommOpenWithBaut = (CommOpenWithBaut)loadlib->resolve("CommOpenWithBaut");
    if (Qt_CommOpenWithBaut == NULL) {
        qDebug() << "Qt_CommOpenWithBaut eror!";
        return FAIL;
    }

    Qt_CommClose = (CommClose)loadlib->resolve("CommClose");
    if (Qt_CommClose == NULL) {
        qDebug() << "Qt_CommClose eror!";
        return FAIL;
    }

    Qt_CommSetting =  (CommSetting)loadlib->resolve("CommSetting");
    if (Qt_CommSetting == NULL) {
        qDebug() << "Qt_CommSetting eror!";
        return FAIL;
    }

    Qt_CRT_R_SetComm = (CRT_R_SetComm)loadlib->resolve("CRT_R_SetComm");
    if (Qt_CRT_R_SetComm == NULL) {
        qDebug() << "Qt_CRT_R_SetComm eror!";
        return FAIL;
    }

    Qt_CRT310_WriteSnr = (CRT310_WriteSnr)loadlib->resolve("CRT310_WriteSnr");
    if (Qt_CRT310_WriteSnr == NULL) {
        qDebug() << "Qt_CRT310_WriteSnr eror!";
        return FAIL;
    }

    Qt_CRT310_ReadSnr= (CRT310_ReadSnr)loadlib->resolve("CRT310_ReadSnr");
    if (Qt_CRT310_ReadSnr == NULL) {
        qDebug() << "Qt_CRT310_ReadSnr eror!";
        return FAIL;
    }

    Qt_CRT310_Reset= (CRT310_Reset)loadlib->resolve("CRT310_Reset");
    if (Qt_CRT310_Reset == NULL) {
        qDebug() << "Qt_CRT310_Reset eror!";
        return FAIL;
    }

    Qt_CRT310_CardSetting= (CRT310_CardSetting)loadlib->resolve("CRT310_CardSetting");
    if (Qt_CRT310_CardSetting == NULL) {
        qDebug() << "Qt_CRT310_CardSetting eror!";
        return FAIL;
    }

    Qt_CRT310_CardPosition= (CRT310_CardPosition)loadlib->resolve("CRT310_CardPosition");
    if (Qt_CRT310_CardPosition == NULL) {
        qDebug() << "Qt_CRT310_CardPosition eror!";
        return FAIL;
    }

    Qt_CRT310_GetStatus= (CRT310_GetStatus)loadlib->resolve("CRT310_GetStatus");
    if (Qt_CRT310_GetStatus == NULL) {
        qDebug() << "Qt_CRT310_GetStatus eror!";
        return FAIL;
    }

    Qt_CRT310_MovePosition = (CRT310_MovePosition)loadlib->resolve("CRT310_MovePosition");
    if (Qt_CRT310_MovePosition == NULL) {
        qDebug() << "Qt_CRT310_MovePosition eror!";
        return FAIL;
    }

    Qt_CRT310_SensorStatusV3 = (CRT310_SensorStatusV3)loadlib->resolve("CRT310_SensorStatusV3");
    if (Qt_CRT310_SensorStatusV3 == NULL) {
        qDebug() << "Qt_CRT310_SensorStatusV3 eror!";
        return FAIL;
    }

    Qt_MC_ReadTrack = (MC_ReadTrack)loadlib->resolve("MC_ReadTrack");
    if (Qt_MC_ReadTrack == NULL) {
        qDebug() << "Qt_MC_ReadTrack eror!";
        return FAIL;
    }


    return SUCCESS;
}


int CardReader::Qt_initAndOpenCardReader(char * deviceName)
{
    int result;
    ComHandle = Qt_CommOpenWithBaut(deviceName,9600);
    if(ComHandle == NULL)
    {
        qDebug()<<"Open Cardreader fail";
        return FAIL;
    }
    qDebug()<<"ComHandle"<<ComHandle;

    //进卡控制设置
    result = Qt_CRT310_CardSetting(ComHandle,0x32,0x31);
    if(result != 0)
    {
        qDebug()<<"Qt_CRT310_CardSetting error";
        return FAIL;
    }

    //停卡设置
    result = Qt_CRT310_CardPosition(ComHandle,0x34);
    if(result != 0)
    {
        qDebug()<<"Qt_CRT310_CardPosition error";
        return FAIL;
    }

    return SUCCESS;
}






int CardReader::Qt_GetTrack(unsigned char *track1, unsigned char *track2, unsigned char *track3, unsigned char *cardNo)
{
    unsigned char position;
    int result;
    position = getCardStatusIn();
    if(position == 0xff)
    {
        qDebug()<<"getCardStatusError";
        return FAIL;
    }

    unsigned char track[500];

    //第一磁道
    memset(track,0x00,sizeof(track));
    result = Qt_MC_ReadTrack(ComHandle,0x30,0x37,track);
    if(result != 0)
    {
        qDebug()<<"Qt_MC_ReadTrack error";
        return FAIL;
    }

    int i = 0;

    if(track[i] == 0x1F)
    {
        if(track[i+1] == 0x4E)
        {
            i+=3;
        }
        else{
            i+=2;
            for(unsigned int j = 0 ; j < sizeof(track);j++)
            {
                if(track[i] == 0x1F)
                {
                    break;
                }
                track1[j] = track[i++];
            }
        }
    }

    if(track[i] == 0x1F)
    {
        if(track[i+1] == 0x4E)
        {
            i+=3;
        }
        else{
            i+=2;
            for(unsigned int j = 0 ; j < sizeof(track);j++)
            {
                if(track[i] == 0x1F)
                {
                    break;
                }
                track2[j] = track[i++];
            }
        }
    }


    if(track[i] == 0x1F)
    {
        if(track[i+1] == 0x4E)
        {
            i+=3;
        }
        else{
            i+=2;
            for(unsigned int j = 0 ; j < sizeof(track);j++)
            {
                if(track[i] == 0x00)
                {
                    break;
                }
                track3[j] = track[i++];
            }
        }
    }

    for(unsigned int j =0 ;j < sizeof(track);j++ )
    {
        if(track2[j] == '=')
            break;
        cardNo[j] = track2[j];
    }

    return SUCCESS;
}

int CardReader::getCardStatusIn()
{
    int result;
    unsigned char position;
    unsigned char frountSetting = 0x4A;
    unsigned char rearSetting = 0x4E;

    while(true)
    {
        result = Qt_CRT310_GetStatus(ComHandle,&position,&frountSetting,&rearSetting);
        if(result !=0)
        {
            qDebug()<<"Qt_CRT310_GetStatus error";
            return 0xff;
        }

        if(position == 0x4C) //卡机后端持卡位有卡
            break;

        QTime dieTime = QTime::currentTime().addMSecs(200);
        while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 200);
    }

    return position;
}

int CardReader::Qt_pushOutCard()
{
    int result;
    result = Qt_CRT310_Reset(ComHandle, 0x31);
    if(result != 0)
    {
        qDebug()<<"弹卡错误";
        return FAIL;
    }
    Qt_CardReader_Close();

    Qt_initAndOpenCardReader("Com4");



    return SUCCESS;
}


int CardReader::Qt_CardReader_Close()
{
    int result;
    result = Qt_CommClose(ComHandle);
    if(result != 0)
    {
        qDebug()<<"关闭卡片错误";
        return FAIL;
    }

    return SUCCESS;

}
