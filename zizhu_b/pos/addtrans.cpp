/***************************************************************************
* FILE NAME:    ADDTRANS.c                                                 *
* MODULE NAME:  ADDTRANS                                                   *
* PROGRAMMER:   Laikey                                                     *
* DESCRIPTION:  增值业务交易模块                                           *
* REVISION:     01.00 2014-4-14 17:07:42                                   *
****************************************************************************/
#include "cupms.h"


/* -----------------------------------------------------------------------------
* External variables / declarations
---------------------------------------------------------------------------- */
extern Term_Config gTermCfg;
extern const ISO8583_FieldFormat ISO8583_stCUPFieldFormat[];

/* -----------------------------------------------------------------------------
 * Private Function declare
 ---------------------------------------------------------------------------- */


/* -----------------------------------------------------------------------------
* Private definitions
---------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
 * Public Function define
 ---------------------------------------------------------------------------- */


/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_ScoreMenu
 * DESCRIPTION:   积分交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_ScoreMenu(void *dummy)
{
	unsigned char  bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem ScoreMenuItem[] =
	{
		{ NULL,    1, "积分消费",      Trans_Add_ScoreSaleMenu, (void *) 0 },
		{ NULL,    2, "积分消费撤销",  Trans_Add_ScoreVoidMenu, (void *) 0 },
		{ NULL,    3, "联盟积分查询",  Trans_Add_Balance, (void *)BTYPE_UNION_SCORE_BALANCE },
		{ NULL,    4, "联盟积分退货",  Trans_Add_Refund, (void *)BTYPE_UNION_SCORE_REFUND },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "积分");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(ScoreMenuItem) / sizeof(PrnDisp_DisplayMenuItem), ScoreMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_ScoreSaleMenu
 * DESCRIPTION:   积分消费交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_ScoreSaleMenu(void *dummy)
{
	unsigned char  bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem ScoreSaleMenuItem[] =
	{
		{ NULL,    1, "发卡行积分消费",    Trans_Add_Sale, (void *)BTYPE_ISSUER_SCORE_SALE },
		{ NULL,    2, "联盟积分消费",      Trans_Add_Sale, (void *)BTYPE_UNION_SCORE_SALE },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	sprintf(cTitleBuffer, "%s", "积分消费");
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(ScoreSaleMenuItem) / sizeof(PrnDisp_DisplayMenuItem), ScoreSaleMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_ScoreVoidMenu
 * DESCRIPTION:   积分消费撤销交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_ScoreVoidMenu(void *dummy)
{
	unsigned char  bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem ScoreSaleVoidMenuItem[] =
	{
		{ NULL,    1, "发卡行积分撤销",    Trans_Add_Void, (void *)BTYPE_ISSUER_SCORE_SALE_VOID },
		{ NULL,    2, "联盟积分撤销",      Trans_Add_Void, (void *)BTYPE_UNION_SCORE_SALE_VOID },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	sprintf(cTitleBuffer, "%s", "积分撤销");
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(ScoreSaleVoidMenuItem) / sizeof(PrnDisp_DisplayMenuItem), ScoreSaleVoidMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_WalletMenu
 * DESCRIPTION:   钱包交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_WalletMenu(void *dummy)
{
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_IntallmentMenu
 * DESCRIPTION:   分期交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_IntallmentMenu(void *dummy)
{
	unsigned char  bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem IntallmentMenuItem[] =
	{
		{ NULL,    1, "分期消费",      Trans_Add_Sale, (void *)BTYPE_INSTALLMENT },
		{ NULL,    2, "分期消费撤销",  Trans_Add_Void, (void *)BTYPE_INSTALLMENT_VOID },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	sprintf(cTitleBuffer, "%s", "分期");
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(IntallmentMenuItem) / sizeof(PrnDisp_DisplayMenuItem), IntallmentMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_MobileChipMenu
 * DESCRIPTION:   手机芯片交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_MobileChipMenu(void *dummy)
{
	unsigned char  bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem MobileChipMenuItem[] =
	{
		{ NULL,    1, "手机消费",          Trans_Add_Sale, (void *)BTYPE_MCHIP_SALE },
		{ NULL,    2, "手机消费撤销",      Trans_Add_Void, (void *)BTYPE_MCHIP_SALE_VOID },
		{ NULL,    3, "手机芯片退货",      Trans_Add_Refund, (void *)BTYPE_MCHIP_REFUND },
		{ NULL,    4, "手机芯片预授权",    Trans_Add_PreAuth, (void *)BTYPE_MCHIP_PREAUTH },
		{ NULL,    5, "手机预授权撤销",    Trans_Add_PreAuthVoid, (void *)BTYPE_MCHIP_PREAUTH_VOID },
		{ NULL,    6, "预授权完成请求",    Trans_Add_PreAuthCompleteRequest, (void *)BTYPE_MCHIP_COMPLETE },
		{ NULL,    7, "预授权完成通知",    Trans_Add_PreAuthCompleteNote, (void *)BTYPE_MCHIP_OFFLINE_COMPLETE },
		{ NULL,    8, "预授权完成撤销",    Trans_Add_PreAuthCompleteVoid, (void *)BTYPE_MCHIP_COMPLETE_VOID },
		{ NULL,    9, "查询余额",          Trans_Add_Balance, (void *)BTYPE_MCHIP_BALANCE },
	};
	
	if(gTermCfg.mTrSwitch.CTLS_Support == 0)
	{
		AppUtils_Cls();
		AppUtils_Warning("非接支持未打开,暂不能做该类交易");
		return BANK_FAIL;
	}
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "手机芯片");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(MobileChipMenuItem) / sizeof(PrnDisp_DisplayMenuItem), MobileChipMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_YuyueMenu
 * DESCRIPTION:   预约交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_YuyueMenu(void *dummy)
{
	unsigned char  bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem YuyueSaleMenuItem[] =
	{
		{ NULL,    1, "预约消费",          Trans_Add_Sale, (void *)BTYPE_YUYUE_SALE },
		{ NULL,    2, "预约消费撤销",      Trans_Add_Void, (void *)BTYPE_YUYUE_SALE_VOID },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "预约消费");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(YuyueSaleMenuItem) / sizeof(PrnDisp_DisplayMenuItem), YuyueSaleMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_OrderMenu
 * DESCRIPTION:   订购交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_OrderMenu(void *dummy)
{
	unsigned char  bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem OrderMenuItem[] =
	{
		{ NULL,    1, "消费",             Trans_Add_Sale, (void *)BTYPE_ORDER_SALE },
		{ NULL,    2, "消费撤销",         Trans_Add_Void, (void *)BTYPE_ORDER_SALE_VOID },
		{ NULL,    3, "退货",             Trans_Add_Refund, (void *)BTYPE_ORDER_REFUND },
		{ NULL,    4, "预授权",           Trans_Add_PreAuth, (void *)BTYPE_ORDER_PREAUTH },
		{ NULL,    5, "预授权撤销",       Trans_Add_PreAuthVoid, (void *)BTYPE_ORDER_PREAUTH_VOID },
		{ NULL,    6, "预授权完成请求",   Trans_Add_PreAuthCompleteRequest, (void *)BTYPE_ORDER_COMPLETE },
		{ NULL,    7, "预授权完成撤销",   Trans_Add_PreAuthCompleteVoid, (void *)BTYPE_ORDER_COMPLETE_VOID },
		{ NULL,    8, "预授权完成通知",   Trans_Add_PreAuthCompleteNote, (void *)BTYPE_ORDER_OFFLINE_COMPLETE },
		{ NULL,    9, "持卡人信息验证",   Trans_Add_CardholderVerify, (void *)0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "订购");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(OrderMenuItem) / sizeof(PrnDisp_DisplayMenuItem), OrderMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_MagLoadMenu
 * DESCRIPTION:   磁条卡(充值)交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_MagLoadMenu(void *dummy)
{
	unsigned char  bMenuOption;
	char cTitleBuffer[ 26 ];
	PrnDisp_DisplayMenuItem MagLoadMenuItem[] =
	{
		{ NULL,    1, "现金充值",  Trans_Add_MagCashLoad, (void *) 0 },
		{ NULL,    2, "账户充值",  Trans_Add_MagAccountLoad, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	sprintf(cTitleBuffer, "%s", "磁条卡充值");
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(MagLoadMenuItem) / sizeof(PrnDisp_DisplayMenuItem), MagLoadMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_MagCashLoad
 * DESCRIPTION:   磁条卡现金充值
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_MagCashLoad(void *dummy)
{
	DynData TransData;
	int iLength;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	byte bt;
	char LoadAmtLimitBuf[12 + 1];
	char cDispBuf[32];
	
	memset(&TransData, 0, sizeof(DynData));
	TransData.TransDB.TrType = BTYPE_MAG_CASHLOAD;
	
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	// Step 1 持卡人信息验证(磁条卡现金充值账户验证)---------------------------------------
	
	// 刷卡/插卡/挥卡，获取卡号
	TransData.AddBuf[ 200 ] = 'M';
	ASSERT_HANGUP_FAIL(Trans_GetTrackPANData(TRAN_SWIP, &TransData));
	TransData.AddBuf[ 200 ] = 0;
	
	// 显示卡号有效期
	ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));
	
	// 预拨号
	Comm_CheckPreConnect();
	
	//获取身份信息
	ASSERT_HANGUP_FAIL(Trans_Add_GetIDInfo(&TransData));
	
	// 输入金额
	if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
		ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
		
	// 检查网络并拨号
	ASSERT_FAIL(Comm_CheckConnect());
	
	// 检测是否有脚本需要上送
	Trans_UploadICCscript(&TransData);
	
	// 检查冲正
	ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));
	
	// 增加流水号
	PrmMngr_AddSystrace();
	
	// 先做持卡人身份验证(也就是磁条卡充值的账户验证)
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
	memcpy(TransData.TransDB.ProcessCode, "330000", 6);
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
	memcpy((char *)TransData.TransDB.ServiceCode, "00", 2);
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
	
	// field 48 后续交易信息: 00-默认，04-订购，11-预付费卡(磁条卡)充值
	ISOEngine_SetBit(&ReqIsoRec, 48, (unsigned char *)"11", 2);
	
	// field 49
	ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);
	
	// field 53
//	Trans_SetField53_Data(&ReqIsoRec, &TransData);
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	//60.1消息类型
	memcpy(cTmpBuf, "01", 2);
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
	memcpy(cTmpBuf + 12, "0", 1);
	
	//60.6 支持部分扣款和返回余额标志
	if(gTermCfg.mTrSwitch.PartPurchaseFlag)
		memcpy(cTmpBuf + 13, "1", 1);
	else
		memcpy(cTmpBuf + 13, "0", 1);
		
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// field 62
	if(TransData.AddBufLen > 0)
		ISOEngine_SetBit(&ReqIsoRec, 62, TransData.AddBuf, TransData.AddBufLen);
		
	// field 64
//	Trans_SetField64_MAC(&ReqIsoRec);
	
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
	
	// 显示处理错误代码
	ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if(ISOEngine_GetBit(&RspIsoRec, 62, cTmpBuf, sizeof(cTmpBuf)) <= 0)
	{
		Comm_Disconnect();
		AppUtils_Warning("无法获取62域信息");
		return BANK_FAIL;
	}
	
	memset(LoadAmtLimitBuf, 0, sizeof(LoadAmtLimitBuf));
	memcpy(LoadAmtLimitBuf, cTmpBuf, 12);
	
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_AmountAsc2String((byte *)LoadAmtLimitBuf, "", '+', (char *)cTmpBuf);
	sprintf(cDispBuf, "可充:%s元", cTmpBuf);
	AppUtils_DisplayLine(cDispBuf, LINE2);
	
	AppUtils_AmountAsc2String((byte *)TransData.TransDB.Amount, "", '+', (char *)cTmpBuf);
	sprintf(cDispBuf, "充值:%s元", cTmpBuf);
	AppUtils_DisplayLine(cDispBuf, LINE3);
	
	if(memcmp(TransData.TransDB.Amount, LoadAmtLimitBuf, 12) > 0)
		AppUtils_DisplayLine("按确认键调整金额", LINE4);
	else
		AppUtils_DisplayLine("按确认键继续", LINE4);
		
	while(1)
	{
		bt = AppUtils_WaitKey(30);
		
		if(bt == bENTER)
		{
			if(memcmp(TransData.TransDB.Amount, LoadAmtLimitBuf, 12) > 0)
			{
				memset(TransData.TransDB.Amount, 0, sizeof(TransData.TransDB.Amount));
				ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
			}
			
			break;
		}
//		else if(bt == bESC || bt == 0)
		{
			Comm_Disconnect();
			return BANK_FAIL;
		}
	}
	
	if(memcmp(TransData.TransDB.Amount, LoadAmtLimitBuf, 12) > 0)
	{
		Comm_Disconnect();
		AppUtils_Warning((char *)"可充值余额不足");
		return BANK_FAIL;
	}
	
	// Step 2 磁条卡现金充值---------------------------------------------------------------
	TransData.TransDB.TrType = BTYPE_MAG_CASHLOAD;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检查网络并拨号
	ASSERT_FAIL(Comm_CheckConnect());
	
	// 增加流水号
	PrmMngr_AddSystrace();
	
	// 清除请求包所有的域
	ISOEngine_ClearAllBits(&ReqIsoRec);
	
	//验证交易返回的卡片有效期已存入ExpDate中了
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if(AppUtils_IsNullData((char *) TransData.TransDB.ExpDate, sizeof(TransData.TransDB.ExpDate)) != BANK_OK)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_Bcd2Asc(TransData.TransDB.ExpDate + 1, cTmpBuf, 4);
		ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
	}
	
	//验证交易返回的参考号
	if(AppUtils_IsNullData((char *) TransData.TransDB.RefCode, sizeof(TransData.TransDB.RefCode)) != BANK_OK)
	{
		ISOEngine_SetBit(&ReqIsoRec, 37, TransData.TransDB.RefCode, 12);
	}
	
	//验证交易的批次号、流水号
	//规范上的描述有误  应该是批次号在前流水号在后 2011-5-18 10:45:53
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf((char *) cTmpBuf, "%06lu%06lu", TransData.TransDB.BatchNum, TransData.TransDB.SysTrace);
	ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, 12);
	
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
	memcpy((char *)TransData.TransDB.ServiceCode, "00", 2);
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
	
	// field 53
    //Trans_SetField53_Data(&ReqIsoRec, &TransData);
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	//60.1消息类型
	memcpy(cTmpBuf, "48", 2);
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
	memcpy(cTmpBuf + 12, "0", 1);
	
	//60.6 支持部分扣款和返回余额标志
	if(gTermCfg.mTrSwitch.PartPurchaseFlag)
		memcpy(cTmpBuf + 13, "1", 1);
	else
		memcpy(cTmpBuf + 13, "0", 1);
		
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// field 64
	Trans_SetField64_MAC(&ReqIsoRec);
	
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
	
	// 发送数据包
	ASSERT_HANGUP_FAIL(Comm_SendPacket(cBuffer, iLength));
	
	// 接收数据包
	memset(cBuffer, 0, sizeof(cBuffer));
	iLength = 0;
	
	if(Comm_RecvPacket(cBuffer, &iLength) == BANK_OK)
	{
		// 解包
		if(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec) == BANK_OK)
		{
			// 显示处理错误代码
			if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
			{
				// 保存交易流水
				if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
				{
					AppUtils_Warning((char *)"保存交易流水失败");
					return BANK_FAIL;
				}
				
				// 打印
				PrnDisp_PrintTxn(&TransData, 0);
				
				return BANK_OK;
			}
			else if(memcmp(TransData.TransDB.RspCode, "98", 2) == 0)
			{
				//返回98 应往后走发充值确认
				;
			}
			else
			{
				ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));
			}
		}
	}
	
	// Step 3 磁条卡现金充值确认-----------------------------------------------------------
	
	// field 0
	memcpy(TransData.MsgID, "0220", 4);
	ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);
	
	// field 3
	memcpy(TransData.TransDB.ProcessCode, "630000", 6);
	ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);
	
	// field 11
	// 使用现金充值交易的流水号
	sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
	ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);
	
	ISOEngine_ClearOneBit(&ReqIsoRec, 35);
	ISOEngine_ClearOneBit(&ReqIsoRec, 36);
	ISOEngine_ClearOneBit(&ReqIsoRec, 37);
	ISOEngine_ClearOneBit(&ReqIsoRec, 53);
	ISOEngine_ClearOneBit(&ReqIsoRec, 61);
	ISOEngine_ClearOneBit(&ReqIsoRec, 62);
	
	// field 64
	Trans_SetField64_MAC(&ReqIsoRec);
	
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
	
	// 增加流水号
	//PrmMngr_AddSystrace();
	
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
	ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));
	
	// 保存交易流水
	ASSERT_HANGUP_FAIL(AppUtils_JournalWriteRecord(0, &TransData));
	
	// 打印
	PrnDisp_PrintTxn(&TransData, 0);
	
	// 检查是否有脱机交易需要上送
	Trans_SubmitOffline(0);
	
	// 挂线
	Comm_Disconnect();
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_MagAccountLoad
 * DESCRIPTION:   磁条卡账户充值
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_MagAccountLoad(void *dummy)
{
	DynData TransData, TmpTranData;
	int iLength;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	byte bt;
	char LoadAmtLimitBuf[12 + 1], cDispBuf[32];
	
	memset(&TransData, 0, sizeof(DynData));     //转出卡数据保存
	memset(&TmpTranData, 0, sizeof(TmpTranData));     //转入卡数据保存
	
	TransData.TransDB.TrType = BTYPE_MAG_ACCTLOAD;
	TmpTranData.TransDB.TrType = BTYPE_MAG_ACCTLOAD;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TmpTranData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TmpTranData.TransDB.TrType));
	
	// Step 1 持卡人信息验证(磁条卡现金充值账户验证)---------------------------------------
	
	//  获取转出卡卡号(刷卡/插卡)
	TransData.AddBuf[ 0 ] = 'O';
	ASSERT_HANGUP_FAIL(Trans_GetTrackPANData(TRAN_SWIP | TRAN_ICC, &TransData));
	
