/****************************************************************************
 * FILE NAME:   COMM.C                                                      *
 * MODULE NAME: COMM                                                        *
 * PROGRAMMER:  XC                                                          *
 * DESCRIPTION: 通讯处理模块                                                *
 * REVISION:                                                                *
 ****************************************************************************/
#include "cupms.h"
#include "transsocket.h"
#include "detaildialog.h"

/* -----------------------------------------------------------------------------
 * Public Variables declare
 ---------------------------------------------------------------------------- */
extern Term_Config gTermCfg;

/* -----------------------------------------------------------------------------
 * Private Function declare
 ---------------------------------------------------------------------------- */
//static int inComm_GetConnectParams(int CurrCommMode, CommCnt_Param *pCntPrm, int PriSecOffset);

#if defined(TRIDENT) || defined(MAKE_LINUX)
static int inComm_UpdateSSLServerIP(int CurrCommMode, CommCnt_Param *pCntPrm);
#endif

/* -----------------------------------------------------------------------------
 * Private Variables declare
 ---------------------------------------------------------------------------- */
static char SSLServerDomainName[64+1];
static char SSLServerIPAddress[15+1];

/* -----------------------------------------------------------------------------
 * Public Function define
 ---------------------------------------------------------------------------- */


/*-----------------------------------------------------------------------------
* FUNCTION NAME:   Comm_AppInitComm
* DESCRIPTION:     应用初始化网络
* PARAMETERS:      无
* RETURN:          无
*
*-----------------------------------------------------------------------------*/
void Comm_AppInitComm(void)
{
	//获取modem 句柄和初始化modem。通讯库内部会处理，应用程序可以不执行
	//by Simon 2014/10/28
	//VComm_Init(gTermCfg.mCommPrm.CurrentCommMode, TRUE);
	
	switch(gTermCfg.mCommPrm.CurrentCommMode)
	{
	case COMM_TYPE_ETHERNET:
	case COMM_TYPE_GPRS:
#ifdef TRIDENT
	case COMM_TYPE_CDMA:
	case COMM_TYPE_WIFI:
#endif
		Comm_RegisterNetwork(1);   //采用背景方式
		break;
	case COMM_TYPE_SDLC:
		Comm_SetDialAtCmd();
		break;
	default:
		break;
	}
	
	return;
}

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_RegisterNetwork
 * DESCRIPTION:     仅对于TCP类通讯方式－注册网络
 * PARAMETERS:      BackGroundFlag: 1为背景方式注册; 0为前景方式注册
 * RETURN:          COMM_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_RegisterNetwork(int BackGroundFlag)
{

    return 1;
}


