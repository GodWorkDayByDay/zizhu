#ifndef __ICCMD_H
#define __ICCMD_H

/*=========================================
 *   T Y P E    D E F I N I T I O N S     *
 =========================================*/

/* application layer command structure*/
typedef struct
{
	byte  CLA;         /*Class byte of the command message*/
	byte  INS;         /*Instrution*/
	byte  P1;          /*Parameter1*/
	byte  P2;          /*Parameter2*/
	byte  Lc;          /*length of being sent data*/
	byte  Le;          /*length of expected data*/
	byte  SW1;         /*status word1*/
	byte  SW2;         /*status word2*/
	byte  C_Data[256]; /*command data*/
	byte  R_Data[256]; /*response data*/
} ICC_APDU;


/*******************************************
 *       MARCO         DECLARATION         *
 *******************************************/
/*Function RETURN code*/
#define ICCARD_MAX_RET          100 /*max RETURN code, special flag*/
#define ICCARD_ENDOK            2   /*end of tran or card*/
#define ICCARD_RESET_OK         1   /*card reset ok*/
#define ICCARD_OK               0   /*operation ok*/
#define ICCARD_FAILED          -1   /*general failure, got sw12 from card*/
#define ICCARD_CARD_ERROR      -2   /*comm with card error,bad card*/
#define ICCARD_EMPTY           -3   /*no card in socket*/

/*Function RETURN code & failure code from card*/
#define ICCARD_USER_ABORT      -4
#define ICCARD_NEED_RESET      -5
#define ICCARD_PARAM_ERR       -6
#define ICCARD_FILE_NOTFOUND   -7
#define ICCARD_REC_NOTFOUND    -8
#define ICCARD_NOAPP_FOUND     -9
#define ICCARD_INVALID_APP     -10  /*invalid application*/
#define ICCARD_INVALID_CRD     -11   /*invalid card number*/
#define ICCARD_PIN_LOCKED      -12
#define ICCARD_INVALID_OP      -13
#define ICCARD_AMTLESS         -14  /*amount not enough*/
#define ICCARD_NOTAPPROV       -15  /*transaction not approved*/
#define ICCARD_INBLACK         -16  /*card in black list*/
#define ICCARD_WRONG_ISSUER    -17  /*wrong card issuer*/
#define ICCARD_EXPIRY_CARD     -18  /*expiry card*/
#define ICCARD_WRONG_PIN       -19  /*wrong user PIN*/
#define ICCARD_COMMERR         -21  /*communication failed*/
#define ICCARD_HOST_SNDERR     -22  /*failed in sending data to host*/
#define ICCARD_HOST_RCVERR     -23  /*failed in receiving data from host*/
#define ICCARD_ISSUE_REFUSE    -24  /*card issuer was refused*/
#define ICCARD_INSUFF_COND     -25  /*insufficient conditions*/
#define ICCARD_LACK_MAC        -26  /*lack of MAC*/
#define ICCARD_NOPSAM          -27   /*no PSAM card found*/
#define ICCARD_NOICCARD        -28  /*no IC card found*/
#define ICCARD_RESET_FAIL      -29  /*failed in reseting PSAM or ICC*/
#define ICCARD_PSAM_FAIL       -30  /*access PSAM failed*/
#define ICCARD_DB_WRONG        -31  /*access DB error*/
#define ICCARD_APP_LOCKED      -32  /*application was locked for ever*/
#define ICCARD_OP_CANCELED     -33  /*operation canceled */
#define ICCARD_OP_FAILED       -34  /* sw1,sw2 not 9000 */
#define ICCARD_NOT_START       -35
#define ICCARD_INV_KEYIDX      -36
#define ICCARD_INV_POSID       -37
#define ICCARD_INV_KEY         -38
#define ICCARD_NO_KEY          -39
#define ICCARD_MORE_MSG        -40
#define ICCARD_FORMAT_ERROR    -41
#define ICCARD_DATA_ERROR      -42
#define ICCARD_RPACK_ERROR     -43
#define ICCARD_INVLD_ATI       -44
#define ICCARD_INVLD_APPVER    -45
#define ICCARD_INVLD_DATE      -46
#define ICCARD_INVLD_CRDTYPE      -47/*added by zyp*/
#define ICCARD_PIN_ERROR          -48/*added by zyp in May-16-2003*/
#define ICCARD_OVER_AMT           -49/*added by zyp in May-16-2003*/
#define ICCARD_APPLOCKED_ALWAYS   -50/*added by zyp in May-21-2003*/


