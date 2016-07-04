/***************************************************************************
* FILE NAME:    AppUtils.c                                                 *
* MODULE NAME:  APPTUTILS                                                  *
* PROGRAMMER:   Laikey                                                     *
* DESCRIPTION:                                                             *
* REVISION:                                                                *
****************************************************************************/
#include "cupms.h"
#include "iso8583.h"
#include "detaildialog.h"
#include <QTime>
#include <QApplication>
#include "prmmngr.h"
#include "zzkeypad.h"
#include "singleton.h"

/* -----------------------------------------------------------------------------
* External variables / declarations
---------------------------------------------------------------------------- */
extern Term_Config gTermCfg;

//虚电标志
unsigned char Batt_Flag;
extern int BankUtil_Bcd2Asc(byte *BcdBuf, byte *AscBuf, usint AscLen );
extern int BankUtil_Asc2Bcd(byte *AscBuf, byte *BcdBuf, usint AscLen);
/* -----------------------------------------------------------------------------
* Private variables / declarations
---------------------------------------------------------------------------- */
static int inAppUtils_ComHandle = 0;
static char inAppUtils_COMName[ 26 ] = {0};
static int inAppUtils_TimeOutSecs = 60; //界面超时时间(默认60s)

static int inAppUtils_PrinterHandle = 0;
unsigned int BankUtil_CStrlcpy(char *dst, const char *src, unsigned int siz)
{
    return 0;
}
/* -----------------------------------------------------------------------------
 * Private Function declare
 ---------------------------------------------------------------------------- */
static int inAppUtils_Scmp(byte *pSrc1, byte *pSrc2);

int MmiUtil_DisplayLine(char * mes, int num)
{

    DetailDialog::getInstance()->setText(num, QString(mes), 2);
    return 0;
}
/* -----------------------------------------------------------------------------
 * AppTool API Function define
 ---------------------------------------------------------------------------- */
/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DisplayLine
 * DESCRIPTION:     Display the message information automatically, if message length > screen width
 *                  it will display on new next line
 * RETURN:          return the next line no. haved been used
 ---------------------------------------------------------------------------- */
