#ifndef __APPUTILS_H__
#define __APPUTILS_H__
#include "define.h"

int AppUtils_GetPassWord(int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, int iTimeOut);

unsigned int BankUtil_CStrlcpy(char *dst, const char *src, unsigned int siz);
/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_flushkdb
 * DESCRIPTION:     clear button memory
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_flushkbd(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_flushcard
 * DESCRIPTION:     clear card memory
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_flushcard(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_ResetScreen
 * DESCRIPTION:     init and clear screen, and set default font
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_ResetScreen(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_ClearLine
 * DESCRIPTION:     clear one line message has been displayed
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
void AppUtils_ClearLine(int iLineNo);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_IsBigScreen
 * DESCRIPTION:     Check if terminal is the big screen type
 * RETURN:          BANK_OK: big screen terminal has more than 4 lines display
                    BANK_FAIL: normal screen terminal has 4 lines display
 ---------------------------------------------------------------------------- */
int AppUtils_IsBigScreen(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_IsTouchScreen
 * DESCRIPTION:     Check if terminal has the touch screen
 * RETURN:          BANK_OK: Yes
                    BANK_FAIL: No
 ---------------------------------------------------------------------------- */
int AppUtils_IsTouchScreen(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_IsBatteryTerminal
 * DESCRIPTION:     Check if terminal has battery
 * RETURN:          TRUE: has battery
                    FALSE: has no battery
 ---------------------------------------------------------------------------- */
int AppUtils_IsBatteryTerminal(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_IsGprsTerminal
 * DESCRIPTION:     Check if terminal has gprs modular
 * RETURN:          TRUE: has gprs modular
                    FALSE: has no gprs modular
 ---------------------------------------------------------------------------- */
int AppUtils_IsGprsTerminal(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_NullComp
 * DESCRIPTION:     Check if the data is NULL or not
 * RETURN:          TRUE: data is NULL
                    FALSE: data is not NULL
 ---------------------------------------------------------------------------- */
int AppUtils_IsNullData(char *pCheckData, int iDataLength);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DelTrail
 * DESCRIPTION:     deletes the trailing ( given ) chars from a string
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_DeleteTrail(char cDeleteLetter, char *pString);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetSerialNo
 * DESCRIPTION:     get terminal serial NO.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_GetSerialNo( char *pRetSerialNo );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_FillBufWithChar
 * DESCRIPTION:     用特定字符填充缓冲区(原始字符串如果>=总长度则不做处理)
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_FillBufWithChar(char *OrgStr, char FillChar, int Length);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetTermModuleName
 * DESCRIPTION:     获取机型名称(Cx平台直接返回C520)
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
char *AppUtils_GetTermModuleName(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_SetYear
 * DESCRIPTION:     set year of date
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_SetYear(char *pNewYear);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetClock
 * DESCRIPTION:     Read Clock Data into pClockData
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_GetClock(char *pClockData);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_UpdateDateTime
 * DESCRIPTION:     Update Date: "MMDD" and Time: "HHMMSS" (Both ASCII String)
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_UpdateDateTime(char *pNewDate, char *pNewTime);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetPosDateTime
 * DESCRIPTION:     Get terminal date time, format: yyyy-mm-dd hh:mm:ss
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_GetPosDateTime(char *pRetTerminalDateTime);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetTerminalTicks
 * DESCRIPTION:     Get terminal Ticks, short second digit from terminal start to now
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_GetTerminalTicks(char *pRetTerminalTicks);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_ReadTicks
 * DESCRIPTION:     Get terminal Ticks, short second digit from terminal start to now
 * RETURN:          None
 ---------------------------------------------------------------------------- */
unsigned long AppUtils_ReadTicks( void );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Asc2Bcd
 * DESCRIPTION:     AscBuf - Ascii input buffer, must ended by '\0'
 *                  BcdBuf - converted result
 *                  Len - double length of BCD code, should be even.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Asc2Bcd(byte *pAscBuf, byte *pBcdBuf, int iAscLength);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Bcd2Asc
 * DESCRIPTION:     BcdBuf - BCD input buffer,
 *                  AscBuf - converted result, terminated by '\0',
 *                  Len - double length of BCD code, must be even.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Bcd2Asc(byte *pBcdBuf, byte *pAscBuf, int iAscLength);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Bcd2Len
 * DESCRIPTION:     Convert BcdLen bytes BCD length to usint.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Bcd2Len(byte *pBcdBuf, int *pRetLength, int iBcdLength);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Bcd2Asc
 * DESCRIPTION:     Convert usint to n bytes BCD
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Len2Bcd(int iLength, byte *pRetBcdBuf, int iBcdLength);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Hex2Long
 * DESCRIPTION:     Convert n bytes HEX data to long data
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Hex2Long( unsigned long *lRetLong, byte *pHexBuf, int iHexLength );


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Long2Hex
 * DESCRIPTION:     Convert long data to n bytes HEX data
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_Long2Hex( unsigned long lLongth, byte *pHexBuf, int iHexLength );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_SetTransStatus
 * DESCRIPTION:     Check Transaction record status
 * RETURN:          BANK_OK: status bit exists
 *                  BANK_FAIL: status bit not exists
 ---------------------------------------------------------------------------- */
int AppUtils_SetTransStatus(byte *pTxnRecStatus, byte cTransRecStatus);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_CheckTransStatus
 * DESCRIPTION:     Check Transaction record status
 * RETURN:          BANK_OK: status bit exists
 *                  BANK_FAIL: status bit not exists
 ---------------------------------------------------------------------------- */
int AppUtils_CheckTransStatus(byte cTxnRecStatus, byte cTransRecStatus);

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_OpenPrinter
* DESCRIPTION:   打开打印机
* PARAMETERS:    None
* RETURN:        BANK_OK / BANK_FAIL
* NOTES:
* ------------------------------------------------------------------------ */
int AppUtils_OpenPrinter(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_OpenRS232
 * DESCRIPTION:     Open RS232 Port
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_OpenRS232(char *pCOMName, int iBoudX);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_SendRS232Data
 * DESCRIPTION:     Send RS232 Data
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_SendRS232Data( char bDataMode, char *pData, int iDataLength);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_RecvRS232Data
 * DESCRIPTION:     Receive RS232 Data
 * RETURN:          length of received data
 ---------------------------------------------------------------------------- */
int AppUtils_RecvRS232Data( char bDataMode, char *pRetData, int iNeedRecvDataLength, int iTimeout );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_CloseRS232
 * DESCRIPTION:     Close the COM handle
 * RETURN:          BANK_OK
 ---------------------------------------------------------------------------- */
int AppUtils_CloseRS232( void );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AmountSub
 * DESCRIPTION:     pResult = pSub1 - pSub2
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_AmountSub(byte *pResult, byte *pSub1, byte *pSub2);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AmountSub
 * DESCRIPTION:     pResult = pAdd1 + pAdd2
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_AmountAdd(byte *pResult, byte *pAdd1, byte *pAdd2);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AmountAsc2String
 * DESCRIPTION:     convert amount asc 12byte to string, 000000000001->0.01
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void AppUtils_AmountAsc2String(byte *pAmountAsc, char *pCurrencyName, char bNegativeFlag, char *pRetAmountString);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AmountAsc2Long
 * DESCRIPTION:     convert amount asc 12byte to long data, 000000000001->0.01
 * RETURN:          None
 ---------------------------------------------------------------------------- */
ulint AppUtils_AmountAsc2Long( byte *pAmountAsc );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Warning
 * DESCRIPTION:     display warning message, wait button press
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
int AppUtils_Warning(char *pWarningString);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_FormatWarning
 * DESCRIPTION:     display warning message, wait button press
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
int AppUtils_FormatWarning( char *szFmt, ... );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_Cls
 * DESCRIPTION:     Clear the whole screen
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
void AppUtils_Cls( void );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_UserClear
 * DESCRIPTION:     clear message has been displayed under iLineNo
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
void AppUtils_UserClear(int iLineNo);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DisplayTitle
 * DESCRIPTION:     第一行居中反显
 * RETURN:          
 *                  
 ---------------------------------------------------------------------------- */
void AppUtils_DisplayTitle(char *TitleMsg);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_WaitKey
 * DESCRIPTION:     wait for button pressed until timeout
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
byte AppUtils_WaitKey( int Timeout);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_WaitKeyMill
 * DESCRIPTION:     wait for button pressed until timeout
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
byte AppUtils_WaitKeyMill(ulint Timeout);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_SVCWAIT
 * DESCRIPTION:     Block and wait for ulMSec mill seconds
 * RETURN:          0
 ---------------------------------------------------------------------------- */
int AppUtils_SVCWAIT(unsigned long ulMSec);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetNum
 * DESCRIPTION:     Input data on key broad
 * RETURN:          -1: timeout
 *                  >=0: input data length
 ---------------------------------------------------------------------------- */
int AppUtils_GetNum( int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, int iTimeOut );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_InputNum
 * DESCRIPTION:     Input num by keyboard
 * RETURN:          -1: timeout
 *                  >=0: input data length
 ---------------------------------------------------------------------------- */
int AppUtils_InputNum( int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, int iTimeOut );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetText
 * DESCRIPTION:     Input data on key broad
 * RETURN:          -1: timeout
 *                  >=0: input data length
 ---------------------------------------------------------------------------- */
int AppUtils_GetText(int iStartLine, char *pMsgPrompt, char *pRetInputData, int Min_Len, int Max_Len, byte MaskChr, int iTimeOut);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetAmout
 * DESCRIPTION:     Input amount on key broad
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_GetAmout(int iStartLine, char *pMsgPrompt, byte bNegativeFlag, char *pCurrencyName, byte bZeroAmtAllowFlag, byte *pRetAmount);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtills_JournalCheckFreeSpace
 * DESCRIPTION:     check ram space is enough
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalCheckFreeSpace(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalWriteRecord
 * DESCRIPTION:     append the journal using pTxnRecord
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalWriteRecord( int iForeSaveFlag, DynData *pTxnRecord);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalUpdateRecord
 * DESCRIPTION:     Update the match journal record using pTxnRecord
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalUpdateRecord(ulint iSearchedTrace, DynData *pTxnRecord);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalUpdateRecordByIndex
 * DESCRIPTION:     Update the match journal record using pTxnRecord
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalUpdateRecordByIndex( int iOffset, DynData *pTxnRecord );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalUpdateRecordByStatus
 * DESCRIPTION:     Update the match journal record though update the status flag
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalUpdateRecordByStatus( ulint iSearchedTrace, int iStatusOffset, byte NewStatusFlag );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalDeleteRecord
 * DESCRIPTION:     delete the journal matched
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalDeleteRecord(ulint iSearchedTrace, DynData *pTxnRecord);
/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalClearAll
 * DESCRIPTION:     clear the all journal
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalClearAll(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalGetRecordLast
 * DESCRIPTION:     get the last journal record
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalGetRecordLast(DynData *pRetTxnRecord);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_JournalGetLast
 * DESCRIPTION:     get the last journal record
 * RETURN:          >=0: sucess
 *                  < 0: fail
 ---------------------------------------------------------------------------- */
int AppUtils_JournalGetRecord(ulint iSearchedTrace, DynData *pRetTxnRecord);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_IsColorScreen
 * DESCRIPTION:     Check if terminal is the color screen type
 * RETURN:          BANK_OK: Color screen type as VX680 VX820
                    BANK_FAIL: Mono screen type
 ---------------------------------------------------------------------------- */
int AppUtils_IsColorScreen(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetPassWord
 * DESCRIPTION:     从终端输入密码
 * RETURN:          >=0: 输入成功  <0: 输入失败
 ---------------------------------------------------------------------------- */
int AppUtils_GetPassWord(int disp_line, char * msgPrompt, char *pwd, usint min, usint max, int disp_mode);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetSysDateTime
 * DESCRIPTION:     获取日期时间(含星期缩写，一般供待机界面使用)
 * RETURN:          BANK_OK: 获取成功
                    BANK_FAIL: 获取失败
 ---------------------------------------------------------------------------- */
int AppUtils_GetSysDateTime(char *buffer);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetTermModuleName
 * DESCRIPTION:     获取终端设备型号名称
 * RETURN:          设备型号字符串指针
 ---------------------------------------------------------------------------- */
char *AppUtils_GetTermModuleName(void);

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_GetTextbyTouchScreen
* DESCRIPTION:   输入字符串(字母和数字)
* PARAMETERS:   
*		LineNo    (in)  -- 提示信息显示行数
*		msgPrompt (in)  -- 提示信息
*		str       (out) -- 输出字符串
*		MinLen    (in)  -- 输入数据最小长度
*		MaxLen    (in)  -- 输入数据最大长度
*		DispMode  (in)  -- 输入数据显示模式(参考DISP_FORMAT定义)
* RETURN:
*	    INPUT_OK/INPUT_TIMEOUT/INPUT_USERABORT
* NOTE:
* -------------------------------------------------------------------------*/
int AppUtils_GetTextbyTouchScreen(int LineNo, char *msgPrompt, char *str, usint MinLen, usint MaxLen, int DispMode);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_YesOrNo
 * DESCRIPTION:     参数用的交易开关选择
 * RETURN:          BANK_OK: 输入成功
                    BANK_FAIL: 输入失败
 ---------------------------------------------------------------------------- */
int AppUtils_YesOrNo(char *pMsg, char *pMsg2, int *Value);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_YesOrNoEx
 * DESCRIPTION:     参数用的交易开关选择
 * RETURN:          BANK_OK: 输入成功
                    BANK_FAIL: 输入失败
 ---------------------------------------------------------------------------- */
int AppUtils_YesOrNoEx(char *pMsg, char *pMsg2, int *Value);
	
/* --------------------------------------------------------------------------
 * FUNCTION NAME: AppUtils_GetIPAddress
 * DESCRIPTION:   输入IP地址风格的字符串，可以用#号键代替输入'.'
 *                可以接受的字符串为16字节（含NULL）
 *                在第二行显示Msg中的提示信息，第三行显示和输入IP地址
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int AppUtils_GetIPAddress(char *Msg, char *str);

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
int AppUtils_GetFileString(char *fn, char *Section, char *key, char *value);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_RemoveFile
 * DESCRIPTION:     delete & remove file in RAM
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_RemoveFile( char *pFileName );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetNumberOfRecords
 * DESCRIPTION:     get file total number
 * RETURN:          num_record
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_GetNumberOfRecords( const char *pFileName, int iRecordLength );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_AppendRecord
 * DESCRIPTION:     append record to Trans Database file
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_AppendRecord(const char *pFileName, char *pRecord, int iRecordLength );


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetRecordByIndex
 * DESCRIPTION:     get file total number
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_GetRecordByIndex(const char *pFileName, void *pRecord, int iRecordLength, int iRecordIndex);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DispChinese
 * DESCRIPTION:     定位显示汉字
 * RETURN:
 *
 ---------------------------------------------------------------------------- */
void AppUtils_DispChinese(char *str, int x, int y);

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_DispEnglish
* DESCRIPTION:   在坐标起始位置开始显示一行英文.
* PARAMETERS: 
*		str (in) -- 要显示的英文
*		x   (in) -- 列号, 从1开始
*		y   (in) -- 行号, 从1开始
* RETURN:        
* NOTES:         128X64点阵的黑白屏上，x最大21  y最大8
* ------------------------------------------------------------------------ */
void AppUtils_DispEnglish(char *str, int x, int y);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DisplayLine
 * DESCRIPTION:     Display the message information automatically, if message length > screen width
 *                  it will display on new next line
 * RETURN:          RETURN the next line no. haved been used
 ---------------------------------------------------------------------------- */
int AppUtils_DisplayLine(char *pDisplayBuf, int iStartLine);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DisplayCenter
 * DESCRIPTION:     Display the message information automatically,
 *                  1 lines will display 16 characters
 * RETURN:          RETURN the next line no. haved been used
 ---------------------------------------------------------------------------- */
int AppUtils_DisplayCenter(char *pDisplayBuf, int iStartLine,unsigned  int  bInvertFlag);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_DisplayRight
 * DESCRIPTION:     Display the message information automatically, if message length > screen width
 *                  it will display on new next line
 * RETURN:          RETURN the next line no. haved been used
 ---------------------------------------------------------------------------- */
int AppUtils_DisplayRight(char *pDisplayBuf, int iStartLine);

void AppUtils_SetStatus( byte *bStatusSetted, byte bStatusFlag );

void AppUtils_ResetStatus( byte *bStatusSetted, byte bStatusFlag );
void AppUtils_ResetUsStatus( unsigned short *usStatusSetted, unsigned short usStatusFlag );

int AppUtils_CheckStatus( byte bStatusChecked, byte bStatusFlag );
int AppUtils_CheckUshortStatus( unsigned short usStatusChecked, unsigned short usStatusFlag );

int AppUtils_CheckDateFormatValid( char *pDateString );

int AppUtils_YesNoConfirm( char *pTitle, char *pMsg1, char *pMsg2, char *pPromptMsg );

int AppUtils_CheckTipFloorLimit( byte *pBaseAmt, byte *pTipAmt );


int AppUtils_OperAdd( Bank_Operator *NewOper);

int AppUtils_OperFind( Bank_Operator *FindOper);

int AppUtils_OperDelete( Bank_Operator *DeletedOper );

int AppUtils_OperUpdate( Bank_Operator *UpdatedOper );

void AppUtils_ConvertTransDateTime( DynData *pTxnData, char *pRetDateTimeASC );

byte AppUtils_ConvertAmountNegativeFlag( DynData *pTxnData );

int AppUtils_ConvertPANbyMask( DynData *pTxnData, int iPANseq, char *pRetPAN );

void AppUtils_ErrorBeep(void);

void AppUtils_Window(int startX, int startY, int endX, int endY);

void AppUtils_SetFontSize(int font_size);

void AppUtils_NormalBeep(void);

void AppUtils_SetTimeOut(unsigned int Timeout);

int AppUtils_GetTimeOut(void);

int AppUtils_GetTrack2(byte *CardBuf1, byte *CardBuf2, byte *CardBuf3, byte *PANnum, int manflag);

int AppUtils_DispBMP(char *BmpFile, int x, int y, int width, int height);

int AppUtils_GetTerminalType(void);

void AppUtils_DispStrByColor(char *str, int startX, int startY, int fontSize,unsigned long  RGBFontColor, unsigned long  RGBBGColor);

void AppUtilS_SetTouchButton(int iButtonHeight,int iTextFontSize,unsigned long ulTextRGBColor);

void AppUtils_ShowTouchButton(char *Str, int *x, int *y,int RetVal);

int AppUtils_CheckTouchButton(int *RetVal);

void AppUtils_SetFontFile( const char *szFonetFileName );

void AppUtils_SetFontColor(unsigned long RGBFontColor);

int AppUtils_Restart(const char *filename);

void AppUtils_FlushCard( void );

void AppUtils_FlushKbd( void );

//返回是否675屏幕(屏幕行宽和列宽正好与680/820屏幕相反)
int AppUtils_IsRotatedScreen(void);

//返回是否只支持内置密码键盘
int AppUtils_IsOnlyBuiltInPinpad(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetTransRecByIdx
 * DESCRIPTION:     Get trans database record by index
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppUtils_GetTransRecByIdx( void *pRecord, int iRecordIndex );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_CheckKey
 * DESCRIPTION:     Check key in keyboard buffer
 * RETURN:          0: timeout
 *                  key value: other
 ---------------------------------------------------------------------------- */
byte AppUtils_CheckKey( void );

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
int AppUtils_IntelligentInputIP(int PromptDispLine, char *msgPrompt, int InputDispLine, char *ip, int TimeOutSec);

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_GetAppBuildTimeStamp
* DESCRIPTION:   获取应用构建的时间戳: 用于测试阶段的版本管理和区分
* PARAMETERS:    None
* RETURN:        时间戳字符串指针
* NOTES:         by XC 2014-6-23 15:20:05
* ------------------------------------------------------------------------ */
char *AppUtils_GetAppBuildTimeStamp(void);

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_BatteryVolToValue
* DESCRIPTION:   把电池电压信息转换成电量百分比数据
* PARAMETERS:    BattVol: 电压(mV毫伏)
* RETURN:        电量(0-100)
* NOTES:         
* ------------------------------------------------------------------------ */
int AppUtils_BatteryVolToValue(int BattVol);

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_CheckBattery
* DESCRIPTION:   电池检查：通过虚电判断或电量判断等情况，决定是否允许做交易
* PARAMETERS:    None
* RETURN:        BANK_OK / BANK_FAIL
* NOTES:         
* ------------------------------------------------------------------------ */
int AppUtils_CheckBattery(void);

/* --------------------------------------------------------------------------
* FUNCTION NAME: AppUtils_ClosePrinter
* DESCRIPTION:   关闭打印机
* PARAMETERS:    None
* RETURN:        BANK_OK / BANK_FAIL
* NOTES:
* ------------------------------------------------------------------------ */
int AppUtils_ClosePrinter(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetColumns
 * DESCRIPTION:     Get max colums.
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_GetColumns(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppUtils_GetRows
 * DESCRIPTION:     Get max rows
 * RETURN:          None
 ---------------------------------------------------------------------------- */
int AppUtils_GetRows(void);

int MmiUtil_DisplayRight(char *cDisplayBuffer, int iLine);
byte AppUtils_get_WaitKey(int Timeout);

#endif