//	if(TransData.TransDB.Swipe == TRAN_ICC)
//		EMVtrans_RemoveCard();
		
	// 显示卡号有效期
	ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));
	
	// 获取转入卡卡号(刷卡)
	TmpTranData.AddBuf[ 200 ] = 'M';
	ASSERT_HANGUP_FAIL(Trans_GetTrackPANData(TRAN_SWIP, &TmpTranData));
	TmpTranData.AddBuf[ 200 ] = 0;
	
	// 显示卡号有效期
	ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TmpTranData));
	
	TmpTranData.TransDB.BatchNum = TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	
	// 预拨号
	Comm_CheckPreConnect();
	
	//获取身份信息
	ASSERT_HANGUP_FAIL(Trans_Add_GetIDInfo(&TmpTranData));
	
	// 输入金额
	if(AppUtils_IsNullData((char *) TmpTranData.TransDB.Amount, sizeof(TmpTranData.TransDB.Amount)) == BANK_OK)
		ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TmpTranData.TransDB.Amount));
		
	// 检查网络并拨号
	ASSERT_FAIL(Comm_CheckConnect());
	
	// 检测是否有脚本需要上送
	Trans_UploadICCscript(&TmpTranData);
	
	// 检查冲正
	ASSERT_USERCANCEL(Trans_DoReversal(TmpTranData.TransDB.TrType));
	
	// 增加流水号
	PrmMngr_AddSystrace();
	
	// 先做持卡人身份验证(也就是磁条卡充值的账户验证)--------------------------------
	ISOEngine_ClearAllBits(&ReqIsoRec);
	ISOEngine_ClearAllBits(&RspIsoRec);
	ISOEngine_SetFieldFormat(ISO8583_BITMAP64, &ISO8583_stCUPFieldFormat[ 0 ]);
	
	// field 0
	memcpy(TmpTranData.MsgID, "0100", 4);
	ISOEngine_SetBit(&ReqIsoRec, 0, TmpTranData.MsgID, 4);
	
	// field 2
	if((TmpTranData.TransDB.Swipe != TRAN_SWIP) && (TmpTranData.TransDB.PANLen > 0))
		ISOEngine_SetBit(&ReqIsoRec, 2, TmpTranData.TransDB.PAN, TmpTranData.TransDB.PANLen);
		
	// field 3
	memcpy(TmpTranData.TransDB.ProcessCode, "330000", 6);
	ISOEngine_SetBit(&ReqIsoRec, 3, TmpTranData.TransDB.ProcessCode, 6);
	
	// field 4
	ISOEngine_SetBit(&ReqIsoRec, 4, TmpTranData.TransDB.Amount, 12);
	
	// field 11
	TransData.TransDB.SysTrace = TmpTranData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	sprintf((char *) cTmpBuf, "%06lu", TmpTranData.TransDB.SysTrace);
	ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);
	
	// field 14
	if(AppUtils_IsNullData((char *) TmpTranData.TransDB.ExpDate, sizeof(TmpTranData.TransDB.ExpDate)) != BANK_OK)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_Bcd2Asc(TmpTranData.TransDB.ExpDate + 1, cTmpBuf, 4);
		ISOEngine_SetBit(&ReqIsoRec, 14, cTmpBuf, 4);
	}
	
	// field 22
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(TmpTranData.TransDB.POSEntry, cTmpBuf, 4);
	ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);
	
	// field 23
	if(TmpTranData.TransDB.SeqNumFlag == 1)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		memcpy(cTmpBuf, "00", 2);
		AppUtils_Bcd2Asc(&TmpTranData.TransDB.SequenceNum, cTmpBuf + 2, 2);
		ISOEngine_SetBit(&ReqIsoRec, 23, cTmpBuf, 3);
	}
	
	// field 25
	memcpy((char *)TransData.TransDB.ServiceCode, "00", 2);
	ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);
	
	// field 35
	if(AppUtils_IsNullData((char *) TmpTranData.TransDB.Track2Data, sizeof((char *) TmpTranData.TransDB.Track2Data)) != BANK_OK)
		ISOEngine_SetBit(&ReqIsoRec, 35, TmpTranData.TransDB.Track2Data, strlen((char *) TmpTranData.TransDB.Track2Data));
		
	// field 36
	if(AppUtils_IsNullData((char *) TmpTranData.TransDB.Track3Data, sizeof((char *) TmpTranData.TransDB.Track3Data)) != BANK_OK)
		ISOEngine_SetBit(&ReqIsoRec, 36, TmpTranData.TransDB.Track3Data, strlen((char *) TmpTranData.TransDB.Track3Data));
		
	// field 41
	ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);
	
	// field 42
	ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);
	
	// field 48 后续交易信息: 00-默认，04-订购，11-预付费卡(磁条卡)充值
	ISOEngine_SetBit(&ReqIsoRec, 48, (unsigned char *)"11", 2);
	
	// field 49
	ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);
	
	// field 53
	Trans_SetField53_Data(&ReqIsoRec, &TransData);
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	//60.1消息类型
	memcpy(cTmpBuf, "01", 2);
	memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);
	
	//60.2批次号码
	sprintf((char *) cTmpBuf + 2, "%06lu", TmpTranData.TransDB.BatchNum);
	
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
	memcpy(cTmpBuf + 12, "0", 1);
	
	//60.6 支持部分扣款和返回余额标志
	if(gTermCfg.mTrSwitch.PartPurchaseFlag)
		memcpy(cTmpBuf + 13, "1", 1);
	else
		memcpy(cTmpBuf + 13, "0", 1);
		
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// field 62
	if(TmpTranData.AddBufLen > 0)
		ISOEngine_SetBit(&ReqIsoRec, 62, TmpTranData.AddBuf, TmpTranData.AddBufLen);
		
	// field 64
	Trans_SetField64_MAC(&ReqIsoRec);
	
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
	
	// 发送数据包
	ASSERT_FAIL(Comm_SendPacket(cBuffer, iLength));
	
	// 接收数据包
	memset(cBuffer, 0, sizeof(cBuffer));
	iLength = 0;
	
	ASSERT_HANGUP_FAIL(Comm_RecvPacket(cBuffer, &iLength));
	
	// 解包
	ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TmpTranData, cBuffer, &RspIsoRec));
	
	// 显示处理错误代码
	ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TmpTranData));
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if(ISOEngine_GetBit(&RspIsoRec, 62, cTmpBuf, sizeof(cTmpBuf)) <= 0)
	{
		Comm_Disconnect();
		AppUtils_Warning("无法获取62域信息");
		return BANK_FAIL;
	}
	
	memset(LoadAmtLimitBuf, 0, sizeof(LoadAmtLimitBuf));
	memcpy(LoadAmtLimitBuf, cTmpBuf, 12);
	
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TmpTranData.TransDB.TrType);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_AmountAsc2String((byte *)LoadAmtLimitBuf, "", '+', (char *)cTmpBuf);
	sprintf(cDispBuf, "可充:%s元", cTmpBuf);
	AppUtils_DisplayLine(cDispBuf, LINE2);
	
	AppUtils_AmountAsc2String((byte *)TmpTranData.TransDB.Amount, "", '+', (char *)cTmpBuf);
	sprintf(cDispBuf, "充值:%s元", cTmpBuf);
	AppUtils_DisplayLine(cDispBuf, LINE3);
	
	if(memcmp(TmpTranData.TransDB.Amount, LoadAmtLimitBuf, 12) > 0)
		AppUtils_DisplayLine("按确认键调整金额", LINE4);
	else
		AppUtils_DisplayLine("按确认键继续", LINE4);
		
	while(1)
	{
		bt = AppUtils_WaitKey(30);
		
		if(bt == bENTER)
		{
			if(memcmp(TmpTranData.TransDB.Amount, LoadAmtLimitBuf, 12) > 0)
			{
				memset(TmpTranData.TransDB.Amount, 0, sizeof(TmpTranData.TransDB.Amount));
				ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TmpTranData.TransDB.Amount));
			}
			
			break;
		}
		else if(bt == bESC || bt == 0)
		{
			Comm_Disconnect();
			return BANK_FAIL;
		}
	}
	
	if(memcmp(TmpTranData.TransDB.Amount, LoadAmtLimitBuf, 12) > 0)
	{
		Comm_Disconnect();
		AppUtils_Warning((char *)"可充值余额不足");
		return BANK_FAIL;
	}
	
	// Step 2 磁条卡账户充值---------------------------------------------------------------
	TransData.TransDB.TrType = BTYPE_MAG_ACCTLOAD;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
	
	// 检查网络并拨号
	ASSERT_FAIL(Comm_CheckConnect());
	
	// 检测是否有脚本需要上送
	Trans_UploadICCscript(&TransData);
	
	// 检查冲正
	ASSERT_USERCANCEL(Trans_DoReversal(TransData.TransDB.TrType));
	
	// 增加流水号
	//PrmMngr_AddSystrace();
	
	// 清除请求包所有的域
	ISOEngine_ClearAllBits(&ReqIsoRec);
	
	// field 0
	memcpy(TransData.MsgID, "0200", 4);
	ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);
	
	// field 2
	if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
		ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);
		
	// field 3
	memcpy(TransData.TransDB.ProcessCode, "400000", 6);
	ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);
	
	// field 4
	memcpy(TransData.TransDB.Amount, TmpTranData.TransDB.Amount, 12);
	ISOEngine_SetBit(&ReqIsoRec, 4, TransData.TransDB.Amount, 12);
	
	// field 11
	// 与验证报文流水号一致
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
	memcpy((char *)TransData.TransDB.ServiceCode, "66", 2);
	ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);
	
	if(TransData.TransDB.Swipe == TRAN_SWIP)
	{
		// 转出卡是磁条卡的时候要上送, IC卡的时候不送
		// field 35
		if(AppUtils_IsNullData((char *) TransData.TransDB.Track2Data, sizeof((char *) TransData.TransDB.Track2Data)) != BANK_OK)
			ISOEngine_SetBit(&ReqIsoRec, 35, TransData.TransDB.Track2Data, strlen((char *) TransData.TransDB.Track2Data));
			
		// field 36
		if(AppUtils_IsNullData((char *) TransData.TransDB.Track3Data, sizeof((char *) TransData.TransDB.Track3Data)) != BANK_OK)
			ISOEngine_SetBit(&ReqIsoRec, 36, TransData.TransDB.Track3Data, strlen((char *) TransData.TransDB.Track3Data));
	}
	
	// field 26
	if(TransData.PinExists == 1)
		ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *) "12", 2);
		
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
	memcpy(cTmpBuf, "49", 2);
	memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);
	
	//60.2批次号码
	sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);
	
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// field 62  存放转入卡卡号
	if(TmpTranData.TransDB.PANLen > 0)
	{
		ISOEngine_SetBit(&ReqIsoRec, 62, TmpTranData.TransDB.PAN, TmpTranData.TransDB.PANLen);
		
		TransData.TransDB.PAN_2Len = TmpTranData.TransDB.PANLen;
		memcpy(TransData.TransDB.PAN_2, TmpTranData.TransDB.PAN, TransData.TransDB.PAN_2Len);
	}
	
	// field 64
	Trans_SetField64_MAC(&ReqIsoRec);
	
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
	
	// 发送数据包
	ASSERT_FAIL(Comm_SendPacket(cBuffer, iLength));
	
	// 接收数据包
	memset(cBuffer, 0, sizeof(cBuffer));
	iLength = 0;
	
	if(Comm_RecvPacket(cBuffer, &iLength) == BANK_OK)
	{
		Trans_CheckNeedDisconnect();
		
		// 解包
		ASSERT_HANGUP_FAIL(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec));
		
		ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));
		
		// 显示处理错误代码
		if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
		{
			// 保存交易流水
			if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
			{
				Comm_Disconnect();
				AppUtils_Warning((char *)"保存交易流水失败");
				return BANK_FAIL;
			}
			
			// 打印
			PrnDisp_PrintTxn(&TransData, 0);
			
			// 检查是否有脱机交易需要上送
			Trans_SubmitOffline(0);
			
			// 挂线
			Comm_Disconnect();
		}
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_Sale
 * DESCRIPTION:   复用的增值业务消费
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_Sale(void *dummy)
{
	DynData TransData;
	int iRet;
	int iLength;
	usint usCardMode;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	
	TransData.TransDB.TrType = (int)dummy;
	
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	//根据交易类型，选择不同的读卡方式
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_INSTALLMENT:       //分期付款
		usCardMode = TRAN_SWIP | TRAN_ICC;
		break;
	case BTYPE_ISSUER_SCORE_SALE: //发卡行积分消费
	case BTYPE_UNION_SCORE_SALE:  //联盟积分消费
		usCardMode = TRAN_SWIP | TRAN_ICC;
		break;
	case BTYPE_YUYUE_SALE:        //预约消费
		usCardMode = TRAN_NOCARD;
		break;
	case BTYPE_MCHIP_SALE:        //手机芯片消费
		usCardMode = TRAN_MCHIP;
		break;
	case BTYPE_ORDER_SALE:        //订购消费
		usCardMode = TRAN_NUM;
		
		// 输入金额
		if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
			ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
			
		break;
	default:
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
	ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usCardMode, &TransData));
	
	// 显示卡号有效期
	ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));
	
	//根据交易类型，采集、录入不同的内容
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_INSTALLMENT:       //分期付款
		ASSERT_HANGUP_FAIL(Trans_Add_GetInstallmentInfo(&TransData));
		break;
	case BTYPE_ISSUER_SCORE_SALE: //发卡行积分消费
	case BTYPE_UNION_SCORE_SALE:  //联盟积分消费
		ASSERT_HANGUP_FAIL(Trans_Add_GetScoreInfo(&TransData));
		break;
	case BTYPE_YUYUE_SALE:        //预约消费
		ASSERT_HANGUP_FAIL(Trans_Add_GetYuyueInfo(&TransData));
		break;
	case BTYPE_MCHIP_SALE:        //手机芯片消费
		break;
	case BTYPE_ORDER_SALE:        //订购消费
		ASSERT_HANGUP_FAIL(Trans_Add_GetOrderInfo(&TransData));
		break;
	default:
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
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
			ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
			
		// 输入卡密码
		TransData.PinRetryFlag = 0;
		
		if(TransData.TransDB.Swipe != TRAN_ICC)
		{
			if(TransData.TransDB.TrType != BTYPE_ORDER_SALE)
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
		switch(TransData.TransDB.TrType)
		{
		case BTYPE_ISSUER_SCORE_SALE:
		case BTYPE_UNION_SCORE_SALE:
			strcpy((char *)TransData.TransDB.ServiceCode, "65");
			break;
		case BTYPE_INSTALLMENT:
			strcpy((char *)TransData.TransDB.ServiceCode, "64");
			break;
		case BTYPE_YUYUE_SALE:
			strcpy((char *)TransData.TransDB.ServiceCode, "67");
			break;
		case BTYPE_ORDER_SALE:
			strcpy((char *)TransData.TransDB.ServiceCode, "08");
			break;
		default:
			strcpy((char *)TransData.TransDB.ServiceCode, "00");
			break;
		}
		
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
//			iLength = EMVtrans_GetEMVICCData(&TransData, cTmpBuf);
			ISOEngine_SetBit(&ReqIsoRec, 55, cTmpBuf, iLength);
		}
		else if(TransData.TransDB.Swipe == TRAN_MCHIP)
		{
			ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
			
			TransData.AddBufLen = 0;  //避免后面62域打包上送
		}
		
		// field 60
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		//60.1消息类型
		switch(TransData.TransDB.TrType)
		{
		case BTYPE_YUYUE_SALE:          //预约消费
			memcpy(cTmpBuf, "54", 2);
			break;
		default:
			memcpy(cTmpBuf, "22", 2);
			break;
		}
		
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
		memcpy(cTmpBuf + 12, "0", 1);
		
		//60.6 支持部分扣款和返回余额标志
		if(gTermCfg.mTrSwitch.PartPurchaseFlag)
			memcpy(cTmpBuf + 13, "1", 1);
		else
			memcpy(cTmpBuf + 13, "0", 1);
			
		switch(TransData.TransDB.TrType)
		{
		case BTYPE_ISSUER_SCORE_SALE: //发卡行积分消费
			strcat((char *)cTmpBuf, "048");
			break;
		case BTYPE_UNION_SCORE_SALE:  //联盟积分消费
			strcat((char *)cTmpBuf, "065");
			break;
		default:
			break;
		}
		
		ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
		
		// Field 62
		if(TransData.AddBufLen > 0)
			ISOEngine_SetBit(&ReqIsoRec, 62, (byte *)TransData.AddBuf, TransData.AddBufLen);
			
		do
		{
			// 重新输入卡密码
			if(TransData.PinRetryFlag > 0)
			{
				if(TransData.TransDB.TrType != BTYPE_ORDER_SALE)
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
			Trans_SetField64_MAC(&ReqIsoRec);
			
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
			if(Trans_SetReversal(&TransData, cBuffer, iLength) != BANK_OK)
			{
				Comm_Disconnect();
				return BANK_FAIL;
			}
			
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
			if(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec) != BANK_OK)
				break;
				
			// 清除冲正标识
			if(Trans_ClearReversal(REVERSAL_CLEAR_STATUS) != BANK_OK)
				break;
				
			// 处理62域的增值业务数据
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			iLength = ISOEngine_GetBit(&RspIsoRec, 62, cTmpBuf, sizeof(cTmpBuf));
			
			if(iLength > 0)
			{
				if(TransData.TransDB.TrType == BTYPE_INSTALLMENT)
				{
					//首期还款金额(必填)
					memcpy(TransData.TransDB.InstallmentFirstAmt, (char *)cTmpBuf, 12);
					
					//还款币种(必填)
					memcpy(TransData.TransDB.InstallmentCurrency, (char *)cTmpBuf + 12, 3);
					
					//持卡人手续费(一次性支付手续费用)
					memcpy(TransData.TransDB.InstallmentHolderFee, (char *)cTmpBuf + 15, 12);
					
					//奖励积分
					memcpy(TransData.TransDB.BonusScore, (char *)cTmpBuf + 27, 12);
					AppUtils_DeleteTrail(' ', (char *)TransData.TransDB.BonusScore);
					
					//手续费支付方式
					TransData.TransDB.InstallmentFeePayway = cTmpBuf[39] - '0';
					
					if(iLength > 40)
					{
						//首期手续费
						memcpy(TransData.TransDB.InstallmentFirstFee, (char *)cTmpBuf + 40, 12);
						
						//每期手续费
						memcpy(TransData.TransDB.InstallmentPerFee, (char *)cTmpBuf + 52, 12);
					}
				}
				else if(TransData.TransDB.TrType == BTYPE_YUYUE_SALE)
				{
					//预约号 90+手机号+预约号，补齐空格到20位
					memcpy(TransData.TransDB.YuYuePhoneNum, cTmpBuf + 2, 11);
					memcpy(TransData.TransDB.YuYueCode, cTmpBuf + 13, 6);
				}
				else if(TransData.TransDB.TrType == BTYPE_UNION_SCORE_SALE
				        || TransData.TransDB.TrType == BTYPE_ISSUER_SCORE_SALE)
				{
					byte cBalanceData[ 32 ];
					
					//兑换积分数
					memcpy(TransData.TransDB.ConvertedScore, "00", 2);
					memcpy(TransData.TransDB.ConvertedScore + 2, cTmpBuf, 10);
					
					//自负金额
					memcpy((char *)TransData.TransDB.ScoreSelfPaidAmt, (char *)cTmpBuf + 10, 12);
					
					//积分余额数
					memset(cBalanceData, 0, sizeof(cBalanceData));
					
					if(ISOEngine_GetBit(&RspIsoRec, 54, cBalanceData, sizeof(cBalanceData)) > 0
					   && memcmp(cBalanceData + 4, "999", 3) == 0)
					{
						memcpy((char *)TransData.TransDB.ScoreBalance, (char *)cBalanceData + 8, 12);
					}
					else
					{
						memcpy(TransData.TransDB.ScoreBalance, "00", 2);
						memcpy(TransData.TransDB.ScoreBalance + 2, (char *)cTmpBuf + 22, 10);
					}
				}
			}
			
			// 保存交易流水
			if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
			{
				Comm_Disconnect();
				Trans_ResetReversalStatus(&TransData);
				
				memcpy(TransData.TransDB.RspCode, "30", 2);
				Trans_UpdateReversalFieldData(39, "96", 2);
				break;
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
			iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));
			
