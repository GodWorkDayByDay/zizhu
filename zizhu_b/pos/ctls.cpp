
/******************************************************* *******************
* FILE NAME:    CTLS.c                                                     *
* MODULE NAME:  CTLS                                                       *
* PROGRAMMER:   Laikey                                                     *
* DESCRIPTION:                                                             *
* REVISION:                                                                *
****************************************************************************/
#include "cupms.h"

/* -----------------------------------------------------------------------------
* External variables / declarations
---------------------------------------------------------------------------- */
extern Term_Config gTermCfg;
#define vfiTerminalAppListCtls int
vfiTerminalAppListCtls  g_globctlsAIDList = {0};
#define DEV_CTLS 1
#define DEV_COM1 2
#define DEV_COM2 3
#define DEV_USBSER 4
/* -----------------------------------------------------------------------------
* Private definitions
---------------------------------------------------------------------------- */
#define  MF_NAME  "2PAY.SYS.DDF01"
#define  EP_NAME  "\xA0\x00\x00\x03\x33\x43\x55\x50\x2D\x4D\x4F\x42\x49\x4C\x45"
#define  CARDINFO_SFI 0x02
#define  CUP_RID_HEX         "\xA0\x00\x00\x03\x33"
char *CtlsDevName[MAX_CTLS_IF_TYPE] = {(char *)DEV_CTLS, (char *)DEV_COM1, (char *)DEV_COM2, (char *)DEV_USBSER};

//回调函数不显示提示信息的标志, 某些特殊情况下需要完全由应用来控制界面时使用
static int NoAppPromptFlag = 1;
static byte CtlsReaderModeFlag = 0;

/* -----------------------------------------------------------------------------
 * Private Function define
 ---------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------
 * Callback Function declare for CTLSLIB - Do not change their names!!!
 ---------------------------------------------------------------------------- */

/*-----------------------------------------------------------------------
 * FUNCTION NAME: CTLS_Callback_PromptRemoveCard
 * DESCRIPTION:   移开卡片的回调函数
 * PARAMETERS:
 * RETURN:
 *----------------------------------------------------------------------*/
