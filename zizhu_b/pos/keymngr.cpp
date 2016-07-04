/****************************************************************************
 * FILE NAME:   KEYMNGR.C                                                   *
 * MODULE NAME: KEYMNGR                                                     *
 * PROGRAMMER:  XC                                                          *
 * DESCRIPTION: 密钥管理模块: 计算MAC;进行密钥校验;各种方式获取密钥         *
 * REVISION:                                                                *
 ****************************************************************************/
#include "cupms.h"
#include "zzkeypad.h"
#include "singleton.h"
#include "sd_des.h"
#include <QDebug>


/******************************************************************************
* External Variables
******************************************************************************/
extern Term_Config gTermCfg;


/******************************************************************************
* Module Internal Variables & Definitions
******************************************************************************/
//卡信息记录结构(密钥卡使用) 27 bytes
typedef struct
{
	unsigned char KeyNum[2];                //密钥数量  16进制字节序如：00 FF为255条
	unsigned char chCheckvalue[4];  //密码校验值
	unsigned char chExpireDate[4];  //卡有效期
	unsigned char chKeyMode;        //卡片密钥类型
	unsigned char chReserved[15];       //保留
	unsigned char chLRCValue;               //校验位
} KeyCard_CardInfo;

//密钥信息记录机构(密钥卡使用) 31 bytes
typedef struct
{
	unsigned char kyMechID[8];              //商户号
	unsigned char kyTermID[4];              //终端号
	unsigned char kyEncryptMKey[16];    //主密钥
	unsigned char kyIndex;                      //主密钥索引
	unsigned char kyLRCValue;                   //校验值
	unsigned char kyNum;                            //主密钥可用次数
} KeyCard_KeyInfoBCD;

//密钥信息记录机构(密钥卡使用) 42 bytes
typedef struct
{
	unsigned char kyMechID[15];             //商户号
	unsigned char kyTermID[8];              //终端号
	unsigned char kyEncryptMKey[16];        //主密钥
	unsigned char kyIndex;                      //主密钥索引
	unsigned char kyLRCValue;                   //校验值
	unsigned char kyNum;                            //主密钥可用次数
} KeyCard_KeyInfoASC;

int inKeyMngr_GetCardInf(KeyCard_CardInfo *CardInf);

int inKeyMngr_GetKeyRecBCD(KeyCard_KeyInfoBCD *KeyInf, int keynum, short *pKeyNo);
int inKeyMngr_GetKeyRecASC(KeyCard_KeyInfoASC *KeyInf, int keynum, short *pKeyNo);

int inKeyMngr_WriteKeyRec(unsigned char *KeyInf, short keyno, int KeyRecLen);
extern int Pinpad_LoadMasterKey(int iKeyIdx, byte * MasterKey);