int AppUtils_DisplayLine(char *pDisplayBuf, int iStartLine)
{

    DetailDialog::getInstance()->setText(iStartLine, QString(pDisplayBuf), 0);
    return 0;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DisplayCenter
 * DESCRIPTION:     Display the message information automatically, if message length > screen width
 *                  it will display on new next line
 * RETURN:          return the next line no. haved been used
 ---------------------------------------------------------------------------- */
int AppUtils_DisplayCenter(char *pDisplayBuf, int iStartLine, unsigned int  bInvertFlag)
{

    DetailDialog::getInstance()->setText(iStartLine, QString(pDisplayBuf), 1);
    return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DisplayRight
 * DESCRIPTION:     Display the message information automatically, if message length > screen width
 *                  it will display on new next line
 * RETURN:          return the next line no. haved been used
 ---------------------------------------------------------------------------- */
int AppUtils_DisplayRight(char *pDisplayBuf, int iStartLine)
{

     DetailDialog::getInstance()->setText(iStartLine, QString(pDisplayBuf), 2);
     return 0;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_FlushKbd
 * DESCRIPTION:     clear keyboard buffer
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_FlushKbd(void)
{
    //MmiUtil_flushkbd();
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_FlushCard
 * DESCRIPTION:     clear card buffer
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_FlushCard(void)
{
    //MmiUtil_flushcard();
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_ResetScreen
 * DESCRIPTION:     init and clear screen, and set default font
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_ResetScreen(void)
{
    //MmiUtil_Reset();
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Warning
 * DESCRIPTION:     display warning message, wait button press
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
int AppUtils_Warning(char *pWarningString)
{
	AppUtils_UserClear(LINE2);
    AppUtils_DisplayCenter(pWarningString, LINE2, 1);

    return AppUtils_WaitKey(1000); //Edit by XC, Code review by Hongwei 2015/3/19
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_FormatWarning
 * DESCRIPTION:     display warning message with format string, wait button press
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
int AppUtils_FormatWarning(char *szFmt, ...)
{
	char szWarningBuf[256], *pArg;
	va_list v_arg;
	
	memset(szWarningBuf, 0, sizeof(szWarningBuf));
	
	if(szFmt != NULL)
	{
		pArg = strchr(szFmt, '%');
		
		if(pArg)
		{
            //////va_end(v_arg, szFmt);
			
			vsprintf(szWarningBuf, szFmt, v_arg);
			
            //va_end(v_arg);
		}
		else
		{
			strcpy(szWarningBuf, szFmt);
		}
	}
	
	return AppUtils_Warning(szWarningBuf);
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Cls
 * DESCRIPTION:     Clear the whole screen
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
void AppUtils_Cls(void)
{
    DetailDialog::getInstance()->clearScreen();
    DetailDialog::getInstance()->isShowInputText(TRUE, FALSE, FALSE);
    return ;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_UserClear
 * DESCRIPTION:     clear message has been displayed under iLineNo
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
void AppUtils_UserClear(int iLineNo)
{
    DetailDialog::getInstance()->clearLine(iLineNo);

    return ;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_ClearLine
 * DESCRIPTION:     clear one line message has been displayed
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
void AppUtils_ClearLine(int iLineNo)
{
     DetailDialog::getInstance()->clearLine(iLineNo);
     return ;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DisplayTitle
 * DESCRIPTION:     第一行居中反显
 * RETURN:
 *
 ---------------------------------------------------------------------------- */
void AppUtils_DisplayTitle(char *TitleMsg)
{
    DetailDialog::getInstance()->setText(1, QString(TitleMsg), 0);
    return ;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DispChinese
 * DESCRIPTION:     定位显示汉字
 * RETURN:
 *
 ---------------------------------------------------------------------------- */
void AppUtils_DispChinese(char *str, int x, int y)
{

    DetailDialog::getInstance()->setText(1, QString(str), 0);
     return ;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_DispEnglish
* DESCRIPTION:   在坐标起始位置开始显示一行英文.
* PARAMETERS:
*       str (in) -- 要显示的英文
*       x   (in) -- 列号, 从1开始
*       y   (in) -- 行号, 从1开始
* RETURN:
* NOTES:         128X64点阵的黑白屏上，x最大21  y最大8
* ------------------------------------------------------------------------ */
void AppUtils_DispEnglish(char *str, int x, int y)
{
    DetailDialog::getInstance()->setText(1, QString(str), 0);
     return ;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_WaitKey
 * DESCRIPTION:     wait for button pressed until timeout
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
byte AppUtils_WaitKey(int Timeout)
{
    unsigned char uckey;

    QTime dieTime = QTime::currentTime().addMSecs(Timeout*100);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, Timeout);

    //zzkeypad::getInstance()->getKey(uckey);

    return uckey;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_WaitKeyMill
 * DESCRIPTION:     wait for button pressed until timeout
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
byte AppUtils_WaitKeyMill(ulint Timeout)
{

    QTime dieTime = QTime::currentTime().addMSecs(Timeout);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, Timeout);

    return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_SVCWAIT
 * DESCRIPTION:     Block and wait for ulMSec mill seconds
 * RETURN:          0
 ---------------------------------------------------------------------------- */
int AppUtils_SVCWAIT(unsigned long ulMSec)
{
#ifdef MAKE_LINUX
	usleep(ulMSec * 1000);
#else
    //SVC_WAIT(ulMSec);
#endif
	return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_CheckKey
 * DESCRIPTION:     Check key in keyboard buffer
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
byte AppUtils_CheckKey(void)
{
    DetailDialog::getInstance()->getKeyConfirm();
    return 0;

}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetSerialNo
 * DESCRIPTION:     get terminal serial NO.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_GetSerialNo(char *pRetSerialNo)
{
	char cTmpBuf[ 26 ];
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
    //MmiUtil_GetSerialNO(cTmpBuf);
	
	if(strchr(cTmpBuf, '-'))    //有杠时直接拷贝
		sprintf(pRetSerialNo, "%s", cTmpBuf);
	else                        //无杠时自己加杠
		sprintf(pRetSerialNo, "%3.3s-%3.3s-%3.3s", cTmpBuf, cTmpBuf + 3, cTmpBuf + 6);
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_FillBufWithChar
 * DESCRIPTION:     用特定字符填充缓冲区(原始字符串如果>=总长度则不做处理)
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_FillBufWithChar(char *OrgStr, char FillChar, int Length)
{
	int iLen;
	
	iLen = strlen(OrgStr);
	
	if(iLen >= Length)
		return iLen;
		
	memset(OrgStr + iLen, FillChar, Length - iLen);
	
	return Length;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_SetYear
 * DESCRIPTION:     set year of date
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_SetYear(char *pNewYear)
{
    //TimeUtil_SetYear((unsigned char *)pNewYear);
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetClock
 * DESCRIPTION:     Read Clock Data into pClockData
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_GetClock(char *pClockData)
{
    //if(SVC_CLOCK(0, pClockData, 15) > 0)
    //	return BANK_OK;
    //else
    //	return BANK_FAIL;

    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间

    QString str = time.toString("yyyyMMddhhmmssddd"); //设置显示格式

    std::string buffstr = str.toStdString();
    const char* ch = buffstr.c_str();
    memcpy(pClockData, ch,15);
    qDebug("DateTime:%s", pClockData);
    return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_UpdateDateTime
 * DESCRIPTION:     Update Date: "MMDD" and Time: "HHMMSS" (Both ASCII String)
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_UpdateDateTime(char *pNewDate, char *pNewTime)
{
	char cClockData[15+1];
	
	memset(cClockData, 0, sizeof(cClockData));
	
	if(AppUtils_GetClock(cClockData) == BANK_OK)
	{
		memcpy(cClockData + 4, pNewDate, 4);
		memcpy(cClockData + 8, pNewTime, 6);
		cClockData[14] = 0;
    //	SVC_CLOCK(1, cClockData, 14);
	}
	
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetPosDateTime
 * DESCRIPTION:     Get terminal date time, format: yyyy-mm-dd hh:mm:ss
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_GetPosDateTime(char *pRetTerminalDateTime)
{
	char cTempBuf[ 26 ];
	char *p = pRetTerminalDateTime;
	
	// Date
    //sprintf(cTempBuf, "%08lu", TimeUtil_GetDate());
	memcpy(p, cTempBuf, 4);
	p += 4;
	*p ++ = '-';
	memcpy(p, cTempBuf + 4, 2);
	p += 2;
	*p ++ = '-';
	memcpy(p, cTempBuf + 6, 2);
	p += 2;
	*p ++ = ' ';
	
	// Time
    //sprintf(cTempBuf, "%06lu", TimeUtil_GetTime());
	memcpy(p, cTempBuf, 2);
	p += 2;
	*p ++ = ':';
	memcpy(p, cTempBuf + 2, 2);
	p += 2;
	*p ++ = ':';
	memcpy(p, cTempBuf + 4, 2);
	p += 2;
	
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetTerminalTicks
 * DESCRIPTION:     Get terminal Ticks, short second digit from terminal start to now
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_GetTerminalTicks(char *pRetTerminalTicks)
{
    //sprintf(pRetTerminalTicks, "%lu", read_ticks());
	return;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_ReadTicks
 * DESCRIPTION:     Get terminal Ticks, short second digit from terminal start to now
 * RETURN:          None
 ---------------------------------------------------------------------------- */
unsigned long AppUtils_ReadTicks(void)
{
    //return read_ticks();
    return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetTrack2
 * DESCRIPTION:     Read track info, immediately return(None block)
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_GetTrack2(byte *CardBuf1, byte *CardBuf2, byte *CardBuf3, byte *PANnum, int manflag)
{
    //return MmiUtil_GetTrack2(CardBuf1, CardBuf2, CardBuf3, PANnum, manflag);
    return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetColumns
 * DESCRIPTION:     Get max colums.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_GetColumns(void)
{
    //return MmiUtil_GetColumms();
    return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetRows
 * DESCRIPTION:     Get max rows
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_GetRows(void)
{
#ifdef MAKE_LINUX
	return (MmiUtil_GetRows() / 2);
#else
    //return MmiUtil_GetRows();
#endif
}

/*
byte a_to_b(byte bchar)
{
    bchar &= 0xFF;
    if ((bchar >='0')&&(bchar <='9'))
        return(bchar-'0');
    if ((bchar >='A')&&(bchar <='F'))
        return(bchar-'A'+10);
    if ((bchar >='a')&&(bchar <='f'))
        return(bchar-'a'+10);
    else
        return(0xf);
}
int BankUtil_Bcd2Asc(byte *BcdBuf, byte *AscBuf, usint AscLen )
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

int BankUtil_Asc2Bcd(byte *AscBuf, byte *BcdBuf, usint AscLen)
{
    int i,j;
    byte bchar;

    for (i=0,j=0; j<AscLen; i++)
    {
        bchar = a_to_b(AscBuf[j++]);
        if(j == AscLen)
            BcdBuf[i] = ((bchar<<4)&0xf0) + 0x0F;
        else
            BcdBuf[i] = ((bchar<<4)&0xf0) + a_to_b(AscBuf[j++]);

    }
    return 0;
}
*/

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Asc2Bcd
 * DESCRIPTION:     AscBuf - Ascii input buffer, must ended by '\0'
 *                  BcdBuf - converted result
 *                  Len - double length of BCD code, should be even.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Asc2Bcd(byte *pAscBuf, byte *pBcdBuf, int iAscLength)
{
    BankUtil_Asc2Bcd(pAscBuf, pBcdBuf, iAscLength);
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Bcd2Asc
 * DESCRIPTION:     BcdBuf - BCD input buffer,
 *                  AscBuf - converted result, terminated by '\0',
 *                  Len - double length of BCD code, must be even.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Bcd2Asc(byte *pBcdBuf, byte *pAscBuf, int iAscLength)
{
    BankUtil_Bcd2Asc(pBcdBuf, pAscBuf, (usint) iAscLength);
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Bcd2Len
 * DESCRIPTION:     Convert BcdLen bytes BCD length to usint.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Bcd2Len(byte *pBcdBuf, int *pRetLength, int iBcdLength)
{
	usint usRetLen;
	
	usRetLen = 0;
//	BankUtil_Bcd2Len(pBcdBuf, (usint *) &usRetLen, (usint) iBcdLength);
	
	*pRetLength = (int) usRetLen;
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Len2Bcd
 * DESCRIPTION:     Convert usint to n bytes BCD
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Len2Bcd(int iLength, byte *pRetBcdBuf, int iBcdLength)
{
    //BankUtil_Len2Bcd((usint) iLength, pRetBcdBuf, (usint) iBcdLength);
	return;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Hex2Long
 * DESCRIPTION:     Convert n bytes HEX data to long data
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Hex2Long(unsigned long *lRetLong, byte *pHexBuf, int iHexLength)
{
	int i;
	int iLength;
	union _h2l
	{
		unsigned long l;
		unsigned char h[ 4 ];
	} h2l;
	
	iLength = iHexLength;
	
	if(iLength > 4)
		iLength = 4;
		
#ifdef MAKE_LINUX
	pHexBuf += iLength - 1;
	memset(h2l.h, 0, sizeof(h2l.h));
	
	for(i = 0; i < iLength; i ++)
	{
		h2l.h[ i ] = *pHexBuf;
		pHexBuf --;
	}
	
	*lRetLong = h2l.l;
#else
	memset(h2l.h, 0, sizeof(h2l.h));
	
	for(i = 0; i < iLength; i ++)
	{
		h2l.h[ i ] = *pHexBuf;
		pHexBuf ++;
	}
	
	*lRetLong = h2l.l;
#endif
	return;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Long2Hex
 * DESCRIPTION:     Convert long data to n bytes HEX data
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Long2Hex(unsigned long lLongth, byte *pHexBuf, int iHexLength)
{
	int i;
	int iLength;
	union _h2l
	{
		unsigned long l;
		unsigned char h[ 4 ];
	} h2l;
	
	iLength = iHexLength;
	
	if(iLength > 4)
		iLength = 4;
		
	h2l.l = lLongth;
#ifdef MAKE_LINUX
	
	for(i = iLength - 1; i >= 0; i --)
	{
		*pHexBuf = h2l.h[ i ];
		pHexBuf ++;
	}
	
#else
	
	for(i = 0; i < iLength; i ++)
	{
		*pHexBuf = h2l.h[ i ];
		pHexBuf ++;
	}
	
#endif
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtil_IsBigScreen
 * DESCRIPTION:     Check if terminal is the big screen type
 * RETURN:          BANK_OK: big screen terminal has more than 4 lines display
                    BANK_FAIL: normal screen terminal has 4 lines display
 ---------------------------------------------------------------------------- */
int AppUtils_IsBigScreen(void)
{
    int type;
    type = _VX680;
    switch(type)
	{
	case _VX810:
	case _VX670:
	case _VX680:
	case _VX680C:
	case _VX820:
		return BANK_OK;
		
	default:
		return BANK_FAIL;
	}
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_IsTouchScreen
 * DESCRIPTION:     Check if terminal has the touch screen
 * RETURN:          BANK_OK: Yes
                    BANK_FAIL: No
 ---------------------------------------------------------------------------- */
int AppUtils_IsTouchScreen(void)
{
    switch(1)
	{
	case _VX680:
	case _VX680C:
	case _VX820:
		return BANK_OK;
		
	default:
		return BANK_FAIL;
	}
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_IsBatteryTerminal
 * DESCRIPTION:     Check if terminal has battery
 * RETURN:          BANK_OK: has battery
                    BANK_FAIL: has no battery
 ---------------------------------------------------------------------------- */
int AppUtils_IsBatteryTerminal(void)
{
    switch(1)
	{
	case _VX610:
	case _VX670:
	case _VX680:
	case _VX680C:
	case _VX675:
		return BANK_OK;
		
	default:
		return BANK_FAIL;
	}
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_IsGprsTerminal
 * DESCRIPTION:     Check if terminal has gprs/cdma modular
 * RETURN:          BANK_OK: has gprs modular
                    BANK_FAIL: has no gprs modular
 ---------------------------------------------------------------------------- */
int AppUtils_IsGprsTerminal(void)
{
    switch(1)
	{
	case _VX510G:
	case _VX610:
	case _VX670:
	case _VX675:
	case _VX680:
	case _VX680C:
		return BANK_OK;
		
	default:
		return BANK_FAIL;
	}
}

//返回是否675屏幕(屏幕行宽和列宽正好与680/820屏幕相反)
int AppUtils_IsRotatedScreen(void)
{
    switch(1)
	{
	case _VX675:
		return BANK_OK;
		
	default:
		return BANK_FAIL;
	}
}

//返回是否只支持内置密码键盘
int AppUtils_IsOnlyBuiltInPinpad(void)
{
    switch(1)
	{
	case _VX610:
	case _VX670:
	case _VX680:
	case _VX680C:
	case _VX810:
	case _VX820:
	case _VX675:
		return BANK_OK;
		
	default:
		return BANK_FAIL;
	}
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_SetTransStatus
 * DESCRIPTION:     Check Transaction record status
 * RETURN:          BANK_OK: status bit exists
 *                  BANK_FAIL: status bit not exists
 ---------------------------------------------------------------------------- */
int AppUtils_SetTransStatus(byte *pTxnRecStatus, byte cTransRecStatus)
{
	*pTxnRecStatus |= cTransRecStatus;
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_CheckTransStatus
 * DESCRIPTION:     Check Transaction record status
 * RETURN:          BANK_OK: status bit exists
 *                  BANK_FAIL: status bit not exists
 ---------------------------------------------------------------------------- */
int AppUtils_CheckTransStatus(byte cTxnRecStatus, byte cTransRecStatus)
{
	if(cTxnRecStatus & cTransRecStatus)
		return BANK_OK;
	else
		return BANK_FAIL;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_OpenPrinter
* DESCRIPTION:   打开打印机
* PARAMETERS:    None
* RETURN:        BANK_OK / BANK_FAIL
* NOTES:
* ------------------------------------------------------------------------ */
int AppUtils_OpenPrinter(void)
{
    return BANK_OK;

#ifdef MAKE_LINUX
	return BANK_OK;
#else
	int owner;
	byte bt;
    int iTermType = 1;
	
    if(1)
	{
        //while((iTermType != _VX675 && get_battery_charge() < 20)
        //      || (iTermType == _VX675 && get_battery_value(BATTERYVOLTAGE) < 3600))
		{
			AppUtils_UserClear(LINE2);
			AppUtils_DisplayLine("电量不足  请充电", LINE2);
			AppUtils_DisplayLine("如须此时打印", LINE3);
			AppUtils_DisplayLine("请按[O]键确认", LINE4);
			AppUtils_ErrorBeep();
	
			bt = AppUtils_WaitKey(1);
	
			if(bt == bENTER || bt == bKEY0)
			{
				AppUtils_UserClear(LINE2);
				AppUtils_DisplayCenter("正在打印", LINE3, FALSE);
//				break;
//        AppUtils_UserClear(LINE2);
//        AppUtils_DisplayCenter("电量不足可能导致", LINE2, FALSE);
//        AppUtils_DisplayCenter("打印信息不全", LINE3, FALSE);
//        AppUtils_DisplayCenter("确认继续打印?", LINE4, FALSE);
//
//        if(AppUtils_WaitKey(30) == KEY_CR)
//        {
//          AppUtils_UserClear(LINE2);
//          AppUtils_DisplayCenter("正在打印", LINE3, FALSE);
//          break;
//        }
			}
		}
	}
	
	//VX520SP每次打印下载汉字库，避免汉字无法打印情况
//	if(MmiUtil_GetPrinterType() == SPROCKET_PRINTER)
	{
    ///////	Print_DelCache();
	}
	
	while(1)
	{
    //	inAppUtils_PrinterHandle = get_owner("/dev/com4", &owner);
		AppDebug_AppLog("handle=%d,owner=%d", inAppUtils_PrinterHandle, owner);
	
		if(!owner)
		{
			inAppUtils_PrinterHandle = open("/dev/com4", 0);
			AppDebug_AppLog("open printer,handle=%d", inAppUtils_PrinterHandle);
			return inAppUtils_PrinterHandle;
		}
        //else if(owner == get_task_id())
		{
			return inAppUtils_PrinterHandle;
		}
//		else
		{
			AppDebug_AppLog("printer is in using by %d", owner);
			inAppUtils_PrinterHandle = -1;
			return inAppUtils_PrinterHandle;
		}
	}
	
#endif
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_OpenRS232
 * DESCRIPTION:     Open RS232 Port
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_OpenRS232(char *pCOMName, int iBoudX)
{
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_RecvRS232Data
 * DESCRIPTION:     Receive RS232 Data
 * RETURN:          length of received data
 ---------------------------------------------------------------------------- */
int AppUtils_RecvRS232Data(char bDataMode, char *pRetData, int iNeedRecvDataLength, int iTimeout)
{
	int iRecvLength;
	int iOffset, iReadLen, iRet = 0;
	long tstart, tnow;
	byte cBcdLen[ 12 ], Lrc;
	char cBuffer[ 2048 ];
	
    //if(bDataMode == COMM_FMT_NOFMT)
	{
		iRecvLength = read(inAppUtils_ComHandle, pRetData, iNeedRecvDataLength);
	}
//	else
	{
		iRecvLength = 0;
		memset(cBuffer, 0, sizeof(cBuffer));
		
    //	tstart = TimeUtil_MillSec();
		AppUtils_FlushKbd();
		
		while(1)
		{
//			tnow = TimeUtil_MillSec();
			
			if((tnow - tstart) / 1000 > iTimeout)
			{
				AppUtils_Warning("Time Out!");
				return -1;
			}
			
			memset(cBuffer, 0, sizeof(cBuffer));
			
			if(read(inAppUtils_ComHandle, cBuffer, 1) > 0)
			{
				if(cBuffer[ 0 ] == 0x02)
					break;
			}
        //	else if(MmiUtil_CheckKey() == bESC)
			{
				return -2;
			}
			
			AppUtils_SVCWAIT(10);
		}
		
		memset(cBcdLen, 0, sizeof(cBcdLen));
		
		if(read(inAppUtils_ComHandle, (char *)cBcdLen, 2) != 2)
			return -3;
			
		AppUtils_SVCWAIT(10);
		
		iRecvLength = (cBcdLen[ 0 ] >> 4) * 1000 + (cBcdLen[ 0 ] & 0x0F) * 100 + (cBcdLen[ 1 ] >> 4) * 10 + (cBcdLen[ 1 ] & 0x0F);
		
		if(iRecvLength <= 0)
			return -4;
			
		iOffset = 0;
		iReadLen = iRecvLength + 2;
		memset(cBuffer, 0, sizeof(cBuffer));
		
//		tstart = TimeUtil_MillSec();
		
		while(1)
		{
			tnow = TimeUtil_MillSec();
			
			if((tnow - tstart) / 1000 > 5)
			{
				if((iOffset >= iRecvLength + 2) || (iReadLen <= 0))
					break;
					
				AppUtils_Warning("通讯超时");
				return -1;
			}
			
			iRet = read(inAppUtils_ComHandle, cBuffer + iOffset, iReadLen);
			
			if(iRet > 0)
			{
				iOffset += iRet;
				iReadLen -= iRet;
				
				if((iOffset >= iRecvLength + 2) || (iReadLen <= 0))
					break;
			}
			
			AppUtils_SVCWAIT(10);
		}
		
		if(cBuffer[ iRecvLength ] != 0x03)
		{
			AppUtils_Warning("ETX ERROR!");
			return -5;
		}
		
    //	Lrc = BankUtil_GenLrc((unsigned char *)cBuffer, iRecvLength + 1);
		Lrc ^= cBcdLen[ 0 ];
		Lrc ^= cBcdLen[ 1 ];
		
		if(cBuffer[ iRecvLength + 1 ] != Lrc)
		{
			AppUtils_Warning("LRC ERROR!");
			return -6;
		}
		
		if(iRecvLength > iNeedRecvDataLength)
			iRecvLength = iNeedRecvDataLength;
			
		memcpy(pRetData, cBuffer, iRecvLength);
	}
	
	return iRecvLength;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_CloseRS232
 * DESCRIPTION:     Close the COM handle
 * RETURN:          BANK_OK
 ---------------------------------------------------------------------------- */
int AppUtils_CloseRS232(void)
{
	if(inAppUtils_ComHandle)
		close(inAppUtils_ComHandle);
		
	inAppUtils_ComHandle = 0;
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * Public Function define
 ---------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_NullComp
 * DESCRIPTION:     Check if the data is NULL or not
 * RETURN:          BANK_OK: data is NULL
                    BANK_FAIL: data is not NULL
 ---------------------------------------------------------------------------- */
int AppUtils_IsNullData(char *pCheckData, int iDataLength)
{
	int Ret = BANK_OK;
	int i;
	
	for(i = 0; i < iDataLength; i ++)
	{
		if(pCheckData[ i ] != 0x00)
		{
			Ret = BANK_FAIL;
			break;
		}
	}
	
	return Ret;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DelTrail
 * DESCRIPTION:     deletes the trailing ( given ) chars from a string
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_DeleteTrail(char cDeleteLetter, char *pString)
{
	int i;
	
	i = strlen(pString);
	
	if(i)
		pString += i - 1;
		
	for(; i && (*pString == cDeleteLetter); *pString-- = 0, i--);
	
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AmountSub
 * DESCRIPTION:     pResult = pSub1 - pSub2
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_AmountSub(byte *pResult, byte *pSub1, byte *pSub2)
{
	int i, c = 0;
	int h;
	int len;
	int l1, l2;
	byte *p;
	byte aTmp[ 126 ];
	
	if(!pResult || !pSub1 || !pSub2)
		return BANK_FAIL;
		
	len = strlen((char*)pSub1);
	
	if(len < strlen((char*)pSub2))
		len = strlen((char*)pSub2);
		
	if(inAppUtils_Scmp(pSub1, pSub2) < 0)
		return BANK_FAIL;
		
	l1 = strlen((char *) pSub1);
	l2 = strlen((char *) pSub2);
	
	memset(aTmp, 0, sizeof(aTmp));
	p = aTmp;
	
	for(i = 0; i < l2; ++i)
	{
		h = (pSub1[ l1 - i - 1 ] - '0') - (pSub2[ l2 - i - 1 ] - '0') - c;
		
		if(h < 0)
		{
			c = 1;
			h = h + 10;
		}
		else
			c = 0;
			
		*p = h + '0';
		p ++;
	}
	
	for(i = l2; i < l1; ++i)
	{
		h = (pSub1[ l1 - i - 1 ] - '0') - c;
		
		if(h < 0)
		{
			c = 1;
			h = h + 10;
		}
		else
			c = 0;
			
		*p = h + '0';
		p ++;
	}
	
	h = p - aTmp;
	
	if(h < 1)
		return BANK_FAIL;
		
	for(i = 0; i < len - h; i ++)
		*pResult ++  = '0';
		
	for(i = h - 1; i >= 0; i--)
		*pResult ++ = aTmp[ i ];
		
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AmountAdd
 * DESCRIPTION:     pResult = pAdd1 + pAdd2
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_AmountAdd(byte *pResult, byte *pAdd1, byte *pAdd2)
{
	int i, h, c = 0;
	int l, l1, l2;
	byte *p;
	byte bTmpBuf[ 126 ];
	byte bRetBuf[ 126 ];
	
	//AppDebug_AppLog( "pAdd1=[%s], pAdd2=[%s], pResult=[%s]", pAdd1, pAdd2, pResult );
	
	memset(bTmpBuf, 0, sizeof(bTmpBuf));
	p = bTmpBuf;
	
	l1 = strlen((char *) pAdd1);
	l2 = strlen((char *) pAdd2);
	l = sizeof(bTmpBuf);
	
	if(l1 > l || l2 > l)
		return BANK_FAIL;
		
	if(l1 > l2)
		l = l2;
	else
		l = l1;
		
	for(i = 0; i < l; ++i)
	{
		h = (pAdd1[ l1 - i - 1 ] - '0') + (pAdd2[ l2 - i - 1 ] - '0') + c;
		
		if(h > 9)
		{
			c = 1;
			h = h - 10;
		}
		else
			c = 0;
			
		*p = h + '0';
		p ++;
	}
	
	if(l1 > l2)
	{
		for(i = l; i < l1; ++i)
		{
			h = (pAdd1[ l1 - i - 1 ] - '0') + c;
			
			if(h > 9)
			{
				c = 1;
				h = h - 10;
			}
			else
				c = 0;
				
			*p = h + '0';
			p ++;
		}
	}
	else if(l1 < l2)
	{
		for(i = l; i < l2; ++i)
		{
			h = (pAdd2[ l2 - i - 1 ] - '0') + c;
			
			if(h > 9)
			{
				c = 1;
				h = h - 10;
			}
			else
				c = 0;
				
			*p = h + '0';
			p ++;
		}
	}
	
	if(c == 1)
		*p ++ = '1';
		
	h = p - bTmpBuf;
	
	if(h > 12)
		return BANK_INVALIDE;
	else if(h < 1)
		return BANK_FAIL;
		
	memset(bRetBuf, 0, sizeof(bRetBuf));
	p = bRetBuf;
	
	for(i = h - 1; i >= 0; i--)
		*p ++ = bTmpBuf[ i ];
		
	memcpy(pResult, bRetBuf, h);
	
	//AppDebug_AppLog( "h=[%d],pAdd1=[%s], pAdd2=[%s], pResult=[%s]", h, pAdd1, pAdd2, pResult );
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AmountAsc2String
 * DESCRIPTION:     convert amount asc 12byte to string, 000000000001->0.01
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_AmountAsc2String(byte *pAmountAsc, char *pCurrencyName, char bNegativeFlag, char *pRetAmountString)
{
	char cBuffer[ 16 ];
	char *p;
	int i, iLen;
	
	iLen = strlen((char *) pAmountAsc);
	
	if(iLen > 12)
		iLen = 12;
		
	memset(cBuffer, '0', sizeof(cBuffer));
	memcpy(cBuffer + 12 - iLen, pAmountAsc, iLen);
	
	for(i = 0; i < 12; i ++)
	{
		if(cBuffer[ i ] != '0')
			break;
	}
	
	p = pRetAmountString;
	memcpy(p, pCurrencyName, strlen(pCurrencyName));
	p += strlen(pCurrencyName);
	
	if(bNegativeFlag == '-' || bNegativeFlag == 'D')
	{
		memcpy(p, "-", 1);
		p ++;
	}
	
	if(i == 12)
	{
		memcpy(p, "0.00", 4);
		p += 4;
	}
	else
	{
		if(i >= 10)
			*p ++ = '0';
		else
		{
			memcpy(p , cBuffer + i, 10 - i);
			p += (10 - i);
		}
		
		*p ++ = '.';
		memcpy(p, cBuffer + 10, 2);
		p += 2;
	}
	
	*p = 0;
	
	//AppDebug_AppLog( "pAmountAsc=[%s], pRetAmountString=[%s]", pAmountAsc, pRetAmountString );
	return;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AmountAsc2Long
 * DESCRIPTION:     convert amount asc 12byte to long data, 000000000001->1
 * RETURN:          None
 ---------------------------------------------------------------------------- */
ulint AppUtils_AmountAsc2Long(byte *pAmountAsc)
{
	int i, x = 1;
	int len;
	ulint ulRetAmt = 0;
	
	len = strlen((char *) pAmountAsc);
	
	for(i = len - 1; i >= 0; i --)
	{
		ulRetAmt += (pAmountAsc[ i ] - 0x30) * x;
		x *= 10;
	}
	
	return ulRetAmt;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtills_JournalCheckFreeSpace
 * DESCRIPTION:     check ram space is enough
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalCheckFreeSpace(void)
{
    //if((File_GetFreeSpace(RAM_PREFIX) / sizeof(TransRecord)) < 50)
    //	return BANK_FAIL;
    //else
        return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalWriteRecord
 * DESCRIPTION:     append the journal using pTxnRecord
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalWriteRecord(int iForeSaveFlag, DynData *pTxnRecord)
{
	if(memcmp(pTxnRecord->TransDB.RspCode, "00", 2) != 0)
	{
		if(iForeSaveFlag != 1)
			return BANK_OK;
	}
	
	if(AppUtils_JournalCheckFreeSpace() != BANK_OK)
	{
		memcpy(pTxnRecord->TransDB.RspCode, "30", 2);
		
		AppUtils_Warning("内存空间不足!");
		return BANK_FAIL;
	}
	
	// 注意:这里头写的交易记录流水只保存结构体 TransRecord的大小
	//      不保存全部的DynData结构体
    if(File_AppendRecord((char *) TRAN_DB, (char *) pTxnRecord, sizeof(TransRecord)) != FILE_SUCCESS)
	{
		memcpy(pTxnRecord->TransDB.RspCode, "30", 2);
		AppUtils_Warning("写交易流水失败!");
		return BANK_FAIL;
	}
	
//	TmsMngr_CheckAndUpdateSettleFlag();
	
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalUpdateRecord
 * DESCRIPTION:     Update the match journal record using pTxnRecord
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalUpdateRecord(ulint iSearchedTrace, DynData *pTxnRecord)
{
	int i, iRecNum;
	int iRet = BANK_FAIL;
	DynData tmpTxnRecord;
	
    //iRecNum = File_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));
	
	if(iRecNum <= 0)
	{
		return iRet;
	}
	
	for(i = 0; i < iRecNum; i ++)
	{
		memset(&tmpTxnRecord, 0, sizeof(tmpTxnRecord));
		
    //	if(File_GetRecordByIndex((char *) TRAN_DB, &tmpTxnRecord, sizeof(TransRecord), i) != FILE_SUCCESS)
			break;
			
		if(tmpTxnRecord.TransDB.SysTrace == iSearchedTrace)
		{
    //		if(File_UpdateRecordByIndex((char *) TRAN_DB, (char *) pTxnRecord, sizeof(TransRecord), i) == FILE_SUCCESS)
			{
				iRet = BANK_OK;
				break;
			}
		}
	}
	
	return iRet;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalUpdateRecordByIndex
 * DESCRIPTION:     Update the match journal record using pTxnRecord
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalUpdateRecordByIndex(int iIndex, DynData *pTxnRecord)
{
	int iRet = BANK_FAIL;
	
    //if(File_UpdateRecordByIndex((char *)TRAN_DB, (char *) pTxnRecord, sizeof(TransRecord), iIndex) == FILE_SUCCESS)
		iRet = BANK_OK;
		
	if(iRet == BANK_FAIL)
		AppUtils_Warning("更新交易记录失败!");
		
	return iRet;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalUpdateRecordByStatus
 * DESCRIPTION:     Update the match journal record using pTxnRecord
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalUpdateRecordByStatus(ulint iSearchedTrace, int iStatusOffset, byte NewStatusFlag)
{
	int i, iRecNum;
	int iRet = BANK_FAIL;
	DynData tmpTxnRecord;
	
    iRecNum = File_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));
	
	if(iRecNum > 0)
	{
		for(i = 0; i < iRecNum; i ++)
		{
			memset(&tmpTxnRecord, 0, sizeof(tmpTxnRecord));
			
            if(File_GetRecordByIndex((char *) TRAN_DB, &tmpTxnRecord, sizeof(TransRecord), i) != FILE_SUCCESS)
				break;
				
			if(tmpTxnRecord.TransDB.SysTrace == iSearchedTrace)
			{
				if((iStatusOffset >= 0) && (iStatusOffset < 2))
					tmpTxnRecord.TransDB.Status[ iStatusOffset ] |= NewStatusFlag;
					
                if(File_UpdateRecordByIndex((char *) TRAN_DB, (char *)&tmpTxnRecord, sizeof(TransRecord), i) == FILE_SUCCESS)
				{
					iRet = BANK_OK;
					break;
				}
			}
		}
	}
	
	if(iRet == BANK_FAIL)
		AppUtils_Warning("更新交易记录失败!");
		
	return iRet;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalDeleteRecord
 * DESCRIPTION:     delete the journal matched
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalDeleteRecord(ulint iSearchedTrace, DynData *pTxnRecord)
{
	int i, iRecNum;
	int iRet = BANK_OK;
	DynData tmpTxnRecord;
	
    //iRecNum = File_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));
	
	if(iRecNum > 0)
	{
		for(i = 0; i < iRecNum; i ++)
		{
			memset(&tmpTxnRecord, 0, sizeof(tmpTxnRecord));
			
        //	if(File_GetRecordByIndex((char *) TRAN_DB, &tmpTxnRecord, sizeof(TransRecord), i) != FILE_SUCCESS)
				break;
				
			if(tmpTxnRecord.TransDB.SysTrace == iSearchedTrace)
			{
        //		if(File_DeleteRecordByIndex((char *) TRAN_DB, sizeof(TransRecord), i) == FILE_SUCCESS)
				{
					iRet = BANK_OK;
					break;
				}
//				else
				{
					iRet = BANK_FAIL;
					break;
				}
			}
		}
	}
	
    //if(iRet == BANK_FAIL)
		AppUtils_Warning("删除交易记录失败!");
		
	return iRet;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalClearAll
 * DESCRIPTION:     clear the all journal
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalClearAll(void)
{
	int iRecNum;
	int i;
	int iRet;
	
	// 成功的交易流水
    //iRecNum = File_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));
	
	if(iRecNum > 0)
	{
		iRet = BANK_FAIL;
		
		for(i = 0; i < 3; i ++)
		{
    //		if(File_Remove((char *) TRAN_DB) == BANK_OK)
			{
				iRet = BANK_OK;
				break;
			}
		}
		
		if(iRet != BANK_OK)
		{
			for(i = 0; i < iRecNum; i ++)
			{
				File_DeleteRecordByIndex((char *) TRAN_DB, sizeof(TransRecord), i);
			}
		}
	}
	
	// 脚本上送流水
	iRecNum = File_GetNumberOfRecords((char *) TRAN_DB_SCRIPT, sizeof(TransRecord));
	
	if(iRecNum > 0)
	{
		iRet = BANK_FAIL;
		
		for(i = 0; i < 3; i ++)
		{
			if(File_Remove((char *) TRAN_DB_SCRIPT) == BANK_OK)
			{
				iRet = BANK_OK;
				break;
			}
		}
		
		if(iRet != BANK_OK)
		{
			for(i = 0; i < iRecNum; i ++)
			{
				File_DeleteRecordByIndex((char *) TRAN_DB_SCRIPT, sizeof(TransRecord), i);
			}
		}
	}
	
	// 脱机失败流水
	iRecNum = File_GetNumberOfRecords((char *) TRAN_DB_FAIL, sizeof(TransRecord));
	
	if(iRecNum > 0)
	{
		iRet = BANK_FAIL;
		
		for(i = 0; i < 3; i ++)
		{
			if(File_Remove((char *) TRAN_DB_FAIL) == BANK_OK)
			{
				iRet = BANK_OK;
				break;
			}
		}
		
		if(iRet != BANK_OK)
		{
			for(i = 0; i < iRecNum; i ++)
			{
				File_DeleteRecordByIndex((char *) TRAN_DB_FAIL, sizeof(TransRecord), i);
			}
		}
	}
	
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalGetRecordLast
 * DESCRIPTION:     get the last journal record
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalGetRecordLast(DynData *pRetTxnRecord)
{
	int iRecNum;
	int iRet = BANK_FAIL;
	
	iRecNum = File_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));
	
	if(iRecNum > 0)
	{
		if(File_GetRecordByIndex((char *) TRAN_DB, pRetTxnRecord, sizeof(TransRecord), iRecNum - 1) == FILE_SUCCESS)
			iRet = BANK_OK;
	}
	
	if(iRet == BANK_FAIL)
		AppUtils_Warning("交易记录不存在");
		
	return iRet;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalGetRecord
 * DESCRIPTION:     get the journal record by SysTrace
 * RETURN:          >=0: sucess
 *                  < 0: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalGetRecord(ulint iSearchedTrace, DynData *pRetTxnRecord)
{
	int i, iRecNum;
	int iRet = BANK_FAIL;
	DynData tmpTxnRecord;
	
	iRecNum = File_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));
	if(iRecNum <= 0)
	{
		return iRet;
	}
    qDebug("num%d", iRecNum);
	for(i = 0; i < iRecNum; i ++)
	{
		memset(&tmpTxnRecord, 0, sizeof(tmpTxnRecord));
		
		if(File_GetRecordByIndex((char *) TRAN_DB, &tmpTxnRecord, sizeof(TransRecord), i) != FILE_SUCCESS)
        {
            qDebug("File_GetRecordByIndex error");
            break;
        }

        qDebug("SysTrace%d", tmpTxnRecord.TransDB.SysTrace);
        print_qdebug((unsigned  char *)"SysTrace:", (unsigned char *)&tmpTxnRecord.TransDB.SysTrace, 8);
		if(tmpTxnRecord.TransDB.SysTrace == iSearchedTrace)
		{
			memcpy(pRetTxnRecord, &tmpTxnRecord, sizeof(TransRecord));
			iRet = BANK_OK;
			break;
		}
	}
	
	//查到了才返回偏移量, 否则返回失败
	if(iRet == BANK_OK)
		return i;
	else
		return BANK_FAIL;
}


void AppUtils_SetStatus(byte *bStatusSetted, byte bStatusFlag)
{
	*bStatusSetted |= bStatusFlag;
	return;
}


void AppUtils_ResetStatus(byte *bStatusSetted, byte bStatusFlag)
{
	*bStatusSetted &= ~bStatusFlag;
	return;
}


void AppUtils_ResetUsStatus(unsigned short *usStatusSetted, unsigned short usStatusFlag)
{
	*usStatusSetted &= ~usStatusFlag;
	return;
}


int AppUtils_CheckStatus(byte bStatusChecked, byte bStatusFlag)
{
	if(bStatusChecked & bStatusFlag)
		return BANK_OK;
	else
		return BANK_FAIL;
}


int AppUtils_CheckUshortStatus(unsigned short usStatusChecked, unsigned short usStatusFlag)
{
	if(usStatusChecked & usStatusFlag)
		return BANK_OK;
	else
		return BANK_FAIL;
}

int AppUtils_OperAdd(Bank_Operator *NewOper)
{
	if(File_AppendRecord((char *)OPER_FILE, (char *)NewOper, sizeof(Bank_Operator)) != FILE_SUCCESS)
		return BANK_FAIL;
		
	return BANK_OK;
}

int AppUtils_OperFind(Bank_Operator *FindOper)
{
	Bank_Operator tmpOper;
	int RecNum;
	int i;
	
	RecNum = File_GetNumberOfRecords((char *)OPER_FILE, sizeof(Bank_Operator));
	
	if(RecNum > 0)
	{
		for(i = 0; i < RecNum; i ++)
		{
			memset(&tmpOper, 0, sizeof(Bank_Operator));
			
			if(File_GetRecordByIndex((char *)OPER_FILE, &tmpOper, sizeof(Bank_Operator), i) != FILE_SUCCESS)
				return BANK_FAIL;
				
			if(memcmp(FindOper->OperID, tmpOper.OperID, FindOper->IDLen) == 0)
			{
				memcpy(FindOper, &tmpOper, sizeof(Bank_Operator));
				return BANK_OK;
			}
		}
	}
	
	return BANK_FAIL;
}

int AppUtils_OperDelete(Bank_Operator *DeletedOper)
{
	Bank_Operator tmpOper;
	int RecNum;
	int i;
	
	RecNum = File_GetNumberOfRecords((char *)OPER_FILE, sizeof(Bank_Operator));
	
	if(RecNum > 0)
	{
		for(i = 0; i < RecNum; i ++)
		{
			memset(&tmpOper, 0, sizeof(Bank_Operator));
			
			if(File_GetRecordByIndex((char *)OPER_FILE, &tmpOper, sizeof(Bank_Operator), i) != FILE_SUCCESS)
				return BANK_FAIL;
				
			if(memcmp(DeletedOper->OperID, tmpOper.OperID, DeletedOper->IDLen) == 0)
			{
				if(File_DeleteRecordByIndex((char *)OPER_FILE, sizeof(Bank_Operator), i) != FILE_SUCCESS)
					return BANK_FAIL;
					
				return BANK_OK;
			}
		}
	}
	
	return BANK_FAIL;
}

int AppUtils_OperUpdate(Bank_Operator *UpdatedOper)
{
	Bank_Operator tmpOper;
	int RecNum;
	int i;
	
	RecNum = File_GetNumberOfRecords((char *)OPER_FILE, sizeof(Bank_Operator));
	
	if(RecNum > 0)
	{
		for(i = 0; i < RecNum; i++)
		{
			memset(&tmpOper, 0, sizeof(Bank_Operator));
			
			if(File_GetRecordByIndex((char *)OPER_FILE, &tmpOper, sizeof(Bank_Operator), i) != FILE_SUCCESS)
				return BANK_FAIL;
				
			if(memcmp(UpdatedOper->OperID, tmpOper.OperID, UpdatedOper->IDLen) == 0)
			{
				if(File_UpdateRecordByIndex((char *)OPER_FILE, UpdatedOper, sizeof(Bank_Operator), i) != FILE_SUCCESS)
					return BANK_FAIL;
					
				return BANK_OK;
			}
		}
	}
	
	return BANK_FAIL;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_WaitKey
 * DESCRIPTION:     wait for button pressed until timeout
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
byte AppUtils_get_WaitKey(int Timeout)
{
    unsigned char uckey = 100;
    int iRet;

    qDebug("input key");
    iRet = Singleton<zzkeypad>::getInstance()->Qt_getInputKey((char *)&uckey);
    if(iRet == -2)
        return bESC;

    return uckey;
}

int MmiUtil_DisplayRight(char *cDisplayBuffer, int iLine)
{
    DetailDialog::getInstance()->setText(iLine, QString(cDisplayBuffer), 2);
    return 0;
}
/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetAmout
 * DESCRIPTION:     Input amount on key broad
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_GetAmout(int iStartLine, char *pMsgPrompt, byte bNegativeFlag, char *pCurrencyName, byte bZeroAmtAllowFlag, byte *pRetAmount)
{
	byte bKey;
	int iLen;
	int iLine = iStartLine;
	char cDisplayBuffer[ 56 ];
	char cInputBuffer[ 56 ];
	char cInputAmount[ 13 ];
	
	AppUtils_FlushKbd();
	AppUtils_UserClear(iLine);
    AppUtils_DisplayLine(pMsgPrompt, iLine);
    iLine += 1;
	memset(cDisplayBuffer, 0, sizeof(cDisplayBuffer));
	AppUtils_AmountAsc2String(pRetAmount, pCurrencyName, bNegativeFlag, cDisplayBuffer);
    qDebug("Disp:%siLine[%d]",cDisplayBuffer, iLine);
    MmiUtil_DisplayRight(cDisplayBuffer, iLine);

	iLen = LINE4;
	
	if(AppUtils_IsBigScreen() == BANK_OK)
		iLen = LINE9;
		
	MmiUtil_DisplayLine("输错请按清除键", iLen);
	
	iLen = 0;
	memset(cInputBuffer, 0, sizeof(cInputBuffer));
	
	while(TRUE)
	{
        qDebug("wait key ");
        bKey = AppUtils_get_WaitKey(60);
        qDebug("bKey:%02x", bKey);
		switch(bKey)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		
			if((bKey == '0') && (iLen == 0))
			{
				MmiUtil_Beep();
				break;
			}
			else if(iLen >= 10)
			{
				MmiUtil_Beep();
				break;
			}
			
			cInputBuffer[ iLen ++ ] = bKey;
			break;
			
        case 0x1b://cancel
        //case 0:
			return BANK_FAIL;
			
        case 0x08://bBKSP
		
			if(iLen > 0)
				cInputBuffer[ -- iLen ] = 0;
				
			break;
			
        case 0x0d://bENTER
		
			if(iLen > 12)
				iLen = 12;
				
			memset(cInputAmount, '0', 12);
			memcpy(cInputAmount + 12 - iLen, cInputBuffer, iLen);
			
			if(memcmp(cInputAmount, "000000000000", 12) != 0)
			{
				memcpy(pRetAmount, cInputAmount, 12);
				return BANK_OK;
			}
			else if(bZeroAmtAllowFlag == TRUE)
				return BANK_OK;
		}
		
		memset(cDisplayBuffer, 0, sizeof(cDisplayBuffer));
		AppUtils_AmountAsc2String((byte *) cInputBuffer, pCurrencyName, bNegativeFlag, cDisplayBuffer);
		MmiUtil_DisplayRight(cDisplayBuffer, iLine);
	}
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetAllPWD
 * DESCRIPTION:     Input text on key broad
 * PARAMETERS:      iStartLine: 起始行号(也就是开始显示提示信息的行号)
 *                  pMsgPrompt: 提示信息
 *                  pRetInputData: 输入数据指针
 *                  Min_Len: 最少输入长度  Max_Len: 最大输入长度
 *                  iTimeOut: 超时时间
 * RETURN:          -1: timeout
 *                  >=0: input data length
 * NOTES:           本函数基于AppUtils_GetText传入掩码'*'重新封装，可以支持字母输入
 ---------------------------------------------------------------------------- */
int AppUtils_GetAllPWD(int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, int iTimeOut)
{
	return AppUtils_GetText(iStartLine, pMsgPrompt, pRetInputData, Min_Len, Max_Len, '*', iTimeOut);
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetText
 * DESCRIPTION:     Input text on key broad
 * PARAMETERS:      iStartLine: 起始行号(也就是开始显示提示信息的行号)
 *                  pMsgPrompt: 提示信息
 *                  pRetInputData: 输入数据指针
 *                  Min_Len: 最少输入长度  Max_Len: 最大输入长度
 *                  MaskChr: 掩码字符，为0时原样显示，否则输入任何数据都显示MaskChr对应的字符
 *                  iTimeOut: 超时时间
 * RETURN:          -1: timeout
 *                  >=0: input data length
 * NOTES:
 ---------------------------------------------------------------------------- */
int AppUtils_GetText(int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, byte MaskChr, int iTimeOut)
{
	const char  Key_Map[12][10] =
	{
		"0-_ +",   "1QZqz.\\", "2ABCabc&", "3DEFdef", "4GHIghi", "5JKLjkl/",
		"6MNOmno~", "7PRSprs^", "8TUVtuv[", "9WXYwxy]", "*,'\":#=$?", ""
	};
	
	char cInputBuffer[ 126 ], cMaskBuffer[ 126 ];
	int ch, lastKey = 0, curKey = 0;
	int i = 0, j = 0, len, olen, map_index = 0;
    //long tstart = 0, tnow = 0;
	
	AppUtils_FlushKbd();
    //AppUtils_Cls();
	
	if(strlen(pMsgPrompt) > 0)
	{
		MmiUtil_DisplayLine(pMsgPrompt, iStartLine);
		iStartLine ++;
	}
	
	if(AppUtils_IsBigScreen() == BANK_OK)
		MmiUtil_DisplayLine("按#键切换数字字母", LINE9);
	else
		MmiUtil_DisplayLine("按#切换数字字母", LINE4);
		
	memset(cInputBuffer, 0, sizeof(cInputBuffer));
	memset(cMaskBuffer, 0, sizeof(cMaskBuffer));
    //tstart = TimeUtil_MillSec();
    len = 0;
	olen = 0;
	
	if(pRetInputData[ 0 ] != 0)
	{
		len = strlen(pRetInputData);
		
		if(len > 0)
		{
			olen = len - 1;
			
			if(olen < 0)
				olen = 0;
				
			memcpy(cInputBuffer, pRetInputData, len);
			
			if(cInputBuffer[ olen ] >= '0' && cInputBuffer[ olen ] <= '9')
				curKey = cInputBuffer[ olen ];
		}
	}
	
	for(; ;)
	{
		if(MaskChr != 0)
			AppUtils_DisplayRight(cMaskBuffer, iStartLine);
		else
			AppUtils_DisplayRight(cInputBuffer, iStartLine);
			
//AppUtils_GetText_WaitingNow:
        //tnow = TimeUtil_MillSec();
		
        //if((tnow - tstart) / 1000 > iTimeOut)
        //	return -1;
			
        ch = AppUtils_get_WaitKey(1);
        qDebug("get text[%02]", ch);
        //if(ch == 0)
        //	goto AppUtils_GetText_WaitingNow;
        //else
        //{
            //tstart = TimeUtil_MillSec();
			AppUtils_FlushKbd();
			
			switch(ch)
			{
			case '#':
			case bALPHAKEY:
            //case 0x2a:
            //case 0x23:
            case 0x2e:
				if((curKey >= '0' && curKey <= '9') || (curKey == '*'))
				{
					if(curKey == '*')
						i = 10;
					else
						i = curKey - 0x30;
						
					j = strlen(Key_Map[ i ]);
					map_index ++;
					
					if(map_index >= j)
						map_index = 0;
						
					lastKey = Key_Map[ i ][ map_index ];
					cInputBuffer[ olen ] = lastKey;
				}
				
				break;
				
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '*':
			
				// 判断最大输入
				if(len >= Max_Len)
					continue;
					
				curKey = ch;
				map_index = 0;
				cInputBuffer[ len ] = curKey;
				cMaskBuffer[ len ] = MaskChr;
				olen = len;
				len ++;
				break;
				
			case bBKSP:
				curKey = 0;
				len --;
				
				if(len < 0)
					len = 0;
					
				memset(cMaskBuffer + len, 0, sizeof(cMaskBuffer) - len);
				memset(cInputBuffer + len, 0, sizeof(cInputBuffer) - len);
				break;
				
			case bESC:
				return -1;
				
			case bENTER:
			
				if(len < Min_Len)
					continue;
				else
				{
					if(len == 0)
						*pRetInputData = 0;
					else
						strcpy(pRetInputData, cInputBuffer);
						
					return len;
				}
				
			default:
				break;
			}
        //}
	}
}


/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_GetTextbyTouchScreen
* DESCRIPTION:   输入字符串(字母和数字)
* PARAMETERS:
*       LineNo    (in)  -- 提示信息显示行数
*       msgPrompt (in)  -- 提示信息
*       str       (out) -- 输出字符串
*       MinLen    (in)  -- 输入数据最小长度
*       MaxLen    (in)  -- 输入数据最大长度
*       DispMode  (in)  -- 输入数据显示模式(参考DISP_FORMAT定义)
* RETURN:
*       INPUT_OK/INPUT_TIMEOUT/INPUT_USERABORT
* NOTE:
* -------------------------------------------------------------------------*/
int AppUtils_GetTextbyTouchScreen(int LineNo, char *msgPrompt, char *str, usint MinLen, usint MaxLen, int DispMode)
{
#ifndef MAKE_LINUX
	unsigned int KeyPositionMap[51][2] = {{0x00010001, 0x00130012}, {0x00160001, 0x00280012}, {0x002B0001, 0x003D0012}, {0x00400001, 0x00510012}, {0x00550001, 0x00670012}, {0x006A0001, 0x007C0012}, {0x007F0001, 0x00910012}, {0x00940001, 0x00A60012}, {0x00A90001, 0x00BB0012}, {0x00BE0001, 0x00D00012}, {0x00D30001, 0x00EE0012},
		{0x00010015, 0x00130026}, {0x00160015, 0x00280026}, {0x002B0015, 0x003D0026}, {0x00400015, 0x00510026}, {0x00550015, 0x00670026}, {0x006A0015, 0x007C0026}, {0x007F0015, 0x00910026}, {0x00940015, 0x00A60026}, {0x00A90015, 0x00BB0026}, {0x00BE0015, 0x00D00026}, {0x00D30015, 0x00EE0026},
		{0x00100029, 0x0021003A}, {0x00240029, 0x0036003A}, {0x00390029, 0x004B003A}, {0x004E0029, 0x0061003A}, {0x00640029, 0x0076003A}, {0x00790029, 0x008B003A}, {0x008E0029, 0x00A0003A}, {0x00A30029, 0x00B4003A}, {0x00B70029, 0x00C9003A}, {0x00CC0029, 0x00EE003A},
		{0x0001003D, 0x001C004E}, {0x001F003D, 0x0031004E}, {0x0034003D, 0x0046004E}, {0x0049003D, 0x005B004E}, {0x005E003D, 0x0070004E}, {0x0073003D, 0x0085004E}, {0x0088003D, 0x009A004E}, {0x009D003D, 0x00AF004E}, {0x00B2003D, 0x00C4004E}, {0x00C7003D, 0x00D9004E}, {0x00DC003D, 0x00EE004E},
		{0x00060051, 0x00180062}, {0x001B0051, 0x002D0062}, {0x00300051, 0x00420062}, {0x00490051, 0x00980062}, {0x009B0051, 0x00AD0062}, {0x00B00051, 0x00C20062}, {0x00C50051, 0x00D70062}, {0x00DA0051, 0x00EC0062}
	};
	char KeyMapTable[2][51] = {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\x08', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',  '|', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\x0D', '\x0C', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '\x27', '~', '`', '_', ' ', '{', '}', '[', ']'},
		{'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '\x08', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\\', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\x0D', '\x0C', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ':', ';',   '\"', '+', '-', '=', ' ', ',', '.', '/', '?'}
	};
	int CurrKeyTable, touch_x, touch_y, i;
	int KeyAreaStartX, KeyAreaStartY, KeyAreaEndX, KeyAreaEndY;
	int BaseX = 0;
	int BaseY = 220;
	unsigned char bt, DirtyFlag;
	char szInputBuffer[128 + 1] = {0}, szDispBuf[32+1];
	int iUsedLen, iCurrSLen, iRvTouch;
	int iDispLine;
	
	//显示标题
	if(msgPrompt != NULL && strlen(msgPrompt) > 0)
	{
		MmiUtil_ClearLine(LineNo);
		MmiUtil_DisplayLine(msgPrompt, LineNo);
	}
	
	iCurrSLen = strlen(str);
	
	if(iCurrSLen > 128)
		return INPUT_FAILED;
		
	//原字符串
	sprintf(szInputBuffer, "%s", str);
	
	CurrKeyTable = 0;
	DirtyFlag = 1;
	
	for(;;)
	{
		if(DirtyFlag)
		{
			DirtyFlag = 0;
			
			//MmiUtil_ClearLines(LineNo + 1, LINE8);
			
				
			//显示原始字符串
			iUsedLen = 0;
			iDispLine = LineNo + 1;
			
			MmiUtil_ClearLine(iDispLine);
			
			while(iUsedLen < iCurrSLen)
			{
				memset(szDispBuf, 0, sizeof(szDispBuf));
				iUsedLen += BankUtil_CStrlcpy(szDispBuf, szInputBuffer + iUsedLen, (iCurrSLen - iUsedLen) > 16 ? 17 : (iCurrSLen - iUsedLen + 1));
				
				MmiUtil_ClearLine(iDispLine);
				MmiUtil_ClearLine(iDispLine + 1);
				
				switch(DispMode)
				{
				case LEFT_JST:
					MmiUtil_DisplayLine(szDispBuf, iDispLine++);
					break;
				case RIGHT_JST:
					MmiUtil_DisplayRight(szDispBuf, iDispLine++);
					break;
				case CENTER_JST:
				default:
                    //MmiUtil_DisplayCenter(szDispBuf, iDispLine++, FALSE);
					break;
				}
			}
		}
		
		touch_x = touch_y = 0;
		


			
		DirtyFlag = 1;
		
		if(bt)
		{
			switch(bt)
			{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
			case '*':
			case '#':
			
				if(iCurrSLen < MaxLen)
				{
					szInputBuffer[iCurrSLen] = bt;
					iCurrSLen++;
				}
				else
				{
					MmiUtil_Beep();
				}
				
				break;
			case bENTER:
			
				if(iCurrSLen >= MinLen && iCurrSLen <= MaxLen)
				{
					strcpy(str, szInputBuffer);
					return iCurrSLen;
				}
				
				break;
			case bESC:
				return INPUT_FAILED;
			case bBKSP:
			
				if(iCurrSLen > 0)
				{
					iCurrSLen--;
					szInputBuffer[iCurrSLen] = 0;
				}
				
				break;
			default:
				break;
			}
		}
		
		if(iRvTouch)
		{
			for(i = 0; i < 51; i++)
			{
				KeyAreaStartX = BaseX + (KeyPositionMap[i][0] >> 16);
				KeyAreaStartY = BaseY + (KeyPositionMap[i][0] & 0x0000FFFF);
				
				KeyAreaEndX   = BaseX + (KeyPositionMap[i][1] >> 16);
				KeyAreaEndY   = BaseY + (KeyPositionMap[i][1] & 0x0000FFFF);
				
				if(touch_x > KeyAreaStartX && touch_x < KeyAreaEndX
				   && touch_y > KeyAreaStartY && touch_y < KeyAreaEndY)
				{

					
					if(KeyMapTable[CurrKeyTable][i] == '\x0C')    //Shift key
					{
						CurrKeyTable = !CurrKeyTable;

					}
					else if(KeyMapTable[CurrKeyTable][i] == '\x08')    //Backspace key
					{
						if(iCurrSLen > 0)
						{
							iCurrSLen--;
							szInputBuffer[iCurrSLen] = 0;
						}
					}
					else if(KeyMapTable[CurrKeyTable][i] == '\x0D')    //Enter key
					{
						if(iCurrSLen >= MinLen && iCurrSLen <= MaxLen)
						{
							strcpy(str, szInputBuffer);
							return iCurrSLen;
						}
					}
					else
					{
						if(iCurrSLen < MaxLen)
						{
							szInputBuffer[iCurrSLen] = KeyMapTable[CurrKeyTable][i];
							iCurrSLen++;
						}
						else
						{
							MmiUtil_Beep();
						}
					}
					
					break;
				}
			}
			
			if(i == 51)
				DirtyFlag = 0;
		}
	}
	
#else
	return 0;
#endif
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetPassWord
 * DESCRIPTION:     Input text on key broad
 * RETURN:          -1: timeout
 *                  >=0: input data length
 ---------------------------------------------------------------------------- */
int AppUtils_GetPassWord(int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, int iTimeOut)
{
    char cInputBuffer[ 126 ];
    int ch, curKey = 0;
    int len, olen;
    char inputsercrt[128];
    //long tstart = 0, tnow = 0;

    AppUtils_FlushKbd();
    AppUtils_UserClear(iStartLine);
    AppUtils_DisplayLine(pMsgPrompt, iStartLine);
    iStartLine += 1;
    memset(cInputBuffer, 0, sizeof(cInputBuffer));
    //tstart = TimeUtil_MillSec();
    len = 0;
    olen = 0;

    if(pRetInputData[ 0 ] != 0)
    {
        len = strlen(pRetInputData);

        if(len > 0)
        {
            olen = len - 1;

            if(olen < 0)
                olen = 0;

            memcpy(cInputBuffer, pRetInputData, len);

            if(cInputBuffer[ olen ] >= '0' && cInputBuffer[ olen ] <= '9')
                curKey = cInputBuffer[ olen ];
        }
    }

    while(1)
    {
        qDebug("numbuff:%s" , cInputBuffer);
        memset(inputsercrt, 0, sizeof(inputsercrt));
        memset(inputsercrt, '*', len);
        AppUtils_DisplayRight(inputsercrt, iStartLine);

//AppUtils_GetNum_WaitingNow:
        //tnow = TimeUtil_MillSec();

        //if((tnow - tstart) / 1000 > iTimeOut)
        //	return -1;

        ch = AppUtils_get_WaitKey(1);
        qDebug("ch[%02x]", ch);
        //if(ch == 0)
        //	goto AppUtils_GetNum_WaitingNow;
        //else
        //{
            //tstart = TimeUtil_MillSec();
            //AppUtils_FlushKbd();

            switch(ch)
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                qDebug("************1****************");
                if(len >= Max_Len)
                    continue;
                qDebug("************2****************");
                curKey = ch;
                cInputBuffer[ len ] = curKey;
                olen = len;
                len ++;
                qDebug("len[%d],buff[%s]", len, cInputBuffer);
                break;

            case bBKSP:
                len --;

                if(len < 0)
                    len = 0;

                memset(cInputBuffer + len, 0, sizeof(cInputBuffer) - len);
                break;

            case bESC:
                return -1;

            case bENTER:

                if(len < Min_Len)
                    continue;
                else
                {
                    if(len == 0)
                        *pRetInputData = 0;
                    else
                        strcpy(pRetInputData, cInputBuffer);
                    qDebug("len:%d", len);
                    return len;
                }

            default:
                break;
            }
        //}
    }
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetNum
 * DESCRIPTION:     Input text on key broad
 * RETURN:          -1: timeout
 *                  >=0: input data length
 ---------------------------------------------------------------------------- */
int AppUtils_GetNum(int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, int iTimeOut)
{
	char cInputBuffer[ 126 ];
	int ch, curKey = 0;
	int len, olen;
    //long tstart = 0, tnow = 0;
	
	AppUtils_FlushKbd();
    AppUtils_UserClear(iStartLine);
    AppUtils_DisplayLine(pMsgPrompt, iStartLine);
    iStartLine += 1;
	memset(cInputBuffer, 0, sizeof(cInputBuffer));
    //tstart = TimeUtil_MillSec();
	len = 0;
	olen = 0;
	
	if(pRetInputData[ 0 ] != 0)
	{
		len = strlen(pRetInputData);
		
		if(len > 0)
		{
			olen = len - 1;
			
			if(olen < 0)
				olen = 0;
				
			memcpy(cInputBuffer, pRetInputData, len);
			
			if(cInputBuffer[ olen ] >= '0' && cInputBuffer[ olen ] <= '9')
				curKey = cInputBuffer[ olen ];
		}
	}
	
    while(1)
	{
        qDebug("numbuff:%s" , cInputBuffer);
		AppUtils_DisplayRight(cInputBuffer, iStartLine);
		
//AppUtils_GetNum_WaitingNow:
        //tnow = TimeUtil_MillSec();
		
        //if((tnow - tstart) / 1000 > iTimeOut)
        //	return -1;
			
        ch = AppUtils_get_WaitKey(1);
        qDebug("ch[%02x]", ch);
        //if(ch == 0)
        //	goto AppUtils_GetNum_WaitingNow;
        //else
        //{
            //tstart = TimeUtil_MillSec();
            //AppUtils_FlushKbd();
			
			switch(ch)
			{
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                qDebug("************1****************");
				if(len >= Max_Len)
					continue;
                qDebug("************2****************");
				curKey = ch;
				cInputBuffer[ len ] = curKey;
				olen = len;
				len ++;
                qDebug("len[%d],buff[%s]", len, cInputBuffer);
				break;
				
			case bBKSP:
				len --;
				
				if(len < 0)
					len = 0;
					
				memset(cInputBuffer + len, 0, sizeof(cInputBuffer) - len);
				break;
				
			case bESC:
				return -1;
				
			case bENTER:
			
				if(len < Min_Len)
					continue;
				else
				{
					if(len == 0)
						*pRetInputData = 0;
					else
						strcpy(pRetInputData, cInputBuffer);
                    qDebug("len:%d", len);
					return len;
				}
				
			default:
				break;
			}
        //}
	}
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_InputNum
 * DESCRIPTION:     Input num by keyboard
 * RETURN:          -1: timeout
 *                  >=0: input data length
 ---------------------------------------------------------------------------- */
int AppUtils_InputNum(int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, int DispMode)
{
//	return MmiUtil_InputNum(iStartLine, pMsgPrompt, pRetInputData, Min_Len, Max_Len, DispMode);
    return 0;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_GetPassWord
* DESCRIPTION:   输入密码
* PARAMETERS:
*       disp_line (in)  -- 提示信息显示行数
*       msgPrompt (in)  -- 提示信息
*       pwd       (out) -- 输出密码
*       min       (in)  -- 输入数据最小长度
*       max       (in)  -- 输入数据最大长度
*       disp_mode (in)  -- 输入数据显示模式(参考DISP_FORMAT定义)
* RETURN:
*             INPUT_OK/INPUT_TIMEOUT/INPUT_USERABORT
* NOTE:
* -------------------------------------------------------------------------*/
int AppUtils_GetPassWord(int disp_line, char *msgPrompt, char *pwd, usint min, usint max, int disp_mode)
{
    //return MmiUtil_InputPWD(disp_line, msgPrompt, pwd, min, max, disp_mode);
    return 0;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: AppUtils_GetIPAddress
 * DESCRIPTION:   输入IP地址风格的字符串，可以用#号键代替输入'.'
 *                可以接受的字符串为16字节（含NULL）
 *                在第二行显示Msg中的提示信息，第三行显示和输入IP地址
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int AppUtils_GetIPAddress(char *Msg, char *str)
{
	char  cBuffer[32], cDispBuf[32];
	int   i = 0, iMaxLen, ch;
	const int iMaxChars = 16;
	
	memset(cBuffer, 0, sizeof(cBuffer));
	
	//显示输入信息
	MmiUtil_DisplayLine((char *) Msg, LINE2);
	
	//左显示'-'
	iMaxLen = 15;
	memset(cDispBuf, 0, sizeof(cDispBuf));
	memset(cDispBuf, '_', iMaxLen);
	
	i = strlen(str);
	
	if(i >= 15)
		i = 15;
		
	memcpy(cBuffer, str, i);
	memcpy(cDispBuf, str, i);
	
	MmiUtil_ClearLine(LINE3);
//	MmiUtil_DispChinese((char *) cDispBuf, (iMaxChars - iMaxLen), LINE3);
	
	for(;;)
	{
		ch = AppUtils_WaitKey(60);
		
		switch(ch)
		{
			//常规输入
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '*':
		case '#':
		
			/*判断最大输入*/
			if(i == iMaxLen)
			{
				MmiUtil_Beep();
				continue;
			}
			
			cBuffer[i] = ch;
			
			if(cBuffer[i] == '*' || cBuffer[i] == '#')
				cBuffer[i] = '.';
				
			i ++;
			break;
			
			//回退
		case bBKSP:
		
			if(i == 0)
			{
				MmiUtil_Beep();
				continue;
			}
			
			cBuffer[i--] = '\0';
			break;
			
			//取消
		case bESC:
			return INPUT_CANCEL;
			
			//确认
		case bENTER:
			cBuffer[i] = 0;
			memset(str, 0, 16);    //清干净原缓冲区
			memcpy(str, cBuffer, i);
			return i;//---返回输入长度
			
		default:
			AppUtils_ErrorBeep();
			break;
		}
		
		
		memset(cDispBuf, 0, sizeof(cDispBuf));
		memset(cDispBuf, '_', iMaxLen);
		memcpy(cDispBuf, cBuffer, i);
		
		MmiUtil_ClearLine(3);
//		MmiUtil_DispChinese((char *) cDispBuf, (iMaxChars - iMaxLen), 3);
	}
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: AppUtils_GetFileString
 * DESCRIPTION:   主要用于只需要供终端读取的可配置内容
 *                (主要是＂响应码－提示信息＂和＂银行代码－中文名称＂)
 * 读取INI类型文件：含[Section], Key=Value模式的Value内容
 * fn:文件名,可带路径
[CONFIG]
ServerIP=192.168.0.170
ServerPort=8901
 * --------------------------------------------------------------------------*/
#define MAX_SECTION_LEN 512
#define MAX_LINE_LEN  512
#define MAX_KEY_LEN     255
#define MAX_VALUE_LEN 255

int AppUtils_GetFileString(char *fileName, char *Section, char *key, char *value)
{
	int fd;
	int  i, flag, len, keylen, valuelen;
	char buf[MAX_LINE_LEN + 1], tmpbuf[MAX_SECTION_LEN + 1];
	
	flag = 0;
	tmpbuf[0] = '[';
	
	len = strlen(Section);
	
	if(len <= 0 || len > MAX_SECTION_LEN)
		return(-1);
		
	memcpy(tmpbuf + 1, Section, len);
	tmpbuf[len + 1] = ']';
	tmpbuf[len + 2] = 0;
	
    fd = open(fileName, O_RDONLY);
	
	if(fd < 0)
		return(-2);
		
	keylen = strlen(key);
	
	if(keylen < 0 || keylen > MAX_KEY_LEN)
	{
        close(fd);
		return(-3);
	}
	
	
//	while(File_ReadLine(fd, buf, sizeof(buf)) > 0)
	{
		if(buf[0] == '#')    //#为注释掉的，跳过
    //		continue;
			
		if(buf[0] == '[')    //遇到'['开头的，是Section Name，下面需要匹配Section Name
			flag = 0;
			
		if(!memcmp(buf, tmpbuf, len + 2))       //如果匹配到了相同的Section，则置flag并且直接读取下一行
		{
			flag = 1;
    //		continue;
		}
		
		
		if(!memcmp(buf, key, keylen) && flag)
		{
			for(i = 0; i < MAX_KEY_LEN; i++)
			{
				//增加'\r'作为结束符，以适应windows/dos文本格式(Windows/Dos文本换行符0x0D 0x0A即'\r' '\n')
				//Linux/Unix换行符是0x0A即'\n'
				if(buf[i] == '#' || buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\r')
				{
					buf[i] = 0;
				}
			}
			
			valuelen = (strchr(buf, '\0') - buf) - keylen - 1;
			
			if(valuelen  <= 0 || valuelen > MAX_VALUE_LEN)
			{
                close(fd);
				return(-3);
			}
			
			memcpy(value, buf + keylen + 1, valuelen);
			value[valuelen] = 0;
            close(fd);
			return(valuelen);
		}
	}
	
    close(fd);
	return(-4);
}


int AppUtils_GetSysDateTime(char *buffer)
{
	char buf[16];
	const char WeekDayMap[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	memset(buf, 0, sizeof(buf));
	
	AppUtils_GetClock(buf);
	
	memcpy(buffer, buf, 4);    //YYYY
	memcpy(buffer + 4, "/", 1);
	memcpy(buffer + 5, buf + 4, 2);    //MM
	memcpy(buffer + 7, "/", 1);
	memcpy(buffer + 8, buf + 6, 2);    //DD
	buffer[10] = ' ';
	memcpy(buffer + 11, WeekDayMap[buf[14] - '0'], 3);
	
	buffer[14] = ' ';
	
	memcpy(buffer + 15, buf + 8, 2);    //HH
	buffer[17] = ':';
	memcpy(buffer + 18, buf + 10, 2);    //MM
	
	return(0);
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetTermModuleName
 * DESCRIPTION:     获取机型名称(Cx平台直接返回C520)
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
char *AppUtils_GetTermModuleName(void)
{
	static char szMNameBuf[80];
	int i;
	
	memset(szMNameBuf, 0, sizeof(szMNameBuf));
//	SVC_INFO_MODELNO(szMNameBuf);
	
	for(i = strlen(szMNameBuf); i > 0; i--)
	{
		if(szMNameBuf[i - 1] == ' ')
			szMNameBuf[i - 1] = 0;
		else
			break;
	}
	
	if(memcmp(szMNameBuf, "O5150", 5) == 0)
	{
		memcpy(szMNameBuf, "VX510", 5);
	}
	
	return (char *) szMNameBuf;
}

int AppUtils_GetTerminalType(void)
{
    return _VX680;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_YesOrNo
 * DESCRIPTION:     参数用的交易开关选择
 * RETURN:          BANK_OK: 输入成功
                    BANK_FAIL: 输入失败
 ---------------------------------------------------------------------------- */
int AppUtils_YesOrNo(char *pMsg, char *pMsg2, int *Value)
{
	int iKey;
	char cDispBuf[ 20 ];
	
	AppUtils_UserClear(2);
    //MmiUtil_DisplayCenter(pMsg, LINE2, FALSE);
    //MmiUtil_DisplayCenter(pMsg2, LINE3, FALSE);
	
	while(1)
	{
		sprintf(cDispBuf, "(%d)", *Value);
		MmiUtil_DisplayRight(cDispBuf, 4);
		
		iKey = AppUtils_WaitKey(60);
		
		if(iKey == bKEY1)
		{
			*Value = 1;
		}
		else if(iKey == bKEY0)
		{
			*Value = 0;
		}
		else if(iKey == bENTER)
			break;
		else if(iKey == bESC || iKey == 0)
			return BANK_FAIL;
	}
	
	return BANK_OK;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_YesOrNoEx
 * DESCRIPTION:     参数用的交易开关选择
 * RETURN:          BANK_OK: 输入成功
                    BANK_FAIL: 输入失败
 ---------------------------------------------------------------------------- */
int AppUtils_YesOrNoEx(char *pMsg, char *pMsg2, int *Value)
{
	int iKey;
	char cDispBuf[ 20 ];
	
	AppUtils_UserClear(2);
    //MmiUtil_DisplayCenter(pMsg, LINE2, FALSE);
    //MmiUtil_DisplayCenter(pMsg2, LINE3, FALSE);
	
	while(1)
	{
		sprintf(cDispBuf, "(%d)", *Value);
		MmiUtil_DisplayRight(cDispBuf, 4);
		
		iKey = AppUtils_WaitKey(60);
		
		if(iKey == bKEY1)
		{
			*Value = 1;
		}
		else if(iKey == bKEY2)
		{
			*Value = 2;
		}
		else if(iKey == bENTER)
			break;
		else if(iKey == bESC || iKey == 0)
			return BANK_FAIL;
	}
	
	return BANK_OK;
}


int AppUtils_YesNoConfirm(char *pTitle, char *pMsg1, char *pMsg2, char *pPromptMsg)
{
	int iLine;
	byte bKey;
	
	if(strlen(pTitle) > 0)
	{
		AppUtils_Cls();
		AppUtils_DisplayCenter(pTitle, LINE1, TRUE);
	}
	else
		AppUtils_UserClear(LINE2);

    iLine = AppUtils_DisplayLine(pMsg1, LINE2);
    iLine = AppUtils_DisplayLine(pMsg2, LINE3);
    iLine = LINE4;
	if(AppUtils_IsBigScreen() != BANK_OK)
		iLine = LINE4;
		
	if(pPromptMsg == NULL || strlen(pPromptMsg) == 0)
		AppUtils_DisplayLine("按O键确认X键退出", iLine);
	else
		AppUtils_DisplayLine(pPromptMsg, iLine);
		
	while(TRUE)
	{
        bKey = AppUtils_get_WaitKey(60);
		
		if((bKey == bESC) || (bKey == 'X') || (bKey == 0))
			return BANK_FAIL;
		else if((bKey == bENTER) || (bKey == '0'))
			break;
	}
	
	return BANK_OK;
}


/******************************************************************************
 * FUNCTION NAME:   AppUtils_IsColorScreen
 * DESCRIPTION:     Check if terminal is the color screen type
 * RETURN:          BANK_OK: Color screen type as VX680 VX820
                    BANK_FAIL: Mono screen type
 ******************************************************************************/
int AppUtils_IsColorScreen(void)
{
	switch(AppUtils_GetTerminalType())
	{
	case _VX680:
	case _VX680C:
	case _VX820:
	case _VX675:
		return BANK_OK;
		
	default:
		return BANK_FAIL;
	}
}




/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_RemoveFile
 * DESCRIPTION:     delete & remove file in RAM
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_RemoveFile(char *pFileName)
{
	int fd;
	
    fd = open(pFileName, O_RDONLY);
	
	if(fd <= 0)
		return BANK_OK;
		
    close(fd);
	
	if(File_Remove(pFileName) != FILE_SUCCESS)
	{
		AppUtils_WaitKeyMill(100);
		
		if(remove(pFileName) < 0)
		{
            fd = open(pFileName, O_WRONLY);
			
			if(fd > 0)
			{
                close(fd);
				return BANK_FAIL;
			}
			else
				return BANK_OK;
		}
	}
	
	return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetNumberOfRecords
 * DESCRIPTION:     get file total records num
 * RETURN:          num_record
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_GetNumberOfRecords(const char *pFileName, int iRecordLength)
{
	return File_GetNumberOfRecords(pFileName, iRecordLength);
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AppendRecord
 * DESCRIPTION:     append record to Trans Database file
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_AppendRecord(const char *pFileName, char *pRecord, int iRecordLength)
{
    if(File_AppendRecord((char *)pFileName,  pRecord, iRecordLength) == FILE_SUCCESS)
		return BANK_OK;
	else
		return BANK_FAIL;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetRecordByIndex
 * DESCRIPTION:     get file total number
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_GetRecordByIndex(const char *pFileName, void *pRecord, int iRecordLength, int iRecordIndex)
{
//	if(File_GetRecordByIndex(pFileName, pRecord, iRecordLength, iRecordIndex) == FILE_SUCCESS)
		return BANK_OK;
//	else//
        //return BANK_FAIL;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetTransRecByIdx
 * DESCRIPTION:     Get trans database record by index
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_GetTransRecByIdx(void *pRecord, int iRecordIndex)
{
	if(File_GetRecordByIndex(TRAN_DB, (void *)pRecord, sizeof(TransRecord), iRecordIndex) == FILE_SUCCESS)
		return BANK_OK;
	else
		return BANK_FAIL;
}

int AppUtils_CheckDateFormatValid(char *pDateString)
{
	int iMon, iDay;
	int DateStr[ ] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	char cTmpBuf[ 3 ];
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	memcpy(cTmpBuf, pDateString, 2);
	iMon = atoi(cTmpBuf);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	memcpy(cTmpBuf, pDateString + 2, 2);
	iDay = atoi(cTmpBuf);
	
	if((iMon < 1) || (iMon > 12) || (iDay == 0))
		return BANK_FAIL;
		
	if(iDay > DateStr[ iMon - 1 ])
		return BANK_FAIL;
		
	return BANK_OK;
}

int AppUtils_CheckTipFloorLimit(byte *pBaseAmt, byte *pTipAmt)
{
	int iPercent;
	double fBaseAmt, fTipAmt;
	double fFeePercent;
	int iRet;
	
	AppUtils_Bcd2Len(&gTermCfg.mFixPrm.FeePercent, &iPercent, 1);
	fFeePercent = iPercent;
	fFeePercent /= 100;
	
	fBaseAmt = atol((char *)pBaseAmt);
	fBaseAmt *= fFeePercent;
	
	fTipAmt = atol((char *)pTipAmt);
	
	AppDebug_AppLog("lFeePercent=%.2f,lBaseAmt=%.2f,lTipAmt=%.2f", fFeePercent, fBaseAmt, fTipAmt);
	
	if(fTipAmt > fBaseAmt)
	{
		iRet = BANK_FAIL;
		PrnDisp_DisplayWarning("", "  输入金额无效  超小费额度");
	}
	else
		iRet = BANK_OK;
		
	return iRet;
}


void AppUtils_ConvertTransDateTime(DynData *pTxnData, char *pRetDateTimeASC)
{
	char *p;
	
	p = pRetDateTimeASC;
	
	AppUtils_Bcd2Asc(pTxnData->TransDB.TranDate, (byte *)p, 4);
	p += 4;
	
	*p ++ = '-';
	
	AppUtils_Bcd2Asc(pTxnData->TransDB.TranDate + 2, (byte *)p, 2);
	p += 2;
	
	*p ++ = '-';
	
	AppUtils_Bcd2Asc(pTxnData->TransDB.TranDate + 3, (byte *)p, 2);
	p += 2;
	
	*p ++ = ' ';
	
	AppUtils_Bcd2Asc(pTxnData->TransDB.TranTime, (byte *)p, 2);
	p += 2;
	
	*p ++ = ':';
	AppUtils_Bcd2Asc(pTxnData->TransDB.TranTime + 1, (byte *)p, 2);
	p += 2;
	
	*p ++ = ':';
	
	AppUtils_Bcd2Asc(pTxnData->TransDB.TranTime + 2, (byte *)p, 2);
	p += 2;
	
	*p = 0;
	
	return;
}


byte AppUtils_ConvertAmountNegativeFlag(DynData *pTxnData)
{
	byte bNegativeFlag;
	
	switch(pTxnData->TransDB.TrType)
	{
	case BTYPE_REFUND:
	case BTYPE_ECASH_REFUND:
	case BTYPE_SALE_VOID:
	case BTYPE_COMPLETE_VOID:
	case BTYPE_INSTALLMENT_VOID:
	case BTYPE_MCHIP_COMPLETE_VOID:
	case BTYPE_MCHIP_SALE_VOID:
	case BTYPE_MCHIP_REFUND:
	case BTYPE_YUYUE_SALE_VOID:
	case BTYPE_ISSUER_SCORE_SALE_VOID:
	case BTYPE_UNION_SCORE_SALE_VOID:
	case BTYPE_UNION_SCORE_REFUND:
	case BTYPE_ORDER_REFUND:
	case BTYPE_ORDER_SALE_VOID:
	case BTYPE_ORDER_COMPLETE_VOID:
	case BTYPE_PREAUTH_VOID:
	case BTYPE_MCHIP_PREAUTH_VOID:
	case BTYPE_ORDER_PREAUTH_VOID:
		bNegativeFlag = '-';
		break;
	default:
		bNegativeFlag = '+';
		break;
	}
	
	return bNegativeFlag;
}


int AppUtils_ConvertPANbyMask(DynData *pTxnData, int iPANseq, char *pRetPAN)
{
	int i, iTmpLen;
	
	memset(pRetPAN, 0, 20);
	
	if(iPANseq == 2)
	{
		memcpy(pRetPAN, pTxnData->TransDB.PAN_2, pTxnData->TransDB.PAN_2Len);
		iTmpLen = pTxnData->TransDB.PAN_2Len;
	}
	else
	{
		memcpy(pRetPAN, pTxnData->TransDB.PAN, pTxnData->TransDB.PANLen);
		iTmpLen = pTxnData->TransDB.PANLen;
	}
	
	if((gTermCfg.mTrSwitch.MaskPanFlag == 0) || (iPANseq == 0))
		return BANK_OK;
		
	if(((pTxnData->TransDB.TrType == BTYPE_PREAUTH) ||
	    (pTxnData->TransDB.TrType == BTYPE_ORDER_PREAUTH) ||
	    (pTxnData->TransDB.TrType == BTYPE_MCHIP_PREAUTH))
	   && !gTermCfg.mTrSwitch.PreAuthCardNoMask)
	{
		;
	}
	else if(AppUtils_CheckStatus(pTxnData->TransDB.Status[ 0 ], STATUS_OFFLINE) == BANK_OK)
	{
		;
	}
	else
	{
		//卡号只显示前6后4位，其余用星号屏蔽掉
		iTmpLen = iTmpLen - 4;
		
		for(i = 6; i < iTmpLen; i ++)
			pRetPAN[ i ] = '*';
	}
	
	return BANK_OK;
}



void AppUtils_NormalBeep(void)
{
	MmiUtil_Beep();
	return;
}

void AppUtils_ErrorBeep(void)
{
    //MmiUtil_ErrorSound();
	return;
}

void AppUtils_Window(int startX, int startY, int endX, int endY)
{
    //MmiUtil_Window(startX, startY, endX, endY);
}

void AppUtils_SetFontSize(int font_size)
{
    //////MmiUtil_SetFontSize(font_size);
}

void AppUtils_SetFontFile(const char *szFonetFileName)
{
    //MmiUtil_SetFont(szFonetFileName);
}

void AppUtils_SetTimeOut(unsigned int Timeout)
{
	inAppUtils_TimeOutSecs = Timeout;
    //MmiUtil_SetTimeOut((unsigned short)Timeout);
}

int AppUtils_GetTimeOut(void)
{
	return inAppUtils_TimeOutSecs;
}

int AppUtils_DispBMP(char *BmpFile, int x, int y, int width, int height)
{
    //return MmiUtil_DispBMP(BmpFile, x, y, width, height);
    return 0;
}

void AppUtils_DispStrByColor(char *str, int startX, int startY, int fontSize, unsigned long  RGBFontColor, unsigned long  RGBBGColor)
{
    //MmiUtil_DispStrByColor(str, startX, startY, fontSize, RGBFontColor, RGBBGColor);
    return ;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtilS_SetTouchButton
* DESCRIPTION:   初始化显示button按钮参数
* PARAMETERS:
*       iButtonHeight        (in) -- button的高度               （0-默认32像素）
*       iTextFontSize        (in) -- button上显示的文本的大小   （0-默认24点阵）
*       ulTextRGBColor       (in) -- button显示的文本的颜色     （0-默认黑色）
* RETURN:
* NOTES:        不会清屏，只是初始化BUTTON显示和触发时的相关参数
* ------------------------------------------------------------------------ */
void AppUtilS_SetTouchButton(int iButtonHeight, int iTextFontSize, unsigned long ulTextRGBColor)
{
    //MmiUtil_SetTouchButton(iButtonHeight, iTextFontSize, ulTextRGBColor);
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_ShowTouchButton
* DESCRIPTION:   显示button按钮,可配合CheckTouchButton使用
* PARAMETERS:
*       Str      (in)     -- button上显示的字（带\x00结束符）
*       x        (in/out) -- button显示左上角横坐标（像素）,返回button的右下角横坐标
*       y        (in/out) -- button显示左上角纵坐标（像素）,返回button的右下角纵坐标
*       RetVal   (in)     -- 按下button时返回值设定
* RETURN:
* NOTES:        最多可支持20个BUTTON，使用前须用MmiUtil_SetTouchButton进行设置
*               若出现BUTTON重叠，则以最后一个显示的BUTTON为准
*               可通过返回的(x,y)坐标决定下一个BUTTON显示的位置
* ------------------------------------------------------------------------ */
void AppUtils_ShowTouchButton(char *Str, int *x, int *y, int RetVal)
{
    //MmiUtil_ShowTouchButton(Str, x, y, RetVal);
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_CheckTouchButton
* DESCRIPTION:   检查是否有BUTTON被按下
* PARAMETERS:
*       RetVal (out)     - 被按下的BUTTON设定的返回值
*
* RETURN:        0 - 无按钮被按下
*                1 - 有按钮被按下
*
* NOTES:         非阻塞，立即返回
*                若出现BUTTON重叠，则以最后一个显示的BUTTON为准
*                如果没有做MmiUtil_SetTouchButton，则无法检测到BUTTON
* ------------------------------------------------------------------------ */
int AppUtils_CheckTouchButton(int *RetVal)
{
    //return MmiUtil_CheckTouchButton(RetVal);
    return 0;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_SetFontColor
* DESCRIPTION:  设置字体颜色，对彩屏机型有效
* PARAMETERS:
*   FontColor (in) -- 字体颜色RGB（后3字节分别表示RED,GREEN,BLUE）
* RETURN:
* NOTES:
* ------------------------------------------------------------------------ */
void AppUtils_SetFontColor(unsigned long RGBFontColor)
{
    //MmiUtil_SetFontColor(RGBFontColor);
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_Restart
* DESCRIPTION:   终端重启
* PARAMETERS:
*       filename (in) -- 可执行文件名或""
* RETURN:
*       成功 --
*       失败 -- (<0)
* NOTES:
* ------------------------------------------------------------------------ */
int AppUtils_Restart(const char *filename)
{
    //return MmiUtil_Restart(filename);
    return 0;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_IntelligentInputIP
* DESCRIPTION:   Intelligent IP address input function
*                智能IP地址输入函数: 可以根据前一位数值自动加分隔点
* PARAMETERS:    PromptDispLine 提示信息显示的行号
*                msgPrompt 提示信息内容
*                InputDispLine 输入信息所在的行号
*                ip 输入的IP地址信息
*                TimeOutSec 输入超时时间
* RETURN:        >0 / BANK_TIMEOUT / BANK_ABORT
* NOTES:
* ------------------------------------------------------------------------ */
int AppUtils_IntelligentInputIP(int PromptDispLine, char *msgPrompt, int InputDispLine, char *ip, int TimeOutSec)
{
    return 0;
}
/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_GetAppBuildTimeStamp
* DESCRIPTION:   获取应用构建的时间戳: 用于测试阶段的版本管理和区分
* PARAMETERS:    None
* RETURN:        时间戳字符串指针
* NOTES:         by XC 2014-6-23 15:20:05
* ------------------------------------------------------------------------ */
char *AppUtils_GetAppBuildTimeStamp(void)
{
	static char BuildTS[32 + 1];
	const char MonthNameMap[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	int i;
	char  TmpBuf[32], BldDate[32], BldTime[32];
	
	memset(BuildTS, 0, sizeof(BuildTS));
	memset(TmpBuf, 0, sizeof(TmpBuf));
	
	//年
	sprintf(BldDate, "%s", __DATE__);
	memcpy(BuildTS, BldDate + 7, 4);
	BuildTS[4] = '/';
	
	//月
	for(i = 0; i < 12; i++)
	{
		if(!memcmp(MonthNameMap[i], BldDate, 3))     /* 比较月份 */
		{
			sprintf(BuildTS + 5, "%02d", i + 1);
			break;
		}
	}
	
	//日
	BuildTS[7] = '/';
	memset(TmpBuf, 0, sizeof(TmpBuf));
	memcpy(TmpBuf, BldDate + 4, 2);
	sprintf(BuildTS + 8, "%02d ", atoi(TmpBuf));
	
	
	sprintf(BldTime, "%s", __TIME__);
	strcat(BuildTS, BldTime);
	
	return (char *)BuildTS;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_BatteryVolToValue
* DESCRIPTION:   把电池电压信息转换成电量百分比数据
* PARAMETERS:    BattVol: 电压(mV毫伏)
* RETURN:        电量(0-100)
* NOTES:
* ------------------------------------------------------------------------ */
int AppUtils_BatteryVolToValue(int BattVol)
{
	int BattValue;
	
	if((BattVol >= 4100))
	{
		BattValue = 100;
	}
	else if((BattVol >= 4049))
	{
		BattValue = 98;
	}
	else if((BattVol >= 3974))
	{
		BattValue = 90;
	}
	else if((BattVol >= 3904))
	{
		BattValue = 80;
	}
	else if((BattVol >= 3848))
	{
		BattValue = 70;
	}
	else if((BattVol >= 3791))
	{
		BattValue = 60;
	}
	else if((BattVol >= 3740))
	{
		BattValue = 50;
	}
	else if((BattVol >= 3697))
	{
		BattValue = 40;
	}
	else if((BattVol >= 3650))
	{
		BattValue = 30;
	}
	else if((BattVol >= 3627))
	{
		BattValue = 20;
	}
	else if((BattVol >= 3600))
	{
		BattValue = 10;
	}
	else
	{
		BattValue = 0;
	}
	
	return BattValue;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_CheckBattery
* DESCRIPTION:   电池检查：通过虚电判断或电量判断等情况，决定是否允许做交易
* PARAMETERS:    None
* RETURN:        BANK_OK / BANK_FAIL
* NOTES:
* ------------------------------------------------------------------------ */
int AppUtils_CheckBattery(void)
{

	return BANK_OK;

}

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_ClosePrinter
* DESCRIPTION:   关闭打印机
* PARAMETERS:    None
* RETURN:        BANK_OK / BANK_FAIL
* NOTES:
* ------------------------------------------------------------------------ */
int AppUtils_ClosePrinter(void)
{
#ifdef MAKE_LINUX
	Print_Close();
	return BANK_OK;
#else
	char b[4];
	int status;
    int iTermType = 1;
	
	//打印后如果电压小于等于3550mV 认为电池存在虚电 置Batt_Flag标志
    //if(iTermType == _VX675 && get_battery_value(BATTERYVOLTAGE) <= 3550)
	{
		Batt_Flag = 0;
	}
	
	do
	{
        //status = get_port_status(inAppUtils_PrinterHandle, b);
	
		if(status == 0)
			break;
	
		AppUtils_SVCWAIT(20);
	}
	while(status > 0);
	
	close(inAppUtils_PrinterHandle);
	inAppUtils_PrinterHandle = -1;
	
	return BANK_OK;
#endif
}

/* -----------------------------------------------------------------------------
 * Private Function define
 ---------------------------------------------------------------------------- */

static void inAppUtils_Cdel0(byte *pSrc)
{
	int l;
	int i = 0, j;
	
	l = strlen((char *) pSrc);
	
	while(pSrc[ i ] == '0')
		++ i;
		
	if(i > 0)
	{
		for(j = 0; j < l - i; ++ j)
			pSrc[ j ] = pSrc[ j + i ];
			
		for(j = l - i; j < l; ++ j)
			pSrc[ j ] = 0;
	}
	
	if(pSrc[ 0 ] == 0)
		pSrc[ 0 ] = '0';
}

static int inAppUtils_Scmp(byte *pSrc1, byte *pSrc2)
{
	int l1, l2;
	
	inAppUtils_Cdel0(pSrc1);
	inAppUtils_Cdel0(pSrc2);
	
	l1 = strlen((char *) pSrc1);
	l2 = strlen((char *) pSrc2);
	
	if(l1 > l2)
		return 1;
	else if(l1 < l2)
		return -1;
		
	return (strcmp((char *) pSrc1, (char *) pSrc2));
}

