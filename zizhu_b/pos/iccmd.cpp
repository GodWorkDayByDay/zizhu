/**************************************************************************
 MODULE_NAME:  ICCMD
 PROGRAMMER:
 FILE:         ICCMD.C
 DESCRIPTION:
 DATE:
 **************************************************************************/

/*========================================
*             I N C L U D E S           *
========================================*/
#include "cupms.h"

/*=========================================
 *   L O C A L    F U N C T I O N S       *
 =========================================*/
int ICCmd_Comm(ICC_APDU *Apdu, byte PortID);
int ICCmd_CheckRsp(byte sw1, byte sw2);
int ICCmdCTLS_Comm(ICC_APDU *Apdu, byte PortID);
int ICCmdCTLS_CheckRsp(unsigned char sw1, unsigned char sw2);

static int  ICC_ErrState;    /*error status of card (accord to SW12)*/
static byte ICCMD_Debug_Flag;

/*========================================
 *   P U B L I C     F U N C T I O N     *
 ========================================*/
/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_Select
 * DESCRIPTION:   Send command 'select...' to ICC and return FCI
 * PARAMETERS:    AppName - file name;       Len - length of AppName;
                  IsFirst - T=get first/F=get next;
                  FCI - data from card(out); PortID - port number
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED - data got from card, but tells not success
 *                ICCARD_CARD_ERROR - bad card or reinserted card
 *                ICCARD_EMPTY - no card in socket
 *----------------------------------------------------------------------*/