/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_LogoutNetwork
 * DESCRIPTION:     仅对于TCP类通讯方式登出网络
 *
 * RETURN:          COMM_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_LogoutNetwork(int BackGroundFlag)
{
	int iCurrCommMode;
	
	iCurrCommMode = gTermCfg.mCommPrm.CurrentCommMode;
	
	if(iCurrCommMode != COMM_TYPE_ETHERNET && iCurrCommMode != COMM_TYPE_GPRS
#ifdef TRIDENT
	   && iCurrCommMode != COMM_TYPE_CDMA && iCurrCommMode != COMM_TYPE_WIFI
#endif
	  )
	{
		return COMM_RET_FAIL;
	}
	
    return 2;
}

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_CheckLineStatus
 * DESCRIPTION:     检查线路状态
 *
 * RETURN:          BANK_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_CheckLineStatus(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_CheckPreConnect
 * DESCRIPTION:     检查预拨号：如果终端当前为拨号通讯方式，且预拨号开关打开，
 *                  则进行预拨号
 * RETURN:          COMM_RET_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_CheckPreConnect(void)
{

	return(COMM_RET_OK);
}

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_CheckConnect
 * DESCRIPTION:     通讯连接：对于拨号方式为拨号；对于TCP通讯方式为建立socket
 *                  对于TCP类的通讯方式如果尚未注册网络本函数会先调用注册网络。
 *                  在发送和接收数据之前应调用本函数并判返回值
 * RETURN:          BANK_OK
 *                  BANK_FAIL
 *                  BANK_ABORT
 *-----------------------------------------------------------------------------*/
int Comm_CheckConnect(void)
{
    int iRet;

    DetailDialog::getInstance()->setText(2,QString("链接中..."),1);
    strcpy((char *)gTermCfg.mCommPrm.EthHostIP1, "192.168.1.161");
    gTermCfg.mCommPrm.EthHostPort1 = 6001;
    qDebug("IP:%s",gTermCfg.mCommPrm.EthHostIP1);
    qDebug("PORT:%d",gTermCfg.mCommPrm.EthHostPort1);
    iRet = transSocket::getInstance()->connectSocket((char *)gTermCfg.mCommPrm.EthHostIP1, gTermCfg.mCommPrm.EthHostPort1, 3000);

    if(iRet == BANK_OK)
        return BANK_OK;
    else
        return BANK_FAIL;
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_TCP_Disconnect
 DESCRIPTION:   TCP通讯方式挂线
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_TCP_Disconnect(void)
{
	int iCurrCommMode;
	
	iCurrCommMode = gTermCfg.mCommPrm.CurrentCommMode;
	
	switch(iCurrCommMode)
	{
	case COMM_TYPE_ETHERNET:
		break;
	case COMM_TYPE_GPRS:
		break;
#ifdef TRIDENT
	case COMM_TYPE_CDMA:
		break;
		
	case COMM_TYPE_WIFI:
		break;
#endif
	default:
		return BANK_OK;
	}
	
	return Comm_Disconnect();
}

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_Disconnect
 * DESCRIPTION:     通讯断开：对于拨号方式为挂机；对于TCP通讯方式为释放socket
 *
 * RETURN:          BANK_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_Disconnect(void)
{
	int iCurrCommMode;
    gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_ETHERNET;
	iCurrCommMode = gTermCfg.mCommPrm.CurrentCommMode;
	
	switch(iCurrCommMode)
	{
	case COMM_TYPE_SDLC:
		iCurrCommMode = COMM_TYPE_SDLC;
		break;
		
	case COMM_TYPE_ETHERNET:
		iCurrCommMode = COMM_TYPE_ETHERNET;
		break;
		
	case COMM_TYPE_GPRS:
		iCurrCommMode = COMM_TYPE_GPRS;
		break;
		
#ifdef TRIDENT
	case COMM_TYPE_CDMA:
		iCurrCommMode = COMM_TYPE_CDMA;
		break;
		
	case COMM_TYPE_WIFI:
		iCurrCommMode = COMM_TYPE_WIFI;
		return BANK_FAIL;
#endif
		
	case COMM_TYPE_RS232:
		//AppUtils_CloseRS232();
		return BANK_OK;
		
	default:
		return BANK_FAIL;
	}
	
    transSocket::getInstance()->closeSocket();
    return BANK_OK;
}


/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_SendPacket
 * DESCRIPTION:     发送数据
 *
 * RETURN:          BANK_OK: 发送成功
 *                  BANK_FAIL: 发送失败
 *-----------------------------------------------------------------------------*/
int Comm_SendPacket(byte *Data, int DataLen)
{
	int iCurrCommMode, iFmt, iRet;
	char cRS232Name[ 26 ];
    char *temp;
    int sendlen = 0;
    char sendbuff[4096] = {0};

    sendlen = DataLen;
    temp = (char *)&sendlen;
    memset(sendbuff, 0, sizeof(sendbuff));
    sendbuff[0] = temp[1];
    sendbuff[1] = temp[0];
    memcpy(sendbuff+2, Data, DataLen);
    sendlen += 2;

	AppUtils_DisplayLine("已连接银联中心", LINE2);
	AppUtils_DisplayLine("处理中...", LINE3);
	AppUtils_DisplayLine("发送数据包", LINE4);
    AppUtils_WaitKey(10);
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_PACKETDATA) == BANK_OK)
        AppDebug_OutputHexData("发送数据包", (char *)sendbuff, sendlen);
		
	if(gTermCfg.mTrSwitch.PacketPrintDebugFlag)
	{
		AppUtils_DisplayLine("是否打印请求报文", LINE2);
		AppUtils_DisplayLine("  [O]是 [X]否", LINE3);
        iRet = AppUtils_get_WaitKey(15);
		
		if(iRet == bENTER || iRet == bKEY0)
			PrnDisp_PrintWidthHEXASC("Send Packet:", (byte *)Data, DataLen);
	}

    gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_ETHERNET;
	iCurrCommMode = gTermCfg.mCommPrm.CurrentCommMode;
	
	switch(iCurrCommMode)
	{
		// (TCPIP通讯方式) 以太网, GPRS, CDMA, WIFI
	case COMM_TYPE_ETHERNET:
	case COMM_TYPE_GPRS:
#ifdef TRIDENT
	case COMM_TYPE_CDMA:
	case COMM_TYPE_WIFI:
#endif
        iRet = transSocket::getInstance()->writeSocket((byte *)sendbuff, sendlen, 3000);
        if(iRet == BANK_OK)
        {
            return BANK_OK;
        }
        else
		{
			Comm_Disconnect();
			
			PrnDisp_DisplayBankCodeError(BANK_SENDERROR);
			
			return BANK_FAIL;
		}
		
		//同步拨号
	case COMM_TYPE_SDLC:

		
		// RS232
	case COMM_TYPE_RS232:
		memset(cRS232Name, 0, sizeof(cRS232Name));

		


		
	default:

		return BANK_FAIL;
	}
}

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_RecvPacket
 * DESCRIPTION:     接收数据
 *
 * RETURN:          BANK_OK: 接收成功
 *                  BANK_FAIL: 接收失败
 *                  BANK_TIMEOUT: 接收超时
 *-----------------------------------------------------------------------------*/