/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_Select
 * DESCRIPTION:   Send command 'select...' to ICC and RETURN FCI
 * PARAMETERS:    AppName - file name;       Len - length of AppName;
                  IsFirst - T=get first/F=get next;
                  FCI - data from card(out); PortID - port number
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED - data got from card, but tells not success
 *                ICCARD_CARD_ERROR - bad card or reinserted card
 *                ICCARD_EMPTY - no card in socket
 *----------------------------------------------------------------------*/
int ICCmd_Select(byte *AppName, byte Len, byte IsFirst, byte *FCI, byte FCILen, byte PortID);

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ReadBinary
 * DESCRIPTION:   Read binary file
 * PARAMETERS:    SFI - short file ID;  StartPos - offset(0..)
 *                data -(out); DLen - length of data; PortID
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_ReadBinary(byte SFI, byte StartPos, byte *data, byte DLen, byte PortID);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ReadRecord
 * DESCRIPTION:   Send command 'read record'
 * PARAMETERS:    SFI - short file ID;  RecNo - record number(1..)
 *                data -(out); DLen - length of data; PortID
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_ReadRecord(byte SFI, char RecNo, byte *data, byte DLen, byte PortID);

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_GetResponse
 * DESCRIPTION:   Send command 'get response'
 * PARAMETERS:    data - out; ExpLen - length expected;
 * RETURN:        >=0 length of data /
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_GetResponse(byte *data, byte ExpLen, byte PortID);

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_VerifyPin
 * DESCRIPTION:   Send command to card to check user's PIN(plain)
 * PARAMETERS:    AscPin - in(terminated by NULL);
 *                RemainCnt - remained try counter;
 * RETURN:        ICCARD_OK/ ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_VerifyPin(char *AscPin, byte *RemainCnt, byte PortID);

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SamDes
 * DESCRIPTION:   Send command  for descrypt
 * PARAMETERS:    mode - In   ;   data - In
 *                DataLen - In
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_SamDes(byte mode, byte *data, byte *DataLen, byte PortID);

/*-------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ConvertPIN
 * DESCRIPTION:   Convert ASC PIN to BCD PIN as 0X 0X 0X 0X 0X 0X
 *                (LEN ..PIN..)
 * PARAMETERS:    Pin - out;  AscPin - in
 * RETURN:        0-AscPin is not correct; > 0 - bytes of Pin block
 *------------------------------------------------------------------------*/
int ICCmd_ConvertPIN(char *Pin, char *AscPin);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SetDebug
 * DESCRIPTION:   Set Printing debug flag
 * RETURN:        NONE
 *----------------------------------------------------------------------*/
void ICCmd_SetDebug(byte flag);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_CheckICC
 * DESCRIPTION:   Check ICC if it is avalible
 * RETURN:        ICCARD_OK/ICCARD_FAILED/ICCARD_EMPTY/ICCARD_RESET_OK
 * NOTE:
 *----------------------------------------------------------------------*/
int ICCmd_CheckICC(byte PortID);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ResetICC
 * DESCRIPTION:   Reset ICC
 * RETURN:        ICCARD_OK/ICCARD_FAILED/ICCARD_EMPTY
 * NOTE:
*----------------------------------------------------------------------*/
int ICCmd_ResetICC(byte PortID);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_EndICComm
 * DESCRIPTION:   End communcation with ICC
 * RETURN:        ICCARD_ENDOK/ICCARD_FAILED
 * NOTE:
 *----------------------------------------------------------------------*/
