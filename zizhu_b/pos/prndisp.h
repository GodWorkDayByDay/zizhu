
/****************************************************************************
 * FILE NAME:   PRNDISP.C                                                   *
 * MODULE NAME: PRNDISP                                                     *
 * PROGRAMMER:  Laikey                                                      *
 * DESCRIPTION: 显示和打印模块                                              *
 * REVISION:                                                                *
 ****************************************************************************/
 
#ifndef __PRNDISP_H
#define __PRNDISP_H

#define PRINTMODE_STRING        0x01
#define PRINTMODE_HEX           0x02

typedef enum
{
	MENUOPT_1LINE2MENU    = 0x0001,   // 一行两个菜单,否则一行一个菜单
	MENUOPT_NOI_NVERT_TITLE  = 0x0002,   // 标题不反显
	MENUOPT_AUTOCHECKLEN  = 0x0004,   // 检查菜单长度,若第二列的超过长度,在下一行显示,否则一行一个菜单
	MENUOPT_INORDER       = 0x0008,   // 翻屏后菜单顺序排列,否则从1开始
	MENUOPT_UP_DOWN_ARROW = 0x0010,   // 是否显示翻页箭头
	MENUOPT_EXEC_FUNC     = 0x0020,   // 是否执行立刻函数
	MENUOPT_RETURN        = 0x0040,   // 选中菜单执行完函数以后直接退出菜单,否则退回到当前菜单界面
	MENUOPT_ENTER_RETURN  = 0x0080,   // 按确定键退出菜单
} PrnDisp_DisplayMenuOption;

typedef struct
{
	byte ( *EnableFunc )( void );	// =1:显示, =0:不显示
	int  ReturnIndex;               // 选中了以后返回值
	char *pMenuTitle;               // 菜单名称
	int (*Func)(void *Parameter);  	// 选中后执行函数
	void *Parameter;                // 传入参数
} PrnDisp_DisplayMenuItem;


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   PrnDisp_DisplayMenu
 * DESCRIPTION:     display menu
 * RETURN:          >0: sucess, RETURN menu index
 *                  -1: timeout
 *                  -2: use press cancel key button
 ---------------------------------------------------------------------------- */
int PrnDisp_DisplayMenu(int iStartLine, char *pMenuTitle, byte bMenuOption, int iMenuItemMaxCnt, PrnDisp_DisplayMenuItem UserMenuItem[], int iMenuTimeout);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   PrnDisp_DisplayBankCodeError
 * DESCRIPTION:     explain error code means
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void PrnDisp_DisplayBankCodeError(int iErrorCode);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   PrnDisp_DisplayTransName
 * DESCRIPTION:     display the meaning of transcation type
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void PrnDisp_DisplayTransName( int iTransType );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   PrnDisp_DisplayWarning
 * DESCRIPTION:     display warning
 * RETURN: none
 ---------------------------------------------------------------------------- */
int PrnDisp_DisplayWarning( char *pTitle, char *pWarningMsg );

int PrnDisp_DisplayShowLogo(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayRespCode
 * DESCRIPTION:   显示响应码对应的错误信息
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayRespCode( DynData *pTxnData );

void PrnDisp_DisplayBatteryStatus( void );

void PrnDisp_DispSignalStatus( void );

int PrnDisp_DisplayVersion(void *dummy );

int PrnDisp_DisplayTxnDetail( DynData *pTxnData, int iTitleTranType );

int PrnDisp_DisplayTxnTotal( TotalRec *pTotalsData );

int PrnDisp_DisplayECashTxnLogDetail( int iRecordOffset, char *pRecordData, char *pLogFormat, int iLogFormatLen );

void PrnDisp_PrintString( char *pString, byte bAttribute, byte bLineFeedFlag );

void PrnDisp_PrintStringCenterAlign( char *pString, byte bAttribute );

void PrnDisp_PrintStringRightAlign( char *pString, byte bAttribute );

void PrnDisp_PrintStringLeftRightAlign( char *pLeftString, byte bLeftAttrib, char *pRightString, byte bRightAttrib );

void PrnDisp_PrintLineFeed( byte bAttribute );

void PrnDisp_PrintFormFeed( void );

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_Trail
 DESCRIPTION:   打印结束线
 PARAMETERS:    void
 RETURN:        None
 NOTE:
 *-------------------------------------------------------------------------*/
void PrnDisp_Trail(void);

int PrnDisp_RePrintLast(void *dummy );

int PrnDisp_RePrintTxn( void *dummy );

int PrnDisp_PrintJournal(void *dummy );

int PrnDisp_PrintTotal(void *dummy );

int PrnDisp_PrintSettleDetail( void * dummy );
	
int PrnDisp_PrintTxn( DynData *pTxnData, byte bReprintFlag );

void PrnDisp_PrintSettlement( TotalRec *TotalsData, byte bReprintFlag );

void PrnDisp_PrintAIDParameter( void );

void PrnDisp_PrintCAKeyParameter( void );

int PrnDisp_PrintPbocParameter( void *dummy );

void PrnDisp_PrintICDebugData( DynData *pTxnData );

void PrnDisp_PrintTesting( void *dummy );

void PrnDisp_PrintUserData(byte bStringMode, char *pPrintBuf, int iPrintLength);

int PrnDisp_PrintTermCfg( void *dummy );

int PrnDisp_GetBankName(byte *pBankCode, char *pBankName);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintWidthHEXASC
 * DESCRIPTION:   打印hex-asc信息
 * PARAMETERS:    header: title of the data
 *                buf: data to be printed
 *                len: length of the data
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_PrintWidthHEXASC(char *header, unsigned char *buf, int len);

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_SepLine
 DESCRIPTION:   打印分隔线
 PARAMETERS:    void
 RETURN:        None
 NOTE:
 *-------------------------------------------------------------------------*/
void PrnDisp_SepLine(void);

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_UnderLine
 DESCRIPTION:   打印下划线
 PARAMETERS:    void
 RETURN:        None
 NOTE:
 *-------------------------------------------------------------------------*/
void PrnDisp_UnderLine(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayUpArrow
 * DESCRIPTION:   显示上箭头
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayUpArrow(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayDownArrow
 * DESCRIPTION:   显示下箭头
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayDownArrow(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_RePrintSettlement
 * DESCRIPTION:   重打印结算单
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_RePrintSettlement(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayAdInfo
 * DESCRIPTION:   显示广告信息，每次调用都加一个汉字
 *                四行机型把时间接到广告信息后，
 *                大屏机型与时间不显示在同一行，不需要拼接时间
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayAdInfo(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_CheckAdInfo
 * DESCRIPTION:   检测是否存在广告信息
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_CheckAdInfo(void);

int PrnDisp_InitShowTimeCount(int TimeSec);

void PrnDisp_CommShowTimeCount(void);

#endif
