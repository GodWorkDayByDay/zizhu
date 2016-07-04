/***************************************************************************
* FILE NAME:    Debug.c                                                    *
* MODULE NAME:  DEBUG                                                      *
* PROGRAMMER:   Laikey                                                     *
* DESCRIPTION:                                                             *
* REVISION:                                                                *
****************************************************************************/
#include "cupms.h"

/*-----------------------------------------------------------------------------
 *External variables / declarations
 *-----------------------------------------------------------------------------*/
extern Term_Config gTermCfg;


/* -----------------------------------------------------------------------------
* private variables / declarations
---------------------------------------------------------------------------- */
static char *DeviceComList[] =
{

};

static byte inAppDebug_bReturnStatus;

/*-----------------------------------------------------------------------------
 *Private Function declare
 *-----------------------------------------------------------------------------*/
static int inAppDebug_OutputDataViaCOM(char *pOutputData, int iDataLength);
static int inAppDebug_SaveFile(char *pOutputData, int iDataLength);


/*-----------------------------------------------------------------------------
 * Public Function define
 *-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * FUNCTION NAME: AppDebug_OutputLog
 * DESCRIPTION:   Display/Output/Print the data according environment variable define
 * RETURN: none
 *----------------------------------------------------------------------------- */
void AppDebug_OutputLog(char *pString, ...)
{
#ifdef MAKE_APPDEBUG

	char cMsgBuf[ 2048 ];
	char *pArg;
	va_list v_Arg;
	
	memset(cMsgBuf, 0, sizeof(cMsgBuf));
	
	if(pString != NULL)
	{
		pArg = strchr(pString, '%');
		
		if(pArg)
		{
			va_start(v_Arg, pString);
			vsprintf(cMsgBuf, pString, v_Arg);
			va_end(v_Arg);
		}
		else
		{
			strcpy(cMsgBuf, pString);
		}
	}
	
	// Display message
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_DISPLAY) == BANK_OK)
	{
		AppUtils_DisplayCenter(cMsgBuf, LINE1, FALSE);
		AppUtils_WaitKey(0);
	}
	
	// Output message by COM
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_COMOUTPUT) == BANK_OK)
		inAppDebug_OutputDataViaCOM(cMsgBuf, strlen(cMsgBuf));
		
	// Print message
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_PRINT) == BANK_OK)
		PrnDisp_PrintUserData(PRINTMODE_STRING, cMsgBuf, strlen(cMsgBuf));
		
	// save message to file
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_SAVEFILE) == BANK_OK)
		inAppDebug_SaveFile(cMsgBuf, strlen(cMsgBuf));
		
#endif
		
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppDebug_OutputDataByHex
 * DESCRIPTION:     Send out the data via COM, all data will convert to hex mode,
                    1 times is 32bytes data
 * RETURN: none
 ---------------------------------------------------------------------------- */
void AppDebug_OutputDataByHex(char *pOutputData, int iDataLength)
{
#if 0
	char DevName[ 16 ];
	char cTempBuf[ 56 ];
	char cSendData[ 126 ];
	int i, j, k, m = 32;
	
	if(iDataLength <= 0)
		return;
		
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_COMOUTPUT) != BANK_OK)
		return;
		
	memset(DevName, 0, sizeof(DevName));
	
#ifdef MAKE_APPDEBUG
	
	if(AppUtils_OpenRS232(AppDebug_GetCOMPortName(DevName), Rt_115200) != BANK_OK)
		return;
		
	AppUtils_WaitKeyMill(200);
#endif
	
	memset(cTempBuf, 0, sizeof(cTempBuf));
	
#ifdef MAKE_APPDEBUG
	
	while(AppUtils_RecvRS232Data(COMM_FMT_NOFMT, cTempBuf, 1, 1) > 0);
	
	AppUtils_WaitKeyMill(200);
#endif
	
	memset(cTempBuf, 0, sizeof(cTempBuf));
	AppUtils_GetPosDateTime(cTempBuf);
	strcat(cTempBuf, "(");
	AppUtils_GetTerminalTicks(cTempBuf + strlen(cTempBuf));
	
#ifdef MAKE_APPDEBUG
	strcat(cTempBuf, ")\n");
	AppUtils_SendRS232Data(COMM_FMT_NOFMT, cTempBuf, strlen(cTempBuf));