int Pinpad_LoadMasterKey(int iKeyIdx, byte * MasterKey)
{
    unsigned char error_buf[1024] = {0};
    int iRet;
    unsigned char keyid;
    unsigned char keylen;

    keyid = iKeyIdx;
    keylen = 16;
    qDebug("1 Qt_SUNSON_SetAlgorithmParameter");
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x05,0x04, error_buf);
    qDebug("getInstance---iRet[%d]",  iRet);

    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_LoadMasterKey(keyid, keylen, MasterKey, error_buf);
    qDebug("ID:%d", keyid);
    print_qdebug((byte *)"MasterKey", MasterKey, keylen);

    qDebug("iRet:[%d]result[%s] ", iRet, error_buf);
    return iRet;
}
/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_ManualLoadKey
 * DESCRIPTION:   手工输入密钥
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int KeyMngr_ManualLoadKey(void *dummy)
{
	int iLen, i, iRv;
    char SUNSONDispBuf[ 64 + 1 ];
	char MasterKey[ 20 ];
	char cTmpBuf[ 56 ];

	AppUtils_Cls();
	AppUtils_DisplayCenter("手工输入密钥", LINE1, TRUE);
    gTermCfg.mFixPrm.KeyIndex = 1;
	for(; ;)
	{
        memset(SUNSONDispBuf, 0, sizeof(SUNSONDispBuf));
		
        if(AppUtils_GetNum(LINE2, (char *) "密钥索引(0-9)", SUNSONDispBuf, 1, 2, 60) <= 0)
		{
			return BANK_FAIL;
		}
		
        i = atoi(SUNSONDispBuf);
		
		PrmMngr_DevInitPinpad((void *) 1);
		
			
        AppUtils_Cls();
        sprintf(SUNSONDispBuf, "输入主密钥%d", i);
		
		memset(MasterKey, 0, sizeof(MasterKey));
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
        iLen = AppUtils_GetText(LINE2, SUNSONDispBuf, cTmpBuf, 16, 32, 0, 30);
		
		if(iLen <= 0)
			return BANK_FAIL;
			
		AppUtils_UserClear(2);
		AppUtils_DisplayCenter("手工输入密钥", LINE1, TRUE);
		AppUtils_DisplayLine("正在下载...", LINE2);
		
        //AppUtils_Asc2Bcd((byte *)cTmpBuf, (byte *)MasterKey, iLen);
        strncpy(MasterKey, cTmpBuf, 32);
        iRv = Pinpad_LoadMasterKey(i, (byte *)MasterKey);
		
        if(iRv != 0)
		{
			AppUtils_Warning("下载主密钥失败");
			return BANK_FAIL;
		}
		
		AppUtils_UserClear(2);
		AppUtils_DisplayLine("下载主密钥成功", LINE2);
		AppUtils_WaitKey(30);
		return BANK_OK;
	}
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_ImportKeyICCard
 * DESCRIPTION:   从IC卡导入密钥
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_ImportKeyICCard(void *dummy)
{
	unsigned short usKeyIdex = 0;
	short  sRet;
	int  iKeyNum;  //密钥IC卡里的主密钥总数
	short  sKeyNo = 0; //匹配到的主密钥在密钥总数中的序号（即第几个）
	byte  bt, temp[255];
	byte  bPassWord[8], bData[8], bValue[8];
	byte  bTMK1[16], bTMK[16];
	char  buf[100];
	KeyCard_CardInfo  Card_Head;  //
	KeyCard_KeyInfoBCD   Key_RecBCD;    //BCD格式密钥信息
	KeyCard_KeyInfoASC   Key_RecASC;    //ASC格式密钥信息
	
	memset(buf, 0, sizeof(buf));
	memset(temp, 0, sizeof(temp));
	memset(&Card_Head, 0, sizeof(KeyCard_CardInfo));
	
	memset(&Key_RecBCD, 0, sizeof(KeyCard_KeyInfoBCD));
	memset(&Key_RecASC, 0, sizeof(KeyCard_KeyInfoASC));
	
	AppUtils_Cls();
	AppUtils_DisplayTitle((char *)"IC卡导入主密钥");
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "终端号: %s", gTermCfg.mFixPrm.TerminalID);
	AppUtils_DisplayLine(buf, LINE2);
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "商户号: %s", gTermCfg.mFixPrm.MerchantID);
	AppUtils_DisplayLine(buf, LINE3);
	
	while(1)
	{
		bt = AppUtils_WaitKey(60);
		
		if(bt == bENTER)
		{
			break;
		}
		else if((bt == bESC) || (bt == 0))
		{
			return BANK_FAIL;
		}
	}
	
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayCenter((char *)"请插入密钥IC卡", LINE2, FALSE);
	

	//获取卡信息
	if(inKeyMngr_GetCardInf(&Card_Head) != BANK_OK)
		return BANK_FAIL;
		
	//判断卡片密钥类型
	if(Card_Head.chKeyMode != 0xFF && Card_Head.chKeyMode != 0x01)
	{
		AppUtils_Warning((char *)"非银行卡密钥卡");
		return BANK_FAIL;
	}
	
	iKeyNum = (int)Card_Head.KeyNum[0] * 256 + (int)Card_Head.KeyNum[1];
	
	if(iKeyNum == 0)
	{
		AppUtils_Warning((char *)"密钥数量为0");
		return BANK_FAIL;
	}
	
	AppUtils_DisplayCenter((char*)"读取密钥信息...", LINE2, FALSE);
	
	//获取密钥信息
	if(Card_Head.chKeyMode == 0x01)
	{
		if(inKeyMngr_GetKeyRecASC(&Key_RecASC, iKeyNum, &sKeyNo) != BANK_OK)
			return BANK_FAIL;
	}
	else
	{
		if(inKeyMngr_GetKeyRecBCD(&Key_RecBCD, iKeyNum, &sKeyNo) != BANK_OK)
			return BANK_FAIL;
	}
	
	AppUtils_UserClear(LINE2);
	
	memset(buf, 0, sizeof(buf));
	
	if(AppUtils_GetNum(LINE2, "请输入密钥索引", buf, 1, 2, 30) < 0)
		return BANK_FAIL;
		
	usKeyIdex = atoi(buf);
	
	AppUtils_UserClear(LINE2);
	
	//输入8位密码，校验密码的合法性
	memset(temp, 0, sizeof(temp));
	
	if(AppUtils_GetText(LINE2, "请输入密码:", (char *)temp, 4, 8, '*', 60) < 0)
		return BANK_FAIL ;
		
	//密码补齐0xFF做运算
	memset(bPassWord, 0xFF, sizeof(bPassWord));
	memcpy(bPassWord, temp, strlen((char*)temp));
	
	AppUtils_UserClear(LINE2);
	
	//用8位密码对8个字节的0x00作Des加密，取数据前4个字节与卡信息记录区中的“密码校验值”进行比对
	memset(bData, 0, sizeof(bData));
	memset(bValue, 0, sizeof(bValue));

	
	if(memcmp(bValue, Card_Head.chCheckvalue, 4))
	{
		//下载失败，需要将密钥信息记录区中的“可用次数”减去1  目前暂时不支持该功能，等支持了放开即可
		/*
		if( Card_Head.chKeyMode == 0x01 )
		{
		    Key_RecASC.kyNum -- ;
		    inKeyMngr_WriteKeyRec((byte*)&Key_RecASC, sKeyNo, sizeof(KeyCard_KeyInfoASC));
		}
		else
		{
		    Key_RecBCD.kyNum -- ;
		    inKeyMngr_WriteKeyRec((byte*)&Key_RecBCD, sKeyNo, sizeof(KeyCard_KeyInfoBCD));
		}
		*/
		AppUtils_Warning((char *)"密码错误");
		return BANK_FAIL;
	}
	
	//主密钥一级解密生成TMK1: 手工输入的密码对IC卡中存在的二级转加密主密钥进行Des解密，获得TMK1
	memset(bTMK1, 0, sizeof(bTMK1));

	//just for test
	//PrintWidthHEXASC("TMK1:", bTMK1, 16);
	
	//主密钥二级解密生成TMK: 使用卡片解密指令解密
	//初始化加密指令
	memset(temp, 0, sizeof(temp));
	temp[0] = 0x80;
	temp[1] = 0x1A;
	temp[2] = 0x08;

	if(sRet < 0)
	{
		AppUtils_Warning((char *)"初始化加密指令失败");
		return BANK_FAIL;
	}
	
	//卡片解密指令
	memset(temp, 0, sizeof(temp));
	temp[0] = 0x80;
	temp[1] = 0xFA;
	temp[2] = 0x80;
	temp[3] = 0x00;
	temp[4] = 0x10;  //密钥长度
    memcpy(temp + 5, bTMK1, 16);
	if(sRet < 16)   //主密钥不应该小于16字节
	{
		AppUtils_Warning((char *)"TMK解密失败");
		return BANK_FAIL;
	}
	
	//just for test
	//PrintWidthHEXASC("TMK:", temp, sRet);
	
	memset(bTMK, 0, sizeof(bTMK));
	memcpy(bTMK, temp, 16);
	
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayCenter((char *)"下载密钥, 请稍候", LINE3, FALSE);
	
	//下载主密钥
	if(Pinpad_LoadMasterKey(usKeyIdex, bTMK) != VS_SUCCESS)
	{
		//下载失败，需要将密钥信息记录区中的“可用次数”减去1  目前暂时不支持该功能，等支持了放开即可
		/*
		if( Card_Head.chKeyMode == 0x01 )
		{
		    Key_RecASC.kyNum -- ;
		    inKeyMngr_WriteKeyRec((byte*)&Key_RecASC, sKeyNo, sizeof(KeyCard_KeyInfoASC));
		}
		else
		{
		    Key_RecBCD.kyNum -- ;
		    inKeyMngr_WriteKeyRec((byte*)&Key_RecBCD, sKeyNo, sizeof(KeyCard_KeyInfoBCD));
		}
		*/
		AppUtils_Warning((char *)"下载主密钥失败");
		return BANK_FAIL;
	}
	else
	{
		AppUtils_UserClear(LINE2);
		AppUtils_DisplayCenter((char *)"主密钥下载成功", LINE3, FALSE);
		AppUtils_WaitKey(5);
		return BANK_OK;
	}
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_ImportKeyExtPort
 * DESCRIPTION:   从外部端口导入密钥(COM1/COM2/USBDEV)
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_ImportKeyExtPort(void *dummy)
{

	

	return BANK_OK;
}