void CTLS_Callback_PromptRemoveCard(void)
{
	AppUtils_Cls();
	AppUtils_DisplayCenter("请移开卡片", LINE2, FALSE);
	
	AppUtils_SVCWAIT(500);
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: CTLS_Callback_PromptBalance
 * DESCRIPTION:   显示金额的回调函数
 * PARAMETERS:
 * RETURN:
 *----------------------------------------------------------------------*/
void CTLS_Callback_PromptBalance(byte rdr_type, byte BuildInUIEnable, byte *BCDBalance)
{
	char buffer[20];
	long bal;
	
	//内置读卡器

	{
		//UI自定义时，可以直接返回,也可以在此处显示余额
		if(BuildInUIEnable == 0)
		{
#if 0
			memset(buffer, 0x00, sizeof(buffer));
			AppUtils_Bcd2Asc(BCDBalance, (byte *)buffer, 12);
			bal = atol(buffer);
			sprintf(buffer, "Balance: %lu.%02lu", bal / 100, bal % 100);
			AppUtils_Cls();
			AppUtils_DisplayLine(buffer, 2);
			return;
#endif
		}
		//UI为系统默认界面时，直接返回系统自动显示余额
		else
		{
			return;
		}
	}

}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: CTLS_Callback_PromptTransResult
 * DESCRIPTION:   显示交易结果的回调函数
 * PARAMETERS:
 * RETURN:
 *----------------------------------------------------------------------*/
void CTLS_Callback_PromptTransResult(byte rdr_type, byte BuildInUIEnable,  int ret, int FallbackFlag)
{
	if(NoAppPromptFlag == 2)
		return;

	
	AppDebug_AppLog("CTLS_Callback_PromptTransResult=[%d], FallbackFlag=[%d]", ret, FallbackFlag);

	
	//无应用的时候不需要走手机芯片了，也就不需要直接返回 by XC 2015/7/9 19:01:25
#if 0
	
	if(NoAppPromptFlag == 0 && ret == CONTACTLESS_NOAPP)
		return;
		
#endif
		
	//AppUtils_FlushKb();
	AppUtils_NormalBeep();
	AppUtils_NormalBeep();
	AppUtils_WaitKey(2);
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: CTLS_Callback_PromptTTRMsg
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN:
 *----------------------------------------------------------------------*/
void CTLS_Callback_PromptTTRMsg(byte *PAN, int PANlen, byte *Amount, int iType)
{
	int i;
	int len;
	char cPanBuf[ 26 ];
	char *p;

	AppDebug_AppLog("CtlsCall_PromptTTRMsg iType[%d]", iType);
	

	
	memset(cPanBuf, 0, sizeof(cPanBuf));
	
	for(len = 0; len < PANlen && len < 20; len ++)
		sprintf(cPanBuf + strlen(cPanBuf), "%02X", PAN[ len ]);
		
	len *= 2;
	p = strchr(cPanBuf, 'F');
	
	if(p)
	{
		i = p - cPanBuf;
		cPanBuf[ i ] = 0;
		len --;
	}
	
	AppDebug_AppLog("CtlsCall_PromptDisplayTTRMsg,PAN[%d]:%s", len, cPanBuf);
	
//  本身就是脱机交易，可以不需要屏蔽卡号。
//  for(i = 4; i < len - 4; i ++)
//    cPanBuf[ i ] = '*';

	if(AppUtils_IsColorScreen())
		AppUtils_DispEnglish(cPanBuf, 1, 3);
	else
		AppUtils_DispEnglish(cPanBuf, 1, 4);
		

	return;
}


void CTLS_Callback_SetNoAppPrompt(int Flag)
{
	NoAppPromptFlag = Flag;
}

/* -----------------------------------------------------------------------------
 * Public Function define
 ---------------------------------------------------------------------------- */
/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_Initiation
 * DESCRIPTION:     非接初始化(回调函数和设备类型)
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_Initiation(void *dummy)
{

	
	return BANK_OK;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_ConnectReader
 * DESCRIPTION:     连接读卡器
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_ConnectReader(void *dummy)
{
	int iRv;
	
	if(CTLS_Initiation((void *)0) != BANK_OK)
		return BANK_FAIL;
		
	//外置PP1000CTLS设备 需要先切换到读卡器模式
	if(gTermCfg.mFixPrm.CtlsInterface != CTLS_IF_INTERNAL)
	{
		//增加判断，避免机器设置成内置pinpad，外置读卡器的情况
		if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
		{
			AppUtils_Warning("内置键盘无法使用外置读卡器");
			return BANK_FAIL;
		}
		

		{
			AppUtils_Warning("连接读卡器失败");
			return BANK_FAIL;
		}
		
		//2012.12.20 Yaping说至少延时900ms add by XC
		AppUtils_SVCWAIT(1000); // by XC 2015/3/5 16:41:59
	}
	
	CtlsReaderModeFlag = 1;
	
	//  AppUtils_DisplayLine( "打开读卡器", LINE2 );
	//
	//  if( Contactless_Open(( char * )CtlsDevName[gTermCfg.mFixPrm.CtlsInterface] ) != CONTACTLESS_OK )
	//  {
	//      AppUtils_Warning( "打开读卡设备失败" );
	//      return BANK_FAIL;
	//  }
	
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayLine("连接非接读卡器", LINE2);
	
	AppDebug_AppLog("ReaderDev=[%s]", (char *)CtlsDevName[gTermCfg.mFixPrm.CtlsInterface]);

	{
		AppDebug_AppLog("Contactless_Connect=[%d]", iRv);
		AppUtils_Warning("连接读卡器失败");
		return BANK_FAIL;
	}
	
	//  if( Contactless_PingReader() != CONTACTLESS_OK )
	//  {
	//      AppUtils_Warning( "访问读卡器失败" );
	//      return BANK_FAIL;
	//  }
	
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_ReconnectReader
 * DESCRIPTION:     重连读卡器, 主要用于非接圈存类交易第二次交互
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_ReconnectReader(void *dummy)
{
	int iRv;
	
	//外置PP1000CTLS设备 需要先切换到读卡器模式
	if(gTermCfg.mFixPrm.CtlsInterface != CTLS_IF_INTERNAL)
	{
		//增加判断，避免机器设置成内置pinpad，外置读卡器的情况
		if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
		{
			AppUtils_Warning("内置键盘无法使用外置读卡器");
			return BANK_FAIL;
		}

		{
			AppUtils_Warning("连接读卡器失败");
			return BANK_FAIL;
		}
		
		//2012.12.20 Yaping说至少延时900ms add by XC
		AppUtils_SVCWAIT(1000); // by XC 2015/3/5 16:41:59
	}
	
	CtlsReaderModeFlag = 1;
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_InitCtlsCardCheck
* DESCRIPTION:    传入交易金额，非阻塞式寻卡
* INPUTS:
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_InitCtlsCardCheck(DynData *pTxnData)
{
	byte AmtBuf[6+1], bTransType;
	
	memset(AmtBuf, 0, sizeof(AmtBuf));
	
	//Contactless_SetPollMode( 1 );
	
	if(pTxnData != NULL)
	{
		AppDebug_AppLog("pTxnData->TransDB.Amount=[%s]", pTxnData->TransDB.Amount);
		AppUtils_Asc2Bcd((byte *)pTxnData->TransDB.Amount, AmtBuf, 12);
	}
	



	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_CancelTransatcion
* DESCRIPTION:    非接交易取消
* INPUTS:
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_CancelTransatcion(void)
{

	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_PingReader
* DESCRIPTION:    发送Ping指令到读卡器
* INPUTS:
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_PingReader(void)
{
	int iRv;
	

		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_Switch2PPMode
* DESCRIPTION:    切换读卡器到密码键盘模式
* INPUTS:
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_Switch2PPMode(void)
{
	int iRv;
	
	if(CtlsReaderModeFlag == 0)
		return BANK_OK;
		
	if(gTermCfg.mTrSwitch.CTLS_Support == 0)
		return BANK_OK;
		
	if(gTermCfg.mFixPrm.CtlsInterface != CTLS_IF_INTERNAL)
	{
		AppUtils_SVCWAIT(100);
		

		//AppUtils_SVCWAIT(200);
		AppUtils_SVCWAIT(400);  //12.96 FW

	}
	
	CtlsReaderModeFlag = 0;
	return BANK_OK;
}


/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_ProcessQPBOC
* DESCRIPTION:    处理qPBOC交易
* INPUTS:         pTxnData: 交易数据
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_ProcessQPBOC(DynData *pTxnData)
{

		return BANK_FAIL;

}




/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_LoadValue
* DESCRIPTION:    圈存脚本处理
* INPUTS:         pField55Data: 55域数据
*                 Field55Length: 55域长度
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_LoadValue(byte *pField55Data, int Field55Length, DynData *pTxnData)
{
	char AuthCode[2];
	int  iRet;

	
	return BANK_OK;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_AIDSaveParameter
 * DESCRIPTION:     save taid parameter to ctls device
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_AIDSaveParameter(vfiTerminalAppListCtls *pCtlsAppList)
{

	
	return BANK_OK;
}




/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_DeleteAllCAPK
 * DESCRIPTION:     delete all CAPK
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_DeleteAllCAPK(void)
{

		
	return BANK_FAIL;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_DeleteAllBlackCardList
 * DESCRIPTION:     delete all black card bin list
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_DeleteAllBlackCardList(void)
{

		
	return BANK_FAIL;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_SetBlackCardList
 * DESCRIPTION:     save black card bin list to ctls device
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_SetBlackCardList(char *pBlackCardFileName)
{

	return BANK_FAIL;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_SetEMVParameters
* DESCRIPTION:    设置读卡器参数
* INPUTS:
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_SetEMVParameters(void)
{

		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_SetPBOCSupport
* DESCRIPTION:    重新封装Contactless_SetPBOCSupport()函数
                  在失败时增加1s延时目的在于使得后面的切换读卡器模式到密码键盘模式指令能正常响应
* INPUTS:         flag: 0-不支持非接PBOC(即支持qPBOC)， 1-支持非接PBOC
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_SetPBOCSupport(int flag)
{

	return BANK_FAIL;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_ProcessMChip
* DESCRIPTION:    处理MCHIP手机芯片交易
* INPUTS:         pTxnData: 交易数据
*                 dummy: (void*)0-需检测读卡器再寻卡, 其他值直接寻卡
* RETURN:
* ------------------------------------------------------------------------ */
int CTLS_ProcessMChip(DynData *pTxnData, void *dummy)
{
	CUPMobile_CardInfo MCardInfo;
	byte MCardSn[16], MDateTime[8], cBuffer[32];
	int i;
	int iOffset;
	int iLength;
	
	AppUtils_UserClear(2);
	AppUtils_DisplayCenter((char *)"请挥卡(或放手机)", LINE2, FALSE);
	
	memset(&MCardInfo, 0, sizeof(CUPMobile_CardInfo));
	memset(MCardSn, 0, sizeof(MCardSn));
	memset(MDateTime, 0, sizeof(MDateTime));

	
	
	/*控制卡号长度*/
	
	/*循环去除卡号的最后F*/
	for(i = 19; i > 12 ;)
	{
		if(cBuffer[i - 1] == 'F')
			i--;
		else
			break;
	}
	
	pTxnData->TransDB.PANLen = i;
	memcpy(pTxnData->TransDB.PAN, cBuffer, pTxnData->TransDB.PANLen);
	
	AppUtils_Bcd2Len(MCardInfo.Card2Buf, &iLength, 1);
	AppUtils_Bcd2Asc((byte *)MCardInfo.Card2Buf + 1, (byte *)pTxnData->TransDB.Track2Data, iLength);
	
	AppUtils_Bcd2Len(MCardInfo.Card3Buf, &iLength, 2);
	AppUtils_Bcd2Asc((byte *)MCardInfo.Card3Buf + 2, (byte *)pTxnData->TransDB.Track3Data, iLength);
	
	iOffset = 0;
	
	//DF 32 芯片序列号
	memcpy(pTxnData->AddBuf + iOffset, "\xDF\x32\x0A", 3);
	iOffset += 3;
	memcpy(pTxnData->AddBuf + iOffset, MCardSn, 10);
	iOffset += 10;
	
	//DF 33 过程密钥数据
	memcpy(pTxnData->AddBuf + iOffset, "\xDF\x33\x08", 3);
	iOffset += 3;
	memcpy(pTxnData->AddBuf + iOffset, MCardInfo.ProcessKey, 8);
	iOffset += 8;
	
	//DF 34 磁道读取时间
	memcpy(pTxnData->AddBuf + iOffset, "\xDF\x34\x07", 3);
	iOffset += 3;
	memcpy(pTxnData->AddBuf + iOffset, MDateTime, 7);
	iOffset += 7;
	
	//长度
	pTxnData->AddBufLen = iOffset;
	
	// backup some data for settlement upload
	memcpy(pTxnData->TransDB.IssSript71Data, pTxnData->AddBuf, pTxnData->AddBufLen);
	pTxnData->TransDB.IssSript71DataLength = pTxnData->AddBufLen;
	
	//卡有效期
	memcpy(pTxnData->TransDB.ExpDate + 1, (byte *)MCardInfo.CardExpDate, 2);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME:  CTLS_ReadRFCard
* DESCRIPTION:    读CUPMOBILE手机芯片卡处理流程
* PARAMETERS:
        OutCardInfo[out] - 读取的卡信息
        bCardSerialNo[out] - 物理卡号
        bTransDateTime[out] - 交易日期时间
* RETURN:

* NOTES:        选择功能后作控制
* --------------------------------------------------------------------------*/
int CTLS_ReadRFCard(CUPMobile_CardInfo *stOutCardInfo, byte *bCardSerialNo, byte *bTransDateTime, void *dummy)
{
#if 0 //模拟测试信息
	AppUtils_UserClear(2);
	AppUtils_DisplayCenter((char *)"请挥卡(或放手机)", LINE2, FALSE);
	
	memcpy(stOutCardInfo->CardNum, "\x62\x00\x87\x65\x43\x21\x11\x22\x33\x00", 10);
	memcpy(stOutCardInfo->Card2Buf, "90011112222333344445", 20);
	memcpy(stOutCardInfo->Card3Buf, "001111222233334444555566667777888899998888777766665533", 54);
	
	stOutCardInfo->Card2Buf[0] = 0x00;
	
	memcpy(stOutCardInfo->ProcessKey, "01234567", 8);
	
	memcpy(bCardSerialNo, "8765432112345678", 16);
	
	memcpy(bTransDateTime, "\x20\x11\x08\x04\x23\x55\x00", 8);
	
	AppUtils_WaitKey(30);
	return ICCARD_OK;
	
#else
	
	int iRet;
	byte  bDataLen, bMF_FCI[256];
	
	if(dummy == (void *)0)
	{
		AppUtils_UserClear(2);
		AppUtils_DisplayCenter((char *)"检测读卡器请稍候", LINE2, FALSE);
	
		/*初始化串口并切换为读卡器模式*/
		iRet = CTLS_ConnectReader((void *)0);
	
		if(iRet != BANK_OK)
		{
			AppUtils_Warning((char *)"连接读卡器失败  请检查读卡器");
			return ICCARD_COMMERR;
		}
	
		AppUtils_UserClear(2);
	
		if(AppUtils_IsRotatedScreen() == BANK_OK)
		{
			AppUtils_DisplayLine((char *)"  请挥卡", LINE2);
			AppUtils_DisplayLine((char *)"(或放手机)", LINE3);
			AppUtils_DispBMP(FLASH_PREFIX "QTAP.BMP", 150, 100, 320, 200);
		}
		else if(AppUtils_IsTouchScreen() == BANK_OK)
		{
			AppUtils_DisplayCenter((char *)"请挥卡(或放手机)", LINE6, FALSE);
			AppUtils_DispBMP(FLASH_PREFIX "QTAP.BMP", 90, 90, 320, 200);
		}
		else
		{
			AppUtils_DisplayCenter((char *)"请挥卡(或放手机)", LINE2, FALSE);
		}
	}
	
	/*寻卡*/
	iRet = ICCmdCTLS_OrgCheckICC(0);
	
	if(iRet != ICCARD_OK)
	{
		return iRet;
	}
	
	AppUtils_UserClear(2);
	AppUtils_DisplayCenter((char *)"正在处理", LINE2, FALSE);
	AppUtils_DisplayCenter((char *)"请稍候...", LINE3, FALSE);
	
	/*读取卡内信*/
	/*选择电子钱包应用*/
	memset(bMF_FCI, 0, sizeof(bMF_FCI));
	bDataLen = 15;
	iRet = ICCmdCTLS_Select((unsigned char *)EP_NAME, bDataLen, TRUE, bMF_FCI, sizeof(bMF_FCI), 0);
	
	if(iRet <= 0)
	{
		ICCmdCTLS_EndICComm(0);
		AppUtils_Warning((char *)"选择电子钱包应用失败");
		return ICCARD_CARD_ERROR;
	}
	
	/*获取卡号*/
	memset(bMF_FCI, 0, sizeof(bMF_FCI));
	bDataLen = 0;
	iRet = ICCmdCTLS_ReadBinarySH(CARDINFO_SFI, 0, bMF_FCI, bDataLen, 0);
	
	if(iRet <= 0)
	{
		ICCmdCTLS_EndICComm(0);
		AppUtils_Warning((char *)"读取卡号失败");
		return ICCARD_CARD_ERROR;
	}
	
	memcpy(bCardSerialNo, bMF_FCI, 16);
	
	
	/*获取默认支付银行卡记录*/
	memset(bMF_FCI, 0, sizeof(bMF_FCI));
	bDataLen = 0;
	iRet = ICCmdCTLS_ReadBinarySH(0x03, 0, bMF_FCI, bDataLen, 0);
	
	if(iRet <= 0)
	{
		ICCmdCTLS_EndICComm(0);
		AppUtils_Warning((char *)"读取默认支付银行卡记录失败");
		return ICCARD_CARD_ERROR;
	}
	
	/*获取交易日期时间*/
	memset(bMF_FCI, 0, sizeof(bMF_FCI));
	AppUtils_GetClock((char *)bMF_FCI);
	AppUtils_Asc2Bcd(bMF_FCI, bTransDateTime, 14);
	
	memset(bMF_FCI, 0, sizeof(bMF_FCI));
	bDataLen = 255;
	iRet = ICCmdCTLS_GetBankCardInfo(bTransDateTime, bMF_FCI, bDataLen, 0);
	
	if(iRet <= 0)
	{
		ICCmdCTLS_EndICComm(0);
		AppUtils_Warning((char *)"读取卡信息失败");
		return ICCARD_CARD_ERROR;
	}
	
	memcpy((byte *)stOutCardInfo, bMF_FCI, sizeof(CUPMobile_CardInfo));
	
	ICCmdCTLS_EndICComm(0);
	
	AppUtils_NormalBeep();
	
	return ICCARD_OK;
#endif
}


void CTLS_CloseReadTag9F51(void *dummy)
{
		
	return;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_GetAIDListInfo
 * DESCRIPTION:     save taid parameter to ctls device
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void CTLS_GetAIDListInfo(void)
{
	//int iRet = 0;
	int fp = 0;
	int iSize = 0;
	
	if((fp = open(CTLSAIDLIST_FILE, O_RDONLY)) > 0)
	{
		AppDebug_AppLog("Open CTLS_AID_LIST ok");
		memset(&g_globctlsAIDList, 0, sizeof(vfiTerminalAppListCtls));
		iSize = read(fp, (char*)&g_globctlsAIDList, sizeof(vfiTerminalAppListCtls));
		
		if(iSize > 0)
		{
			if(iSize != sizeof(vfiTerminalAppListCtls))
			{
				AppDebug_AppLog("Read size[%d], not[%d]", iSize, sizeof(vfiTerminalAppListCtls));
			}
		}
		else
		{
			AppDebug_AppLog("Read CTLS_AID_LIST fail");
		}
		
		AppDebug_AppLog("Read CTLS_AID_LIST Finished");
		close(fp);
	}
	
	return ;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_SaveAIDListInfo
 * DESCRIPTION:     save taid parameter to ctls device
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void CTLS_SaveAIDListInfo(vfiTerminalAppListCtls *pCtlsAppList)
{
	//int iRet = 0;
	int fp = 0;
	
	if((fp = open(CTLSAIDLIST_FILE, O_WRONLY | O_CREAT | O_TRUNC)) > 0)
	{
		lseek(fp, 0L, SEEK_SET);
		write(fp, (char*)pCtlsAppList, sizeof(vfiTerminalAppListCtls));
		close(fp);
	}
	
	return ;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_GetBalanceFromIAD
 * DESCRIPTION:     Get Balance from IAD
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_GetBalanceFromIAD(byte *IAD, int IADLen, byte *BalBuf)
{
	byte *Offset;
	byte bLen;
	
	Offset = IAD;
	
	bLen = *Offset;
	Offset ++;  //跳过长度
	Offset += bLen;  //跳过数据
	
	while((Offset - IAD) < IADLen)
	{
		bLen = *Offset;
		Offset++;
		
		if(*Offset == 0x01)    //Found 9f79
		{
			memcpy(BalBuf + 1, Offset + 1, 5);
			return BANK_OK;
		}
		else if(*Offset == 0x05)   //Found 9f5d
		{
			memcpy(BalBuf + 1, Offset + 1, 5);
			return BANK_OK;
		}
		
		Offset += bLen;
	}
	
	return BANK_FAIL;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   CTLS_SetPriorityOption
 * DESCRIPTION:     iOpt = 0: qPBOC强制走联机
                  iOpt = 1: qPBOC优先走脱机
                  iOpt = 2: 非接PBOC
 * RETURN:          BANK_OK: success
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int CTLS_SetPriorityOption(int iOpt)
{

	return BANK_OK;
}

char *CTLS_GetCurrencyName(byte *pCurrencyCode)
{
	static Prn_Type CurrencyList[] =
	{
		{"\x01\x56", "RMB"},
		{"\x03\x44", "HKD"},
		{"\x04\x46", "MOP"},
		{NULL, NULL}
	};
	int i ;
	
	for(i = 0 ; CurrencyList[i].eng != NULL; i++)
	{
		if(!memcmp(pCurrencyCode, CurrencyList[i].eng, 2))
		{
			return CurrencyList[i].chn;
		}
	}
	
	return NULL;
}

int CTLS_SetSencondCurrencyFlag(void *dummy)
{


    return 1;
}