#elif MAKE_LINUX
	ApplLog_DebugHex(MultHead_GetAppName(), "AppDebug_OutputDataByHex", cTempBuf, strlen(cTempBuf));
#endif
	
	j = (iDataLength % m) ? (iDataLength / m + 1) : (iDataLength / m);
	
	for(i = 0; i < j; i++)
	{
		memset(cSendData, 0, sizeof(cSendData));
		
		for(k = 0; k < m; k++)
		{
			if(i *m + k >= iDataLength)
				break;
				
			sprintf(cTempBuf, "%02X", pOutputData[ i * m + k ]);
			strcat(cSendData, cTempBuf);
			
			if((k + 1) % 4 == 0)
				strcat(cSendData, " ");
		}
		
#ifdef MAKE_APPDEBUG
		strcat(cSendData, "\n");
		AppUtils_SendRS232Data(COMM_FMT_NOFMT, (char *) cSendData, strlen(cSendData));
#elif MAKE_LINUX
		ApplLog_DebugHex(MultHead_GetAppName(), "AppDebug_OutputDataByHex", cTempBuf, strlen(cTempBuf));
#endif
	}
	
#ifdef MAKE_APPDEBUG
	AppUtils_SendRS232Data(COMM_FMT_NOFMT, "\n", 1);
	AppUtils_WaitKeyMill(200);
#endif
	
#endif
	
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME: AppDebug_PrintISOPacketData
 * DESCRIPTION:   Print the data sent to bank host or received from bank host
 * RETURN: none
 ---------------------------------------------------------------------------- */
void AppDebug_OutputHexData(char *pMsgtitle, char *pPacketData, int iPacketDataLength)
{
	int iLen;
	char cBuffer[ 2048 ];
	
	memset(cBuffer, 0, sizeof(cBuffer));
	
	if(iPacketDataLength / 2 > sizeof(cBuffer))
		iLen = sizeof(cBuffer);
	else
		iLen = iPacketDataLength * 2;
		
	AppUtils_Bcd2Asc((byte *)pPacketData, (byte *)cBuffer, iLen);
	
	// Display message
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_DISPLAY) == BANK_OK)
	{
		AppUtils_DisplayCenter(cBuffer, LINE1, FALSE);
		AppUtils_WaitKey(0);
	}
	
	// Output message by COM
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_COMOUTPUT) == BANK_OK)
	{
		inAppDebug_OutputDataViaCOM(pMsgtitle, strlen(pMsgtitle));
		AppDebug_OutputDataByHex(pPacketData, iPacketDataLength);
	}
	
	// Print message
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_PRINT) == BANK_OK)
	{
		PrnDisp_PrintUserData(PRINTMODE_STRING, pMsgtitle, strlen(pMsgtitle));
		PrnDisp_PrintUserData(PRINTMODE_HEX, pPacketData, iPacketDataLength);
	}
	
	// save message to file
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_SAVEFILE) == BANK_OK)
		inAppDebug_SaveFile(cBuffer, iLen);
		
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppDebug_OutputFileLog
 * DESCRIPTION:     Send out the data saved by file mode
 * RETURN: none
 ---------------------------------------------------------------------------- */
void AppDebug_OutputFileLog(void)
{
#ifdef MAKE_APPDEBUG
	char DevName[ 16 ];
	char cTempBuf[ 56 ];
	char cSendData[ 1024 ];
	int iLen;
	int fd;
	
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_SAVEFILE) != BANK_OK)
		return;
		
	fd = File_Open(APPDEBUG_FILE, F_RDONLY);
	
	if(fd < 0)
		return;
		
	memset(DevName, 0, sizeof(DevName));
	
	if(AppUtils_OpenRS232(AppDebug_GetCOMPortName(DevName), Rt_115200) != BANK_OK)
	{
		File_Close(fd);
		return;
	}
	
	AppUtils_WaitKeyMill(200);
	
	memset(cTempBuf, 0, sizeof(cTempBuf));
	
	while(AppUtils_RecvRS232Data(COMM_FMT_NOFMT, cTempBuf, 1, 1) > 0);
	
	AppUtils_WaitKeyMill(200);
	
	memset(cTempBuf, 0, sizeof(cTempBuf));
	AppUtils_GetPosDateTime(cTempBuf);
	strcat(cTempBuf, "(");
	AppUtils_GetTerminalTicks(cTempBuf + strlen(cTempBuf));
	strcat(cTempBuf, ")\n");
	AppUtils_SendRS232Data(COMM_FMT_NOFMT, cTempBuf, strlen(cTempBuf));
	
	while(1)
	{
		memset(cSendData, 0, sizeof(cSendData));
		iLen = File_Read(fd, cSendData, sizeof(cSendData));
		
		if(iLen <= 0)
			break;
			
		AppUtils_SendRS232Data(COMM_FMT_NOFMT, cSendData, iLen);
	}
	
	AppUtils_SendRS232Data(COMM_FMT_NOFMT, "\n", 1);
	AppUtils_WaitKeyMill(200);
	AppUtils_CloseRS232();
	File_Close(fd);
	
	remove(APPDEBUG_FILE);
