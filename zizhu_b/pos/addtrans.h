/***************************************************************************
* FILE NAME:    addtrans.h                                                 *
* MODULE NAME:  ADDTRANS                                                   *
* PROGRAMMER:   Laikey                                                     *
* DESCRIPTION:  增值业务模块，包含直连规范中的增值业务交易                 *
* REVISION:                                                                *
****************************************************************************/

#ifndef __ADDTRANS_H__
#define __ADDTRANS_H__

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_Sale
 * DESCRIPTION:   复用的增值业务消费
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_Sale(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_Void
 * DESCRIPTION:   复用的增值业务撤销
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_Void(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_Refund
 * DESCRIPTION:   复用的增值业务退货
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_Refund(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_Balance
 * DESCRIPTION:   复用的增值业务查余额
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_Balance(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_PreAuth
 * DESCRIPTION:   复用的增值业务预授权
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuth(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_PreAuthVoid
 * DESCRIPTION:   复用的增值业务预授权撤销
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuthVoid(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_PreAuthCompleteRequest
 * DESCRIPTION:   复用的增值业务预授权完成请求(联机)
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuthCompleteRequest(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_PreAuthCompleteNote
 * DESCRIPTION:   复用的增值业务预授权完成通知(离线)
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuthCompleteNote(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_PreAuthCompleteVoid
 * DESCRIPTION:   复用的增值业务预授权完成撤销
 * PARAMETERS:    dummy: 交易类型
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_PreAuthCompleteVoid(void *dummy);

int Trans_Add_ScoreMenu(void *dummy);

int Trans_Add_ScoreSaleMenu(void *dummy);

int Trans_Add_ScoreVoidMenu(void *dummy);

int Trans_Add_WalletMenu(void *dummy);

int Trans_Add_IntallmentMenu(void *dummy);

int Trans_Add_MobileChipMenu(void *dummy);

int Trans_Add_YuyueMenu(void *dummy);

int Trans_Add_OrderMenu(void *dummy);

int Trans_Add_MagLoadMenu(void *dummy);

int Trans_Add_MagCashLoad( void *dummy );

int Trans_Add_MagAccountLoad( void *dummy );

int Trans_Add_GetInstallmentInfo( DynData *pTxnData );
int Trans_Add_GetOrderInfo( DynData *pTxnData );
int Trans_Add_GetScoreInfo( DynData *pTxnData );
int Trans_Add_GetYuyueInfo( DynData *pTxnData );
/*输入身份证件类型及号码*/
int Trans_Add_GetIDInfo( DynData *pTxnData );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: Trans_Add_CardholderVerify
 * DESCRIPTION:   持卡人信息验证
 * PARAMETERS:    dummy
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int Trans_Add_CardholderVerify( void *dummy );

#endif
