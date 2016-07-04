/***************************************************************************
* FILE NAME:    TRANS.c                                                    *
* MODULE NAME:  TRANS                                                      *
* PROGRAMMER:   Laikey                                                     *
* DESCRIPTION:  传统交易业务模块                                           *
* REVISION:                                                                *
****************************************************************************/
#include "cupms.h"
#include "detaildialog.h"
#include "pos/Proc8583.h"
#include "cardreader.h"
#include "zzkeypad.h"
#include "singleton.h"
#include "sd_des.h"
// ------------------------------------------------------------------------
//
//  Extern buffer define
//
// ------------------------------------------------------------------------
extern Term_Config gTermCfg;
extern int EMVtrans_RemoveCard();

// ------------------------------------------------------------------------
//
//  Private buffer define
//
// ------------------------------------------------------------------------
#define EMV_ICC_HEADER_MAX  30

struct EMV_AIDHeader
{
	byte Length;
	byte AID[ 17 ];
};

struct EMV_CAKeyHeader
{
	byte RID[ 5 ];
	byte Index;
};


static int inTrans_iTmpHeaderNum = 0;
static struct EMV_AIDHeader inTrans_stTmpAIDHeader[ EMV_ICC_HEADER_MAX ];
static struct EMV_CAKeyHeader inTrans_stTmpCAKeyHeader[ EMV_ICC_HEADER_MAX ];
int Trans_SaleSetField64_MAC(ISO_data *pReqIsoRec);
static int inTrans_Sale_CheckPacket(DynData *pTxnData, ISO_data *pRspIsoRec);
static int inTrans_SaleGetField64_CheckMac(ISO_data *pRspIsoRec);
//本应用根据银联直连终端规范指定的报文格式
const ISO8583_FieldFormat ISO8583_stCUPFieldFormat[] =
{
	{ISO8583TYPE_BCD,                        4},     //  1 MSG ID
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      19},    //  2 PAN
	{ISO8583TYPE_BCD,                        6},     //  3 Processing Code
	{ISO8583TYPE_BCD,                        12},    //  4 Amount
	{ISO8583TYPE_BCD,                        12},    //  5
	{ISO8583TYPE_BCD,                        12},    //  6
	{ISO8583TYPE_BCD,                        10},    //  7
	{ISO8583TYPE_ASC,                        1},     //  8
	{ISO8583TYPE_BCD,                        8},     //  9
	{ISO8583TYPE_BCD,                        8},     // 10
	{ISO8583TYPE_BCD,                        6},     // 11 System trace
	{ISO8583TYPE_BCD,                        6},     // 12 Time
	{ISO8583TYPE_BCD,                        4},     // 13 Date
	{ISO8583TYPE_BCD,                        4},     // 14 ExpDate
	{ISO8583TYPE_BCD,                        4},     // 15 Settlement date
	{ISO8583TYPE_ASC,                        1},     // 16
	{ISO8583TYPE_BCD,                        4},     // 17
	{ISO8583TYPE_BCD,                        5},     // 18
	{ISO8583TYPE_BCD,                        3},     // 19
	{ISO8583TYPE_BCD,                        3},     // 20
	{ISO8583TYPE_ASC,                        7},     // 21
	{ISO8583TYPE_BCD,                        3},     // 22 POS entry mode
	{ISO8583TYPE_BCD,                        3},     // 23 IC Application PAN
	{ISO8583TYPE_ASC,                        2},     // 24 NII
	{ISO8583TYPE_BCD,                        2},     // 25
	{ISO8583TYPE_BCD,                        2},     // 26
	{ISO8583TYPE_BCD,                        1},     // 27
	{ISO8583TYPE_BCD,                        8},     // 28
	{ISO8583TYPE_BCD,                        8},     // 29
	{ISO8583TYPE_BCD,                        8},     // 30
	{ISO8583TYPE_BCD,                        8},     // 31
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      11},    // 32
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      11},    // 33
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      28},    // 34
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      37},    // 35 Track2
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      104},   // 36 Track3
	{ISO8583TYPE_ASC,                        12},    // 37 System Reference No
	{ISO8583TYPE_ASC,                        6},     // 38 System AuthID
	{ISO8583TYPE_ASC,                        2},     // 39 Response Code
	{ISO8583TYPE_ASC,                        3},     // 40
	{ISO8583TYPE_ASC,                        8},     // 41 TID
	{ISO8583TYPE_ASC,                        15},    // 42 CustomID
	{ISO8583TYPE_ASC,                        40},    // 43 Custom Name
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      25},    // 44
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      76},    // 45 Track1
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 46
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 47
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      999},   // 48
	{ISO8583TYPE_ASC,                        3},     // 49 Currency Code  Transaction
	{ISO8583TYPE_ASC,                        3},     // 50
	{ISO8583TYPE_ASC,                        3},     // 51
	{ISO8583TYPE_BIN,                        64},    // 52 PIN block Data
	{ISO8583TYPE_BCD,                        16},    // 53 Security Data
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      320},   // 54
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 55 ICC information
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 56
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 57
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 58
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 59
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      999},   // 60 Additional Data
	{ISO8583TYPE_BCD | ISO8583TYPE_VAR,      999},   // 61 Additional Data
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 62 Additional Data
	{ISO8583TYPE_ASC | ISO8583TYPE_VAR,      999},   // 63 Additional Data
	{ISO8583TYPE_BIN,                        64},    // 64 MAC data
};

// ------------------------------------------------------------------------
//
//  Private function declare
//
// ------------------------------------------------------------------------

static byte inTrans_IsSmallAmountAuth_Support(void);

static int inTrans_GetField64_CheckMac(ISO8583_Rec *pRspIsoRec);
static int inTrans_UpdateOrignalJournalAdjust(byte bAdjustJournalFlag, DynData *pTxnData);
static int inTrans_UploadOfflineBeforeSettlement(byte bOrignalTransType);
static int inTrans_UploadAfterSettlement(DynData *pTxnRec);
static int inTrans_UploadOffline(byte bUploadFlag, unsigned short usSwipMode);
static int inTrans_UploadOnline(DynData *pTxnReca);
static int inTrans_UploadNotification(DynData *pTxnRec);
static int inTrans_UploadTC(int iARPCflag, DynData *pTxnRec);
static int inTrans_UploadEnd(DynData *pTxnRec);
static int inTrans_VoidOnline(ulint lSearchTraceNum, int iHandFlag);
static int inTrans_CheckPacketValid(DynData *pTxnData, ISO8583_Rec *pRspIsoRec);
static int inTrans_CheckPacket(DynData *pTxnData, ISO_data *pRspIsoRec);
static void inTrans_AIDHeaderReset(void);
static int inTrans_AIDHeaderGetAID(int nIndex, char *pRetRID);
static int inTrans_AIDHeaderSave(char *pAIDHeader, int iAIDHeaderLength);
static int inTrans_AIDSave2File(char *pAIDData, int iAIDDataLength);
static void inTrans_CAKeyHeaderReset(void);
static int inTrans_CAKeyHeaderGetRID(int nIndex, char *pRetRID);
static int inTrans_CAKeyHeaderSave(char *pCAKeyHeader, int iCAKeyHeaderLength);
static int inTrans_CAKeySave2File(char *pCAKeyData, int iCAKeyDataLength);
static int inTrans_SaveBlackList(int iBlackListNum, byte *pBlackListData, int iDataLength);
static int inTrans_SaveTotalData(TotalRec *pstTotalsData);

static int inTrans_LoadConfig(byte *bField62Data, int iField62Length);
static byte inTrans_CheckTxnSupport(int iTxnType);
static int inTrans_GetCUPLicense(char *pcCUPLicense, int iSizeofLicense);
static int inTrans_CheckPrintFailOfflineList(void);
static int inTrans_ResolveHolderName(char *HolderName, char *TrackData);

static int Trans_SaleSetField53_Data(ISO_data *pReqIsoRec, DynData *pstTransData);
// ------------------------------------------------------------------------
//
//  Public function declare
//
// ------------------------------------------------------------------------
int EMVtrans_GetEMVIccTCUpload(DynData *TransDat, char *data)
{return 0;}
int EMVtrans_ReadTagData(int dat, char *data, int num)
{
    return 0;

}
int BankUtil_CStrlcpy(char *da,char *dag,int f )
{
    return 0;
}
int EMVtrans_ConfirmProcessing(DynData *TransData)
{return BANK_EMV_BYPASS;}
int EMVtrans_ConfirmProcessing_OnlyAuthOfflineData(DynData *TransD)
{
    return 0;
}
int EMVtrans_GetEcashBalance(DynData *TransData)
{

        return 0;
}
int EMVtrans_GetEMVICCData(DynData *TransData,unsigned char * cTmpBuf)
{
    return 0;
}
/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_MainMenu
 * DESCRIPTION:   主交易菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_MainMenu(void)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 32 ];

    return 0;
	PrnDisp_DisplayMenuItem UserMainMenuItem[] =
	{
		{ NULL, 1, "消  费",    Trans_Sale, (void *) 0 },
		{ NULL, 2, "撤  销",    Trans_Void, (void *) 0 },
		{ NULL, 3, "退  货",    Trans_Refund, (void *) 0 },
		{ NULL, 4, "预授权",    Trans_AuthMenu, (void *) 0 },
		{ NULL, 5, "离  线",    Trans_OfflineMenu, (void *) 0 },
		{ NULL, 6, "打  印",    Trans_PrintMenu, (void *) 0 },
		{ NULL, 7, "管  理",    Trans_ManageMenu, (void *) 0 },
		{ NULL, 8, "其  他",    Trans_OtherMenu, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "交易菜单");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_AUTOCHECKLEN | MENUOPT_1LINE2MENU;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(UserMainMenuItem) / sizeof(PrnDisp_DisplayMenuItem), UserMainMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_AuthMenu
 * DESCRIPTION:   预授权菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_AuthMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem PreAuthMenuItem[] =
	{
		{ NULL, 1, "预授权",            Trans_PreAuth, (void *) 0 },
		{ NULL, 2, "预授权完成请求",    Trans_PreAuthCompleteRequest, (void *) 0 },
		{ NULL, 3, "预授权完成通知",    Trans_PreAuthCompleteNote, (void *) 0 },
		{ NULL, 4, "预授权撤销",        Trans_PreAuthVoid, (void *) 0 },
		{ NULL, 5, "预授权完成撤销",    Trans_PreAuthCompleteVoid, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	sprintf(cTitleBuffer, "%s", "预授权类交易");
	
	bMenuOption = MENUOPT_INORDER | MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(PreAuthMenuItem) / sizeof(PrnDisp_DisplayMenuItem), PreAuthMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_OfflineMenu
 * DESCRIPTION:   离线交易菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_OfflineMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem OfflineMenuItem[] =
	{
		{ NULL, 1, "离线结算",  Trans_OfflineSettlement, (void *) 0 },
		{ NULL, 2, "结算调整",  Trans_AdjustSettlement, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	sprintf(cTitleBuffer, "%s", "离线");
	
	bMenuOption = MENUOPT_EXEC_FUNC;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(OfflineMenuItem) / sizeof(PrnDisp_DisplayMenuItem), OfflineMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PrintMenu
 * DESCRIPTION:   打印菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PrintMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem PrintMenuItem[] =
	{
		{ NULL,    1, "重打最后一笔",  PrnDisp_RePrintLast, (void *) 0 },
		{ NULL,    2, "重打任意一笔",  PrnDisp_RePrintTxn, (void *) 0 },
		{ NULL,    3, "打印交易明细",  PrnDisp_PrintJournal, (void *) 4 },
		{ NULL,    4, "打印交易汇总",  PrnDisp_PrintTotal, (void *) 0 },
		{ NULL,    5, "重打印结算单",  PrnDisp_RePrintSettlement, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	sprintf(cTitleBuffer, "%s", "打印");
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(PrintMenuItem) / sizeof(PrnDisp_DisplayMenuItem), PrintMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ManageMenu
 * DESCRIPTION:   管理菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ManageMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem ManageMenuItem[] =
	{
		{ NULL,    1, "签到",      Trans_LogonMenu, (void *) 0 },
		{ NULL,    2, "签退",      Trans_Logout, (void *) 0 },
		{ NULL,    3, "交易查询",  Trans_InquireMenu, (void *) 0 },
		{ NULL,    4, "柜员",      PrmMngr_OperManageMenu, (void *) 0 },
		{ NULL,    5, "外线号码",  PrmMngr_SetPABX, (void *) 0 },
		{ NULL,    6, "结算",      Trans_Settlement, (void *) 0 },
		{ NULL,    7, "锁定终端",  Trans_LockTerminal, (void *) 0 },
		{ NULL,    8, "版本",      PrnDisp_DisplayVersion, (void *) 0 },
		{ NULL,    9, "通讯类型",  PrmMngr_SetCommMode, (void *) 1 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "管理");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_AUTOCHECKLEN | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(ManageMenuItem) / sizeof(PrnDisp_DisplayMenuItem), ManageMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_LogonMenu
 * DESCRIPTION:   签到菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_LogonMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem LogonMenuItem[] =
	{
		{ NULL,    1, "POS签到",        Trans_Logon, (void *) 0 },
		{ NULL,    2, "操作员签到",     Trans_LogonOper, (void *) 2 },
		{ NULL,    3, "收银员积分签到", Trans_LogonScore, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	sprintf(cTitleBuffer, "%s", "签到");
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(LogonMenuItem) / sizeof(PrnDisp_DisplayMenuItem), LogonMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_InquireMenu
 * DESCRIPTION:   交易查询菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_InquireMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem InquireMenuItem[] =
	{
		{ NULL,    1, "查询交易明细",  Trans_InquireJournal, (void *) 0 },
		{ NULL,    2, "查询交易汇总",  Trans_InquireTotal, (void *) 0 },
		{ NULL,    3, "按凭证号查询",  Trans_InquireByTrace, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	sprintf(cTitleBuffer, "%s", "交易查询");
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(InquireMenuItem) / sizeof(PrnDisp_DisplayMenuItem), InquireMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_DownloadFuncMenu
 * DESCRIPTION:   下载功能菜单(系统管理->其他功能设置->下载功能)
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_DownloadFuncMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem OtherMenuItem[] =
	{
		{ NULL, 1, "回响测试",      Trans_EchoTesting, (void *) 0 },
    //	{ NULL, 2, "参数传递",      ////Trans_ParametersDownload, (void *) 0 },
		{ NULL, 3, "IC卡公钥下载",  Trans_CAKeyDownload, (void *) 1 },
		{ NULL, 4, "IC卡参数下载",  Trans_AIDDownload, (void *) 1 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "下载功能菜单");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(OtherMenuItem) / sizeof(PrnDisp_DisplayMenuItem), OtherMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_OtherMenu
 * DESCRIPTION:   其他交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_OtherMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem OtherMenuItem[] =
	{
		{ NULL, 1, "电子现金",   Trans_ECashMenu, (void *) 0 },
		//{ NULL, 2, "钱包",       Trans_EPurseMenu, (void *) 0 },
#ifdef  MAKE_ADDTRANS
		{ NULL, 3, "分期",       Trans_Add_IntallmentMenu, (void *) 0 },
#endif
		{ NULL, 4, "余额查询",   Trans_BalanceInquire, (void *) 0 },
#ifdef  MAKE_ADDTRANS
		{ NULL, 5, "积分",       Trans_Add_ScoreMenu, (void *) 0 },
		//{ NULL, 6, "手机芯片",   Trans_Add_MobileChipMenu, (void *) 0 },
		{ NULL, 7, "预约消费",   Trans_Add_YuyueMenu, (void *) 0 },
		{ NULL, 8, "订购",       Trans_Add_OrderMenu, (void *) 0 },
		{ NULL, 9, "磁条卡充值", Trans_Add_MagLoadMenu, (void *) 0 },
#endif
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
#ifdef  MAKE_ADDTRANS
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "其他");
		
#endif
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(OtherMenuItem) / sizeof(PrnDisp_DisplayMenuItem), OtherMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_EPurseMenu
 * DESCRIPTION:   电子现金交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_EPurseMenu(void *dummy)
{
	return 0;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashMenu
 * DESCRIPTION:   电子现金交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem ECashMenuItem[] =
	{
		{ NULL, 1, "快速支付",  Trans_ECashQPay, (void *) 0 },
		{ NULL, 2, "普通支付",  Trans_ECashPay, (void *) 0 },
		{ NULL, 3, "圈存",      Trans_ECLoadMenu, (void *) 0 },
		{ NULL, 4, "余额查询",  Trans_ECashBalance, (void *) 0 },
		{ NULL, 5, "明细查询",  Trans_PBOCQueryTransDetail, (void *) 0 },
		{ NULL, 6, "圈存日志查询",  Trans_PBOCReadReloadTransLog, (void *) 0 },
		{ NULL, 7, "脱机退货",  Trans_Refund, (void *)1 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "电子现金");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(ECashMenuItem) / sizeof(PrnDisp_DisplayMenuItem), ECashMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECLoadMenu
 * DESCRIPTION:   电子现金圈存交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECLoadMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem LoadMenuItem[] =
	{
		{ NULL,    1, "现金充值",       Trans_ECashLoad, (void *) 0 },
		{ NULL,    2, "指定账户圈存",   Trans_ECashInLoad, (void *) 0 },
		{ NULL,    3, "非指定账户圈存", Trans_ECashOutLoad, (void *) 0 },
		{ NULL,    4, "现金充值撤销",   Trans_ECashVoidLoad, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "圈存");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(LoadMenuItem) / sizeof(PrnDisp_DisplayMenuItem), LoadMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PbocHandingMenu
 * DESCRIPTION:   PBOC维护操作菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PbocHandingMenu(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem LoadMenuItem[] =
	{
		{ NULL,    1, "打印参数",       PrnDisp_PrintPbocParameter, (void *) 1 },
		{ NULL,    2, "打印公钥",       PrnDisp_PrintPbocParameter, (void *) 2 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	sprintf(cTitleBuffer, "%s", "EMV/PBOC管理菜单");
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(LoadMenuItem) / sizeof(PrnDisp_DisplayMenuItem), LoadMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_QuickMenu
 * DESCRIPTION:   待机界面按2进入的快速方式
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_QuickMenu(void *dummy)
{
	unsigned char bMenuOption;
	PrnDisp_DisplayMenuItem QuickMenuItem[] =
	{
		{ NULL, 1, "消费",   Trans_Sale, (void *) 0 },
		{ NULL, 2, "圈存",   Trans_ECLoadMenu, (void *) 0 },
	};
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, "快捷菜单", bMenuOption, sizeof(QuickMenuItem) / sizeof(PrnDisp_DisplayMenuItem), QuickMenuItem, 60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_GetTransName
 * DESCRIPTION: 获取交易类型对应的中文名称
 * PARAMETERS:
 *  iTransType:交易类型
 *  pcRetTransName:中文名称
 * RETURN:
 *  BANK_OK:成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_GetTransName(int iTransType, char *pcRetTransName)
{
	switch(iTransType)
	{
	case BTYPE_LOGON:
		sprintf(pcRetTransName, "%s",  "签到");
		break;
	case BTYPE_LOGOUT:
		sprintf(pcRetTransName, "%s",  "签退");
		break;
	case BTYPE_AID_INQUERY:
		sprintf(pcRetTransName, "%s",  "EMV参数下载请求");
		break;
	case BTYPE_AID_UPDATE:
		sprintf(pcRetTransName, "%s",  "EMV参数下载");
		break;
	case BTYPE_AID_END:
		sprintf(pcRetTransName, "%s",  "EMV参数下载结束");
		break;
	case BTYPE_CAKEY_INQUERY:
		sprintf(pcRetTransName, "%s",  "EMV公钥下载请求");
		break;
	case BTYPE_CAKEY_UPDATE:
		sprintf(pcRetTransName, "%s",  "EMV公钥下载");
		break;
	case BTYPE_CAKEY_END:
		sprintf(pcRetTransName, "%s",  "EMV公钥下载结束");
		break;
	case BTYPE_SALE:
		sprintf(pcRetTransName, "%s",  "消费");
		break;
	case BTYPE_LOGON_CASHIER:
		sprintf(pcRetTransName, "%s",  "收银员积分签到");
		break;
	case BTYPE_SETTLE:
		sprintf(pcRetTransName, "%s",  "结算");
		break;
	case BTYPE_SETTLE_END:
		sprintf(pcRetTransName, "%s",  "批上送结束");
		break;
	case BTYPE_BATCH_UPLOAD:
		sprintf(pcRetTransName, "%s",  "批上送");
		break;
	case BTYPE_SCRIPTADVICE:
		sprintf(pcRetTransName, "%s",  "脚本结果通知");
		break;
	case BTYPE_ICOFFLINE_UPLOAD:
		sprintf(pcRetTransName, "%s",  "IC卡脱机交易上送");
		break;
	case BTYPE_TC_UPLOAD:
		sprintf(pcRetTransName, "%s",  "IC卡TC上送");
		break;
	case BTYPE_DOWNLOAD_PARAMETERS:
		sprintf(pcRetTransName, "%s",  "参数下载");
		break;
	case BTYPE_UPLOAD_STATUS:
		sprintf(pcRetTransName, "%s",  "状态上送");
		break;
	case BTYPE_ECHO_TESTING:
		sprintf(pcRetTransName, "%s",  "回响测试");
		break;
	case BTYPE_DOWNLOAD_END:
		sprintf(pcRetTransName, "%s",  "参数下载结束");
		break;
	case BTYPE_BALANCE:
		sprintf(pcRetTransName, "%s",  "余额查询");
		break;
	case BTYPE_SALE_VOID:
		sprintf(pcRetTransName, "%s",  "消费撤销");
		break;
	case BTYPE_REFUND:
		sprintf(pcRetTransName, "%s",  "退货");
		break;
	case BTYPE_PREAUTH:
		sprintf(pcRetTransName, "%s",  "预授权");
		break;
	case BTYPE_PREAUTH_VOID:
		sprintf(pcRetTransName, "%s",  "预授权撤销");
		break;
	case BTYPE_COMPLETE:
		sprintf(pcRetTransName, "%s",  "预授权完成请求");
		break;
	case BTYPE_COMPLETE_VOID:
		sprintf(pcRetTransName, "%s",  "预授权完成撤销");
		break;
	case BTYPE_OFFLINE_COMPLETE:
		sprintf(pcRetTransName, "%s",  "预授权完成通知");
		break;
	case BTYPE_OFFLINE_SETTLE:
		sprintf(pcRetTransName, "%s",  "离线结算");
		break;
	case BTYPE_OFFLINE_ADJUST:
		sprintf(pcRetTransName, "%s",  "结算调整");
		break;
	case BTYPE_ECASH_CASH_RELOAD:
		sprintf(pcRetTransName, "%s",  "现金充值");
		break;
	case BTYPE_ECASH_CASH_RELOAD_VOID:
		sprintf(pcRetTransName, "%s",  "现金充值撤销");
		break;
	case BTYPE_ECASH_IN_RELOAD:
		sprintf(pcRetTransName, "%s",  "指定账户圈存");
		break;
	case BTYPE_ECASH_OUT_RELOAD:
		sprintf(pcRetTransName, "%s",  "非指定账户圈存");
		break;
	case BTYPE_ECASH_REFUND:
		sprintf(pcRetTransName, "%s",  "电子现金脱机退货");
		break;
	case BTYPE_MAG_ACCT_VERIFY:
		sprintf(pcRetTransName, "%s",  "磁条现金充值验证");
		break;
	case BTYPE_MAG_CASHLOAD:
		sprintf(pcRetTransName, "%s",  "磁条卡现金充值");
		break;
	case BTYPE_MAG_CASHLOAD_CONFIRM:
		sprintf(pcRetTransName, "%s",  "磁条现金充值确认");
		break;
	case BTYPE_MAG_ACCTLOAD:
		sprintf(pcRetTransName, "%s",  "磁条卡账户充值");
		break;
	case BTYPE_INSTALLMENT:
		sprintf(pcRetTransName, "%s",  "分期付款");
		break;
	case BTYPE_INSTALLMENT_VOID:
		sprintf(pcRetTransName, "%s",  "分期付款撤销");
		break;
	case BTYPE_YUYUE_SALE:
		sprintf(pcRetTransName, "%s",  "预约消费");
		break;
	case BTYPE_YUYUE_SALE_VOID:
		sprintf(pcRetTransName, "%s",  "预约消费撤销");
		break;
	case BTYPE_ISSUER_SCORE_SALE:
		sprintf(pcRetTransName, "%s",  "发卡行积分消费");
		break;
	case BTYPE_ISSUER_SCORE_SALE_VOID:
		sprintf(pcRetTransName, "%s",  "发卡行积分撤销");
		break;
	case BTYPE_UNION_SCORE_SALE:
		sprintf(pcRetTransName, "%s",  "联盟积分消费");
		break;
	case BTYPE_UNION_SCORE_SALE_VOID:
		sprintf(pcRetTransName, "%s",  "联盟积分撤销");
		break;
	case BTYPE_UNION_SCORE_BALANCE:
		sprintf(pcRetTransName, "%s",  "联盟积分查询");
		break;
	case BTYPE_UNION_SCORE_REFUND:
		sprintf(pcRetTransName, "%s",  "联盟积分退货");
		break;
	case BTYPE_MCHIP_BALANCE:
		sprintf(pcRetTransName, "%s",  "手机芯片余额查询");
		break;
	case BTYPE_MCHIP_SALE:
		sprintf(pcRetTransName, "%s",  "手机芯片消费");
		break;
	case BTYPE_MCHIP_SALE_VOID:
		sprintf(pcRetTransName, "%s",  "手机芯片消费撤销");
		break;
	case BTYPE_MCHIP_REFUND:
		sprintf(pcRetTransName, "%s",  "手机芯片退货");
		break;
	case BTYPE_MCHIP_PREAUTH:
		sprintf(pcRetTransName, "%s",  "手机芯片预授权");
		break;
	case BTYPE_MCHIP_PREAUTH_VOID:
		sprintf(pcRetTransName, "%s",  "手机芯片授权撤销");
		break;
	case BTYPE_MCHIP_COMPLETE:
		sprintf(pcRetTransName, "%s",  "手机芯片完成请求");
		break;
	case BTYPE_MCHIP_COMPLETE_VOID:
		sprintf(pcRetTransName, "%s",  "手机芯片完成撤销");
		break;
	case BTYPE_MCHIP_OFFLINE_COMPLETE:
		sprintf(pcRetTransName, "%s",  "手机芯片完成通知");
		break;
	case BTYPE_ORDER_SALE:
		sprintf(pcRetTransName, "%s",  "订购消费");
		break;
	case BTYPE_ORDER_SALE_VOID:
		sprintf(pcRetTransName, "%s",  "订购消费撤销");
		break;
	case BTYPE_ORDER_REFUND:
		sprintf(pcRetTransName, "%s",  "订购退货");
		break;
	case BTYPE_ORDER_PREAUTH:
		sprintf(pcRetTransName, "%s",  "订购预授权");
		break;
	case BTYPE_ORDER_PREAUTH_VOID:
		sprintf(pcRetTransName, "%s",  "订购预授权撤销");
		break;
	case BTYPE_ORDER_COMPLETE:
		sprintf(pcRetTransName, "%s",  "订购完成请求");
		break;
	case BTYPE_ORDER_COMPLETE_VOID:
		sprintf(pcRetTransName, "%s",  "订购完成撤销");
		break;
	case BTYPE_ORDER_OFFLINE_COMPLETE:
		sprintf(pcRetTransName, "%s",  "订购完成通知");
		break;
	case BTYPE_ORDER_CARDHOLDER_VERIFY:
		sprintf(pcRetTransName, "%s",  "持卡人信息验证");
		break;
	case BTYPE_TMS_UPDATE_PARAMETER:
		sprintf(pcRetTransName, "%s",  "更新TMS");
		break;
	case BTYPE_TC_UPLOAD_END:
		sprintf(pcRetTransName, "%s",  "IC卡TC上送完成");
		break;
	case BTYPE_REVERSAL:
		sprintf(pcRetTransName, "%s",  "冲正");
		break;
	case BTYPE_BLACK_UPDATE:
		sprintf(pcRetTransName, "%s",  "黑名单下载");
		break;
	case BTYPE_BLACK_END:
		sprintf(pcRetTransName, "%s",  "黑名单下载结束");
		break;
	case BTYPE_ECASH_BALANCE:
		sprintf(pcRetTransName, "%s",  "电子现金查询余额");
		break;
	default:
		sprintf(pcRetTransName, "%s",  "未知交易");
		break;
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_GetShortTransName
 * DESCRIPTION:   获取交易类型对应的短中文名称, 以便在某些显示的情况下使用
 * PARAMETERS:
 *  iTransType:交易类型
 *  pcRetTransName:中文名称
 * RETURN:
 *  BANK_OK:成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_GetShortTransName(int iTransType, char *pcRetTransName)
{
	int iRet = BANK_OK;
	
	switch(iTransType)
	{
	case BTYPE_SALE:
		sprintf(pcRetTransName, "%s",  "消费");
		break;
	case BTYPE_COMPLETE:
		sprintf(pcRetTransName, "%s",  "完成请求");
		break;
	case BTYPE_OFFLINE_COMPLETE:
		sprintf(pcRetTransName, "%s",  "完成通知");
		break;
	case BTYPE_OFFLINE_SETTLE:
		sprintf(pcRetTransName, "%s",  "离线结算");
		break;
	case BTYPE_OFFLINE_ADJUST:
		sprintf(pcRetTransName, "%s",  "结算调整");
		break;
	case BTYPE_ECASH_CASH_RELOAD:
		sprintf(pcRetTransName, "%s",  "现金充值");
		break;
	case BTYPE_INSTALLMENT:
		sprintf(pcRetTransName, "%s",  "分期付款");
		break;
	case BTYPE_YUYUE_SALE:
		sprintf(pcRetTransName, "%s",  "预约消费");
		break;
	case BTYPE_ISSUER_SCORE_SALE:
		sprintf(pcRetTransName, "%s",  "发卡消费");
		break;
	case BTYPE_UNION_SCORE_SALE:
		sprintf(pcRetTransName, "%s",  "联盟消费");
		break;
	case BTYPE_MCHIP_SALE:
		sprintf(pcRetTransName, "%s",  "芯片消费");
		break;
	case BTYPE_MCHIP_COMPLETE:
		sprintf(pcRetTransName, "%s",  "芯片完成");
		break;
	case BTYPE_ORDER_SALE:
		sprintf(pcRetTransName, "%s",  "订购消费");
		break;
	case BTYPE_ORDER_COMPLETE:
		sprintf(pcRetTransName, "%s",  "订购完成");
		break;
	default:
		iRet = BANK_FAIL;
		break;
	}
	
	return iRet;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Logon
 * DESCRIPTION:   签到交易
 * PARAMETERS:    dummy: (void*)0-手工发起的签到; (void*)1-处理要求发起的签到
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Logon(void *dummy)
{
	DynData TransData;
	int iLength;
	byte cDate[8+1];
	byte cTime[8+1];
	byte cTmpBuf[ 512 ];
	char SerNo[12] = {0};
	char cCUPLicense[8] = {0};
	byte cBuffer[ ISO8583_MAXLENTH ];
    ISO_data ReqIsoRec, RspIsoRec;
    int i;

	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = BTYPE_LOGON;
	
	// 显示交易类型
    AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	// 检测操作员是否已经登录
    if(dummy == (void *) 0)
		ASSERT_FAIL(Trans_LogonOper((void *)1));

	// 再次显示交易类型
	AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

	// 增加流水号
    PrmMngr_AddSystrace();

    // 打包
    ClearBit(&ReqIsoRec);
    ClearBit(&RspIsoRec);
    //ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0800", 4);
    SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 11
    TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
    sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
    qDebug("index:%s", cTmpBuf);
    SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

    // field 41
    SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);
    qDebug("Terminal:%s", gTermCfg.mFixPrm.TerminalID);
    // field 42
    SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);
    qDebug("MerchantID:%s", gTermCfg.mFixPrm.MerchantID);
    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    // 60.1
    memcpy(cTmpBuf, "00", 2);
    memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

    // 60.2
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    // 60.3
    if(gTermCfg.mFixPrm.PP_DesType == PP222_DES_MODE)
        memcpy(cTmpBuf + 8, "001", 3);
    else if(gTermCfg.mTrSwitch.TrackEncryptFlag)
        memcpy(cTmpBuf + 8, "004", 3);
    else
        memcpy(cTmpBuf + 8, "003", 3);

    SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *)cTmpBuf));
    qDebug("60:%s", cTmpBuf);
    // field 62
    memset(cCUPLicense, 0, sizeof(cCUPLicense));

    if(inTrans_GetCUPLicense(cCUPLicense, sizeof(cCUPLicense)) != BANK_OK)
    {
        switch(AppUtils_GetTerminalType())
        {
        case _VX510:
            strcpy(cCUPLicense, "3043");
            break;

        case _VX520:
            strcpy(cCUPLicense, "3134");
            break;

        case 520:   //For C520
            strcpy(cCUPLicense, "3225");
            break;

        case _VX520S:
            strcpy(cCUPLicense, "3138");
            break;

        case _VX610:
            strcpy(cCUPLicense, "3064");
            break;

        case _VX670:
            strcpy(cCUPLicense, "3122");
            break;

        case _VX680:
        case _VX680C:
            strcpy(cCUPLicense, "3135");
            break;

        case _VX675:
            strcpy(cCUPLicense, "3217");
            break;

        default: //Set default as _VX510
            strcpy(cCUPLicense, "3043");
            break;
        }
    }

    AppUtils_GetSerialNo(SerNo);

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    strcpy(cCUPLicense, "82609730");
    strcpy(SerNo, "826-097-30");
    sprintf((char *)cTmpBuf, "Sequence No%02d%s%s", strlen(cCUPLicense) + strlen(SerNo), cCUPLicense, SerNo);
    SetBit(&ReqIsoRec, 62, cTmpBuf, strlen((char *) cTmpBuf));
    qDebug("Sequence:%s", cTmpBuf);
    // field 63
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    strcpy((char *)gTermCfg.mDynPrm.OperID, "01");
    memcpy(cTmpBuf, gTermCfg.mDynPrm.OperID, 2);
    cTmpBuf[ 2 ] = ' ';
    SetBit(&ReqIsoRec, 63, cTmpBuf, 3);
    qDebug("oper:%s", cTmpBuf);

    // 组包转换
    memset(cBuffer, 0x00, sizeof(cBuffer));
    // TPDU
    memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);
    // CUP Header
    memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

    iLength = IsoToStr(cBuffer + 5 + 6, &ReqIsoRec);

    if(iLength <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }


    iLength += 11;
    qDebug("send iLength:%d",iLength);
	// 检查网络并拨号
	ASSERT_FAIL(Comm_CheckConnect());

	// 发送数据包
	ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

	// 接收数据包
	memset(cBuffer, 0, sizeof(cBuffer));
	iLength = 0;

	ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));
	
	// 挂线
	Comm_Disconnect();
    qDebug("recv iLength:%d",iLength);
	// 解包

    ASSERT_HANGUP_FAIL(Trans_UnPackData(&TransData, cBuffer, &RspIsoRec));

	// 显示处理错误代码

	ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));
    AppUtils_Cls();
	AppUtils_DisplayCenter("密钥检验中", LINE2, FALSE);
	AppUtils_DisplayCenter("请稍候...", LINE3, FALSE);
	
	//密钥校验
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = GetBit(&RspIsoRec, 62, cTmpBuf);
	if(iLength <= 0)
	{
		AppUtils_Warning("无法获取62域");
		return BANK_FAIL;
	}
    AppUtils_Cls();

	if(KeyMngr_CheckVal((char *) cTmpBuf, iLength) != BANK_OK)
	{
		Comm_Disconnect();
		//PrmMngr_ResetLogonData((void*)0);
		return BANK_FAIL;
	}
    //qDebug("**6****************************");
	//保存后台日期时间
	memset(cDate, 0, sizeof(cDate));
	memset(cTime, 0, sizeof(cTime));
	
    if(GetBit(&RspIsoRec, 13, cDate) > 0 &&
       GetBit(&RspIsoRec, 12, cTime) > 0)
	{
		AppUtils_UpdateDateTime((char *) cDate, (char *) cTime);
	}

	//同步批次号
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = GetBit(&RspIsoRec, 60, cTmpBuf);
	
	if(iLength <= 0)
	{
		AppUtils_Warning("无法获取60域");
		return BANK_FAIL;
	}
	
	cTmpBuf[ 8 ] = 0;
	gTermCfg.mDynPrm.BatchNum = (ulint) atol((char *)cTmpBuf + 2);
	AppUtils_SetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_OPENED);
    //qDebug("**8****************************");
	//如果签到时，后台处理要求签到，是否还要继续签？ 2014/12/4 11:30:15
	if(gTermCfg.mDynPrm.NextEvent == 0x03)
		gTermCfg.mDynPrm.NextEvent = 0;
		
	PrmMngr_SavePrmToSamDisk();
    //qDebug("**9****************************");
    //TmsMngr_CheckAndUpdateSettleFlag();
	
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayCenter("签到成功", LINE2, FALSE);
    AppUtils_WaitKey(10);
	
	// 判断PBOC参数是否需要下载参数和公钥
	if(AppUtils_CheckStatus(gTermCfg.mDynPrm.EMVPrmStatus, STATUS_AIDDOWN) != BANK_OK)
	{
		if(Trans_AIDDownload(dummy) == BANK_OK)
			AppUtils_SetStatus(&gTermCfg.mDynPrm.EMVPrmStatus, STATUS_AIDDOWN);
	}
	
	if(AppUtils_CheckStatus(gTermCfg.mDynPrm.EMVPrmStatus, STATUS_CAKEYDOWN) != BANK_OK)
	{
		if(Trans_CAKeyDownload(dummy) == BANK_OK)
			AppUtils_SetStatus(&gTermCfg.mDynPrm.EMVPrmStatus, STATUS_CAKEYDOWN);
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_LogonOper
 * DESCRIPTION:   操作员签到
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_LogonOper(void *dummy)
{
	int iLen;
	Bank_Operator tmpOper;
	char cBuffer[ 56 ];
	char cTmpBuf[ 56 ];
	
	// 主管不能做交易
	if((memcmp(gTermCfg.mDynPrm.OperID, "00", 2) == 0) ||
	   (memcmp(gTermCfg.mDynPrm.OperID, "99", 2) == 0))
	{
		if(dummy != (void *)2)
		{
			AppUtils_Warning("主管不允许做交易!");
			return BANK_FAIL;
		}
	}
	
	if(dummy != (void *)3 && dummy != (void *)2)
	{
		if(AppUtils_IsNullData((char *)gTermCfg.mDynPrm.OperID, 2) != BANK_OK)
		{
			return BANK_OK;
		}
	}
	
	if(dummy == (void *)0 || dummy == (void *)2)
	{
		AppUtils_Cls();
		AppUtils_DisplayCenter("操作员签到", LINE1, TRUE);
	}
	else if(dummy == (void *)3)
	{
		AppUtils_Cls();
		AppUtils_DisplayCenter("操作员积分签到", LINE1, TRUE);
	}
	
	memset(&tmpOper, 0, sizeof(tmpOper));
	iLen = AppUtils_GetNum(LINE2, "请输入操作员号:", tmpOper.OperID, 2, 2, 60);
	
	if(iLen <= 0)
		return BANK_FAIL;
		
	tmpOper.IDLen = iLen;
	
	memset(cBuffer, 0, sizeof(cBuffer));
	
	if(memcmp(tmpOper.OperID, "99", 2) == 0)
	{
		iLen = 8;
		sprintf(cBuffer, "请输入管理员密码");
	}
	else if(memcmp(tmpOper.OperID, "98", 2) == 0)
	{
		AppUtils_Warning("禁止管理员签到");
		return BANK_FAIL;
	}
	else if(memcmp(tmpOper.OperID, "00", 2) == 0)
	{
		iLen = 6;
		sprintf(cBuffer, "请输入主管密码");
	}
	else
	{
		iLen = 4;
		sprintf(cBuffer, "请输入操作员密码:");
	}
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	iLen = AppUtils_GetPassWord(LINE2, cBuffer, cTmpBuf, iLen, iLen, RIGHT_JST);
	
	if(iLen <= 0)
		return BANK_FAIL;
		
	if(memcmp(tmpOper.OperID, "99", 2) == 0)
	{
		if(memcmp(cTmpBuf, "20100322", 8) == 0)
		{
			PrmMngr_ManufacturerSettings((void*)0);
			return BANK_FAIL;
		}
		else if(memcmp(cTmpBuf, "19491911", 8) == 0)
		{
			PrmMngr_SetDebug((void*)0);
			return BANK_FAIL;
		}
	}
	
	
	tmpOper.PwdLen = iLen;
	
	if(AppUtils_OperFind(&tmpOper) != BANK_OK)
	{
		AppUtils_Warning("操作员不存在");
		return BANK_FAIL;
	}
	
	if(memcmp(tmpOper.OperPwd, cTmpBuf, iLen) != 0)
	{
		AppUtils_Warning("密码错误");
		return BANK_FAIL;
	}
	
	if(memcmp(tmpOper.OperID, "99", 2) == 0 && tmpOper.OperType == OPER_SUPERVISOR)
	{
		PrmMngr_Menu_SysManage((void *) 1);
		
		return BANK_FAIL;
	}
	
	
	if(dummy == (void *)1 && memcmp(tmpOper.OperID, "00", 2) == 0 && tmpOper.OperType == OPER_MANAGER)
	{
		AppUtils_Warning("不允许主管进行联机签到");
		return BANK_FAIL;
	}
	
	memcpy(gTermCfg.mDynPrm.OperID, tmpOper.OperID, tmpOper.IDLen);
	memcpy(gTermCfg.mDynPrm.OperPwd, tmpOper.OperPwd, tmpOper.PwdLen);
	
	if(memcmp(tmpOper.OperID, "00", 2) == 0)
	{
		PrmMngr_OperManageMenu((void *)0);
		
		//主管操作员做操作员签到后，同样应该保存为当前操作员
		PrmMngr_SavePrmToSamDisk();
		return BANK_FAIL;
	}
	
	
	if(dummy == (void *)2)
	{
		//有工作密钥，直接置终端签到状态，之后可以直接做交易
		if(AppUtils_IsNullData((char*)gTermCfg.mDynPrm.PINKey, sizeof(gTermCfg.mDynPrm.PINKey)) != BANK_OK
		   && AppUtils_IsNullData((char*)gTermCfg.mDynPrm.MACKey, sizeof(gTermCfg.mDynPrm.MACKey)) != BANK_OK)
		{
			AppUtils_SetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_OPENED);
		}
	}
	
	PrmMngr_SavePrmToSamDisk();
	
	
	if(dummy == (void *)0 || dummy == (void *)2)
	{
		AppUtils_DisplayCenter((char *)"操作员签到成功", LINE2, FALSE);
		AppUtils_WaitKey(3);
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_LogonScore
 * DESCRIPTION:   收银员积分签到
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_LogonScore(void *dummy)
{
	DynData TransData;
	int iLength;
	byte cTmpBuf[ 126 ];
	byte cBuffer[ ISO8583_MAXLENTH ];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = BTYPE_LOGON_CASHIER;
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_UserClear(LINE1);
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	// 刷卡/插卡/挥卡，获取卡号
	ASSERT_FAIL(Trans_GetTrackPANData(TRAN_SWIP | TRAN_ICC | TRAN_NUM, &TransData));
	
	// 预拨号
	Comm_CheckPreConnect();
	
	// 显示卡号有效期
	ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));
	
	// 打包
	ISOEngine_ClearAllBits(&ReqIsoRec);
	ISOEngine_ClearAllBits(&RspIsoRec);
	ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);
	
	// field 0
	memcpy(TransData.MsgID, "0820", 4);
	ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);
	
	// field 2
	if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
		ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);
		
	// field 14
	if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
		ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
	}
	
	// field 22
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
	ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);
	
	// field 35
	if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
		ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));
		
	// field 36
	if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
		ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));
		
	// field 41
	ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);
	
	// field 42
	ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);
	
	// field 53
	Trans_SetField53_Data(&ReqIsoRec, &TransData);
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	memcpy(cTmpBuf, "00", 2);
	sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);
	memcpy(cTmpBuf + 8, "401", 3);
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// 组包转换
	memset(cBuffer, 0x00, sizeof(cBuffer));
	// TPDU
	memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);
	// CUP Header
	memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);
	
	iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);
	
	if(iLength <= 0)
	{
		Comm_Disconnect();
		return BANK_FAIL;
	}
	
	iLength += 11;
	
	// 检查网络并拨号
	ASSERT_FAIL(Comm_CheckConnect());
	
	// 发送数据包
	ASSERT_FAIL(Comm_SendPacket(cBuffer, iLength));
	
	// 接收数据包
	memset(cBuffer, 0, sizeof(cBuffer));
	iLength = 0;
	
	ASSERT_FAIL(Comm_RecvPacket(cBuffer, &iLength));
	
	// 挂线
	Comm_Disconnect();
	
	// 解包
	ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));
	
	// 显示处理错误代码
	ASSERT_FAIL(PrnDisp_DisplayRespCode(&TransData));
	
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayCenter("收银员签到成功", LINE2, FALSE);
	AppUtils_WaitKey(5);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_OperatorLogout
 * DESCRIPTION:   操作员签退
 * PARAMETERS:    dummy: (void*)0-无提示信息直接签退, (void*)1显示签退成功提示信息
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_OperatorLogout(void *dummy)
{
	memset(gTermCfg.mDynPrm.OperID, 0, sizeof(gTermCfg.mDynPrm.OperID));
	memset(gTermCfg.mDynPrm.OperPwd, 0, sizeof(gTermCfg.mDynPrm.OperPwd));
	PrmMngr_SavePrmToSamDisk();
	
	if(dummy == (void*)1)
	{
		AppUtils_UserClear(LINE2);
		AppUtils_DisplayCenter("操作员签退成功", LINE2, FALSE);
		AppUtils_WaitKey(5);
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Logout
 * DESCRIPTION:   签退
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Logout(void *dummy)
{
	DynData TransData;
	int iLength;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ ISO8583_MAXLENTH ];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = BTYPE_LOGOUT;
	
	// 显示交易类型
	AppUtils_UserClear(LINE1);
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	if(dummy == (void *) 0)
	{
		if(AppUtils_GetNumberOfRecords((char *)TRAN_DB, sizeof(TransRecord)) > 0)
		{
			PrnDisp_DisplayWarning("签退",  "请先结算");
			return BANK_FAIL;
		}
		
		// 判断是否有冲正
		if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL) == BANK_OK)
		{
			PrnDisp_DisplayWarning("签退",  "请先结算");
			return BANK_FAIL;
		}
	}
	
	// 预拨号 签退不做预拨号
	// Comm_CheckPreConnect();
	
	// 增加流水号
	PrmMngr_AddSystrace();
	
	// 打包
	ISOEngine_ClearAllBits(&ReqIsoRec);
	ISOEngine_ClearAllBits(&RspIsoRec);
	ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);
	
	// field 0
	memcpy(TransData.MsgID, "0820", 4);
	ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);
	
	// field 11
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
	ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);
	
	// field 41
	ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);
	
	// field 42
	ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	memcpy(cTmpBuf, "00", 2);
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);
	memcpy(cTmpBuf + 8, "002", 3);
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// 组包转换
	memset(cBuffer, 0x00, sizeof(cBuffer));
	// TPDU
	memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);
	// CUP Header
	memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);
	
	iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);
	
	if(iLength <= 0)
	{
		Comm_Disconnect();
		return BANK_FAIL;
	}
	
	iLength += 11;
	
	// 检查网络并拨号
	ASSERT_HANGUP_FAIL(Comm_CheckConnect());
	
	// 检测是否有脚本需要上送
	Trans_UploadICCscript(&TransData);
	
	// 检查冲正
	ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));
	
	// 检查是否有脱机交易需要上送
	Trans_SubmitOffline(0);
	
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检查网络并拨号
	ASSERT_HANGUP_FAIL(Comm_CheckConnect());
	
	// 发送数据包
	ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));
	
	// 接收数据包
	memset(cBuffer, 0, sizeof(cBuffer));
	iLength = 0;
	
	ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));
	
	// 挂线
	Comm_Disconnect();
	
	// 解包
	ASSERT_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));
	
	// 显示处理错误代码
	ASSERT_FAIL(PrnDisp_DisplayRespCode(&TransData));
	
	Trans_OperatorLogout((void*)0);
	
	PrmMngr_ResetLogonData((void*)0);
	
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayCenter("签退成功", LINE2, FALSE);
	AppUtils_WaitKey(5);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Sale
 * DESCRIPTION:   消费交易
 * PARAMETERS:    (void*)1:电子现金普通支付
 *                (void*)2:电子现金快速支付
 *                (void*)3:电子现金普通支付/快速支付
 *                (void*)99: 待机界面刷卡/插卡消费交易
 *                (void*)0: 主菜单进来的消费交易
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Sale(void *dummy)
{
	DynData TransData;
	int iRet;
	int iLength;
	byte bPartOfSuccessFlag = FALSE;
	usint usCardMode;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ ISO8583_MAXLENTH ];
    ISO_data ReqIsoRec, RspIsoRec;
	
	AppDebug_AppLog("trans_sale( void *dummy=[%d])", (int)dummy);
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = BTYPE_SALE;

    //消费输入金额
    AppUtils_Cls();

	if(dummy == (void*)1)
		AppUtils_DisplayCenter((char*)"普通支付", LINE1, TRUE);
	else if(dummy == (void*)2)
		AppUtils_DisplayCenter((char*)"快速支付", LINE1, TRUE);
	else if(dummy == (void*)3)
		AppUtils_DisplayCenter((char*)"电子现金消费", LINE1, TRUE);
	else
    {
        printf("TransData.TransDB.TrType[%d]\n", TransData.TransDB.TrType);
		PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    }

	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

	// 刷卡/插卡/挥卡，获取卡号
	if(dummy == (void*)1)
	{
		// 通过菜单选择:电子现金普通消费
		usCardMode = TRAN_ICC;
		TransData.EC_PaybyMenuFlag = 1;
	}
	else if(dummy == (void *)2)
	{
		if(gTermCfg.mTrSwitch.CTLS_Support)
		{
			// 通过菜单选择:电子现金快速消费
			usCardMode = TRAN_QPBOC;
			TransData.EC_PaybyMenuFlag = 1;
			TransData.EC_CTLSPriOpt = 0;
		}
		else
		{
			AppUtils_Warning((char *)"不支持非接设备");
			return BANK_FAIL;
		}
	}
	else if(dummy == (void *)3)
	{
		if(gTermCfg.mTrSwitch.ECash_Support == 0)
		{
			AppUtils_Warning((char *)"电子现金功能已关闭");
			return BANK_FAIL;
		}
		
		usCardMode = TRAN_ICC;
		TransData.EC_PaybyMenuFlag = 1;
		
		if(gTermCfg.mTrSwitch.CTLS_Support)
		{
			usCardMode |= TRAN_QPBOC;
			TransData.EC_CTLSPriOpt = 1;
		}
	}
	else
	{
		usCardMode = TRAN_SWIP | TRAN_ICC;
		
		if(gTermCfg.mTrSwitch.CTLS_Support && gTermCfg.mTrSwitch.GetPanSupportCTLS)
		{
			usCardMode |= TRAN_QPBOC;
			usCardMode |= TRAN_MCHIP;
			TransData.EC_CTLSPriOpt = 0;
			
			if(dummy == (void*)0)
			{
				if(gTermCfg.mTrSwitch.CtlsTransChannelFlag == 0)
				{
					TransData.EC_CTLSPriOpt = 0;
				}
				else
				{
					TransData.EC_CTLSPriOpt = 1;
				}
			}
		}
		
	}
	
	if(dummy == (void *)98)
		usCardMode = TRAN_ICC | TRAN_IDLECARD;
	else if(dummy == (void *)99)
		usCardMode = TRAN_SWIP | TRAN_IDLECARD;
	else if(AppUtils_CheckUshortStatus(usCardMode, TRAN_QPBOC) == BANK_OK)
	{
		AppUtils_UserClear(LINE1);
		
		if(dummy == (void*)1)
			AppUtils_DisplayCenter((char*)"普通支付", LINE1, TRUE);
		else if(dummy == (void*)2)
			AppUtils_DisplayCenter((char*)"快速支付", LINE1, TRUE);
		else if(dummy == (void*)3)
			AppUtils_DisplayCenter((char*)"电子现金消费", LINE1, TRUE);
		else
			PrnDisp_DisplayTransName(TransData.TransDB.TrType);
			
		// 有QPBOC支持，需要预先输入金额
		if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
			ASSERT_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
	}
	
    ASSERT_FAIL(Trans_GetTrackPANData(usCardMode, &TransData));
	
	// 显示卡号有效期
	ASSERT_FAIL(Trans_ShowCardPAN(&TransData));
	
	// 预拨号

	if((TransData.TransDB.Swipe == TRAN_SWIP) ||
	   ((TransData.TransDB.Swipe == TRAN_ICC) && (TransData.TransDB.EC_flag != 1)) ||
	   ((TransData.TransDB.Swipe == TRAN_QPBOC) && (TransData.TransDB.qPBOC_OnlineFlag)))
	{
		Comm_CheckPreConnect();
	}

    qDebug("input money");
	// EMV脱机、联机交易认证
    iRet = EMVtrans_ConfirmProcessing(&TransData);
	if(iRet == BANK_FAIL)
	{
		Comm_Disconnect();
		return BANK_FAIL;
	}
	else if(iRet == BANK_EMV_BYPASS || iRet == BANK_EMV_ONLINE)
	{
        qDebug("*************************************");
        AppUtils_Cls();
        PrnDisp_DisplayTransName(TransData.TransDB.TrType);
		// 输入金额
		if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
		{
			ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
		}
		
		// 输入卡密码
		TransData.PinRetryFlag = 0;
		
		if(TransData.TransDB.Swipe != TRAN_ICC)
		{
            AppUtils_Cls();
            PrnDisp_DisplayTransName(TransData.TransDB.TrType);
			ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
		}
		
		// 检测是否有脚本需要上送
        //Trans_UploadICCscript(&TransData);
		
		// 检查冲正
		ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));
		
		// 增加流水号
		PrmMngr_AddSystrace();
		
		// 打包
        ClearBit(&ReqIsoRec);
        ClearBit(&RspIsoRec);
        //ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);
		
		// field 0
		memcpy(TransData.MsgID, "0200", 4);
        SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);
		
		// field 2
		if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
            SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);
			
		// field 3
		memcpy(TransData.TransDB.ProcessCode, "000000", 6);
        SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);
		
		// field 4
        SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);
		
		// field 14
		if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
		{
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
		}
		
		// field 23
		if(TransData.TransDB.SeqNumFlag == 1)
		{
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			memcpy(cTmpBuf, "00", 2);
			AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
		}
		
		// field 25
		memcpy(TransData.TransDB.ServiceCode, "00", 2);
        SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);
		
		// field 35
		if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
            SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));
			
		// field 36
		if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
            SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));
			
		// field 41
        SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);
		
		// field 42
        SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);
		
		// field 49
        SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);
		
		// field 55
		if((TransData.TransDB.Swipe == TRAN_ICC) || (TransData.TransDB.Swipe == TRAN_QPBOC))
		{
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			iLength = EMVtrans_GetEMVICCData(&TransData, cTmpBuf);
            SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);
		}
		else if(TransData.TransDB.Swipe == TRAN_MCHIP)
		{
            SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
		}
		
		// field 60
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		//60.1消息类型
		memcpy(cTmpBuf, "22", 2);
		memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);
		
		//60.2批次号码
		TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
		sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);
		
		//60.3网络管理信息码
		memcpy(cTmpBuf + 8, "000", 3);
		
		//60.4终端读取能力
		if(gTermCfg.mTrSwitch.CTLS_Support == 1)
			memcpy(cTmpBuf + 11, "6", 1);
		else
			memcpy(cTmpBuf + 11, "5", 1);
			
		//60.5 IC卡条件代码
		//0 未使用或后续子域存在，或手机芯片交易
		//1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
		//2 上一笔交易虽是IC卡交易但失败
		if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
			memcpy(cTmpBuf + 12, "2", 1);
		else
			memcpy(cTmpBuf + 12, "0", 1);
			
		//60.6 支持部分扣款和返回余额标志
		if(gTermCfg.mTrSwitch.PartPurchaseFlag)
			memcpy(cTmpBuf + 13, "1", 1);
		else
			memcpy(cTmpBuf + 13, "0", 1);
			
        SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
		
		do
		{
			// 重新输入卡密码
			if(TransData.PinRetryFlag > 0)
			{
				ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
				
				// 增加流水号
				PrmMngr_AddSystrace();
			}
			
			// field 11 SysTraceNo
			TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
			sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
            SetBit(&ReqIsoRec, 11, cTmpBuf, 6);
			
			// field 22
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
            SetBit(&ReqIsoRec, 22, cTmpBuf, 3);
			
			// field 26
			if(TransData.PinExists == 1)
                SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
			else
                ClearBit1(&ReqIsoRec, 26);
				
			// field 52
			if(TransData.PinExists == 1)
                SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);
			else
                ClearBit1(&ReqIsoRec, 52);
				
			// field 53
            Trans_SaleSetField53_Data(&ReqIsoRec, &TransData);
			
			// field 64
            Trans_SaleSetField64_MAC(&ReqIsoRec);
			
			// 组包转换
			memset(cBuffer, 0x00, sizeof(cBuffer));
			
			// TPDU
			memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);
			
			// CUP Header
			memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);
			
            iLength = IsoToStr(cBuffer + 5 + 6 , &ReqIsoRec);
			
			if(iLength <= 0)
			{
				Comm_Disconnect();
				return BANK_FAIL;
			}
			
			iLength += 11;
			
			// 检查网络并拨号
			if(Comm_CheckConnect() != BANK_OK)
				break;
				
			// 保存本交易冲正记录
			if(Trans_SetReversal(&TransData, cBuffer, iLength) != BANK_OK)
				break;
				
			// 发送数据包
			if(Comm_SendPacket(cBuffer, iLength) != BANK_OK)
				break;
				
			// 接收数据包
			memset(cBuffer, 0, sizeof(cBuffer));
			iLength = 0;
			
			if(Comm_RecvPacket(cBuffer, &iLength) != BANK_OK)
				break;
				
			Trans_CheckNeedDisconnect();
			
			// 解包
            if(Trans_UnPackData(&TransData, cBuffer, &RspIsoRec) != BANK_OK)
				break;
				
			// 部分扣款情况
			if((TransData.TransDB.Swipe == TRAN_SWIP) &&
			   (memcmp(TransData.TransDB.RspCode, "10", 2) == 0))
			{
				bPartOfSuccessFlag = TRUE;
				memcpy(TransData.TransDB.RspCode, "00", 2);
				
				memcpy(TransData.TransDB.OrgAmount, TransData.TransDB.Amount, 12);
				
				memset(TransData.TransDB.Amount, 0, sizeof(TransData.TransDB.Amount));
				
                if(GetBit(&RspIsoRec, 4, TransData.TransDB.Amount) <= 0)
				{
					// 挂线
					Comm_Disconnect();
					AppUtils_Warning("部分扣款成功,但无实际扣款金额返回,交易失败!");
					return BANK_FAIL;
				}
			}
			
			// 清除冲正标识
			if(Trans_ClearReversal(REVERSAL_CLEAR_STATUS) != BANK_OK)
				break;
				

			// 保存交易流水
			if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
			{
				Comm_Disconnect();
				Trans_ResetReversalStatus(&TransData);
				Trans_UpdateReversalFieldData(39, "06", 2);
				
				AppUtils_Warning("写流水失败交易失败!");
				return BANK_FAIL;
			}
		}
		while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0)
		      && TransData.TransDB.Swipe != TRAN_ICC
		      && (TransData.PinRetryFlag ++ < 2));
		      
		Trans_CheckNeedDisconnect();
		
		// IC卡联机交易卡片处理
		if(TransData.TransDB.Swipe == TRAN_ICC)
		{
			iLength = 0;
			memset(cBuffer, 0, sizeof(cBuffer));
            iLength = GetBit(&RspIsoRec, 55, cBuffer);
			
            //EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
		}
		else if(bPartOfSuccessFlag == TRUE)
		{
			// 磁条卡部分扣款流程
			
			// 用户选择
			AppUtils_Cls();
			AppUtils_DisplayCenter("请确认", LINE1, FALSE);
			AppUtils_DisplayCenter("部分承兑交易", LINE2, FALSE);
			AppUtils_DisplayCenter("[O]接受  [X]取消", LINE3, FALSE);
			
			iRet = AppUtils_WaitKey(60);
			
			if((iRet == bENTER) || (iRet == bKEY0))
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				memset(cTmpBuf, '0', 12);
				AppUtils_AmountSub(cTmpBuf, TransData.TransDB.OrgAmount, TransData.TransDB.Amount);
				
				memset(cBuffer, 0, sizeof(cBuffer));
				AppUtils_AmountAsc2String(cTmpBuf, "", '+', (char *)cBuffer);
				
				AppUtils_Cls();
				AppUtils_DisplayCenter("未付金额:", LINE1, FALSE);
				AppUtils_DisplayCenter((char *)cBuffer, LINE2, FALSE);
				AppUtils_DisplayCenter("请另行收取", LINE3, FALSE);
				AppUtils_WaitKey(30);
				
				AppUtils_UserClear(LINE1);
				
				if(dummy == (void*)1)
					AppUtils_DisplayCenter((char*)"普通支付", LINE1, TRUE);
				else if(dummy == (void*)2)
					AppUtils_DisplayCenter((char*)"快速支付", LINE1, TRUE);
				else if(dummy == (void*)3)
					AppUtils_DisplayCenter((char*)"电子现金消费", LINE1, TRUE);
				else
					PrnDisp_DisplayTransName(TransData.TransDB.TrType);
			}
			else
			{
				AppUtils_UserClear(LINE1);
				
				if(dummy == (void*)1)
					AppUtils_DisplayCenter((char*)"普通支付", LINE1, TRUE);
				else if(dummy == (void*)2)
					AppUtils_DisplayCenter((char*)"快速支付", LINE1, TRUE);
				else if(dummy == (void*)3)
					AppUtils_DisplayCenter((char*)"电子现金消费", LINE1, TRUE);
				else
					PrnDisp_DisplayTransName(TransData.TransDB.TrType);
					
				// 清除冲正标识和流水
				Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);
				
				// 打印已经扣款的单据
				PrnDisp_PrintTxn(&TransData, 0);
				
				// 撤销刚才的部分扣款交易
				inTrans_VoidOnline(TransData.TransDB.SysTrace, FALSE);
				
				// 检查是否有脱机交易需要上送
				Trans_SubmitOffline(0);
				
				// 挂线
				Comm_Disconnect();
				
				if(TransData.TransDB.Swipe == TRAN_ICC)
					EMVtrans_RemoveCard();
					
				return BANK_OK;
			}
		}
		
		// 清除冲正标识和流水
		if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
			Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);
	}
	else
	{
		if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
		{
			if((TransData.TransDB.EC_flag == 1) ||
			   (TransData.TransDB.Swipe == TRAN_QPBOC))
			{
				// 若交易成功,脱机的电子现金消费完以后要显示余额5秒钟
				memset(cBuffer, 0, sizeof(cBuffer));
				AppUtils_AmountAsc2String(TransData.TransDB.EC_Balance, CURRENCY_NAME, '+', (char *)cBuffer);
				
				if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
				{
					AppUtils_DisplayCenter("交易成功", LINE2, FALSE);
					AppUtils_DisplayLine("电子现金余额:", LINE3);
					AppUtils_DisplayLine((char *)cBuffer, LINE4);
					AppUtils_WaitKey(5);
				}
				else
				{
					AppUtils_DisplayCenter("交易成功", LINE2, FALSE);
					AppUtils_DisplayLine("电子现金余额", LINE3);
					AppUtils_DisplayLine("请查看密码键盘", LINE4);
					
                    //Pinpad_DisplayMsg("ECBALANCE:", (char *)cBuffer);
					AppUtils_WaitKey(5);
                    //Pinpad_Idle();
				}
			}
		}
	}
	
	// 显示处理错误代码
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));
	
	// 打印
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	PrnDisp_PrintTxn(&TransData, 0);
	
	if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_OFFLINE) != BANK_OK)
	{
		AppDebug_AppLog("upload offline after online txn");
		
		// 检查是否有脱机交易需要上送
		Trans_SubmitOffline(0);
	}
	
	// 检查脱机交易最大笔数是否达到自动上送
	Trans_CheckAutoUploadOffline();
	
	// 挂线
	Comm_Disconnect();
	
    //if(TransData.TransDB.Swipe == TRAN_ICC)
    EMVtrans_RemoveCard();


	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Void
 * DESCRIPTION:   消费撤销交易
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Void(void *dummy)
{
    char cBuffer[ 7 ];
    int iRet;

    // 显示交易类型
    AppUtils_Cls();
    PrnDisp_DisplayTransName(BTYPE_SALE_VOID);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(BTYPE_SALE_VOID));

    // 输入主管密码
    if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
        return BANK_FAIL;

    // 获取原交易流水号
    memset(cBuffer, 0, sizeof(cBuffer));

    if(AppUtils_GetNum(LINE2, "请输入原凭证号:", cBuffer, 1, 6, 60) <= 0)
        return BANK_FAIL;

    // 联机取消
     iRet = inTrans_VoidOnline(atol(cBuffer), TRUE);
     return iRet;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Refund
 * DESCRIPTION:   退货
 * PARAMETERS:    (void*)0: 普通退货交易
 *                (void*)1: 电子现金脱机交易的退货
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Refund(void *dummy)
{
    DynData TransData;
    int iLength;
    usint usSwipeMode;
    char cTitleBuf[ 26 ];
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO_data ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));

    if(dummy == (void *)0)
        TransData.TransDB.TrType = BTYPE_REFUND;
    else
        TransData.TransDB.TrType = BTYPE_ECASH_REFUND;


    // 显示交易类型
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 输入主管密码
    if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
        return BANK_FAIL;

    // 刷卡/插卡/挥卡，获取卡号
    usSwipeMode = TRAN_SWIP | TRAN_ICC;

    // 电子现金脱机交易退货
    if(dummy == (void *)1)
    {
        usSwipeMode = TRAN_ICC;
    }

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_QPBOC;
        TransData.EC_CTLSPriOpt = 0;
    }
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
    ASSERT_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    if(TransData.TransDB.Swipe == TRAN_QPBOC)   //如果是非接挥卡，脱机退货需要显示卡号确认
        TransData.TransDB.qPBOC_OnlineFlag = 1;   //最新的非接改造,前面已经强制联机,这边可以考虑去掉 2015-07-16
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
    // 显示卡号有效期
    ASSERT_FAIL(Trans_ShowCardPAN(&TransData));
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
    // 输入原参考号 普通退货交易需要输入
    if(dummy == (void *)0)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetText(LINE3, "请输入原参考号:", (char *)cTmpBuf, 12, 12, 0, 60) < 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        memcpy(TransData.TransDB.OrgRefCode, cTmpBuf, 12);
    }

    // 输入原交易日期
    while(TRUE)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE3, "请输入原交易日期(月月日日):", (char *)cTmpBuf, 4, 4, 60) <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        if(AppUtils_CheckDateFormatValid((char *)cTmpBuf) == BANK_OK)
        {
            memcpy(TransData.TransDB.OrgTranDate, cTmpBuf, 4);
            break;
        }
    }

    // 若IC卡脱机交易退货
    if(dummy == (void *)1)
    {
        //原终端号
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE3, "请输入原终端号:", (char *)cTmpBuf, 8, 8, 60) <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        memcpy(TransData.TransDB.OrgTermID, cTmpBuf, 8);

        //原批次号
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE2, "请输入原批次号:", (char *)cTmpBuf, 1, 6, 60) <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        TransData.TransDB.OrgBatchNum = (ulint)atol((char *)cTmpBuf);

        //原流水号
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE3, "请输入原流水号:", (char *)cTmpBuf, 1, 6, 60) <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        TransData.TransDB.OrgSysTrace = (ulint)atol((char *)cTmpBuf);
    }

    // 预拨号
    Comm_CheckPreConnect();
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
    // 输入金额
    while(TRUE)
    {
        ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE3, "请输入退货金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));

        if(memcmp(TransData.TransDB.Amount, gTermCfg.mFixPrm.RefundAmt, 12) > 0)
        {
            memset(TransData.TransDB.Amount, 0, sizeof(TransData.TransDB.Amount));

            if(TransData.TransDB.TrType != BTYPE_ECASH_REFUND)
                sprintf(cTitleBuf, "%s", "退货");
            else
                sprintf(cTitleBuf, "%s", "电子现金脱机退货");

            PrnDisp_DisplayWarning(cTitleBuf, "退货金额超限!");
            continue;
        }

        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_AmountAsc2String(TransData.TransDB.Amount, "", '+', (char *)cTmpBuf);

        if(TransData.TransDB.TrType != BTYPE_ECASH_REFUND)
            sprintf(cTitleBuf, "%s", "退货");
        else
            sprintf(cTitleBuf, "%s", "电子现金脱机退货");
        qDebug("amount:%s", cTmpBuf);
        if(AppUtils_YesNoConfirm((char *)cTitleBuf, "请确认金额:", (char *)cTmpBuf, "按O键确认X键修改") == BANK_OK)
            break;
        else
            memset(TransData.TransDB.Amount, 0, sizeof(TransData.TransDB.Amount));
    }

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 检测是否有脚本需要上送
    Trans_UploadICCscript(&TransData);

    // 检查冲正
    ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

    // 增加流水号
    PrmMngr_AddSystrace();

    // 打包
    ClearBit(&ReqIsoRec);
    ClearBit(&RspIsoRec);
    //ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0220", 4);
    SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 2
    if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
        SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

    // field 3
    memcpy(TransData.TransDB.ProcessCode, "200000", 6);
    SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

    // field 4
    SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

    // field 11
    TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
    sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
    SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

    // field 14
    if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
        SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
    }

    // field 22
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
    SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

    // field 23
    if(TransData.TransDB.SeqNumFlag == 1)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, "00", 2);
        AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
        SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
    }

    // field 25
    memcpy(TransData.TransDB.ServiceCode, "00", 2);
    SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

    // field 26
    if(TransData.PinExists == 1)
        SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);

    // field 35
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
        SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

    // field 36
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
        SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

    // field 37  原参考号: 普通退货交易用, 电子现金脱机退货不填
    if(dummy == (void *)0)
        SetBit(&ReqIsoRec, 37, TransData.TransDB.OrgRefCode, 12);

    // field 41
    SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 49
    SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

    // field 52
    if(TransData.PinExists == 1)
        SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);

    // field 53
    Trans_SaleSetField53_Data(&ReqIsoRec, &TransData);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    //60.1消息类型
    if(dummy == (void *)1)     //电子现金脱机退货填27
        memcpy(cTmpBuf, "27", 2);
    else
        memcpy(cTmpBuf, "25", 2);

    memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

    //60.2批次号码
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3网络管理信息码
    memcpy(cTmpBuf + 8, "000", 3);

    //60.4终端读取能力
    if(gTermCfg.mTrSwitch.CTLS_Support == 1)
        memcpy(cTmpBuf + 11, "6", 1);
    else
        memcpy(cTmpBuf + 11, "5", 1);

    //60.5 IC卡条件代码
    //0 未使用或后续子域存在，或手机芯片交易
    //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
    //2 上一笔交易虽是IC卡交易但失败
    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
        memcpy(cTmpBuf + 12, "2", 1);
    else
        memcpy(cTmpBuf + 12, "0", 1);

    //60.6 支持部分扣款和返回余额标志
    if(gTermCfg.mTrSwitch.PartPurchaseFlag)
        memcpy(cTmpBuf + 13, "1", 1);
    else
        memcpy(cTmpBuf + 13, "0", 1);

    SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // field 61
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    sprintf((char *)cTmpBuf, "%06ld%06ld", TransData.TransDB.OrgBatchNum, TransData.TransDB.OrgSysTrace);
    memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);
    SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));

    // field 62 脱机退货 填写原终端号  参看62域用法十八
    if(dummy == (void *)1)
        SetBit(&ReqIsoRec, 62, TransData.TransDB.OrgTermID, 8);

    // field 63
    SetBit(&ReqIsoRec, 63, (unsigned char *)"000", 3);

    // field 64
    Trans_SaleSetField64_MAC(&ReqIsoRec);

    // 组包转换
    memset(cBuffer, 0x00, sizeof(cBuffer));

    // TPDU
    memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

    // CUP Header
    memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

    iLength = IsoToStr(cBuffer + 5 + 6, &ReqIsoRec);

    if(iLength <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    iLength += 11;

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 发送数据包
    ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

    // 接收数据包
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = 0;

    ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

    Trans_CheckNeedDisconnect();

    // 解包
    ASSERT_HANGUP_FAIL(Trans_UnPackData(&TransData, cBuffer, &RspIsoRec));

    // 保存交易流水
    ASSERT_HANGUP_FAIL(AppUtils_JournalWriteRecord(0, &TransData));

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    //if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PreAuth
 * DESCRIPTION:   预授权
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PreAuth(void *dummy)
{
    DynData TransData;
    int iRet;
    int iLength;
    usint usCardMode;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO_data ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_PREAUTH;

    // 显示交易类型
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 预授权不支持手输卡号
    usCardMode = TRAN_SWIP | TRAN_ICC;

    if(dummy == (void *)98 || dummy == (void *)99)
        usCardMode |= TRAN_IDLECARD;
    else
    {
        if(gTermCfg.mTrSwitch.CTLS_Support && gTermCfg.mTrSwitch.GetPanSupportCTLS)
        {
            usCardMode |= TRAN_QPBOC;
            TransData.EC_CTLSPriOpt = 0;

            // 输入金额
            if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
                ASSERT_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
        }
    }

    // 显示交易类型
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
    // 刷卡/插卡/挥卡，获取卡号
    ASSERT_FAIL(Trans_GetTrackPANData(usCardMode, &TransData));

    // 显示交易类型
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
    // 显示卡号有效期
    ASSERT_FAIL(Trans_ShowCardPAN(&TransData));

    // 预拨号
    Comm_CheckPreConnect();

    // EMV脱机、联机交易认证
    iRet = EMVtrans_ConfirmProcessing(&TransData);
    iRet = BANK_EMV_BYPASS;
    if(iRet == BANK_FAIL)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }
    else if(iRet == BANK_EMV_BYPASS || iRet == BANK_EMV_ONLINE)
    {
        // 输入金额
        if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
            ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));

        // 输入卡密码
        TransData.PinRetryFlag = 0;

        if(TransData.TransDB.Swipe != TRAN_ICC)
        {
            ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
        }

        // 检测是否有脚本需要上送
        Trans_UploadICCscript(&TransData);

        // 检查冲正
        ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

        // 增加流水号
        PrmMngr_AddSystrace();

        // 打包
        ClearBit(&ReqIsoRec);
        ClearBit(&RspIsoRec);
        //ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0100", 4);
        SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
            SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 3
        memcpy(TransData.TransDB.ProcessCode, "030000", 6);
        SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

        // field 4
        SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

        // field 14
        if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
        }

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 25
        memcpy(TransData.TransDB.ServiceCode, "06", 2);
        SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

        // field 35
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
            SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

        // field 36
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
            SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

        // field 41
        SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 49
        SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

        // field 55
        if((TransData.TransDB.Swipe == TRAN_ICC) || (TransData.TransDB.Swipe == TRAN_QPBOC))
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            iLength = EMVtrans_GetEMVICCData(&TransData, cTmpBuf);
            SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);
        }

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        //60.1消息类型
        memcpy(cTmpBuf, "10", 2);
        memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

        //60.2批次号码
        TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

        //60.3网络管理信息码
        memcpy(cTmpBuf + 8, "000", 3);

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        //0 未使用或后续子域存在，或手机芯片交易
        //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
        //2 上一笔交易虽是IC卡交易但失败
        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
            memcpy(cTmpBuf + 12, "2", 1);
        else
            memcpy(cTmpBuf + 12, "0", 1);

        SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        TransData.PinRetryFlag = 0;

        do
        {
            // 重新输入卡密码
            if(TransData.PinRetryFlag > 0)
            {
                ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));

                // 增加流水号
                PrmMngr_AddSystrace();
            }

            // field 11
            TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
            sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
            SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

            // field 22
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
            SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

            // field 26
            if(TransData.PinExists == 1)
                SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
            else
                ClearBit1(&ReqIsoRec, 26);

            // field 52
            if(TransData.PinExists == 1)
                SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);
            else
                ClearBit1(&ReqIsoRec, 52);

            // field 53
            Trans_SaleSetField53_Data(&ReqIsoRec, &TransData);

            // field 64
            Trans_SaleSetField64_MAC(&ReqIsoRec);

            // 组包转换
            memset(cBuffer, 0x00, sizeof(cBuffer));

            // TPDU
            memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

            // CUP Header
            memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

            iLength = IsoToStr(cBuffer + 5 + 6,&ReqIsoRec);

            if(iLength <= 0)
            {
                Comm_Disconnect();
                return BANK_FAIL;
            }

            iLength += 11;

            // 检查网络并拨号
            if(Comm_CheckConnect() != BANK_OK)
                break;

            // 保存本交易冲正记录
            if(Trans_SetReversal(&TransData, cBuffer, iLength) != BANK_OK)
                break;

            // 发送数据包
            if(Comm_SendPacket(cBuffer, iLength) != BANK_OK)
                break;

            // 接收数据包
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = 0;

            if(Comm_RecvPacket(cBuffer, &iLength) != BANK_OK)
                break;

            Trans_CheckNeedDisconnect();

            // 解包
            if(Trans_UnPackData(&TransData, cBuffer, &RspIsoRec) != BANK_OK)
                break;

            // 清除冲正标识
            if(Trans_ClearReversal(REVERSAL_CLEAR_STATUS) != BANK_OK)
                break;

            // 保存交易流水
            if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
            {
                Trans_ResetReversalStatus(&TransData);

                Trans_UpdateReversalFieldData(39, "06", 2);

                break;
            }
        }
        while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0)
              && (TransData.TransDB.Swipe != TRAN_ICC || TransData.TransDB.Swipe != TRAN_QPBOC)
              && (TransData.PinRetryFlag ++ < 2));

        Trans_CheckNeedDisconnect();

        // IC卡联机交易卡片处理
        if(TransData.TransDB.Swipe == TRAN_ICC)
        {
            iLength = 0;
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = GetBit(&RspIsoRec, 55, cBuffer);

            //EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
//      if((iLength > 0) || (memcmp(TransData.TransDB.RspCode, "00", 2) == 0))
//      {
//        //EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
//      }
        }

    }

    // 清除冲正标识和流水
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PreAuthCompleteRequest
 * DESCRIPTION:   预授权完成请求
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PreAuthCompleteRequest(void *dummy)
{
    DynData TransData;
    int iLength;
    usint usSwipeMode;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_COMPLETE;
    TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 刷卡/插卡，获取卡号
    usSwipeMode = TRAN_SWIP | TRAN_ICC;

    if(gTermCfg.mTrSwitch.ManualFlag == 1)
        usSwipeMode |= TRAN_NUM;

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_QPBOC;
        TransData.EC_CTLSPriOpt = 0;
    }

    //////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    // 显示卡号有效期
    ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

    // 预拨号
    Comm_CheckPreConnect();

    // 输入原交易日期
    while(TRUE)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE2, "请输入原交易日期(月月日日):", (char *)cTmpBuf, 4, 4, 60) <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        if(AppUtils_CheckDateFormatValid((char *)cTmpBuf) == BANK_OK)
        {
            memcpy(TransData.TransDB.OrgTranDate, cTmpBuf, 4);
            break;
        }
    }

    // 输入原参考号
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    if(AppUtils_GetText(LINE2, "请输入原授权号:", (char *)cTmpBuf, 2, 6, 0, 60) <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    memset(TransData.TransDB.OrgAuthCode, ' ', 6);
    memcpy(TransData.TransDB.OrgAuthCode, cTmpBuf, strlen((char *)cTmpBuf));

    // 输入金额
    if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
        ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));

    // 输入卡密码
    TransData.PinRetryFlag = 0;

    if(gTermCfg.mTrSwitch.PreAuthEndReqPIN == 1)
    {
        ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
    }

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 检测是否有脚本需要上送
    Trans_UploadICCscript(&TransData);

    // 检查冲正
    ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

    // 增加流水号
    PrmMngr_AddSystrace();

    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;

    // 打包
    ISOEngine_ClearAllBits(&ReqIsoRec);
    ISOEngine_ClearAllBits(&RspIsoRec);
    ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0200", 4);
    ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 2
    if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
        ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

    // field 3
    memcpy(TransData.TransDB.ProcessCode, "000000", 6);
    ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

    // field 4
    ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

    // field 14
    if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
    }

    // field 23
    if(TransData.TransDB.SeqNumFlag == 1)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, "00", 2);
        AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
        ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
    }

    // field 25
    memcpy(TransData.TransDB.ServiceCode, "06", 2);
    ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

    // field 35
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

    // field 36
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

    // field 38
    ISOEngine_SetBit(&ReqIsoRec, 38, TransData.TransDB.OrgAuthCode, 6);

    // field 41
    ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 49
    ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    //60.1消息类型
    memcpy(cTmpBuf, "20", 2);
    memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

    //60.2批次号码
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3网络管理信息码
    memcpy(cTmpBuf + 8, "000", 3);

    //60.4终端读取能力
    if(gTermCfg.mTrSwitch.CTLS_Support == 1)
        memcpy(cTmpBuf + 11, "6", 1);
    else
        memcpy(cTmpBuf + 11, "5", 1);

    //60.5 IC卡条件代码
    //0 未使用或后续子域存在，或手机芯片交易
    //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
    //2 上一笔交易虽是IC卡交易但失败
    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
        memcpy(cTmpBuf + 12, "2", 1);
    else
        memcpy(cTmpBuf + 12, "0", 1);

    ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // field 61
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    sprintf((char *)cTmpBuf, "%012d", 0);
    memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);
    ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));

    do
    {
        // 重新输入卡密码
        if(TransData.PinRetryFlag > 0)
        {
            if(gTermCfg.mTrSwitch.PreAuthEndReqPIN == 1)
            {
                ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));

                // 增加流水号
                PrmMngr_AddSystrace();
            }
            else //预授权完成请求不需要输密时, 重输密码功能没有意义
                break;
        }

        // field 11
        TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 26
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
        else
            ISOEngine_ClearOneBit(&ReqIsoRec, 26);

        // field 52
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);
        else
            ISOEngine_ClearOneBit(&ReqIsoRec, 52);

        // field 53
        Trans_SetField53_Data(&ReqIsoRec, &TransData);

        // field 64
        //Trans_SetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        iLength += 11;

        // 检查网络并拨号
        if(Comm_CheckConnect() != BANK_OK)
            break;

        // 保存本交易冲正记录
        ASSERT_HANGUP_FAIL(Trans_SetReversal(&TransData, cBuffer, iLength));

        // 发送数据包
        ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

        // 解包
        ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

        // 清除冲正标识
        ASSERT_HANGUP_FAIL(Trans_ClearReversal(REVERSAL_CLEAR_STATUS));

        // 保存交易流水
        if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
        {
            Trans_ResetReversalStatus(&TransData);
            Trans_UpdateReversalFieldData(39, "06", 2);
            break;
        }
    }
    while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0) && (TransData.PinRetryFlag ++ < 2));

    Trans_CheckNeedDisconnect();

    // 清除冲正标识和流水
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PreAuthCompleteNote
 * DESCRIPTION:   预授权完成通知
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PreAuthCompleteNote(void *dummy)
{
    DynData TransData;
    int iLength;
    unsigned short usSwipeMode;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_OFFLINE_COMPLETE;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 刷卡/插卡，获取卡号
    usSwipeMode = TRAN_SWIP | TRAN_ICC;

    if(gTermCfg.mTrSwitch.ManualFlag == 1)
        usSwipeMode |= TRAN_NUM;

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_QPBOC;
        TransData.EC_CTLSPriOpt = 0;
    }

    //////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    // 显示卡号有效期
    ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

    // 预拨号
    Comm_CheckPreConnect();

    // 输入原交易日期
    while(TRUE)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE2, "请输入原交易日期(月月日日):", (char *)cTmpBuf, 4, 4, 60) <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        if(AppUtils_CheckDateFormatValid((char *)cTmpBuf) == BANK_OK)
        {
            memcpy(TransData.TransDB.OrgTranDate, cTmpBuf, 4);
            break;
        }
    }

    // 输入原参考号
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    if(AppUtils_GetText(LINE2, "请输入原授权号:", (char *)cTmpBuf, 2, 6, 0, 60) <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    memset(TransData.TransDB.OrgAuthCode, ' ', 6);
    memcpy(TransData.TransDB.OrgAuthCode, cTmpBuf, strlen((char *)cTmpBuf));

    // 输入金额
    while(TRUE)
    {
        if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
            ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));

        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_AmountAsc2String(TransData.TransDB.Amount, CURRENCY_NAME, '+', (char *)cTmpBuf);

        if(AppUtils_YesNoConfirm("预授权完成通知", "请确认金额:", (char *)cTmpBuf, (char *)"按[X]键修改") == BANK_OK)
            break;
        else
            memset(TransData.TransDB.Amount, 0, sizeof(TransData.TransDB.Amount));
    }

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 检测是否有脚本需要上送
    Trans_UploadICCscript(&TransData);

    // 检查冲正
    ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

    // 增加流水号
    PrmMngr_AddSystrace();

    // 打包
    ISOEngine_ClearAllBits(&ReqIsoRec);
    ISOEngine_ClearAllBits(&RspIsoRec);
    ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0220", 4);
    ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 2
    if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
        ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

    // field 3
    memcpy(TransData.TransDB.ProcessCode, "000000", 6);
    ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

    // field 4
    ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

    // field 11
    TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
    sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
    ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

    // field 14
    if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
    }

    // field 22
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
    ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

    // field 23
    if(TransData.TransDB.SeqNumFlag == 1)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, "00", 2);
        AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
        ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
    }

    // field 25
    memcpy(TransData.TransDB.ServiceCode, "06", 2);
    ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

    // field 35
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

    // field 36
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

    // field 38
    ISOEngine_SetBit(&ReqIsoRec, 38, TransData.TransDB.OrgAuthCode, 6);

    // field 41
    ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 49
    ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

    // field 53
    Trans_SetField53_Data(&ReqIsoRec, &TransData);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    //60.1消息类型
    memcpy(cTmpBuf, "24", 2);
    memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

    //60.2批次号码
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3网络管理信息码
    memcpy(cTmpBuf + 8, "000", 3);

    //60.4终端读取能力
    if(gTermCfg.mTrSwitch.CTLS_Support == 1)
        memcpy(cTmpBuf + 11, "6", 1);
    else
        memcpy(cTmpBuf + 11, "5", 1);

    //60.5 IC卡条件代码
    //0 未使用或后续子域存在，或手机芯片交易
    //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
    //2 上一笔交易虽是IC卡交易但失败
    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
        memcpy(cTmpBuf + 12, "2", 1);
    else
        memcpy(cTmpBuf + 12, "0", 1);

    ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // field 61
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    sprintf((char *)cTmpBuf, "%012d", 0);
    memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);
    ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));

    // field 64
    //Trans_SetField64_MAC(&ReqIsoRec);

    // 组包转换
    memset(cBuffer, 0x00, sizeof(cBuffer));

    // TPDU
    memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

    // CUP Header
    memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

    iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

    if(iLength <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    iLength += 11;

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 发送数据包
    ASSERT_FAIL(Comm_SendPacket(cBuffer, iLength));

    // 接收数据包
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = 0;

    ASSERT_FAIL(Comm_RecvPacket(cBuffer, &iLength));

    Trans_CheckNeedDisconnect();

    // 解包
    ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

    // 保存交易流水
    if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PreAuthVoid
 * DESCRIPTION:   预授权撤销
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PreAuthVoid(void *dummy)
{
    DynData TransData;
    int iLength;
    usint usSwipeMode;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_PREAUTH_VOID;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 主管密码
    if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
        return BANK_FAIL;

    usSwipeMode = TRAN_SWIP | TRAN_ICC;

    if(gTermCfg.mTrSwitch.ManualFlag == 1)
        usSwipeMode |= TRAN_NUM;

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_QPBOC;
        TransData.EC_CTLSPriOpt = 0;
    }

    // 刷卡/插卡，获取卡号
    //////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    // 显示卡号有效期
    ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

    // 预拨号
    Comm_CheckPreConnect();

    // 输入原交易日期
    while(TRUE)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE2, "请输入原交易日期(月月日日):", (char *)cTmpBuf, 4, 4, 60) <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        if(AppUtils_CheckDateFormatValid((char *)cTmpBuf) == BANK_OK)
        {
            memcpy(TransData.TransDB.OrgTranDate, cTmpBuf, 4);
            break;
        }
    }

    // 输入原参考号
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    if(AppUtils_GetText(LINE2, "请输入原授权号:", (char *)cTmpBuf, 2, 6, 0, 60) <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    memset(TransData.TransDB.OrgAuthCode, ' ', 6);
    memcpy(TransData.TransDB.OrgAuthCode, cTmpBuf, strlen((char *)cTmpBuf));

    // 输入金额
    if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
        ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));

    // 输入卡密码
    if(gTermCfg.mTrSwitch.VoidPreAuthPIN == 1)
    {
        ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
    }

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 检测是否有脚本需要上送
    Trans_UploadICCscript(&TransData);

    // 检查冲正
    ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

    // 增加流水号
    PrmMngr_AddSystrace();

    // 打包
    ISOEngine_ClearAllBits(&ReqIsoRec);
    ISOEngine_ClearAllBits(&RspIsoRec);
    ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0100", 4);
    ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 2
    if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
        ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

    // field 3
    memcpy(TransData.TransDB.ProcessCode, "200000", 6);
    ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

    // field 4
    ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

    // field 11
    TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
    sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
    ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

    // field 14
    if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
    }

    // field 22
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
    ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

    // field 23
    if(TransData.TransDB.SeqNumFlag == 1)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, "00", 2);
        AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
        ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
    }

    // field 25
    memcpy(TransData.TransDB.ServiceCode, "06", 2);
    ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

    // field 26
    if(TransData.PinExists == 1)
        ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);

    // field 35
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

    // field 36
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

    // field 38
    ISOEngine_SetBit(&ReqIsoRec, 38, TransData.TransDB.OrgAuthCode, 6);

    // field 41
    ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 49
    ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

    // field 52
    if(TransData.PinExists == 1)
        ISOEngine_SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);

    // field 53
    Trans_SetField53_Data(&ReqIsoRec, &TransData);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    //60.1消息类型
    memcpy(cTmpBuf, "11", 2);
    memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

    //60.2批次号码
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3网络管理信息码
    memcpy(cTmpBuf + 8, "000", 3);

    //60.4终端读取能力
    if(gTermCfg.mTrSwitch.CTLS_Support == 1)
        memcpy(cTmpBuf + 11, "6", 1);
    else
        memcpy(cTmpBuf + 11, "5", 1);

    //60.5 IC卡条件代码
    //0 未使用或后续子域存在，或手机芯片交易
    //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
    //2 上一笔交易虽是IC卡交易但失败
    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
        memcpy(cTmpBuf + 12, "2", 1);
    else
        memcpy(cTmpBuf + 12, "0", 1);

    ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // field 61
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    sprintf((char *)cTmpBuf, "%012d", 0);
    memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);
    ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));

    // field 64
    //Trans_SetField64_MAC(&ReqIsoRec);

    // 组包转换
    memset(cBuffer, 0x00, sizeof(cBuffer));

    // TPDU
    memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

    // CUP Header
    memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

    iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

    if(iLength <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    iLength += 11;

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 保存本交易冲正记录
    ASSERT_HANGUP_FAIL(Trans_SetReversal(&TransData, cBuffer, iLength));

    // 发送数据包
    ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

    // 接收数据包
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = 0;

    ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

    Trans_CheckNeedDisconnect();

    // 解包
    ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

    // 清除冲正标识
    ASSERT_HANGUP_FAIL(Trans_ClearReversal(REVERSAL_CLEAR_STATUS));

    // 保存交易流水
    if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
    {
        Trans_ResetReversalStatus(&TransData);
        Trans_UpdateReversalFieldData(39, "06", 2);
    }

    // 清除冲正标识和流水
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PreAuthCompleteVoid
 * DESCRIPTION:   预授权完成撤销
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PreAuthCompleteVoid(void *dummy)
{
    DynData TransData;
    int iLength;
    usint usSwipeMode;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;
    int iRet;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_COMPLETE_VOID;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 主管密码
    if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
        return BANK_FAIL;

    // 获取原交易数据
    ASSERT_FAIL(Trans_GetOrignalJournal("请输入原凭证号:", &TransData));

    // 检查原交易状态
    if(TransData.TransDB.TrType != BTYPE_COMPLETE)
    {
        PrnDisp_DisplayWarning("预授权完成撤销", "原交易不是预授权完成!");
        return BANK_FAIL;
    }

    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_VOID) == BANK_OK)
    {
        PrnDisp_DisplayWarning("预授权完成撤销", "该交易已经撤销!");
        return BANK_FAIL;
    }

    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
    {
        PrnDisp_DisplayWarning("预授权完成撤销", "调整交易不能撤销!");
        return BANK_FAIL;
    }

    // 显示原交易相关信息
    while(TRUE)
    {
        iRet = PrnDisp_DisplayTxnDetail(&TransData, BTYPE_COMPLETE_VOID);

        if(iRet == bESC)
            return BANK_FAIL;
        else if(iRet == bENTER)
            break;
    }

    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(BTYPE_COMPLETE_VOID);

    // 复制原交易相关信息
    TransData.TransDB.OrgTranType = TransData.TransDB.TrType;
    TransData.TransDB.TrType = BTYPE_COMPLETE_VOID;
    TransData.TransDB.OrgSysTrace = TransData.TransDB.SysTrace;
    TransData.TransDB.OrgBatchNum = TransData.TransDB.BatchNum;
    memcpy(TransData.TransDB.OrgRefCode, TransData.TransDB.RefCode, 12);
    AppUtils_Bcd2Asc(TransData.TransDB.TranDate + 2, TransData.TransDB.OrgTranDate, 4);
    TransData.TransDB.Swipe = TRAN_NUM;
    TransData.PinExists = 0;
    TransData.TransDB.EncryptedTrkFlag = 0;
    memcpy(TransData.TransDB.POSEntry, "\x01\x20", 2);
    memset(TransData.TransDB.RspCode, 0, sizeof(TransData.TransDB.RspCode));
    memset(TransData.TransDB.Track2Data, 0, sizeof(TransData.TransDB.Track2Data));
    memset(TransData.TransDB.Track3Data, 0, sizeof(TransData.TransDB.Track3Data));

    //清除原交易中的TC上送标志：撤销交易不需要上送TC
    AppUtils_ResetStatus(&TransData.TransDB.Status[0], STATUS_TCUPLOAD);

    if(gTermCfg.mTrSwitch.VoidPreauthEndSwipCard)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        usSwipeMode = TRAN_SWIP | TRAN_ICC;

        if(gTermCfg.mTrSwitch.CTLS_Support)
        {
            usSwipeMode |= TRAN_QPBOC;
            TransData.EC_CTLSPriOpt = 0;
        }

        // 刷卡/插卡/挥卡，获取卡号
        //////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

        // 显示卡号有效期
        ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

        //匹配原卡号是否匹配
        iLength = strlen((char *)cTmpBuf);

        if(TransData.TransDB.PANLen != iLength)
        {
            Comm_Disconnect();
            AppUtils_Warning("卡号不匹配");
            return BANK_FAIL;
        }

        if(memcmp(cTmpBuf, TransData.TransDB.PAN, iLength) != 0)
        {
            Comm_Disconnect();
            AppUtils_Warning("卡号不匹配!");
            return BANK_FAIL;
        }
    }

    // 输入卡密码
    TransData.PinRetryFlag = 0;

    if(gTermCfg.mTrSwitch.VoidPreAuthEndPIN == 1)
    {
        // 预拨号
        Comm_CheckPreConnect();
        ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
    }

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 检测是否有脚本需要上送
    Trans_UploadICCscript(&TransData);

    // 检查冲正
    ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

    // 增加流水号
    PrmMngr_AddSystrace();

    // 打包
    ISOEngine_ClearAllBits(&ReqIsoRec);
    ISOEngine_ClearAllBits(&RspIsoRec);
    ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0200", 4);
    ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 2
    if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
        ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

    // field 3
    memcpy(TransData.TransDB.ProcessCode, "200000", 6);
    ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

    // field 4
    ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

    // field 14
    if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
    }

    // field 23
    if(TransData.TransDB.SeqNumFlag == 1)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, "00", 2);
        AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
        ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
    }

    // field 25
    memcpy(TransData.TransDB.ServiceCode, "06", 2);
    ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

    // field 35
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

    // field 36
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

    // field 37
    ISOEngine_SetBit(&ReqIsoRec, 37, TransData.TransDB.OrgRefCode, 12);

    // field 38
    if(AppUtils_IsNullData((char *) TransData.TransDB.OrgAuthCode, sizeof((char *) TransData.TransDB.OrgAuthCode)) != BANK_OK)
        ISOEngine_SetBit(&ReqIsoRec, 38, TransData.TransDB.OrgAuthCode, 6);

    // field 41
    ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 49
    ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    //60.1消息类型
    memcpy(cTmpBuf, "21", 2);
    memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

    //60.2批次号码
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3网络管理信息码
    memcpy(cTmpBuf + 8, "000", 3);

    //60.4终端读取能力
    if(gTermCfg.mTrSwitch.CTLS_Support == 1)
        memcpy(cTmpBuf + 11, "6", 1);
    else
        memcpy(cTmpBuf + 11, "5", 1);

    //60.5 IC卡条件代码
    //0 未使用或后续子域存在，或手机芯片交易
    //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
    //2 上一笔交易虽是IC卡交易但失败
    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
        memcpy(cTmpBuf + 12, "2", 1);
    else
        memcpy(cTmpBuf + 12, "0", 1);

    ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // field 61
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    sprintf((char *)cTmpBuf, "%06ld", TransData.TransDB.OrgBatchNum);
    sprintf((char *)cTmpBuf + 6, "%06ld", TransData.TransDB.OrgSysTrace);
    memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);
    ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));

    do
    {
        // 输入卡密码
        if(TransData.PinRetryFlag > 0)
        {
            if(gTermCfg.mTrSwitch.VoidPreAuthEndPIN == 1)
            {
                ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));

                // 增加流水号
                PrmMngr_AddSystrace();
            }
            else
                break;
        }

        // field 11
        TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 26
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
        else
            ISOEngine_ClearOneBit(&ReqIsoRec, 26);

        // field 52
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);
        else
            ISOEngine_ClearOneBit(&ReqIsoRec, 52);

        // field 53
        Trans_SetField53_Data(&ReqIsoRec, &TransData);

        // field 64
        //Trans_SetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        iLength += 11;

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 保存本交易冲正记录
        ASSERT_HANGUP_FAIL(Trans_SetReversal(&TransData, cBuffer, iLength));

        // 发送数据包
        ASSERT_FAIL(Comm_SendPacket(cBuffer, iLength));

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        ASSERT_FAIL(Comm_RecvPacket(cBuffer, &iLength));

        // 解包
        ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

        // 清除冲正标识
        ASSERT_HANGUP_FAIL(Trans_ClearReversal(REVERSAL_CLEAR_STATUS));

        // 保存交易流水
        if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
        {
            Comm_Disconnect();

            Trans_ResetReversalStatus(&TransData);
            Trans_UpdateReversalFieldData(39, "06", 2);
            return BANK_FAIL;
        }
    }
    while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0) && (TransData.PinRetryFlag ++ < 2));

    Trans_CheckNeedDisconnect();

    // 清除冲正标识和流水
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 更改原交易流水为已撤销
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        AppUtils_JournalUpdateRecordByStatus(TransData.TransDB.OrgSysTrace, 0, STATUS_VOID);

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_OfflineSettlement
 * DESCRIPTION:   离线结算
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_OfflineSettlement(void *dummy)
{
    DynData TransData;
    int iRet;
    char cTmpBuf[ 56 ];
    byte bt;
    char *CardOrgCode[7] = {"CUP", "VIS", "MCC", "MAE", "JCB", "DCC", "AMX"};
    PrnDisp_DisplayMenuItem aMenuItem[] =
    {
        { NULL,    1, "POS",        NULL, (void *) 0 },
        { NULL,    2, "电话",       NULL, (void *) 0 },
        { inTrans_IsSmallAmountAuth_Support,    3, "小额代授权", NULL, (void *) 0 },
    };


    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_OFFLINE_SETTLE;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 刷卡/插卡/挥卡，获取卡号
    ASSERT_FAIL(Trans_GetTrackPANData(TRAN_NUM, &TransData));

    // 显示卡号有效期
    ASSERT_FAIL(Trans_ShowCardPAN(&TransData));

    // 选择授权方式
    AppUtils_Cls();
    iRet = PrnDisp_DisplayMenu(LINE1, "选择授权方式", MENUOPT_RETURN | MENUOPT_AUTOCHECKLEN, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);

    if(iRet >= 1 && iRet <= 3)
        TransData.TransDB.AuthType = iRet - 1;
    else
        return BANK_FAIL;

    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 授权机构代码
    if(TransData.TransDB.AuthType == 1)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE2, "请输入授权机构代码:", cTmpBuf, 11, 11, 60) <= 0)
        {
            return BANK_FAIL;
        }

        memcpy(TransData.TransDB.AuthInst, cTmpBuf, 11);
    }

    // 输入授权号
    if((TransData.TransDB.AuthType == 0) || (TransData.TransDB.AuthType == 1))
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetText(LINE2, "请输入授权号:", cTmpBuf, 2, 6, 0, 60) <= 0)
        {
            return BANK_FAIL;
        }

        memset(TransData.TransDB.AuthCode, ' ', 6);
        memcpy(TransData.TransDB.AuthCode, cTmpBuf, strlen(cTmpBuf));
    }

    while(1)
    {
        // 选择国际金融卡机构代码
        if(AppUtils_IsBigScreen() == BANK_OK)
        {
            AppUtils_UserClear(LINE2);
            AppUtils_DisplayLine("选择国际公司代码", LINE2);
            AppUtils_DisplayLine("1.CUP 2.VIS 3.MCC", LINE3);
            AppUtils_DisplayLine("4.MAE 5.JCB 6.DCC", LINE4);
            AppUtils_DisplayLine("7.AEX", LINE5);
        }
        else
        {
            AppUtils_UserClear(LINE1);
            AppUtils_DisplayLine("选择国际公司代码", LINE1);
            AppUtils_DispEnglish("1.CUP      2.VIS", 1, 3);
            AppUtils_DispEnglish("3.MCC      4.MAE", 1, 4);
            AppUtils_DispEnglish("5.JCB      6.DCC", 1, 5);
            AppUtils_DispEnglish("7.AMX           ", 1, 6);
        }

        bt = AppUtils_WaitKey(60);

        if(bt == bKEY1 || bt == bENTER)
        {
            strcpy((char *)TransData.TransDB.CardTypeName, CardOrgCode[0]);
            break;
        }
        else if(bt >= bKEY2 && bt <= bKEY7)
        {
            strcpy((char *)TransData.TransDB.CardTypeName, CardOrgCode[bt-bKEY1]);
            break;
        }
        else if(bt == bESC || bt == 0)
        {
            return BANK_FAIL;
        }
    }

    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 输入金额
    if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
        ASSERT_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));

    // 增加流水号
    PrmMngr_AddSystrace();
    TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;

    AppUtils_GetClock(cTmpBuf);
    AppUtils_Asc2Bcd((byte *)cTmpBuf, TransData.TransDB.TranDate, 8);
    AppUtils_Asc2Bcd((byte *)cTmpBuf + 8, TransData.TransDB.TranTime, 6);

    memcpy(TransData.TransDB.POSEntry, "\x01\x20", 2);

    memcpy(TransData.TransDB.RspCode, "00", 2);

    AppUtils_SetStatus(&TransData.TransDB.Status[ 0 ], STATUS_OFFLINE);

    // 保存交易流水
    if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
    {
        return BANK_FAIL;
    }

    //离线未上送笔数
    gTermCfg.mDynPrm.OfflineRecNum ++;
    PrmMngr_SavePrmToSamDisk();

    // 显示处理错误代码
    ASSERT_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查脱机交易最大笔数是否达到自动上送
    Trans_CheckAutoUploadOffline();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_AdjustSettlement
 * DESCRIPTION:   结算调整
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_AdjustSettlement(void *dummy)
{
    DynData TransData;
    byte bAdjustJournalFlag = TRUE;
    byte cTmpBuf[64];
    char cDispBuf[64];
    int iRet;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_OFFLINE_ADJUST;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 获取原交易数据
    ASSERT_FAIL(Trans_GetOrignalJournal("请输入原凭证号:", &TransData));

    // 排除不允许调整的交易
    if((AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_VOID) == BANK_OK) ||
       ((TransData.TransDB.TrType != BTYPE_SALE) && (TransData.TransDB.TrType != BTYPE_OFFLINE_SETTLE)) ||
       ((TransData.TransDB.TrType == BTYPE_SALE) && (memcmp(TransData.TransDB.CardTypeName, "CUP", 3) == 0)))
    {
        PrnDisp_DisplayWarning("结算调整", "不允许做结算调整!");
        return BANK_FAIL;
    }

    if((TransData.TransDB.TrType == BTYPE_SALE) && (gTermCfg.mTrSwitch.FeeFlag != 1))
    {
        PrnDisp_DisplayWarning("结算调整", "不支持小费交易!");
        return BANK_FAIL;
    }

    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
    {
        PrnDisp_DisplayWarning("结算调整", "该交易已经调整!");
        return BANK_FAIL;
    }

    // 显示原交易相关信息
    while(TRUE)
    {
        iRet = PrnDisp_DisplayTxnDetail(&TransData, 0);

        if(iRet == bESC)
            return BANK_FAIL;
        else if(iRet == bENTER)
            break;
    }

    // 复制原交易相关信息
    TransData.TransDB.OrgTranType = TransData.TransDB.TrType;
    TransData.TransDB.TrType = BTYPE_OFFLINE_ADJUST;
    TransData.PinExists = 0;
    TransData.TransDB.OrgSysTrace = TransData.TransDB.SysTrace;
    TransData.TransDB.OrgBatchNum = TransData.TransDB.BatchNum;
    AppUtils_Bcd2Asc(TransData.TransDB.TranDate + 2, TransData.TransDB.OrgTranDate, 4);
    memcpy(TransData.TransDB.OrgAmount, TransData.TransDB.Amount, 12);

    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(BTYPE_OFFLINE_ADJUST);

    // 输入新金额或者小费金额
    if(TransData.TransDB.OrgTranType == BTYPE_OFFLINE_SETTLE)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_AmountAsc2String(TransData.TransDB.Amount, "", '+', (char *)cTmpBuf);

        if(strlen((char*)cTmpBuf) > 10)
        {
            AppUtils_DisplayLine((char *)"原金额 ", LINE1);

            AppUtils_SetFontFile("");
            AppUtils_DispEnglish((char *)cTmpBuf, 9, 2);
            AppUtils_SetFontSize(16);//设回中文字体，否则后面会以英文字体的行号来清屏
        }
        else
        {
            sprintf(cDispBuf, "原金额%10s", cTmpBuf);
            AppUtils_DisplayLine((char *)cDispBuf, LINE1);
        }

        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        //暂不需要填入原交易金额
        //memcpy(cTmpBuf, TransData.TransDB.OrgAmount, 12);
        ASSERT_FAIL(AppUtils_GetAmout(LINE2, "输入调整后金额:", '+' , CURRENCY_NAME, FALSE, cTmpBuf));

        memcpy(TransData.TransDB.Amount, cTmpBuf, 12);

        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_UPLOAD) != BANK_OK)
        {
            // 原离线结算交易尚未上送,该交易覆盖原离线结算交易,以离线结算的报文上送
            bAdjustJournalFlag = FALSE;
        }
        else
        {
            //对已上送的离线结算进行调整,生成一笔新交易，使用结算调整报文
        }

        AppUtils_UserClear(LINE1);
        PrnDisp_DisplayTransName(BTYPE_OFFLINE_ADJUST);
    }
    else
    {
        ASSERT_FAIL(AppUtils_GetAmout(LINE2, "请输入小费金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Fee));

        ASSERT_FAIL(AppUtils_CheckTipFloorLimit(TransData.TransDB.Amount, TransData.TransDB.Fee));

        AppUtils_AmountAdd(TransData.TransDB.Amount, TransData.TransDB.Amount, TransData.TransDB.Fee);
    }

    AppUtils_GetClock((char *)cTmpBuf);
    AppUtils_Asc2Bcd(cTmpBuf, TransData.TransDB.TranDate, 8);
    AppUtils_Asc2Bcd(cTmpBuf + 8, TransData.TransDB.TranTime, 6);

    // 更新原交易流水为已调整
    ASSERT_FAIL(inTrans_UpdateOrignalJournalAdjust(bAdjustJournalFlag, &TransData));

    // 保存本次调整交易流水记录
    if(bAdjustJournalFlag == TRUE)
    {
        // 增加流水号
        PrmMngr_AddSystrace();

        TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
        TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;

        AppUtils_Bcd2Asc(TransData.TransDB.TranDate + 2, TransData.TransDB.OrgTranDate, 4);

        memcpy(TransData.TransDB.POSEntry, "\x01\x20", 2);

        memcpy(TransData.TransDB.RspCode, "00", 2);

        AppUtils_SetStatus(&TransData.TransDB.Status[ 0 ], STATUS_OFFLINE);
        AppUtils_ResetStatus(&TransData.TransDB.Status[ 0 ], STATUS_UPLOAD);

        if(TransData.TransDB.OrgTranType == BTYPE_OFFLINE_SETTLE)
        {
            memcpy(TransData.TransDB.RefCode, TransData.TransDB.OrgRefCode, sizeof(TransData.TransDB.RefCode));
            memcpy(TransData.TransDB.AuthCode, TransData.TransDB.OrgAuthCode, sizeof(TransData.TransDB.AuthCode));
        }

        // 保存交易流水
        if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
            return BANK_FAIL;

        //离线未上送笔数
        gTermCfg.mDynPrm.OfflineRecNum ++;
        PrmMngr_SavePrmToSamDisk();
    }
    else
        TransData.TransDB.TrType = BTYPE_OFFLINE_SETTLE;

    // 显示处理错误代码
    ASSERT_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查脱机交易最大笔数是否达到自动上送
    Trans_CheckAutoUploadOffline();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Settlement
 * DESCRIPTION:   结算交易
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Settlement(void *dummy)
{
    DynData TransData;
    TotalRec TotalsData;
    int iLength;
//  int iDetailPrintFlag;
    char cTmpBuf[ 126 ];
    char *p;
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_SETTLE;

    if((AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK) ||
       (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK))
    {
        // 检查是否上次批上送不成功,需要继续批上送
        Trans_ReadTotalData(&TotalsData);
        AppDebug_AppLog("start to do uploading directly as upload batch fail last time");
        goto Next_DoBatchUpload;
    }
    else if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_TC) == BANK_OK)
    {
        // 检查是否上次批上送TC不成功,需要继续批上送
        Trans_ReadTotalData(&TotalsData);
        AppDebug_AppLog("start to do uploading directly as upload tc fail last time");
        goto Next_DoBatchUpload;
    }
    else if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_BFST) == BANK_OK)
    {
        AppDebug_AppLog("start to do uploading offline transactions before settlement");
        goto Next_DoUploadBeforeSettlement;
    }

    // 显示交易类型
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

Next_DoUploadBeforeSettlement:
    // 统计汇总数据
    memset(&TotalsData, 0, sizeof(TotalsData));
    AppUtils_DisplayLine((char*)"正在交易统计...", LINE2);
    ASSERT_HANGUP_FAIL(Trans_CalculateTotals(&TotalsData));

    // 预拨号
    Comm_CheckPreConnect();

    // 显示汇总数据
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayTxnTotal(&TotalsData));

    // 写汇总数据文件
    TotalsData.CupSettleFlag = TotalsData.FgnSettleFlag = 0;
    inTrans_SaveTotalData(&TotalsData);

    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    //结算前批上送离线交易
    ASSERT_HANGUP_FAIL(inTrans_UploadOfflineBeforeSettlement(TransData.TransDB.TrType));

    // 增加流水号
    PrmMngr_AddSystrace();

    // 打包
    ISOEngine_ClearAllBits(&ReqIsoRec);
    ISOEngine_ClearAllBits(&RspIsoRec);
    ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    gTermCfg.mDynPrm.UploadTotalCnt = 0;
    gTermCfg.mFixPrm.UploadCurTimes = 0;
    gTermCfg.mFixPrm.UploadCurNote = 0;
    PrmMngr_SavePrmToSamDisk();

    // field 0
    memcpy(TransData.MsgID, "0500", 4);
    ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 11
    TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
    sprintf(cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
    ISOEngine_SetBit(&ReqIsoRec, 11, (byte *) cTmpBuf, 6);

    // field 41
    ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 48  内卡和外卡的借贷记笔数
    memset(cTmpBuf, '0', sizeof(cTmpBuf));
    p = cTmpBuf;

    if(AppUtils_IsNullData((char *)TotalsData.CupTotalSaleAmt, sizeof(TotalsData.CupTotalSaleAmt)) != BANK_OK)
        memcpy(p, TotalsData.CupTotalSaleAmt, 12);

    p += 12;
    sprintf(p, "%03d", TotalsData.CupTotalSaleCnt);
    p += 3;
    *p = '0';

    if(AppUtils_IsNullData((char *)TotalsData.CupTotalVoidAmt, sizeof(TotalsData.CupTotalVoidAmt)) != BANK_OK)
        memcpy(p, TotalsData.CupTotalVoidAmt, 12);

    p += 12;
    sprintf(p, "%03d", TotalsData.CupTotalVoidCnt);
    p += 3;

    *p ++ = '0';

    if(AppUtils_IsNullData((char *)TotalsData.FgnTotalSaleAmt, sizeof(TotalsData.FgnTotalSaleAmt)) != BANK_OK)
        memcpy(p, TotalsData.FgnTotalSaleAmt, 12);

    p += 12;
    sprintf(p, "%03d", TotalsData.FgnTotalSaleCnt);
    p += 3;
    *p = '0';

    if(AppUtils_IsNullData((char *)TotalsData.FgnTotalVoidAmt, sizeof(TotalsData.FgnTotalVoidAmt)) != BANK_OK)
        memcpy(p, TotalsData.FgnTotalVoidAmt, 12);

    p += 12;
    sprintf(p, "%03d", TotalsData.FgnTotalVoidCnt);
    p += 3;

    *p ++ = '0';

    iLength = p - cTmpBuf;
    ISOEngine_SetBit(&ReqIsoRec, 48, (byte *)cTmpBuf, iLength);


    // field 49
    ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    //60.1
    memcpy(cTmpBuf, "00", 2);

    //60.2
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf(cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3
    memcpy(cTmpBuf + 8, "201", 3);
    ISOEngine_SetBit(&ReqIsoRec, 60, (byte *)cTmpBuf, strlen(cTmpBuf));

    // field 63
    memcpy(cTmpBuf, gTermCfg.mDynPrm.OperID, 2);
    cTmpBuf[ 2 ] = ' ';
    ISOEngine_SetBit(&ReqIsoRec, 63, (byte *)cTmpBuf, 3);

    // 组包转换
    memset(cBuffer, 0x00, sizeof(cBuffer));

    // TPDU
    memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

    // CUP Header
    memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

    iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

    if(iLength <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    iLength += 11;

    // 检查网络并拨号
    ASSERT_HANGUP_FAIL(Comm_CheckConnect());

    // 发送数据包
    ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

    // 接收数据包
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = 0;

    ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

    // 解包
    ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

    // 39域, 银联结算交易没有返回,直接copy一个进去
    memcpy(TransData.TransDB.RspCode, "00", 2);

    // 取48域结算结果数据
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    if(ISOEngine_GetBit(&RspIsoRec, 48, (byte *)cTmpBuf, sizeof(cTmpBuf)) > 0)
    {
        // 内卡
        if(cTmpBuf[ 30 ] == '2' || cTmpBuf[ 30 ] == '3')    // 对帐不平/对帐出错
        {
            TotalsData.CupSettleFlag = 1;
            AppUtils_SetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP);
            PrmMngr_SavePrmToSamDisk();

            AppDebug_AppLog("settlement result: cup need to do upload");
        }

        // 外卡
        if(cTmpBuf[ 61 ] == '2' || cTmpBuf[ 61 ] == '3')    // 对帐不平/对帐出错
        {
            TotalsData.FgnSettleFlag = 1;
            AppUtils_SetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN);
            PrmMngr_SavePrmToSamDisk();

            AppDebug_AppLog("settlement result: foreign card need to do upload");
        }

        inTrans_SaveTotalData(&TotalsData);

        if((AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK) ||
           (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK))
        {
            gTermCfg.mFixPrm.UploadCurNote = 0;
        }
        else
        {
            gTermCfg.mFixPrm.UploadCurNote = 5;
        }

        PrmMngr_SavePrmToSamDisk();
    }
    else
    {
        Comm_Disconnect();
        PrnDisp_DisplayWarning("结算", "主机返回没有48域错!");
        return BANK_FAIL;
    }

Next_DoBatchUpload:
    //设置结算后的批上送标志 by XC 2014/12/19 10:20:26
    AppUtils_SetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_TC);
    PrmMngr_SavePrmToSamDisk();

    ASSERT_HANGUP_FAIL(inTrans_UploadAfterSettlement(&TransData));

    //清除结算后的批上送标志 by XC 2014/12/19 10:20:26
    AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_TC);
    PrmMngr_SavePrmToSamDisk();

    // 挂线
    Comm_Disconnect();

    // 显示处理错误代码-银联结算交易没有39域返回
    // ASSERT_FAIL( PrnDisp_DisplayRespCode( &TransData ) );

    // 打印结算单
    memcpy(TotalsData.DateTime, TransData.TransDB.TranDate, 4);
    memcpy(TotalsData.DateTime + 4, TransData.TransDB.TranTime, 3);
    inTrans_SaveTotalData(&TotalsData);
    PrnDisp_PrintSettlement(&TotalsData, FALSE);

    // 增加结算计数
    PrmMngr_AddSettCount();

//  iDetailPrintFlag = 0;

    // 打印明细单
    if(gTermCfg.mTrSwitch.DetailPrintFlag == 1)
    {
//    iDetailPrintFlag = 1;
        // 显示交易类型
        AppUtils_Cls();
        PrnDisp_DisplayTransName(TransData.TransDB.TrType);
        PrnDisp_PrintSettleDetail(dummy);
    }

    // 打印上送失败的离线明细单
    if(gTermCfg.mTrSwitch.FailDetailPrint && inTrans_CheckPrintFailOfflineList() == BANK_OK)
    {
//    iDetailPrintFlag = 1;
        PrnDisp_PrintJournal((void *) 1);
        PrnDisp_PrintJournal((void *) 2);
    }

//  if(iDetailPrintFlag == 0)
//  {
//    PrnDisp_PrintFormFeed();
//  }

    // 若结算成功,更改批上送标识
    //if( memcmp( TransData.TransDB.RspCode, "00", 2 ) == 0 )
    {
        AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP);
        AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN);
        PrmMngr_SavePrmToSamDisk();

        // 清空全部交易流水记录
        PrmMngr_ClearAllTrans((void *)1);

        //重打印结算单需要使用
        //File_Remove(TRAN_TOTAL);
    }

    //更新批次号(自动加1)
    gTermCfg.mDynPrm.BatchNum ++;
    PrmMngr_SavePrmToSamDisk();

    // 自动签退
    if(gTermCfg.mTrSwitch.AutoLogoutFlag)
        Trans_Logout((void *) 1);

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_LockTerminal
 * DESCRIPTION:   锁定终端，当前操作员和主管可以解锁
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_LockTerminal(void *dummy)
{
    char cTmpBuf[ 56 ];
    Bank_Operator tmpOper;

    gTermCfg.mDynPrm.BankStatus |= BANK_LOCKTERM;
    PrmMngr_SavePrmToSamDisk();

    while(TRUE)
    {
        AppUtils_Cls();
        AppUtils_DisplayCenter("POS已锁定", LINE1, TRUE);

        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(AppUtils_GetNum(LINE2, "请输入操作员号:", cTmpBuf, 2, 2, 60) <= 0)
            continue;

        if(memcmp(cTmpBuf, "00", 2) == 0)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));

            if(AppUtils_GetPassWord(LINE2, "请输入主管密码:", cTmpBuf, 6, 6, RIGHT_JST) <= 0)
                continue;

            memset(&tmpOper, 0, sizeof(Bank_Operator));
            memcpy(tmpOper.OperID, "00", 2);
            tmpOper.IDLen = 2;

            if(AppUtils_OperFind(&tmpOper) != BANK_OK)
            {
                AppUtils_Warning("主管操作员不存在");
                return BANK_FAIL;
            }

            if(memcmp(tmpOper.OperPwd, cTmpBuf, 6) == 0)
            {
                memcpy(gTermCfg.mDynPrm.OperID, tmpOper.OperID, 2);
                memcpy(gTermCfg.mDynPrm.OperPwd, tmpOper.OperPwd, 6);
                //memcpy(inTrans_cLockOper, "00", 2);
                break;
            }
            else
            {
                AppUtils_Warning("主管密码错误");
            }
        }
        else if(memcmp(cTmpBuf, gTermCfg.mDynPrm.OperID, 2) == 0)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));

            if(AppUtils_GetPassWord(LINE2, "请输入操作员密码:", cTmpBuf, 4, 4, RIGHT_JST) <= 0)
                continue;

            if(memcmp(gTermCfg.mDynPrm.OperPwd, cTmpBuf, 4) == 0)
            {
                //memset(inTrans_cLockOper, 0, sizeof(inTrans_cLockOper));
                break;
            }
            else
            {
                AppUtils_Warning("操作员密码不符!");
            }
        }
        else
        {
            AppUtils_Warning("不是当前操作员");
        }
    }

    gTermCfg.mDynPrm.BankStatus &= ~BANK_LOCKTERM;
    PrmMngr_SavePrmToSamDisk();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_InquireJournal
 * DESCRIPTION:   查询交易明细
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_InquireJournal(void *dummy)
{
    int i;
    int iRecNum;
    int iRet;
    int iOption;
    DynData TransData;

    AppUtils_Cls();
    iRecNum = AppUtils_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));

    if(iRecNum > 0)
    {
        i = iRecNum - 1;

        iOption = bENTER;

        while(TRUE)
        {
            memset(&TransData, 0, sizeof(TransData));

            if(AppUtils_GetTransRecByIdx(&TransData, i) != BANK_OK)
                break;

            if(TransData.TransDB.TrType == BTYPE_BALANCE)
            {
                if(iOption == bENTER)
                    i --;
                else
                    i ++;

                continue;
            }

            iRet = PrnDisp_DisplayTxnDetail(&TransData, 0);

            if(iRet == bENTER)
            {
                iOption = bENTER;

                //向前翻
                i --;

                if(i < 0)
                {
                    i = 0;
                    AppUtils_NormalBeep();
                }
            }

            else if(iRet == bBKSP || iRet == bKEYC)
            {
                iOption = bBKSP;

                //向前翻
                i ++;

                if(i > iRecNum - 1)
                {
                    i = iRecNum - 1;
                    AppUtils_NormalBeep();
                }
            }
            else if(iRet == bESC)
                break;
        }
    }
    else
        AppUtils_Warning("无交易记录存在!");

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_InquireTotal
 * DESCRIPTION:   查询交易汇总
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_InquireTotal(void *dummy)
{
    TotalRec TotalsData;

    // 统计汇总数据
    memset(&TotalsData, 0, sizeof(TotalsData));
    ASSERT_FAIL(Trans_CalculateTotals(&TotalsData));

    PrnDisp_DisplayTxnTotal(&TotalsData);

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_InquireByTrace
 * DESCRIPTION:   按凭证号查询流水
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_InquireByTrace(void *dummy)
{
    DynData TransData;

    AppUtils_Cls();
    AppUtils_DisplayCenter("按凭证号查询", LINE1, TRUE);

    // 获取原交易数据
    memset(&TransData, 0, sizeof(TransData));
    ASSERT_FAIL(Trans_GetOrignalJournal("请输入凭证号:", &TransData));

    PrnDisp_DisplayTxnDetail(&TransData, 0);

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_BalanceInquire
 * DESCRIPTION:   查余额
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_BalanceInquire(void *dummy)
{
    DynData TransData;
    int i;
    int iRet;
    int iLength;
    usint usSwipeMode;
    byte cTmpBuf[ 512 ];
    byte *p, bTmp;
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_BALANCE;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    usSwipeMode = TRAN_SWIP | TRAN_ICC;

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_QPBOC;
        TransData.EC_CTLSPriOpt = 0;
    }

    // 刷卡/插卡/挥卡，获取卡号
    //////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    // 显示卡号有效期
    ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

    // 预拨号
    Comm_CheckPreConnect();

    // EMV脱机、联机交易认证
    iRet = EMVtrans_ConfirmProcessing(&TransData);

    if(iRet == BANK_FAIL)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }
    else if(iRet == BANK_EMV_BYPASS || iRet == BANK_EMV_ONLINE)
    {
        // 输入卡密码
        TransData.PinRetryFlag = 0;

        if(TransData.TransDB.Swipe != TRAN_ICC)
            ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 检测是否有脚本需要上送
        Trans_UploadICCscript(&TransData);

        // 检查冲正
        ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

        // 增加流水号
        PrmMngr_AddSystrace();

        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0200", 4);
        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
            ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 3
        memcpy(TransData.TransDB.ProcessCode, "310000", 6);
        ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

        // field 14
        if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
        }

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 25
        memcpy(TransData.TransDB.ServiceCode, "00", 2);
        ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

        // field 35
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

        // field 36
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 49
        ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

        // field 55
        if((TransData.TransDB.Swipe == TRAN_ICC) || (TransData.TransDB.Swipe == TRAN_QPBOC))
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            iLength = EMVtrans_GetEMVICCData(&TransData, cTmpBuf);
            ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);
        }

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        //60.1消息类型
        memcpy(cTmpBuf, "01", 2);
        memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

        //60.2批次号码
        TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

        //60.3网络管理信息码
        memcpy(cTmpBuf + 8, "000", 3);

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        //0 未使用或后续子域存在，或手机芯片交易
        //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
        //2 上一笔交易虽是IC卡交易但失败
        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
            memcpy(cTmpBuf + 12, "2", 1);
        else
            memcpy(cTmpBuf + 12, "0", 1);

        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        do
        {
            // 重新输入卡密码
            if(TransData.PinRetryFlag > 0)
            {
                ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));

                // 增加流水号
                PrmMngr_AddSystrace();
            }

            // field 11
            TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
            sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
            ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

            // field 22
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

            // field 26
            if(TransData.PinExists == 1)
                ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
            else
                ISOEngine_ClearOneBit(&ReqIsoRec, 26);

            // field 52
            if(TransData.PinExists == 1)
                ISOEngine_SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);
            else
                ISOEngine_ClearOneBit(&ReqIsoRec, 52);

            // field 53
            Trans_SetField53_Data(&ReqIsoRec, &TransData);

            // field 64
            //Trans_SetField64_MAC(&ReqIsoRec);

            // 组包转换
            memset(cBuffer, 0x00, sizeof(cBuffer));

            // TPDU
            memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

            // CUP Header
            memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

            iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

            if(iLength <= 0)
            {
                Comm_Disconnect();
                return BANK_FAIL;
            }

            iLength += 11;

            // 检查网络并拨号
            ASSERT_FAIL(Comm_CheckConnect());

            // 发送数据包
            ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

            // 接收数据包
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = 0;

            ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

            // 解包
            ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

        }
        while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0)
              && TransData.TransDB.Swipe != TRAN_ICC
              && (TransData.PinRetryFlag ++ < 2));

        Trans_CheckNeedDisconnect();

        // IC卡联机交易卡片处理
        if(TransData.TransDB.Swipe == TRAN_ICC)
        {
            iLength = 0;
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));

            //EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
        }
    }

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 显示余额
    AppUtils_UserClear(LINE2);
    AppUtils_DisplayLine("余额查询成功", LINE2);

    memset(cBuffer, 0, sizeof(cBuffer));

    if(ISOEngine_GetBit(&RspIsoRec, 54, cBuffer, sizeof(cBuffer)) > 0)
    {
        p = cBuffer;
        p += 7; // pass 7 bytes

        bTmp = *p ++;

        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, p, 12);

        for(i = 0; i < 12; i++)
        {
            if(cTmpBuf[ i ] == ' ')
                cTmpBuf[ i ] = '0';
        }

        memset(cBuffer, 0, sizeof(cBuffer));
        AppUtils_AmountAsc2String(cTmpBuf, "", bTmp, (char *)cBuffer);


        if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
        {
            AppUtils_DisplayLine("可用余额:", LINE3);
            AppUtils_DisplayRight((char *)cBuffer, LINE4);
            AppUtils_WaitKey(5);
        }
        else
        {
            AppUtils_DisplayLine("请查看密码键盘", LINE4);
            ////Pinpad_DisplayMsg("BAL:", (char *)cBuffer);     //此处只能使用英文提示信息
            AppUtils_WaitKey(5);
            //Pinpad_Idle();
        }
    }
    else
    {
        AppUtils_DisplayCenter("无余额返回", LINE4, FALSE);
        AppUtils_WaitKey(5);
    }


    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashQPay
 * DESCRIPTION:   电子现金普通支付/快速支付
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashQPay(void *dummy)
{
    if(!gTermCfg.mTrSwitch.CTLS_Support)
    {
        AppUtils_UserClear(LINE1);
        //AppUtils_DisplayCenter((char*)"快速支付", LINE1, TRUE);
        AppUtils_DisplayCenter((char*)"电子现金消费", LINE1, TRUE);
        AppUtils_Warning("该功能未开通");
        return BANK_FAIL;
    }

    return(Trans_Sale((void *) 3));
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashPay
 * DESCRIPTION:   普通支付
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashPay(void *dummy)
{
    if(!gTermCfg.mTrSwitch.ECash_Support)
    {
        AppUtils_UserClear(LINE1);
        AppUtils_DisplayCenter((char*)"普通支付", LINE1, TRUE);
        AppUtils_Warning("该功能未开通");
        return BANK_FAIL;
    }

    return(Trans_Sale((void *) 1));
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashBalance
 * DESCRIPTION:   电子现金查余额
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashBalance(void *dummy)
{
    usint usCardMode;
    DynData TransData;
    char cBuffer[ 126 ], cBuffer2[128];

    // 显示交易类型
    AppUtils_UserClear(LINE1);

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_ECASH_BALANCE;
    TransData.EC_PaybyMenuFlag = 1;

    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    usCardMode =  TRAN_ICC | TRAN_ICFORLOG;

    if(gTermCfg.mTrSwitch.CTLS_Support == 1)
    {
        usCardMode |= TRAN_CTLSPBOC;
        TransData.EC_CTLSPriOpt = 2;
        strcpy((char *)TransData.TransDB.Amount, "000000000000");
        //CTLS_SetSencondCurrencyFlag((void*)0);
    }

    //////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usCardMode, &TransData));

    if((TransData.TransDB.Swipe == TRAN_ICC) && (TransData.TransDB.EC_flag != 1))
    {
        AppUtils_Warning((char*)"非电子现金卡不能查余额");
        return BANK_OK;
    }

    if(TransData.TransDB.Swipe == TRAN_ICC)
    {
#ifdef  MAKE_LINUX
        unsigned long ulBalance = 0;

        if(vfiEmv_GetCardECBalance(&ulBalance) != VFI_EMV_OK)
        {
            AppDebug_AppLog("vfiEmv_GetCardECBalance fail");
            AppUtils_Warning((char*)"读电子现金余额失败!");
            return BANK_FAIL;
        }

        sprintf((char *)TransData.TransDB.EC_Balance, "%012ld", ulBalance);
#else

        if(EMVtrans_GetEcashBalance(&TransData) != BANK_OK)
        {
            AppDebug_AppLog("EMVtrans_GetEcashBalance fail");
            AppUtils_Warning((char*)"读电子现金余额失败!");
            return BANK_FAIL;
        }

#endif
    }

    // 显示余额
    AppUtils_UserClear(LINE2);
    AppUtils_DisplayCenter("余额查询成功", LINE2, FALSE);

    memset(cBuffer, 0, sizeof(cBuffer));

    if(strlen((char *)TransData.TransDB.CTLSCurrencyName1) > 0)
    {
        AppUtils_AmountAsc2String(TransData.TransDB.EC_Balance, (char *)TransData.TransDB.CTLSCurrencyName1, '+', (char *)cBuffer);
    }
    else
    {
        AppUtils_AmountAsc2String(TransData.TransDB.EC_Balance, CURRENCY_NAME, '+', (char *)cBuffer);
    }

    memset(cBuffer2, 0, sizeof(cBuffer2));

    if(strlen((char *)TransData.TransDB.CTLSCurrencyName2) > 0)
    {
        AppUtils_AmountAsc2String(TransData.TransDB.EC_Balance2, (char *)TransData.TransDB.CTLSCurrencyName2, '+', (char *)cBuffer2);
    }

    if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
    {
        AppUtils_DisplayLine("电子现金余额:", LINE3);
        AppUtils_DisplayLine((char *)cBuffer, LINE4);
        AppUtils_WaitKey(5);

        if(strlen(cBuffer2))
        {
            AppUtils_ClearLine(LINE3);
            AppUtils_DisplayLine("电子现金余额2:", LINE3);
            AppUtils_DisplayLine((char *)cBuffer2, LINE4);
            AppUtils_WaitKey(5);
        }
    }
    else
    {
        ////Pinpad_DisplayMsg("ECBALANCE:", (char *)cBuffer);
        AppUtils_DisplayLine("请查看密码键盘", LINE4);
        AppUtils_WaitKey(5);
        //Pinpad_Idle();

        if(strlen(cBuffer2))
        {
            ////Pinpad_DisplayMsg("ECBAL:", (char *)cBuffer2);
            AppUtils_DisplayLine("请查看密码键盘", LINE4);
            AppUtils_WaitKey(5);
            //Pinpad_Idle();
        }
    }

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PBOCQueryTransDetail
 * DESCRIPTION:   (交易日志)明细查询
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PBOCQueryTransDetail(void *dummy)
{
    DynData TransData;
    int iRet;
    int iLen;
    int iRecordMaxNum;
    int iRecordOffset;
    int iLogFormatLen;
    byte bSFI;
    char bLogFormat[ 32 ];
    char cBuffer[ 256 ];

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    AppUtils_DisplayCenter("明细查询", LINE1, TRUE);

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_ECASH_BALANCE;
    TransData.EC_PaybyMenuFlag = 1;

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    ASSERT_FAIL(Trans_GetTrackPANData(TRAN_ICC | TRAN_ICFORLOG, &TransData));

    if(TransData.TransDB.EC_flag != 1)
    {
        PrnDisp_DisplayWarning("", "非电子现金卡不能查明细");
        return BANK_OK;
    }

    memset(cBuffer, 0, sizeof(cBuffer));

    iLen = EMVtrans_ReadTagData(0x9f4d, cBuffer, sizeof(cBuffer));

    if(iLen <= 0)
    {
        bSFI = 0x0B;
        iRecordMaxNum = 0x0A;
    }
    else
    {
        bSFI = cBuffer[ 0 ];
        iRecordMaxNum = cBuffer[ 1 ];
    }

    memset(cBuffer, 0, sizeof(cBuffer));
    memcpy(cBuffer, "\x80\xCA\x9F\x4F\x00", 5);
    iLen = ICCmd_SendCommand((byte *)cBuffer, 0, 1);

    if(iLen <= 0)
    {
        PrnDisp_DisplayWarning("", "无日志格式信息");
        return BANK_FAIL;
    }

    if(memcmp(cBuffer, "\x9F\x4F", 2) == 0)
    {
        iLogFormatLen = cBuffer[ 2 ];

        iLen = iLogFormatLen;

        if(iLen > sizeof(bLogFormat))
            iLen = sizeof(bLogFormat);

        memcpy(bLogFormat, &cBuffer[ 3 ], iLen);
    }
    else
    {
        PrnDisp_DisplayWarning("", "没有交易明细!");
        return BANK_OK;
    }

    // Read Log Record Data
    iRecordOffset = 1;

    while(TRUE)
    {
        memset(cBuffer, 0, sizeof(cBuffer));
        cBuffer[ 0 ] = 0x00;
        cBuffer[ 1 ] = 0xB2;
        cBuffer[ 2 ] = iRecordOffset;
        cBuffer[ 3 ] = (bSFI << 3) | 0x04;
        cBuffer[ 4 ] = 0;

        iLen = ICCmd_SendCommand((byte *)cBuffer, 0, 1);

        if(iLen <= 0)
        {
            PrnDisp_DisplayWarning("明细查询", "无明细记录!");
            break;
        }

        if(AppUtils_IsColorScreen() == BANK_OK)
            AppUtils_SetFontSize(28);

        iRet = PrnDisp_DisplayECashTxnLogDetail(iRecordOffset, cBuffer, bLogFormat, iLogFormatLen);

        if(iRet == BANK_NEXT)
            iRecordOffset ++;
        else if(iRet == BANK_PRE)
            iRecordOffset --;
        else
        {
            AppUtils_UserClear(LINE1);
            AppUtils_DisplayCenter("明细查询", LINE1, TRUE);

            return BANK_OK;
        }

        if((iRecordOffset > iRecordMaxNum) || (iRecordOffset < 1))
        {
            if(AppUtils_IsColorScreen() == BANK_OK)
                AppUtils_SetFontSize(28);

            AppUtils_UserClear(LINE1);
            AppUtils_DisplayCenter("明细查询", LINE1, TRUE);

            return BANK_OK;
        }
    }

    AppUtils_UserClear(LINE1);
    AppUtils_DisplayCenter("明细查询", LINE1, TRUE);

    return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PBOCReadReloadTransLog
 * DESCRIPTION:   圈存交易日志(不是所有的卡都可以做)
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PBOCReadReloadTransLog(void *dummy)
{
    byte buf[10] = {0};
    byte i, bt;
    unsigned short usDataLen;
    char SUNSONDispBuf[32 + 1];
    int iOffset, iTagLen, iLen;
    char cBuffer[ 256 ], Buf1[128], Buf2[128];
    DynData TransData;

    AppUtils_Cls();
    AppUtils_DisplayCenter("圈存日志查询", LINE1, TRUE);

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_ECASH_BALANCE;
    TransData.EC_PaybyMenuFlag = 1;

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    memset(&TransData, 0, sizeof(DynData));
    ASSERT_FAIL(Trans_GetTrackPANData(TRAN_ICC | TRAN_ICFORLOG, &TransData));

    //从应用选择的返回FCI中判断卡片是否有交易日志入口数据元DF4D
    memset(buf, 0, sizeof(buf));
    iTagLen = EMVtrans_ReadTagData(0xDF4D, (char *)buf, sizeof(buf)) ;

    if(iTagLen <= 0)
    {
        iTagLen = 2;
        buf[0] = 0x0C;
        buf[1] = 0x0A;
    }

    AppDebug_AppLog("GET 0xDF4D Finished");
    AppDebug_AppLog("iTagLen = %d 0xDF4D = %02x %02x", iTagLen, buf[0], buf[1]);

    //从卡片上获取交易日志格式信息DF4F-直接发送GETDATA指令给卡片
    memset(cBuffer, 0, sizeof(cBuffer));
    memcpy(cBuffer, "\x80\xCA\xDF\x4F\x00", 5);
    iLen = ICCmd_SendCommand((byte *)cBuffer, 0, 1);

    //判断返回值是否
    if(iLen <= 0)
    {
        AppUtils_Warning("获取圈存日志失败");
        return BANK_FAIL;
    }

    AppDebug_AppLog("GET DF4F OK");


    //分解DF4F交易日志格式;
    AppDebug_AppLog("GET Reload LOG FORMAT BEGIN");

    //逐条读取交易日志-直接发送READ RECORD指令给卡片
    AppDebug_AppLog("READ RECORD BEGIN");

    for(i = 1; i <= buf[1]; i++)
    {
        // Read Record
        memset(cBuffer, 0, sizeof(cBuffer));

        //CLA INS
        memcpy(cBuffer, "\x00\xB2", 2);

        //P1
        cBuffer[2] = i;

        //P2
        cBuffer[3] = (buf[0] << 3) | 0x04;

        //Le
        cBuffer[4] = 0;

        iLen = ICCmd_SendCommand((byte *)cBuffer, 0, 1);

        if(iLen <= 0)
        {
            break;
        }


        usDataLen = iLen;

        AppUtils_Cls();
        AppUtils_DisplayCenter("圈存日志查询", LINE1, TRUE);

        //for(;;)
        {
            //圈存日志中的固定内容
            sprintf(SUNSONDispBuf, "记录%d", i);
            AppUtils_DisplayLine(SUNSONDispBuf, LINE2);

            sprintf(SUNSONDispBuf, "Putdata P1:%02X", cBuffer[0]);
            AppUtils_DisplayLine(SUNSONDispBuf, LINE3);

            sprintf(SUNSONDispBuf, "Putdata P2:%02X", cBuffer[1]);
            AppUtils_DisplayLine(SUNSONDispBuf, LINE4);

            bt = AppUtils_WaitKey(60);

            if(bt == bESC || bt == 0)
                break;

            AppUtils_Cls();
            AppUtils_DisplayLine("Putdata前金额:", LINE1);
            memset(Buf1, 0, sizeof(Buf1));
            AppUtils_Bcd2Asc((byte *)cBuffer + 2, (byte *)Buf1, 12);
            sprintf(SUNSONDispBuf, "%.2f", atof(Buf1) / 100.00);
            AppUtils_DisplayLine(SUNSONDispBuf, LINE2);

            AppUtils_DisplayLine("Putdata后金额:", LINE3);
            memset(Buf1, 0, sizeof(Buf1));
            AppUtils_Bcd2Asc((byte *)cBuffer + 8, (byte *)Buf1, 12);
            sprintf(SUNSONDispBuf, "%.2f", atof(Buf1) / 100.00);
            AppUtils_DisplayLine(SUNSONDispBuf, LINE4);

            bt = AppUtils_WaitKey(60);

            if(bt == bESC || bt == 0)
                break;

            AppUtils_DisplayCenter("圈存日志查询", LINE1, TRUE);
            AppUtils_UserClear(2);

            if(usDataLen > 14)   //圈存日志中的可变内容
            {
                sprintf(SUNSONDispBuf, "交易日期:%02X%02X%02X", cBuffer[14], cBuffer[15], cBuffer[16]);
                AppUtils_DisplayLine(SUNSONDispBuf, LINE2);

                sprintf(SUNSONDispBuf, "交易时间:%02X%02X%02X", cBuffer[17], cBuffer[18], cBuffer[19]);
                AppUtils_DisplayLine(SUNSONDispBuf, LINE3);

                sprintf(SUNSONDispBuf, "国家代码:%02X%02X", cBuffer[20], cBuffer[21]);
                AppUtils_DisplayLine(SUNSONDispBuf, LINE4);

                bt = AppUtils_WaitKey(60);

                if(bt == bESC || bt == 0)
                    break;

                AppUtils_UserClear(2);
                memset(Buf1, 0, sizeof(Buf1));
                iOffset = 0;
                memcpy(Buf1, &cBuffer[22], 20);
                sprintf(Buf2, "商户名称:%s", Buf1);
                memset(SUNSONDispBuf, 0, sizeof(SUNSONDispBuf));
                iOffset += BankUtil_CStrlcpy(SUNSONDispBuf, Buf2 + iOffset, 17);
                AppUtils_DisplayLine(SUNSONDispBuf, LINE2);
                memset(SUNSONDispBuf, 0, sizeof(SUNSONDispBuf));
                iOffset += BankUtil_CStrlcpy(SUNSONDispBuf, Buf2 + iOffset, 17);
                AppUtils_DisplayLine(SUNSONDispBuf, LINE3);

                sprintf(SUNSONDispBuf, "交易计数器 %02X%02X", cBuffer[usDataLen - 2], cBuffer[usDataLen - 1]);
                AppUtils_DisplayLine(SUNSONDispBuf, LINE4);

                bt = AppUtils_WaitKey(60);

                if(bt == bESC || bt == 0)
                    break;
            }
        }
    }

    return  BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashLoad
 * DESCRIPTION:   电子现金的现金充值
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashLoad(void *dummy)
{
    DynData TransData;
    int iRet;
    int iLength;
    usint usSwipeMode;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_ECASH_CASH_RELOAD;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    usSwipeMode = TRAN_ICC;

#if 0 //Modify on 2015-07-23 for 暂时不考虑非接圈存,等后续统一版本修改、详测后再做合并

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_CTLSPBOC;
        TransData.EC_CTLSPriOpt = 2;

        // 输入金额
        if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
            ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
    }

#endif

    // 插卡，获取卡号
    //////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    if(TransData.TransDB.Swipe == TRAN_ICC)
    {
        TransData.TransDB.EC_flag = 1;
    }

    // 显示卡号有效期
    ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

    // 预拨号
    Comm_CheckPreConnect();

    // EMV脱机、联机交易认证
    iRet = EMVtrans_ConfirmProcessing(&TransData);

    if(iRet == BANK_FAIL)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }
    else if(iRet == BANK_EMV_BYPASS || iRet == BANK_EMV_ONLINE)
    {
        // 输入金额
        if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
        {
            Comm_Disconnect();
            PrnDisp_DisplayWarning("", "充值交易金额不能为0");
            return BANK_FAIL;
        }

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 检测是否有脚本需要上送
        Trans_UploadICCscript(&TransData);

        // 检查冲正
        ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

        // 增加流水号
        PrmMngr_AddSystrace();

        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0200", 4);
        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
            ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 3
        memcpy(TransData.TransDB.ProcessCode, "630000", 6);
        ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

        // field 4
        ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

        // field 11
        TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 14
        if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
        }

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 25
        memcpy(TransData.TransDB.ServiceCode, "91", 2);
        ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

        // field 26
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);

        // field 35
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

        // field 36
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 49
        ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

        // field 52
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);

        // field 53
        Trans_SetField53_Data(&ReqIsoRec, &TransData);

        // field 55
        if((TransData.TransDB.Swipe == TRAN_ICC) || (TransData.TransDB.Swipe == TRAN_QPBOC))
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            iLength = EMVtrans_GetEMVICCData(&TransData, cTmpBuf);
            ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);
        }

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        //60.1消息类型
        memcpy(cTmpBuf, "46", 2);
        memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

        //60.2批次号码
        TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

        //60.3网络管理信息码
        memcpy(cTmpBuf + 8, "000", 3);

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        //0 未使用或后续子域存在，或手机芯片交易
        //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
        //2 上一笔交易虽是IC卡交易但失败
        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
            memcpy(cTmpBuf + 12, "2", 1);
        else
            memcpy(cTmpBuf + 12, "0", 1);

        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        // field 64
        //Trans_SetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        iLength += 11;

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 发送数据包
        ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

        Trans_CheckNeedDisconnect();

        // 解包
        ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

        // 保存交易流水
        if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        // IC卡联机交易卡片处理
        if(TransData.TransDB.Swipe == TRAN_ICC)
        {
            iLength = 0;
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));

            //EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);

            if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
            {
                // 重新获取电子现金余额
                ////EMVtrans_UpdateECashBalance(&TransData);

                // 更新原交易流水
                AppUtils_JournalUpdateRecord(TransData.TransDB.SysTrace, &TransData);
            }
        }
        else if(TransData.TransDB.Swipe == TRAN_QPBOC)
        {
            iLength = 0;
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));

            //CTLS_LoadValue(cBuffer, iLength, &TransData);

            if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
            {
                // 更新原交易流水
                AppUtils_JournalUpdateRecord(TransData.TransDB.SysTrace, &TransData);
            }
        }
    }

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashInLoad
 * DESCRIPTION:   电子现金的指定账户圈存
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashInLoad(void *dummy)
{
    DynData TransData;
    int iRet;
    int iLength;
    usint usSwipeMode;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_ECASH_IN_RELOAD;


    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    usSwipeMode = TRAN_ICC;

#if 0 //Modify on 2015-07-23 for 暂时不考虑非接圈存,等后续统一版本修改、详测后再做合并

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_CTLSPBOC;
        TransData.EC_CTLSPriOpt = 2;

        // 输入金额
        if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
            ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
    }

#endif

    // 刷卡/插卡/挥卡，获取卡号
    ////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    if(TransData.TransDB.Swipe == TRAN_ICC)
    {
        TransData.TransDB.EC_flag = 1;
    }

    // 显示卡号有效期
    ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

    // 预拨号
    Comm_CheckPreConnect();

    // EMV脱机、联机交易认证
    iRet = EMVtrans_ConfirmProcessing(&TransData);

    if(iRet == BANK_FAIL)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }
    else if(iRet == BANK_EMV_BYPASS || iRet == BANK_EMV_ONLINE)
    {
        // 输入金额
        if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
        {
            Comm_Disconnect();
            PrnDisp_DisplayWarning("", "充值交易金额不能为0");
            return BANK_FAIL;
        }

        // 输入卡密码
        TransData.PinRetryFlag = 0;

        while(TransData.PinExists == 0)
            ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 检测是否有脚本需要上送
        Trans_UploadICCscript(&TransData);

        // 检查冲正
        ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

        // 增加流水号
        PrmMngr_AddSystrace();


        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0200", 4);
        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 3
        memcpy(TransData.TransDB.ProcessCode, "600000", 6);
        ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

        // field 4
        ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

        // field 11
        TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 14
        if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
        }

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 25
        memcpy(TransData.TransDB.ServiceCode, "91", 2);
        ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

        // field 26
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
        else
            ISOEngine_ClearOneBit(&ReqIsoRec, 26);

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 49
        ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

        // field 52
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);
        else
            ISOEngine_ClearOneBit(&ReqIsoRec, 52);

        // field 53
        Trans_SetField53_Data(&ReqIsoRec, &TransData);

        // field 55
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        iLength = EMVtrans_GetEMVICCData(&TransData, cTmpBuf);
        ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        //60.1消息类型
        memcpy(cTmpBuf, "45", 2);
        memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

        //60.2批次号码
        TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

        //60.3网络管理信息码
        memcpy(cTmpBuf + 8, "000", 3);

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        //0 未使用或后续子域存在，或手机芯片交易
        //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
        //2 上一笔交易虽是IC卡交易但失败
        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
            memcpy(cTmpBuf + 12, "2", 1);
        else
            memcpy(cTmpBuf + 12, "0", 1);

        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        // field 64
        //Trans_SetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        iLength += 11;

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 发送数据包
        ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

        // 解包
        ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

        // 保存交易流水
        if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        Trans_CheckNeedDisconnect();

        // IC卡联机交易卡片处理
        if(TransData.TransDB.Swipe == TRAN_ICC)
        {
            iLength = 0;
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));

            //EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);

            if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
            {
                // 重新获取电子现金余额
                ////EMVtrans_UpdateECashBalance(&TransData);

                // 更新原交易流水
                AppUtils_JournalUpdateRecord(TransData.TransDB.SysTrace, &TransData);
            }
        }
        else if(TransData.TransDB.Swipe == TRAN_QPBOC)
        {
            iLength = 0;
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));

            //CTLS_LoadValue(cBuffer, iLength, &TransData);

            if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
            {
                // 更新原交易流水
                AppUtils_JournalUpdateRecord(TransData.TransDB.SysTrace, &TransData);
            }
        }
    }

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashOutLoad
 * DESCRIPTION:   电子现金的非指定账户圈存
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashOutLoad(void *dummy)
{
    DynData TransData;
    int iRet;
    int iLength;
    usint usSwipeMode;
    usint iSwipeCardMode;
    byte bEncryptedTrkFlag;
    byte cPAN[20];
    byte cPANLen;
    byte cExpDate[ 3 ];
    byte cCrd1Buf[80];
    byte cCrd2Buf[40];
    byte cCrd3Buf[108];
    byte cPOSEntry[ 3 ];
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_ECASH_OUT_RELOAD;

    memset(cPAN, 0, sizeof(cPAN));
    memset(cExpDate, 0, sizeof(cExpDate));
    memset(cCrd1Buf, 0, sizeof(cCrd1Buf));
    memset(cCrd2Buf, 0, sizeof(cCrd2Buf));
    memset(cCrd3Buf, 0, sizeof(cCrd3Buf));
    memset(cPOSEntry, 0, sizeof(cPOSEntry));

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 刷卡转出卡，获取卡号
    TransData.AddBuf[ 0 ] = 'O';
    TransData.TransDB.TrType = BTYPE_BALANCE; // 防止emv lib调用回调函数输入交易金额
    ////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(TRAN_SWIP | TRAN_ICC, &TransData));

    TransData.TransDB.TrType = BTYPE_ECASH_OUT_RELOAD;

    if(EMVtrans_ConfirmProcessing_OnlyAuthOfflineData(&TransData) != BANK_OK)
    {
        AppUtils_Warning("交易拒绝");
        EMVtrans_RemoveCard();
        return BANK_FAIL;
    }

    // 显示卡号有效期
    ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    TransData.TransDB.TrType = BTYPE_ECASH_OUT_RELOAD;

    iSwipeCardMode = TransData.TransDB.Swipe;
    bEncryptedTrkFlag = TransData.TransDB.EncryptedTrkFlag;
    cPANLen = TransData.TransDB.PANLen;
    memcpy(cPAN, TransData.TransDB.PAN, cPANLen);
    memcpy(cExpDate, TransData.TransDB.ExpDate, 3);
    memcpy(cCrd1Buf, TransData.Track1Data, strlen((char *)TransData.Track1Data));
    memcpy(cCrd2Buf, TransData.TransDB.Track2Data, strlen((char *)TransData.TransDB.Track2Data));
    memcpy(cCrd3Buf, TransData.TransDB.Track3Data, strlen((char *)TransData.TransDB.Track3Data));
    memcpy(cPOSEntry, TransData.TransDB.POSEntry, 2);

    // 插入转入卡，获取卡号
    TransData.AddBuf[ 0 ] = 0;

    usSwipeMode = TRAN_ICC;

#if 0 //Modify on 2015-07-23 for 暂时不考虑非接圈存,等后续统一版本修改、详测后再做合并

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_CTLSPBOC;
        TransData.EC_CTLSPriOpt = 2;

        // 输入金额
        if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
            ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
    }

#endif

    ////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    if(TransData.TransDB.Swipe == TRAN_ICC)
    {
        TransData.TransDB.EC_flag = 1;
    }

    // 显示卡号有效期
    ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));

    TransData.TransDB.PAN_2Len = TransData.TransDB.PANLen;
    memcpy(TransData.TransDB.PAN_2, TransData.TransDB.PAN, TransData.TransDB.PAN_2Len);

    memset(TransData.TransDB.PAN, 0, sizeof(TransData.TransDB.PAN));
    memset(TransData.Track1Data, 0, sizeof(TransData.Track1Data));
    memset(TransData.TransDB.Track2Data, 0, sizeof(TransData.TransDB.Track2Data));
    memset(TransData.TransDB.Track3Data, 0, sizeof(TransData.TransDB.Track3Data));

    TransData.TransDB.EncryptedTrkFlag = bEncryptedTrkFlag;
    TransData.TransDB.PANLen = cPANLen;
    memcpy(TransData.TransDB.PAN, cPAN, cPANLen);
    memcpy(TransData.TransDB.ExpDate, cExpDate, 3);
    memcpy(TransData.Track1Data, cCrd1Buf, strlen((char *)cCrd1Buf));
    memcpy(TransData.TransDB.Track2Data, cCrd2Buf, strlen((char *)cCrd2Buf));
    memcpy(TransData.TransDB.Track3Data, cCrd3Buf, strlen((char *)cCrd3Buf));
    memcpy(TransData.TransDB.POSEntry, cPOSEntry, 2);

    // 预拨号
    Comm_CheckPreConnect();

    // EMV脱机、联机交易认证
    iRet = EMVtrans_ConfirmProcessing(&TransData);

    if(iRet == BANK_FAIL)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }
    else if(iRet == BANK_EMV_BYPASS || iRet == BANK_EMV_ONLINE)
    {
        // 输入金额
        if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
        {
            Comm_Disconnect();
            PrnDisp_DisplayWarning("", "充值交易金额不能为0");
            return BANK_FAIL;
        }

        // 输入转出卡密码
        TransData.PinRetryFlag = 0;
        ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 检测是否有脚本需要上送
        Trans_UploadICCscript(&TransData);

        // 检查冲正
        ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

        // 增加流水号
        PrmMngr_AddSystrace();

        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0200", 4);
        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        if((iSwipeCardMode != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
            ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 3
        memcpy(TransData.TransDB.ProcessCode, "620000", 6);
        ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

        // field 4
        ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

        // field 11
        TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 14
        if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
        }

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 25
        memcpy(TransData.TransDB.ServiceCode, "91", 2);
        ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

        // field 26
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
        else
            ISOEngine_ClearOneBit(&ReqIsoRec, 26);

        if(iSwipeCardMode == TRAN_SWIP)
        {
            // field 35
            if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
                ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

            // field 36
            if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
                ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));
        }

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 48
        ISOEngine_SetBit(&ReqIsoRec, 48, (unsigned char *)"0520", 4);

        // field 49
        ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

        // field 52
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);
        else
            ISOEngine_ClearOneBit(&ReqIsoRec, 52);

        // field 53
        Trans_SetField53_Data(&ReqIsoRec, &TransData);

        // field 55
        if((TransData.TransDB.Swipe == TRAN_ICC) || (TransData.TransDB.Swipe == TRAN_QPBOC))
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            iLength = EMVtrans_GetEMVICCData(&TransData, cTmpBuf);
            ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);
        }

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        //60.1 消息类型
        memcpy(cTmpBuf, "47", 2);

        //60.2批次号码
        TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

        //60.3网络管理信息码
        memcpy(cTmpBuf + 8, "000", 3);

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        //0 未使用或后续子域存在，或手机芯片交易
        //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
        //2 上一笔交易虽是IC卡交易但失败
        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
            memcpy(cTmpBuf + 12, "2", 1);
        else
            memcpy(cTmpBuf + 12, "0", 1);

        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        // field 62
        ISOEngine_SetBit(&ReqIsoRec, 62, TransData.TransDB.PAN_2, TransData.TransDB.PAN_2Len);

        // field 64
        //Trans_SetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        iLength += 11;

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 发送数据包
        ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

        // 解包
        ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

        // 保存交易流水
        if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }


        Trans_CheckNeedDisconnect();

        // IC卡联机交易卡片处理
        if(TransData.TransDB.Swipe == TRAN_ICC)
        {
            iLength = 0;
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));

            //EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);

            if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
            {
                // 重新获取电子现金余额
                ////EMVtrans_UpdateECashBalance(&TransData);

                // 更新原交易流水
                AppUtils_JournalUpdateRecord(TransData.TransDB.SysTrace, &TransData);
            }
        }
        else if(TransData.TransDB.Swipe == TRAN_QPBOC)
        {
            iLength = 0;
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));

            //CTLS_LoadValue(cBuffer, iLength, &TransData);

            if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
            {
                // 更新原交易流水
                AppUtils_JournalUpdateRecord(TransData.TransDB.SysTrace, &TransData);
            }
        }
    }

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashVoidLoad
 * DESCRIPTION:   电子现金的现金充值撤销
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashVoidLoad(void *dummy)
{
    DynData TransData;
    int iLength;
    usint usSwipeMode;
    int iRet;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_ECASH_CASH_RELOAD_VOID;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 主管密码
    if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
        return BANK_FAIL;

    // 获取原交易数据
    ASSERT_FAIL(Trans_GetOrignalJournal("请输入原凭证号:", &TransData));

    // 检查原交易状态
    if(TransData.TransDB.TrType != BTYPE_ECASH_CASH_RELOAD)
    {
        PrnDisp_DisplayWarning("", "原交易不是现金充值!");
        return BANK_FAIL;
    }

    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_VOID) == BANK_OK)
    {
        PrnDisp_DisplayWarning("", "该交易已经撤销!");
        return BANK_FAIL;
    }

    // 显示原交易相关信息
    while(TRUE)
    {
        iRet = PrnDisp_DisplayTxnDetail(&TransData, BTYPE_ECASH_CASH_RELOAD_VOID);

        if(iRet == bESC)
            return BANK_FAIL;
        else if(iRet == bENTER)
            break;
    }

    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(BTYPE_ECASH_CASH_RELOAD_VOID);

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    memcpy(cTmpBuf, TransData.TransDB.PAN, TransData.TransDB.PANLen);

    TransData.TransDB.TrType = BTYPE_ECASH_CASH_RELOAD_VOID;

    usSwipeMode = TRAN_ICC;

#if 0 //Modify on 2015-07-23 for 暂时不考虑非接圈存,等后续统一版本修改、详测后再做合并

    if(gTermCfg.mTrSwitch.CTLS_Support)
    {
        usSwipeMode |= TRAN_CTLSPBOC;
        TransData.EC_CTLSPriOpt = 2;
    }

#endif

    // 插卡，获取卡号
    ////ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));

    //匹配原卡号是否匹配
    iLength = strlen((char *)cTmpBuf);

    if(TransData.TransDB.PANLen != iLength)
    {
        AppUtils_Warning("卡号不匹配");
        return BANK_FAIL;
    }

    if(memcmp(cTmpBuf, TransData.TransDB.PAN, iLength) != 0)
    {
        AppUtils_Warning("卡号不匹配!");
        return BANK_FAIL;
    }

    if(TransData.TransDB.Swipe == TRAN_ICC)
    {
        TransData.TransDB.EC_flag = 1;
    }

    // 显示卡号有效期
    TransData.TransDB.EncryptedTrkFlag = 0;
    ASSERT_FAIL(Trans_ShowCardPAN(&TransData));

    // 预拨号
    Comm_CheckPreConnect();

    // EMV脱机、联机交易认证
    iRet = EMVtrans_ConfirmProcessing(&TransData);

    if(iRet == BANK_FAIL)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }
    else if(iRet == BANK_EMV_BYPASS || iRet == BANK_EMV_ONLINE)
    {
        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 检测是否有脚本需要上送
        Trans_UploadICCscript(&TransData);

        // 检查冲正
        ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

        // 增加流水号
        PrmMngr_AddSystrace();

        // 复制原交易相关信息
        TransData.TransDB.OrgTranType = TransData.TransDB.TrType;
        TransData.TransDB.TrType = BTYPE_ECASH_CASH_RELOAD_VOID;
        TransData.PinExists = 0;
        TransData.TransDB.OrgSysTrace = TransData.TransDB.SysTrace;
        TransData.TransDB.OrgBatchNum = TransData.TransDB.BatchNum;
        memcpy(TransData.TransDB.OrgAuthCode, TransData.TransDB.AuthCode, 6);
        memcpy(TransData.TransDB.OrgRefCode, TransData.TransDB.RefCode, 12);
        AppUtils_Bcd2Asc(TransData.TransDB.TranDate + 2, TransData.TransDB.OrgTranDate, 4);
        memset(TransData.TransDB.RspCode, 0, sizeof(TransData.TransDB.RspCode));

        //清除原交易中的TC上送标志：撤销交易不需要上送TC
        AppUtils_ResetStatus(&TransData.TransDB.Status[0], STATUS_TCUPLOAD);

        TransData.PinRetryFlag = 0;

        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0200", 4);
        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        if(TransData.TransDB.PANLen > 0)
            ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 3
        memcpy(TransData.TransDB.ProcessCode, "170000", 6);
        ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

        // field 4
        ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

        // field 11
        TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 14
        if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
        }

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 25
        memcpy(TransData.TransDB.ServiceCode, "91", 2);
        ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

        // field 35
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

        // field 36
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

        // field 37
        ISOEngine_SetBit(&ReqIsoRec, 37, TransData.TransDB.OrgRefCode, 12);

        // field 38
        if(AppUtils_IsNullData((char *) TransData.TransDB.OrgAuthCode, strlen((char *) TransData.TransDB.OrgAuthCode)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 38, TransData.TransDB.OrgAuthCode, 6);

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 49
        ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

        // field 53
        Trans_SetField53_Data(&ReqIsoRec, &TransData);

        // field 55
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        //iLength = EMVtrans_GetEMVIccTCUpload(&TransData, cTmpBuf);
        ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        //60.1
        memcpy(cTmpBuf, "51", 2);

        //60.2
        TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

        //60.3
        memcpy(cTmpBuf + 8, "000", 3);

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        //0 未使用或后续子域存在，或手机芯片交易
        //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
        //2 上一笔交易虽是IC卡交易但失败
        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
            memcpy(cTmpBuf + 12, "2", 1);
        else
            memcpy(cTmpBuf + 12, "0", 1);

        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        // field 61
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        sprintf((char *)cTmpBuf, "%06ld", TransData.TransDB.OrgBatchNum);
        sprintf((char *)cTmpBuf + 6, "%06ld", TransData.TransDB.OrgSysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));

        // field 64
        //Trans_SetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        iLength += 11;

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 保存本交易冲正记录
        ASSERT_HANGUP_FAIL(Trans_SetReversal(&TransData, cBuffer, iLength));

        // 发送数据包
        ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

        Trans_CheckNeedDisconnect();

        // 解包
        ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

        // 清除冲正标识
        ASSERT_HANGUP_FAIL(Trans_ClearReversal(REVERSAL_CLEAR_STATUS));

        // 保存交易流水
        if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
        {
            Comm_Disconnect();

            Trans_ResetReversalStatus(&TransData);
            Trans_UpdateReversalFieldData(39, "06", 2);
            return BANK_FAIL;
        }
    }
    else
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    // IC卡联机交易卡片处理
    if(TransData.TransDB.Swipe == TRAN_ICC)
    {
        iLength = 0;
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));

        //EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
    }
    else if(TransData.TransDB.Swipe == TRAN_QPBOC)
    {
        iLength = 0;
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));
        //CTLS_LoadValue(cBuffer, iLength, &TransData);
    }

    // 清除冲正标识和流水
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);

    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 更改原交易流水为已撤销
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        AppUtils_JournalUpdateRecordByStatus(TransData.TransDB.OrgSysTrace, 0, STATUS_VOID);

    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_EchoTesting
 * DESCRIPTION:   回响测试
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_EchoTesting(void *dummy)
{
    DynData TransData;
    int iLength;
    byte cTmpBuf[ 126 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_ECHO_TESTING;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 检测是否有脚本需要上送
    Trans_UploadICCscript(&TransData);

    // 检查冲正
    ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

    // 增加流水号 (本交易报文未使用流水号，流水号不变亦可? by XC 2014/12/3 11:48:59)
    PrmMngr_AddSystrace();

    // 打包
    ISOEngine_ClearAllBits(&ReqIsoRec);
    ISOEngine_ClearAllBits(&RspIsoRec);
    ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0820", 4);
    ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 41
    ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    //60.1
    memcpy(cTmpBuf, "00", 2);

    //60.2
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3
    memcpy(cTmpBuf + 8, "301", 3);
    ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // 组包转换
    memset(cBuffer, 0x00, sizeof(cBuffer));

    // TPDU
    memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

    // CUP Header
    memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

    iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

    if(iLength <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    iLength += 11;

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 发送数据包
    ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

    // 接收数据包
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = 0;

    ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

    Trans_CheckNeedDisconnect();

    // 解包
    ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

    // 显示处理错误代码
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
    {
        AppUtils_UserClear(LINE2);
        AppUtils_DisplayCenter("交易成功", LINE2, FALSE);
        AppUtils_WaitKey(3);
    }
    else
    {
        PrnDisp_DisplayRespCode(&TransData);
    }

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();
    return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_UploadPOSStatus
 * DESCRIPTION:   POS状态传递
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_UploadPOSStatus(void *dummy)
{
    DynData TransData;
    int iLength;
    byte cTmpBuf[ 256 ];
    byte *p;
    byte cClock[ 16 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_UPLOAD_STATUS;

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 检测终端各自状态
    ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 增加流水号  (本交易报文未使用流水号，流水号不变亦可? by XC 2014/12/3 11:48:59)
    PrmMngr_AddSystrace();

    // 打包
    ISOEngine_ClearAllBits(&ReqIsoRec);
    ISOEngine_ClearAllBits(&RspIsoRec);
    ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0820", 4);
    ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 41
    ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    //60.1
    memcpy(cTmpBuf, "00", 2);

    //60.2
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3
    memcpy(cTmpBuf + 8, "362", 3);
    ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // field 62
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    p = cTmpBuf;

    sprintf((char *)p, "011021031041051116012%02d", gTermCfg.mCommPrm.TelTimeout);
    p += 23;

    memcpy(p, "13", 2);
    p += 2;
    *p ++ = gTermCfg.mCommPrm.Retrys + 0x30;

    memcpy(p, "14", 2);
    p += 2;
    iLength = strlen((char *)gTermCfg.mCommPrm.TelPhone[ 0 ]);

    if(iLength > 14)
        iLength = 14;

    memset(p, ' ', 14);
    memcpy(p, gTermCfg.mCommPrm.TelPhone[ 0 ], iLength);
    p += 14;

    memcpy(p, "15", 2);
    p += 2;
    iLength = strlen((char *)gTermCfg.mCommPrm.TelPhone[ 1 ]);

    if(iLength > 14)
        iLength = 14;

    memset(p, ' ', 14);
    memcpy(p, gTermCfg.mCommPrm.TelPhone[ 1 ], iLength);
    p += 14;

    memcpy(p, "16", 2);
    p += 2;
    iLength = strlen((char *)gTermCfg.mCommPrm.TelPhone[ 2 ]);

    if(iLength > 14)
        iLength = 14;

    memset(p, ' ', 14);
    memcpy(p, gTermCfg.mCommPrm.TelPhone[ 2 ], iLength);
    p += 14;

    memcpy(p, "17", 2);
    p += 2;
    iLength = strlen((char *)gTermCfg.mFixPrm.ManageTelNo);

    if(iLength > 14)
        iLength = 14;

    memset(p, ' ', 14);
    memcpy(p, gTermCfg.mFixPrm.ManageTelNo, iLength);
    p += 14;

    memcpy(p, "18", 2);
    p += 2;
    *p ++ = gTermCfg.mTrSwitch.FeeFlag + 0x30;

    memcpy(p, "19", 2);
    p += 2;
    sprintf((char *)p, "%02x", gTermCfg.mFixPrm.FeePercent);
    p += 2;

    memcpy(p, "20", 2);
    p += 2;
    *p ++ = gTermCfg.mTrSwitch.ManualFlag + 0x30;

    memcpy(p, "21", 2);
    p += 2;
    *p ++ = gTermCfg.mTrSwitch.AutoLogoutFlag + 0x30;

    memcpy(p, "23", 2);
    p += 2;
    *p ++ = gTermCfg.mFixPrm.ReversalRetryTimes + 0x30;

#if 0
    memcpy(p, "24", 2);
    p += 2;
    *p ++ = gTermCfg.mFixPrm.OfflineUploadMode + 0x30;
#endif

    memcpy(p, "25", 2);
    p += 2;
    *p ++ = gTermCfg.mFixPrm.KeyIndex + 0x30;

    memcpy(p, "27", 2);
    p += 2;
    sprintf((char *)p, "%02d", gTermCfg.mFixPrm.OffUploadNumLimit);
    p += 2;

    memcpy(p, "51", 2);
    p += 2;
    iLength = 0;

    if(AppUtils_IsNullData((char *)gTermCfg.mDynPrm.StartUsingTime, sizeof(gTermCfg.mDynPrm.StartUsingTime)) != BANK_OK)
    {
        memset(cClock, 0, sizeof(cClock));

        if(AppUtils_GetClock((char *)cClock) == BANK_OK)
        {
            if(memcmp(gTermCfg.mDynPrm.StartUsingTime, cClock, 4))
                iLength = 24 * 365;

            if(memcmp(gTermCfg.mDynPrm.StartUsingTime + 4, cClock + 4, 2))
                iLength += (((cClock[ 4 ] - gTermCfg.mDynPrm.StartUsingTime[ 4 ]) * 10 + (cClock[ 5 ] - gTermCfg.mDynPrm.StartUsingTime[ 5 ])) * 24 * 30);

            if(memcmp(gTermCfg.mDynPrm.StartUsingTime + 6, cClock + 6, 2))
                iLength += ((cClock[ 6 ] - gTermCfg.mDynPrm.StartUsingTime[ 6 ]) * 10 + (cClock[ 7 ] - gTermCfg.mDynPrm.StartUsingTime[ 7 ])) * 24;

            if(memcmp(gTermCfg.mDynPrm.StartUsingTime + 8, cClock + 8, 2))
                iLength += ((cClock[ 8 ] - gTermCfg.mDynPrm.StartUsingTime[ 8 ]) * 10 + (cClock[ 9 ] - gTermCfg.mDynPrm.StartUsingTime[ 9 ]));

            AppDebug_AppLog("start using time=[%s],clock=[%s],hounr=[%d]", gTermCfg.mDynPrm.StartUsingTime, cClock, iLength);

            if((iLength > 10000) || (iLength < 0))
                iLength = 0;
        }
    }

    sprintf((char *)p, "%04d%05ld%03ld", iLength, gTermCfg.mDynPrm.DialTimes, (gTermCfg.mDynPrm.DialSuccTimes * 100 / gTermCfg.mDynPrm.DialTimes));
    p += 12;

    iLength = p - cTmpBuf;
    ISOEngine_SetBit(&ReqIsoRec, 62, cTmpBuf, iLength);

    // 组包转换
    memset(cBuffer, 0x00, sizeof(cBuffer));

    // TPDU
    memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

    // CUP Header
    memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

    iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

    if(iLength <= 0)
    {
        Comm_Disconnect();
        return BANK_FAIL;
    }

    iLength += 11;

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 发送数据包
    ASSERT_FAIL(Comm_SendPacket(cBuffer, iLength));

    // 接收数据包
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = 0;

    ASSERT_FAIL(Comm_RecvPacket(cBuffer, &iLength));

    Trans_CheckNeedDisconnect();

    // 解包
    ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

    // 显示处理错误代码
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
    {
        if(gTermCfg.mDynPrm.NextEvent == 0x02)
        {
            gTermCfg.mDynPrm.NextEvent = 0;
            PrmMngr_SavePrmToSamDisk();
        }

        if(AppUtils_GetClock((char *)cTmpBuf) == BANK_OK)
            memcpy(gTermCfg.mDynPrm.StartUsingTime, cTmpBuf, 10);

        gTermCfg.mDynPrm.DialTimes = 0;
        gTermCfg.mDynPrm.DialSuccTimes = 0;
        PrmMngr_SavePrmToSamDisk();

        if(dummy == (void*)0)
        {
            AppUtils_UserClear(LINE2);
            AppUtils_DisplayCenter("交易成功", LINE2, FALSE);
            AppUtils_WaitKey(3);
        }
    }
    else
    {
        PrnDisp_DisplayRespCode(&TransData);
    }

    // 挂线
    Comm_Disconnect();
    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_GetTrackPANData
 * DESCRIPTION:   读卡主函数，支持多界面
 * PARAMETERS:    usSwipeMode: 刷卡方式，pTxnData: 交易数据结构指针
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_GetTrackPANData(unsigned short usEventMode, DynData *pTxnData)
{
    int iLine;
    unsigned char ucTrack1[32];
    unsigned char ucTrack2[64];
    unsigned char ucTrack3[128];
    unsigned char cardno[20];

    pTxnData->TransDB.Swipe = TRAN_SWIP;

    iLine = 2;
    AppUtils_DisplayLine("请插/刷/挥卡", iLine);
    CardReader::getInstance()->Qt_GetTrack(ucTrack1, ucTrack2, ucTrack3, cardno);
       qDebug("card:%s", cardno);
    qDebug("%s|%|%s", ucTrack1,ucTrack2, ucTrack3);


    //strcpy(pTxnData->TransDB.Track1Data, ucTrack1);

    strcpy((char *)pTxnData->TransDB.Track2Data, (char *)ucTrack2);
    strcpy((char *)pTxnData->TransDB.Track3Data, (char *)ucTrack3);
    strcpy((char *)pTxnData->TransDB.PAN, (char *)cardno);
    pTxnData->TransDB.PANLen  = strlen((char *)cardno);

    return 0;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_CheckMagneticCard
 * DESCRIPTION:   检测磁条卡
 * PARAMETERS:    usSwipeMode: 刷卡方式，pTxnData: 交易数据结构指针
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_CheckMagneticCard(unsigned short usSwipeMode, DynData *pTxnData)
{
    int iRet;
    int iHandInputFlag = 0;
    int iTrk1Len, iTrk2Len, iTrk3Len;
    int iPANLen, iExpMonth;
    byte cTrk1Buffer[ 126 ];
    byte cTrk2Buffer[ 126 ];
    byte cTrk3Buffer[ 126 ];
    byte cPANnum[ 21 ];
    byte cExpDate[ 4 + 1 ];

    if(AppUtils_CheckUshortStatus(usSwipeMode, TRAN_NUM) == BANK_OK)
        iHandInputFlag = 1;

    if(AppUtils_CheckStatus(pTxnData->TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
        iHandInputFlag = 0;

    memset(cTrk1Buffer, 0, sizeof(cTrk1Buffer));
    memset(cTrk2Buffer, 0, sizeof(cTrk2Buffer));
    memset(cTrk3Buffer, 0, sizeof(cTrk3Buffer));
    memset(cPANnum, 0, sizeof(cPANnum));

    iRet = AppUtils_GetTrack2(cTrk1Buffer, cTrk2Buffer, cTrk3Buffer, cPANnum, iHandInputFlag);

    AppDebug_AppLog("AppUtils_GetTrack2 iRet=[%d]", iRet);

    if(iRet == 0)
    {
        if(AppUtils_CheckUshortStatus(usSwipeMode, TRAN_SWIP) != BANK_OK)
        {
            AppUtils_FlushCard();
            AppUtils_FlushKbd();
            MmiUtil_StartTrack();
            return(BANK_INVALIDE);
        }

        iTrk1Len = strlen((char *) cTrk1Buffer);
        iTrk2Len = strlen((char *) cTrk2Buffer);
        iTrk3Len = strlen((char *) cTrk3Buffer);
        iPANLen = strlen((char *) cPANnum);

        AppDebug_AppLog("org track1=[%d,%s]", iTrk1Len, cTrk1Buffer);
        AppDebug_AppLog("org track2=[%d,%s]", iTrk2Len, cTrk2Buffer);
        AppDebug_AppLog("org track3=[%d,%s]", iTrk3Len, cTrk3Buffer);

        if(cTrk2Buffer[ iTrk2Len - 1 ] == 'f')
        {
            cTrk2Buffer[ iTrk2Len - 1 ] = 0x0;
            iTrk2Len --;
        }

        if(cTrk3Buffer[ iTrk3Len - 1 ] == 'f')
        {
            cTrk3Buffer[ iTrk3Len - 1 ] = 0x0;
            iTrk3Len --;
        }

        if(iTrk2Len <= 19 || iTrk2Len > 37)
        {
            AppDebug_AppLog("track2 data length=[%d] is invalide", iTrk2Len);
            AppDebug_AppLog("get track2=[%d,%s]", iTrk2Len, cTrk2Buffer);

            AppUtils_FlushCard();
            AppUtils_FlushKbd();
            MmiUtil_StartTrack();

            if(AppUtils_CheckUshortStatus(usSwipeMode, TRAN_IDLECARD) == BANK_OK)
            {
                AppUtils_UserClear(LINE1);
                PrnDisp_DisplayTransName(pTxnData->TransDB.TrType);
            }

            AppUtils_DisplayLine("刷卡错误", LINE2);
            AppUtils_DisplayLine("请继续刷卡", LINE3);
            AppUtils_DisplayLine("或按取消键退出", LINE4);
            return(BANK_INVALIDE);
        }

        if(iTrk2Len > 1 && cTrk2Buffer[ iTrk2Len-1 ] == 'f')
        {

            cTrk2Buffer[ iTrk2Len-1 ] = 0x00;
            iTrk2Len --;
        }

        if(iTrk3Len > 1 && cTrk3Buffer[ iTrk3Len-1 ] == 'f')
        {

            cTrk3Buffer[ iTrk3Len-1 ] = 0x00;
            iTrk3Len --;
        }

        AppDebug_AppLog("get track1=[%d,%s]", iTrk1Len, cTrk1Buffer);
        AppDebug_AppLog("get track2=[%d,%s]", iTrk2Len, cTrk2Buffer);
        AppDebug_AppLog("get track3=[%d,%s]", iTrk3Len, cTrk3Buffer);

        pTxnData->TransDB.PANLen = 0;
        memset(pTxnData->TransDB.PAN, 0, sizeof(pTxnData->TransDB.PAN));
        memset(pTxnData->Track1Data, 0, sizeof(pTxnData->Track1Data));
        memset(pTxnData->TransDB.Track2Data, 0, sizeof(pTxnData->TransDB.Track2Data));
        memset(pTxnData->TransDB.Track3Data, 0, sizeof(pTxnData->TransDB.Track3Data));

        if(iTrk1Len > sizeof(pTxnData->Track1Data))
            iTrk1Len = sizeof(pTxnData->Track1Data);

        if(iTrk2Len > sizeof(pTxnData->TransDB.Track2Data))
            iTrk2Len = sizeof(pTxnData->TransDB.Track2Data);

        if(iTrk3Len > sizeof(pTxnData->TransDB.Track3Data))
            iTrk3Len = sizeof(pTxnData->TransDB.Track3Data);

        if(iPANLen > sizeof(pTxnData->TransDB.PAN))
            iPANLen = sizeof(pTxnData->TransDB.PAN);

        if(iTrk2Len > 0)
        {
            if(iTrk1Len > 0)
                memcpy(pTxnData->Track1Data, cTrk1Buffer, iTrk1Len);

            if(iTrk2Len > 0)
                memcpy(pTxnData->TransDB.Track2Data, cTrk2Buffer, iTrk2Len);

            if(iTrk3Len > 0)
                memcpy(pTxnData->TransDB.Track3Data, cTrk3Buffer, iTrk3Len);

            if(Trans_GetPANExpDatebyTrack2(pTxnData) == BANK_OK)
            {
                pTxnData->TransDB.Swipe = TRAN_SWIP;

                memcpy(pTxnData->TransDB.POSEntry, "\x02\x20", 2);

                AppUtils_FlushCard();
                AppUtils_FlushKbd();
                return BANK_OK;
            }
        }

        AppUtils_FlushCard();
        AppUtils_FlushKbd();
        MmiUtil_StartTrack();

        if(AppUtils_CheckUshortStatus(usSwipeMode, TRAN_IDLECARD) == BANK_OK)
        {
            AppUtils_UserClear(LINE1);
            PrnDisp_DisplayTransName(pTxnData->TransDB.TrType);
        }

        AppUtils_DisplayLine("刷卡错误", LINE2);
        AppUtils_DisplayLine("请继续刷卡", LINE3);
        AppUtils_DisplayLine("或按取消键退出", LINE4);

        return(BANK_INVALIDE);
    }
    else if(iRet == 1)
    {
        if(AppUtils_CheckUshortStatus(usSwipeMode, TRAN_NUM) != BANK_OK)
        {
            AppUtils_FlushCard();
            AppUtils_FlushKbd();
            return(BANK_INVALIDE);
        }

        iPANLen = strlen((char *) cPANnum);

        //越界判断，iPANLen不允许超过TransDB.PAN的缓冲区大小(含结束符)
        if(iPANLen > sizeof(pTxnData->TransDB.PAN) - 1)
            iPANLen = sizeof(pTxnData->TransDB.PAN) - 1;

        if(iPANLen > 0)
        {
            pTxnData->TransDB.PANLen = iPANLen;

            //先清一下原来的数据 by XC 2015/2/10 11:48:04
            memset(pTxnData->TransDB.PAN, 0, sizeof(pTxnData->TransDB.PAN));
            memcpy(pTxnData->TransDB.PAN, cPANnum, iPANLen);

            while(TRUE)
            {
                memset(cExpDate, 0, sizeof(cExpDate));

                if(AppUtils_GetNum(LINE2, "请输入卡有效期: (年年月月)", (char *)cExpDate, 0, 4, 60) < 0)
                    return BANK_FAIL;

                if(strlen((char *)cExpDate) == 0)      //有效期不填
                    break;
                else if(strlen((char *)cExpDate) != 4)
                    continue;

                iExpMonth = atoi((char *)(cExpDate + 2));

                if((iExpMonth >= 1) && (iExpMonth <= 12))
                    break;
            }

            if(strlen((char *)cExpDate) > 0)      //输入了有效期才需要给世纪赋值
            {
                pTxnData->TransDB.ExpDate[ 0 ] = 0x20;
                AppUtils_Asc2Bcd(cExpDate, pTxnData->TransDB.ExpDate + 1, 4);
            }

            pTxnData->TransDB.Swipe = TRAN_NUM;
            memcpy(pTxnData->TransDB.POSEntry, "\x01\x20", 2);

            AppUtils_FlushCard();
            AppUtils_FlushKbd();
            return BANK_OK;
        }
        else
        {
            AppUtils_FlushCard();
            AppUtils_FlushKbd();
            return BANK_FAIL;
        }
    }
    else if(iRet == 9)
    {
        //如果是待机界面入口进来的，直接返回出去
        if(AppUtils_CheckUshortStatus(usSwipeMode, TRAN_IDLECARD) == BANK_OK)
        {
            return BANK_FAIL;
        }

        return(BANK_CONTINUE);
    }
    else if(iRet == 0)
    {
        AppUtils_FlushCard();
        AppUtils_FlushKbd();
        MmiUtil_StartTrack();

        //检测到刷卡失败，但本界面并不需要检测刷卡，直接返回。 by XC 2014/11/10 15:47:42
        if(AppUtils_CheckUshortStatus(usSwipeMode, TRAN_SWIP) != BANK_OK)
        {
            return(BANK_INVALIDE);
        }

        if(AppUtils_CheckUshortStatus(usSwipeMode, TRAN_IDLECARD) == BANK_OK)
        {
            return(BANK_FAIL);
//      AppUtils_UserClear(LINE1);
//      PrnDisp_DisplayTransName(pTxnData->TransDB.TrType);
        }

        AppUtils_DisplayLine("刷卡错误", LINE2);
        AppUtils_DisplayLine("请继续刷卡", LINE3);
        AppUtils_DisplayLine("或按取消键退出", LINE4);
        return(BANK_INVALIDE);
    }
    else
    {
        return BANK_FAIL;
    }
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_GetPANExpDatebyTrack2
 * DESCRIPTION:   从磁道2中解析有效期
 * PARAMETERS:    pTxnData: 交易数据结构指针
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_GetPANExpDatebyTrack2(DynData *pTxnData)
{
    int i;
    int iTrk2Len;

    iTrk2Len = strlen((char *) pTxnData->TransDB.Track2Data);

    for(i = 0; i < iTrk2Len; i ++)
    {
        if((pTxnData->TransDB.Track2Data[ i ] == 0x3d) || (pTxnData->TransDB.Track2Data[ i ] == 'D') ||
           (pTxnData->TransDB.Track2Data[ i ] == 'd') || (pTxnData->TransDB.Track2Data[ i ] == '='))
        {
            break;
        }
    }

    if(i >= iTrk2Len)
    {
        AppDebug_AppLog("get pan & expdate from track2 fail, i=[%d], trk2len=[%d]", i, iTrk2Len);
        return BANK_FAIL;
    }

    if(pTxnData->TransDB.PANLen <= 0)
    {
        memset(pTxnData->TransDB.PAN, 0, sizeof(pTxnData->TransDB.PAN));

        if(i <= 8)        //小于等于8位卡号
        {
            AppDebug_AppLog("get pan len < 8 fail, i=[%d]", i);
            return BANK_FAIL;
        }
        else if(i <= 19)
            memcpy(pTxnData->TransDB.PAN, pTxnData->TransDB.Track2Data, i);
        else              //大于19位卡号
        {
            AppDebug_AppLog("get pan len > 19 fail, i=[%d]", i);
            return BANK_FAIL;
        }

        pTxnData->TransDB.PANLen = i;
    }

    //不解析磁道信息内的有效期
#if 0

    if(i < sizeof(pTxnData->TransDB.Track2Data) - 5)
    {
        i ++;
        pTxnData->TransDB.ExpDate[ 0 ] = 0x20;
        AppUtils_Asc2Bcd(pTxnData->TransDB.Track2Data + i, pTxnData->TransDB.ExpDate + 1, 4);
    }

#endif

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_CheckServiceCode
 * DESCRIPTION:   检测磁道信息中的服务条件码
 * PARAMETERS:    pTxnData: 交易数据
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_CheckServiceCode(DynData *pTxnData)
{
    int i, j;
    int iTrk2Len;
    char cServiceCode[ 4 ];

    if(pTxnData->TransDB.Swipe == TRAN_NUM)
        return BANK_OK;

    iTrk2Len = strlen((char *) pTxnData->TransDB.Track2Data);

    for(i = 0; i < iTrk2Len; i ++)
    {
        if((pTxnData->TransDB.Track2Data[ i ] == 0x3d) || (pTxnData->TransDB.Track2Data[ i ] == 'D') ||
           (pTxnData->TransDB.Track2Data[ i ] == 'd') || (pTxnData->TransDB.Track2Data[ i ] == '='))
            break;
    }

    if(i >= iTrk2Len)
    {

        AppDebug_AppLog("check servvice code from track2 fail, i=[%d], trk2len=[%d]", i, iTrk2Len);
        return BANK_OK;
    }

    i ++;
    i += 4;

    memset(cServiceCode, 0, sizeof(cServiceCode));

    for(j = 0 ; j < 3; j ++, i ++)
    {
        if(((pTxnData->TransDB.Track2Data[ i ] < '0') || (pTxnData->TransDB.Track2Data[ i ] > '9')) &&
           (pTxnData->TransDB.Track2Data[ i ] != '?'))
        {
            AppDebug_AppLog("service code data invalid,i=[%d], track2data=[%c]", i, pTxnData->TransDB.Track2Data[ i ]);
            return BANK_OK;
        }
        else
            cServiceCode[ j ] = pTxnData->TransDB.Track2Data[ i ];
    }

    if(cServiceCode[ 0 ] == '2' || cServiceCode[ 0 ] == '6')
    {
        // 强制可以使用磁条卡刷卡模式
        if(pTxnData->AddBuf[ 200 ] == 'M')
            return BANK_OK;
        else
            return BANK_EMV_ICC;
    }

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ShowCardPAN
 * DESCRIPTION:   处理磁道加密，显示卡号
 * PARAMETERS:    pTxnData: 交易数据
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ShowCardPAN(DynData *pTxnData)
{
    int iLine = LINE2;
    char cBuffer[ 32+1 ];
    byte bkey;

    AppDebug_AppLog("EMVtrans_PreProcessing, start to show card pan...");

    if(pTxnData->TransDB.Swipe == TRAN_QPBOC && pTxnData->TransDB.qPBOC_OnlineFlag == 0)
        return BANK_OK;

    if(pTxnData->TransDB.Swipe == TRAN_NOCARD || pTxnData->TransDB.Swipe == TRAN_NUM)
        return BANK_OK;

    //此处进行磁道加密处理, 在等待按键前进行, 操作者完全感觉不到迟滞
    if(gTermCfg.mTrSwitch.TrackEncryptFlag && pTxnData->TransDB.EncryptedTrkFlag == 0)
    {
        byte TmpBuf[ 128 ];

        memset(TmpBuf, 0, sizeof(TmpBuf));

        if(KeyMngr_EncryptTrackData(pTxnData->TransDB.Track2Data, TmpBuf) != BANK_OK)
        {
            AppUtils_Warning((char *)"磁道2加密失败");
            return BANK_FAIL;
        }

        strcpy((char *)pTxnData->TransDB.Track2Data, (char *)TmpBuf);

        memset(TmpBuf, 0, sizeof(TmpBuf));

        if(KeyMngr_EncryptTrackData(pTxnData->TransDB.Track3Data, TmpBuf) != BANK_OK)
        {
            AppUtils_Warning((char *)"磁道3加密失败");
            return BANK_FAIL;
        }

        strcpy((char *)pTxnData->TransDB.Track3Data, (char *)TmpBuf);

        pTxnData->TransDB.EncryptedTrkFlag = 1;
    }


    if(pTxnData->TransDB.Swipe == TRAN_SWIP
       && gTermCfg.mTrSwitch.ReceiptCardHolderName
       && strlen((char*)pTxnData->Track1Data) > 0)
    {
        if(inTrans_ResolveHolderName((char*)pTxnData->TransDB.CardHolderName, (char*)pTxnData->Track1Data) != BANK_OK)
        {
            AppDebug_AppLog("Resolve CardHolderName from Track1 fail");
        }
    }

    AppDebug_AppLog("EMVtrans_PreProcessing, end to show card pan...");

    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(pTxnData->TransDB.TrType);

//  AppUtils_UserClear(LINE2);

    memset(cBuffer, 0, sizeof(cBuffer));
    sprintf(cBuffer, "%s", pTxnData->TransDB.PAN);
    AppUtils_DisplayLine(cBuffer, LINE2);
    qDebug("card %s", cBuffer);
    if(AppUtils_IsBigScreen() == BANK_OK)
        iLine = LINE9;
    else if(AppUtils_IsRotatedScreen() == BANK_OK)
        iLine = LINE5;
    else
        iLine = LINE4;

    AppUtils_DisplayRight("请确认卡号...", LINE4);
    qDebug("qing que ren card no");
    bkey = 0;
    while(1)
    {
        qDebug("card no check");
        bkey = AppUtils_get_WaitKey(30);
        qDebug("key[%02x]", bkey);
        switch(bkey)
        {
        case bENTER:
            qDebug("return OK");
            return BANK_OK;

        case bESC:
        case 0:
            AppDebug_AppLog("show card pan to confirm, user cancel!");
            return BANK_FAIL;

        default:
            break;
        }
    }
}

int input_pwd_test()
{
    byte bKey;

    while(1)
    {
        bKey = AppUtils_get_WaitKey(60);
        if(bKey == bENTER)
            break;
        else if(bKey == bESC)
            return BANK_FAIL;
    }
    return 1;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_GetCardPin
 * DESCRIPTION:   获取联机密码
 * PARAMETERS:    pTxnData: 交易数据
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_GetCardPin(DynData *pTxnData)
{
    char cPANBuf[20];
    char cDispBuf[80], cTmpBuf[80];
    byte iLine = LINE2;
    int iRet;
    byte result[128];
    unsigned int ilen;

    memset(cPANBuf, 0x00, sizeof(cPANBuf));
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    ilen = 0;
    strncpy(cTmpBuf, (char *)pTxnData->TransDB.PAN, pTxnData->TransDB.PANLen);
    qDebug("PAN%s", cTmpBuf);
    memcpy(cPANBuf, &cTmpBuf[pTxnData->TransDB.PANLen-1-12], 12);


    //手机芯片卡, 预约类 不带主账号运算
    if(pTxnData->TransDB.Swipe == TRAN_MCHIP
       || pTxnData->TransDB.Swipe == TRAN_NOCARD)
    {
        memset(cPANBuf, 0x00, sizeof(cPANBuf));
        memset(cPANBuf, '0', 19);
    }

    AppUtils_UserClear(LINE2);

    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    if(AppUtils_IsNullData((char *)pTxnData->TransDB.Amount, sizeof(pTxnData->TransDB.Amount)) != BANK_OK
       && memcmp(pTxnData->TransDB.Amount, "000000000000", 12))
    {
        //存在金额
        if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
        {
            memset(cDispBuf, 0, sizeof(cDispBuf));
            AppUtils_AmountAsc2String(pTxnData->TransDB.Amount, "", '+', cDispBuf);
            strcat(cDispBuf, "元");
            AppUtils_DisplayCenter(cDispBuf, LINE1, FALSE);
        }

        AppUtils_AmountAsc2String(pTxnData->TransDB.Amount, "", '+', cTmpBuf);
    }
    else
    {
        //不存在金额
        if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
        {
            AppUtils_AmountAsc2String(pTxnData->TransDB.Amount, "", '+', cTmpBuf);
        }
        else
        {
            strcpy(cTmpBuf, " ");   //填一个空格在外置密码键盘上显示
        }
    }


    if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
    {
        AppDebug_AppLog("pTxnData->PinRetryFlag=[%d]", pTxnData->PinRetryFlag);

        if(pTxnData->PinRetryFlag)
            AppUtils_DisplayLine("密码错请重新输入", LINE2);
        else
            AppUtils_DisplayLine("请输入密码", LINE2);

        //指定账户圈存不允许跳过PIN输入，不应该有此提示 (Liu Yan)
        if(pTxnData->TransDB.TrType != BTYPE_ECASH_IN_RELOAD)
            AppUtils_DisplayLine("无密码按确认键", LINE4);

        AppUtils_SetFontFile("");

        if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
            AppUtils_Window(100, 100, 300, 300);
        else
            AppUtils_Window(6, 6, 17, 6);
    }
    else
    {
        AppDebug_AppLog("pTxnData->PinRetryFlag=[%d]", pTxnData->PinRetryFlag);

        if(pTxnData->PinRetryFlag == 0)
            AppUtils_DisplayLine("请持卡人输入密码", iLine ++);
        else
            AppUtils_DisplayLine("密码错请重新输入", iLine ++);

        //指定账户圈存不允许跳过PIN输入，不应该有此提示 (Liu Yan)
        if(pTxnData->TransDB.TrType != BTYPE_ECASH_IN_RELOAD)
            AppUtils_DisplayLine("无密码按确认键", iLine);
    }

    AppUtils_DisplayLine("请持卡人输入密码", iLine ++);
    AppUtils_DisplayLine("无密码按确认键", iLine);

    //激活秘钥
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_ActiveKey(gTermCfg.mFixPrm.KeyIndex, 0, result);
    qDebug("1 iRet:[%d]result[%s] ", iRet, result);
    //设置加密方式
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x01,0x30, result);
    qDebug("2 iRet[%d]%s ", iRet, result);
    //设置pin右补FF
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x02,0xFF, result);
    qDebug("3 iRet[%d]%s ", iRet, result);
    //设置加密格式ansi
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x04,0x10, result);
    qDebug("4 iRet[%d]%s ", iRet, result);
    //输入6位自动加送回车
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x05,0x01, result);
    qDebug("5 iRet[%d]%s ", iRet, result);
    //下载卡号
    qDebug("PAN%s",cPANBuf);
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_LoadCardNumber((byte *)cPANBuf, result);
    qDebug("6 iRet[%d]%s ", iRet, result);
    //打开键盘
    //iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_UseEppPlainTextMode(0x02, result);
    //qDebug("7 iRet[%d]%s ", iRet, result);

    // 启动密码加密
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_StartEpp(6,0x00,60,result);
    qDebug("8 iRet[%d]%s ", iRet, result);
    if(iRet != 0)
    {
        AppUtils_Warning((char *)"密码输入失败");
        return BANK_FAIL;
    }
   memset(cTmpBuf, 0, sizeof(cTmpBuf));
   int num;
   num =0;
    // 获取按键
    while(1)
    {
        AppUtils_DisplayLine(cTmpBuf, LINE3);
        memset(result, 0, sizeof(result));
        iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_ScanKeyPress(result);

        if(iRet == 0)
        {
            qDebug("ScanKey[%d]%s ", iRet, result);
            switch(result[0])
            {
                 case 0x2a:
                     strcat(cTmpBuf, (char *)result);
                     num += 1;
                     break;
                 case 0x08:  //删除
                     memset((byte *)cTmpBuf, 0, sizeof(cTmpBuf));
                     num = 0;
                     break;
                 case 0x23:
                      break;
                  case 0x1b:
                      AppUtils_Warning("用户取消密码输入");
                      return -1;
                  case 0x0d:
                      break;
                  case 0x38:
                      AppUtils_Warning("输入密码超时！");
                      break;
                  case 0x80: //卡键   键盘关闭

                      AppUtils_Warning("键盘卡键");
                      break;
                  case 0x82: //两个键同时按下 键盘关闭

                       AppUtils_Warning("两个按键同时按下");
                      break;
                  default :
                      break;
                   }
        }
        if(num >= 6)
            break;

    }

    Singleton<zzkeypad>::getInstance()->Qt_SUNSON_UseEppPlainTextMode(0x00, result);
    qDebug("10 iRet[%d]%s ", iRet, result);
    //获取加密结果
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_ReadCypherPin(result);
    qDebug("9 iRet[%d][%s] ", iRet, result);
    if(iRet != 13)
    {
        AppUtils_Warning((char *)"密码获取失败");
        return BANK_FAIL;
    }
    else
        iRet = 1;


    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    AppUtils_Asc2Bcd((byte *)result, (byte *)cTmpBuf, 16);

    memcpy(pTxnData->PINdata, cTmpBuf, 8);
    if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
    {
   //     AppUtils_SetFontFile(CHNFONT);

        if(AppUtils_IsColorScreen() == BANK_OK)
            AppUtils_SetFontSize(28);
        else
            AppUtils_SetFontSize(16);
    }
    else
    {
        AppUtils_FlushKbd();
    }

    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(pTxnData->TransDB.TrType);

    switch(iRet)
    {
    case 0:
        pTxnData->PinExists = 1;
        pTxnData->TransDB.POSEntry[ 1 ] = 0x10;
        //Pinpad_Idle();
        return BANK_OK;

    case 1:
        pTxnData->PinExists = 0;
        pTxnData->TransDB.POSEntry[ 1 ] = 0x20;
        //Pinpad_Idle();
        return BANK_OK;

#ifdef  MAKE_LINUX
    case PIN_TIMEOUT:
        AppDebug_AppLog("Pinpad_GetPin=[%d]", iRet);
        AppUtils_Warning((char *)"输密超时");
        //Pinpad_Idle();
        return BANK_FAIL;
#endif

    case 2:
        AppDebug_AppLog("Pinpad_GetPin=[%d]", iRet);
        AppUtils_Warning((char *)"密码输入失败"); //by XC 2015/1/18 23:17:39
        //Pinpad_Idle();
        return BANK_FAIL;

    case 3:
    case 4:
    default:
        AppDebug_AppLog("Pinpad_GetPin=[%d]", iRet);
        AppUtils_Warning((char *)"用户取消输密");
        //Pinpad_Idle();
        return BANK_FAIL;
    }

}


int Trans_CheckECashLoadTxn(byte bTransType)
{
    if((bTransType == BTYPE_ECASH_CASH_RELOAD) ||
       (bTransType == BTYPE_ECASH_IN_RELOAD) ||
       (bTransType == BTYPE_ECASH_OUT_RELOAD) ||
       (bTransType == BTYPE_ECASH_CASH_RELOAD_VOID))
    {
        return BANK_OK;
    }
    else
    {
        return BANK_FAIL;
    }
}


int Trans_CalculateTotals(TotalRec *pTotalsData)
{
    int i, iRecNum;
    DynData tmpTxnRecord;

    // total for settlement
    memset(pTotalsData->CupTotalSaleAmt, '0', 12);
    memset(pTotalsData->CupTotalVoidAmt, '0', 12);
    memset(pTotalsData->FgnTotalSaleAmt, '0', 12);
    memset(pTotalsData->FgnTotalVoidAmt, '0', 12);

    // total for print
    memset(pTotalsData->CupPrtECSaleAmt, '0', 12);
    memset(pTotalsData->CupPrtSaleAmt, '0', 12);
    memset(pTotalsData->CupPrtCompAmt, '0', 12);
    memset(pTotalsData->CupPrtRefundAmt, '0', 12);
    memset(pTotalsData->CupPrtOfflineAmt, '0', 12);
    memset(pTotalsData->CupPrtCompOfflineAmt, '0', 12);
    memset(pTotalsData->CupPrtLoadAmt, '0', 12);
    memset(pTotalsData->FgnPrtSaleAmt, '0', 12);
    memset(pTotalsData->FgnPrtCompAmt, '0', 12);
    memset(pTotalsData->FgnPrtRefundAmt, '0', 12);
    memset(pTotalsData->FgnPrtOfflineAmt, '0', 12);
    memset(pTotalsData->FgnPrtCompOfflineAmt, '0', 12);
    memset(pTotalsData->FgnPrtLoadAmt, '0', 12);

    iRecNum = AppUtils_GetNumberOfRecords((char *)TRAN_DB, sizeof(TransRecord));

    if(iRecNum <= 0)
        return BANK_OK;

    //第一轮统计，用于统计借贷记笔数、不打印全部交易情况下的分类数据
    for(i = 0; i < iRecNum; i ++)
    {
        memset(&tmpTxnRecord, 0, sizeof(tmpTxnRecord));

        if(AppUtils_GetTransRecByIdx(&tmpTxnRecord, i) != BANK_OK)
            break;

        // 内外卡的借贷记笔数 - 结算报文用
        switch(tmpTxnRecord.TransDB.TrType)
        {
            // 预授权/预授权撤销类交易
        case BTYPE_PREAUTH:
        case BTYPE_PREAUTH_VOID:
        case BTYPE_MCHIP_PREAUTH:
        case BTYPE_MCHIP_PREAUTH_VOID:
        case BTYPE_ORDER_PREAUTH:
        case BTYPE_ORDER_PREAUTH_VOID:
            pTotalsData->PreAuthCnt ++;
            break;

            // 取消/退货/现金充值类交易
        case BTYPE_REFUND:
        case BTYPE_ECASH_REFUND://电子现金脱机退货
        case BTYPE_ECASH_CASH_RELOAD://现金充值
        case BTYPE_SALE_VOID:
        case BTYPE_COMPLETE_VOID:
        case BTYPE_MAG_CASHLOAD:
        case BTYPE_INSTALLMENT_VOID:
        case BTYPE_MCHIP_COMPLETE_VOID:
        case BTYPE_MCHIP_SALE_VOID:
        case BTYPE_MCHIP_REFUND:
        case BTYPE_YUYUE_SALE_VOID:
        case BTYPE_YUYUE_REFUND:
        case BTYPE_ISSUER_SCORE_SALE_VOID:
        case BTYPE_UNION_SCORE_SALE_VOID:
        case BTYPE_UNION_SCORE_REFUND:
        case BTYPE_ORDER_REFUND:
        case BTYPE_ORDER_SALE_VOID:
        case BTYPE_ORDER_COMPLETE_VOID:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupTotalVoidCnt ++;
                AppUtils_AmountAdd(pTotalsData->CupTotalVoidAmt, pTotalsData->CupTotalVoidAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnTotalVoidCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnTotalVoidAmt, pTotalsData->FgnTotalVoidAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 消费/完成/调整/分期/扣款类交易
        case BTYPE_SALE:
        case BTYPE_COMPLETE:
        case BTYPE_OFFLINE_COMPLETE:
        case BTYPE_OFFLINE_SETTLE:
        case BTYPE_OFFLINE_ADJUST:
        case BTYPE_INSTALLMENT:
        case BTYPE_YUYUE_SALE:
        case BTYPE_ISSUER_SCORE_SALE:
        case BTYPE_UNION_SCORE_SALE:
        case BTYPE_MCHIP_SALE:
        case BTYPE_MCHIP_COMPLETE:
        case BTYPE_MCHIP_OFFLINE_COMPLETE:
        case BTYPE_ORDER_SALE:
        case BTYPE_ORDER_COMPLETE:
        case BTYPE_ORDER_OFFLINE_COMPLETE:

            if(AppUtils_CheckStatus(tmpTxnRecord.TransDB.Status[ 0 ], STATUS_ADJUST) != BANK_OK)
            {
                if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
                   memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
                {
                    pTotalsData->CupTotalSaleCnt ++;
                    AppUtils_AmountAdd(pTotalsData->CupTotalSaleAmt, pTotalsData->CupTotalSaleAmt, tmpTxnRecord.TransDB.Amount);
                }
                else
                {
                    pTotalsData->FgnTotalSaleCnt ++;
                    AppUtils_AmountAdd(pTotalsData->FgnTotalSaleAmt, pTotalsData->FgnTotalSaleAmt, tmpTxnRecord.TransDB.Amount);
                }
            }

            break;

        default:
            break;
        }

        // 结算总计单用的打印汇总
        if(AppUtils_CheckStatus(tmpTxnRecord.TransDB.Status[ 0 ], STATUS_VOID) == BANK_OK)
            continue;

        switch(tmpTxnRecord.TransDB.TrType)
        {
            // 退货类交易
        case BTYPE_REFUND:
        case BTYPE_ECASH_REFUND:
        case BTYPE_MCHIP_REFUND:
        case BTYPE_UNION_SCORE_REFUND:
        case BTYPE_ORDER_REFUND:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupPrtRefundCnt++;
                AppUtils_AmountAdd(pTotalsData->CupPrtRefundAmt, pTotalsData->CupPrtRefundAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnPrtRefundCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnPrtRefundAmt, pTotalsData->FgnPrtRefundAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 现金充值类交易
        case BTYPE_ECASH_CASH_RELOAD:
        case BTYPE_MAG_CASHLOAD:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupPrtLoadCnt++;
                AppUtils_AmountAdd(pTotalsData->CupPrtLoadAmt, pTotalsData->CupPrtLoadAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnPrtLoadCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnPrtLoadAmt, pTotalsData->FgnPrtLoadAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 消费/扣款类交易
        case BTYPE_SALE:

            if(AppUtils_CheckStatus(tmpTxnRecord.TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
                break;

        case BTYPE_YUYUE_SALE:
        case BTYPE_ISSUER_SCORE_SALE:
        case BTYPE_UNION_SCORE_SALE:
        case BTYPE_MCHIP_SALE:
        case BTYPE_ORDER_SALE:
        case BTYPE_INSTALLMENT:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupPrtSaleCnt ++;
                AppUtils_AmountAdd(pTotalsData->CupPrtSaleAmt, pTotalsData->CupPrtSaleAmt, tmpTxnRecord.TransDB.Amount);

                if((tmpTxnRecord.TransDB.Swipe == TRAN_QPBOC && tmpTxnRecord.TransDB.qPBOC_OnlineFlag == 0) ||
                   (tmpTxnRecord.TransDB.EC_flag == 1))
                {
                    pTotalsData->CupPrtECSaleCnt ++;
                    AppUtils_AmountAdd(pTotalsData->CupPrtECSaleAmt, pTotalsData->CupPrtECSaleAmt, tmpTxnRecord.TransDB.Amount);
                }
            }
            else
            {
                pTotalsData->FgnPrtSaleCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnPrtSaleAmt, pTotalsData->FgnPrtSaleAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 完成请求类交易
        case BTYPE_COMPLETE:
        case BTYPE_MCHIP_COMPLETE:
        case BTYPE_ORDER_COMPLETE:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupPrtCompCnt++;
                AppUtils_AmountAdd(pTotalsData->CupPrtCompAmt, pTotalsData->CupPrtCompAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnPrtCompCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnPrtCompAmt, pTotalsData->FgnPrtCompAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 完成通知类交易
        case BTYPE_OFFLINE_COMPLETE:
        case BTYPE_MCHIP_OFFLINE_COMPLETE:
        case BTYPE_ORDER_OFFLINE_COMPLETE:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupPrtCompOfflineCnt++;
                AppUtils_AmountAdd(pTotalsData->CupPrtCompOfflineAmt, pTotalsData->CupPrtCompOfflineAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnPrtCompOfflineCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnPrtCompOfflineAmt, pTotalsData->FgnPrtCompOfflineAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 离线结算类交易
        case BTYPE_OFFLINE_SETTLE:

            if(AppUtils_CheckStatus(tmpTxnRecord.TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
                break;

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupPrtOfflineCnt ++;
                AppUtils_AmountAdd(pTotalsData->CupPrtOfflineAmt, pTotalsData->CupPrtOfflineAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnPrtOfflineCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnPrtOfflineAmt, pTotalsData->FgnPrtOfflineAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 离线调整类交易
        case BTYPE_OFFLINE_ADJUST:

            if(tmpTxnRecord.TransDB.OrgTranType == BTYPE_OFFLINE_SETTLE)
            {
                if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
                   memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
                {
                    pTotalsData->CupPrtOfflineCnt ++;
                    AppUtils_AmountAdd(pTotalsData->CupPrtOfflineAmt, pTotalsData->CupPrtOfflineAmt, tmpTxnRecord.TransDB.Amount);
                }
                else
                {
                    pTotalsData->FgnPrtOfflineCnt ++;
                    AppUtils_AmountAdd(pTotalsData->FgnPrtOfflineAmt, pTotalsData->FgnPrtOfflineAmt, tmpTxnRecord.TransDB.Amount);
                }
            }
            else
            {
                if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
                   memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
                {
                    pTotalsData->CupPrtSaleCnt ++;
                    AppUtils_AmountAdd(pTotalsData->CupPrtSaleAmt, pTotalsData->CupPrtSaleAmt, tmpTxnRecord.TransDB.Amount);
                }
                else
                {
                    pTotalsData->FgnPrtSaleCnt ++;
                    AppUtils_AmountAdd(pTotalsData->FgnPrtSaleAmt, pTotalsData->FgnPrtSaleAmt, tmpTxnRecord.TransDB.Amount);
                }
            }

        default:
            break;
        }
    }


    //第二轮统计，用于打印全部交易时使用
    for(i = 0; i < iRecNum; i ++)
    {
        memset(&tmpTxnRecord, 0, sizeof(tmpTxnRecord));

        if(AppUtils_GetTransRecByIdx(&tmpTxnRecord, i) != BANK_OK)
            break;

        switch(tmpTxnRecord.TransDB.TrType)
        {
            // 消费类交易
        case BTYPE_SALE:
        case BTYPE_INSTALLMENT:
        case BTYPE_YUYUE_SALE:
        case BTYPE_ISSUER_SCORE_SALE:
        case BTYPE_UNION_SCORE_SALE:
        case BTYPE_MCHIP_SALE:
        case BTYPE_ORDER_SALE:

            if(AppUtils_CheckStatus(tmpTxnRecord.TransDB.Status[ 0 ], STATUS_ADJUST) != BANK_OK)
            {
                if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
                   memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
                {
                    if((tmpTxnRecord.TransDB.Swipe == TRAN_QPBOC && tmpTxnRecord.TransDB.qPBOC_OnlineFlag == 0) ||
                       (tmpTxnRecord.TransDB.EC_flag == 1))
                    {
                        //电子现金不统计
                        continue;
                    }

                    pTotalsData->CupAllSaleCnt ++;
                    AppUtils_AmountAdd(pTotalsData->CupAllSaleAmt, pTotalsData->CupAllSaleAmt, tmpTxnRecord.TransDB.Amount);
                }
                else
                {
                    pTotalsData->FgnAllSaleCnt ++;
                    AppUtils_AmountAdd(pTotalsData->FgnAllSaleAmt, pTotalsData->FgnAllSaleAmt, tmpTxnRecord.TransDB.Amount);
                }
            }

            break;

            //完成请求类
        case BTYPE_COMPLETE:
        case BTYPE_MCHIP_COMPLETE:
        case BTYPE_ORDER_COMPLETE:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupAllCompCnt++;
                AppUtils_AmountAdd(pTotalsData->CupAllCompAmt, pTotalsData->CupAllCompAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnAllCompCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnAllCompAmt, pTotalsData->FgnAllCompAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 消费撤销类交易
        case BTYPE_SALE_VOID:
        case BTYPE_INSTALLMENT_VOID:
        case BTYPE_MCHIP_SALE_VOID:
        case BTYPE_YUYUE_SALE_VOID:
        case BTYPE_ISSUER_SCORE_SALE_VOID:
        case BTYPE_UNION_SCORE_SALE_VOID:
        case BTYPE_ORDER_SALE_VOID:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupAllVoidCnt ++;
                AppUtils_AmountAdd(pTotalsData->CupAllVoidAmt, pTotalsData->CupAllVoidAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnAllVoidCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnAllVoidAmt, pTotalsData->FgnAllVoidAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

            // 预授权完成撤销类交易
        case BTYPE_COMPLETE_VOID:
        case BTYPE_MCHIP_COMPLETE_VOID:
        case BTYPE_ORDER_COMPLETE_VOID:

            if(memcmp(tmpTxnRecord.TransDB.CardTypeName, "CUP", 3) == 0 ||
               memcmp(tmpTxnRecord.TransDB.CardTypeName, "000", 3) == 0)
            {
                pTotalsData->CupAllCompVoidCnt ++;
                AppUtils_AmountAdd(pTotalsData->CupAllCompVoidAmt, pTotalsData->CupAllCompVoidAmt, tmpTxnRecord.TransDB.Amount);
            }
            else
            {
                pTotalsData->FgnAllCompVoidCnt ++;
                AppUtils_AmountAdd(pTotalsData->FgnAllCompVoidAmt, pTotalsData->FgnAllCompVoidAmt, tmpTxnRecord.TransDB.Amount);
            }

            break;

        default:
            break;
        }
    }

    return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_AIDDownload
 * DESCRIPTION:     download AID information parameter
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_AIDDownload(void *dummy)
{

    return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_AIDSave2Kernel
 * DESCRIPTION:     save AID data to EMV Kernel, include CTLS device
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_AIDSave2Kernel(void *dummy)
{

    return BANK_OK;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_CAKeyDownload
 * DESCRIPTION:     download CA KEY
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_CAKeyDownload(void *dummy)
{


        return 0;

}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_CAKeySave2Kernal
 * DESCRIPTION:     save CA KEY to EMV Kernel, include CTLS device
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_CAKeySave2Kernal(void *dummy)
{

    return 0;
}



/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_ClearCAKey
 * DESCRIPTION:     Clear CA KEY in EMV Kernel, include CTLS device
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_ClearCAKey(void *dummy)
{
   return 0;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_BlackListDownload
 * DESCRIPTION:     download AID information parameter
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_BlackListDownload(void *dummy)
{
    DynData TransData;
    int iLength;
    int iTotalParaNum;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    byte *p;
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    iTotalParaNum = 0;

    AppUtils_Cls();
    TransData.TransDB.TrType = BTYPE_BLACK_UPDATE;
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // deleted exists black card list
    if(AppUtils_RemoveFile((char *)BLACK_LIST_CARD) != BANK_OK)
    {
        if(AppUtils_RemoveFile((char *)BLACK_LIST_CARD) != BANK_OK)
        {
            PrnDisp_DisplayWarning("", "删除原来的黑名单文件失败");
            return BANK_FAIL;
        }
    }

    while(TRUE)
    {
        PrnDisp_DisplayTransName(TransData.TransDB.TrType);

        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0800", 4);

        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        sprintf((char *) cTmpBuf, "00%06lu", gTermCfg.mDynPrm.BatchNum);

        if(TransData.TransDB.TrType == BTYPE_BLACK_UPDATE)
            memcpy(cTmpBuf + 8, "390", 3);
        else
            memcpy(cTmpBuf + 8, "391", 3);

        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, 11);

        // field 62
        if(TransData.TransDB.TrType == BTYPE_BLACK_UPDATE)
        {
            if(iTotalParaNum > 0)
                sprintf((char *)cTmpBuf, "%03d", iTotalParaNum + 1);
            else
                sprintf((char *)cTmpBuf, "%03d", iTotalParaNum);

            ISOEngine_SetBit(&ReqIsoRec, 62, cTmpBuf, 3);
        }

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        iLength += 11;

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 发送数据包
        ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));
        Comm_TCP_Disconnect();//连续发送接收，对于TCP通讯方式需要挂断

        // 解包
        ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

        // 判断交易是否成功
        if(memcmp(TransData.TransDB.RspCode, "00", 2) != 0)
        {
            Comm_Disconnect();

            // 显示处理错误代码
            PrnDisp_DisplayRespCode(&TransData);
            return BANK_FAIL;
        }

        if(TransData.TransDB.TrType == BTYPE_BLACK_END)
        {
            break;
        }

        // 获取黑名单数据
        iLength = 0;
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength =  ISOEngine_GetBit(&RspIsoRec, 62, (byte *)cBuffer, sizeof(cBuffer));

        if(iLength <= 0)
        {
            Comm_Disconnect();
            PrnDisp_DisplayWarning("黑名单参数下载", "62域信息有误");
            return BANK_FAIL;
        }

        p = cBuffer;

        if(TransData.TransDB.TrType == BTYPE_BLACK_UPDATE)
        {
            if(*p == '0')
            {
                PrnDisp_DisplayWarning("", "无黑名单可更新!");
                break;
            }
            else if(*p == '1' || *p == '2')
            {
                if(*p == '1')
                    TransData.TransDB.TrType = BTYPE_BLACK_END;

                p ++;

                iTotalParaNum = inTrans_SaveBlackList(iTotalParaNum, p, iLength - 1);

                if(iTotalParaNum < 0)
                {
                    memset(TransData.TransDB.RspCode, 0, sizeof(TransData.TransDB.RspCode));
                    break;
                }
            }
            else
            {
                memset(TransData.TransDB.RspCode, 0, sizeof(TransData.TransDB.RspCode));
                PrnDisp_DisplayWarning("", "黑名单下载62域标识错误!");
                break;
            }

        }
    }

    // 挂线
    Comm_Disconnect();


    if(gTermCfg.mDynPrm.NextEvent == 0x07)
    {
        gTermCfg.mDynPrm.NextEvent = 0;
        PrmMngr_SavePrmToSamDisk();
    }

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_DoHostRequireEventFuncion
 * DESCRIPTION:   后台返回报文头处理请求
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_DoHostRequireEventFuncion(void)
{
    int iTryTimes;

    switch(gTermCfg.mDynPrm.NextEvent)
    {
    case 0x01://下传终端磁条卡参数
        iTryTimes = 0;

        while(iTryTimes ++ <= gTermCfg.mFixPrm.ReversalRetryTimes)
        {
            ////Trans_ParametersDownload((void *)0);

            if(gTermCfg.mDynPrm.NextEvent != 0x01)
                break;
        }

        break;

    case 0x02://上传终端磁条卡状态信息
        iTryTimes = 0;

        while(iTryTimes ++ <= gTermCfg.mFixPrm.ReversalRetryTimes)
        {
            Trans_UploadPOSStatus((void *) 1);

            if(gTermCfg.mDynPrm.NextEvent != 0x02)
                break;
        }

        break;

    case 0x03://重新签到
        AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_OPENED);

        iTryTimes = 0;

        while(iTryTimes ++ <= gTermCfg.mFixPrm.ReversalRetryTimes)
        {
            Trans_Logon((void *) 1);

            if(gTermCfg.mDynPrm.NextEvent != 0x03)
                break;
        }

        break;

    case 0x04://通知终端发起更新公钥信息操作
        iTryTimes = 0;

        while(iTryTimes ++ <= gTermCfg.mFixPrm.ReversalRetryTimes)
        {
            Trans_CAKeyDownload((void *) 0);

            if(gTermCfg.mDynPrm.NextEvent != 0x04)
                break;
        }

        break;

    case 0x05://下载终端IC卡参数
        iTryTimes = 0;

        while(iTryTimes ++ <= gTermCfg.mFixPrm.ReversalRetryTimes)
        {
            Trans_AIDDownload((void *) 0);

            if(gTermCfg.mDynPrm.NextEvent != 0x05)
                break;
        }

        break;

    case 0x06://TMS参数下载
        break;

    case 0x07://卡BIN黑名单下载
        iTryTimes = 0;

        while(iTryTimes ++ <= gTermCfg.mFixPrm.ReversalRetryTimes)
        {
            Trans_BlackListDownload((void *) 0);

            if(gTermCfg.mDynPrm.NextEvent != 0x07)
                break;
        }

        break;

    case 0x08://币种汇率下载（仅在境外使用）助农取款 手续费比率下载（仅在境内使用）
        break;

    default:
        break;
    }

    gTermCfg.mDynPrm.NextEvent = 0;
    PrmMngr_SavePrmToSamDisk();
    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_CheckTerminalStatus
 * DESCRIPTION:   检查终端状态(交易开关，主密钥存在性...and so on)
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_CheckTerminalStatus(int iTrType)
{
    if(inTrans_CheckTxnSupport(iTrType) != 1)
    {
        AppUtils_UserClear(LINE2);
        AppUtils_Warning("该功能未开通");
        return BANK_FAIL;
    }


    // 主管不能做交易
    if(memcmp(gTermCfg.mDynPrm.OperID, "00", 2) == 0 || memcmp(gTermCfg.mDynPrm.OperID, "99", 2) == 0)
    {
        AppUtils_UserClear(LINE2);
        AppUtils_Warning("主管不允许做交易");
        return BANK_FAIL;
    }



    if(iTrType != BTYPE_LOGON)
    {
        //终端时间不应该前于应用程序出来的时间


        if(AppUtils_JournalCheckFreeSpace() != BANK_OK)
        {
            AppUtils_UserClear(LINE2);
            AppUtils_Warning("内存空间不足,请先结算!");
            return BANK_FAIL;
        }

        if((BTYPE_SETTLE != iTrType) && (AppUtils_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord)) >= gTermCfg.mFixPrm.MaxTranNumber))
        {
            AppUtils_UserClear(LINE2);
            AppUtils_Warning("流水记录已满,请先结算!");
            return BANK_FAIL;
        }

        // 检测是否已经POS签到
        if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_OPENED) != BANK_OK)
        {
            AppUtils_UserClear(LINE2);
            AppUtils_Warning("请先签到");
            return BANK_FAIL;
        }

        // 检测是否已经操作员签到
        if(AppUtils_IsNullData((char *)gTermCfg.mDynPrm.OperID, 2) == BANK_OK)
        {
            AppUtils_UserClear(LINE2);
            AppUtils_Warning("请做操作员签到");
            return BANK_FAIL;
        }

        if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_BFST) == BANK_OK)
        {
            AppUtils_UserClear(LINE2);
            AppUtils_Warning("请先结算");
            return BANK_FAIL;
        }

        // 若上次结算未完成,必须先结算
        if((AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK) ||
           (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK) ||
           (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_TC) == BANK_OK))
        {
            AppUtils_UserClear(LINE2);
            AppUtils_Warning("结算未完成,请先做结算!");
            return BANK_FAIL;
        }
    }

//  if( TmsMngr_GetDisableFlag() == 1 )
//  {
//    return BANK_FAIL;
//  }

    if(AppUtils_CheckBattery() != BANK_OK)
    {
        return BANK_FAIL;
    }

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ResetReversalStatus
 * DESCRIPTION:   重新置冲正标记(不是清冲正标记哦)
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ResetReversalStatus(DynData *pTransData)
{
    // 写冲正标志
    AppUtils_SetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL);

    if(PrmMngr_SavePrmToSamDisk() < 0)
    {
        AppUtils_Warning("恢复冲正标志失败!");
        return BANK_FAIL;
    }

    memcpy(pTransData->TransDB.RspCode, "UN", 2);
    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_UpdateReversalFieldData
 * DESCRIPTION:   更新冲正报文域中的内容
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_UpdateReversalFieldData(int iFieldNo, char *pcFieldData, int iDataLength)
{
    int fd;
    int iLength;
    int iOffset;
    byte cBuffer[ ISO8583_MAXLENTH ];
    byte cTmpBuf[ 512 ];
    ISO_data ReqIsoRec;
    DynData TransData;

    // 判断是否有冲正
    if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL) != BANK_OK)
        return BANK_OK;

    fd = open(REVERSAL_FILE, O_RDONLY);

    if(fd < 0)
    {
        AppDebug_AppLog("open orignal reversal file fail!");
        return BANK_FAIL;
    }

    memset((char *)&TransData, 0, sizeof(TransData));

    // 读取冲正数据长度
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = read(fd, (char *)cTmpBuf, 4);

    if(iLength != 4)
    {
        close(fd);
        AppUtils_Warning("读冲正长度失败!");
        return BANK_FAIL;
    }

    iOffset = atoi((char *)cTmpBuf);

    //读取交易类型和刷卡方式
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = read(fd, (char *)cTmpBuf, 2);

    if(iLength != 2)
    {
        close(fd);
        AppUtils_Warning("读冲正数据失败!");
        return BANK_FAIL;
    }

    TransData.TransDB.TrType = cTmpBuf[ 0 ];
    TransData.TransDB.Swipe = cTmpBuf[ 1 ];

    iOffset -= 2;

    // 读取冲正数据
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = read(fd, (char *)cBuffer, iOffset);

    if(iLength != iOffset)
    {
        close(fd);
        AppUtils_Warning("读冲正数据失败2!");
        return BANK_FAIL;
    }

    close(fd);

    // 冲正数据转换
    ClearBit(&ReqIsoRec);
    iLength = StrToIso((byte *)cBuffer + 11, &ReqIsoRec );

    if(iLength < 0)
    {
        PrnDisp_DisplayBankCodeError(BANK_UNPACKERROR);
        return BANK_FAIL;
    }

    if(iFieldNo == 55)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        iLength = GetBit(&ReqIsoRec, 55, cTmpBuf);

        if(iLength + iDataLength > sizeof(cTmpBuf))
            return BANK_FAIL;

        memcpy(cTmpBuf + iLength, pcFieldData, iDataLength);

        if(SetBit(&ReqIsoRec, 55, cTmpBuf, iDataLength + iLength) < 0)
            return BANK_FAIL;
    }
    else if(SetBit(&ReqIsoRec, iFieldNo, (unsigned char *)pcFieldData, iDataLength) < 0)
        return BANK_FAIL;

    memset(cBuffer + 11, 0, sizeof(cBuffer) - 11);
    iLength = IsoToStr(cBuffer + 11, &ReqIsoRec);

    if(iLength <= 0)
        return BANK_FAIL;

    iLength += 11;

    // 保存冲正记录
    if(Trans_SetReversal(&TransData, cBuffer, iLength) != BANK_OK)
        return BANK_FAIL;

    return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_DoReversal
 * DESCRIPTION:     check whether there exists a reversal, if yes, do reversal transcation, else return BANK_OK
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_DoReversal(byte bOrignalTranType)
{
    DynData TransData;
    ISO_data ReqIsoRec, RspIsoRec;
    int fd;
    int iOffset;
    int iLength;
    int iRet;
    int iSendTimes;
    int iSuccessFlag;
    int iCardDeclinedFlag;
    char cReversalData[ ISO8583_MAXLENTH ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    byte cTmpBuf[ 56 ];

    // 判断是否有冲正
    if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL) != BANK_OK)
        return BANK_OK;

    fd = open(REVERSAL_FILE, O_RDONLY);

    if(fd < 0)
    {
        //AppUtils_Warning("打开冲正记录失败!");
        return BANK_OK;
    }

    // 冲正超过次数,自动清除冲正记录
    if(gTermCfg.mDynPrm.ReversalCount >= gTermCfg.mFixPrm.ReversalRetryTimes)
    {
        close(fd);

        AppDebug_AppLog("冲正超过次数=[%d >= %d],自动清除冲正记录", gTermCfg.mDynPrm.ReversalCount, gTermCfg.mFixPrm.ReversalRetryTimes)
        ASSERT_FAIL(Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA));

        return BANK_OK;
    }

    memset(&TransData, 0, sizeof(TransData));

    // 读取冲正数据长度
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = read(fd, (char *)cBuffer, 4);

    if(iLength != 4)
    {
        close(fd);
        AppUtils_Warning("读冲正长度失败!");
        return BANK_OK;
    }

    iOffset = atoi((char *)cBuffer);

    //读取交易类型和刷卡方式
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = read(fd, (char *)cTmpBuf, 2);

    if(iLength != 2)
    {
        close(fd);
        AppUtils_Warning("读冲正类型失败!");
        return BANK_OK;
    }

    TransData.TransDB.TrType = cTmpBuf[ 0 ];
    TransData.TransDB.Swipe = cTmpBuf[ 1 ];

    iOffset -= 2;

    // 读取冲正数据
    memset(cReversalData, 0, sizeof(cReversalData));
    iLength = read(fd, cReversalData, iOffset);

    if(iLength != iOffset)
    {
        close(fd);
        AppUtils_Warning("读冲正数据失败!");
        return BANK_OK;
    }

    close(fd);

    // 冲正数据转换, 发送冲正
    AppUtils_UserClear(LINE1);
    AppUtils_DisplayCenter("冲正", LINE1, TRUE);

    ClearBit(&ReqIsoRec);
    ClearBit(&RspIsoRec);
    //ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    iRet = StrToIso((byte *)cReversalData + 5 + 6, &ReqIsoRec );

    if(iRet != 0)
    {
        PrnDisp_DisplayBankCodeError(BANK_UNPACKERROR);
        return BANK_OK;
    }

    // 打包

    // field 0
    memcpy(TransData.MsgID, "0400", 4);
    SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 3
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    if(GetBit(&ReqIsoRec, 3, cTmpBuf) > 0)
        memcpy(TransData.TransDB.ProcessCode, cTmpBuf, 6);
    qDebug("TransData.TransDB.ProcessCode[%s]", TransData.TransDB.ProcessCode);
    // field 11
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    if(GetBit(&ReqIsoRec, 11, cTmpBuf) > 0)
        TransData.TransDB.SysTrace = atol((char *)cTmpBuf);

    // field 37 // 撤销交易冲正不送37域
    if(memcmp(TransData.TransDB.ProcessCode, "200000", 6) == 0)
        ClearBit1(&ReqIsoRec, 37);

    // field 39
    iCardDeclinedFlag = 0;
    iLength = GetBit(&ReqIsoRec, 39, cTmpBuf);

    if(iLength <= 0)
        SetBit(&ReqIsoRec, 39, (unsigned char *)"98", 2);
    else if(memcmp(cTmpBuf, "Z3", 2) == 0)
    {
        iCardDeclinedFlag = 1;
        SetBit(&ReqIsoRec, 39, (unsigned char *)"06", 2);
    }
    else if(memcmp(cTmpBuf, "98", 2) != 0 && memcmp(cTmpBuf, "A0", 2) != 0)
        SetBit(&ReqIsoRec, 39, (unsigned char *)"06", 2);

    // clear some bit
    ClearBit1(&ReqIsoRec, 26);
    ClearBit1(&ReqIsoRec, 35);
    ClearBit1(&ReqIsoRec, 36);
    ClearBit1(&ReqIsoRec, 37);
    ClearBit1(&ReqIsoRec, 52);
    ClearBit1(&ReqIsoRec, 53);

    if(TransData.TransDB.Swipe != TRAN_MCHIP)
    {
        // field 55
        iLength = GetBit(&ReqIsoRec, 55, cBuffer);

        if(iLength > 0)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            GetBit(&ReqIsoRec, 39, cTmpBuf);

            SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);
        }
    }

    // field 61
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(&ReqIsoRec, 61, cBuffer);

    if(iLength >= 0)
    {
        // 61.1
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(GetBit(&ReqIsoRec, 60, cTmpBuf) > 0)
            memcpy(cBuffer, cTmpBuf + 2, 6);

        // 61.2
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(GetBit(&ReqIsoRec, 11, cTmpBuf) > 0)
            memcpy(cBuffer + 6, cTmpBuf, 6);

        // 61.3
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(GetBit(&ReqIsoRec, 13, cTmpBuf) > 0)
            memcpy(cBuffer + 12, cTmpBuf, 4);
        else if(iLength > 12)
            memcpy(cBuffer + 12, "0000", 4);

        SetBit(&ReqIsoRec, 61, cBuffer, iLength);
    }

    ClearBit1(&ReqIsoRec, 13);

    // field 62 增值业务类的冲正都不送62域
    if((TransData.TransDB.TrType >= BTYPE_INSTALLMENT && TransData.TransDB.TrType <= BTYPE_INSTALLMENT_VOID) ||
       (TransData.TransDB.TrType >= BTYPE_UNION_SCORE_SALE && TransData.TransDB.TrType <= BTYPE_UNION_SCORE_REFUND) ||
       (TransData.TransDB.TrType >= BTYPE_ISSUER_SCORE_SALE && TransData.TransDB.TrType <= BTYPE_ISSUER_SCORE_SALE_VOID) ||
       (TransData.TransDB.TrType >= BTYPE_YUYUE_SALE && TransData.TransDB.TrType <= BTYPE_YUYUE_REFUND) ||
       (TransData.TransDB.TrType >= BTYPE_ORDER_SALE && TransData.TransDB.TrType <= BTYPE_ORDER_OFFLINE_COMPLETE))
    {
        ClearBit1(&ReqIsoRec, 62);
    }

    iSendTimes = 0;
    iSuccessFlag = 1;

    while(iSendTimes < gTermCfg.mFixPrm.ReversalRetryTimes + 1)
    {
        // field 64
        Trans_SaleSetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0, sizeof(cBuffer));

        // TPDU + Header  使用原冲正包中保存的TPDU和报文头
        memcpy(cBuffer, cReversalData, 5 + 6);

        iLength = IsoToStr(cBuffer + 5 + 6, &ReqIsoRec);

        if(iLength <= 0)
        {
            PrnDisp_DisplayBankCodeError(BANK_PACKERROR);
            return BANK_OK;
        }

        iLength += 11;

        // 检查网络并拨号
        iRet = Comm_CheckConnect();

        if(iRet != BANK_OK)
        {
            AppDebug_AppLog("doreversal, check connect return=[%d]", iRet);
            //return iRet;
            return BANK_USERCANCEL;
        }

        // 发送数据包
        if(Comm_SendPacket(cBuffer, iLength) != BANK_OK)
        {
            if(AppUtils_WaitKeyMill(500) == bESC)
                //return BANK_OK;
                return BANK_USERCANCEL;

            continue;
        }

        // 冲正次数累加
        gTermCfg.mDynPrm.ReversalCount ++;
        PrmMngr_SavePrmToSamDisk();

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        iSuccessFlag = 0;

        if(Comm_RecvPacket(cBuffer, &iLength) == BANK_OK)
        {
            // 解包
            if(Trans_UnPackData(&TransData, cBuffer, &RspIsoRec) == BANK_OK)
            {
                // 清除冲正
                if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0 ||
                   memcmp(TransData.TransDB.RspCode, "25", 2) == 0 ||
                   memcmp(TransData.TransDB.RspCode, "12", 2) == 0)
                {
                    iSuccessFlag = 1;
                    break;
                }
            }
        }
        else
        {
            SetBit(&ReqIsoRec, 39, (unsigned char *)"98", 2);
        }

        iSendTimes ++;

        if(iSendTimes < gTermCfg.mFixPrm.ReversalRetryTimes + 1)
        {
            Comm_Disconnect();
            AppUtils_UserClear(LINE2);
            AppUtils_DisplayCenter("冲正失败,将重试!", LINE2, FALSE);
            AppUtils_WaitKey(2);
        }
        else
        {
            Comm_Disconnect();
            AppUtils_UserClear(LINE2);
            AppUtils_DisplayCenter("冲正失败", LINE2, FALSE);
            AppUtils_DisplayCenter("请联系银行", LINE3, FALSE);
            AppUtils_WaitKey(3);
            break;
        }
    }

    AppDebug_AppLog("reversal excess[%d ~ %d]", gTermCfg.mDynPrm.ReversalCount, gTermCfg.mFixPrm.ReversalRetryTimes)
    ASSERT_FAIL(Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA));

    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(bOrignalTranType);
    AppUtils_WaitKey(10);
    if(iSuccessFlag == 0)
    {
        if(Comm_CheckConnect() != BANK_OK)
            return BANK_USERCANCEL;
    }

    return BANK_OK;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_SetReversal
 * DESCRIPTION:     set reversal journal
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_SetReversal(DynData *pstTxnData, byte *pReversalData, int DataLength)
{
    int fd;
    int iLen;
    char cBuffer[ 16 ];

    AppDebug_AppLog("Trans_SetReversal 1..............");

    // 写冲正标志
    AppUtils_SetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL);

    gTermCfg.mDynPrm.ReversalCount = 0;

    if(PrmMngr_SavePrmToSamDisk() < 0)
    {
        AppUtils_Warning("保存冲正标志失败!");
        return BANK_FAIL;
    }

    //TmsMngr_CheckAndUpdateSettleFlag();

    AppDebug_AppLog("Trans_SetReversal 2..............");

    qDebug("%d", chmod(REVERSAL_FILE,S_IRUSR|S_IWUSR));
    // 打开冲正文件
    fd = open(REVERSAL_FILE, O_RDWR | O_CREAT);

    qDebug("fd[%d]\n", fd);
    if(fd < 0)
    {
         AppUtils_Warning("创建冲正记录失败!");
         return BANK_FAIL;
    }

    // 写冲正数据长度
    sprintf(cBuffer, "%04d", DataLength + 2);
    iLen = write(fd, cBuffer, 4);
    qDebug("iLen[%d]\n", iLen);
    if(iLen != 4)
    {
        close(fd);
        AppUtils_Warning("写冲正长度失败!");
        return BANK_FAIL;
    }

    AppDebug_AppLog("Trans_SetReversal 3..............");

    // 写交易类型和刷卡方式
    memset(cBuffer, 0, sizeof(cBuffer));
    cBuffer[ 0 ] = pstTxnData->TransDB.TrType;
    cBuffer[ 1 ] = pstTxnData->TransDB.Swipe;
    iLen = write(fd, cBuffer, 2);
    qDebug("iLen[%d]\n", iLen);
    if(iLen != 2)
    {
        close(fd);
        AppUtils_Warning("写冲正记录失败!");
        return BANK_FAIL;
    }

    AppDebug_AppLog("Trans_SetReversal 4..............");

    // 写原报文数据
    iLen = write(fd, (char *)pReversalData, DataLength);
    qDebug("iLen[%d]\n", iLen);
    if(iLen != DataLength)
    {
        close(fd);
        AppUtils_Warning("写冲正记录失败2!");
        return BANK_FAIL;
    }

    close(fd);

    qDebug("Trans_SetReversal 5..............");
    return BANK_OK;
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_ClearReversal
 * DESCRIPTION:     clear reversal journal
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_ClearReversal(byte bClearReversalFlag)
{
    if(bClearReversalFlag & REVERSAL_CLEAR_STATUS)
    {
        AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL);

        if(PrmMngr_SavePrmToSamDisk() < 0)
        {
            AppUtils_Warning("清空冲正标志失败!");
            return BANK_FAIL;
        }
    }

    if(bClearReversalFlag & REVERSAL_CLEAR_DATA)
    {
        if(AppUtils_RemoveFile(REVERSAL_FILE) != BANK_FAIL)
        {
            int fd;

            fd = remove(REVERSAL_FILE);

            if(fd < 0)
            {
                AppUtils_WaitKeyMill(100);

                fd = open(REVERSAL_FILE, O_WRONLY);

                close(fd);
            }
        }
    }

    //TmsMngr_CheckAndUpdateSettleFlag();
    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_SetField53_Data
 * DESCRIPTION:   设置53
 * PARAMETERS:    pReqIsoRec: 请求报文结构指针, pstTransData: 交易动态数据
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_SaleSetField53_Data(ISO_data *pReqIsoRec, DynData *pstTransData)
{
    char cTmpBuf[ 126 ];
    char cBuffer[ 56 ];

    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    //先设置默认值
    strcpy(cTmpBuf, (char *)"0600000000000000");

    //设置加密算法类型
    cTmpBuf[ 1 ] = (gTermCfg.mFixPrm.PP_DesType == PP222_DES_MODE) ? '0' : '6';

    if(pstTransData->TransDB.Swipe == TRAN_SWIP ||
       pstTransData->TransDB.Swipe == TRAN_ICC ||
       pstTransData->TransDB.Swipe == TRAN_QPBOC)
    {
        //有磁情况
        cTmpBuf[ 0 ] = pstTransData->PinExists ? '2' : '0';
        cTmpBuf[ 2 ] = pstTransData->TransDB.EncryptedTrkFlag ? '1' : '0';

        if(GetBit(pReqIsoRec, 35, (unsigned char *)cBuffer) > 0)
            cTmpBuf[ 2 ] = pstTransData->TransDB.EncryptedTrkFlag ? '1' : '0';
        else
            cTmpBuf[ 2 ] = '0';

        SetBit(pReqIsoRec, 53, (unsigned char *)cTmpBuf, 16);
    }
    else if(pstTransData->TransDB.Swipe == TRAN_MCHIP)
    {
        if(pstTransData->PinExists == 1)
            cTmpBuf[ 0 ] = '1';

        SetBit(pReqIsoRec, 53, (unsigned char *)cTmpBuf, 16);
    }
    else
    {
        //无磁情况
        if(pstTransData->PinExists == 1)
        {
            if(pstTransData->TransDB.Swipe == TRAN_NUM)
                cTmpBuf[ 0 ] = '2'; //手输卡号方式, 带主账号运算
            else
                cTmpBuf[ 0 ] = '1'; //无卡号方式, 不带主账号运算

            SetBit(pReqIsoRec, 53, (unsigned char *)cTmpBuf, 16);
        }

        //无磁无PIN的情况，不打53域
    }

    return BANK_OK;
}
/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_SetField53_Data
 * DESCRIPTION:   设置53
 * PARAMETERS:    pReqIsoRec: 请求报文结构指针, pstTransData: 交易动态数据
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_SetField53_Data(ISO8583_Rec *pReqIsoRec, DynData *pstTransData)
{
    char cTmpBuf[ 126 ];
    char cBuffer[ 56 ];

    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    //先设置默认值
    strcpy(cTmpBuf, (char *)"0600000000000000");

    //设置加密算法类型
    cTmpBuf[ 1 ] = (gTermCfg.mFixPrm.PP_DesType == PP222_DES_MODE) ? '0' : '6';

    if(pstTransData->TransDB.Swipe == TRAN_SWIP ||
       pstTransData->TransDB.Swipe == TRAN_ICC ||
       pstTransData->TransDB.Swipe == TRAN_QPBOC)
    {
        //有磁情况
        cTmpBuf[ 0 ] = pstTransData->PinExists ? '2' : '0';
        cTmpBuf[ 2 ] = pstTransData->TransDB.EncryptedTrkFlag ? '1' : '0';

        if(ISOEngine_GetBit(pReqIsoRec, 35, (unsigned char *)cBuffer, sizeof(cBuffer)) > 0)
            cTmpBuf[ 2 ] = pstTransData->TransDB.EncryptedTrkFlag ? '1' : '0';
        else
            cTmpBuf[ 2 ] = '0';

        ISOEngine_SetBit(pReqIsoRec, 53, (unsigned char *)cTmpBuf, 16);
    }
    else if(pstTransData->TransDB.Swipe == TRAN_MCHIP)
    {
        if(pstTransData->PinExists == 1)
            cTmpBuf[ 0 ] = '1';

        ISOEngine_SetBit(pReqIsoRec, 53, (unsigned char *)cTmpBuf, 16);
    }
    else
    {
        //无磁情况
        if(pstTransData->PinExists == 1)
        {
            if(pstTransData->TransDB.Swipe == TRAN_NUM)
                cTmpBuf[ 0 ] = '2'; //手输卡号方式, 带主账号运算
            else
                cTmpBuf[ 0 ] = '1'; //无卡号方式, 不带主账号运算

            ISOEngine_SetBit(pReqIsoRec, 53, (unsigned char *)cTmpBuf, 16);
        }

        //无磁无PIN的情况，不打53域
    }

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: //Trans_SetField64_MAC
 * DESCRIPTION:   设置64域MAC域
 * PARAMETERS:    pReqIsoRec: 请求报文结构指针
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_SaleSetField64_MAC(ISO_data *pReqIsoRec)
{
    int iPacketLength;
    byte cMac[ 8 ];
    byte cTemp[32];
    byte cTmpBuf[ ISO8583_MAXLENTH ];
    int iRet;

    memset(cMac, 0x00, sizeof(cMac));
    SetBit(pReqIsoRec, 64, cMac, 8);

    memset(cTmpBuf, 0x00, sizeof(cTmpBuf));
    iPacketLength = IsoToStr(cTmpBuf, pReqIsoRec);

    if(iPacketLength <= 0)
        return BANK_FAIL;

    iPacketLength -= 8;


    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_ActiveKey(gTermCfg.mFixPrm.KeyIndex, 1, cTemp);
    qDebug("1 iRet:[%d]result[%s] ", iRet, cTemp);

    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_SetAlgorithmParameter(0x06,0x03, cTemp);
    qDebug("2 iRet[%d]%s ", iRet, cTemp);

    memset(cTemp, 0, sizeof(cTemp));
    iRet = Singleton<zzkeypad>::getInstance()->Qt_SUNSON_MakeBaseMac(iPacketLength, cTmpBuf, cTemp);
    qDebug("3 iRet[%d]res[%s]", iRet, cTemp);
    if(iRet != 0)
        return BANK_FAIL;
    AppUtils_Asc2Bcd((byte *)cTemp, (byte *)cMac, 16);
    //KeyMngr_CalMac(cTmpBuf, iPacketLength, cTemp);
    SetBit(pReqIsoRec, 64, cMac, 8);
    return BANK_OK;
}
/* --------------------------------------------------------------------------
 * FUNCTION NAME: //Trans_SetField64_MAC
 * DESCRIPTION:   设置64域MAC域
 * PARAMETERS:    pReqIsoRec: 请求报文结构指针
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_SetField64_MAC(ISO8583_Rec *pReqIsoRec)
{
    int iPacketLength;
    byte cMac[ 8 ];
    byte cTmpBuf[ ISO8583_MAXLENTH ];

    memset(cMac, 0x00, sizeof(cMac));
    ISOEngine_SetBit(pReqIsoRec, 64, cMac, 8);

    memset(cTmpBuf, 0x00, sizeof(cTmpBuf));
    iPacketLength = ISOEngine_IsoToString(pReqIsoRec, cTmpBuf, sizeof(cTmpBuf));

    if(iPacketLength <= 0)
        return BANK_FAIL;

    iPacketLength -= 8;
    KeyMngr_CalMac(cTmpBuf, iPacketLength, cMac);
    ISOEngine_SetBit(pReqIsoRec, 64, cMac, 8);
    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_UnPackPublicData
 * DESCRIPTION:   解包函数
 * PARAMETERS:    pTxnData:交易动态数据  pRecvData:返回报文  pRspIsoRec:返回包8583结构体
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_UnPackPublicData(DynData *pTxnData, byte *pRecvData, ISO8583_Rec *pRspIsoRec)
{
    int iRet;
    int iLength;
    byte cBuffer[ 512 ]; //扩展到512字节，之前的126字节在增值项目移植时可能会不够，造成隐患(Jing Xu)
    byte *p;

    // 接收数据非法
    if(pRecvData[ 0 ] != 0x60)
    {
        AppDebug_AppLog("Received data from host is invalide");
        PrnDisp_DisplayBankCodeError(BANK_INVALIDE);
        return(BANK_INVALIDE);
    }

    // 解包
    ISOEngine_ClearAllBits(pRspIsoRec);

    iRet = ISOEngine_StringToIso(pRspIsoRec, pRecvData + 5 + 6);

    if(iRet != 0)
    {
        PrnDisp_DisplayBankCodeError(BANK_UNPACKERROR);
        return(BANK_UNPACKERROR);
    }

    // 校验MAC
    iRet = inTrans_GetField64_CheckMac(pRspIsoRec);

    if(iRet != BANK_OK)
        return iRet;

    // 检查报文合法性
    ASSERT_FAIL(inTrans_CheckPacketValid(pTxnData, pRspIsoRec));

    //处理报文头终端状态
    if((pRecvData[ 7 ] & 0x0F) != 0x00)
    {
        gTermCfg.mDynPrm.NextEvent = (pRecvData[ 7 ] & 0x0F);
        PrmMngr_SavePrmToSamDisk();
    }

    // field 2
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 2, cBuffer, sizeof(cBuffer));

    if(iLength > 0)
    {
        pTxnData->TransDB.PANLen = iLength;
        memcpy(pTxnData->TransDB.PAN, cBuffer, iLength);
    }

    // field 12
    memset(cBuffer, 0, sizeof(cBuffer));
    AppUtils_GetClock((char *) cBuffer);
    AppUtils_Asc2Bcd(cBuffer + 8, pTxnData->TransDB.TranTime, 6);

    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 12, cBuffer, sizeof(cBuffer));

    if(iLength > 0)
        AppUtils_Asc2Bcd(cBuffer, pTxnData->TransDB.TranTime, 6);

    // field 13
    memset(cBuffer, 0, sizeof(cBuffer));
    AppUtils_GetClock((char *) cBuffer);
    AppUtils_Asc2Bcd(cBuffer, pTxnData->TransDB.TranDate, 8);

    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 13, cBuffer, sizeof(cBuffer));

    if(iLength == 4)
        AppUtils_Asc2Bcd(cBuffer, pTxnData->TransDB.TranDate + 2, 4);

    // field 14
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 14, cBuffer, sizeof(cBuffer));

    if(iLength > 0 && strcmp((char *)cBuffer, "0000"))
    {
        pTxnData->TransDB.ExpDate[ 0 ] = 0x20;
        AppUtils_Asc2Bcd(cBuffer, pTxnData->TransDB.ExpDate + 1, 4);
    }

    // field 32
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 32, cBuffer, sizeof(cBuffer));

    if(iLength > 0)
    {
        if(iLength > 11)
            iLength = 11;

        memcpy(pTxnData->TransDB.IssuerCode, cBuffer, iLength);
    }

    // field 37
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 37, cBuffer, sizeof(cBuffer));

    if(iLength > 0)
        memcpy(pTxnData->TransDB.RefCode, cBuffer, 12);

    // field 38
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 38, cBuffer, sizeof(cBuffer));

    if(iLength > 0)
        memcpy(pTxnData->TransDB.AuthCode, cBuffer, 6);

    // field 39
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 39, cBuffer, sizeof(cBuffer));

    if(iLength > 0)
    {
        // 交易应答码中仅"00"为交易成功,
        // "11","A2","A4","A5","A6"为有缺陷的成功,其它为交易不成功。
        if((memcmp(cBuffer, "11", 2) == 0) ||
           (memcmp(cBuffer, "A2", 2) == 0) ||
           (memcmp(cBuffer, "A4", 2) == 0) ||
           (memcmp(cBuffer, "A5", 2) == 0) ||
           (memcmp(cBuffer, "A6", 2) == 0))
        {
            memcpy(pTxnData->TransDB.RspCode, "00", 2);

            memcpy(pTxnData->AddRspCode, cBuffer, 2);
        }
        else
            memcpy(pTxnData->TransDB.RspCode, cBuffer, 2);

        // field 4
        if(memcmp(pTxnData->TransDB.RspCode, "00", 2) == 0)
        {
            if(AppUtils_IsNullData((char *)pTxnData->TransDB.Amount, 12) != BANK_OK
               && memcmp(pTxnData->TransDB.Amount, "000000000000", 12) != 0)
            {
                memset(cBuffer, 0, sizeof(cBuffer));
                iLength = ISOEngine_GetBit(pRspIsoRec, 4, cBuffer, 12);

                if(iLength > 0)
                {
                    if(memcmp(pTxnData->TransDB.Amount, cBuffer, 12) != 0)
                    {
                        memcpy(pTxnData->TransDB.RspCode, "06", 2);

                        Trans_UpdateReversalFieldData(39, "06", 2);

                        AppUtils_Warning("返回金额不匹配!");
                        return BANK_FAIL;
                    }
                }
            }
        }
    }

    // field 44
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 44, cBuffer, sizeof(cBuffer));

    if(iLength > 0)
    {
        memcpy(pTxnData->TransDB.Issuer, cBuffer, 11);
        AppUtils_DeleteTrail(' ', (char *)pTxnData->TransDB.Issuer);

        if(iLength > 11)
        {
            memcpy(pTxnData->TransDB.Acquirer, cBuffer + 11, 11);
            AppUtils_DeleteTrail(' ', (char *)pTxnData->TransDB.Acquirer);
        }
    }

    // field 63
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = ISOEngine_GetBit(pRspIsoRec, 63, cBuffer, sizeof(cBuffer));

    if(iLength > 0)
    {
        p = cBuffer;

        memcpy(pTxnData->TransDB.CardTypeName, p, 3);

        p += 3;
        iLength -= 3;

        memset((char *)pTxnData->TransDB.HostNote1, 0, sizeof(pTxnData->TransDB.HostNote1));
        memset((char *)pTxnData->TransDB.HostNote2, 0, sizeof(pTxnData->TransDB.HostNote2));
        memset((char *)pTxnData->TransDB.HostNote3, 0, sizeof(pTxnData->TransDB.HostNote3));

        if(iLength > 0)
        {
            if(iLength <= sizeof(pTxnData->TransDB.HostNote1))
                memcpy(pTxnData->TransDB.HostNote1, p, iLength);
            else
            {
                memcpy(pTxnData->TransDB.HostNote1, p, 20);

                p += 20;
                iLength -= 20;

                if(iLength > 0)
                {
                    if(iLength <= 20)
                        memcpy(pTxnData->TransDB.HostNote2, p, iLength);
                    else
                    {
                        memcpy(pTxnData->TransDB.HostNote2, p, 20);

                        p += 20;
                        iLength -= 20;

                        if(iLength > 0)
                        {
                            if(iLength <= 20)
                                memcpy(pTxnData->TransDB.HostNote3, p, iLength);
                            else
                                memcpy(pTxnData->TransDB.HostNote3, p, 20);
                        }
                    }
                }
            }
        }
    }
    else
        memcpy(pTxnData->TransDB.CardTypeName, "CUP", 3);

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_UnPackData
 * DESCRIPTION:   解包函数
 * PARAMETERS:    pTxnData:交易动态数据  pRecvData:返回报文  pRspIsoRec:返回包8583结构体
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_UnPackData(DynData *pTxnData, byte *pRecvData, ISO_data *pRspIsoRec)
{
    int iRet;
    int iLength;
    byte cBuffer[ 512 ]; //扩展到512字节，之前的126字节在增值项目移植时可能会不够，造成隐患(Jing Xu)
    byte *p;

    // 接收数据非法
    if(pRecvData[ 0 ] != 0x60)
    {
        AppDebug_AppLog("Received data from host is invalide");
        PrnDisp_DisplayBankCodeError(BANK_INVALIDE);
        return(BANK_INVALIDE);
    }
    //qDebug("*****************1******************");
    // 解包
    ClearBit(pRspIsoRec);
    iRet = StrToIso(pRecvData+5+6, pRspIsoRec);
    //qDebug("iso:%d", iRet);
    if(iRet != 0)
    {
        PrnDisp_DisplayBankCodeError(BANK_UNPACKERROR);
        return(BANK_UNPACKERROR);
    }
    //qDebug("*****************2******************");
    // 校验MAC
    iRet = inTrans_SaleGetField64_CheckMac(pRspIsoRec);
    if(iRet != BANK_OK)
        return iRet;

    // 检查报文合法性
    ASSERT_FAIL(inTrans_Sale_CheckPacket(pTxnData, pRspIsoRec));

    //处理报文头终端状态
    if((pRecvData[ 7 ] & 0x0F) != 0x00)
    {
        gTermCfg.mDynPrm.NextEvent = (pRecvData[ 7 ] & 0x0F);
        PrmMngr_SavePrmToSamDisk();
    }
    //qDebug("*****************3******************");
    // field 2
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 2, cBuffer);
    qDebug("PAN:%s, iLength[%d]", cBuffer, iLength);
    if(iLength > 0)
    {
        pTxnData->TransDB.PANLen = iLength;

        memcpy(pTxnData->TransDB.PAN, cBuffer, iLength);
    }
    //qDebug("*****************3******************");
    // field 12
    memset(cBuffer, 0, sizeof(cBuffer));
    AppUtils_GetClock((char *) cBuffer);
    AppUtils_Asc2Bcd(cBuffer + 8, pTxnData->TransDB.TranTime, 6);

    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 12, cBuffer);
    if(iLength > 0)
        AppUtils_Asc2Bcd(cBuffer, pTxnData->TransDB.TranTime, 6);
     qDebug("time:%s", pTxnData->TransDB.TranTime);
    //qDebug("*****************4******************");
    // field 13
    memset(cBuffer, 0, sizeof(cBuffer));
    AppUtils_GetClock((char *) cBuffer);
    AppUtils_Asc2Bcd(cBuffer, pTxnData->TransDB.TranDate, 8);
    //qDebug("*****************5******************");
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 13, cBuffer);

    if(iLength == 4)
        AppUtils_Asc2Bcd(cBuffer, pTxnData->TransDB.TranDate + 2, 4);
     print_qdebug((byte *)"TranDate:%s", pTxnData->TransDB.TranDate, 4);
    // field 14
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 14, cBuffer);

    if(iLength > 0 && strcmp((char *)cBuffer, "0000"))
    {
        pTxnData->TransDB.ExpDate[ 0 ] = 0x20;
        AppUtils_Asc2Bcd(cBuffer, pTxnData->TransDB.ExpDate + 1, 4);
    }

    // field 32
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 32, cBuffer);

    if(iLength > 0)
    {
        if(iLength > 11)
            iLength = 11;

        memcpy(pTxnData->TransDB.IssuerCode, cBuffer, iLength);
    }

    // field 37
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 37, cBuffer);

    if(iLength > 0)
        memcpy(pTxnData->TransDB.RefCode, cBuffer, 12);

    // field 38
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 38, cBuffer);

    if(iLength > 0)
        memcpy(pTxnData->TransDB.AuthCode, cBuffer, 6);
    qDebug("*****************6******************");
    // field 39
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 39, cBuffer);
    qDebug("39:%s", cBuffer);
    if(iLength > 0)
    {
        // 交易应答码中仅"00"为交易成功,
        // "11","A2","A4","A5","A6"为有缺陷的成功,其它为交易不成功。
        if((memcmp(cBuffer, "11", 2) == 0) ||
           (memcmp(cBuffer, "A2", 2) == 0) ||
           (memcmp(cBuffer, "A4", 2) == 0) ||
           (memcmp(cBuffer, "A5", 2) == 0) ||
           (memcmp(cBuffer, "A6", 2) == 0))
        {
            memcpy(pTxnData->TransDB.RspCode, "00", 2);

            memcpy(pTxnData->AddRspCode, cBuffer, 2);
        }
        else
            memcpy(pTxnData->TransDB.RspCode, cBuffer, 2);

        // field 4
        if(memcmp(pTxnData->TransDB.RspCode, "00", 2) == 0)
        {
            if(AppUtils_IsNullData((char *)pTxnData->TransDB.Amount, 12) != BANK_OK
               && memcmp(pTxnData->TransDB.Amount, "000000000000", 12) != 0)
            {
                memset(cBuffer, 0, sizeof(cBuffer));
                iLength = GetBit(pRspIsoRec, 4, cBuffer);

                if(iLength > 0)
                {
                    if(memcmp(pTxnData->TransDB.Amount, cBuffer, 12) != 0)
                    {
                        memcpy(pTxnData->TransDB.RspCode, "06", 2);

                        Trans_UpdateReversalFieldData(39, "06", 2);

                        AppUtils_Warning("返回金额不匹配!");
                        return BANK_FAIL;
                    }
                }
            }
        }
    }
 //qDebug("*****************7******************");
    // field 44
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 44, cBuffer);

    if(iLength > 0)
    {
        memcpy(pTxnData->TransDB.Issuer, cBuffer, 11);
        AppUtils_DeleteTrail(' ', (char *)pTxnData->TransDB.Issuer);

        if(iLength > 11)
        {
            memcpy(pTxnData->TransDB.Acquirer, cBuffer + 11, 11);
            AppUtils_DeleteTrail(' ', (char *)pTxnData->TransDB.Acquirer);
        }
    }

    // field 63
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = GetBit(pRspIsoRec, 63, cBuffer);
 //qDebug("*****************8******************");
    if(iLength > 0)
    {
        p = cBuffer;

        memcpy(pTxnData->TransDB.CardTypeName, p, 3);

        p += 3;
        iLength -= 3;

        memset((char *)pTxnData->TransDB.HostNote1, 0, sizeof(pTxnData->TransDB.HostNote1));
        memset((char *)pTxnData->TransDB.HostNote2, 0, sizeof(pTxnData->TransDB.HostNote2));
        memset((char *)pTxnData->TransDB.HostNote3, 0, sizeof(pTxnData->TransDB.HostNote3));

        if(iLength > 0)
        {
            if(iLength <= sizeof(pTxnData->TransDB.HostNote1))
                memcpy(pTxnData->TransDB.HostNote1, p, iLength);
            else
            {
                memcpy(pTxnData->TransDB.HostNote1, p, 20);

                p += 20;
                iLength -= 20;

                if(iLength > 0)
                {
                    if(iLength <= 20)
                        memcpy(pTxnData->TransDB.HostNote2, p, iLength);
                    else
                    {
                        memcpy(pTxnData->TransDB.HostNote2, p, 20);

                        p += 20;
                        iLength -= 20;

                        if(iLength > 0)
                        {
                            if(iLength <= 20)
                                memcpy(pTxnData->TransDB.HostNote3, p, iLength);
                            else
                                memcpy(pTxnData->TransDB.HostNote3, p, 20);
                        }
                    }
                }
            }
        }
    }
    else
        memcpy(pTxnData->TransDB.CardTypeName, "CUP", 3);

    return BANK_OK;
}
/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_GetOrignalJournal
 * DESCRIPTION:   读取交易数据
 * PARAMETERS:    pTitle:提示信息  pTxnData:交易动态数据
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_GetOrignalJournal(char *pTitle, DynData *pTxnData)
{
    char cBuffer[ 7 ];

    while(TRUE)
    {
        memset(cBuffer, 0, sizeof(cBuffer));

        if(AppUtils_GetNum(LINE2, pTitle, cBuffer, 1, 6, 60) <= 0)
            return BANK_FAIL;

        if(AppUtils_JournalGetRecord(atol(cBuffer), pTxnData) >= 0)
            break;
        else
            PrnDisp_DisplayWarning("", "交易不存在");
    }

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_SubmitOffline
 * DESCRIPTION:   上送离线交易
 * PARAMETERS:    bUploadFlag: 0-联机交易前, 1-批结算前, 2-笔数达到总数自动上送
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_SubmitOffline(byte bUploadFlag)
{
    int iRet = BANK_OK;
    int iTimes;
    unsigned char bUploadFailFlag;
    qDebug("Trans_SubmitOffline");
    //bUploadFlag = 0-联机交易前, 1-批结算前, 2-笔数达到总数自动上送
    if(gTermCfg.mFixPrm.UploadMaxTimes <= 0)
    {
        gTermCfg.mFixPrm.UploadMaxTimes = 3;
        PrmMngr_SavePrmToSamDisk();
    }

    iTimes = 0;
    bUploadFailFlag = 0x0F;

    while(iTimes ++ < gTermCfg.mFixPrm.UploadMaxTimes + 1)
    {
        AppDebug_AppLog("Trans_SubmitOffline, iTimes=[%d], gTermCfg.mFixPrm.UploadMaxTimes=[%d]", iTimes, gTermCfg.mFixPrm.UploadMaxTimes);

        // 先处理脱机的磁条卡交易数据上送
        if(bUploadFailFlag & 0x01)
        {
            iRet = inTrans_UploadOffline(0, TRAN_SWIP);

            if(iRet == BANK_USERCANCEL)
                break;
            else if(iRet == BANK_OK)
                bUploadFailFlag &= ~0x01;
        }

        // 再处理脱机的PBOC IC卡交易数据上送
        if(bUploadFailFlag & 0x02)
        {
            iRet = inTrans_UploadOffline(0, TRAN_ICC);

            if(iRet == BANK_USERCANCEL)
                break;
            else if(iRet == BANK_OK)
                bUploadFailFlag &= ~0x02;
        }

        // 再处理脱机的电子现金IC卡交易数据上送
        if(bUploadFailFlag & 0x04)
        {
            iRet = inTrans_UploadOffline(0, TRAN_ECASH);

            if(iRet == BANK_USERCANCEL)
                break;
            else if(iRet == BANK_OK)
                bUploadFailFlag &= ~0x04;
        }

        // 再处理qPBOC交易数据上送
        if(bUploadFailFlag & 0x08)
        {
            iRet = inTrans_UploadOffline(0, TRAN_QPBOC);

            if(iRet == BANK_USERCANCEL)
                break;
            else if(iRet == BANK_OK)
                bUploadFailFlag &= ~0x08;
        }

        AppDebug_AppLog("bUploadFailFlag=[%02x]", bUploadFailFlag);

        if(!(bUploadFailFlag & 0x0F))
            break;
    }

    return iRet;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_CheckNeedDisconnect
 * DESCRIPTION:   联机交易完成后检测是否需要挂线
 * PARAMETERS:
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:   非TCPIP通讯方式下，需要检查是否有脱机交易需要上送
 *          TCPIP通讯方式下，不管有无离线交易，都应挂线(断SOCKET)后返回
 * --------------------------------------------------------------------------*/
int Trans_CheckNeedDisconnect(void)
{
    // 非TCPIP通讯方式下，需要检查是否有脱机交易需要上送
    // TCPIP通讯方式下，不管有无离线交易，都应挂线(断SOCKET)后返回
    if(!Comm_IsTCPIPCommMode(gTermCfg.mCommPrm.CurrentCommMode))
    {
        if(gTermCfg.mDynPrm.OfflineRecNum > 0)
        {
            //有离线交易，不挂线直接返回
            return BANK_FAIL;
        }
    }

    Comm_Disconnect();
    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_CheckAutoUploadOffline
 * DESCRIPTION:   检测当前脱机交易笔数是否达到脱机交易上限，如果达到则自动上送脱机交易
 * PARAMETERS:
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_CheckAutoUploadOffline(void)
{
    int iRv;

    // 检查是否有脱机交易需要上送
    if((gTermCfg.mDynPrm.OfflineRecNum <= 0) ||
       (gTermCfg.mDynPrm.OfflineRecNum < gTermCfg.mFixPrm.OffUploadNumLimit))
    {
        return BANK_OK;
    }

    AppDebug_AppLog("upload offline auto,[%ld]>=[%d]", gTermCfg.mDynPrm.OfflineRecNum, gTermCfg.mFixPrm.OffUploadNumLimit);

    iRv = Trans_SubmitOffline(2);
    Comm_Disconnect();

    return iRv;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_UploadICCscript
 * DESCRIPTION:   上送IC卡脚本
 * PARAMETERS:    pTransData: 交易动态数据
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_UploadICCscript(DynData *pTransData)
{
    DynData TransData;
    TransRecord OrgTransJournal;
    int iLength;
    int iCnt;
    int i;
    int iRecNum;
    int iTimes;
    int iRecvFlag;
    ulint iOldSystemTrace;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    if(gTermCfg.mTrSwitch.Script_Support == 0)
    {
        AppDebug_AppLog("pos do not support scripte upload,script support=[%d]", gTermCfg.mTrSwitch.Script_Support);
        return BANK_OK;
    }

    if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_SCRIPT) != BANK_OK)
        return BANK_OK;

    iRecNum = AppUtils_GetNumberOfRecords((char *)TRAN_DB_SCRIPT, sizeof(TransRecord));

    if(iRecNum <= 0)
        return BANK_OK;

    AppDebug_AppLog("Trans_UploadICCscript,GetNumberOfRecords=[%d]", iRecNum);

    iTimes = 0;

    if(gTermCfg.mFixPrm.UploadMaxTimes <= 0)
    {
        gTermCfg.mFixPrm.UploadMaxTimes = 3;
        PrmMngr_SavePrmToSamDisk();
    }

    iRecvFlag = 1;

    while(iTimes ++ < gTermCfg.mFixPrm.UploadMaxTimes + 1)
    {
        AppDebug_AppLog("upload icc script, times=[%d,%d]", iTimes, gTermCfg.mFixPrm.UploadMaxTimes + 1);

        iCnt = 1;

        for(i = 0; i < iRecNum; i ++)
        {
            memset(&TransData, 0, sizeof(TransData));

            if(AppUtils_GetRecordByIndex((char *)TRAN_DB_SCRIPT, (void *)&TransData, sizeof(TransRecord), i) != BANK_OK)
            {
                AppDebug_AppLog("Trans_UploadICCscript, AppUtils_GetRecordByIndex[%d] fail", i);
                break;
            }

            AppDebug_AppLog("TransData.TransDB.Status=[%02X %02X]", TransData.TransDB.Status[ 0 ], TransData.TransDB.Status[ 1 ]);

            if(AppUtils_CheckStatus(TransData.TransDB.Status[ 1 ], STATUS_SCRIPT) != BANK_OK)
                continue;

            if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_BATCH) == BANK_OK)
                continue;

            memset(&OrgTransJournal, 0, sizeof(TransRecord));
            memcpy(&OrgTransJournal, &TransData.TransDB, sizeof(TransRecord));

            AppDebug_AppLog("start to do Trans_UploadICCscript=[%d], Cnt=%d, Trace=%06lu,trytime=%d", i, iCnt, TransData.TransDB.SysTrace, iTimes);

            // 显示交易类型
            AppUtils_UserClear(LINE1);
            sprintf((char *)cTmpBuf, "脚本上送:%d", iCnt ++);
            AppUtils_DisplayCenter((char *)cTmpBuf, LINE1, TRUE);

            if(pTransData->TransDB.TrType != BTYPE_SETTLE)
            {
                if(AppUtils_WaitKeyMill(300) == bESC)
                {
                    AppDebug_AppLog("user canel script upload...");
                    return BANK_OK;
                }
            }

            if(iTimes == 1)
            {
                //脚本上送的重发交易，重发的交易11域流水号保持第一次的流水号不变
                // 增加流水号
                PrmMngr_AddSystrace();
            }

            // 打包
            ISOEngine_ClearAllBits(&ReqIsoRec);
            ISOEngine_ClearAllBits(&RspIsoRec);
            ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

            // field 0
            memcpy(TransData.MsgID, "0620", 4);
            ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

            // field 2
            if(TransData.TransDB.TrType == BTYPE_ECASH_OUT_RELOAD)
                ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN_2, TransData.TransDB.PAN_2Len);
            else
                ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

            // field 3
            ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

            // field 4
            if(AppUtils_IsNullData((char *)TransData.TransDB.Amount, 12) != BANK_OK &&
               TransData.TransDB.TrType != BTYPE_BALANCE)
                ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

            // field 11
            iOldSystemTrace = TransData.TransDB.SysTrace;
            TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
            sprintf((char *)cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
            ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

            // field 22
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

            // field 23
            if(TransData.TransDB.SeqNumFlag == 1)
            {
                memset(cTmpBuf, 0, sizeof(cTmpBuf));
                memcpy(cTmpBuf, "00", 2);
                AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
                ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
            }

            // field 32
            if(AppUtils_IsNullData((char *)TransData.TransDB.IssuerCode, sizeof(TransData.TransDB.IssuerCode)) != BANK_OK)
                ISOEngine_SetBit(&ReqIsoRec, 32, TransData.TransDB.IssuerCode, strlen((char *)TransData.TransDB.IssuerCode));

            // field 37
            if(AppUtils_IsNullData((char *)TransData.TransDB.RefCode, sizeof(TransData.TransDB.RefCode)) != BANK_OK)
                ISOEngine_SetBit(&ReqIsoRec, 37, TransData.TransDB.RefCode, 12);

            // field 38
            if(AppUtils_IsNullData((char *)TransData.TransDB.AuthCode, sizeof(TransData.TransDB.AuthCode)) != BANK_OK)
                ISOEngine_SetBit(&ReqIsoRec, 38, TransData.TransDB.AuthCode, 6);

            // field 41
            ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

            // field 42
            ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

            // field 49
            ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

            // field 55
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);

            // field 60
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);
            memcpy(cTmpBuf + 8, "951", 3);

            //60.4终端读取能力
            if(gTermCfg.mTrSwitch.CTLS_Support == 1)
                memcpy(cTmpBuf + 11, "6", 1);
            else
                memcpy(cTmpBuf + 11, "5", 1);

            //60.5 IC卡条件代码
            memcpy(cTmpBuf + 12, "0", 1);

            ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

            // field 61
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            sprintf((char *)cTmpBuf, "%06ld", TransData.TransDB.BatchNum);
            sprintf((char *)cTmpBuf + 6, "%06ld", iOldSystemTrace);
            AppUtils_Bcd2Asc(TransData.TransDB.TranDate + 2, cTmpBuf + 12, 4);
            iLength = 16;
            ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, iLength);

            // field 64
            //Trans_SetField64_MAC(&ReqIsoRec);

            // 组包转换
            memset(cBuffer, 0x00, sizeof(cBuffer));

            // TPDU
            memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

            // CUP Header
            memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

            iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

            if(iLength <= 0)
            {
                AppDebug_AppLog("script upload, ISOEngine_IsoToString fail=[%d]", iLength);
                continue;
            }

            iLength += 11;

            iRecvFlag = 0;

            // 检查网络并拨号
            ASSERT_FAIL(Comm_CheckConnect());

            // 发送数据包
            Comm_SendPacket(cBuffer, iLength);

            // 接收数据包
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = 0;

            memcpy(TransData.TransDB.RspCode, "ZZ", 2);

            if(Comm_RecvPacket(cBuffer, &iLength) == BANK_OK)
            {
                iRecvFlag = 1;
                AppDebug_AppLog("receive ok");

                // 解包
                Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec);
            }

            // 更新已上送标识
            if(iRecvFlag == 1)
                AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_BATCH);
            else
            {
                OrgTransJournal.BeforeStlUploadCount ++;

                if(OrgTransJournal.BeforeStlUploadCount > gTermCfg.mFixPrm.UploadMaxTimes)
                    AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_BATCH);
            }

            //File_UpdateRecordByIndex((char *)TRAN_DB_SCRIPT, (char *)&OrgTransJournal, sizeof(TransRecord), i);
        }
    }

    AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_SCRIPT);
    PrmMngr_SavePrmToSamDisk();

    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(pTransData->TransDB.TrType);

    AppDebug_AppLog("end of Trans_UploadICCscript");

    if(iRecvFlag != 1)
        ASSERT_FAIL(Comm_CheckConnect());

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ClearScriptResult
 * DESCRIPTION:   清除脚本结果通知
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ClearScriptResult(void *dummy)
{
    int iRecNum;
    int i;
    int iRet = BANK_FAIL;

    AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_SCRIPT);
    PrmMngr_SavePrmToSamDisk();

    // 脚本上送流水
    iRecNum = AppUtils_GetNumberOfRecords((char *) TRAN_DB_SCRIPT, sizeof(TransRecord));

    if(iRecNum > 0)
    {
        for(i = 0; i < 3; i ++)
        {
            if(AppUtils_RemoveFile((char *) TRAN_DB_SCRIPT) == BANK_OK)
            {
                iRet = BANK_OK;
                break;
            }
        }

        if(iRet != BANK_OK)
        {
            for(i = 0; i < iRecNum; i ++)
            {
                //File_DeleteRecordByIndex((char *) TRAN_DB_SCRIPT, sizeof(TransRecord), i);
            }
        }
    }

    return BANK_OK;
}




// ------------------------------------------------------------------------
//
//  Private function define
//
// ------------------------------------------------------------------------




/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   inTrans_StortAIDHeader
 * DESCRIPTION:     save the temporary taid header
 * RETURN:          BANK_OK: success, BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
static void inTrans_AIDHeaderReset(void)
{
    inTrans_iTmpHeaderNum = 0;
    memset((byte *) &inTrans_stTmpAIDHeader, 0, sizeof(inTrans_stTmpAIDHeader));

    // deleted temporary taid parameter
    if(AppUtils_RemoveFile(AIDtmp_FILE) != BANK_OK)
    {
        if(AppUtils_RemoveFile(AIDtmp_FILE) != BANK_OK)
            return;
    }

    // deleted temporary taid parameter
    if(AppUtils_RemoveFile(CTLSAIDtmp_FILE) != BANK_OK)
    {
        if(AppUtils_RemoveFile(CTLSAIDtmp_FILE) != BANK_OK)
            return;
    }

    return;
}

static int inTrans_AIDHeaderGetAID(int nIndex, char *pRetRID)
{
    int ubLen = 0;

    if(nIndex >= 0 && nIndex < EMV_ICC_HEADER_MAX)
    {
        ubLen = inTrans_stTmpAIDHeader[ nIndex ].Length;
        memcpy(pRetRID, inTrans_stTmpAIDHeader[ nIndex ].AID, ubLen);
    }

    return ubLen;
}

static int inTrans_AIDHeaderSave(char *pAIDHeader, int iAIDHeaderLength)
{
    int i;
    byte iLength;
    byte ValidFlag;
    byte bFoundFlag;
    byte AID[ 17 ];
    char *pHeader;

    pHeader = pAIDHeader;

    do
    {
        ValidFlag = 0x0;

        if(!memcmp(pHeader, "\x9F\x06", 2))
        {
            iLength = * (pHeader + 2);

            if(iLength > 16)
                iLength = 16;

            memcpy(AID, pHeader + 3 , iLength);
            ValidFlag |= 0x01;
        }

        if((*pHeader & 0x0F) == 0x0F)
            pHeader += 2;
        else
            pHeader ++;

        iLength = *pHeader;
        pHeader ++;
        pHeader += iLength;

        if(ValidFlag == 0x01)
        {
            if(iLength > 16)
                iLength = 16;

            bFoundFlag = FALSE;

            for(i = 0; i < EMV_ICC_HEADER_MAX; i++)
            {
                if(inTrans_stTmpAIDHeader[ i ].Length == iLength &&
                   memcmp(AID, inTrans_stTmpAIDHeader[ i ].AID, iLength) == 0)
                {
                    bFoundFlag = TRUE;
                    break;
                }
                else if(inTrans_stTmpAIDHeader[ i ].AID[ 0 ] == 0)
                {
                    break;
                }
            }

            if((i < EMV_ICC_HEADER_MAX) && (bFoundFlag == FALSE))
            {
                memcpy(inTrans_stTmpAIDHeader[ i ].AID, AID, iLength);
                inTrans_stTmpAIDHeader[ i ].Length = iLength;

                if(!bFoundFlag)
                    inTrans_iTmpHeaderNum ++;
            }
        }

    }
    while((pHeader - pAIDHeader) < iAIDHeaderLength);

    return inTrans_iTmpHeaderNum;
}


static int inTrans_AIDSave2File(char *pAIDData, int iAIDDataLength)
{

    return BANK_OK;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_UploadScriptResult
 * DESCRIPTION:     上送脚本结果通知
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_UploadScriptResult(void *dummy)
{
    DynData TransData;
    int iResult;

    // 检测是否有脚本需要上送
    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_SCRIPTADVICE;

    iResult = Trans_UploadICCscript(&TransData);

    return iResult;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_UploadOfflineTrans
 * DESCRIPTION:     上送脱机交易
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_UploadOfflineTrans(void *dummy)
{
    int iResult;

    // 检查是否有脱机交易需要上送
    iResult = Trans_SubmitOffline(0);

    return iResult;
}
/* -----------------------------------------------------------------------------
 * Private Function define
 ---------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   inTrans_StortAIDHeader
 * DESCRIPTION:     save the temporary taid header
 * RETURN:          BANK_OK: success, BANK_FAIL: fail
 ---------------------------------------------------------------------------- */

static void inTrans_CAKeyHeaderReset(void)
{
    inTrans_iTmpHeaderNum = 0;
    memset((byte *)&inTrans_stTmpCAKeyHeader, 0, sizeof(inTrans_stTmpCAKeyHeader));

    // deleted temporary CAKEY parameter
    if(AppUtils_RemoveFile(CAKEYtmp_FILE) != BANK_OK)
    {
        if(AppUtils_RemoveFile(CAKEYtmp_FILE) != BANK_OK)
            return;
    }

    return;
}


static int inTrans_CAKeyHeaderGetRID(int nIndex, char *pRetRID)
{
    int ubLen = 0;

    if((nIndex >= 0) && (nIndex < EMV_ICC_HEADER_MAX))
    {
        ubLen = inTrans_stTmpCAKeyHeader[ nIndex ].Index;
        memcpy(pRetRID, inTrans_stTmpCAKeyHeader[ nIndex ].RID, 5);
    }

    return ubLen;
}


static int inTrans_CAKeyHeaderSave(char *pCAKeyHeader, int iCAKeyHeaderLength)
{
    int i;
    byte iLength;
    byte ValidFlag;
    byte bFoundFlag;
    byte RID[ 6 ], Index;
    char *pHeader;

    pHeader = pCAKeyHeader;

    ValidFlag = 0x0;
    memset(RID, 0, sizeof(RID));
    Index = 0;

    do
    {
        if(!memcmp(pHeader, "\x9F\x06", 2))
        {
            iLength = *(pHeader + 2);

            if(iLength > 5)
                iLength = 5;

            memcpy(RID, pHeader + 3 , iLength);
            ValidFlag |= 0x01;
        }

        if(!memcmp(pHeader, "\x9F\x22", 2))
        {
            iLength = *(pHeader + 2);
            Index = *(pHeader + 3);
            ValidFlag |= 0x02;
        }

        if((*pHeader & 0x0F) == 0x0F)
            pHeader += 2;
        else
            pHeader ++;

        iLength = *pHeader;
        pHeader++;
        pHeader += iLength;

        if(ValidFlag == 0x03)
        {
            bFoundFlag = FALSE;

            for(i = 0; i < EMV_ICC_HEADER_MAX; i++)
            {
                if((Index == inTrans_stTmpCAKeyHeader[ i ].Index) &&
                   (memcmp(RID, inTrans_stTmpCAKeyHeader[ i ].RID, 5) == 0))
                {
                    bFoundFlag = TRUE;
                    break;
                }
                else if(inTrans_stTmpCAKeyHeader[ i ].RID[ 0 ] == 0)
                {
                    break;
                }
            }

            if((i < EMV_ICC_HEADER_MAX) && (bFoundFlag == FALSE))
            {
                memcpy(inTrans_stTmpCAKeyHeader[ i ].RID, RID, 5);
                inTrans_stTmpCAKeyHeader[ i ].Index = Index;

                if(!bFoundFlag)
                    inTrans_iTmpHeaderNum ++;
            }

            ValidFlag = 0x0;
        }

    }
    while((pHeader - pCAKeyHeader) < iCAKeyHeaderLength);

    return inTrans_iTmpHeaderNum;
}


static int inTrans_CAKeySave2File(char *pCAKeyData, int iCAKeyDataLength)
{

    return BANK_OK;
}


static int inTrans_SaveBlackList(int iBlackListNum, byte *pBlackListData, int iDataLength)
{
    int fd;
    int i;
    int iCnt;
    int iCardLength;
    int iLength;
    char cTmpBuf[ 26 ];
    char *p;

    AppDebug_AppLog("save black list, string from host, length=[%d],string=[%s]", iDataLength, pBlackListData);

    p = (char *)pBlackListData;

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    memcpy(cTmpBuf, p, 3);
    p += 3;

    iCnt = atoi(cTmpBuf);

    AppDebug_AppLog("save black list, black list old count=[%d], new count=[%d] fail", iBlackListNum, iCnt);

    if(iCnt <= 0)
    {
        return(iCnt);
    }

    if((fd = open(BLACK_LIST_CARD, O_WRONLY)) < 0)
    {
        if((fd = open(BLACK_LIST_CARD, O_WRONLY | O_CREAT)) < 0)
        {
            PrnDisp_DisplayWarning("", "打开黑名单文件错");
            return -1;
        }
    }

   // File_Lseek(fd, 0L, SEEK_END);

    for(i = 0 ; i < iCnt - iBlackListNum; i ++)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, p, 2);
        p += 2;

        iLength = atoi(cTmpBuf);
        iCardLength = iLength;

        if(iCardLength > 14)
            iCardLength = 14;

        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, p, iCardLength);

        p += iLength;

        cTmpBuf[ iCardLength ] = '\n';
        iCardLength ++;

        write(fd, cTmpBuf, iCardLength);
    }

    close(fd);

    return(iCnt);
}

static byte inTrans_IsSmallAmountAuth_Support(void)
{
    return gTermCfg.mTrSwitch.SmallAmountStandinAuth;
}

static int inTrans_SaleGetField64_CheckMac(ISO_data *pRspIsoRec)
{
    int iLength;
    byte cRspMac[ 9 ], cMac[ 9 ], cRespCode[ 3 ];
    byte cTmpBuf[ ISO8583_MAXLENTH ];
    int i;

    // 银联，管理类，结算，批上送类交易没有MAC数据
    memset(cRspMac, 0, sizeof(cRspMac));
    GetBit(pRspIsoRec, 0, cRspMac);

    qDebug("MTI:%s",pRspIsoRec->message_id);
    if(memcmp(pRspIsoRec->message_id, "03", 2) == 0 ||
       memcmp(pRspIsoRec->message_id, "05", 2) == 0 ||
       memcmp(pRspIsoRec->message_id, "08", 2) == 0)
    {
        return BANK_OK;
    }

    memset(cRespCode, 0, sizeof(cRespCode));
    GetBit(pRspIsoRec, 39, cRespCode);

    // 获取MAC值
    memset(cRspMac, 0, sizeof(cRspMac));

    // 取不到MAC的情况下
    if(GetBit(pRspIsoRec, 64, cRspMac) <= 0)
    {
        // 若不成功的交易, MAC域不存在的时候，不需要校验
        if((memcmp(cRespCode, "00", 2) == 0) ||
           (memcmp(cRespCode, "10", 2) == 0) ||
           (memcmp(cRespCode, "11", 2) == 0) ||
           (memcmp(cRespCode, "A2", 2) == 0) ||
           (memcmp(cRespCode, "A4", 2) == 0) ||
           (memcmp(cRespCode, "A5", 2) == 0) ||
           (memcmp(cRespCode, "A6", 2) == 0))
        {
            AppDebug_AppLog("field 64 is empty, set mac error to trans");

            Trans_UpdateReversalFieldData(39, "A0", 2);//有问题，稍后解决

            // field 13
            iLength = GetBit(pRspIsoRec, 13, cRspMac);

            if(iLength == 4)
            {
                Trans_UpdateReversalFieldData(13, (char *)cRspMac, 4);
                AppDebug_AppLog("set field 13 for reversal as MAC error");
            }

            PrnDisp_DisplayBankCodeError(BANK_MACERROR);
            return(BANK_MACERROR);
        }
        else if(memcmp(cRespCode, "A0", 2) == 0)
        {
            PrmMngr_ResetLogonData((void*)0);
            return BANK_OK;
        }
        else
            return BANK_OK;
    }

    memset(cMac, 0, sizeof(cMac));
    SetBit(pRspIsoRec, 64, cMac, 8);

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = IsoToStr(cTmpBuf, pRspIsoRec);
    //print_qdebug((unsigned char *)"isodata", cTmpBuf, iLength);
    if(iLength <= 0)
    {
        PrnDisp_DisplayBankCodeError(BANK_UNPACKERROR);
        return(BANK_UNPACKERROR);
    }

    qDebug("1iLength%d", iLength);
    iLength -= 8;
    //qDebug("2iLength%d", iLength);


    KeyMngr_CalMac(cTmpBuf, iLength, cMac);
    //print_qdebug((unsigned char *)"isodata", cTmpBuf, iLength);
    if(memcmp(cRspMac, cMac, 8) != 0)
    {
        AppDebug_AppLog("mac error, [%8.8s-%8.8s]", cRspMac, cMac);

        Trans_UpdateReversalFieldData(39, "A0", 2);

        // field 13
        iLength = GetBit(pRspIsoRec, 13, cRspMac);

        if(iLength == 4)
        {
            Trans_UpdateReversalFieldData(13, (char *)cRspMac, 4);
            AppDebug_AppLog("set field 13 for reversal as MAC error");
        }

        PrmMngr_ResetLogonData((void*)0);
        PrnDisp_DisplayBankCodeError(BANK_MACERROR);
        return(BANK_MACERROR);
    }
    else
        return BANK_OK;
}

static int inTrans_GetField64_CheckMac(ISO8583_Rec *pRspIsoRec)
{
    int iLength;
    byte cRspMac[ 9 ], cMac[ 9 ], cRespCode[ 3 ];
    byte cTmpBuf[ ISO8583_MAXLENTH ];

    // 银联，管理类，结算，批上送类交易没有MAC数据
    memset(cRspMac, 0, sizeof(cRspMac));
    ISOEngine_GetBit(pRspIsoRec, 0, cRspMac, sizeof(cRspMac));

    if(memcmp(cRspMac, "03", 2) == 0 ||
       memcmp(cRspMac, "05", 2) == 0 ||
       memcmp(cRspMac, "08", 2) == 0)
    {
        return BANK_OK;
    }

    memset(cRespCode, 0, sizeof(cRespCode));
    ISOEngine_GetBit(pRspIsoRec, 39, cRespCode, sizeof(cRespCode));

    // 获取MAC值
    memset(cRspMac, 0, sizeof(cRspMac));

    // 取不到MAC的情况下
    if(ISOEngine_GetBit(pRspIsoRec, 64, cRspMac, sizeof(cRspMac)) <= 0)
    {
        // 若不成功的交易, MAC域不存在的时候，不需要校验
        if((memcmp(cRespCode, "00", 2) == 0) ||
           (memcmp(cRespCode, "10", 2) == 0) ||
           (memcmp(cRespCode, "11", 2) == 0) ||
           (memcmp(cRespCode, "A2", 2) == 0) ||
           (memcmp(cRespCode, "A4", 2) == 0) ||
           (memcmp(cRespCode, "A5", 2) == 0) ||
           (memcmp(cRespCode, "A6", 2) == 0))
        {
            AppDebug_AppLog("field 64 is empty, set mac error to trans");

            Trans_UpdateReversalFieldData(39, "A0", 2);

            // field 13
            iLength = ISOEngine_GetBit(pRspIsoRec, 13, cRspMac, sizeof(cRspMac));

            if(iLength == 4)
            {
                Trans_UpdateReversalFieldData(13, (char *)cRspMac, 4);
                AppDebug_AppLog("set field 13 for reversal as MAC error");
            }

            PrnDisp_DisplayBankCodeError(BANK_MACERROR);
            return(BANK_MACERROR);
        }
        else if(memcmp(cRespCode, "A0", 2) == 0)
        {
            PrmMngr_ResetLogonData((void*)0);
            return BANK_OK;
        }
        else
            return BANK_OK;
    }

    memset(cMac, 0, sizeof(cMac));
    ISOEngine_SetBit(pRspIsoRec, 64, cMac, 8);

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = ISOEngine_IsoToString(pRspIsoRec, cTmpBuf, sizeof(cTmpBuf));

    if(iLength <= 0)
    {
        PrnDisp_DisplayBankCodeError(BANK_UNPACKERROR);
        return(BANK_UNPACKERROR);
    }

    iLength -= 8;
    KeyMngr_CalMac(cTmpBuf, iLength, cMac);

    if(memcmp(cRspMac, cMac, 8) != 0)
    {
        AppDebug_AppLog("mac error, [%8.8s-%8.8s]", cRspMac, cMac);

        Trans_UpdateReversalFieldData(39, "A0", 2);

        // field 13
        iLength = ISOEngine_GetBit(pRspIsoRec, 13, cRspMac, sizeof(cRspMac));

        if(iLength == 4)
        {
            Trans_UpdateReversalFieldData(13, (char *)cRspMac, 4);
            AppDebug_AppLog("set field 13 for reversal as MAC error");
        }

        PrmMngr_ResetLogonData((void*)0);
        PrnDisp_DisplayBankCodeError(BANK_MACERROR);
        return(BANK_MACERROR);
    }
    else
        return BANK_OK;
}

static int inTrans_CheckPacketValid(DynData *pTxnData, ISO8583_Rec *pRspIsoRec)
{
    byte cTmp[ 26 ];
    byte cMsgId[ 4 ];
    char cBuffer[ 56 ];
    int iLength;

    iLength = 0;

    // Message ID
    memset(cTmp, 0, sizeof(cTmp));
    iLength = ISOEngine_GetBit(pRspIsoRec, 0, cTmp, sizeof(cTmp));
    memcpy(cMsgId, pTxnData->MsgID, 4);
    cMsgId[ 2 ] += 0x01;

    if(memcmp(cTmp, cMsgId, 4) != 0)
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        sprintf(cBuffer, "主机返回消息类型不匹配(%-4.4s<>%-4.4s)", cTmp, cMsgId);
        AppUtils_Warning(cBuffer);
        return BANK_FAIL;
    }

    // Processing Code
    memset(cTmp, 0, sizeof(cTmp));
    iLength = ISOEngine_GetBit(pRspIsoRec, 3, cTmp, sizeof(cTmp));

    if(iLength > 0 && memcmp(cTmp, pTxnData->TransDB.ProcessCode, 6) != 0)
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        sprintf(cBuffer, "主机返回处理码不匹配(%6.6s<>%6.6s)", cTmp, pTxnData->TransDB.ProcessCode);
        AppUtils_Warning(cBuffer);
        return BANK_FAIL;
    }

    // System trace
    memset(cTmp, 0, sizeof(cTmp));
    iLength = ISOEngine_GetBit(pRspIsoRec, 11, cTmp, sizeof(cTmp));

    if((atoi((char *) cTmp) != pTxnData->TransDB.SysTrace) &&
       (pTxnData->TransDB.TrType != BTYPE_DOWNLOAD_PARAMETERS) &&
       (pTxnData->TransDB.TrType != BTYPE_UPLOAD_STATUS) &&
       (pTxnData->TransDB.TrType != BTYPE_ECHO_TESTING) &&
       (pTxnData->TransDB.TrType != BTYPE_LOGON_CASHIER))
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        sprintf(cBuffer, "主机返回流水号不匹配(%6.6s<>%06lu)", cTmp, pTxnData->TransDB.SysTrace);
        AppUtils_Warning(cBuffer);
        return BANK_FAIL;
    }

    // Terminal ID
    memset(cTmp, 0, sizeof(cTmp));
    iLength = ISOEngine_GetBit(pRspIsoRec, 41, cTmp, sizeof(cTmp));

    if(memcmp(cTmp, gTermCfg.mFixPrm.TerminalID, 8) != 0)
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        AppUtils_Warning("主机返回终端号不匹配");
        return BANK_FAIL;
    }

    // Merchant ID
    memset(cTmp, 0, sizeof(cTmp));
    iLength = ISOEngine_GetBit(pRspIsoRec, 42, cTmp, sizeof(cTmp));

    if(iLength > 0 && memcmp(cTmp, gTermCfg.mFixPrm.MerchantID, 15) != 0)
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        AppUtils_Warning("主机返回商户号不匹配");
        return BANK_FAIL;
    }

    return BANK_OK;
}


static int inTrans_Sale_CheckPacket(DynData *pTxnData, ISO_data *pRspIsoRec)
{
    byte cTmp[ 26 ];
    byte cMsgId[ 4 ];
    char cBuffer[ 56 ];
    int iLength;

    iLength = 0;

    // Message ID
    memset(cTmp, 0, sizeof(cTmp));
    //iLength = GetBit(pRspIsoRec, 0, cTmp);
    memcpy(cTmp, pRspIsoRec->message_id, 4);
    memcpy(cMsgId, pTxnData->MsgID, 4);
    cMsgId[ 2 ] += 0x01;

    if(memcmp(cTmp, cMsgId, 4) != 0)
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        sprintf(cBuffer, "主机返回消息类型不匹配(%-4.4s<>%-4.4s)", cTmp, cMsgId);
        AppUtils_Warning(cBuffer);
        return BANK_FAIL;
    }

    // Processing Code
    memset(cTmp, 0, sizeof(cTmp));
    iLength = GetBit(pRspIsoRec, 3, cTmp);

    if(iLength > 0 && memcmp(cTmp, pTxnData->TransDB.ProcessCode, 6) != 0)
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        sprintf(cBuffer, "主机返回处理码不匹配(%6.6s<>%6.6s)", cTmp, pTxnData->TransDB.ProcessCode);
        AppUtils_Warning(cBuffer);
        return BANK_FAIL;
    }

    // System trace
    memset(cTmp, 0, sizeof(cTmp));
    iLength = GetBit(pRspIsoRec, 11, cTmp);

    if((atoi((char *) cTmp) != pTxnData->TransDB.SysTrace) &&
       (pTxnData->TransDB.TrType != BTYPE_DOWNLOAD_PARAMETERS) &&
       (pTxnData->TransDB.TrType != BTYPE_UPLOAD_STATUS) &&
       (pTxnData->TransDB.TrType != BTYPE_ECHO_TESTING) &&
       (pTxnData->TransDB.TrType != BTYPE_LOGON_CASHIER))
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        sprintf(cBuffer, "主机返回流水号不匹配(%6.6s<>%06lu)", cTmp, pTxnData->TransDB.SysTrace);
        AppUtils_Warning(cBuffer);
        return BANK_FAIL;
    }

    // Terminal ID
    memset(cTmp, 0, sizeof(cTmp));
    iLength = GetBit(pRspIsoRec, 41, cTmp);

    if(memcmp(cTmp, gTermCfg.mFixPrm.TerminalID, 8) != 0)
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        AppUtils_Warning("主机返回终端号不匹配");
        return BANK_FAIL;
    }

    // Merchant ID
    memset(cTmp, 0, sizeof(cTmp));
    iLength = GetBit(pRspIsoRec, 42, cTmp);

    if(iLength > 0 && memcmp(cTmp, gTermCfg.mFixPrm.MerchantID, 15) != 0)
    {
        Trans_UpdateReversalFieldData(39, "06", 2);

        AppUtils_Warning("主机返回商户号不匹配");
        return BANK_FAIL;
    }

    return BANK_OK;
}

static int inTrans_UpdateOrignalJournalAdjust(byte bAdjustJournalFlag, DynData *pTxnData)
{
    int iIndex;
    DynData tmpTxnData;

    memset(&tmpTxnData, 0, sizeof(tmpTxnData));
    iIndex = AppUtils_JournalGetRecord(pTxnData->TransDB.OrgSysTrace, &tmpTxnData);

    if(iIndex < 0)
        return BANK_FAIL;

    if(bAdjustJournalFlag == FALSE)
    {
        memcpy(tmpTxnData.TransDB.OrgAmount, tmpTxnData.TransDB.OrgAmount, 12);
        memcpy(tmpTxnData.TransDB.Amount, pTxnData->TransDB.Amount, 12);

        memcpy(tmpTxnData.TransDB.TranDate, pTxnData->TransDB.TranDate, sizeof(tmpTxnData.TransDB.TranDate));
        memcpy(tmpTxnData.TransDB.TranTime, pTxnData->TransDB.TranTime, sizeof(tmpTxnData.TransDB.TranTime));
    }
    else
        AppUtils_SetStatus(&tmpTxnData.TransDB.Status[ 0 ], STATUS_ADJUST);

    if(AppUtils_JournalUpdateRecordByIndex(iIndex, &tmpTxnData) != BANK_OK)
    {
        return BANK_FAIL;
    }

    return BANK_OK;
}

static int inTrans_UploadOfflineBeforeSettlement(byte bTransType)
{
    DynData TransData;

    // 检测是否有脚本需要上送
    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = bTransType;
    Trans_UploadICCscript(&TransData);


    // 检查冲正
    ASSERT_USERCANCEL(Trans_DoReversal(bTransType));

    AppUtils_SetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_BFST);
    PrmMngr_SavePrmToSamDisk();

    // 检查是否有脱机交易需要上送
    ASSERT_FAIL(Trans_SubmitOffline(1));

    AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_BFST);
    PrmMngr_SavePrmToSamDisk();

    // 根据规范，结算前的脚本上送应该在各类交易上送之后 by XC 2014/12/18 12:55:58
    //Trans_UploadICCscript(&TransData);

    PrnDisp_DisplayTransName(bTransType);

    return BANK_OK;
}


static int inTrans_UploadAfterSettlement(DynData *pTxnRec)
{
    int iBatchUploadFlag = 0;
    int iTcARPC0UploadFlag = 1;
    int iTcARPC1UploadFlag = 1;
    int iRetVal;

    // 默认交易成功,因为银联结算没有39域数据返回
    memcpy(pTxnRec->TransDB.RspCode, "00", 2);

    while(gTermCfg.mFixPrm.UploadCurTimes ++ < gTermCfg.mFixPrm.UploadMaxTimes + 1)
    {
        while(1)
        {
            AppDebug_AppLog("gTermCfg.mFixPrm:UploadCurTimes=[%d],UploadMaxTimes=[%d],UploadCurNote=[%d]",
                            gTermCfg.mFixPrm.UploadCurTimes - 1, gTermCfg.mFixPrm.UploadMaxTimes, gTermCfg.mFixPrm.UploadCurNote);

            switch(gTermCfg.mFixPrm.UploadCurNote)
            {
            case 0:
                iBatchUploadFlag = 1;

                //上送磁条卡离线类交易，包括离线结算和结算调整
                ASSERT_HANGUP_FAIL(inTrans_UploadOffline(1, TRAN_SWIP));
                break;

            case 1:
                iBatchUploadFlag = 1;

                //上送全部基于PBOC标准的借/贷记IC卡脱机消费（含小额支付）成功交易
                ASSERT_HANGUP_FAIL(inTrans_UploadOffline(1, TRAN_ICC));
                ASSERT_HANGUP_FAIL(inTrans_UploadOffline(1, TRAN_ECASH));
                ASSERT_HANGUP_FAIL(inTrans_UploadOffline(1, TRAN_QPBOC));
                break;

            case 2:
                iBatchUploadFlag = 1;

                //上送全部基于PBOC标准的电子钱包IC卡脱机消费成功交易
                break;

            case 3:
                iBatchUploadFlag = 1;

                //上送全部磁条卡的请求类联机成功交易明细
                ASSERT_HANGUP_FAIL(inTrans_UploadOnline(pTxnRec));
                break;

            case 4:
                iBatchUploadFlag = 1;

                //上送磁条卡和基于PBOC借/贷记标准IC卡的通知类交易明细
                ASSERT_HANGUP_FAIL(inTrans_UploadNotification(pTxnRec));
                break;

            case 5:
                //上送所有成功的IC卡借贷记联机交易明细
                iRetVal = inTrans_UploadTC(0, pTxnRec);

                if(iRetVal == BANK_FAIL)
                {
                    return BANK_FAIL;
                }
                else if(iRetVal != BANK_DB_FOUND)
                {
                    iTcARPC0UploadFlag = 0;
                }

                break;

            case 6:
                //上送所有失败的脱机消费交易明细,QPBOC
                iRetVal = inTrans_UploadTC(2, pTxnRec);

                if(iRetVal == BANK_FAIL)
                {
                    return BANK_FAIL;
                }
                else if(iRetVal != BANK_DB_FOUND)
                {
                    iTcARPC1UploadFlag = 0;
                }

                break;

            case 7:
                //上送所有ARPC错但卡片仍然承兑的IC卡借贷记联机交易明细
                iRetVal = inTrans_UploadTC(1, pTxnRec);

                if(iRetVal == BANK_FAIL)
                {
                    return BANK_FAIL;
                }
                else if(iRetVal != BANK_DB_FOUND)
                {
                    iTcARPC1UploadFlag = 0;
                }

                break;
            }

            // 保存批结后目前已上送次数,用于断电续传
            gTermCfg.mFixPrm.UploadCurNote ++;
            PrmMngr_SavePrmToSamDisk();

            if(gTermCfg.mFixPrm.UploadCurNote >= 8)
            {
                gTermCfg.mFixPrm.UploadCurNote = 0;
                PrmMngr_SavePrmToSamDisk();
                break;
            }
        }
    }

    if((iBatchUploadFlag == 1) || (iTcARPC0UploadFlag == 1) || (iTcARPC1UploadFlag == 1))
    {
        // 批上送完成交易
        ASSERT_HANGUP_FAIL(inTrans_UploadEnd(pTxnRec));
    }

    return BANK_OK;
}

static int inTrans_UploadOffline(byte bUploadFlag, unsigned short usSwipMode)
{
    DynData TransData;
    TransRecord OrgTransJournal;
    int iLength;
    int i;
    int iRecNum;
    int iCnt;
    int iRet;
    int iResult;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    // bUploadFlag  = 0-批结前脱机上送, 1-批结后脱机批上送
    iRecNum = AppUtils_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));

    if(iRecNum <= 0)
        return BANK_OK;

    iResult = BANK_OK;
    iCnt = 1;

    for(i = 0; i < iRecNum; i ++)
    {
        memset(&TransData, 0, sizeof(TransData));

        if(AppUtils_GetTransRecByIdx(&TransData, i) != BANK_OK)
            break;

        //非脱机交易 不上送
        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_OFFLINE) != BANK_OK)
            continue;

        if(bUploadFlag == 0)
        {
            //成功的脱机交易上送
            if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_UPLOAD) == BANK_OK)
                continue;
        }
        else if(bUploadFlag == 1)
        {
            //成功的脱机交易批上送
            if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_BATCH) == BANK_OK)
                continue;

            // 内卡/外卡只上送结算不平的交易
            if(((memcmp(TransData.TransDB.CardTypeName, "CUP", 3) == 0) && (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK)) ||
               ((memcmp(TransData.TransDB.CardTypeName, "CUP", 3) != 0) && (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK)))
                ;
            else
                continue;
        }
        else
            continue;

        if(usSwipMode == TRAN_SWIP)
        {
            if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.Swipe != TRAN_NUM))
                continue;
        }
        else if(usSwipMode == TRAN_ICC)
        {
            if((TransData.TransDB.Swipe != TRAN_ICC) || (TransData.TransDB.EC_flag == 1))
                continue;
        }
        else if(usSwipMode == TRAN_ECASH)
        {
            if(TransData.TransDB.EC_flag != 1)
                continue;
        }
        else if(usSwipMode == TRAN_QPBOC)
        {
            if((TransData.TransDB.Swipe != TRAN_QPBOC) || TransData.TransDB.qPBOC_OnlineFlag)
                continue;
        }

        memset(&OrgTransJournal, 0, sizeof(TransRecord));
        memcpy(&OrgTransJournal, &TransData.TransDB, sizeof(TransRecord));

        // 显示交易类型
        AppDebug_AppLog("start to do UploadOffline function: uploadflag=%d,swipmode=%d,Cnt=%d,Trace=%06lu",
                        bUploadFlag, usSwipMode, iCnt, TransData.TransDB.SysTrace);

        AppUtils_UserClear(LINE1);

        if(bUploadFlag == 0)
        {
            if(usSwipMode == TRAN_SWIP)
                sprintf((char *)cTmpBuf, "离线上送:%d", iCnt ++);
            else if(usSwipMode == TRAN_ICC)
                sprintf((char *)cTmpBuf, "IC卡脱机上送:%d", iCnt ++);
            else if(usSwipMode == TRAN_ECASH)
                sprintf((char *)cTmpBuf, "ECASH脱机上送:%d", iCnt ++);
            else if(usSwipMode == TRAN_QPBOC)
                sprintf((char *)cTmpBuf, "QPBOC脱机上送:%d", iCnt ++);
            else
                sprintf((char *)cTmpBuf, "脱机上送:%d", iCnt ++);

            AppUtils_DisplayCenter((char *)cTmpBuf, LINE1, TRUE);

            if(AppUtils_WaitKeyMill(100) == bESC)
                return BANK_USERCANCEL;
        }
        else if(bUploadFlag == 1)
        {
            sprintf((char *)cTmpBuf, "批上送脱机:%d", iCnt ++);
            AppUtils_DisplayCenter((char *)cTmpBuf, LINE1, TRUE);
        }

        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        if(bUploadFlag == 0)
        {
            if((TransData.TransDB.TrType == BTYPE_OFFLINE_SETTLE) ||
               (TransData.TransDB.TrType == BTYPE_OFFLINE_ADJUST))
                memcpy(TransData.MsgID, "0220", 4);
            else
                memcpy(TransData.MsgID, "0200", 4);
        }
        else
            memcpy(TransData.MsgID, "0320", 4);

        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 3
        memcpy(TransData.TransDB.ProcessCode, "000000", 6);
        ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

        // field 4
        ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

        // field 11
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 14
        if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
        }

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 25
        memcpy(TransData.TransDB.ServiceCode, "00", 2);
        ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

        // field 26
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);

        if(usSwipMode == TRAN_SWIP)
        {
            // field 37
            if(TransData.TransDB.TrType == BTYPE_OFFLINE_ADJUST)
            {
                if(AppUtils_IsNullData((char *)TransData.TransDB.RefCode, sizeof((char *)TransData.TransDB.RefCode)) != BANK_OK)
                    ISOEngine_SetBit(&ReqIsoRec, 37, TransData.TransDB.RefCode, 12);
            }

            // field 38
            if(AppUtils_IsNullData((char *)TransData.TransDB.AuthCode, sizeof((char *)TransData.TransDB.AuthCode)) != BANK_OK)
                ISOEngine_SetBit(&ReqIsoRec, 38, TransData.TransDB.AuthCode, 6);
        }

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 48
        if(AppUtils_IsNullData((char *)TransData.TransDB.Fee, sizeof((char *)TransData.TransDB.Fee)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 48, TransData.TransDB.Fee, 12);

        // field 49
        ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

        // field 55
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        iLength = EMVtrans_GetEMVICCData(&TransData, cTmpBuf);

        if(iLength > 0)
            ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(TransData.TransDB.TrType == BTYPE_OFFLINE_SETTLE)
            memcpy(cTmpBuf, "30", 2);
        else if(TransData.TransDB.TrType == BTYPE_OFFLINE_ADJUST)
        {
            if(AppUtils_IsNullData((char *)TransData.TransDB.Fee, sizeof((char *)TransData.TransDB.Fee)) == BANK_OK)
                memcpy(cTmpBuf, "32", 2);
            else
                memcpy(cTmpBuf, "34", 2);
        }
        else
            memcpy(cTmpBuf, "36", 2);

        memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);
        memcpy(cTmpBuf + 8, "000", 3);

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        memcpy(cTmpBuf + 12, "0", 1);
        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        // field 61
        if((TransData.TransDB.TrType == BTYPE_OFFLINE_SETTLE) ||
           (TransData.TransDB.TrType == BTYPE_OFFLINE_ADJUST))
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));

            // 61.1+61.2:original batch + original trace
            if(TransData.TransDB.TrType == BTYPE_OFFLINE_SETTLE)
                memcpy(cTmpBuf, "000000000000", 12);
            else
                sprintf((char *)cTmpBuf, "%06ld%06ld", TransData.TransDB.OrgBatchNum, TransData.TransDB.OrgSysTrace);

            // 61.3:original date
            if(AppUtils_IsNullData((char *)TransData.TransDB.OrgTranDate, sizeof((char *)TransData.TransDB.OrgTranDate)) == BANK_OK)
                memcpy(cTmpBuf + 12, "0000", 4);
            else
                memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);

            // 61.4
            sprintf((char *)cTmpBuf + 16, "%02d", TransData.TransDB.AuthType);

            // 61.5
            if(AppUtils_IsNullData((char *)TransData.TransDB.AuthInst, sizeof((char *)TransData.TransDB.AuthInst)) != BANK_OK)
                memcpy(cTmpBuf + 18, TransData.TransDB.AuthInst, 11);

            ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *)cTmpBuf));
        }

        // field 63
        ISOEngine_SetBit(&ReqIsoRec, 63, TransData.TransDB.CardTypeName, 3);

        // field 64
        //if(bUploadFlag == 0)
            //Trans_SetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            AppDebug_AppLog("inTrans_UploadOffline, ISOEngine_IsoToString=[%d]", iLength);
            continue;
        }

        iLength += 11;

        // 检查网络并拨号
        if(Comm_CheckConnect() != BANK_OK)
        {
            if(bUploadFlag == 0)
            {
                return BANK_USERCANCEL;
            }
            else
            {
                return BANK_FAIL;
            }
        }

        // 发送数据包
        if(Comm_SendPacket(cBuffer, iLength) != BANK_OK)
        {
            if(AppUtils_WaitKeyMill(500) == bESC)
            {
                return BANK_FAIL;
            }

            i --;
            //Comm_Disconnect(); //by XC 2014/12/10 15:40:38
            continue;
        }

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        memcpy(TransData.TransDB.RspCode, "ZZ", 2);

        iRet = Comm_RecvPacket(cBuffer, &iLength);

        if(iRet == BANK_OK)
        {
            // 解包
            Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec);
        }
        else if(iRet == BANK_USERCANCEL) //用户取消才可以退出-其他情况应该往后走，以免
        {
            if(bUploadFlag == 0)
            {
                return BANK_USERCANCEL;
            }
        }

        AppDebug_AppLog("inTrans_UploadOffline, TransDB.RspCode=[%s]", TransData.TransDB.RspCode);

        // 脱机交易上送成功
        if((memcmp(TransData.TransDB.RspCode, "00", 2) == 0) ||
           (memcmp(TransData.TransDB.RspCode, "94", 2) == 0))
        {
            if(bUploadFlag == 0)
            {
                if(usSwipMode == TRAN_SWIP)
                {
                    memcpy(OrgTransJournal.OrgRefCode, TransData.TransDB.RefCode, 12);
                    memcpy(OrgTransJournal.OrgAuthCode, TransData.TransDB.AuthCode, 6);
                }

                AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_UPLOAD);
                AppUtils_ResetStatus(&OrgTransJournal.Status[ 1 ], (STATUS_UPLFAIL_TO | STATUS_UPLFAIL_ER));

                if(gTermCfg.mDynPrm.OfflineRecNum > 0)
                {
                    gTermCfg.mDynPrm.OfflineRecNum --;
                    PrmMngr_SavePrmToSamDisk();
                }
            }
            else
            {
                AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_BATCH);

                gTermCfg.mDynPrm.UploadTotalCnt ++;
                PrmMngr_SavePrmToSamDisk();

                AppDebug_AppLog("in uploadoffline, upload successfully, gTermCfg.mDynPrm.UploadTotalCnt=[%ld]", gTermCfg.mDynPrm.UploadTotalCnt);
            }
        }
        else
        {
            if(bUploadFlag == 0)
            {
                OrgTransJournal.BeforeStlUploadCount ++;

                if(memcmp(TransData.TransDB.RspCode, "ZZ", 2) == 0) //接收超时的情况
                {
                    iResult = BANK_FAIL;

                    AppUtils_SetStatus(&OrgTransJournal.Status[ 1 ], STATUS_UPLFAIL_TO);      //接收超时

                    if(OrgTransJournal.BeforeStlUploadCount > gTermCfg.mFixPrm.UploadMaxTimes)
                    {
                        //接收超时次数用光，该笔不再上送，也应返成功
                        iResult = BANK_OK;

                        AppDebug_AppLog("inTrans_UploadOffline, BeforeStlUploadCount=[%d]", OrgTransJournal.BeforeStlUploadCount);

                        AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_UPLOAD);

                        if(gTermCfg.mDynPrm.OfflineRecNum > 0)
                        {
                            gTermCfg.mDynPrm.OfflineRecNum --;
                            PrmMngr_SavePrmToSamDisk();
                        }
                    }
                }
                else    //被主机明确拒绝的情况
                {
                    AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_UPLOAD);          //已上送

                    AppUtils_ResetStatus(&OrgTransJournal.Status[ 1 ], STATUS_UPLFAIL_TO);

                    AppUtils_SetStatus(&OrgTransJournal.Status[ 1 ], STATUS_UPLFAIL_ER);      //主机返回拒绝

                    if(gTermCfg.mDynPrm.OfflineRecNum > 0)
                    {
                        gTermCfg.mDynPrm.OfflineRecNum --;
                        PrmMngr_SavePrmToSamDisk();
                    }
                }
            }
            else
            {
                OrgTransJournal.AfterStlUploadCount ++;

                if(memcmp(TransData.TransDB.RspCode, "ZZ", 2) == 0)
                {
                    iResult = BANK_FAIL;

                    if(OrgTransJournal.AfterStlUploadCount > gTermCfg.mFixPrm.UploadMaxTimes)
                    {
                        //接收超时次数用光，该笔不再上送，也应返成功
                        iResult = BANK_OK;

                        AppDebug_AppLog("inTrans_UploadOffline, AfterStlUploadCount=[%d]", OrgTransJournal.AfterStlUploadCount);

                        AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_BATCH);

                        gTermCfg.mDynPrm.UploadTotalCnt ++;
                        PrmMngr_SavePrmToSamDisk();

                        AppDebug_AppLog("in uploadoffline, upload timeout, gTermCfg.mDynPrm.UploadTotalCnt=[%ld]", gTermCfg.mDynPrm.UploadTotalCnt);
                    }
                }
                else
                {
                    AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_BATCH);

                    gTermCfg.mDynPrm.UploadTotalCnt ++;
                    PrmMngr_SavePrmToSamDisk();

                    AppDebug_AppLog("in uploadoffline, upload reject, gTermCfg.mDynPrm.UploadTotalCnt=[%ld]", gTermCfg.mDynPrm.UploadTotalCnt);
                }
            }
        }

        //if(File_UpdateRecordByIndex((char *)TRAN_DB, (void *)&OrgTransJournal, sizeof(TransRecord), i) != FILE_SUCCESS)
        //{
       //     AppDebug_AppLog("inTrans_UploadOffline, update orignal record=[%d] status_upload fail", i);
       // }
    }

    AppDebug_AppLog("end of inTrans_UploadOffline: swipmode=%02x, return=[%d]", usSwipMode, iResult);

    return iResult;
}


static int inTrans_UploadOnline(DynData * pTxnRec)
{
    DynData TransData;
    int i;
    int j;
    int iLength;
    int iRecNum;
    int iCnt;
    int iOffset;
    int iSuccessFlag;
    byte cField48Data[ 350 ];
    byte cTmpBuf[ 126 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;
    struct upload_rec
    {
        int Cnt;
        int Ident[ 8 ];
    } UploadOnlineRec;

    iRecNum = AppUtils_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));

    if(iRecNum <= 0)
        return  BANK_OK;

    iCnt = 1;
    UploadOnlineRec.Cnt = 0;
    iOffset = 0;
    memset(cField48Data, 0, sizeof(cField48Data));

    for(i = 0; i <= iRecNum; i ++)
    {
        memset(&TransData, 0, sizeof(TransData));

        if(i < iRecNum)
        {
            if(AppUtils_GetTransRecByIdx(&TransData, i) != BANK_OK)
                break;

            // already upload and success
            if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_BATCH) == BANK_OK)
                continue;

            if(TransData.TransDB.Swipe == TRAN_QPBOC)
                continue;

            switch(TransData.TransDB.TrType)
            {
                //正交易
            case BTYPE_SALE:
            case BTYPE_INSTALLMENT:
            case BTYPE_UNION_SCORE_SALE:
            case BTYPE_ISSUER_SCORE_SALE:
            case BTYPE_YUYUE_SALE:
            case BTYPE_ORDER_SALE:
            case BTYPE_MCHIP_SALE:
            case BTYPE_ORDER_COMPLETE:
            case BTYPE_MCHIP_COMPLETE:

                if(TransData.TransDB.Swipe == TRAN_ICC)
                    continue;

                break;

            case BTYPE_COMPLETE: //预授权完成请求，属简易流程，不判断是否IC卡
                break;

                //反交易
            case  BTYPE_SALE_VOID:
            case  BTYPE_COMPLETE_VOID:
            case  BTYPE_INSTALLMENT_VOID:
            case  BTYPE_UNION_SCORE_SALE_VOID:
            case  BTYPE_ISSUER_SCORE_SALE_VOID:
            case  BTYPE_YUYUE_SALE_VOID:
            case  BTYPE_MAG_CASHLOAD:
            case  BTYPE_MAG_CASHLOAD_CONFIRM:
            case BTYPE_ORDER_SALE_VOID:
            case BTYPE_ORDER_COMPLETE_VOID:
            case BTYPE_MCHIP_SALE_VOID:
            case BTYPE_MCHIP_COMPLETE_VOID:
                break;

            default:
                continue;
            }

            if(((memcmp(TransData.TransDB.CardTypeName, "CUP", 3) == 0) && (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK)) ||
               ((memcmp(TransData.TransDB.CardTypeName, "CUP", 3) != 0) && (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK)))
            {

                if(memcmp(TransData.TransDB.CardTypeName, "CUP", 3) == 0)
                    memcpy(cField48Data + iOffset, "00", 2);
                else
                    memcpy(cField48Data + iOffset, "01", 2);

                iOffset += 2;

                sprintf((char *)cField48Data + iOffset, "%06ld", TransData.TransDB.SysTrace);
                iOffset += 6;

                memset(cField48Data + iOffset, '0', 20 - TransData.TransDB.PANLen);
                iOffset += 20 - TransData.TransDB.PANLen;
                memcpy(cField48Data + iOffset, TransData.TransDB.PAN, TransData.TransDB.PANLen);
                iOffset += TransData.TransDB.PANLen;

                memcpy(cField48Data + iOffset, TransData.TransDB.Amount, 12);
                iOffset += 12;

                UploadOnlineRec.Ident[ UploadOnlineRec.Cnt ] = i;
                UploadOnlineRec.Cnt ++;

                if(UploadOnlineRec.Cnt < 8)
                    continue;

            }
        }

        if(UploadOnlineRec.Cnt > 0)
        {
            AppDebug_AppLog("start to do inTrans_UploadOnline function: online txn upload:%d", iCnt);

            sprintf((char *)cTmpBuf, "批上送联机:%d", iCnt ++);
            AppUtils_DisplayCenter((char *)cTmpBuf, LINE1, TRUE);

            memset(&TransData, 0, sizeof(TransData));
            TransData.TransDB.TrType = BTYPE_BATCH_UPLOAD;

            PrmMngr_AddSystrace();

            // 打包
            ISOEngine_ClearAllBits(&ReqIsoRec);
            ISOEngine_ClearAllBits(&RspIsoRec);
            ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

            // field 0
            memcpy(TransData.MsgID, "0320", 4);
            ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

            // field 11
            TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
            sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
            ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

            // field 41
            ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

            // field 42
            ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

            // field 48
            sprintf((char *)cBuffer, "%02d", UploadOnlineRec.Cnt);
            memcpy(cBuffer + 2, cField48Data, UploadOnlineRec.Cnt * 40);
            ISOEngine_SetBit(&ReqIsoRec, 48, cBuffer, UploadOnlineRec.Cnt * 40 + 2);

            // field 60
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            //60.1
            memcpy(cTmpBuf, "00", 2);

            //60.2
            TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
            sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

            //60.3
            memcpy(cTmpBuf + 8, "201", 3);

            ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

            // 组包转换
            memset(cBuffer, 0x00, sizeof(cBuffer));

            // TPDU
            memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

            // CUP Header
            memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

            iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

            if(iLength <= 0)
            {
                AppDebug_AppLog("inTrans_UploadOnline, ISOEngine_IsoToString=[%d]", iLength);
                continue;
            }

            iLength += 11;

            // 检查网络并拨号
            ASSERT_FAIL(Comm_CheckConnect());

            // 发送数据包
            if(Comm_SendPacket(cBuffer, iLength) != BANK_OK)
            {
                if(AppUtils_WaitKeyMill(500) == bESC)
                {
                    return BANK_FAIL;
                }

                continue;
            }

            // 接收数据包
            memset(cBuffer, 0, sizeof(cBuffer));
            iLength = 0;

            memcpy(TransData.TransDB.RspCode, "ZZ", 2);

            if(Comm_RecvPacket(cBuffer, &iLength) == BANK_OK)
            {
                // 解包
                Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec);
            }

            iSuccessFlag = 0;

            // 上送成功或者已经达到上送最大次数
            if((memcmp(TransData.TransDB.RspCode, "ZZ", 2) != 0) ||
               (TransData.TransDB.AfterStlUploadCount > gTermCfg.mFixPrm.UploadMaxTimes))
            {
                iSuccessFlag = 1;

                gTermCfg.mDynPrm.UploadTotalCnt += UploadOnlineRec.Cnt;
                PrmMngr_SavePrmToSamDisk();

                AppDebug_AppLog("in uploadOnline, upload successfully, gTermCfg.mDynPrm.UploadTotalCnt=[%ld]", gTermCfg.mDynPrm.UploadTotalCnt);
            }

            // 更新流水标识
            for(j = 0; j < UploadOnlineRec.Cnt; j ++)
            {
                memset(&TransData, 0, sizeof(TransData));

                if(AppUtils_GetTransRecByIdx(&TransData, UploadOnlineRec.Ident[j]) != BANK_OK)
                {
                    AppDebug_AppLog("inTrans_UploadOnline, read orignal record=[%d] fail", UploadOnlineRec.Ident[j]);
                }

                if(iSuccessFlag == 1)
                {
                    AppUtils_SetStatus(&TransData.TransDB.Status[ 0 ], STATUS_BATCH);
                }
                else
                    TransData.TransDB.AfterStlUploadCount ++;

               // if(File_UpdateRecordByIndex((char *) TRAN_DB, (char *)&TransData, sizeof(TransRecord), UploadOnlineRec.Ident[j]) != FILE_SUCCESS)
              //  {
              //      AppDebug_AppLog("inTrans_UploadOnline, update orignal record=[%d] status_batch fail", UploadOnlineRec.Ident[j]);
              //  }
            }

            UploadOnlineRec.Cnt = 0;
            iOffset = 0;
            memset(cField48Data, 0, sizeof(cField48Data));
        }
    }

    AppDebug_AppLog("end of inTrans_UploadOnline");

    return BANK_OK;
}


static int inTrans_UploadNotification(DynData * pTxnRec)
{
    DynData TransData;
    TransRecord OrgTransJournal;
    int i;
    int iLength;
    int iRecNum;
    int iCnt;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    // iUploadType = 0 - 普通的通知交易
    iRecNum = AppUtils_GetNumberOfRecords((char *) TRAN_DB, sizeof(TransRecord));

    if(iRecNum <= 0)
        return BANK_OK;

    iCnt = 1;

    for(i = 0; i < iRecNum; i ++)
    {
        memset(&TransData, 0, sizeof(TransData));

        if(AppUtils_GetTransRecByIdx(&TransData, i) != BANK_OK)
            break;

        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_BATCH) == BANK_OK)
            continue;

        switch(TransData.TransDB.TrType)
        {
        case BTYPE_REFUND:
        case BTYPE_ECASH_REFUND:
        case BTYPE_MCHIP_REFUND:
        case BTYPE_UNION_SCORE_REFUND:
        case BTYPE_YUYUE_REFUND:
            //case BTYPE_ORDER_REFUND:  //终端规范9.4.6中写明订购退货和订购预授权不上送
        case BTYPE_OFFLINE_COMPLETE:
        case BTYPE_MCHIP_OFFLINE_COMPLETE:
            //case BTYPE_ORDER_OFFLINE_COMPLETE:    //终端规范9.4.6中写明订购退货和订购预授权不上送
            break;
        default:
            continue;
        }

        if(((memcmp(TransData.TransDB.CardTypeName, "CUP", 3) == 0) && (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK)) ||
           ((memcmp(TransData.TransDB.CardTypeName, "CUP", 3) != 0) && (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK)))
        {
            ;
        }
        else
            continue;

        memset(&OrgTransJournal, 0, sizeof(TransRecord));
        memcpy(&OrgTransJournal, &TransData.TransDB, sizeof(TransRecord));

        AppDebug_AppLog("start to do UploadNotification function: Cnt=%d,Trace=%06ld", iCnt, TransData.TransDB.SysTrace);

        sprintf((char *)cTmpBuf, "批上送通知:%d", iCnt ++);
        AppUtils_DisplayCenter((char *)cTmpBuf, LINE1, TRUE);

        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0320", 4);
        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
            ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 3
        ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

        // field 4
        ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

        // field 11
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 14
        if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
            ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
        }

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 25
        ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

        // field 26
        if(TransData.PinExists == 1)
            ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);

        // field 35
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

        // field 36
        if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

        // field 37  原参考号: 普通退货交易用, 电子现金脱机退货不填
        if(AppUtils_IsNullData((char *)TransData.TransDB.OrgRefCode, 12) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 37, TransData.TransDB.OrgRefCode, 12);

        // field 38
        if(AppUtils_IsNullData((char *)TransData.TransDB.OrgAuthCode, 6) != BANK_OK)
            ISOEngine_SetBit(&ReqIsoRec, 38, TransData.TransDB.OrgAuthCode, 6);

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 49
        ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

        // field 53
        Trans_SetField53_Data(&ReqIsoRec, &TransData);

        // field 55 Only for CUP Mobile
        if(TransData.TransDB.Swipe == TRAN_MCHIP)
        {
            if(TransData.TransDB.IssSript71DataLength > 0)
                ISOEngine_SetBit(&ReqIsoRec, 55, TransData.TransDB.IssSript71Data, TransData.TransDB.IssSript71DataLength);
        }

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        //60.1消息类型
        memcpy(cTmpBuf, TransData.TransDB.TransTypeCode, 2);

        //60.2批次号码
        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

        //60.3网络管理信息码
        memcpy(cTmpBuf + 8, "000", 3);

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        //0 未使用或后续子域存在，或手机芯片交易
        //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
        //2 上一笔交易虽是IC卡交易但失败
        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
            memcpy(cTmpBuf + 12, "2", 1);
        else
            memcpy(cTmpBuf + 12, "0", 1);

        if(TransData.TransDB.TrType != BTYPE_OFFLINE_COMPLETE
           && TransData.TransDB.TrType != BTYPE_MCHIP_OFFLINE_COMPLETE
           /*&& TransData.TransDB.TrType != BTYPE_ORDER_OFFLINE_COMPLETE */)
        {
            //60.6 支持部分扣款和返回余额标志
            if(gTermCfg.mTrSwitch.PartPurchaseFlag)
                memcpy(cTmpBuf + 13, "1", 1);
            else
                memcpy(cTmpBuf + 13, "0", 1);
        }

        if(TransData.TransDB.TrType == BTYPE_UNION_SCORE_REFUND)      //联盟积分退货
            strcat((char *)cTmpBuf, "065");

        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        // field 61
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        sprintf((char *)cTmpBuf, "%06ld%06ld", TransData.TransDB.OrgBatchNum, TransData.TransDB.OrgSysTrace);
        memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);
        ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));

        if(TransData.TransDB.TrType != BTYPE_OFFLINE_COMPLETE
           && TransData.TransDB.TrType != BTYPE_MCHIP_OFFLINE_COMPLETE
           /*&& TransData.TransDB.TrType != BTYPE_ORDER_OFFLINE_COMPLETE */)
        {
            // field 62 脱机退货 填写原终端号  参看62域用法十八
            if(AppUtils_IsNullData((char *)TransData.TransDB.OrgTermID, 8) != BANK_OK)
                ISOEngine_SetBit(&ReqIsoRec, 62, TransData.TransDB.OrgTermID, 8);

            // field 63
            ISOEngine_SetBit(&ReqIsoRec, 63, (unsigned char *)"000", 3);
        }

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            AppDebug_AppLog("inTrans_UploadOnline, ISOEngine_IsoToString=[%d]", iLength);
            continue;
        }

        iLength += 11;

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 发送数据包
        if(Comm_SendPacket(cBuffer, iLength) != BANK_OK)
        {
            if(AppUtils_WaitKeyMill(500) == bESC)
            {
                return BANK_FAIL;
            }

            i --;
            continue;
        }

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        memcpy(TransData.TransDB.RspCode, "ZZ", 2);

        if(Comm_RecvPacket(cBuffer, &iLength) == BANK_OK)
        {
            // 解包
            Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec);
        }

        // 上送成功或者已经达到上送最大次数
        if((memcmp(TransData.TransDB.RspCode, "ZZ", 2) != 0) ||
           (TransData.TransDB.AfterStlUploadCount > gTermCfg.mFixPrm.UploadMaxTimes))
        {
            AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_BATCH);

            gTermCfg.mDynPrm.UploadTotalCnt ++;
            PrmMngr_SavePrmToSamDisk();

            AppDebug_AppLog("in uploadnotification, gTermCfg.mDynPrm.UploadTotalCnt=[%ld]", gTermCfg.mDynPrm.UploadTotalCnt);
        }
        else
        {
            OrgTransJournal.AfterStlUploadCount ++;
        }

        // 更新流水标识
      //  if(File_UpdateRecordByIndex((char *) TRAN_DB, (void *)&OrgTransJournal, sizeof(TransRecord), i) != FILE_SUCCESS)
      //  {
     //       AppDebug_AppLog("inTrans_UploadNotification, update orignal record=[%d] status_batch fail", i);
     //   }
    }

    AppDebug_AppLog("end of inTrans_UploadNotification");

    return BANK_OK;
}


static int inTrans_UploadTC(int iUploadType, DynData * pTxnRec)
{
    DynData TransData;
    TransRecord OrgTransJournal;
    int iLength;
    int iCnt;
    int i;
    int iRecNum;
    int iRet = BANK_OK;
    char cDBFileName[ 56 ];
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    // iUploadType: 0-ARPC成功的TC, 1-ARPC失败的TC, 2-失败的IC卡脱机上送
    if(iUploadType == 2)
        sprintf(cDBFileName, "%s", (char *)TRAN_DB_FAIL);
    else
        sprintf(cDBFileName, "%s", (char *)TRAN_DB);

    iRecNum = AppUtils_GetNumberOfRecords(cDBFileName, sizeof(TransRecord));

    if(iRecNum <= 0)
        return BANK_OK;

    iCnt = 1;

    for(i = 0; i < iRecNum; i ++)
    {
        memset(&TransData, 0, sizeof(TransData));

        if(AppUtils_GetRecordByIndex(cDBFileName, (void *)&TransData, sizeof(TransRecord), i) != BANK_OK)
            break;

        if(TransData.TransDB.TrType == BTYPE_ECASH_IN_RELOAD ||
           TransData.TransDB.TrType == BTYPE_ECASH_OUT_RELOAD)
            continue;

        if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_BATCH) == BANK_OK)
            continue;

        if(iUploadType == 0 || iUploadType == 1)
        {
            if(iUploadType != TransData.TransDB.ARPCFlag)
                continue;

            if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_TCUPLOAD) != BANK_OK)
                continue;
        }
        else if(iUploadType == 2)
        {
            if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_OFFLINE) != BANK_OK)
                continue;

            if(memcmp(TransData.TransDB.EMVRspCode, "Z1", 2) != 0)
                continue;
        }
        else
            continue;

        iRet = BANK_DB_FOUND;

        memset(&OrgTransJournal, 0, sizeof(TransRecord));
        memcpy(&OrgTransJournal, &TransData.TransDB, sizeof(TransRecord));

        AppDebug_AppLog("start to do UploadTC uploadtype=[%d], iCnt=%d, Trace=%06lu", iUploadType, iCnt, TransData.TransDB.SysTrace);

        if(iUploadType == 2)
            sprintf((char *)cTmpBuf, "批上送失败脱机%d", iCnt ++);
        else
            sprintf((char *)cTmpBuf, "批上送TC:%d", iCnt ++);

        AppUtils_DisplayCenter((char *)cTmpBuf, LINE1, TRUE);

        // 打包
        ISOEngine_ClearAllBits(&ReqIsoRec);
        ISOEngine_ClearAllBits(&RspIsoRec);
        ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

        // field 0
        memcpy(TransData.MsgID, "0320", 4);
        ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

        // field 2
        ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        // field 4
        ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

        // field 11
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 23
        if(TransData.TransDB.SeqNumFlag == 1)
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            memcpy(cTmpBuf, "00", 2);
            AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
            ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
        }

        // field 41
        ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

        // field 42
        ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

        // field 55
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        //iLength = EMVtrans_GetEMVIccTCUpload(&TransData, cTmpBuf);
        ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);

        // field 60
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        //60.1
        memcpy(cTmpBuf, "00", 2);
        //60.2
        sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

        //60.3
        if(iUploadType == 0)
        {
            if((AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK) ||
               (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK))
                memcpy(cTmpBuf + 8, "205", 3);
            else
                memcpy(cTmpBuf + 8, "203", 3);
        }
        else
        {
            if((AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK) ||
               (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK))
                memcpy(cTmpBuf + 8, "206", 3);
            else
                memcpy(cTmpBuf + 8, "204", 3);
        }

        //60.4终端读取能力
        if(gTermCfg.mTrSwitch.CTLS_Support == 1)
            memcpy(cTmpBuf + 11, "6", 1);
        else
            memcpy(cTmpBuf + 11, "5", 1);

        //60.5 IC卡条件代码
        memcpy(cTmpBuf + 12, "0", 1);
        ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

        // field 62 参考62域用法六,用法七
        memset(cTmpBuf, 0, sizeof(cTmpBuf));

        if(iUploadType == 0)
            memcpy(cTmpBuf, "61", 2);
        else
            memcpy(cTmpBuf, "71", 2);

        if(memcmp(TransData.TransDB.CardTypeName, "CUP", 3) == 0)
            memcpy(cTmpBuf + 2, "00", 2);
        else
            memcpy(cTmpBuf + 2, "01", 2);

        if(TransData.TransDB.ARPCFlag == 1)
            memcpy(cTmpBuf + 4, "05", 2);
        else if(memcmp(TransData.TransDB.RspCode, "Z1", 2) == 0)
            memcpy(cTmpBuf + 4, "11", 2);
        else if(memcmp(TransData.TransDB.RspCode, "Z2", 2) == 0)
            memcpy(cTmpBuf + 4, "13", 2);
        else if(AppUtils_IsNullData((char *)TransData.TransDB.RspCode, 2) == BANK_OK)
            memcpy(cTmpBuf + 4, "11", 2);
        else
            memcpy(cTmpBuf + 4, TransData.TransDB.RspCode, 2);

        memcpy(cTmpBuf + 6, TransData.TransDB.Amount, 12);
        memcpy(cTmpBuf + 18, CURRENCY_CODE, 3);

        if(iUploadType != 0)
        {
            if(TransData.TransDB.ARPCFlag == 1)
                memcpy(cTmpBuf + 21, "22", 2);
            else
                memcpy(cTmpBuf + 21, "10", 2);

            iLength = 23;
        }
        else
            iLength = 21;

        ISOEngine_SetBit(&ReqIsoRec, 62, cTmpBuf, iLength);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

        if(iLength <= 0)
        {
            AppDebug_AppLog("inTrans_UploadOffline, ISOEngine_IsoToString=[%d]", iLength);
            continue;
        }

        iLength += 11;

        // 检查网络并拨号
        ASSERT_FAIL(Comm_CheckConnect());

        // 发送数据包
        if(Comm_SendPacket(cBuffer, iLength) != BANK_OK)
        {
            if(AppUtils_WaitKeyMill(500) == bESC)
            {
                return BANK_FAIL;
            }

            i --;
            continue;
        }

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        memcpy(TransData.TransDB.RspCode, "ZZ", 2);

        if(Comm_RecvPacket(cBuffer, &iLength) == BANK_OK)
        {
            // 解包
            Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec);
        }

        // 上送成功或者已经达到上送最大次数
        if((memcmp(TransData.TransDB.RspCode, "ZZ", 2) != 0) ||
           (TransData.TransDB.AfterStlUploadCount > gTermCfg.mFixPrm.UploadMaxTimes))
        {
            AppUtils_SetStatus(&OrgTransJournal.Status[ 0 ], STATUS_BATCH);

            gTermCfg.mDynPrm.UploadTotalCnt ++;
            PrmMngr_SavePrmToSamDisk();

            AppDebug_AppLog("in uploadTC, upload finished, gTermCfg.mDynPrm.UploadTotalCnt=[%ld]", gTermCfg.mDynPrm.UploadTotalCnt);
        }
        else
        {
            OrgTransJournal.AfterStlUploadCount ++;
        }

        // 更新流水标识
       // if(File_UpdateRecordByIndex(cDBFileName, (void *)&OrgTransJournal, sizeof(TransRecord), i) != FILE_SUCCESS)
       // {
       //     AppDebug_AppLog("UploadTC, update orignal record=[%d] status_batch fail", i);
       // }
    }

    AppDebug_AppLog("end of ARPC=[%d], UploadTC=[%d]", iUploadType, iRet);

    return iRet;
}


static int inTrans_UploadEnd(DynData * pTxnRec)
{
    DynData TransData;
    int iLength;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO8583_Rec ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_SETTLE_END;

    PrmMngr_AddSystrace();

    AppDebug_AppLog("start to do inTrans_UploadEnd");

    // 显示交易类型
    AppUtils_UserClear(LINE1);
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);

    // 打包
    ISOEngine_ClearAllBits(&ReqIsoRec);
    ISOEngine_ClearAllBits(&RspIsoRec);
    ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0320", 4);
    ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 11
    TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
    sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
    ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

    // field 41
    ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 48
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = 4;
    sprintf((char *)cTmpBuf, "%04ld", gTermCfg.mDynPrm.UploadTotalCnt);
    ISOEngine_SetBit(&ReqIsoRec, 48, cTmpBuf, iLength);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    //60.1
    memcpy(cTmpBuf, "00", 2);

    //60.2
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3
    if((AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_CUP) == BANK_OK) ||
       (AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_UPLOAD_FGN) == BANK_OK))
        memcpy(cTmpBuf + 8, "202", 3);
    else
        memcpy(cTmpBuf + 8, "207", 3);

    ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // 组包转换
    memset(cBuffer, 0x00, sizeof(cBuffer));

    // TPDU
    memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

    // CUP Header
    memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

    iLength = ISOEngine_IsoToString(&ReqIsoRec, cBuffer + 5 + 6, sizeof(cBuffer) - 11);

    if(iLength <= 0)
        return BANK_FAIL;

    iLength += 11;

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 增加流水号
    PrmMngr_AddSystrace();

    // 发送数据包
    ASSERT_FAIL(Comm_SendPacket(cBuffer, iLength));

    // 接收数据包
    memset(cBuffer, 0, sizeof(cBuffer));
    iLength = 0;

    ASSERT_FAIL(Comm_RecvPacket(cBuffer, &iLength));

    // 解包
    ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));

    pTxnRec->TransDB.SysTrace = TransData.TransDB.SysTrace;
    pTxnRec->TransDB.BatchNum = TransData.TransDB.BatchNum;
    memcpy(pTxnRec->TransDB.TranDate, TransData.TransDB.TranDate, sizeof(TransData.TransDB.TranDate));
    memcpy(pTxnRec->TransDB.TranTime, TransData.TransDB.TranTime, sizeof(TransData.TransDB.TranTime));

    AppDebug_AppLog("end of inTrans_UploadEnd");

    return BANK_OK;
}

/*
  inTrans_VoidOnline
  模块内撤销函数
  人工撤销时需要确认原流水信息

  lSearchTraceNum: 待撤销的交易流水号
  iHandFlag: 是否人工撤销的标记, 1-人工撤销, 0-自动撤销
*/
static int inTrans_VoidOnline(ulint lSearchTraceNum, int iHandFlag)
{
    DynData TransData;
    int iLength;
    usint usCardMode;
    int iRet;
    byte cTmpBuf[ 512 ];
    byte cBuffer[ ISO8583_MAXLENTH ];
    ISO_data ReqIsoRec, RspIsoRec;

    memset(&TransData, 0, sizeof(TransData));
    TransData.TransDB.TrType = BTYPE_SALE_VOID;

    // 显示交易类型
    AppUtils_Cls();
    PrnDisp_DisplayTransName(TransData.TransDB.TrType);
    qDebug("search result[%d]",lSearchTraceNum);
    if(AppUtils_JournalGetRecord(lSearchTraceNum, &TransData) < 0)
    {
        PrnDisp_DisplayWarning("", "交易不存在");
        return BANK_FAIL;
    }

    // 检查原交易状态
    if(TransData.TransDB.TrType != BTYPE_SALE)
    {
        PrnDisp_DisplayWarning("", "原交易不是消费!");
        return BANK_FAIL;
    }

    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_VOID) == BANK_OK)
    {
        PrnDisp_DisplayWarning("", "该交易已经撤销!");
        return BANK_FAIL;
    }

    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
    {
        PrnDisp_DisplayWarning("", "调整交易不能撤销!");
        return BANK_FAIL;
    }

    if((AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_OFFLINE) == BANK_OK)
       /* && (AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_UPLOAD) != BANK_OK) 脱机交易不管是否上送都不可以撤销 (LiuYan)*/)
    {
        PrnDisp_DisplayWarning("", "脱机交易不能撤销!");
        return BANK_FAIL;
    }

    // 人工撤销标记 需要显示原交易相关信息
    while(iHandFlag == TRUE)
    {
        iRet = PrnDisp_DisplayTxnDetail(&TransData, BTYPE_SALE_VOID);

        if(iRet == bESC)
            return BANK_FAIL;
        else if(iRet == bENTER)
            break;
    }

    AppUtils_Cls();
    PrnDisp_DisplayTransName(BTYPE_SALE_VOID);

    // 复制原交易相关信息
    TransData.TransDB.OrgTranType = TransData.TransDB.TrType;
    TransData.TransDB.TrType = BTYPE_SALE_VOID;
    TransData.TransDB.OrgSysTrace = TransData.TransDB.SysTrace;
    TransData.TransDB.OrgBatchNum = TransData.TransDB.BatchNum;
    memcpy(TransData.TransDB.OrgAuthCode, TransData.TransDB.AuthCode, 6);
    memcpy(TransData.TransDB.OrgRefCode, TransData.TransDB.RefCode, 12);
    AppUtils_Bcd2Asc(TransData.TransDB.TranDate + 2, TransData.TransDB.OrgTranDate, 4);
    TransData.TransDB.Swipe = TRAN_NUM;
    TransData.PinExists = 0;
    TransData.TransDB.EncryptedTrkFlag = 0;
    memcpy(TransData.TransDB.POSEntry, "\x01\x20", 2);
    memset(TransData.TransDB.RspCode, 0, sizeof(TransData.TransDB.RspCode));
    memset(TransData.TransDB.Track2Data, 0, sizeof(TransData.TransDB.Track2Data));
    memset(TransData.TransDB.Track3Data, 0, sizeof(TransData.TransDB.Track3Data));

    //清除原交易中的TC上送标志：撤销交易没有TC，因此不需要上送
    AppUtils_ResetStatus(&TransData.TransDB.Status[0], STATUS_TCUPLOAD);

    if((iHandFlag == TRUE) && (gTermCfg.mTrSwitch.VoidSwipCard))
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, TransData.TransDB.PAN, TransData.TransDB.PANLen);

        usCardMode = TRAN_SWIP | TRAN_ICC;

        if(gTermCfg.mTrSwitch.CTLS_Support)
        {
            usCardMode |= TRAN_QPBOC;
            TransData.EC_CTLSPriOpt = 0;
        }

        // 刷卡/插卡/挥卡，获取卡号
        ASSERT_FAIL(Trans_GetTrackPANData(usCardMode, &TransData));

        // 显示卡号有效期
        ASSERT_FAIL(Trans_ShowCardPAN(&TransData));

        //匹配原卡号是否匹配
        iLength = strlen((char *)cTmpBuf);

        if(TransData.TransDB.PANLen != iLength)
        {
            AppUtils_Warning("卡号不匹配");
            return BANK_FAIL;
        }

        if(memcmp(cTmpBuf, TransData.TransDB.PAN, iLength) != 0)
        {
            AppUtils_Warning("卡号不匹配!");
            return BANK_FAIL;
        }
    }


    // 输入卡密码
    TransData.PinRetryFlag = 0;

    if(iHandFlag == TRUE)
    {
        if(gTermCfg.mTrSwitch.VoidPIN == 1)
        {
            Comm_CheckPreConnect();

            ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
        }
    }

    // 检查网络并拨号
    ASSERT_FAIL(Comm_CheckConnect());

    // 检测是否有脚本需要上送
    Trans_UploadICCscript(&TransData);

    // 检查冲正
    ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));

    // 增加流水号
    PrmMngr_AddSystrace();

    // 打包
    ClearBit(&ReqIsoRec);
    ClearBit(&RspIsoRec);
    //ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);

    // field 0
    memcpy(TransData.MsgID, "0200", 4);
    SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);

    // field 2
    if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
        SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);

    // field 3
    memcpy(TransData.TransDB.ProcessCode, "200000", 6);
    SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);

    // field 4
    SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);

    // field 14
    if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
        SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
    }

    // field 23
    if(TransData.TransDB.SeqNumFlag == 1)
    {
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        memcpy(cTmpBuf, "00", 2);
        AppUtils_Bcd2Asc(&TransData.TransDB.SequenceNum, cTmpBuf + 2, 2);
        SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
    }

    // field 25
    memcpy(TransData.TransDB.ServiceCode, "00", 2);
    SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);

    // field 35
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
        SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));

    // field 36
    if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
        SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));

    // field 37
    SetBit(&ReqIsoRec, 37, TransData.TransDB.OrgRefCode, 12);

    // field 38
    if(AppUtils_IsNullData((char *)TransData.TransDB.OrgAuthCode, 6) != BANK_OK)
        SetBit(&ReqIsoRec, 38, TransData.TransDB.OrgAuthCode, 6);

    // field 41
    SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);

    // field 42
    SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);

    // field 49
    SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);

    // field 60
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    //60.1消息类型
    memcpy(cTmpBuf, "23", 2);
    memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);

    //60.2批次号码
    TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
    sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);

    //60.3网络管理信息码
    memcpy(cTmpBuf + 8, "000", 3);

    //60.4终端读取能力
    if(gTermCfg.mTrSwitch.CTLS_Support == 1)
        memcpy(cTmpBuf + 11, "6", 1);
    else
        memcpy(cTmpBuf + 11, "5", 1);

    //60.5 IC卡条件代码
    //0 未使用或后续子域存在，或手机芯片交易
    //1 上一笔交易不是IC卡交易或是一笔成功的IC卡交易
    //2 上一笔交易虽是IC卡交易但失败
    if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
        memcpy(cTmpBuf + 12, "2", 1);
    else
        memcpy(cTmpBuf + 12, "0", 1);

    //60.6 支持部分扣款和返回余额标志
    if(gTermCfg.mTrSwitch.PartPurchaseFlag)
        memcpy(cTmpBuf + 13, "1", 1);
    else
        memcpy(cTmpBuf + 13, "0", 1);

    SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));

    // field 61
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    sprintf((char *)cTmpBuf, "%06lu", TransData.TransDB.OrgBatchNum);
    sprintf((char *)cTmpBuf + 6, "%06lu", TransData.TransDB.OrgSysTrace);
    SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));

    do
    {
        // 重新输入卡密码
        if(iHandFlag == TRUE)
        {
            if(TransData.PinRetryFlag > 0)
            {
                if(gTermCfg.mTrSwitch.VoidPIN == 1)
                {
                    ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));

                    // 增加流水号
                    PrmMngr_AddSystrace();
                }
                else
                    break;
            }
        }

        // field 11
        TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
        sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
        SetBit(&ReqIsoRec, 11, cTmpBuf, 6);

        // field 22
        memset(cTmpBuf, 0, sizeof(cTmpBuf));
        AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
        SetBit(&ReqIsoRec, 22, cTmpBuf, 3);

        // field 26
        if(TransData.PinExists == 1)
            SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
        else
            ClearBit1(&ReqIsoRec, 26);

        // field 52
        if(TransData.PinExists == 1)
            SetBit(&ReqIsoRec, 52, TransData.PINdata, 8);
        else
            ClearBit1(&ReqIsoRec, 52);

        // field 53
        Trans_SaleSetField53_Data(&ReqIsoRec, &TransData);

        // field 64
        Trans_SaleSetField64_MAC(&ReqIsoRec);

        // 组包转换
        memset(cBuffer, 0x00, sizeof(cBuffer));

        // TPDU
        memcpy(cBuffer, gTermCfg.mFixPrm.TPDU, 5);

        // CUP Header
        memcpy(cBuffer + 5, gTermCfg.mFixPrm.Header, 6);

        iLength = IsoToStr(cBuffer + 5 + 6, &ReqIsoRec);

        if(iLength <= 0)
        {
            Comm_Disconnect();
            return BANK_FAIL;
        }

        iLength += 11;

        // 检查网络并拨号
        if(Comm_CheckConnect() != BANK_OK)
            break;

        // 保存本交易冲正记录
        ASSERT_HANGUP_FAIL(Trans_SetReversal(&TransData, cBuffer, iLength));

        // 发送数据包
        ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));

        // 接收数据包
        memset(cBuffer, 0, sizeof(cBuffer));
        iLength = 0;

        AppUtils_Cls();
        PrnDisp_DisplayTransName(BTYPE_SALE_VOID);
        ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));

        //检查是否应挂线 by XC 2014/12/2 8:40:36
        Trans_CheckNeedDisconnect();

        // 解包
        ASSERT_HANGUP_FAIL(Trans_UnPackData(&TransData, cBuffer, &RspIsoRec));

        // 清除冲正标识
        ASSERT_HANGUP_FAIL(Trans_ClearReversal(REVERSAL_CLEAR_STATUS));

        // 保存交易流水
        AppUtils_SetStatus(&TransData.TransDB.Status[0], STATUS_VOID);

        if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
        {
            Comm_Disconnect();

            Trans_ResetReversalStatus(&TransData);

            Trans_UpdateReversalFieldData(39, "06", 2);

            return BANK_FAIL;
        }
    }
    while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0) && (TransData.PinRetryFlag ++ < 2));

    // 清除冲正标识和流水
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);
    AppUtils_Cls();
    PrnDisp_DisplayTransName(BTYPE_SALE_VOID);
    // 显示处理错误代码
    ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));

    // 更改原交易流水为已撤销
    if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
        AppUtils_JournalUpdateRecordByStatus(TransData.TransDB.OrgSysTrace, 0, STATUS_VOID);
    AppUtils_Cls();
    PrnDisp_DisplayTransName(BTYPE_SALE_VOID);
    // 打印
    PrnDisp_PrintTxn(&TransData, 0);

    // 检查是否有脱机交易需要上送
    Trans_SubmitOffline(0);

    // 挂线
    Comm_Disconnect();

    //if(TransData.TransDB.Swipe == TRAN_ICC)
        EMVtrans_RemoveCard();

    return BANK_OK;
}


static int inTrans_SaveTotalData(TotalRec * pstTotalsData)
{
    int fd;
    int iLen;

    fd = open(TRAN_TOTAL, O_WRONLY | O_CREAT);

    if(fd < 0)
    {
        AppDebug_AppLog("Create TRAN_TOTAL fail");
        //AppUtils_Warning("创建汇总文件失败");
        return BANK_FAIL;
    }

    pstTotalsData->BatchNo = gTermCfg.mDynPrm.BatchNum; //批次号
    strcpy((char*)pstTotalsData->CnName, (char*)gTermCfg.mFixPrm.MerchantNameChn);  // 商户名称-CHINESE
    strcpy((char*)pstTotalsData->EnName, (char*)gTermCfg.mFixPrm.MerchantNameEng);  // 商户名称-ENGLISH
    strcpy((char*)pstTotalsData->MechID, (char*)gTermCfg.mFixPrm.MerchantID);       //商户号
    strcpy((char*)pstTotalsData->TermID, (char*)gTermCfg.mFixPrm.TerminalID);       //终端号
    strcpy((char*)pstTotalsData->OperID, (char*)gTermCfg.mDynPrm.OperID);           //操作员号

    //File_Lseek(fd, 0, SEEK_SET);
    iLen = write(fd, (char *)pstTotalsData, sizeof(TotalRec));

    if(iLen != sizeof(TotalRec))
    {
        AppDebug_AppLog("Write TotalRec fail");
        close(fd);
        //AppUtils_Warning("写汇总数据失败!");
        return BANK_FAIL;
    }

    close(fd);
    return BANK_OK;
}

int Trans_ReadTotalData(TotalRec * pstTotalsData)
{
    int fd;
    int iLen;

    fd = open(TRAN_TOTAL, O_RDONLY);

    if(fd < 0)
    {
        AppDebug_AppLog("Open TRAN_TOTAL fail");
        //AppUtils_UserClear(LINE2);
        //AppUtils_Warning("打开汇总文件失败");
        return BANK_FAIL;
    }

    iLen = read(fd, (char *)pstTotalsData, sizeof(TotalRec));
    close(fd);

    if(iLen != sizeof(TotalRec))
    {
        AppDebug_AppLog("Read TRAN_TOTAL data fail");
        //AppUtils_UserClear(LINE2);
        //AppUtils_Warning("读汇总数据失败");
        return BANK_FAIL;
    }

    return BANK_OK;
}


static int inTrans_LoadConfig(byte * bField62Data, int iField62Length)
{
    int iOffset, iTag;
    byte bTagBuf[2 + 1];
    byte cTempBuf[128];

    iOffset = 0;

    while(iField62Length > iOffset)
    {
        memcpy(bTagBuf, &bField62Data[iOffset], 2);
        bTagBuf[2] = 0;
        iOffset += 2;
        iTag = atoi((char *)bTagBuf);

        switch(iTag)
        {
        case 11:
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 2);
            iOffset += 2;
            //暂时先不更新应用类型
            //AppUtils_Asc2Bcd(( byte * )cTempBuf, ( byte * )gTermCfg.mFixPrm.Header, 2 );
            break;

        case 12: //通讯超时时间
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 2);
            iOffset += 2;

            switch(gTermCfg.mCommPrm.CurrentCommMode)
            {
            case COMM_TYPE_SDLC:
                gTermCfg.mCommPrm.TelTimeout = atoi((char *)cTempBuf);
                break;
            case COMM_TYPE_GPRS:
                gTermCfg.mCommPrm.GprsTimeout = atoi((char *)cTempBuf);
                break;
            case COMM_TYPE_ETHERNET:
                gTermCfg.mCommPrm.EthTimeout = atoi((char *)cTempBuf);
                break;
#ifdef TRIDENT
            case COMM_TYPE_CDMA:
                gTermCfg.mCommPrm.CdmaTimeout = atoi((char *)cTempBuf);
                break;
            case COMM_TYPE_WIFI:
                gTermCfg.mCommPrm.WifiTimeout = atoi((char *)cTempBuf);
                break;
#endif
            case COMM_TYPE_RS232:
            default:
                break;
            }

            break;

        case 13:
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 1);
            gTermCfg.mCommPrm.Retrys = cTempBuf[ 0 ] - 0x30;
            iOffset += 1;

            break;

        case 14:  //电话号码1
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 14);
            iOffset += 14;

            AppUtils_DeleteTrail(' ', (char *)cTempBuf);
            memset(gTermCfg.mCommPrm.TelPhone[ 0 ], 0, sizeof(gTermCfg.mCommPrm.TelPhone[ 0 ]));
            strcpy((char *)gTermCfg.mCommPrm.TelPhone[ 0 ], (char *)cTempBuf);
            break;

        case 15:  //电话号码2
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 14);
            iOffset += 14;

            AppUtils_DeleteTrail(' ', (char *)cTempBuf);
            memset(gTermCfg.mCommPrm.TelPhone[ 1 ], 0, sizeof(gTermCfg.mCommPrm.TelPhone[ 1 ]));
            strcpy((char *)gTermCfg.mCommPrm.TelPhone[ 1 ], (char *)cTempBuf);
            break;

        case 16:  //电话号码3
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 14);
            iOffset += 14;

            AppUtils_DeleteTrail(' ', (char *)cTempBuf);
            memset(gTermCfg.mCommPrm.TelPhone[ 2 ], 0, sizeof(gTermCfg.mCommPrm.TelPhone[ 2 ]));
            strcpy((char *)gTermCfg.mCommPrm.TelPhone[ 2 ], (char *)cTempBuf);
            break;

        case 17:  //管理电话  本架构中暂未使用
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 14);
            iOffset += 14;

            AppUtils_DeleteTrail(' ', (char *)cTempBuf);
            memset(gTermCfg.mFixPrm.ManageTelNo, 0, sizeof(gTermCfg.mFixPrm.ManageTelNo));
            strcpy((char *)gTermCfg.mFixPrm.ManageTelNo, (char *)cTempBuf);
            break;

        case 18:  //是否支持小费
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 1);
            iOffset += 1;

            if(cTempBuf[ 0 ] == '1')
                gTermCfg.mTrSwitch.FeeFlag = 1;
            else
                gTermCfg.mTrSwitch.FeeFlag = 0;

            break;

        case 19:  //小费百分比
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 2);
            iOffset += 2;

            AppUtils_Asc2Bcd((byte *)cTempBuf, &gTermCfg.mFixPrm.FeePercent, 2);
            break;

        case 20:  //手输卡号
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 1);
            iOffset += 1;

            if(cTempBuf[0] == '1')
                gTermCfg.mTrSwitch.ManualFlag = 1;
            else
                gTermCfg.mTrSwitch.ManualFlag = 0;

            break;

        case 21:  //自动签退
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 1);
            iOffset += 1;

            if(cTempBuf[0] == '1')
                gTermCfg.mTrSwitch.AutoLogoutFlag = 1;
            else
                gTermCfg.mTrSwitch.AutoLogoutFlag = 0;

            break;

        case 22:  //商户名称
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 40);
            iOffset += 40;

            AppUtils_DeleteTrail(' ', (char *)cTempBuf);
            strcpy((char *)gTermCfg.mFixPrm.MerchantNameChn, (char *)cTempBuf);
            break;

        case 23:  //交易重发次数
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 1);
            iOffset += 1;
            gTermCfg.mFixPrm.ReversalRetryTimes = cTempBuf[0] - '0';
            break;

        case 25:  //主密钥索引
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 1);
            iOffset += 1;
            gTermCfg.mFixPrm.KeyIndex = cTempBuf[0] - '0';
            break;

        case 26:  //交易开关
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 4);
            iOffset += 4;

            AppDebug_AppLog("switch onoff=[%02X%02X%02X%02X]", cTempBuf[ 0 ], cTempBuf[ 1 ], cTempBuf[ 2 ], cTempBuf[ 3 ]);

            //查询余额
            gTermCfg.mTrSwitch.Balance_Support = (cTempBuf[0] & 0x80) ? 1 : 0;

            //预授权
            gTermCfg.mTrSwitch.PreAuth_Support = (cTempBuf[0] & 0x40) ? 1 : 0;

            //预授权撤销
            gTermCfg.mTrSwitch.PreAuthVoid_Support = (cTempBuf[0] & 0x20) ? 1 : 0;

            //预授权完成(请求)
            gTermCfg.mTrSwitch.CompeleteReq_Support = (cTempBuf[0] & 0x10) ? 1 : 0;

            //预授权完成撤销
            gTermCfg.mTrSwitch.CompeleteVoid_Support = (cTempBuf[0] & 0x08) ? 1 : 0;

            //消费
            gTermCfg.mTrSwitch.Sale_Support = (cTempBuf[0] & 0x04) ? 1 : 0;

            //消费撤销
            gTermCfg.mTrSwitch.Void_Support = (cTempBuf[0] & 0x02) ? 1 : 0;

            //退货
            gTermCfg.mTrSwitch.Refund_Support = (cTempBuf[0] & 0x01) ? 1 : 0;

            //离线结算
            gTermCfg.mTrSwitch.OfflineSettle_Support = (cTempBuf[1] & 0x80) ? 1 : 0;

            //结算调整
            gTermCfg.mTrSwitch.SettleAdjust_Support = (cTempBuf[1] & 0x40) ? 1 : 0;

            //预授权完成(通知)
            gTermCfg.mTrSwitch.CompeleteNote_Support = (cTempBuf[1] & 0x20) ? 1 : 0;

            //脚本处理结果通知
            gTermCfg.mTrSwitch.Script_Support = (cTempBuf[1] & 0x10) ? 1 : 0;

            //电子现金脱机消费
            gTermCfg.mTrSwitch.ECash_Support = (cTempBuf[1] & 0x08) ? 1 : 0;
            gTermCfg.mTrSwitch.CTLS_Support = gTermCfg.mTrSwitch.ECash_Support;

            //(暂不支持)
            //gTermCfg.mTrSwitch.Wallet_Support = (cTempBuf[1] & 0x04)?1:0;

            //基于PBOC电子钱包的圈存类交易 (暂不支持)
            //gTermCfg.mTrSwitch.Wallet_Support = (cTempBuf[1] & 0x02)?1:0;

            //分期付款
            gTermCfg.mTrSwitch.IntallSale_Support = (cTempBuf[1] & 0x01) ? 1 : 0;

            //分期付款撤销
            gTermCfg.mTrSwitch.IntallVoid_Support = (cTempBuf[2] & 0x80) ? 1 : 0;

            //积分消费
            gTermCfg.mTrSwitch.UnionScoreSale_Support = (cTempBuf[2] & 0x40) ? 1 : 0;
            gTermCfg.mTrSwitch.IssuerScoreSale_Support = gTermCfg.mTrSwitch.UnionScoreSale_Support;

            //积分消费撤销
            gTermCfg.mTrSwitch.UnionScoreVoid_Support = (cTempBuf[2] & 0x20) ? 1 : 0;
            gTermCfg.mTrSwitch.IssuerScoreVoid_Support = gTermCfg.mTrSwitch.UnionScoreVoid_Support;

            //基于PBOC借贷记的圈存类交易
            gTermCfg.mTrSwitch.EcashLoad_Support = (cTempBuf[2] & 0x10) ? 1 : 0;    //现金圈存
            gTermCfg.mTrSwitch.EcashInLoad_Support = gTermCfg.mTrSwitch.EcashLoad_Support;  //指定账户圈存
            gTermCfg.mTrSwitch.EcashOutLoad_Support = gTermCfg.mTrSwitch.EcashLoad_Support; //非指定账户圈存


            //预约消费
            gTermCfg.mTrSwitch.YuyueSale_Support = (cTempBuf[2] & 0x08) ? 1 : 0;

            //预约消费撤销
            gTermCfg.mTrSwitch.YuyueVoid_Support = (cTempBuf[2] & 0x04) ? 1 : 0;

            //订购消费
            gTermCfg.mTrSwitch.OrderSale_Support = (cTempBuf[2] & 0x02) ? 1 : 0;

            //订购消费撤销
            gTermCfg.mTrSwitch.OrderVoid_Support = (cTempBuf[2] & 0x01) ? 1 : 0;

            //磁条卡现金(账户)充值类交易
            gTermCfg.mTrSwitch.MagCashLoad_Support = (cTempBuf[3] & 0x80) ? 1 : 0;
            gTermCfg.mTrSwitch.MagAccountLoad_Support = gTermCfg.mTrSwitch.MagCashLoad_Support;
            break;

        case 27:  //离线累计交易笔数
            memset(cTempBuf, 0, sizeof(cTempBuf));
            memcpy(cTempBuf, &bField62Data[iOffset], 2);
            iOffset += 2;

            gTermCfg.mFixPrm.OffUploadNumLimit = atoi((char *)cTempBuf);
            break;

        default:
            iOffset++;
            break;
        }
    }

    PrmMngr_SavePrmToSamDisk();

    return BANK_OK;
}


static byte inTrans_CheckTxnSupport(int iTxnType)
{
    switch(iTxnType)
    {
    case BTYPE_BALANCE:
        //查询余额
        return gTermCfg.mTrSwitch.Balance_Support;

    case BTYPE_PREAUTH:
        //预授权
        return gTermCfg.mTrSwitch.PreAuth_Support ;

    case BTYPE_PREAUTH_VOID:
        //预授权撤销
        return gTermCfg.mTrSwitch.PreAuthVoid_Support;

    case BTYPE_COMPLETE:
        //预授权完成(请求)
        return gTermCfg.mTrSwitch.CompeleteReq_Support;

    case BTYPE_COMPLETE_VOID:
        //预授权完成撤销
        return gTermCfg.mTrSwitch.CompeleteVoid_Support;

    case BTYPE_SALE:
        //消费
        printf("gTermCfg.mTrSwitch.Sale_Support:%d\n", gTermCfg.mTrSwitch.Sale_Support);
        return gTermCfg.mTrSwitch.Sale_Support;

    case BTYPE_SALE_VOID:
        //消费撤销
        return gTermCfg.mTrSwitch.Void_Support;

    case BTYPE_REFUND:
        //退货
        return gTermCfg.mTrSwitch.Refund_Support;

    case BTYPE_ECASH_REFUND: //电子现金退货
        return gTermCfg.mTrSwitch.EcRefund_Support;

    case BTYPE_OFFLINE_SETTLE:
        //离线结算
        return gTermCfg.mTrSwitch.OfflineSettle_Support;

    case BTYPE_OFFLINE_ADJUST:
        //结算调整
        return gTermCfg.mTrSwitch.SettleAdjust_Support;

    case BTYPE_OFFLINE_COMPLETE:
        //预授权完成(通知)
        return gTermCfg.mTrSwitch.CompeleteNote_Support;

    case BTYPE_SCRIPTADVICE:
        //脚本处理结果通知
        return gTermCfg.mTrSwitch.Script_Support;

    case BTYPE_INSTALLMENT:
        //分期付款
        return gTermCfg.mTrSwitch.IntallSale_Support;

    case BTYPE_INSTALLMENT_VOID:
        //分期付款撤销
        return gTermCfg.mTrSwitch.IntallVoid_Support;

    case BTYPE_ISSUER_SCORE_SALE:
        //发卡行积分消费
        return gTermCfg.mTrSwitch.IssuerScoreSale_Support;

    case BTYPE_UNION_SCORE_SALE:
        //联盟积分消费
        return gTermCfg.mTrSwitch.UnionScoreSale_Support;

    case BTYPE_ISSUER_SCORE_SALE_VOID:
        //发卡行积分消费撤销
        return gTermCfg.mTrSwitch.IssuerScoreVoid_Support;

    case BTYPE_UNION_SCORE_SALE_VOID:   //联盟积分消费撤销
        return gTermCfg.mTrSwitch.UnionScoreVoid_Support;

    case BTYPE_UNION_SCORE_BALANCE:     //联盟积分查询
        return gTermCfg.mTrSwitch.UnionScoreBalance_Support;

    case BTYPE_UNION_SCORE_REFUND:      //联盟积分退货
        return gTermCfg.mTrSwitch.UnionScoreRefund_Support;

        //基于PBOC借贷记的圈存类交易
    case BTYPE_ECASH_CASH_RELOAD:       //现金圈存
        return gTermCfg.mTrSwitch.EcashLoad_Support;

    case BTYPE_ECASH_IN_RELOAD:         //指定账户圈存
        return gTermCfg.mTrSwitch.EcashInLoad_Support;

    case BTYPE_ECASH_OUT_RELOAD:        //非指定账户圈存
        return gTermCfg.mTrSwitch.EcashOutLoad_Support;

    case BTYPE_ECASH_CASH_RELOAD_VOID:  //现金充值撤销
        return gTermCfg.mTrSwitch.EcashLoadVoid_Support;

    case BTYPE_YUYUE_SALE:              //预约消费
        return gTermCfg.mTrSwitch.YuyueSale_Support;

    case BTYPE_YUYUE_SALE_VOID:         //预约消费撤销
        return gTermCfg.mTrSwitch.YuyueVoid_Support;

    case BTYPE_ORDER_SALE:                  // 订购消费
        return gTermCfg.mTrSwitch.OrderSale_Support;

    case BTYPE_ORDER_SALE_VOID:             // 订购消费撤销
        return gTermCfg.mTrSwitch.OrderVoid_Support;

    case BTYPE_ORDER_REFUND:                // 订购退货
        return gTermCfg.mTrSwitch.OrderRefund_Support;

    case BTYPE_ORDER_PREAUTH:               // 订购预授权
        return gTermCfg.mTrSwitch.OrderPreAuth_Support;

    case BTYPE_ORDER_PREAUTH_VOID:          // 订购预授权撤销
        return gTermCfg.mTrSwitch.OrderPreAuthVoid_Support;

    case BTYPE_ORDER_COMPLETE:              // 订购预授权完成请求(联机)
        return gTermCfg.mTrSwitch.OrderCompleteReq_Support;

    case BTYPE_ORDER_COMPLETE_VOID:         // 订购预授权完成撤销
        return gTermCfg.mTrSwitch.OrderCompleteVoid_Support;

    case BTYPE_ORDER_OFFLINE_COMPLETE:      // 订购预授权完成通知(联机)
        return gTermCfg.mTrSwitch.OrderCompleteNote_Support;

    case BTYPE_ORDER_CARDHOLDER_VERIFY:     // 订购交易持卡人身份验证
        return gTermCfg.mTrSwitch.OrderVerifyCardHolder;

    case BTYPE_MAG_CASHLOAD:                //磁条卡现金充值交易
        return gTermCfg.mTrSwitch.MagCashLoad_Support;

    case BTYPE_MAG_ACCTLOAD:                //磁条卡账户充值交易
        return gTermCfg.mTrSwitch.MagAccountLoad_Support;

    case BTYPE_MCHIP_BALANCE:              //手机芯片余额查询
        return gTermCfg.mTrSwitch.MChipBalance_Support;

    case BTYPE_MCHIP_SALE:                //手机芯片消费交易
        return gTermCfg.mTrSwitch.MChipSale_Support;

    case BTYPE_MCHIP_SALE_VOID:                //手机芯片消费撤销
        return gTermCfg.mTrSwitch.MChipVoid_Support;

    case BTYPE_MCHIP_REFUND:                //手机芯片退货
        return gTermCfg.mTrSwitch.MChipRefund_Support;

    case BTYPE_MCHIP_PREAUTH:                //手机芯片预授权
        return gTermCfg.mTrSwitch.MChipPreAuth_Support;

    case BTYPE_MCHIP_PREAUTH_VOID:                //手机芯片预授权撤销
        return gTermCfg.mTrSwitch.MChipPreAuthVoid_Support;

    case BTYPE_MCHIP_COMPLETE:                //手机芯片预授权完成请求(联机)
        return gTermCfg.mTrSwitch.MChipCompleteReq_Support;

    case BTYPE_MCHIP_COMPLETE_VOID:                //手机芯片预授权完成撤销
        return gTermCfg.mTrSwitch.MChipCompleteVoid_Support;

    case BTYPE_MCHIP_OFFLINE_COMPLETE:                //手机芯片预授权完成通知(联机)
        return gTermCfg.mTrSwitch.MChipCompleteNote_Support;

    case BTYPE_ECASH_BALANCE:
    default:
        return 1;
    }
}


static int inTrans_GetCUPLicense(char * pcCUPLicense, int iSizeofLicense)
{
    int fd;

    fd = open("F:CUPLICENSE.INI", O_RDONLY);

    if(fd <= 0)
        return BANK_FAIL;

    if(read(fd, pcCUPLicense, iSizeofLicense) > 0)
    {
        if(strchr(pcCUPLicense, 0x0D))
            pcCUPLicense[ strlen(pcCUPLicense) - 1 ] = 0;
        else if(strchr(pcCUPLicense, 0x0A))
            pcCUPLicense[ strlen(pcCUPLicense) - 1 ] = 0;

        close(fd);
        return BANK_OK;
    }
    else
    {
        close(fd);
        return BANK_FAIL;
    }

}


static int inTrans_CheckPrintFailOfflineList(void)
{
    int i, iChoice;
    int iRecNum;
    int iFoundFlag = 0;
    DynData TransData;

    iRecNum = AppUtils_GetNumberOfRecords((char *)TRAN_DB, sizeof(TransRecord));

    if(iRecNum > 0)
    {
        for(i = 0; i < iRecNum; i ++)
        {
            memset(&TransData, 0, sizeof(TransData));

            if(AppUtils_GetTransRecByIdx(&TransData, i) != BANK_OK)
                break;

            //必须是离线交易
            if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_OFFLINE) != BANK_OK)
                continue;

            //必须是上送失败交易
            if((AppUtils_CheckStatus(TransData.TransDB.Status[ 1 ], STATUS_UPLFAIL_TO) == BANK_OK) ||
               (AppUtils_CheckStatus(TransData.TransDB.Status[ 1 ], STATUS_UPLFAIL_ER) == BANK_OK))
            {
                iFoundFlag = 1;
                break;
            }
        }
    }

    if(iFoundFlag == 0)
        return BANK_FAIL;

    iChoice = 1;

    if(AppUtils_YesOrNo("是否打印失败明细", "      0-否  1-是", &iChoice) != BANK_OK)
        return BANK_OK;

    if(iChoice == 1)
        return BANK_OK;
    else
        return BANK_FAIL;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: inTrans_ResolveHolderName
 * DESCRIPTION:   在磁道信息中解析持卡人姓名
 * PARAMETERS:    TrackData: 磁道信息(一般是1磁)  HolderName: 解析到的姓名
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
static int inTrans_ResolveHolderName(char * HolderName, char * TrackData)
{
    int i, StartOffset, EndOffset, iNameLen;
    int TrackDataLen;
    char TempName[30+1];

    StartOffset = EndOffset = 0;
    TrackDataLen = strlen(TrackData);

    for(i = 0; i < TrackDataLen; i++)
    {
        if(TrackData[i] == '^')
        {
            StartOffset = i;
            break;
        }
    }

    if(i >= TrackDataLen)
        return -1;

    StartOffset++;

    for(i = StartOffset; i < TrackDataLen; i++)
    {
        if(TrackData[i] == '^')
        {
            EndOffset = i;
            break;
        }
    }

    if(i >= TrackDataLen)
        return -1;

    if(EndOffset <= StartOffset)
        return -2;

    iNameLen = EndOffset - StartOffset;

    if(iNameLen > 30)
        iNameLen = 30;

    memset(TempName, 0, sizeof(TempName));
    memcpy(TempName, TrackData + StartOffset, iNameLen);

    i = strlen(TempName) - 1;

    while(i >= 0 && TempName[i] == ' ')
    {
        TempName[i] = '\0';
        i--;
    }

    strcpy(HolderName, TempName);

    return BANK_OK;
}
int print_qdebug(unsigned char * Title,unsigned char * buff, int len)
{
        unsigned char printbuff[4096];
        unsigned char temp[32];
        int printlen;
        int i;

        memset(printbuff, 0, sizeof(printbuff));
        for(i = 0; i < len; i++)
        {
            memset(temp, 0, sizeof(temp));
            sprintf((char *)temp,"%02x", (char *)buff[i]);
            strcat((char *)printbuff, (char *)temp);
        }

        qDebug("%s:%s", Title, printbuff);
        return len*2;
}
int EMVtrans_RemoveCard()
{
        CardReader::getInstance()->Qt_pushOutCard();
        return 0;
}