int ICCmd_Select(byte *AppName, byte Len, byte IsFirst, byte *FCI, byte FCILen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.INS = 0xA4;
	Apdu.Lc  = Len;
	Apdu.P1  = 0x04;
	memcpy((char *)Apdu.C_Data, (char *)AppName, Len);
	
	if(!IsFirst)    Apdu.P2 = 0x02;     /*select next DF*/
	
	rv = ICCmd_Comm(&Apdu, PortID);
	
	if(rv >= 0)
	{
		if((rv > FCILen))    return ICCARD_FAILED;     /*buffer is too small*/
		
		memcpy((char *)FCI, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ReadBinary
 * DESCRIPTION:   Read binary file
 * PARAMETERS:    SFI - short file ID;  StartPos - offset(0..)
 *                data -(out); DLen - length of data; PortID
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_ReadBinary(byte SFI, byte StartPos,
                     byte *data, byte DLen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.INS = 0xB0;
	Apdu.P1 = SFI | 0x80;
	Apdu.P2 = StartPos;
	Apdu.Le = DLen;
	
	rv = ICCmd_Comm(&Apdu, PortID);
	
	if(rv >= 0)     /*command success*/
	{
		if(rv > DLen)  return ICCARD_FAILED;
		
		memcpy((char *)data, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ReadRecord
 * DESCRIPTION:   Send command 'read record'
 * PARAMETERS:    SFI - short file ID;  RecNo - record number(0..)
 *                data -(out); DLen - length of data; PortID
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_ReadRecord(byte SFI, char RecNo, byte *data, byte DLen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	char DispBuffer[ 32 ];
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.CLA = 0x00;
	Apdu.INS = 0xB2;
	Apdu.P1 = RecNo;
	Apdu.P2 = 0x04 | (SFI << 3);
	Apdu.Lc = 0;
	Apdu.Le = 0;
	
	rv = ICCmd_Comm(&Apdu, PortID);    /*rv is the Length of received data*/
	
	if(rv > 0)
	{
		if(rv > DLen)
			rv = DLen;
			
		memcpy((char *)data, (char *)Apdu.R_Data, rv);
	}
	
	if(ICCMD_Debug_Flag)
	{
		sprintf(DispBuffer, "rv = %d", rv);
		AppUtils_DisplayLine((char *)DispBuffer, LINE1);

		
		sprintf(DispBuffer, "SW1=%02X SW2=%02X", Apdu.SW1, Apdu.SW2);
		AppUtils_DisplayLine((char *)DispBuffer, LINE1);
        //get_char();
	}
	
	return rv;
}

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_GetResponse
 * DESCRIPTION:   Send command 'get response'
 * PARAMETERS:    data - out; ExpLen - length expected;
 * RETURN:        >=0 length of data /
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_GetResponse(byte *data, byte ExpLen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.INS = 0xC0;
	Apdu.Le = ExpLen;
	
	rv = ICCmd_Comm(&Apdu, PortID);
	
	if(rv > 0)
	{
		memcpy((char *)data, (char *)Apdu.R_Data, rv);
		return rv;
	}
	
	return rv;
}

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_VerifyPin
 * DESCRIPTION:   Send command to card to check user's PIN(plain)
 * PARAMETERS:    AscPin - in(terminated by NULL);
 *                RemainCnt - remained try counter;
 * RETURN:        ICCARD_OK/ ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_VerifyPin(char *AscPin, byte *RemainCnt, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv, len;
	byte      Pin[10];
	
	memset((char *)Pin, 0, sizeof(Pin));          /*get PIN in BCD format*/
	len = ICCmd_ConvertPIN((char *)Pin, AscPin);
	
	if(len == 0)  return ICCARD_FAILED;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.INS = 0x20;
	Apdu.Lc = Pin[0]; /* 密码长度 */
	memcpy((char *)Apdu.C_Data, (char *)(Pin + 1), Apdu.Lc);
	
	rv = ICCmd_Comm(&Apdu, PortID);
	
	if(rv >= 0)    return ICCARD_OK;     /*success in verifing*/
	
	*RemainCnt = 0;
	
	if(ICC_ErrState == ICCARD_WRONG_PIN)
		*RemainCnt = Apdu.SW2 & 0x0F;
		
	return rv;
}


/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SamDes
 * DESCRIPTION:   Send command  for descrypt
 * PARAMETERS:    mode - In   ;   data - In
 *                DataLen - In
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_SamDes(byte mode, byte *data, byte *DataLen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.CLA = 0x80;
	Apdu.INS = 0xF8;
	Apdu.P1 = mode;
	Apdu.P2 = 0x02;
	Apdu.Lc = *DataLen;
	memcpy((char *)Apdu.C_Data, (char *)data, *DataLen);
	
	rv = ICCmd_Comm(&Apdu, PortID);
	
	if(rv >= 0)
	{
		memcpy((char *)data, (char *)Apdu.R_Data, rv);
		return ICCARD_OK;
	}
	
	return rv;
	
}

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SendCommand
 * DESCRIPTION:   Send command
 * PARAMETERS:    mode - In   ;   data - In
 *                DataLen - In
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmd_SendCommand(byte *data, byte DataLen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	
	Apdu.CLA = *data;
	Apdu.INS = *(data + 1);
	Apdu.P1 = *(data + 2);
	Apdu.P2 = *(data + 3);
	Apdu.Lc = *(data + 4);
	Apdu.Le = DataLen;
	
	if(Apdu.Lc > 0)
		memcpy((char *)Apdu.C_Data, (char *)(data + 5), Apdu.Lc);
		
	rv = ICCmd_Comm(&Apdu, PortID);
	
	if(rv > 0)
	{
		if((DataLen != 0) && (rv > DataLen))
			return ICCARD_FAILED; /*buffer is too small*/
			
		memcpy((char *)data, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}

/*-------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ConvertPIN
 * DESCRIPTION:   Convert ASC PIN to BCD PIN as 0X 0X 0X 0X 0X 0X
 *                (LEN ..PIN..)
 * PARAMETERS:    Pin - out;  AscPin - in
 * RETURN:        0-AscPin is not correct; > 0 - bytes of Pin block
 *------------------------------------------------------------------------*/
int ICCmd_ConvertPIN(char *Pin, char *AscPin)
{
	int len, i;
	
	len = strlen(AscPin);
	
	if(len > 12)  return 0;    /*pin is too long*/
	
	for(i = 0; i < len; i++)
	{
		Pin[i + 1] = AscPin[i] - '0';
	}
	
	Pin[0] = len;
	
	return len;
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SetDebug
 * DESCRIPTION:   Set Printing debug flag
 * RETURN:        NONE
 *----------------------------------------------------------------------*/
void ICCmd_SetDebug(byte flag)
{
	ICCMD_Debug_Flag = flag;
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_CheckICC
 * DESCRIPTION:   Check ICC if it is avalible
 * RETURN:        ICCARD_OK/ICCARD_FAILED/ICCARD_EMPTY/ICCARD_RESET_OK
 * NOTE:
 *----------------------------------------------------------------------*/
int ICCmd_CheckICC(byte PortID)
{
	int i;
	byte bRv, ucProtocol, IFBuff[64], HistBuff[64];
	int rv;
	short IFLen, HistLen;
#ifndef MAKE_LINUX
	byte Value;
#endif
	
#ifdef MAKE_LINUX
	bRv = Close_CardSlot(PortID);
	AppDebug_AppLog("Close_CardSlot [%u]", bRv);
	
    if(Reinit_CardSlot(PortID, NULL) != 0) //C520的应用所有卡初始化都是调用该Reinit函数，init貌似在OS底层处理完了
	{
		AppUtils_ErrorBeep();
		AppDebug_AppLog("Reinit_CardSlot fail");
		return ICCARD_FAILED;
	}
	
#else
	

		return ICCARD_FAILED;

	
#endif
	
	
#ifdef MAKE_LINUX
	IFBuff[0] = (unsigned char)(CLASS_A >> 24);
	IFBuff[1] = (unsigned char)(CLASS_A >> 16);
	IFBuff[2] = (unsigned char)(CLASS_A >> 8);
	IFBuff[3] = (unsigned char)CLASS_A;
	
	bRv = Set_Capability(PortID, Tag_Class_Management, IFBuff);
	AppDebug_AppLog("Set_Capability =[%d]", bRv);
	
    if(bRv != 0)
	{
		AppDebug_AppLog("Set_Capability fail");
		return ICCARD_FAILED;
	}
	
	bRv = Set_Card_Type(PortID, ASYNC, BSCD_Standard_eISO);
	AppDebug_AppLog("Set_Card_Type =[%d]", bRv);
	
    if(bRv != 0)
	{
		AppDebug_AppLog("Set_Card_Type fail");
		return ICCARD_FAILED;
	}
	
#else

		return ICCARD_FAILED;

	
#endif
	

	
	AppDebug_AppLog("Get_Card_State =[%d]", bRv);
	

	

	{
#ifdef MAKE_LINUX
	
        if(Set_Card_Type(PortID, ASYNC, BSCD_Standard_eEMV2000) != 0)
		{
			AppDebug_AppLog("Set_Card_Type fail");
			return ICCARD_FAILED;
		}
		
#endif

		
        if(rv != 0)
		{
#ifdef MAKE_LINUX
			Terminate_CardSlot(PortID, SWITCH_OFF_CARD);
#endif
			AppDebug_AppLog("Reset_CardSlot fail");
			AppUtils_ErrorBeep();
			return ICCARD_FAILED;
		}
	}
	
	/* 获取协议类型T0 or T1 */

	AppDebug_AppLog("ucProtocol[%u]", ucProtocol);
	

	{
		AppDebug_AppLog("Get_Protocol failed");
		return ICCARD_FAILED;
	}
	
	
	memset(IFBuff, 0, sizeof(IFBuff));

	
	if(IFLen < 0)
	{
		AppDebug_AppLog("Get_Interface_Bytes failed");
		AppUtils_ErrorBeep();
		return ICCARD_FAILED;
	}
	
	
	memset(HistBuff , 0 , sizeof(HistBuff));

	
	if(HistLen < 0)
	{
		AppDebug_AppLog("Get_Historical_Bytes failed");
		AppUtils_ErrorBeep();
		return ICCARD_FAILED;
	}
	
	return ICCARD_OK;
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ResetICC
 * DESCRIPTION:   Reset ICC
 * RETURN:        ICCARD_OK/ICCARD_FAILED/ICCARD_EMPTY
 * NOTE:
*----------------------------------------------------------------------*/
int ICCmd_ResetICC(byte PortID)
{


    return 0;
}

/*===========================================
*      P R I V A T E    F U N C T I O N     *
=============================================*/
/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_Comm
 * DESCRIPTION:   Execute communication with specified card, if card return
 *                failure, this function will interpret and show result.
 *                It will set ICC_ErrState.
 * PARAMETERS:    Apdu - command apdu;  PortID - port number
 * RETURN:        >=0 (data)/ ICCARD_FAILED/
 *                ICCARD_EMPTY/ ICCARD_CARD_ERROR/
 *-----------------------------------------------------------------------*/
int ICCmd_Comm(ICC_APDU *Apdu, byte PortID)
{
	byte TxData[256], RxData[256], bSW1, bSW2;
	unsigned short TxLen, RxLen, TxOffset;
	byte  TxGetRespCmd[5] = {0x00, 0xC0, 0x00, 0x00, 0x00};
	byte bRet;
	char DispBuffer[64];
	
	memset(TxData, 0, sizeof(TxData));
	memset(RxData, 0, sizeof(RxData));
	TxLen = RxLen = 0;
	
	TxOffset = 0;
	TxData[TxOffset++] = Apdu->CLA;
	TxData[TxOffset++] = Apdu->INS;
	TxData[TxOffset++] = Apdu->P1;
	TxData[TxOffset++] = Apdu->P2;
	
	if(Apdu->Lc > 0)   /* Data to card */
	{
		TxData[TxOffset++] = Apdu->Lc;
		memcpy(TxData + TxOffset, Apdu->C_Data, Apdu->Lc);
		TxOffset += Apdu->Lc;
	}
	
	if(Apdu->Le > 0)   /* Expect data from card */
	{
		TxData[TxOffset++] = Apdu->Le;
	}
	
	// AppDebug_AppLog( "apdu=%02x,%02x,%02x,%02x,%02x,%02x", TxData[0], TxData[1], TxData[2], TxData[3], TxData[4], TxData[5] );
	

	//AppDebug_AppLog("bR1=%u RxLen=%u", bRet, RxLen);
	//AppDebug_AppLog("sw11=%02x sw12=%02x", RxData[RxLen - 2], RxData[RxLen - 1]);
	if(ICCMD_Debug_Flag)
	{
		sprintf(DispBuffer, "bR1=%u RxLen=%u", bRet, RxLen);
		AppUtils_DisplayLine((char *)DispBuffer, LINE1);
		sprintf(DispBuffer, "sw11=%02x sw12=%02x", RxData[RxLen - 2], RxData[RxLen - 1]);
		AppUtils_DisplayLine((char *)DispBuffer, LINE2);

	}

	if(RxLen < 2 + (Apdu->Le))
	{
		return -2;
	}
	
	/* 获取SW1和SW2 */
	bSW1 = RxData[RxLen - 2];
	bSW2 = RxData[RxLen - 1];
	
	
	if(bSW1 == 0x6C)    /* Le错误－使用SW2作为新的Le重传一次 */
	{
		memset(TxData, 0, sizeof(TxData));
		memset(RxData, 0, sizeof(RxData));
		
		TxOffset = 0;
		TxData[TxOffset++] = Apdu->CLA;
		TxData[TxOffset++] = Apdu->INS;
		TxData[TxOffset++] = Apdu->P1;
		TxData[TxOffset++] = Apdu->P2;
		
		if(Apdu->Lc > 0)   /* Data to card */
		{
			TxData[TxOffset++] = Apdu->Lc;
			memcpy(TxData + TxOffset, Apdu->C_Data, Apdu->Lc);
			TxOffset += Apdu->Lc;
		}
		
		//if (Apdu->Le > 0) /* Expect data from card 不能去判断Le */
		{
			TxData[TxOffset++] = bSW2;
		}
		
		
		TxLen = TxOffset;
		RxLen = 0;

		
		//AppDebug_AppLog("bR2=%u RxLen=%u", bRet, RxLen);
		//AppDebug_AppLog("sw21=%02x sw22=%02x", RxData[RxLen - 2], RxData[RxLen - 1]);
		if(ICCMD_Debug_Flag)
		{
			sprintf(DispBuffer, "bR2=%u RxLen=%u", bRet, RxLen);
			AppUtils_DisplayLine((char *)DispBuffer, LINE1);
			sprintf(DispBuffer, "sw21=%02x sw22=%02x", RxData[RxLen - 2], RxData[RxLen - 1]);
			AppUtils_DisplayLine((char *)DispBuffer, LINE2);

		}
		

		if(RxLen < 2 + bSW2)
		{
			return -2;
		}
		
		/* 获取SW1和SW2 */
		bSW1 = RxData[RxLen - 2];
		bSW2 = RxData[RxLen - 1];
	}
	
	
	if(bSW1 == 0x61)    /* 正确执行－还需要Get Response取响应数据 */
	{
		TxGetRespCmd[4] = bSW2;
		
		memset(RxData, 0, sizeof(RxData));
		RxLen = 0;

		//AppDebug_AppLog("bR3=%u RxLen=%u", bRet, RxLen);
		//AppDebug_AppLog("sw31=%02x sw32=%02x", RxData[RxLen - 2], RxData[RxLen - 1]);
		if(ICCMD_Debug_Flag)
		{
			sprintf(DispBuffer, "bR3=%u RxLen=%u", bRet, RxLen);
			AppUtils_DisplayLine((char *)DispBuffer, LINE1);
			sprintf(DispBuffer, "sw31=%02x sw32=%02x", RxData[RxLen - 2], RxData[RxLen - 1]);
			AppUtils_DisplayLine((char *)DispBuffer, LINE2);

		}

		if(RxLen < 2 + bSW2)
		{
			return -2;
		}
		
		/* 获取SW1和SW2 */
		Apdu->SW1 = bSW1 = RxData[RxLen - 2];
		Apdu->SW2 = bSW2 = RxData[RxLen - 1];
		
		if(ICCmd_CheckRsp(Apdu->SW1, Apdu->SW2))
		{
			memcpy(Apdu->R_Data, RxData, RxLen - 2);
			return (RxLen - 2);
		}
	}
	else
	{
		Apdu->SW1 = bSW1;
		Apdu->SW2 = bSW2;
		
		if(ICCmd_CheckRsp(Apdu->SW1, Apdu->SW2))
		{
			RxLen -= 2;
			
			//AppDebug_AppLog("ok(len=%d)", RxLen);
			if(ICCMD_Debug_Flag)
			{
				sprintf(DispBuffer, "ok(len=%d)", RxLen);
				AppUtils_DisplayLine((char *)DispBuffer, LINE1);

			}
			
			memcpy(Apdu->R_Data, RxData, RxLen);
			return (RxLen);
		}
	}
	
	return ICCARD_FAILED;
}

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_CheckRsp
 * DESCRIPTION:   Check the answer form the card
 * PARAMETERS:    sw1; sw2
 * RETURN:        TRUE  - sw1=0x90, sw2=0, operation success;
 *                FALSE - Error stauts stored in ICC_ErrStatus
 *-----------------------------------------------------------------------*/
int ICCmd_CheckRsp(byte sw1, byte sw2)
{
	if((sw1 == 0x90) && (sw2 == 0x00))
	{
		ICC_ErrState = ICCARD_OK;
		return 1;
	}
	
	else if(sw1 == 0x63)
		ICC_ErrState = ICCARD_WRONG_PIN;
	else if((sw1 == 0x69) && (sw2 == 0x83))
		ICC_ErrState = ICCARD_PIN_LOCKED;
	else if((sw1 == 0x94) && (sw2 == 0x01))
		ICC_ErrState = ICCARD_AMTLESS;
	else if((sw1 == 0x94) && (sw2 == 0x03))
		ICC_ErrState = ICCARD_INV_KEYIDX;
	else if((sw1 == 0x93) && (sw2 == 0x03))
		ICC_ErrState = ICCARD_APPLOCKED_ALWAYS;
	else if((sw1 == 0x6A) && (sw2 == 0x82))
		ICC_ErrState = ICCARD_FILE_NOTFOUND;
	else if((sw1 == 0x94) && (sw2 == 0x06))
		ICC_ErrState = ICCARD_OP_CANCELED;
	else if((sw1 == 0x6A) && (sw2 == 0x83))
		ICC_ErrState = ICCARD_REC_NOTFOUND;
	else if((sw1 == 0x69) && (sw2 == 0x85))
		ICC_ErrState = ICCARD_OVER_AMT;
	else
		ICC_ErrState = ICCARD_OP_FAILED;
		
	return 0;
}


/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SelectFile
 * DESCRIPTION:   Send command 'select...' to ICC and return FCI
 * PARAMETERS:    AppName - file name;       Len - length of AppName;
                  FCI - data from card(out); PortID - port number
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED - data got from card, but tells not success
 *                ICCARD_CARD_ERROR - bad card or reinserted card
 *                ICCARD_EMPTY - no card in socket
 *----------------------------------------------------------------------*/
int ICCmd_SelectFile(byte *AppName, byte *FCI, byte FCILen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	
	Apdu.INS = 0xA4;
	Apdu.P1  = 0x02;
	Apdu.P2  = 0x00;
	Apdu.Lc  = 2;
	memcpy((char *)Apdu.C_Data, (char *)AppName, 2);
	
	rv = ICCmd_Comm(&Apdu, PortID);
	
	if(rv >= 0)
	{
		if((rv > FCILen))    return ICCARD_FAILED;    /*buffer is too small*/
		
		memcpy((char *)FCI, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SelectMF
 * DESCRIPTION:   Select root in card/psam, and get SFI of file DIR
 * PARAMETER:     PortID - card socket
 * RETURN:        ICCARD_OK / ICCARD_FAILED
 * NOTE:          The MF name is "1PAY.SYS.DDF01"
 *----------------------------------------------------------------------*/
int ICCmd_SelectMF(byte PortID)
{
	byte  Mfile[20], len;
	byte  MF_FCI[100];
	int  rv;
	
	sprintf((char *)Mfile, "1PAY.SYS.DDF01");
	len = 14;
	rv = ICCmd_Select(Mfile, len, TRUE, (byte *)MF_FCI, sizeof(MF_FCI), PortID);
	
	if(rv <= 0)
		return ICCARD_FAILED;
		
	return ICCARD_OK;
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_SelectMF_ByID
 * DESCRIPTION:   Select root in card/psam, and get SFI of file DIR
 * PARAMETER:     PortID - card socket
 * RETURN:        ICCARD_OK / ICCARD_FAILED
 * NOTE:
 *----------------------------------------------------------------------*/
int ICCmd_SelectMF_ByID(byte *AppName, byte *FCI, byte FCILen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	
	Apdu.INS = 0xA4;
	Apdu.Lc  = 2;
	Apdu.P1  = 0x00;
	Apdu.P2  = 0x00;
	memcpy((char *)Apdu.C_Data, (char *)AppName, 2);
	rv = ICCmd_Comm(&Apdu, PortID);
	
	if(rv >= 0)
	{
		if((rv > FCILen))    return ICCARD_FAILED;    /*buffer is too small*/
		
		memcpy((char *)FCI, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_GetErrState
 * DESCRIPTION:   获取错误状态信息
 * PARAMETERS:
 *
 * RETURN:
 *-----------------------------------------------------------------------*/
int ICCmd_GetErrState(void)
{
	return ICC_ErrState;
}


/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmd_ReadBinary2
 * DESCRIPTION:   Read binary file
 * PARAMETERS:
 * RETURN:
 *-----------------------------------------------------------------------*/
int ICCmd_ReadBinary2(byte StartH, byte StartL, byte *data, byte DLen, byte PortID)
{
	ICC_APDU  Apdu;
	int      rv;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.INS = 0xB0;
	Apdu.P1 = StartH;
	Apdu.P2 = StartL;
	Apdu.Le = DLen;
	
	rv = ICCmd_Comm(&Apdu, PortID);
	//AppDebug_AppLog("ICCmd_Comm rv=[%d]", rv);
	
	if(rv >= 0)     /*command success*/
	{
		if(rv > DLen)  return ICCARD_FAILED;
		
		memcpy((char *)data, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}



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
                     short FCILen, unsigned char PortID)
{
	ICC_APDU Apdu;
	int rv = ICCARD_CARD_ERROR;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.INS = 0xA4;
	Apdu.Lc = Len;
	Apdu.P1 = 0x04;
	memcpy((char *)Apdu.C_Data, (char *)AppName, Len);
	
	Apdu.Le = 0xff;
	
	if(!IsFirst)
		Apdu.P2 = 0x02; /*select next DF*/
		
	rv = ICCmdCTLS_Comm(&Apdu, PortID);
	
	if(rv > 0)
	{
		if(rv > FCILen)
			rv = FCILen;
			
		memcpy((char *)FCI, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}


/*------------------------------------------------------------------------
* FUNCTION NAME: ICCmdCTLS_GetBankCardInfo
* DESCRIPTION:
发送APDU命令，获取卡的磁道信息
*-----------------------------------------------------------------------*/
int ICCmdCTLS_GetBankCardInfo(byte *DateTime, byte *data, byte Dlen, byte PortID)
{
	ICC_APDU  Apdu;
	int rv = ICCARD_CARD_ERROR;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	
	Apdu.CLA = 0x80;
	Apdu.INS = 0xF8;
	Apdu.P1 = 0x02;
	Apdu.P2 = 0x00;
	Apdu.Lc = 0x08;
	
	/*Data=Date+Time*/
	memcpy(Apdu.C_Data, DateTime, 7);
	Apdu.C_Data[7] = 0x80;
	
	rv = ICCmdCTLS_Comm(&Apdu, PortID);   /*rv is the Length of received data*/
	
	if(rv >= 0)
	{
		if(rv > Dlen)
		{
			return ICCARD_FAILED;
		}
		
		memcpy((char *)data, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}

/*-----------------------------------------------------------------------
 * 功能:  等待插卡
 *
 *----------------------------------------------------------------------*/
int ICCmdCTLS_OrgCheckICC(unsigned char PortID)
{
return 0;
}

/*-----------------------------------------------------------------------
 * FUNCTION NAME: ICCmdCTLS_EndICComm
 * DESCRIPTION:   End communcation with ICC
 * RETURN:        ICCARD_ENDOK/ICCARD_FAILED
 * NOTE:
 *----------------------------------------------------------------------*/
int ICCmdCTLS_EndICComm(unsigned char PortID)
{
    return 0;
}


/*===========================================
 *      P R I V A T E    F U N C T I O N     *
 =============================================*/
#define BYTE char
int ICCmdCTLS_APDU(BYTE PortID, BYTE CLA, BYTE INS, BYTE P1 , BYTE P2 , BYTE LC
                   , BYTE *DATA , BYTE LE, BYTE *RespData, BYTE *SW1, BYTE *SW2)
{
	ICC_APDU Apdu;
	int rv = ICCARD_CARD_ERROR;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.CLA = CLA;
	Apdu.INS = INS;
	Apdu.Lc = LC;
	Apdu.Le = LE;
	Apdu.P1 = P1;
	Apdu.P2 = P2;
	memcpy((char *)Apdu.C_Data, (char *)DATA, LC);
	
	/*************************************************************************
	 modified by LNM from "rv >=0" to "rv>0" at Nov-03-2005
	 **************************************************************************/
	rv = ICCmdCTLS_Comm(&Apdu, PortID);
	
	if(rv > 0)
	{
		memcpy((char *)RespData, (char *)Apdu.R_Data, rv);
		*SW1 = Apdu.SW1;
		*SW2 = Apdu.SW2;
	}
	
	return rv;
}

/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmdCTLS_Comm
 * DESCRIPTION:   Execute communication with specified card, if card return
 *                failure, this function will interpret and show result.
 *                It will set ICC_ErrState.
 * PARAMETERS:    Apdu - command apdu;  PortID - port number
 * RETURN:        >=0 (data)/ ICCARD_FAILED/
 *                ICCARD_EMPTY/ ICCARD_CARD_ERROR/
 *-----------------------------------------------------------------------*/
int ICCmdCTLS_Comm(ICC_APDU *Apdu, byte PortID)
{
	return ICCARD_FAILED;
}


/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmdCTLS_CheckRsp
 * DESCRIPTION:   Check the answer form the card
 * PARAMETERS:    sw1; sw2
 * RETURN:        TRUE  - sw1=0x90, sw2=0, operation success;
 *                FALSE - Error stauts stored in ICC_ErrStatus
 *-----------------------------------------------------------------------*/
int ICCmdCTLS_CheckRsp(unsigned char sw1, unsigned char sw2)
{
	if((sw1 == 0x90) && (sw2 == 0x00))
	{
		ICC_ErrState = ICCARD_OK;
		return TRUE;
	}
	else if(sw1 == 0x63)
		ICC_ErrState = ICCARD_WRONG_PIN;
	else if((sw1 == 0x69) && (sw2 == 0x83))
		ICC_ErrState = ICCARD_PIN_LOCKED;
	else if((sw1 == 0x94) && (sw2 == 0x01))
		ICC_ErrState = ICCARD_AMTLESS;
	else if((sw1 == 0x94) && (sw2 == 0x03))
		ICC_ErrState = ICCARD_INV_KEYIDX;
	else if((sw1 == 0x93) && (sw2 == 0x03))
		ICC_ErrState = ICCARD_APPLOCKED_ALWAYS; /*added by zyp*/
	else if((sw1 == 0x6A) && (sw2 == 0x82))
		ICC_ErrState = ICCARD_FILE_NOTFOUND;
	else if((sw1 == 0x94) && (sw2 == 0x06))
		ICC_ErrState = ICCARD_OP_CANCELED;
	else if((sw1 == 0x6A) && (sw2 == 0x83))      /*record not found*/
		ICC_ErrState = ICCARD_REC_NOTFOUND;
	else if((sw1 == 0x69) && (sw2 == 0x85))      /*added by zyp*/
		ICC_ErrState = ICCARD_OVER_AMT;
	else
		ICC_ErrState = ICCARD_OP_FAILED;
		
	return FALSE;
}


/*------------------------------------------------------------------------
 * FUNCTION NAME: ICCmdCTLS_ReadBinarySH
 * DESCRIPTION:   Read binary file
 * PARAMETERS:    SFI - short file ID;  StartPos - offset(0..)
 *                data -(out); DLen - length of data; PortID
 * RETURN:        >= 0, length of received data/
 *                ICCARD_FAILED/ ICCARD_CARD_ERROR/ ICCARD_EMPTY
 *-----------------------------------------------------------------------*/
int ICCmdCTLS_ReadBinarySH(unsigned char SFI, unsigned char StartPos, unsigned char *data, short DLen,
                           unsigned char PortID)
{
	ICC_APDU Apdu;
	int rv = ICCARD_CARD_ERROR;
	
	memset((char *)&Apdu, 0, sizeof(ICC_APDU));
	Apdu.INS = 0xB0;
	Apdu.P1 = SFI | 0x80;
	Apdu.P2 = StartPos;
	Apdu.Le = DLen;
	rv = ICCmdCTLS_Comm(&Apdu, PortID);
	
	if(rv >= 0)
	{
		if(rv > 0xFF)
			return ICCARD_FAILED;
			
		memcpy((char *)data, (char *)Apdu.R_Data, rv);
	}
	
	return rv;
}