static int key_Bcd2Asc(BYTE *BcdBuf, BYTE *AscBuf, unsigned short AscLen )
{
    int i, j;
    unsigned char bchar;
    int BcdLen = AscLen/2;

    for(i=0,j=0; i<BcdLen; i++)
    {
        bchar = ((BcdBuf[i]>>4)&0xf);
        AscBuf[j++] = (bchar<=9)?('0'+bchar):(bchar-10+'A');
        bchar = BcdBuf[i]&0x0f;
        AscBuf[j++] = (bchar<=9)?('0'+bchar):(bchar-10+'A');
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_CheckVal
 * DESCRIPTION:   密钥校验
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_CheckVal(char *pKeyBuffer, int iKeyDataLen)
{
	int  iRet;
    byte bOrgData[32], bResult[32], bTmpKeyData[32];
	byte *pKeyData;
	
	pKeyData = (byte *) pKeyBuffer;
    qDebug("iKeyDataLen:%d", iKeyDataLen);
    gTermCfg.mFixPrm.KeyIndex = 0;

	if(iKeyDataLen == 24)    //单倍长
	{
		memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
		memcpy(bTmpKeyData, pKeyData, 8);
		memset(bOrgData, 0, sizeof(bOrgData));
		memset(bResult, 0, sizeof(bResult));
        DES_1((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11",bTmpKeyData, bOrgData);
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bOrgData[0], bOrgData[1], bOrgData[2], bOrgData[3], bOrgData[4], bOrgData[5], bOrgData[6], bOrgData[7]);

        DES(bOrgData,(byte *)"\x00\x00\x00\x00\x00\x00\x00\x00", bResult);
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bResult[0], bResult[1], bResult[2], bResult[3], bResult[4], bResult[5], bResult[6], bResult[7]);
        /*
        if(iRet != VS_SUCCESS)
		{
			AppUtils_Warning("获取PIN校验失败");
			return BANK_FAIL;
		}
        */


		if(memcmp(bResult, pKeyData + 8, 4))
		{
			AppUtils_Warning("PIN密钥校验错");
			return BANK_FAIL;
		}
		
        memcpy(gTermCfg.mDynPrm.PINKey, bOrgData, 8);
		
		
		memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
		memcpy(bTmpKeyData, pKeyData + 12, 8);
		memset(bOrgData, 0, sizeof(bOrgData));
		memset(bResult, 0, sizeof(bResult));
        DES_1((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11",bTmpKeyData, bOrgData);
        DES(bOrgData,(byte *)"\x00\x00\x00\x00\x00\x00\x00\x00", bResult);
        /*
		if(iRet != VS_SUCCESS)
		{
			AppUtils_Warning("获取MAC校验失败");
			return BANK_FAIL;
		}
        */
		
		if(memcmp(bResult, pKeyData + 20, 4))
		{
			AppUtils_Warning("MAC密钥校验错");
			return BANK_FAIL;
		}
		
        memcpy(gTermCfg.mDynPrm.MACKey, bOrgData, 8);
	}
	else if(iKeyDataLen == 40 || iKeyDataLen == 60)    //双倍长
	{
		// pin key
		memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
        memcpy(bTmpKeyData, pKeyData, 20);
        qDebug("****************1*******************");
		memset(bOrgData, 0, sizeof(bOrgData));
        memset(bResult, 0, sizeof(bResult));

        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x00,0x30, bResult);
        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x05,0x04, bResult);

        key_Bcd2Asc(bTmpKeyData, bOrgData, 32);
        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_LoadWorkKey(gTermCfg.mFixPrm.KeyIndex, 0, 16, bOrgData, bResult);
        qDebug("1 iRet:[%d]result[%s] ", iRet, bResult);
        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_ActiveKey(gTermCfg.mFixPrm.KeyIndex, 0, bResult);
        qDebug("2 iRet:[%d]result[%s] ", iRet, bResult);
        int ilen;
        ilen = 8;
         memset(bResult, 0, sizeof(bResult));

        unsigned char data[128];
        strcpy((char *)data, "0000000000000000");
        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_DataEncrypt(ilen,data, bResult);

        qDebug("3 iRet:[%d]result[%s] ", iRet, bResult);
        memset(bOrgData, 0, sizeof(bOrgData));
        AppUtils_Asc2Bcd((byte *)bResult, (byte *)bOrgData, 8);
        if(memcmp(bOrgData, bTmpKeyData+16, 4) != 0)
        {
            AppUtils_Warning("PIN密钥校验错");
            return BANK_FAIL;
        }
        qDebug("****************2*******************");
        memcpy(gTermCfg.mDynPrm.PINKey, bTmpKeyData, 16);
		
		// mac key
		memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
        memcpy(bTmpKeyData, pKeyData + 20, 20);
        //memcpy(bTmpKeyData + 8, pKeyData + 20, 8);
		
		memset(bOrgData, 0, sizeof(bOrgData));
		memset(bResult, 0, sizeof(bResult));

        key_Bcd2Asc(bTmpKeyData, bOrgData, 32);

        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x00,0x20, bResult);
        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x05,0x04, bResult);
        memset(bResult, 0, sizeof(bResult));
        print_qdebug((byte *)"mac", bOrgData, 16);
        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_LoadWorkKey(gTermCfg.mFixPrm.KeyIndex,1, 8, bOrgData, bResult);
        qDebug("iRet:[%d]result[%s] ", iRet, bResult);

        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_ActiveKey(gTermCfg.mFixPrm.KeyIndex, 1, bResult);
        qDebug("2 iRet:[%d]result[%s] ", iRet, bResult);

        memset(bResult, 0, sizeof(bResult));
        strcpy((char *)data, "0000000000000000");
        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_DataEncrypt(ilen,data, bResult);

        qDebug("3 iRet:[%d]result[%s] ", iRet, bResult);

        memset(bOrgData, 0, sizeof(bOrgData));
        AppUtils_Asc2Bcd((byte *)bResult, (byte *)bOrgData, 8);
        if(memcmp(bOrgData, bTmpKeyData+16, 4) != 0)
		{
			AppUtils_Warning("MAC密钥校验错");
			return BANK_FAIL;
		}
        qDebug("****************3*******************");
        memcpy(gTermCfg.mDynPrm.MACKey, bTmpKeyData, 16);
		
		//磁道加密密钥校验，仅长度为60时起作用
		if(iKeyDataLen == 60)    //双倍长+磁道加密方式
		{
			memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
            memcpy(bTmpKeyData, pKeyData + 40, 20);
			
			memset(bOrgData, 0, sizeof(bOrgData));
			memset(bResult, 0, sizeof(bResult));
            key_Bcd2Asc(bTmpKeyData, bOrgData, 32);
            iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x00,0x30, bResult);
            iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x05,0x05, bResult);

            iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_LoadWorkKey(gTermCfg.mFixPrm.KeyIndex,2, 16, bOrgData, bResult);
            qDebug("iRet:[%d]result[%s] ", iRet, bResult);
            iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_ActiveKey(gTermCfg.mFixPrm.KeyIndex, 2, bResult);
            qDebug("2 iRet:[%d]result[%s] ", iRet, bResult);

            memset(bResult, 0, sizeof(bResult));
            strcpy((char *)data, "0000000000000000");
            iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_DataEncrypt(ilen,data, bResult);

            qDebug("3 iRet:[%d]result[%s] ", iRet, bResult);
            memset(bOrgData, 0, sizeof(bOrgData));
            AppUtils_Asc2Bcd((byte *)bResult, (byte *)bOrgData, 8);
            if(memcmp(bOrgData, bTmpKeyData+16, 4) != 0)
			{
				AppUtils_Warning("TRK密钥校验错");
				return BANK_FAIL;
			}
			
            memcpy(gTermCfg.mDynPrm.TRACKKey, bTmpKeyData, 16);
		}


	}
	else
	{
		AppUtils_Warning((char *) "密钥数据长度有误");
		return BANK_FAIL;
	}
	
	//保存工作密钥信息到磁盘参数
	PrmMngr_SavePrmToSamDisk();
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_CheckVal
 * DESCRIPTION:   密钥校验
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_CheckVal_bak(char *pKeyBuffer, int iKeyDataLen)
{
    int  iRet;
    byte bOrgData[32], bResult[32], bTmpKeyData[32];
    byte *pKeyData;

    pKeyData = (byte *) pKeyBuffer;
    qDebug("iKeyDataLen:%d", iKeyDataLen);

    if(iKeyDataLen == 24)    //单倍长
    {
        memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
        memcpy(bTmpKeyData, pKeyData, 8);
        memset(bOrgData, 0, sizeof(bOrgData));
        memset(bResult, 0, sizeof(bResult));
        DES_1((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11",bTmpKeyData, bOrgData);
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bOrgData[0], bOrgData[1], bOrgData[2], bOrgData[3], bOrgData[4], bOrgData[5], bOrgData[6], bOrgData[7]);

        DES(bOrgData,(byte *)"\x00\x00\x00\x00\x00\x00\x00\x00", bResult);
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bResult[0], bResult[1], bResult[2], bResult[3], bResult[4], bResult[5], bResult[6], bResult[7]);
        /*
        if(iRet != VS_SUCCESS)
        {
            AppUtils_Warning("获取PIN校验失败");
            return BANK_FAIL;
        }
        */


        if(memcmp(bResult, pKeyData + 8, 4))
        {
            AppUtils_Warning("PIN密钥校验错");
            return BANK_FAIL;
        }

        memcpy(gTermCfg.mDynPrm.PINKey, bOrgData, 8);


        memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
        memcpy(bTmpKeyData, pKeyData + 12, 8);
        memset(bOrgData, 0, sizeof(bOrgData));
        memset(bResult, 0, sizeof(bResult));
        DES_1((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11",bTmpKeyData, bOrgData);
        DES(bOrgData,(byte *)"\x00\x00\x00\x00\x00\x00\x00\x00", bResult);
        /*
        if(iRet != VS_SUCCESS)
        {
            AppUtils_Warning("获取MAC校验失败");
            return BANK_FAIL;
        }
        */

        if(memcmp(bResult, pKeyData + 20, 4))
        {
            AppUtils_Warning("MAC密钥校验错");
            return BANK_FAIL;
        }

        memcpy(gTermCfg.mDynPrm.MACKey, bOrgData, 8);
    }
    else if(iKeyDataLen == 40 || iKeyDataLen == 60)    //双倍长
    {
        // pin key
        memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
        memcpy(bTmpKeyData, pKeyData, 16);

        memset(bOrgData, 0, sizeof(bOrgData));
        memset(bResult, 0, sizeof(bResult));
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bTmpKeyData[0], bTmpKeyData[1], bTmpKeyData[2], bTmpKeyData[3], bTmpKeyData[4], bTmpKeyData[5], bTmpKeyData[6], bTmpKeyData[7]);

        DES_31((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11", bTmpKeyData, bOrgData);
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bOrgData[0], bOrgData[1], bOrgData[2], bOrgData[3], bOrgData[4], bOrgData[5], bOrgData[6], bOrgData[15]);
        DES_31((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11", bTmpKeyData+8, bOrgData+8);
        DES_3(bOrgData,(byte *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", bResult);
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bResult[0], bResult[1], bResult[2], bResult[3], bResult[4], bResult[5], bResult[6], bResult[7]);
        /*
        if(iRet != VS_SUCCESS)
        {
            AppUtils_Warning("获取PIN校验失败");
            return BANK_FAIL;
        }
        */

        if(memcmp(bResult, pKeyData + 16, 4))
        {
            AppUtils_Warning("PIN密钥校验错");
            return BANK_FAIL;
        }

        memcpy(gTermCfg.mDynPrm.PINKey, bOrgData, 16);

        // mac key
        memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
        memcpy(bTmpKeyData, pKeyData + 20, 8);
        memcpy(bTmpKeyData + 8, pKeyData + 20, 8);

        memset(bOrgData, 0, sizeof(bOrgData));
        memset(bResult, 0, sizeof(bResult));
        DES_31((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11",bTmpKeyData, bOrgData);
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bOrgData[0], bOrgData[1], bOrgData[2], bOrgData[3], bOrgData[4], bOrgData[5], bOrgData[6], bOrgData[7]);
        DES_31((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11",bTmpKeyData+8, bOrgData+8);

        DES_3(bOrgData,(byte *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", bResult);
        //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bResult[0], bResult[1], bResult[2], bResult[3], bResult[4], bResult[5], bResult[6], bResult[7]);
        /*
        if(iRet != VS_SUCCESS)
        {
            AppUtils_Warning("获取MAC校验失败");
            return BANK_FAIL;
        }
        */

        if(memcmp(bResult, pKeyData + 36, 4))
        {
            AppUtils_Warning("MAC密钥校验错");
            return BANK_FAIL;
        }

        memcpy(gTermCfg.mDynPrm.MACKey, bOrgData, 16);

        //磁道加密密钥校验，仅长度为60时起作用
        if(iKeyDataLen == 60)    //双倍长+磁道加密方式
        {
            memset(bTmpKeyData, 0, sizeof(bTmpKeyData));
            memcpy(bTmpKeyData, pKeyData + 40, 16);

            memset(bOrgData, 0, sizeof(bOrgData));
            memset(bResult, 0, sizeof(bResult));
            DES_31((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11",bTmpKeyData, bOrgData);
            //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bOrgData[0], bOrgData[1], bOrgData[2], bOrgData[3], bOrgData[4], bOrgData[5], bOrgData[6], bOrgData[7]);
            DES_31((byte *)"\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11",bTmpKeyData+8, bOrgData+8);

            DES_3(bOrgData,(byte *)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", bResult);
            //qDebug("%02x%02x%02x%02x%02x%02x%02x%02x", bResult[0], bResult[1], bResult[2], bResult[3], bResult[4], bResult[5], bResult[6], bResult[7]);
            /*
            if(iRet != VS_SUCCESS)
            {
                AppUtils_Warning("获取TRK校验失败");
                return BANK_FAIL;
            }*/

            if(memcmp(bResult, pKeyData + 56, 4))
            {
                AppUtils_Warning("TRK密钥校验错");
                return BANK_FAIL;
            }

            memcpy(gTermCfg.mDynPrm.TRACKKey, bOrgData, 16);
        }
    }
    else
    {
        AppUtils_Warning((char *) "密钥数据长度有误");
        return BANK_FAIL;
    }

    //保存工作密钥信息到磁盘参数
    PrmMngr_SavePrmToSamDisk();

    return BANK_OK;
}
/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_CalMac
 * DESCRIPTION:   计算MAC
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_CalMac(byte *pPacketBuffer, int iPacketLen, byte *pRetMac)
{
    char macBuff[4096];
    int len;
    //qDebug("iPacketLen:%d", iPacketLen);
    memset(macBuff, 0, sizeof(macBuff));
    len = 0;
    memcpy(macBuff, pPacketBuffer, iPacketLen);
    len = iPacketLen/8==0? iPacketLen : (iPacketLen+(8-iPacketLen%8));
    //print_qdebug((byte *)"mackey", gTermCfg.mDynPrm.MACKey, 16);
    Des1_ECBMac(gTermCfg.mDynPrm.MACKey,(byte *)macBuff,len,pRetMac);
    //print_qdebug((byte *)"pRetMac", pRetMac, 8);
    return 0;
	
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_EncryptTrackData
 * DESCRIPTION:   加密磁道信息
 * PARAMETERS     OrgTrackData: ASCII码格式的磁道信息
 *                EncryptTrackData: 加密后的磁道信息
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_EncryptTrackData(byte *OrgTrackData, byte *EncryptTrackData)
{
	char cBuffer[64] = {0};
	char cTmpBuf[16] = {0};
	char cTDB[16 + 1] = {0};
	int iTrackLen = 0;
	int iEncryptOffset;
	
	iTrackLen = strlen((char *)OrgTrackData);
	iEncryptOffset = (iTrackLen + 1) / 2 - 9;
	
	AppUtils_Asc2Bcd((byte *)OrgTrackData, (byte *)cBuffer, iTrackLen);
	
	//取出需要加密的内容
	memcpy(cTDB, cBuffer + iEncryptOffset, 8);
	

	
	//把加密后的数据放回
	memcpy(cBuffer + iEncryptOffset, cTmpBuf, 8);
	
	AppUtils_Bcd2Asc((byte *)cBuffer, EncryptTrackData, iTrackLen);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_SetTestKey
 * DESCRIPTION:   设置测试主密钥
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_SetTestKey(void *dummy)
{
	char MasterKey[20];
	
	AppUtils_DisplayTitle("设置测试密钥");
	
	AppUtils_UserClear(2);
	AppUtils_DisplayLine("正在下载...", LINE2);
	
	memset(MasterKey, 0, sizeof(MasterKey));
	//memcpy( MasterKey, "\x0E\x37\xB6\xC8\x8C\xDA\xAD\xFE\x40\x2A\x15\xE0\xAD\xA2\x0B\x25", 16 );
	memcpy(MasterKey, "\x86\xCB\xBA\x3E\x7F\x85\x38\xCB\xB5\x57\x83\x34\x46\x13\x7A\x23", 16);
	
	if(Pinpad_LoadMasterKey(gTermCfg.mFixPrm.KeyIndex, (byte *) MasterKey) != VS_SUCCESS)
	{
		AppUtils_Warning("下载主密钥失败");
		return BANK_FAIL;
	}
	
	AppUtils_UserClear(2);
	AppUtils_DisplayLine("下载主密钥成功", LINE2);
	AppUtils_WaitKey(30);
	return BANK_OK;
}


/* --------------------------------------------------------------------------
* FUNCTION NAME: inKeyMngr_GetCardInf
* DESCRIPTION:   获取密钥卡卡信息记录
* PARAMETER:     CardInf 卡信息结构指针
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int inKeyMngr_GetCardInf(KeyCard_CardInfo *CardInf)
{
	int i;
	short sRet;
	char  buf[100];
	byte  temp[255], bCheckValue;
	KeyCard_CardInfo  cardinf_tmp;
	
	memset(&cardinf_tmp, 0, sizeof(KeyCard_CardInfo));
	
	
	if(sRet < 0)
	{
		AppUtils_Warning((char *)"选择MF失败");
		return BANK_FAIL;
	}
	
	memset(temp, 0, sizeof(temp));

	if(sRet < 0)
	{
		AppUtils_Warning((char *)"选择卡信息文件失败");
		return BANK_FAIL;
	}
	

	if(sRet <= 0)
	{
		AppUtils_Warning((char *)"读卡信息数据失败");
		return BANK_FAIL;
	}
	
	//just for test
	//PrnDisp_PrintWidthHEXASC("card_inf:", (byte*)&cardinf_tmp, sRet);
	
	//卡片校验
	bCheckValue = 0x00;
	memset(temp, 0, sizeof(temp));
	memcpy(temp, (byte *)&cardinf_tmp, sizeof(KeyCard_CardInfo) - 1);
	
	for(i = 0; i < (sizeof(KeyCard_CardInfo) - 1); i++)
	{
		bCheckValue ^= temp[i];
	}
	
	if(bCheckValue != cardinf_tmp.chLRCValue)
	{
		AppUtils_Warning((char *)"IC卡错误");
		return BANK_FAIL;
	}
	
	//判断有效期
	memset(buf, 0, sizeof(buf));
	memset(temp, 0, sizeof(temp));

	if((memcmp(buf, temp, 8)) > 0)
	{
		AppUtils_Warning((char *)"此卡已过期");
		return BANK_FAIL;
	}
	
	memcpy((byte *)CardInf, (byte *)&cardinf_tmp, sizeof(KeyCard_CardInfo));
	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: inKeyMngr_WriteKeyRec
* DESCRIPTION:   写密钥信息文件
* PARAMETER:     KeyInf 密钥信息记录结构指针   keyno 记录编号
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int inKeyMngr_WriteKeyRec(unsigned char *KeyInf, short keyno, int KeyRecLen)
{
	short sStart;
	short sRet;
	byte  temp[255];
	
	memset(temp, 0, sizeof(temp));

	if(sRet < 0)
	{
		AppUtils_Warning((char *)"选择密钥信息文件失败");
		return BANK_FAIL;
	}
	
	//写入文件位置
	sStart = keyno * KeyRecLen;
	
	//写密钥信息文件
	memset(temp, 0, sizeof(temp));
	temp[0] = 0x00;
	temp[1] = 0xD6;
	temp[2] = sStart / 256; //写入位置高位
	temp[3] = sStart % 256; //写入位置低位
	temp[4] = KeyRecLen;   //写入密钥信息长度
	memcpy(temp + 5, (byte *)KeyInf, KeyRecLen);

	
	if(sRet < 0)
	{
		AppUtils_Warning((char *)"写密钥信息失败");
		return BANK_FAIL;
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: inKeyMngr_GetKeyRecBCD
* DESCRIPTION:   获取密钥信息记录(BCD格式记录)
* PARAMETER:     KeyInf  keynum  pKeyNo
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int inKeyMngr_GetKeyRecBCD(KeyCard_KeyInfoBCD *KeyInf, int keynum, short *pKeyNo)
{
	int   i;
	short sStart;
	short sRet;
	byte  temp[255], bCheckValue;
	char  buf[100];
	KeyCard_KeyInfoBCD keyinf_tmp;
	
	memset(&keyinf_tmp, 0, sizeof(KeyCard_KeyInfoBCD));
	

	if(sRet < 0)
	{
		AppUtils_Warning((char *)"选择密钥信息文件失败");
		return BANK_FAIL;
	}
	
	//根据终端的商户号和终端号，与卡片中每条主密钥所对应的商户号、终端号逐条进行匹配
	for(i = 0; i < keynum; i++)
	{
#if 0   //全匹配方式，速度慢
		sStart = i * sizeof(KeyCard_KeyInfoBCD);
		
		sRet = ICCmd_ReadBinary2((sStart / 256), (sStart % 256), (byte *) & keyinf_tmp, sizeof(KeyCard_KeyInfoBCD), CUSTOMER_CARD);
		
		if(sRet <= 0)
		{
			AppUtils_Warning((char *)"读密钥信息数据失败");
			return BANK_FAIL;
		}
		
		//just for test
		//PrintWidthHEXASC("key_inf:", (byte*)&keyinf_tmp, sRet);
		
		memset(buf, 0, sizeof(buf));
		memset(temp, 0, sizeof(temp));
		AppUtils_Bcd2Asc((byte *)keyinf_tmp.kyMechID, (byte *)buf, 16);
		AppUtils_Bcd2Asc((byte *)keyinf_tmp.kyTermID, (byte *)temp, 8);
		buf[15] = 0;
		
		if((!memcmp(gTermCfg.mFixPrm.MerchantID, buf, 15)) && (!memcmp(gTermCfg.mFixPrm.TerminalID, temp, 8)))
			break;
			
#else  //部分匹配方式，优化读取速度: 先只读取终端号，匹配上再读取整条记录做全匹配。 by XC 2015/3/12 16:51:02
		sStart = i * sizeof(KeyCard_KeyInfoBCD) + 8;
		memset(buf, 0, sizeof(buf));

		if(sRet <= 0)
		{
			AppUtils_Warning((char *)"读密钥记录失败");
			return BANK_FAIL;
		}
			
		AppUtils_Bcd2Asc((byte *)buf, (byte *)temp, 8);
			
		if(!memcmp(gTermCfg.mFixPrm.TerminalID, temp, 8))
		{
			sStart = i * sizeof(KeyCard_KeyInfoBCD);

			if(sRet <= 0)
			{
				AppUtils_Warning((char *)"读密钥数据失败");
				return BANK_FAIL;
			}
			
			//just for test
			//PrintWidthHEXASC("key_inf:", (byte*)&keyinf_tmp, sRet);
			
			memset(buf, 0, sizeof(buf));
			memset(temp, 0, sizeof(temp));
			AppUtils_Bcd2Asc((byte *)keyinf_tmp.kyMechID, (byte *)buf, 16);
			AppUtils_Bcd2Asc((byte *)keyinf_tmp.kyTermID, (byte *)temp, 8);
			buf[15] = 0;
			
			if((!memcmp(gTermCfg.mFixPrm.MerchantID, buf, 15)) && (!memcmp(gTermCfg.mFixPrm.TerminalID, temp, 8)))
				break;
		}
			
#endif
	}
	
	//匹配不到密钥
	if(i == keynum)
	{
		AppUtils_Warning((char *)"主密钥未找到");
		return BANK_FAIL;
	}
	
	*pKeyNo = i;
	
	//匹配到密钥，但是该密钥已经无可用次数了
	/*if (keyinf_tmp.kyNum == 0)
	{
	  AppUtils_Warning((char *)"该密钥已无法使用");
	  return BANK_FAIL;
	}*/ //目前暂时不支持该功能，等支持了放开即可
	
	//密钥信息校验
	bCheckValue = 0x00;
	memset(temp, 0, sizeof(temp));
	memcpy(temp, (byte *)&keyinf_tmp, sizeof(KeyCard_KeyInfoBCD) - 2);
	
	for(i = 0; i < (sizeof(KeyCard_KeyInfoBCD) - 2); i++)
	{
		bCheckValue ^= temp[i];
	}
	
	//如果密钥信息校验失败，需要将“可用次数”减去1
	if(bCheckValue != keyinf_tmp.kyLRCValue)
	{
		/*i = *pKeyNo;
		keyinf_tmp.kyNum --;
		inKeyMngr_WriteKeyRec((byte*)&keyinf_tmp, (short)i, sizeof(KeyCard_KeyInfoBCD));*/ //目前暂时不支持该功能，等支持了放开即可
		
		AppUtils_Warning((char *)"主密钥有误");
		return BANK_FAIL;
	}
	
	memcpy((byte *)KeyInf, (byte *)&keyinf_tmp, sizeof(KeyCard_KeyInfoBCD));
	
	return BANK_OK;
}



/* --------------------------------------------------------------------------
* FUNCTION NAME: inKeyMngr_GetKeyRecASC
* DESCRIPTION:   获取密钥信息记录(ASC格式记录)
* PARAMETER:     KeyInf  keynum  pKeyNo
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int inKeyMngr_GetKeyRecASC(KeyCard_KeyInfoASC *KeyInf, int keynum, short *pKeyNo)
{
	int   i;
	short sStart;
	short sRet;
	byte  temp[255], bCheckValue;
	char  buf[100];
	KeyCard_KeyInfoASC keyinf_tmp;
	
	memset(&keyinf_tmp, 0, sizeof(KeyCard_KeyInfoASC));
	
	
	if(sRet < 0)
	{
		AppUtils_Warning((char *)"选择密钥信息文件失败");
		return BANK_FAIL;
	}
	
	//根据终端的商户号和终端号，与卡片中每条主密钥所对应的商户号、终端号逐条进行匹配
	for(i = 0; i < keynum; i++)
	{
#if 0   //全匹配方式，速度慢
		sStart = i * sizeof(KeyCard_KeyInfoASC);
		

		
		if(sRet <= 0)
		{
			AppUtils_Warning((char *)"读密钥记录失败~");
			return BANK_FAIL;
		}
		
		//just for test
		//PrintWidthHEXASC("key_inf:", (byte*)&keyinf_tmp, sRet);
		
		if((!memcmp(gTermCfg.mFixPrm.MerchantID, keyinf_tmp.kyMechID, 15)) && (!memcmp(gTermCfg.mFixPrm.TerminalID, keyinf_tmp.kyTermID, 8)))
			break;
			
#else  //部分匹配方式，优化读取速度: 先只读取终端号，匹配上再读取整条记录做全匹配。 by XC
		sStart = i * sizeof(KeyCard_KeyInfoASC) + 16;
		memset(buf, 0, sizeof(buf));

		if(sRet <= 0)
		{
			AppUtils_Warning((char *)"读密钥记录失败!");
			return BANK_FAIL;
		}
			
		//终端号是ASCII存储，直接比较
		if(!memcmp(gTermCfg.mFixPrm.TerminalID, buf, 8))
		{
			sStart = i * sizeof(KeyCard_KeyInfoASC);

			
			if(sRet <= 0)
			{
				AppUtils_Warning((char *)"读密钥数据失败!");
				return BANK_FAIL;
			}
			
			//just for test
			//PrintWidthHEXASC("key_inf:", (byte*)&keyinf_tmp, sRet);
			
			if((!memcmp(gTermCfg.mFixPrm.MerchantID, keyinf_tmp.kyMechID, 15)) && (!memcmp(gTermCfg.mFixPrm.TerminalID, keyinf_tmp.kyTermID, 8)))
				break;
		}
			
#endif
	}
	
	//匹配不到密钥
	if(i == keynum)
	{
		AppUtils_Warning((char *)"主密钥未找到~");
		return BANK_FAIL;
	}
	
	*pKeyNo = i;
	
	//匹配到密钥，但是该密钥已经无可用次数了
	/*if (keyinf_tmp.kyNum == 0)
	{
	  AppUtils_Warning((char *)"该密钥已无法使用");
	  return BANK_FAIL;
	}*/ //目前暂时不支持该功能，等支持了放开即可
	
	//密钥信息校验
	bCheckValue = 0x00;
	memset(temp, 0, sizeof(temp));
	memcpy(temp, (byte *)&keyinf_tmp, sizeof(KeyCard_KeyInfoASC) - 2);
	
	for(i = 0; i < (sizeof(KeyCard_KeyInfoASC) - 2); i++)
	{
		bCheckValue ^= temp[i];
	}
	
	//如果密钥信息校验失败，需要将“可用次数”减去1
	if(bCheckValue != keyinf_tmp.kyLRCValue)
	{
		/*i = *pKeyNo;
		keyinf_tmp.kyNum --;
		inKeyMngr_WriteKeyRec((byte*)&keyinf_tmp, (short)i, sizeof(KeyCard_KeyInfoASC));*/ //目前暂时不支持该功能，等支持了放开即可
		
		AppUtils_Warning((char *)"主密钥有误!");
		return BANK_FAIL;
	}
	
	memcpy((byte *)KeyInf, (byte *)&keyinf_tmp, sizeof(KeyCard_KeyInfoASC));
	
	return BANK_OK;
}