int ICCmd_EndICComm(byte PortID);

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SendCommand
 * DESCRIPTION:   Send command
 * PARAMETERS:    mode - In   ;   data - In
 *                DataLen - In
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_SendCommand(byte *data, byte DataLen, byte PortID);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SelectFile
 * DESCRIPTION:   Send command 'select...' to ICC and RETURN FCI
 * PARAMETERS:    AppName - file name;       Len - length of AppName;
                  FCI - data from card(out); PortID - port number
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED - data got from card, but tells not success
 *                ICCARD_CARD_ERROR - bad card or reinserted card
 *                ICCARD_EMPTY - no card in socket
 *----------------------------------------------------------------------*/
int ICCmd_SelectFile(byte *AppName, byte *FCI, byte FCILen, byte PortID);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SelectMF
 * DESCRIPTION:   Select root in card/psam, and get SFI of file DIR
 * PARAMETER:     PortID - card socket
 * RETURN:        ICCARD_OK / ICCARD_FAILED
 * NOTE:          The MF name is "1PAY.SYS.DDF01"
 *----------------------------------------------------------------------*/
int ICCmd_SelectMF(byte PortID);

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_GetErrState
 * DESCRIPTION:   获取错误状态信息
 * PARAMETERS:
 *
 * RETURN:
 *-----------------------------------------------------------------------*/
int ICCmd_GetErrState(void);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SetDebug
 * DESCRIPTION:   Set Printing debug flag
 * RETURN:        NONE
 *----------------------------------------------------------------------*/
void ICCmd_SetDebug(byte flag);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SelectMF_ByID
 * DESCRIPTION:   Select root in card/psam, and get SFI of file DIR
 * PARAMETER:     PortID - card socket
 * RETURN:        ICCARD_OK / ICCARD_FAILED
 * NOTE:
 *----------------------------------------------------------------------*/
int ICCmd_SelectMF_ByID(byte *AppName, byte *FCI, byte FCILen, byte PortID);

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ReadBinary2
 * DESCRIPTION:   Read binary file
 * PARAMETERS:
 * RETURN:
 *-----------------------------------------------------------------------*/
int ICCmd_ReadBinary2(byte StartH, byte StartL, byte *data, byte DLen, byte PortID);


int ICCmdCTLS_OrgCheckICC(unsigned char PortID);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmdCTLS_EndICComm
 * DESCRIPTION:   End communcation with ICC
 * RETURN:        ICCARD_ENDOK/ICCARD_FAILED
 * NOTE:
 *----------------------------------------------------------------------*/
int ICCmdCTLS_EndICComm(unsigned char PortID);

/*------------------------------------------------------------------------
* FUNCTION NAME: ICCmdCTLS_GetBankCardInfo
* DESCRIPTION:
发送APDU命令，获取卡的磁道信息
*-----------------------------------------------------------------------*/
int ICCmdCTLS_GetBankCardInfo(byte *DateTime, byte *data, byte Dlen, byte PortID);

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmdCTLS_ReadBinarySH
 * DESCRIPTION:   Read binary file
 * PARAMETERS:    SFI - short file ID;  StartPos - offset(0..)
 *                data -(out); DLen - length of data; PortID
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmdCTLS_ReadBinarySH(unsigned char SFI, unsigned char StartPos, unsigned char *data, short DLen, unsigned char PortID);

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmdCTLS_Select
 * DESCRIPTION:   Send command 'select...' to ICC and return FCI
 * PARAMETERS:    AppName - file name;       Len - length of AppName;
 IsFirst - T=get first/F=get next;
 FCI - data from card(out); PortID - port number
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED - data got from card, but tells not success
 *                ICCARD_CARD_ERROR - bad card or reinserted card
 *                ICCARD_EMPTY - no card in socket
 *----------------------------------------------------------------------*/
int ICCmdCTLS_Select(unsigned char *AppName, unsigned char Len, int IsFirst, unsigned char *FCI,
                     short FCILen, unsigned char PortID);

#endif