#endif
	
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME: AppDebug_CheckAppDebugStatus
 * DESCRIPTION:   Check which mode can be used for debug
 * RETURN:        BANK_OK: bit match, BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppDebug_CheckAppDebugStatus(byte bCheckMaskStatus)
{
	byte bDebugFlag;
	char cBuf[ 16 ];
	
	memset(cBuf, 0, sizeof(cBuf));
    //get_env("#ADFLAG", cBuf, 2);
	
	AppUtils_Asc2Bcd((byte *)cBuf, &bDebugFlag, 2);

    bDebugFlag = 0x00;//test

	if(bDebugFlag & bCheckMaskStatus)
		return BANK_OK;
	else
		return BANK_FAIL;
}


void AppDebug_SetReturnStatus(byte bStatus)
{
	inAppDebug_bReturnStatus = bStatus;
}

byte AppDebug_CheckReturnStatus(void)
{
	return inAppDebug_bReturnStatus;
}


char *AppDebug_GetCOMPortName(char *pRetComPortName)
{
	int i;
	char cTmpBuf[ 6 ];
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
    //get_env("#ADPORT", cTmpBuf, 1);
	i = atoi(cTmpBuf);
	
	if(i >= 0 && i <= 6)
		sprintf(pRetComPortName, "%s", DeviceComList[ i ]);
	else
		sprintf(pRetComPortName, "%s", DeviceComList[ 1 ]);
		
//  AppDebug_AppLog("com name=[%s]", pRetComPortName);

	return pRetComPortName;
}




/* -----------------------------------------------------------------------------
 * Private Function define
 ---------------------------------------------------------------------------- */






/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   inAppDebug_OutputDataByCOM
 * DESCRIPTION:     Send out the data via COM, 1 times is 32bytes data
 * RETURN:          BANK_OK: success, BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
static int inAppDebug_OutputDataViaCOM(char *pOutputData, int iDataLength)
{
#ifdef MAKE_APPDEBUG
	char DevName[ 16 ];
	char cTempBuf[ 56 ];
	
	if(iDataLength <= 0)
		return BANK_OK;
		
	memset(DevName, 0, sizeof(DevName));
	
	if(AppUtils_OpenRS232(AppDebug_GetCOMPortName(DevName), Rt_115200) != BANK_OK)
		return BANK_FAIL;
		
//  AppUtils_WaitKeyMill(50);

	memset(cTempBuf, 0, sizeof(cTempBuf));
	
	while(AppUtils_RecvRS232Data(COMM_FMT_NOFMT, cTempBuf, 1, 1) > 0);
	
//  AppUtils_WaitKeyMill(50);

#if 0
	memset(cTempBuf, 0, sizeof(cTempBuf));
	AppUtils_GetPosDateTime(cTempBuf);
	strcat(cTempBuf, "(");
	
	AppUtils_GetTerminalTicks(cTempBuf + strlen(cTempBuf));
	strcat(cTempBuf, ")\n");
	
	AppUtils_SendRS232Data(COMM_FMT_NOFMT, cTempBuf, strlen(cTempBuf));
	AppUtils_SendRS232Data(COMM_FMT_NOFMT, pOutputData, iDataLength);
	AppUtils_SendRS232Data(COMM_FMT_NOFMT, "\n\n", 2);
	
	AppUtils_WaitKeyMill(50);
#else
	AppUtils_SendRS232Data(COMM_FMT_NOFMT, pOutputData, iDataLength);
#endif
	
#endif
	
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   inAppDebug_SaveFile
 * DESCRIPTION:     Save the message to file
 * RETURN:          BANK_OK: success, BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
static int inAppDebug_SaveFile(char *pOutputData, int iDataLength)
{
	int fd;
	

	return BANK_OK;
}