//			EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
		}
		
		// 清除冲正标识和流水
		if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
			Trans_ClearReversal(REVERSAL_CLEAR_STATUS | REVERSAL_CLEAR_DATA);
	}
	
	// 显示处理错误代码
	ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));
	
	// 打印
	PrnDisp_PrintTxn(&TransData, 0);
	
	// 检查是否有脱机交易需要上送
	Trans_SubmitOffline(0);
	
	// 挂线
	Comm_Disconnect();
	
//	if(TransData.TransDB.Swipe == TRAN_ICC)
//		EMVtrans_RemoveCard();
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_Void
 * DESCRIPTION:   复用的增值业务撤销
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_Void(void *dummy)
{
	DynData TransData;
	int iLength;
	int iCurrTranType;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	usint usOrgSwipeMode, usSwipeMode = 0;
	
	iCurrTranType = (int)dummy;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(iCurrTranType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(iCurrTranType));
	
	// 输入主管密码
	if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
		return BANK_FAIL;
		
	// 获取原交易数据
	memset(&TransData, 0, sizeof(TransData));
	ASSERT_FAIL(Trans_GetOrignalJournal("请输入原凭证号:", &TransData));
	
	// 检查原交易状态
	switch(iCurrTranType)
	{
		//分期付款撤销
	case BTYPE_INSTALLMENT_VOID:
	
		if(TransData.TransDB.TrType != BTYPE_INSTALLMENT)
		{
			PrnDisp_DisplayWarning("", "原交易非分期付款");
			return BANK_FAIL;
		}
		
		break;
		//预约消费撤销
	case BTYPE_YUYUE_SALE_VOID:
	
		if(TransData.TransDB.TrType != BTYPE_YUYUE_SALE)
		{
			PrnDisp_DisplayWarning("", "原交易非预约消费");
			return BANK_FAIL;
		}
		
		break;
	case BTYPE_ISSUER_SCORE_SALE_VOID:
	
		if(TransData.TransDB.TrType != BTYPE_ISSUER_SCORE_SALE)
		{
			PrnDisp_DisplayWarning("", "非发卡行积分消费");
			return BANK_FAIL;
		}
		
		break;
	case BTYPE_UNION_SCORE_SALE_VOID:
	
		if(TransData.TransDB.TrType != BTYPE_UNION_SCORE_SALE)
		{
			PrnDisp_DisplayWarning("", "非联盟积分消费");
			return BANK_FAIL;
		}
		
		break;
	case BTYPE_MCHIP_SALE_VOID:
	
		if(TransData.TransDB.TrType != BTYPE_MCHIP_SALE)
		{
			PrnDisp_DisplayWarning("", "非手机芯片消费");
			return BANK_FAIL;
		}
		
		break;
	case BTYPE_ORDER_SALE_VOID:
	
		if(TransData.TransDB.TrType != BTYPE_ORDER_SALE)
		{
			PrnDisp_DisplayWarning("", "原交易非订购消费");
			return BANK_FAIL;
		}
		
		break;
	default:
		AppUtils_FormatWarning((char *)"无效的交易类型%d", iCurrTranType);
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
	
	if((AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_OFFLINE) == BANK_OK) &&
	   (AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_UPLOAD) != BANK_OK))
	{
		PrnDisp_DisplayWarning("", "脱机交易不能撤销!");
		return BANK_FAIL;
	}
	
	// 显示原交易相关信息
	if(PrnDisp_DisplayTxnDetail(&TransData, iCurrTranType) == bESC)
		return BANK_FAIL;
		
	AppUtils_Cls();
	PrnDisp_DisplayTransName(iCurrTranType);
	
	// 复制原交易相关信息
	TransData.TransDB.OrgTranType = TransData.TransDB.TrType; //保存原交易类型
	TransData.TransDB.TrType = iCurrTranType; //交易类型设为当前交易类型
	TransData.TransDB.OrgSysTrace = TransData.TransDB.SysTrace;
	TransData.TransDB.OrgBatchNum = TransData.TransDB.BatchNum;
	memcpy(TransData.TransDB.OrgAuthCode, TransData.TransDB.AuthCode, 6);
	memcpy(TransData.TransDB.OrgRefCode, TransData.TransDB.RefCode, 12);
	AppUtils_Bcd2Asc(TransData.TransDB.TranDate + 2, TransData.TransDB.OrgTranDate, 4);
	usOrgSwipeMode = TransData.TransDB.Swipe;
	TransData.PinExists = 0;
	TransData.TransDB.POSEntry[ 1 ] = 0x20;
	TransData.TransDB.EncryptedTrkFlag = 0;
	memset(TransData.TransDB.RspCode, 0, sizeof(TransData.TransDB.RspCode));
	memset(TransData.TransDB.Track2Data, 0, sizeof(TransData.TransDB.Track2Data));
	memset(TransData.TransDB.Track3Data, 0, sizeof(TransData.TransDB.Track3Data));
	
	//清除原交易中的TC上送标志：撤销交易不需要上送TC
	AppUtils_ResetStatus(&TransData.TransDB.Status[0], STATUS_TCUPLOAD);
	
	if(usOrgSwipeMode != TRAN_NOCARD && usOrgSwipeMode != TRAN_MCHIP)
	{
		TransData.TransDB.Swipe = TRAN_NUM;
		memcpy(TransData.TransDB.POSEntry, "\x01\x20", 2);
	}
	
	//原交易中有主账号的，默认先设置刷卡模式为"手输卡号"
	if(TransData.TransDB.PANLen > 0)
		TransData.TransDB.Swipe = TRAN_NUM;
		
	if((usOrgSwipeMode & TRAN_ICC) || (usOrgSwipeMode & TRAN_SWIP))
	{
		usSwipeMode = TRAN_SWIP | TRAN_ICC;
	}
	else if(usOrgSwipeMode & TRAN_MCHIP)
	{
		usSwipeMode = TRAN_MCHIP;
	}
	
	if(TransData.TransDB.OrgTranType == BTYPE_YUYUE_SALE)
	{
		usSwipeMode = 0;
		//原交易中返回了主账号，预约撤销交易，是否应属于手输卡号类型?以便在53域标识pin是带主账号运算的?
		//TransData.TransDB.Swipe = TRAN_NOCARD;
	}
	
	// 撤销交易是否刷卡
	if(usSwipeMode && gTermCfg.mTrSwitch.VoidSwipCard)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		memcpy(cTmpBuf, TransData.TransDB.PAN, TransData.TransDB.PANLen);
		
		// 刷卡/插卡/挥卡，获取卡号
		ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));
		
		// 显示卡号有效期
		ASSERT_FAIL(Trans_ShowCardPAN(&TransData));
		
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
	
	// 输入密码
	TransData.PinRetryFlag = 0;
	
	if(gTermCfg.mTrSwitch.VoidPIN == 1)
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
	switch(iCurrTranType)
	{
	case BTYPE_INSTALLMENT_VOID:
		strcpy((char *)TransData.TransDB.ServiceCode, "64");
		break;
	case BTYPE_YUYUE_SALE_VOID:
		strcpy((char *)TransData.TransDB.ServiceCode, "67");
		break;
	case BTYPE_ISSUER_SCORE_SALE_VOID:
	case BTYPE_UNION_SCORE_SALE_VOID:
		strcpy((char *)TransData.TransDB.ServiceCode, "65");
		break;
	case BTYPE_ORDER_SALE_VOID:
		strcpy((char *)TransData.TransDB.ServiceCode, "08");
		break;
	case BTYPE_MCHIP_SALE_VOID:
		strcpy((char *)TransData.TransDB.ServiceCode, "00");
		break;
	default:
		Comm_Disconnect();
		AppUtils_FormatWarning((char *)"无效的交易类型%d", iCurrTranType);
		return BANK_FAIL;
	}
	
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
	
	// field 55 Only for CUP Mobile
	if(TransData.TransDB.Swipe == TRAN_MCHIP)
	{
		ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
		TransData.AddBufLen = 0;
	}
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	//60.1消息类型
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_YUYUE_SALE_VOID:       //预约消费撤销
		memcpy(cTmpBuf, "53", 2);
		break;
	default:
		memcpy(cTmpBuf, "23", 2);
		break;
	}
	
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
	memcpy(cTmpBuf + 12, "0", 1);
	
	//60.6 支持部分扣款和返回余额标志
	if(gTermCfg.mTrSwitch.PartPurchaseFlag)
		memcpy(cTmpBuf + 13, "1", 1);
	else
		memcpy(cTmpBuf + 13, "0", 1);
		
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_ISSUER_SCORE_SALE_VOID:  //发卡行积分消费撤销
		strcat((char *)cTmpBuf, "048");
		break;
	case BTYPE_UNION_SCORE_SALE_VOID:   //联盟积分消费撤销
		strcat((char *)cTmpBuf, "065");
		break;
	default:
		break;
	}
	
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// field 61
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf((char *)cTmpBuf, "%06lu", TransData.TransDB.OrgBatchNum);
	sprintf((char *)cTmpBuf + 6, "%06lu", TransData.TransDB.OrgSysTrace);
	ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));
	
	do
	{
		// 重输输入卡密码
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
		Trans_SetField64_MAC(&ReqIsoRec);
		
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
		
		if(TransData.TransDB.TrType == BTYPE_YUYUE_SALE_VOID)
		{
			//预约撤销交易，刷卡方式应改为无卡(打印的时候需要用)
			TransData.TransDB.Swipe = TRAN_NOCARD;
		}
		
		// 保存交易流水
		if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
		{
			Comm_Disconnect();
			Trans_ResetReversalStatus(&TransData);
			Trans_UpdateReversalFieldData(39, "96", 2);
			
			return BANK_FAIL;
		}
		
		if(!gTermCfg.mTrSwitch.VoidPIN)
			break;
	}
	while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0) && (TransData.PinRetryFlag ++ < 2));
	
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
 * FUNCTION NAME: Trans_Add_Refund
 * DESCRIPTION:   复用的增值业务退货
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_Refund(void *dummy)
{
	DynData TransData;
	int iLength;
	usint usSwipeMode;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = (int)dummy;
	
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	// 输入主管密码
	if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
		return BANK_FAIL;
		
	//根据交易类型，采集、录入不同的内容
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_UNION_SCORE_REFUND:  //联盟积分退货
		usSwipeMode = TRAN_SWIP | TRAN_ICC;
		break;
	case BTYPE_YUYUE_REFUND:        //预约退货
		usSwipeMode = TRAN_NOCARD;
		break;
	case BTYPE_MCHIP_REFUND:        //手机芯片退货
		usSwipeMode = TRAN_MCHIP;
		break;
	case BTYPE_ORDER_REFUND:        //订购退货
		usSwipeMode = TRAN_NUM;
		break;
	default:
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
	ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));
	
	// 显示卡号有效期
	ASSERT_HANGUP_FAIL(Trans_ShowCardPAN(&TransData));
	
	// 预拨号
	Comm_CheckPreConnect();
	
	// 输入原参考号
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if(AppUtils_GetText(LINE2, "请输入原参考号:", (char *)cTmpBuf, 12, 12, 0, 60) < 0)
	{
		Comm_Disconnect();
		return BANK_FAIL;
	}
	
	memcpy(TransData.TransDB.OrgRefCode, cTmpBuf, 12);
	
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
	
	// 若IC卡脱机交易退货
	if((dummy == (void *)1) &&
	   (TransData.TransDB.Swipe == TRAN_ICC))
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		if(AppUtils_GetNum(LINE2, "请输入原终端号:", (char *)cTmpBuf, 8, 8, 60) <= 0)
		{
			Comm_Disconnect();
			return BANK_FAIL;
		}
		
		memcpy(TransData.AddBuf, cTmpBuf, 8);
		TransData.AddBufLen = 8;
	}
	
	// 输入金额
	while(TRUE)
	{
		ASSERT_FAIL(AppUtils_GetAmout(LINE2, "请输入退货金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
		
		if(memcmp(TransData.TransDB.Amount, gTermCfg.mFixPrm.RefundAmt, 12) > 0)
		{
			memset(TransData.TransDB.Amount, 0, sizeof(TransData.TransDB.Amount));
			PrnDisp_DisplayWarning("退货", "退货金额超限!");
			continue;
		}
		
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_AmountAsc2String(TransData.TransDB.Amount, "", '+', (char *)cTmpBuf);
		
		memset(cBuffer, 0, sizeof(cBuffer));
		Trans_GetTransName(TransData.TransDB.TrType, (char *)cBuffer);
		
		if(AppUtils_YesNoConfirm((char *)cBuffer, "请确认金额:", (char *)cTmpBuf, "按O键确认X键修改") == BANK_OK)
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
	
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
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
	memcpy(TransData.TransDB.ProcessCode, "200000", 6);
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
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_ORDER_REFUND:
		strcpy((char *)TransData.TransDB.ServiceCode, "08");
		break;
	default:
		strcpy((char *)TransData.TransDB.ServiceCode, "00");
		break;
	}
	
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
		
	// field 37
	ISOEngine_SetBit(&ReqIsoRec, 37, TransData.TransDB.OrgRefCode, 12);
	
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
	
	// field 55 Only for CUP Mobile
	if(TransData.TransDB.Swipe == TRAN_MCHIP)
	{
		ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
		TransData.AddBufLen = 0;
	}
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	//60.1消息类型
	memcpy(cTmpBuf, "25", 2);
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
	memcpy(cTmpBuf + 12, "0", 1);
	
	//60.6 支持部分扣款和返回余额标志
	if(gTermCfg.mTrSwitch.PartPurchaseFlag)
		memcpy(cTmpBuf + 13, "1", 1);
	else
		memcpy(cTmpBuf + 13, "0", 1);
		
	if(TransData.TransDB.TrType == BTYPE_UNION_SCORE_REFUND)      //联盟积分退货
		strcat((char *)cTmpBuf, "065");
		
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// field 61
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf((char *)cTmpBuf, "%012d", 0);
	memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);
	ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));
	
	// field 62
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if((dummy == (void *)1) &&
	   (TransData.TransDB.Swipe == TRAN_ICC))
	{
		ISOEngine_SetBit(&ReqIsoRec, 62, TransData.AddBuf, TransData.AddBufLen);
	}
	
	// field 63
	ISOEngine_SetBit(&ReqIsoRec, 63, (unsigned char *)"000", 3);
	
	// field 64
	Trans_SetField64_MAC(&ReqIsoRec);
	
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
 * FUNCTION NAME: Trans_Add_Balance
 * DESCRIPTION:   复用的增值业务查余额
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_Balance(void *dummy)
{
	DynData TransData;
	int i;
	int iRet;
	int iLength;
	Ushort usMode;
	byte cTmpBuf[ 512 ];
	byte *p, bTmp;
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = (int)dummy;
	
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	//根据交易类型，采集、录入不同的内容
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_UNION_SCORE_BALANCE: //联盟积分查询余额
		usMode = TRAN_SWIP | TRAN_ICC;
		break;
	case BTYPE_MCHIP_BALANCE:       //手机芯片查询余额
		usMode = TRAN_MCHIP;
		break;
	default:
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
	ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usMode, &TransData));
	
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
		if(TransData.TransDB.TrType == BTYPE_UNION_SCORE_BALANCE)
			memcpy(TransData.TransDB.ServiceCode, "65", 2);
		else
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
		else if(TransData.TransDB.Swipe == TRAN_MCHIP)
		{
			ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
			TransData.AddBufLen = 0;
		}
		
		// field 60
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		//60.1消息类型
		switch(TransData.TransDB.TrType)
		{
		case BTYPE_UNION_SCORE_BALANCE: //联盟积分查询余额
			memcpy(cTmpBuf, "03", 2);
			break;
		case BTYPE_MCHIP_BALANCE:       //手机芯片查询余额
		default:
			memcpy(cTmpBuf, "01", 2);
			break;
		}
		
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
		memcpy(cTmpBuf + 12, "0", 1);
		
		if(TransData.TransDB.TrType == BTYPE_UNION_SCORE_BALANCE)   //联盟积分查询
		{
			//60.6 支持部分扣款和返回余额标志
			if(gTermCfg.mTrSwitch.PartPurchaseFlag)
				memcpy(cTmpBuf + 13, "1", 1);
			else
				memcpy(cTmpBuf + 13, "0", 1);
				
			//60.6
			//memcpy(cTmpBuf + 13, "0", 1);
			
			//60.7
			memcpy(cTmpBuf + 14, "065", 3);
		}
		
		ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
		
		do
		{
			// 重新输入卡密码
			if(TransData.PinRetryFlag > 0)
			{
				ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
				
				// 检查网络并拨号
				if(Comm_CheckConnect() != BANK_OK)
					break;
					
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
				ISOEngine_SetBit(&ReqIsoRec, 26, (unsigned char *)"12", 2);
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
			Trans_SetField64_MAC(&ReqIsoRec);
			
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
			
			// 发送数据包
			ASSERT_FAIL(Comm_SendPacket(cBuffer, iLength));
			
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
			
			EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
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
			Pinpad_DisplayMsg("ACCBAL:", (char *)cBuffer);
			AppUtils_WaitKey(5);
			Pinpad_Idle();
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
 * FUNCTION NAME: Trans_Add_PreAuth
 * DESCRIPTION:   复用的增值业务预授权
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuth(void *dummy)
{
	DynData TransData;
	int iRet;
	int iLength;
	usint usCardMode;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = (int)dummy;
	
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	//根据交易类型，采集、录入不同的内容
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_MCHIP_PREAUTH:     //手机芯片预授权
		usCardMode = TRAN_MCHIP;
		break;
		
	case BTYPE_ORDER_PREAUTH:        //订购预授权
		usCardMode = TRAN_NUM;
		
		// 输入金额
		if(AppUtils_IsNullData((char *) TransData.TransDB.Amount, sizeof(TransData.TransDB.Amount)) == BANK_OK)
			ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
			
		break;
	default:
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
	// 刷卡/插卡/挥卡，获取卡号
	ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usCardMode, &TransData));
	
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
			ASSERT_HANGUP_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
			
		if(TransData.TransDB.TrType == BTYPE_ORDER_PREAUTH)
			ASSERT_HANGUP_FAIL(Trans_Add_GetOrderInfo(&TransData));
			
		// 输入卡密码
		TransData.PinRetryFlag = 0;
		
		if(TransData.TransDB.Swipe != TRAN_ICC)
		{
			if(TransData.TransDB.TrType != BTYPE_ORDER_PREAUTH)
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
		memcpy(TransData.MsgID, "0100", 4);
		ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);
		
		// field 2
		if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
			ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);
			
		// field 3
		memcpy(TransData.TransDB.ProcessCode, "030000", 6);
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
		switch(TransData.TransDB.TrType)
		{
		case BTYPE_ORDER_PREAUTH:
			strcpy((char *)TransData.TransDB.ServiceCode, "18");
			break;
		default:
			strcpy((char *)TransData.TransDB.ServiceCode, "06");
			break;
		}
		
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
		else if(TransData.TransDB.Swipe == TRAN_MCHIP)
		{
			ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
			
			TransData.AddBufLen = 0;  //避免后面62域打包上送
		}
		
		// field 60
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		//60.1消息类型
		memcpy(cTmpBuf, "10", 2);
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
		
		// Field 62
		if(TransData.AddBufLen > 0)
			ISOEngine_SetBit(&ReqIsoRec, 62, (byte *)TransData.AddBuf, TransData.AddBufLen);
			
		do
		{
			// 重新输入卡密码
			if(TransData.PinRetryFlag > 0)
			{
				if(TransData.TransDB.TrType != BTYPE_ORDER_PREAUTH)
				{
					ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
					
					// 检查网络并拨号
					if(Comm_CheckConnect() != BANK_OK)
						break;
						
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
			Trans_SetField64_MAC(&ReqIsoRec);
			
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
			if(Trans_UnPackPublicData(&TransData, cBuffer, &RspIsoRec) != BANK_OK)
				break;
				
			// 清除冲正标识
			if(Trans_ClearReversal(REVERSAL_CLEAR_STATUS) != BANK_OK)
				break;
				
			// 保存交易流水
			if(AppUtils_JournalWriteRecord(0, &TransData) != BANK_OK)
			{
				Trans_ResetReversalStatus(&TransData);
				Trans_UpdateReversalFieldData(39, "96", 2);
				break;
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
			iLength = ISOEngine_GetBit(&RspIsoRec, 55, cBuffer, sizeof(cBuffer));
			
//      if((iLength > 0) || (memcmp(TransData.TransDB.RspCode, "00", 2) == 0))
//      {
//        EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
//      }
			EMVtrans_OnlineProcessing(cBuffer, iLength, &TransData);
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
 * FUNCTION NAME: Trans_Add_PreAuthVoid
 * DESCRIPTION:   复用的增值业务预授权撤销
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuthVoid(void *dummy)
{
	DynData TransData;
	int iLength;
	usint usCardMode;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	
	TransData.TransDB.TrType = (int)dummy;
	
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	// 主管密码
	if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
		return BANK_FAIL;
		
	//根据交易类型，采集、录入不同的内容
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_MCHIP_PREAUTH_VOID:     //手机芯片预授权撤销
		usCardMode = TRAN_MCHIP;
		break;
	case BTYPE_ORDER_PREAUTH_VOID:     //订购预授权撤销
		usCardMode = TRAN_NUM;
		break;
	default:
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
	// 刷卡/插卡，获取卡号
	ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usCardMode, &TransData));
	
	// 显示卡号有效期
	ASSERT_FAIL(Trans_ShowCardPAN(&TransData));
	
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
		ASSERT_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
		
	// 输入卡密码
	TransData.PinRetryFlag = 0;
	
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
	
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
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
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_ORDER_PREAUTH_VOID:
		strcpy((char *)TransData.TransDB.ServiceCode, "18");
		break;
	default:
		strcpy((char *)TransData.TransDB.ServiceCode, "06");
		break;
	}
	
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
	
	// field 55 Only for CUP Mobile
	if(TransData.TransDB.Swipe == TRAN_MCHIP)
	{
		ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
		TransData.AddBufLen = 0;
	}
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	//60.1消息类型
	memcpy(cTmpBuf, "11", 2);
	memcpy(TransData.TransDB.TransTypeCode, cTmpBuf, 2);
	
	//60.2批次号码
	sprintf((char *) cTmpBuf + 2, "%06lu", TransData.TransDB.BatchNum);
	
	//60.3网络管理信息码
	memcpy(cTmpBuf + 8, "000", 3);
	
	//60.4终端读取能力
	if(gTermCfg.mTrSwitch.CTLS_Support == 1)
		memcpy(cTmpBuf + 11, "6", 1);
	else
		memcpy(cTmpBuf + 11, "5", 1);;
		
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
			if(gTermCfg.mTrSwitch.VoidPreAuthPIN == 1)
			{
				ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
				
				// 检查网络并拨号
				if(Comm_CheckConnect() != BANK_OK)
					break;
					
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
		Trans_SetField64_MAC(&ReqIsoRec);
		
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
			Trans_UpdateReversalFieldData(39, "96", 2);
			break;
		}
		
	}
	while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0) && (TransData.PinRetryFlag ++ < 2));
	
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
 * FUNCTION NAME: Trans_Add_PreAuthCompleteRequest
 * DESCRIPTION:   复用的增值业务预授权完成请求(联机)
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuthCompleteRequest(void *dummy)
{
	DynData TransData;
	int iLength;
	usint usSwipeMode;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = (int)dummy;
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	//根据交易类型，采集、录入不同的内容
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_MCHIP_COMPLETE:     //手机芯片预授权完成
		usSwipeMode = TRAN_MCHIP;
		break;
	case BTYPE_ORDER_COMPLETE:      //订购预授权完成
		usSwipeMode = TRAN_NUM;
		break;
	default:
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
	// 刷卡/插卡/挥卡，获取卡号
	ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));
	
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
		ASSERT_FAIL(AppUtils_GetAmout(LINE2, "请输入金额:", '+' , CURRENCY_NAME, FALSE, TransData.TransDB.Amount));
		
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	//根据交易类型，采集、录入不同的内容
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_MCHIP_COMPLETE:     //手机芯片预授权完成请求
		break;
	case BTYPE_ORDER_COMPLETE:     //订购预授权完成请求
		ASSERT_HANGUP_FAIL(Trans_Add_GetOrderInfo(&TransData));
		break;
	default:
		Comm_Disconnect();
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
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
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_ORDER_COMPLETE:
		strcpy((char *)TransData.TransDB.ServiceCode, "18");
		break;
	default:
		strcpy((char *)TransData.TransDB.ServiceCode, "06");
		break;
	}
	
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
	
	// field 55 Only for CUP Mobile
	if(TransData.TransDB.Swipe == TRAN_MCHIP)
	{
		ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
		TransData.AddBufLen = 0;
	}
	
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
	
	// Field 62
	if(TransData.AddBufLen > 0)
		ISOEngine_SetBit(&ReqIsoRec, 62, (byte *)TransData.AddBuf, TransData.AddBufLen);
		
	do
	{
		// 重新输入卡密码
		if(TransData.PinRetryFlag > 0)
		{
			if(gTermCfg.mTrSwitch.PreAuthEndReqPIN == 1)
			{
				ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
				
				// 检查网络并拨号
				if(Comm_CheckConnect() != BANK_OK)
					break;
					
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
		Trans_SetField64_MAC(&ReqIsoRec);
		
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
			Trans_UpdateReversalFieldData(39, "96", 2);
			break;
		}
	}
	while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0) && (TransData.PinRetryFlag ++ < 2));
	
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
 * FUNCTION NAME: Trans_Add_PreAuthCompleteNote
 * DESCRIPTION:   复用的增值业务预授权完成通知(离线)
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuthCompleteNote(void *dummy)
{
	DynData TransData;
	int iLength;
	usint usSwipeMode;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = (int)dummy;
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	//根据交易类型，采集、录入不同的内容
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_MCHIP_OFFLINE_COMPLETE:     //手机芯片预授权完成
		usSwipeMode = TRAN_MCHIP;
		break;
	case BTYPE_ORDER_OFFLINE_COMPLETE:      //订购预授权完成
		usSwipeMode = TRAN_NUM;
		break;
	default:
		AppUtils_Warning((char *)"不支持的交易类型");
		return BANK_FAIL;
	}
	
	ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));
	
	// 显示卡号有效期
	ASSERT_FAIL(Trans_ShowCardPAN(&TransData));
	
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
	
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	TransData.TransDB.BatchNum = gTermCfg.mDynPrm.BatchNum;
	
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
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_ORDER_OFFLINE_COMPLETE:
		strcpy((char *)TransData.TransDB.ServiceCode, "18");
		break;
	default:
		strcpy((char *)TransData.TransDB.ServiceCode, "06");
		break;
	}
	
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
	
	// field 55 Only for CUP Mobile
	if(TransData.TransDB.Swipe == TRAN_MCHIP)
	{
		ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
		TransData.AddBufLen = 0;
	}
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	//60.1消息类型
	memcpy(cTmpBuf, "24", 2);
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
	
	// field 64
	Trans_SetField64_MAC(&ReqIsoRec);
	
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
 * FUNCTION NAME: Trans_Add_PreAuthCompleteVoid
 * DESCRIPTION:   复用的增值业务预授权完成撤销
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuthCompleteVoid(void *dummy)
{
	DynData TransData;
	int iLength;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	int iCurrTranType;
	usint usOrgSwipeMode, usSwipeMode = 0;
	
	memset(&TransData, 0, sizeof(TransData));
	iCurrTranType = (int)dummy;
	TransData.TransDB.TrType = iCurrTranType;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(iCurrTranType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	// 主管密码
	if(gTermCfg.mTrSwitch.MngPwdRequired && (PrmMngr_InputPwd((void *)0) != BANK_OK))
		return BANK_FAIL;
		
	// 获取原交易数据
	ASSERT_FAIL(Trans_GetOrignalJournal("请输入原凭证号:", &TransData));
	
	// 检查原交易状态
	switch(iCurrTranType)
	{
		//手机芯片完成撤销
	case BTYPE_MCHIP_COMPLETE_VOID:
	
		if(TransData.TransDB.TrType != BTYPE_MCHIP_COMPLETE)
		{
			PrnDisp_DisplayWarning("", "原交易非手机芯片预授权完成");
			return BANK_FAIL;
		}
		
		break;
		//订购预授权完成撤销
	case BTYPE_ORDER_COMPLETE_VOID:
	
		if(TransData.TransDB.TrType != BTYPE_ORDER_COMPLETE)
		{
			PrnDisp_DisplayWarning("", "原交易非订购完成");
			return BANK_FAIL;
		}
		
		break;
	default:
		AppUtils_FormatWarning((char *)"无效的交易类型%d", iCurrTranType);
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
	
	// 显示原交易相关信息
	if(PrnDisp_DisplayTxnDetail(&TransData, iCurrTranType) == bESC)
		return BANK_FAIL;
		
	AppUtils_Cls();
	PrnDisp_DisplayTransName(iCurrTranType);
	
	// 复制原交易相关信息
	TransData.TransDB.OrgTranType = TransData.TransDB.TrType;
	TransData.TransDB.TrType = iCurrTranType;
	TransData.TransDB.OrgSysTrace = TransData.TransDB.SysTrace;
	TransData.TransDB.OrgBatchNum = TransData.TransDB.BatchNum;
	memcpy(TransData.TransDB.OrgRefCode, TransData.TransDB.RefCode, 12);
	AppUtils_Bcd2Asc(TransData.TransDB.TranDate + 2, TransData.TransDB.OrgTranDate, 4);
	usOrgSwipeMode = TransData.TransDB.Swipe;
	TransData.PinExists = 0;
	TransData.TransDB.EncryptedTrkFlag = 0;
	memcpy(TransData.TransDB.POSEntry, "\x01\x20", 2);
	memset(TransData.TransDB.RspCode, 0, sizeof(TransData.TransDB.RspCode));
	memset(TransData.TransDB.Track2Data, 0, sizeof(TransData.TransDB.Track2Data));
	memset(TransData.TransDB.Track3Data, 0, sizeof(TransData.TransDB.Track3Data));
	
	//清除原交易中的TC上送标志：撤销交易不需要上送TC
	AppUtils_ResetStatus(&TransData.TransDB.Status[0], STATUS_TCUPLOAD);
	
	if(usOrgSwipeMode != TRAN_NOCARD && usOrgSwipeMode != TRAN_MCHIP)
	{
		TransData.TransDB.Swipe = TRAN_NUM;
		memcpy(TransData.TransDB.POSEntry, "\x01\x20", 2);
	}
	
	if((usOrgSwipeMode & TRAN_ICC) || (usOrgSwipeMode & TRAN_SWIP))
	{
		usSwipeMode = TRAN_SWIP | TRAN_ICC;
	}
	else if(usOrgSwipeMode & TRAN_MCHIP)
	{
		usSwipeMode = TRAN_MCHIP;
	}
	
	if(usSwipeMode && gTermCfg.mTrSwitch.VoidPreauthEndSwipCard)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		memcpy(cTmpBuf, TransData.TransDB.PAN, TransData.TransDB.PANLen);
		
		// 刷卡/插卡/挥卡，获取卡号
		ASSERT_SWITCH2PP_FAIL(Trans_GetTrackPANData(usSwipeMode, &TransData));
		
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
	ASSERT_USERCANCEL(Trans_DoReversal(iCurrTranType));
	
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
	switch(TransData.TransDB.TrType)
	{
	case BTYPE_ORDER_COMPLETE_VOID:
		strcpy((char *)TransData.TransDB.ServiceCode, "18");
		break;
	default:
		strcpy((char *)TransData.TransDB.ServiceCode, "06");
		break;
	}
	
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
	
	// field 55 Only for CUP Mobile
	if(TransData.TransDB.Swipe == TRAN_MCHIP)
	{
		ISOEngine_SetBit(&ReqIsoRec, 55, TransData.AddBuf, TransData.AddBufLen);
		TransData.AddBufLen = 0;
	}
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	//60.1消息类型
	memcpy(cTmpBuf, "21", 2);
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
	sprintf((char *)cTmpBuf, "%06ld", TransData.TransDB.OrgBatchNum);
	sprintf((char *)cTmpBuf + 6, "%06ld", TransData.TransDB.OrgSysTrace);
	memcpy(cTmpBuf + 12, TransData.TransDB.OrgTranDate, 4);
	ISOEngine_SetBit(&ReqIsoRec, 61, cTmpBuf, strlen((char *) cTmpBuf));
	
	do
	{
		// 重新输入卡密码
		if(TransData.PinRetryFlag > 0)
		{
			if(gTermCfg.mTrSwitch.VoidPreAuthEndPIN == 1)
			{
				ASSERT_HANGUP_FAIL(Trans_GetCardPin(&TransData));
				
				// 检查网络并拨号
				if(Comm_CheckConnect() != BANK_OK)
					break;
					
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
		Trans_SetField64_MAC(&ReqIsoRec);
		
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
			Trans_UpdateReversalFieldData(39, "96", 2);
			return BANK_FAIL;
		}
		
		if(!gTermCfg.mTrSwitch.VoidPIN)
			break;
	}
	while((memcmp(TransData.TransDB.RspCode, "55", 2) == 0) && (TransData.PinRetryFlag ++ < 2));
	
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
 * FUNCTION NAME: Trans_Add_CardholderVerify
 * DESCRIPTION:   持卡人信息验证
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_CardholderVerify(void *dummy)
{
	DynData TransData;
	int iLength;
	byte cTmpBuf[ 512 ];
	byte cBuffer[ISO8583_MAXLENTH];
	ISO8583_Rec ReqIsoRec, RspIsoRec;
	char szPANno[20] = { 0 };
	
	memset(&TransData, 0, sizeof(TransData));
	TransData.TransDB.TrType = BTYPE_ORDER_CARDHOLDER_VERIFY;
	
	// 显示交易类型
	AppUtils_Cls();
	PrnDisp_DisplayTransName(TransData.TransDB.TrType);
	
	// 检测终端各自状态
	ASSERT_FAIL(Trans_CheckTerminalStatus(TransData.TransDB.TrType));
	
	// 输入卡号
	if(AppUtils_InputNum(LINE2, "请输入卡号:", szPANno, 8, 19, RIGHT_JST) < 0)
		return BANK_FAIL;
		
	// 按手输卡号处理
	TransData.TransDB.PANLen = strlen(szPANno);
	memcpy(TransData.TransDB.PAN, szPANno, TransData.TransDB.PANLen);
	TransData.TransDB.Swipe = TRAN_NUM;
	memcpy(TransData.TransDB.POSEntry, "\x01\x20", 2);
	
	ASSERT_FAIL(Trans_Add_GetOrderInfo(&TransData));
	
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
	memcpy(TransData.MsgID, "0100", 4);
	ISOEngine_SetBit(&ReqIsoRec, 0, TransData.MsgID, 4);
	
	// field 2
	if((TransData.TransDB.Swipe != TRAN_SWIP) && (TransData.TransDB.PANLen > 0))
		ISOEngine_SetBit(&ReqIsoRec, 2, TransData.TransDB.PAN, TransData.TransDB.PANLen);
		
	// field 3
	memcpy(TransData.TransDB.ProcessCode, "330000", 6);
	ISOEngine_SetBit(&ReqIsoRec, 3, TransData.TransDB.ProcessCode, 6);
	
	// field 11
	TransData.TransDB.SysTrace = gTermCfg.mDynPrm.SysTrace;
	sprintf((char *) cTmpBuf, "%06lu", TransData.TransDB.SysTrace);
	ISOEngine_SetBit(&ReqIsoRec, 11, cTmpBuf, 6);
	
	// field 22
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(TransData.TransDB.POSEntry, cTmpBuf, 4);
	ISOEngine_SetBit(&ReqIsoRec, 22, cTmpBuf, 3);
	
	// field 25
	memcpy(TransData.TransDB.ServiceCode, "00", 2);
	ISOEngine_SetBit(&ReqIsoRec, 25, TransData.TransDB.ServiceCode, 2);
	
	// field 41
	ISOEngine_SetBit(&ReqIsoRec, 41, gTermCfg.mFixPrm.TerminalID, 8);
	
	// field 42
	ISOEngine_SetBit(&ReqIsoRec, 42, gTermCfg.mFixPrm.MerchantID, 15);
	
	// field 48 后续交易信息: 00-默认，04-订购，11-预付费卡充值
	ISOEngine_SetBit(&ReqIsoRec, 48, (unsigned char *)"04", 2);
	
	// field 49
	ISOEngine_SetBit(&ReqIsoRec, 49, (unsigned char *)CURRENCY_CODE, 3);
	
	// field 60
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	//60.1消息类型
	memcpy(cTmpBuf, "01", 2);
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
	memcpy(cTmpBuf + 12, "0", 1);
	
	//60.6 支持部分扣款和返回余额标志
	if(gTermCfg.mTrSwitch.PartPurchaseFlag)
		memcpy(cTmpBuf + 13, "1", 1);
	else
		memcpy(cTmpBuf + 13, "0", 1);
		
	ISOEngine_SetBit(&ReqIsoRec, 60, cTmpBuf, strlen((char *) cTmpBuf));
	
	// field 62
	if(TransData.AddBufLen > 0)
		ISOEngine_SetBit(&ReqIsoRec, 62, TransData.AddBuf, TransData.AddBufLen);
		
	// field 64
	Trans_SetField64_MAC(&ReqIsoRec);
	
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
	ASSERT_HANGUP_FAIL(PrnDisp_DisplayRespCode(&TransData));
	
	if(memcmp(TransData.TransDB.RspCode, "00", 2) == 0)
	{
		AppUtils_UserClear(LINE2);
		AppUtils_DisplayCenter((char *)"交易成功", LINE2, FALSE);
		AppUtils_NormalBeep();
		AppUtils_WaitKey(5);
	}
	
	// 检查是否有脱机交易需要上送
	Trans_SubmitOffline(0);
	
	// 挂线
	Comm_Disconnect();
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_GetScoreInfo
 * DESCRIPTION:   获取积分消费相关信息
 * PARAMETERS:    pTxnData: 交易数据
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_GetScoreInfo(DynData *pTxnData)
{
	char cTmpBuf[128];
	
	//输入商品代码
	AppUtils_UserClear(2);
	AppUtils_DisplayRight("输错请按[退格]键", LINE4);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if(AppUtils_GetText(LINE2, "请输入商品代码:", (char *)cTmpBuf, 1, 30, 0, 60) < 0)
	{
		Comm_Disconnect();
		return BANK_FAIL;
	}
	
	strcpy((char *)pTxnData->TransDB.ScoreProductCode, (char *)cTmpBuf);
	
	/*左对齐 右补空格(根据规范共60字节)*/
	pTxnData->AddBufLen = sprintf((char *)pTxnData->AddBuf, "%-60s", cTmpBuf);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_GetInstallmentInfo
 * DESCRIPTION:   录入分期付款交易信息
 * PARAMETERS:    pTxnData: 交易数据
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_GetInstallmentInfo(DynData *pTxnData)
{
	char cTmpBuf[128];
	byte bt;
	
	while(1)   /* 输入期数 */
	{
		AppUtils_UserClear(2);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		if(AppUtils_InputNum(LINE2, "请输入交易期数:", (char *)cTmpBuf, 1, 2, RIGHT_JST) < 0)
		{
			Comm_Disconnect();
			return BANK_FAIL;
		}
		
		if(atoi((char *)cTmpBuf) >= 2 || atoi((char *)cTmpBuf) <= 36)
		{
			pTxnData->TransDB.InstallmentPeriod = (byte)atoi((char *)cTmpBuf);
			break;
		}
		else
		{
			AppUtils_Warning("无效的交易期数");
		}
	}
	
	/* 输入项目编码 */
	AppUtils_UserClear(2);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if(AppUtils_GetText(LINE2, "输入项目编码:", (char *)cTmpBuf, 1, 30, 0, 60) < 0)
	{
		Comm_Disconnect();
		return BANK_FAIL;
	}
	
	strcpy((char *)pTxnData->TransDB.InstallmentProjectNo, (char *)cTmpBuf);
	
	while(1)
	{
		AppUtils_UserClear(2);
		AppUtils_DisplayLine("手续费支付方式", LINE2);
		AppUtils_DisplayLine("1:一次性支付", LINE3);
		AppUtils_DisplayLine("2:分期支付", LINE4);
		
		bt = AppUtils_WaitKey(30);
		
		if(bt == bKEY1 || bt == bKEY2)
		{
			pTxnData->TransDB.InstallmentFeePayway = bt - '1';
			break;
		}
		else if(bt == bESC || bt == 0)
		{
			Comm_Disconnect();
			return BANK_FAIL;
		}
	}
	
	pTxnData->AddBufLen = sprintf((char *)pTxnData->AddBuf, "%02u%-30s1%u%-28s",
	                              pTxnData->TransDB.InstallmentPeriod,
	                              pTxnData->TransDB.InstallmentProjectNo,
	                              pTxnData->TransDB.InstallmentFeePayway,
	                              " ");
	                              
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_GetYuyueInfo
 * DESCRIPTION:   录入预约交易信息
 * PARAMETERS:    pTxnData: 交易数据
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_GetYuyueInfo(DynData *pTxnData)
{
	char cTmpBuf[128];
	
	/*手机号码最大11位，控制长度*/
	AppUtils_UserClear(2);
	
	while(1)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		if(AppUtils_InputNum(LINE2, "请输入手机号码:", (char *)cTmpBuf, 0, 11, RIGHT_JST) < 0)
		{
			//Comm_Disconnect();
			return BANK_FAIL;
		}
		
		//BCTC 李远要求，此处手机号码增加位数控制 2013-6-6 8:33:54
		if(strlen((char *)cTmpBuf) == 11)
		{
			strcpy((char *)pTxnData->TransDB.YuYuePhoneNum, (char *)cTmpBuf);
			break;
		}
		else
			AppUtils_Warning((char *)"位数不足,请重输:");
	}
	
	
	/* 预约号码必须是6位 */
	AppUtils_UserClear(2);
	
	while(1)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		if(AppUtils_InputNum(LINE2, "请输入预约号码:", (char *)cTmpBuf, 0, 6, RIGHT_JST) < 0)
		{
			//Comm_Disconnect();
			return BANK_FAIL;
		}
		
		if(strlen((char *)cTmpBuf) == 6)
		{
			strcpy((char *)pTxnData->TransDB.YuYueCode, (char *)cTmpBuf);
			break;
		}
		else
			AppUtils_Warning((char *)"位数不足,请重输:");
	}
	
	/* 90+电话号码+预约码 左对齐 右补空格*/
	pTxnData->AddBufLen = sprintf((char *)pTxnData->AddBuf, "90%-11s%-9s", (char *)pTxnData->TransDB.YuYuePhoneNum, (char *)pTxnData->TransDB.YuYueCode);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_GetOrderInfo
 * DESCRIPTION:   获取订购交易信息
 * PARAMETERS:    pTxnData: 交易数据
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_GetOrderInfo(DynData *pTxnData)
{
	char szCVN2[32];
	char szIDPart[32];
	char szCardHolderName[32 + 1];
	char szCellPhoneNo[16];
	int iCVN2Len, iIDPartLen, iHolderLen, iCellPhoneLen;
	
	iCVN2Len = iIDPartLen = iHolderLen = iCellPhoneLen = 0;
	
	if(pTxnData->TransDB.TrType != BTYPE_ORDER_CARDHOLDER_VERIFY)
	{
		AppUtils_UserClear(2);
		
		while(1)
		{
			memset(szCVN2, 0, sizeof(szCVN2));
			
			if(AppUtils_InputNum(LINE2, "请输入卡片CVN2", szCVN2, 0, 3, RIGHT_JST) < 0)
				return BANK_FAIL;
				
			iCVN2Len = strlen(szCVN2);
			
			if(iCVN2Len == 3)
				break;
			else
				AppUtils_Warning((char *)"位数不足,请重输:");
		}
	}
	
	if(AppUtils_IsColorScreen() == BANK_OK)
	{
		T9PY_Init(0, 0, '#', 0, 0, 0, 0, 1);
	}
	else
	{
		T9PY_Init(0, 0, 0, 0, 0, 0, 0, 1);
	}
	
	AppUtils_UserClear(2);
	memset(szCardHolderName, 0, sizeof(szCardHolderName));
	AppUtils_DisplayLine("请输入持卡人姓名", LINE1);
	
	if(T9PY_Input(szCardHolderName, 30, 60) < 0)
		return BANK_FAIL;
		
	iHolderLen = strlen(szCardHolderName);
	
	//重新显示交易标题栏
	PrnDisp_DisplayTransName(pTxnData->TransDB.TrType);
	
	while(1)
	{
		AppUtils_UserClear(2);
		memset(szCellPhoneNo, 0, sizeof(szCellPhoneNo));
		
		if(AppUtils_InputNum(LINE2, "请输入手机号", szCellPhoneNo, 0, 11, RIGHT_JST) < 0)
			return BANK_FAIL;
			
		iCellPhoneLen = strlen(szCellPhoneNo);
		
		if(iCellPhoneLen == 0 || iCellPhoneLen == 11)
		{
			break;
		}
		else
			AppUtils_Warning((char *)"位数不足,请重输:");
	}
	
	while(1)
	{
		AppUtils_UserClear(2);
		memset(szIDPart, 0, sizeof(szIDPart));
		
		if(AppUtils_GetText(LINE2, "输入身份证后6位", szIDPart, 0, 6, 0, 60) < 0)
			return BANK_FAIL;
			
		iIDPartLen = strlen(szIDPart);
		
		if(iIDPartLen == 0 || iIDPartLen == 6)
		{
			break;
		}
		else
			AppUtils_Warning((char *)"位数不足,请重输:");
	}
	
	memset(pTxnData->AddBuf, 0, sizeof(pTxnData->AddBuf));
	
	//跳过不输，62域不填
	if(pTxnData->TransDB.TrType == BTYPE_ORDER_CARDHOLDER_VERIFY && (iIDPartLen + iHolderLen + iCellPhoneLen) == 0)
	{
		pTxnData->AddBufLen = 0;
		return BANK_OK;
	}
	
	if(pTxnData->TransDB.TrType != BTYPE_ORDER_CARDHOLDER_VERIFY)
		pTxnData->AddBufLen = sprintf((char *)pTxnData->AddBuf, "92CV%03d%sSF%03d%sTX%03d%sNM%03d%s", iCVN2Len, szCVN2, iIDPartLen, szIDPart, iCellPhoneLen, szCellPhoneNo, iHolderLen, szCardHolderName);
	else
		pTxnData->AddBufLen = sprintf((char *)pTxnData->AddBuf, "92SF%03d%sTX%03d%sNM%03d%s", iIDPartLen, szIDPart, iCellPhoneLen, szCellPhoneNo, iHolderLen, szCardHolderName);
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_GetIDInfo
 * DESCRIPTION:   输入身份证件类型及号码
 * PARAMETERS:    pTxnData: 交易数据
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_GetIDInfo(DynData *pTxnData)
{
	char cTempBuf[30] = {0};
	int  iIDType = 0;
	char DispBuf[32] = {0};
	byte bt;
	byte bDispFlag;
	
	bDispFlag = 0;
	
	while(1)
	{
		AppUtils_Cls();
		
		if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)   //大屏幕直接显示5行
		{
			AppUtils_DisplayCenter("选择身份证件类型:", LINE1, TRUE);
			AppUtils_DisplayLine("1身份证  2军官证", LINE2);
			AppUtils_DisplayLine("3护照    4回乡证", LINE3);
			AppUtils_DisplayLine("5台胞证  6警官证", LINE4);
			AppUtils_DisplayLine("7士兵证  0其他", LINE5);
		}
		else if(bDispFlag == 0)
		{
			AppUtils_DisplayCenter("选择身份证件类型:", LINE1, TRUE);
			AppUtils_DisplayLine("1身份证  2军官证", LINE2);
			AppUtils_DisplayLine("3护照    4回乡证", LINE3);
			AppUtils_DisplayLine("5台胞证  6警官证", LINE4);
		}
		else
		{
			AppUtils_DisplayCenter("选择身份证件类型:", LINE1, FALSE);
			AppUtils_DisplayLine("7士兵证  0其他", LINE2);
		}
		
		bt = AppUtils_WaitKey(60);
		
		if((bt >= bKEY0) && (bt <= bKEY7))
		{
			iIDType = bt - bKEY0;
			break;
		}
		else if(bt == bESC || bt == 0)
		{
			return BANK_FAIL;
		}
		else if(bt == bENTER)   //翻页
		{
			bDispFlag = !bDispFlag;
		}
	}
	
	AppUtils_Cls();
	PrnDisp_DisplayTransName(pTxnData->TransDB.TrType);
	
	switch(iIDType)
	{
	case 1:
		strcpy(DispBuf , "请输入身份证号码");
		break ;
		
	case 2:
		strcpy(DispBuf , "请输入军官证号码");
		break ;
		
	case 3:
		strcpy(DispBuf , "请输入护照号码");
		break ;
		
	case 4:
		strcpy(DispBuf , "请输入回乡证号码");
		break ;
		
	case 5:
		strcpy(DispBuf , "请输入台胞证号码");
		break ;
		
	case 6:
		strcpy(DispBuf , "请输入警官证号码");
		break ;
		
	case 7:
		strcpy(DispBuf , "请输入士兵证号码");
		break ;
		
	default:
		strcpy(DispBuf , "请输入证件号码");
		iIDType = 99 ;
		break ;
	}
	
	memset(cTempBuf, 0, sizeof(cTempBuf));
	
	if(AppUtils_GetText(LINE2, DispBuf, cTempBuf, 1, 20, 0, 60) < 0)
		return BANK_FAIL;
		
	pTxnData->AddBufLen = 22;
	sprintf((char *)pTxnData->AddBuf, "%02d%-20s", iIDType, cTempBuf);
	
	return BANK_OK;
}