int Comm_RecvPacket(byte *Data, int *DataLen)
{
    int iRet;
    int   iRecvLen = 4096;
    byte  RecvBuf[MAX_COMM_BUF_SIZE];

    AppUtils_DisplayLine("已连接银联中心", LINE2);
    AppUtils_DisplayLine("处理中...", LINE3);
    AppUtils_DisplayLine("接收数据包", LINE4);

    qDebug("recv start");
    iRet = transSocket::getInstance()->readSocket(RecvBuf, 30000, &iRecvLen);
    qDebug("recv [%d]", iRet);
    if((iRet == BANK_OK) && (iRecvLen < 21))
        iRet = BANK_FAIL;


    if(iRet == BANK_OK)
    {
        *DataLen = iRecvLen-2;
        memcpy(Data, RecvBuf+2, iRecvLen-2);
        AppUtils_WaitKey(10);
        //return iRet;
    }
    else if(iRet == BANK_TIMEOUT)
    {
        Comm_Disconnect();

        PrnDisp_DisplayBankCodeError(BANK_TIMEOUT);

    }
    else
    {
        Comm_Disconnect();

        PrnDisp_DisplayBankCodeError(BANK_RECVERROR);

        return BANK_FAIL;
    }

    if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_PACKETDATA) == BANK_OK)
        AppDebug_OutputHexData("接收数据包", (char *)Data, iRecvLen);

    if(gTermCfg.mTrSwitch.PacketPrintDebugFlag)
    {
        AppUtils_DisplayLine("是否打印返回报文", LINE2);
        AppUtils_DisplayLine("  [O]是 [X]否", LINE3);
        iRet = AppUtils_get_WaitKey(30);

        if(iRet == bENTER || iRet == bKEY0)
            PrnDisp_PrintWidthHEXASC("Recv Packet:", (byte *)Data, iRecvLen);
    }


    return BANK_OK;
}


