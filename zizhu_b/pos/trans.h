/***************************************************************************
* FILE NAME:    trans.h                                                    *
* MODULE NAME:  TRANS                                                      *
* PROGRAMMER:   Laikey                                                     *
* DESCRIPTION:                                                             *
* REVISION:                                                                *
****************************************************************************/

#ifndef __TRANS_H__
#define __TRANS_H__
#include "iso8583.h"

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_UnPackData
 * DESCRIPTION:   解包函数
 * PARAMETERS:    pTxnData:交易动态数据  pRecvData:返回报文  pRspIsoRec:返回包8583结构体
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_UnPackData(DynData *pTxnData, byte *pRecvData, ISO_data *pRspIsoRec);
/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_MainMenu
 * DESCRIPTION:   主交易菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_MainMenu( void );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_AuthMenu
 * DESCRIPTION:   预授权菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_AuthMenu(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_OfflineMenu
 * DESCRIPTION:   离线交易菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_OfflineMenu(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_PrintMenu
 * DESCRIPTION:   打印菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_PrintMenu(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ManageMenu
 * DESCRIPTION:   管理菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ManageMenu(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_LogonMenu
 * DESCRIPTION:   签到菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_LogonMenu(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_InquireMenu
 * DESCRIPTION:   交易查询菜单
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_InquireMenu(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_DownloadFuncMenu
 * DESCRIPTION:   下载功能菜单(系统管理->其他功能设置->下载功能)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_DownloadFuncMenu( void *dummy );

int Trans_OtherMenu(void *dummy);

int Trans_ECashMenu(void *dummy);

int Trans_ECLoadMenu(void *dummy);

int Trans_PbocHandingMenu( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_QuickMenu
 * DESCRIPTION:   待机界面按2进入的快速方式
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_QuickMenu( void *dummy );

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
int Trans_GetTransName( int iTransType, char *pRetTransName );

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
int Trans_GetShortTransName( int iTransType, char *pcRetTransName );

int Trans_Logon(void *dummy);

int Trans_Logout(void *dummy);

int Trans_Sale(void *dummy);

int Trans_Void(void *dummy);

int Trans_Refund(void *dummy);

int Trans_PreAuth(void *dummy);

int Trans_PreAuthCompleteRequest(void *dummy);

int Trans_PreAuthCompleteNote(void *dummy);

int Trans_PreAuthVoid( void *dummy );

int Trans_PreAuthCompleteVoid( void *dummy );

int Trans_OfflineSettlement( void *dummy );

int Trans_AdjustSettlement( void *dummy );

int Trans_Settlement( void *dummy );

int Trans_LockTerminal( void *dummy );

int Trans_LogonOper( void *dummy );

int Trans_LogonScore( void *dummy );

int Trans_InquireJournal( void *dummy );

int Trans_InquireTotal( void *dummy );

int Trans_InquireByTrace( void *dummy );

int Trans_BalanceInquire( void *dummy );

int Trans_ECashQPay( void *dummy );

int Trans_ECashPay( void *dummy );

int Trans_ECashBalance( void *dummy );

int Trans_ECashLoad( void *dummy );

int Trans_ECashVoidLoad( void *dummy );

int Trans_CheckECashLoadTxn( byte bTransType );

int Trans_CalculateTotals( TotalRec *pTotalsData );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_ParametersDownload
 * DESCRIPTION:     Download parameters from host
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_ParametersDownload( void *dummy );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:	Trans_AIDDownload
 * DESCRIPTION: 	download AID information parameter 
 * RETURN: 			BANK_OK: sucess
 *					BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_AIDDownload( void *dummy );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:	Trans_AIDSave2Kernel
 * DESCRIPTION: 	save AID data to EMV Kernel, include CTLS device 
 * RETURN: 			BANK_OK: sucess
 *					BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_AIDSave2Kernel( void *dummy );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:	Trans_CAKeyDownload
 * DESCRIPTION: 	download CA KEY 
 * RETURN: 			BANK_OK: sucess
 *					BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_CAKeyDownload( void *dummy );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:	Trans_CAKeySave2Kernal
 * DESCRIPTION: 	save CA KEY to EMV Kernel, include CTLS device 
 * RETURN: 			BANK_OK: sucess
 *					BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_CAKeySave2Kernal( void *dummy );

int Trans_BlackListDownload( void *dummy );

void Trans_AIDDownload_Test( void );

void Trans_CAKeyDownload_Test( void );

int Trans_GetTrackPANData( unsigned short usEventMode, DynData *pTxnRec );

int Trans_CheckMagneticCard( unsigned short usSwipeMode, DynData *pTxnData );

int Trans_GetPANExpDatebyTrack2( DynData *pTxnData );

int Trans_CheckServiceCode( DynData *pTxnData );

int Trans_ShowCardPAN( DynData *pTxnData );

int Trans_GetCardPin( DynData *pTxnData );

int Trans_ResetReversalStatus( DynData *pTransData );

int Trans_UpdateReversalFieldData( int iFieldNo, char *pFieldData, int iDataLength );

int Trans_EchoTesting( void *dummy );

int Trans_UploadPOSStatus( void *dummy );

int Trans_DoHostRequireEventFuncion( void );

int Trans_GetOrignalJournal(char *pTitle, DynData *pTxnData);
int Trans_UnPackPublicData(DynData *pTxnData, byte *pRecvData, ISO8583_Rec *pRspIsoRec);

int Trans_CheckTerminalStatus(int iTrType);
int Trans_ClearReversal(byte bClearReversalFlag);
int Trans_DoReversal(byte bOrignalTranType);
int Trans_SetReversal( DynData *pstTxnData, byte *pReversalData, int DataLength);
//int Trans_SetField53_Data( ISO8583_Rec *pReqIsoRec, DynData *pstTransData );
//int Trans_SetField64_MAC(ISO8583_Rec *pReqIsoRec);

int Trans_UploadICCscript( DynData *pTransData );
int Trans_SubmitOffline(byte bUploadFlag);
int Trans_CheckNeedDisconnect(void);
int Trans_CheckAutoUploadOffline(void);
int Trans_OperatorLogout(void *dummy);

/*(交易日志)明细查询*/
int Trans_PBOCQueryTransDetail(void *dummy);

/*圈存交易日志*/
int Trans_PBOCReadReloadTransLog(void *dummy);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_ClearCAKey
 * DESCRIPTION:     Clear CA KEY in EMV Kernel, include CTLS device
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_ClearCAKey( void *dummy );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_ClearAIDPrm
 * DESCRIPTION:     Clear AID prm from EMV Kernel, include CTLS device
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_ClearAIDPrm( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashInLoad
 * DESCRIPTION:   电子现金的指定账户圈存
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashInLoad( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_ECashOutLoad
 * DESCRIPTION:   电子现金的非指定账户圈存
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_ECashOutLoad( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_EPurseMenu
 * DESCRIPTION:   电子现金交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_EPurseMenu(void *dummy);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_UploadScriptResult
 * DESCRIPTION:     上送脚本结果通知
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_UploadScriptResult(void *dummy);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_UploadOfflineTrans
 * DESCRIPTION:     上送脱机交易
 * RETURN:          BANK_OK: sucess
 *                  BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_UploadOfflineTrans(void *dummy);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   Trans_ClearScriptResult
 * DESCRIPTION:     清除脚本结果通知
 * RETURN:          BANK_OK: success, BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int Trans_ClearScriptResult( void *dummy );

int Trans_ReadTotalData(TotalRec *pstTotalsData);

#endif
