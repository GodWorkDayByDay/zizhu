/****************************************************************************
 * FILE NAME:   ctlscall.h
 * MODULE NAME: CTLSCALL
 * PROGRAMMER:  Zhao Yaping
 * DESCRIPTION: 非接回调函数
 * REVISION:    01.00 18/07/2012
  ****************************************************************************/

          /*==========================================*
           *         I N T R O D U C T I O N          *
           *==========================================*/
#ifndef __CTLSCALL_H
  #define __CTLSCALL_H



          /*==========================================*
           *         P A R A M E T E R S              *
           *==========================================*/

          /*==========================================*
           *           D E F I N I T I O N S          *
           *==========================================*/

          /*==========================================*
           *      T Y P E   D E C L A R A T I O N     *
           *==========================================*/


           /*=========================================*
            *        M I S C E L L A N E O U S        *
            *=========================================*/
/*-----------------------------------------------------------------------
 * FUNCTION NAME: CtlsCall_PromptRemoveCar
 * DESCRIPTION:   
 * PARAMETERS:    
 * RETURN:
 *----------------------------------------------------------------------*/
void CtlsCall_PromptRemoveCard(void);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: CtlsCall_PromptBalance
 * DESCRIPTION:   
 * PARAMETERS:    
 * RETURN:
 *----------------------------------------------------------------------*/
void CtlsCall_PromptBalance(byte rdr_type, byte BuildInUIEnable,  unsigned char *BCDBalance);
/*-----------------------------------------------------------------------
 * FUNCTION NAME: CtlsCall_PromptTransResult
 * DESCRIPTION:   
 * PARAMETERS:    
 * RETURN:
 *----------------------------------------------------------------------*/
void CtlsCall_PromptTransResult(byte rdr_type, byte BuildInUIEnable,  int ret, int FallbackFlag);

#endif