//TRIDENT平台或Cx(Linux)平台才支持SSL
#if defined(TRIDENT) || defined(MAKE_LINUX)
/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_HostVerifyPos
 DESCRIPTION:   SSL双向认证方式，平台验证终端
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_HostVerifyPos(int CommType)
{
	int iRet, iSendLen, iRecvLen, iTimeOut;
	int iMainAppMKeyIndex;
	char cTempBuf[128];
	byte Random[16], AuthKey[16], E_Data[24];
	byte bSendData[128], bRecvData[128];
	
	memset(Random, 0, sizeof(Random));
	memset(AuthKey, 0, sizeof(AuthKey));
	
	iRet = PrmMngr_GetMainAppMkeyIndex();
	
	if((iRet < 0) || (iRet > 99))
	{
		iMainAppMKeyIndex = 7; //主控主密钥索引默认用7
	}
	else
	{
		iMainAppMKeyIndex = iRet;
	}
	
	//检测主控主密钥是否存在
	if(Pinpad_CheckMKey(iMainAppMKeyIndex) !=  VS_SUCCESS)
	{
		AppUtils_FormatWarning("主控密钥%d不存在", iMainAppMKeyIndex);
		return COMM_RET_FAIL;
	}
	
	//随机数产生方式: 使用MACKey工作密钥密文前16个字节对时间做双倍长DES
	memset(cTempBuf, 0, sizeof(cTempBuf));
	AppUtils_GetClock(cTempBuf);
	BankUtil_DES(TDES2KE, gTermCfg.mDynPrm.MACKey, (byte*)cTempBuf, Random);
	BankUtil_DES(TDES2KE, gTermCfg.mDynPrm.MACKey, (byte*)cTempBuf + 8, Random + 8);
	
	
	//用主控主密钥对随机数做3DES加密，得到认证密钥
	Pinpad_DesByMKey(Random, AuthKey, 1, iMainAppMKeyIndex);
	Pinpad_DesByMKey(Random + 8, AuthKey + 8, 1, iMainAppMKeyIndex);
	
	//24字节加密数据：20字节厂商机型 + 补4字节空格
	memset(E_Data, 0, sizeof(E_Data));
	memset(cTempBuf, 0, sizeof(cTempBuf));
	sprintf(cTempBuf, "%s", AppUtils_GetTermModuleName());
	AppUtils_FillBufWithChar(cTempBuf, ' ', 24);
	
	//用认证密钥对以上加密数据做3DES得到认证密文
	BankUtil_DES(TDES2KE, AuthKey, (byte*)cTempBuf, E_Data);
	BankUtil_DES(TDES2KE, AuthKey, (byte*)cTempBuf + 8, E_Data + 8);
	BankUtil_DES(TDES2KE, AuthKey, (byte*)cTempBuf + 16, E_Data + 16);
	
	
	//组织整个认证报文：厂商机型(20) + 硬件序列号(38) + 认证密文(24) + 随机数(16) + 校验位(1)
	memset(bSendData, 0, sizeof(bSendData));
	iSendLen = 0;
	
	//厂商机型 20
	memcpy(bSendData + iSendLen, cTempBuf, 20);
	iSendLen += 20;
	
	//硬件序列号 38
	memset(cTempBuf, 0, sizeof(cTempBuf));
	AppUtils_GetSerialNo(cTempBuf);
	AppUtils_FillBufWithChar(cTempBuf, ' ', 38);
	memcpy(bSendData + iSendLen, cTempBuf, 38);
	iSendLen += 38;
	
	//认证密文 24
	memcpy(bSendData + iSendLen, E_Data, 24);
	iSendLen += 24;
	
	//随机数 16
	memcpy(bSendData + iSendLen, Random, 16);
	iSendLen += 16;
	
	bSendData[iSendLen] = BankUtil_GenLrc(Random, 16);
	iSendLen ++;
	
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayLine("双向认证", LINE2);
	AppUtils_DisplayLine("发送数据...", LINE3);
	
	if(gTermCfg.mTrSwitch.PacketPrintDebugFlag)
	{
		AppUtils_DisplayLine("打印认证请求包?", LINE2);
		AppUtils_DisplayCenter("[O]是 [X]否", LINE3, FALSE);
		
		switch(MmiUtil_WaitKey(30))
		{
		case bENTER:
		case bKEY0:
			PrnDisp_PrintWidthHEXASC("Verify Send data:", (unsigned char*)bSendData, iSendLen);
			break;
		case bESC:
		case 0:
		default:
			break;
		}
	}
	
	iRet = Comm_SSL_Send(CommType, bSendData, iSendLen, COMM_FMT_HEXLEN_DATA);
	
	if(iRet != COMM_OK)
	{
		AppUtils_FormatWarning("发送失败[%d]", iRet);
		//Comm_GetErrCode(iRet, CommType);
		return COMM_RET_SEND_FAIL;
	}
	
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayLine("双向认证", LINE2);
	AppUtils_DisplayLine("接收数据...", LINE3);
	
	switch(CommType)
	{
	case COMM_TYPE_ETHERNET:
		iTimeOut = gTermCfg.mCommPrm.EthTimeout;
		break;
	case COMM_TYPE_GPRS:
		iTimeOut = gTermCfg.mCommPrm.GprsTimeout;
		break;
#ifdef TRIDENT
	case COMM_TYPE_CDMA:
		iTimeOut = gTermCfg.mCommPrm.CdmaTimeout;
		break;
	case COMM_TYPE_WIFI:
		iTimeOut = gTermCfg.mCommPrm.WifiTimeout;
		break;
#endif
	default:
		iTimeOut = 50;
		break;
		
	}
	
	if(iTimeOut <= 0 || iTimeOut > 99)
		iTimeOut = 50;
		
	memset(bRecvData, 0, sizeof(bRecvData));
	iRecvLen = 0;
	iRet = Comm_SSL_Recv(CommType, bRecvData, sizeof(bRecvData), &iRecvLen, COMM_FMT_HEXLEN_DATA, iTimeOut);
	
	if(iRet != COMM_OK)
	{
		Comm_GetErrCode(iRet, CommType);
		return COMM_RET_RECV_FAIL;
	}
	
	if(gTermCfg.mTrSwitch.PacketPrintDebugFlag)
	{
		AppUtils_DisplayLine("打印认证返回包?", LINE2);
		AppUtils_DisplayCenter("[O]是 [X]否", LINE3, FALSE);
		
		switch(MmiUtil_WaitKey(30))
		{
		case bENTER:
		case bKEY0:
			PrnDisp_PrintWidthHEXASC("Verify Recv data:", (unsigned char*)bRecvData, iRecvLen);
			break;
		case bESC:
		case 0:
		default:
			break;
		}
	}
	
	
	if(iRecvLen < 2)
	{
		AppUtils_Warning("返回数据长度错误");
		return COMM_RET_RECV_FAIL;
	}
	
	if(!memcmp(bRecvData, "00", 2))
	{
		return COMM_RET_OK;
	}
	else
	{
		memset(cTempBuf, 0, sizeof(cTempBuf));
		strncpy(cTempBuf, (char*)bRecvData + 2, 32);
		AppUtils_DeleteTrail(' ', cTempBuf);
		
		if(strlen(cTempBuf) == 0)
		{
			AppUtils_FormatWarning("[%c%c]认证失败", (char)bRecvData[0], (char)bRecvData[1]);
		}
		else
		{
			AppUtils_FormatWarning("[%c%c]%s", (char)bRecvData[0], (char)bRecvData[1], cTempBuf);
		}
		
		return COMM_RET_FAIL;
	}
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_Connect
 DESCRIPTION:   SSL连接主机
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_Connect(int CommType, CommCnt_Param *ConnectParam, BOOL ManualCancel)
{
	VCommLib_SSLConfig AppSSLParam;
	
	//Add on 2015-07-17 for SSL
#ifdef MAKE_LINUX
	VComm_SSLShutdown(CommType, FALSE);
#else
	VCommLib_SSLShutdown();
#endif
	
	AppDebug_AppLog("Start Comm_SSL_Connect...");
	
	memset((char *)&AppSSLParam, 0, sizeof(VCommLib_SSLConfig));
	
	//是否使用SSL协议传输数据( 包括:1=使用,0=不使用 )
	AppSSLParam.iSSLUsed = 1;
	
	//SSL采用的算法标准 SSLV23
	AppSSLParam.iSSLMethod = VCOMMLIB_SSLV23;
	
	//证书文件的类型 *.PEM 文件格式
	AppSSLParam.iCertFileType = VCOMMLIB_SSL_FILETYPE_PEM;
	
	//CA证书寻址的目录深度指数
	//2级CA证书 1级CA证书
	//0不校验CA
	AppSSLParam.iChainDepth = 2;
	
	//客户端是否做证书/私钥合法性校验( 包括:1=校验,0=不校验 )
	//不用提供客户端证书和私钥
	AppSSLParam.iClientIsVerify = 0;
	
	//服务器校验认证类型
	//SSL_VERIFY_NONE 不校验服务端证书
	//SSL_VERIFY_PEER 服务器要验证对方 双向认证
	//SSL_VERIFY_CLIENT_ONCE 客户端需要验证对方，而服务器不需要 单向认证
	AppSSLParam.iServerAuthType = VCOMMLIB_SSL_VERIFY_CLIENT_ONCE;
	
	//加密算法标识符 SSL3_TXT_RSA_RC4_128_SHA  , RC4-SHA
	AppSSLParam.iCipherAlgType = VCOMMLIB_SSL3_TXT_RSA_RC4_128_SHA;
	
	//CA证书 放在group15
	if(strlen(gTermCfg.mFixPrm.SSLCertFileName) > 0)
	{
#ifdef MAKE_LINUX
		sprintf((char *)AppSSLParam.cCACertFile, "/home/Group15/%s", gTermCfg.mFixPrm.SSLCertFileName);
#else
		sprintf((char *)AppSSLParam.cCACertFile, "F:/%s", gTermCfg.mFixPrm.SSLCertFileName);
#endif
	}
	else
	{
#ifdef MAKE_LINUX
		sprintf((char *)AppSSLParam.cCACertFile, "/home/Group15/%s", DEFAULT_SSL_CERT_FILE_NAME);
#else
		sprintf((char *)AppSSLParam.cCACertFile, "F:/%s", DEFAULT_SSL_CERT_FILE_NAME);
#endif
	}
	
	//客户端证书
	strcpy((char *)AppSSLParam.cClCertFile, "");
	
	//客户端私钥
	strcpy((char *)AppSSLParam.cClPrivateKey, "");
	
	//客户端私钥密码
	strcpy((char *)AppSSLParam.cPKeyPassword, "");
	
#ifdef MAKE_LINUX
	return VComm_SSLConnect(CommType, ConnectParam, &AppSSLParam, TRUE);
#else
	return VCommLib_SSLConnect(CommType, ConnectParam, &AppSSLParam, TRUE);
#endif
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_Send
 DESCRIPTION:   SSL发送数据
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_Send(int CommType, byte *SndData, int SndLen, int Fmt)
{
	byte sbuf[1024+5];
	int i , slen ;
	byte Lrc = 0;
	
	memset(sbuf, 0 , sizeof(sbuf));
	slen = 0 ;
	
	switch(Fmt)
	{
	case COMM_FMT_STX_HEXLEN_DATA_LRC_ETX:
		sbuf[slen++] = STX;
		sbuf[slen++] = SndLen / 256;
		sbuf[slen++] = SndLen % 256;
		memcpy(sbuf + slen, SndData, SndLen);
		slen += SndLen;
		
		for(i = 0, Lrc = 0; i < SndLen; i++)
			Lrc ^= *((byte *)SndData + i);
			
		sbuf[slen++] = Lrc;
		sbuf[slen++] = ETX;
		break ;
	case COMM_FMT_HEXLEN_DATA:
		sbuf[slen++] = SndLen / 256;
		sbuf[slen++] = SndLen % 256;
		memcpy(sbuf + slen , SndData , SndLen);
		slen += SndLen ;
		break ;
	default:
		memcpy(sbuf + slen , SndData , SndLen);
		slen += SndLen ;
		break ;
	}
	
#ifdef MAKE_LINUX
	return VComm_SSLSend(CommType, (char*)sbuf, slen);
#else
	return VCommLib_SSLSend(sbuf, slen);
#endif
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_Recv
 DESCRIPTION:   SSL接收数据
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_Recv(int CommType, byte *RecvBuf, int MaxRecvBufSize, int *RcvLen, int RecvDataFmt, int TimeOutSec)
{
	int iRet;
	int rlen;
	byte rbuf[2048];
	
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;
	
	PrnDisp_InitShowTimeCount(TimeOutSec);
#ifdef MAKE_LINUX
	iRet = VComm_SSLRecv_TCD(CommType, (char*)rbuf, sizeof(rbuf), &rlen, TimeOutSec, PrnDisp_CommShowTimeCount);
#else
	iRet = VCommLib_SSLRecv(rbuf, sizeof(rbuf), &rlen, TimeOutSec);
#endif
	
	if(iRet == COMM_OK)
	{
		switch(RecvDataFmt)
		{
		case COMM_FMT_STX_HEXLEN_DATA_LRC_ETX:
			*RcvLen = rlen - 5;
			memcpy(RecvBuf, rbuf + 3, *RcvLen);
			break ;
		case COMM_FMT_HEXLEN_DATA:
			*RcvLen = rlen - 2;
			memcpy(RecvBuf, rbuf + 2, *RcvLen);
			break ;
		default:
			*RcvLen = rlen ;
			memcpy(RecvBuf, rbuf, *RcvLen);
			break ;
		}
	}
	
	return iRet;
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_DisConnect
 DESCRIPTION:   SSL断开
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_DisConnect(int CommType, BOOL BackGroundFlag)
{
#ifdef MAKE_LINUX
	return VComm_SSLShutdown(CommType, FALSE);
#else
	return VCommLib_SSLShutdown();
#endif
}

#endif


/*--------------------------------------------------------------------------
 * FUNCTION NAME:   COMM_Get_ErrCode.
 * DESCRIPTION:     显示通讯库错误代码含义
 * RETURN:
 * NOTE:
 *
 *-------------------------------------------------------------------------- */
int Comm_GetErrCode(int ErrCode, int Comm_Mode)
{

	
	return BANK_OK;
}


/*--------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_IsTCPIPCommMode
 * DESCRIPTION:     是否TCPIP通讯方式(以支持SSL)
 *
 * RETURN:
 * NOTE:
 *
 *-------------------------------------------------------------------------- */
int Comm_IsTCPIPCommMode(int CommMode)
{
	int iRet = 0;
	
	switch(CommMode)
	{
	case COMM_TYPE_ETHERNET:
	case COMM_TYPE_GPRS:
#ifdef TRIDENT
	case COMM_TYPE_CDMA:
	case COMM_TYPE_WIFI:
#endif
		iRet = 1;
		break;
		
	case COMM_TYPE_SDLC:
	default:
		iRet = 0;
		break;
	}
	
	
	return(iRet);
}


/* -----------------------------------------------------------------------------
 * Private Function define
 ---------------------------------------------------------------------------- */

#if defined(TRIDENT) || defined(MAKE_LINUX)
/*--------------------------------------------------------------------------
 * FUNCTION NAME:   inComm_UpdateSSLServerIP
 * DESCRIPTION:     更新SSL使用域名方式下解析出的服务器IP地址
 *
 * RETURN:
 * NOTE:
 *
 *-------------------------------------------------------------------------- */
static int inComm_UpdateSSLServerIP(int CurrCommMode, CommCnt_Param *pCntPrm)
{
	if(!Comm_IsTCPIPCommMode(CurrCommMode))
		return COMM_RET_OK;
		
		
	//TCP方式连接成功，如果使用域名并且未解析IP地址，则保存解析出来的IP地址，下次直接使用IP地址连接
	if(gTermCfg.mTrSwitch.SSLCommFlag && gTermCfg.mTrSwitch.UseDNSFlag && (strlen(SSLServerIPAddress) < 7))
	{
		if(CurrCommMode == COMM_TYPE_ETHERNET)
		{
			memset(SSLServerIPAddress, 0, sizeof(SSLServerIPAddress));
			strncpy(SSLServerIPAddress, (char*)pCntPrm->CntParam.EtherParam.HostIP, 15);
		}
		else if(CurrCommMode == COMM_TYPE_GPRS)
		{
			memset(SSLServerIPAddress, 0, sizeof(SSLServerIPAddress));
			strncpy(SSLServerIPAddress, (char*)pCntPrm->CntParam.GprsParam.HostIP, 15);
		}
		
#ifdef TRIDENT
		else if(CurrCommMode == COMM_TYPE_CDMA)
		{
			memset(SSLServerIPAddress, 0, sizeof(SSLServerIPAddress));
			strncpy(SSLServerIPAddress, (char*)pCntPrm->CntParam.CdmaParam.HostIP, 15);
		}
		else if(CurrCommMode == COMM_TYPE_WIFI)
		{
			memset(SSLServerIPAddress, 0, sizeof(SSLServerIPAddress));
			strncpy(SSLServerIPAddress, (char*)pCntPrm->CntParam.WifiParam.HostIP, 15);
		}
		
#endif
		else
		{
			return(COMM_RET_TYPE_FAIL);
		}
	}
	
	return COMM_RET_OK;
}
#endif

/*--------------------------------------------------------------------------
 * FUNCTION NAME:   inComm_GetConnectParams
 * DESCRIPTION:     获取连接参数 CurrCommMode: 当前通讯类型
 *                  PriSecOffset:
 *                     TCPIP: 0-主通讯参数  1-备通讯参数
 *                     拨号: 0-电话号码1  1-电话号码2  1-电话号码3
 * RETURN:
 * NOTE:
 *
 *-------------------------------------------------------------------------- */

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SetDialAtCmd
 DESCRIPTION:   设置拨号AT指令
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SetDialAtCmd(void)
{
	char TmpBuf[128], ATString[32];
	
	memset(TmpBuf, 0, sizeof(TmpBuf));
	memset(ATString, 0, sizeof(ATString));
	
	if(gTermCfg.mTrSwitch.CheckDialTone)    //检测拨号音
	{
#ifdef MAKE_LINUX
		strcpy(ATString, "ATX4");    //将两条AT指令合并写在一条语句中执行，提高执行效率
#elif defined(TRIDENT)
		VComm_SetDialTone(TRUE);
#endif
	}
	else                                    //不检测拨号音(忽略摘机忙音)
	{
#ifdef MAKE_LINUX
		strcpy(ATString, "ATX3");    //将两条AT指令合并写在一条语句中执行，提高执行效率
#elif defined(TRIDENT)
		VComm_SetDialTone(FALSE);
#endif
	}
	
#ifdef MAKE_LINUX
	//将两条AT指令合并写在一条语句中执行，提高执行效率
	sprintf(TmpBuf, "ATS91=%u", gTermCfg.mCommPrm.ModemRegS91);
	strcat(ATString, TmpBuf);
	//通讯库在处理VComm_ATCommand时会自动调用VComm_Init，故应用程序无需执行VComm_Init
	memset(TmpBuf, 0, sizeof(TmpBuf));
	VComm_ATCommand(COMM_TYPE_SDLC, ATString, TmpBuf);
#endif
	
	return BANK_OK;
}
