/****************************************************************************
 * FILE NAME:   PRNDISP.C                                                   *
 * MODULE NAME: PRNDISP                                                     *
 * PROGRAMMER:  Laikey                                                      *
 * DESCRIPTION: 显示和打印模块                                              *
 * REVISION:                                                                *
 ****************************************************************************/
#include "cupms.h"
#include "zzprinter.h"
#include "detaildialog.h"

/* -----------------------------------------------------------------------------
* External variables / declarations
---------------------------------------------------------------------------- */
extern Term_Config gTermCfg;
extern unsigned char Batt_Flag;

#ifdef MAKE_LINUX
#define PAPER_MAX_COLUMN    48
#else
#define PAPER_MAX_COLUMN    42
#endif

/* -----------------------------------------------------------------------------
 * Private Function declare
 ---------------------------------------------------------------------------- */
static char *inPrnDisp_GetTransSupportName(int iSupportFlag, char *pRetName);
static char inPrnDisp_GetAmountSignSymbol(byte TransType);

//暂未使用
//static int inPrnDisp_CheckLocalArea(byte *IssuerCode);

static unsigned long gulStartTime;
static int giTimeOut;
extern int EMVtrans_RemoveCard();
void Print_CheckPrintFinish();
/* -----------------------------------------------------------------------------
 * Public Function define
 ---------------------------------------------------------------------------- */
void Print_CheckPrintFinish()
{
    return;
}
/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   PrnDisp_DisplayMenu
 * DESCRIPTION:     display menu
 * RETURN:          >0: sucess, return menu index
 *                  -1: timeout
 *                  -2: use press cancel key button
 ---------------------------------------------------------------------------- */
int PrnDisp_DisplayMenu(int iStartLine, char *pMenuTitle, byte bMenuOption, int iMenuItemMaxCnt, PrnDisp_DisplayMenuItem UserMenuItem[], int iMenuTimeout)
{
	int i, j;
	int X, Y;
	int iLen, iIndex;
	int iCurrentCnt, iDisplayCnt;
	int iPage, iMaxPage, iMaxLineCnt;
	int iDisplayLine;
	int iRetSelected;
	int iMaxCol, iMaxRow;
	int iSelectedMin, iSelectedMax;
	int iScreenItemNum[ 10 ];
	long lTimouter;
	byte bKey, bSelectedFlag;
	byte bTouchScreenFlag = 0x00;
	char cBuffer[ 56 ];
	char cTempBuf[ 26 ];
	PrnDisp_DisplayMenuItem tmpMenuItem[ 20 ], *pOneMenu;
	
	// get how many line can be used for menu on one screen
//  if(AppUtils_IsBigScreen() == BANK_OK)
//  {
//    iMaxLineCnt = 9;
//    iMaxCol = 18;
//  }
//  else
//  {
//    iMaxLineCnt = 4;
//    iMaxCol = 16;
//  }

	iMaxCol = AppUtils_GetColumns();
	iMaxLineCnt = AppUtils_GetRows();
	
	iMaxRow = iMaxLineCnt;
	
	if(strlen(pMenuTitle) > 0)
		iMaxLineCnt --;
		
	if(iStartLine != LINE1)
		iMaxLineCnt = iMaxLineCnt - iStartLine + 1;
		
	// get menu max count
	if(iMenuItemMaxCnt > 20)
		iMenuItemMaxCnt = 20;
		
	// get all menu can be display on screen by menuOption
	memset(&tmpMenuItem, 0, sizeof(tmpMenuItem));
	pOneMenu = &UserMenuItem[ 0 ];
	
	for(i = 0, j = 0; i < iMenuItemMaxCnt; i ++)
	{
		if(pOneMenu->EnableFunc == NULL || pOneMenu->EnableFunc() == 1)
			memcpy(&tmpMenuItem[ j++ ], pOneMenu, sizeof(PrnDisp_DisplayMenuItem));
			
		pOneMenu ++;
	}
	
	iMenuItemMaxCnt = j;
	
	// get max pages
	iMaxPage = (iMenuItemMaxCnt % iMaxLineCnt) ? (iMenuItemMaxCnt / iMaxLineCnt + 1) : (iMenuItemMaxCnt / iMaxLineCnt);
	
	if(bMenuOption & MENUOPT_1LINE2MENU)
		iMaxPage = (iMaxPage % 2) ? (iMaxPage / 2 + 1) : (iMaxPage / 2);
		
	// Set touch button property if need
	if(AppUtils_IsTouchScreen() == BANK_OK)
		AppUtilS_SetTouchButton(32, 24, 0x000000);
		
	// init some var
	for(i = 0; i < 10; i ++)
		iScreenItemNum[ i ] = 0;
		
	i = 0;
	iPage = 1;
	iIndex = 1;
	iDisplayCnt = 0;
	pOneMenu = &tmpMenuItem[ 0 ];
	
	while(TRUE)
	{
		// Display title if need
		if(strlen(pMenuTitle) > 0)
		{
			AppUtils_ClearLine(LINE1);
			
			if(bMenuOption & MENUOPT_NOI_NVERT_TITLE)
				AppUtils_DisplayCenter(pMenuTitle, LINE1, FALSE);
			else
				AppUtils_DisplayCenter(pMenuTitle, LINE1, TRUE);
		}
		
		iLen = 0;
		iCurrentCnt = 1;
		iDisplayLine = iStartLine;
		
		if((iStartLine == LINE1) && (strlen(pMenuTitle) > 0))
			iDisplayLine = LINE2;
			
		AppUtils_UserClear(iDisplayLine);
		
		if(!(bMenuOption & MENUOPT_INORDER))
			iIndex = 1;
		else
			iIndex = (pOneMenu - &tmpMenuItem[ 0 ]) + 1;
			
		// Display the menu on one screen
		if(AppUtils_IsColorScreen() == BANK_OK)
            AppUtils_SetFontSize(28);
			
		while(TRUE)
		{
			if(iDisplayLine ++ > iMaxRow)
				break;
			else if(memcmp(pOneMenu, &tmpMenuItem[ iMenuItemMaxCnt ], sizeof(PrnDisp_DisplayMenuItem)) == 0)
				break;
				
			memset(cBuffer, 0, sizeof(cBuffer));
			sprintf(cBuffer, "%d.%s", iIndex, pOneMenu->pMenuTitle);
			iLen = strlen(cBuffer);
			
			X = 1;
			Y = (iDisplayLine - 1) * 32;
			
			if(Y == 0)
				Y = 1;
				
			if(bMenuOption & MENUOPT_1LINE2MENU)
			{
				// if fix 1 line 2 menus option, each menu length must be 8bytes
				if(strlen(cBuffer) > 8)
					memset(cBuffer + 8, 0, sizeof(cBuffer) - 8);
				else
					memset(cBuffer + strlen(cBuffer), ' ', 8 - strlen(cBuffer));
			}
			else if(!(bMenuOption & MENUOPT_AUTOCHECKLEN))
			{
				// if fix 1 line 1 menu option, each menu length must be columms
				if(strlen(cBuffer) < iMaxCol)
					memset(cBuffer + strlen(cBuffer), ' ', iMaxCol - strlen(cBuffer));
			}
			
			if(AppUtils_IsTouchScreen() == BANK_OK)
				AppUtils_ShowTouchButton(cBuffer, &X, &Y, pOneMenu->ReturnIndex);
			else
				AppUtils_DisplayLine(cBuffer, iDisplayLine - 1);
				
			pOneMenu ++;
			iIndex ++;
			iCurrentCnt ++;
			
			// According menu option, check if menu need display self-adaptation by menu title length
			if(bMenuOption & MENUOPT_AUTOCHECKLEN)
			{
				if(memcmp(pOneMenu, &tmpMenuItem[ iMenuItemMaxCnt ], sizeof(PrnDisp_DisplayMenuItem)) == 0)
					break;
					
				iLen += strlen(pOneMenu->pMenuTitle) + 2;
				
				if(iLen <= iMaxCol)
				{
					if(AppUtils_IsTouchScreen() == BANK_OK)
					{
						sprintf(cBuffer, "%d.%s", iIndex, pOneMenu->pMenuTitle);
						
						iLen = strlen(cBuffer);
						j = 240 - iLen * 14;
						
						if(j < X + 2)
							X += 2;
						else if(j >= 156)
							X = j - 8;
						else
							X = j - 4;
							
						Y = (iDisplayLine - 1) * 32;
						
						if(Y == 0)
							Y = 1;
							
						AppUtils_ShowTouchButton(cBuffer, &X, &Y, pOneMenu->ReturnIndex);
					}
					else
					{
						sprintf(cTempBuf, "%d.%s", iIndex, pOneMenu->pMenuTitle);
						j = strlen(cTempBuf);
						
						if(j > iMaxCol)
							j = iMaxCol;
							
						memset(cBuffer + strlen(cBuffer), ' ', sizeof(cBuffer) - strlen(cBuffer));
						memcpy(cBuffer + iMaxCol - j, cTempBuf, j);
						
						AppUtils_ClearLine(iDisplayLine - 1);
						AppUtils_DisplayLine(cBuffer, iDisplayLine - 1);
					}
					
					pOneMenu ++;
					iIndex ++;
					iCurrentCnt ++;
				}
				
			}
			
			// According menu option, check if need display 2 menus at 1 line
			else if(bMenuOption & MENUOPT_1LINE2MENU)
			{
				if(memcmp(pOneMenu, &tmpMenuItem[ iMenuItemMaxCnt ], sizeof(PrnDisp_DisplayMenuItem)) == 0)
					break;
					
				memset(cTempBuf, 0, sizeof(cTempBuf));
				sprintf(cTempBuf, "%d.%s", iIndex, pOneMenu->pMenuTitle);
				
				// if fix 1 line 2 menus option, each menu length must be 8bytes
				if(strlen(cTempBuf) > 8)
					memset(cTempBuf + 8, 0, sizeof(cTempBuf) - 8);
				else
					memset(cTempBuf + strlen(cTempBuf), ' ', 8 - strlen(cTempBuf));
					
				if(AppUtils_IsTouchScreen() == BANK_OK)
				{
					X = 122;
					Y = (iDisplayLine - 1) * 32;
					
					if(Y == 0)
						Y = 1;
						
					AppUtils_ShowTouchButton(cTempBuf, &X, &Y, pOneMenu->ReturnIndex);
				}
				else
				{
					memcpy(cBuffer + 8, cTempBuf, 8);
					
					AppUtils_ClearLine(iDisplayLine - 1);
					AppUtils_DisplayLine(cBuffer, iDisplayLine - 1);
				}
				
				pOneMenu ++;
				iIndex ++;
				iCurrentCnt ++;
			}
		}   // while
		
		iDisplayCnt += iCurrentCnt - 1;
		iScreenItemNum[ iPage - 1 ] = iCurrentCnt - 1;
		
		// if all menu item has displayed on the 1st page screen, set max page num = 1
		if(iPage == 1 && iCurrentCnt > iMenuItemMaxCnt)
			iMaxPage = 1;
			
		// check if display up andd down arrow
		if((bMenuOption & MENUOPT_UP_DOWN_ARROW) && (iMaxPage > 1))
		{
			if(AppUtils_IsBigScreen() != BANK_OK)
			{
#if 0 //暂时去掉显示箭头
				AppUtils_SetFontFile(MENU_VxBUTTON);
				AppUtils_Window(1, 1, 21, 8);
				
				if((iPage == 1) && (iPage < iMaxPage))
					MmiUtil_WriteAt("\x2", 1 , 16, 8);
				else if((iPage == iMaxPage) || (iDisplayCnt >= iMenuItemMaxCnt))
					MmiUtil_WriteAt("\x1", 1 , 12, 8);
				else
				{
					MmiUtil_WriteAt("\x1", 1 , 12, 8);
					MmiUtil_WriteAt("\x2", 1 , 16, 8);
				}
				
				AppUtils_SetFontSize(FONT_16X16);
#endif
			}
			else
			{
				if((iPage == 1) && (iPage < iMaxPage))
				{
					bTouchScreenFlag = 0x01;
					AppUtils_DispBMP(MENU_NEXTBUTTON, 207, 300, 33, 17);
				}
				else if((iPage == iMaxPage) || (iDisplayCnt >= iMenuItemMaxCnt))
				{
					bTouchScreenFlag = 0x02;
					AppUtils_DispBMP(MENU_PREBUTTON, 172, 300, 33, 17);
				}
				else
				{
					bTouchScreenFlag = 0x03;
					AppUtils_DispBMP(MENU_NEXTBUTTON, 207, 300, 33, 17);
					AppUtils_DispBMP(MENU_PREBUTTON, 172, 300, 33, 17);
				}
			}
		}
		
		// Wait for button
		bSelectedFlag = FALSE;
		bKey = 0;
		lTimouter = TimeUtil_MillSec();
		AppUtils_FlushKbd();
		AppUtils_FlushCard();
		
		while(TRUE)
		{
			if((TimeUtil_MillSec() - lTimouter) / 1000 > iMenuTimeout)
				return -1;
				
			if((bKey = MmiUtil_CheckKey()) != 0)
			{
				if(bKey == bESC)
					return -2;
				else if(bKey == bENTER || bKey == bUP || bKey == bDOWN || bKey == bKEYC || bKey == bKEYD)
					break;
				else
				{
					iRetSelected = bKey - 0x30;
					
					if(bMenuOption & MENUOPT_INORDER)
					{
						iSelectedMin = 1; //( iPage - 1 ) * iMaxLineCnt + 1;
						iSelectedMax = iMenuItemMaxCnt + 1; //( iPage - 1 ) * iMaxLineCnt + iCurrentCnt;
					}
					else
					{
						iSelectedMin = 1;
						iSelectedMax = iCurrentCnt;
					}
					
					if((iRetSelected >= iSelectedMin) && (iRetSelected < iSelectedMax))
					{
						if(!(bMenuOption & MENUOPT_INORDER))
						{
							for(j = 1; j < iPage; j ++)
								iRetSelected += iScreenItemNum[ j - 1 ];
						}
						
						iRetSelected = tmpMenuItem[ iRetSelected - 1 ].ReturnIndex;
						bSelectedFlag = TRUE;
						break;
					}
				}
			}
			else if(AppUtils_CheckTouchButton(&iRetSelected))
			{
				bSelectedFlag = TRUE;
				break;
			}
			else if(AppUtils_IsBigScreen() == BANK_OK)
			{
                if(1)
				{
					if((bTouchScreenFlag & 0x01) && X > 207 && X < 207 + 33 && Y > 300 && Y < 300 + 17)
					{
						bKey = bENTER;
						break;
					}
					else if(bTouchScreenFlag & 0x02 && X > 172 && X < 172 + 33 && Y > 300 && Y < 300 + 17)
					{
						bKey = bUP;
						break;
					}
				}
			}
		} // while
		
		AppUtils_FlushKbd();
		AppUtils_FlushCard();
		
		// Process key action
		if(bSelectedFlag == TRUE)
		{
			// exec function or return menu index
			for(j = 0; j < iMenuItemMaxCnt; j ++)
			{
				if(tmpMenuItem[ j ].ReturnIndex == iRetSelected)
					break;
			}
			
			if(j < iMenuItemMaxCnt)
			{
				if(bMenuOption & MENUOPT_EXEC_FUNC)
				{
					if(tmpMenuItem[ j ].Func != NULL)
					{
						tmpMenuItem[ j ].Func((void *) tmpMenuItem[ j ].Parameter);
						
						// 请拔IC卡
						EMVtrans_RemoveCard();
						
						// 检测是否有事件要求
						Trans_DoHostRequireEventFuncion();
						
						if(bMenuOption & MENUOPT_RETURN)
							return bKey;
						else
						{
							// simulate up key to pre page
							if(iPage <= 1)
							{
								iPage = 1;
								iIndex = 1;
								iDisplayCnt = 0;
								pOneMenu = &tmpMenuItem[ 0 ];
							}
							else
							{
								pOneMenu -= iScreenItemNum[ iPage - 1 ];
								iDisplayCnt -= iScreenItemNum[ iPage - 1 ];
								
								if(iDisplayCnt < 0)
									iDisplayCnt = 0;
							}
						}
					}
					else
						return iRetSelected;
				}
				else
					return iRetSelected;
			}
		}
		else if(bKey == bESC)
			return -2;
		else if(bKey == bENTER || bKey == bDOWN || bKey == bKEYD)
		{
			if(bKey == bENTER && (bMenuOption & MENUOPT_ENTER_RETURN))
				return -3;
				
			// down key to next page
			iPage ++;
			
			if(iPage > iMaxPage || iDisplayCnt >= iMenuItemMaxCnt)
			{
				iPage = 1;
				iIndex = 1;
				iDisplayCnt = 0;
				pOneMenu = &tmpMenuItem[ 0 ];
			}
		}
		else if(bKey == bUP || bKey == bKEYC)
		{
			// up key to pre page
			iPage --;
			
			if(iPage <= 1)
			{
				iPage = 1;
				iIndex = 1;
				iDisplayCnt = 0;
				pOneMenu = &tmpMenuItem[ 0 ];
			}
			else
			{
				pOneMenu -= iScreenItemNum[ iPage ] + iScreenItemNum[ iPage - 1 ];
				iDisplayCnt -= iScreenItemNum[ iPage ] + iScreenItemNum[ iPage - 1 ];
				
				if(iDisplayCnt < 0)
					iDisplayCnt = 0;
			}
		}
		
	}   // while
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_DisplayBankCodeError
 DESCRIPTION:   explain error code means
 PARAMETERS:    iErrorCode
 RETURN:        None
 NOTE:
 *-------------------------------------------------------------------------*/
void PrnDisp_DisplayBankCodeError(int iErrorCode)
{
    DetailDialog::getInstance()->clearStartLines(LINE2, 6);
	switch(iErrorCode)
	{
	case BANK_OK:
		break;
		
	case BANK_ABORT:
		AppUtils_Warning("用户取消");
		break;
		
	case BANK_TIMEOUT:
		AppUtils_Warning("超时");
		break;
		
	case BANK_FAIL:
		AppUtils_Warning("失败");
		break;
		
	case BANK_DB_FOUND:
		AppUtils_Warning("记录已存在");
		break;
		
	case BANK_DB_NOTFOUND:
		AppUtils_Warning("记录未找到");
		break;
		
	case BANK_DB_FAIL:
		AppUtils_Warning("数据库失败");
		break;
		
	case BANK_DB_EMPTY:
		AppUtils_Warning("数据库为空");
		break;
		
	case BANK_DB_NOFILE:
		AppUtils_Warning("没有文件");
		break;
		
	case BANK_NOCONNECT:
		AppUtils_Warning("未连接");
		break;
		
	case BANK_PACKERROR:
		AppUtils_Warning("打包错误");
		break;
		
	case BANK_SENDERROR:
		AppUtils_Warning("发送错误");
		break;
		
	case BANK_COMMERROR:
		AppUtils_Warning("通讯错误");
		break;
		
	case BANK_MACERROR:
		AppUtils_Warning("MAC错误");
		break;
		
	case BANK_UNPACKERROR:
		AppUtils_Warning("解包错误");
		break;
		
	case BANK_HOSTERROR:
		AppUtils_Warning("主机错误");
		break;
		
	case BANK_CONTINUEDIAL:
		AppUtils_Warning("正在重拨");
		break;
		
	case BANK_COMM_FAIL:
		AppUtils_Warning("通讯错误");
		break;
		
	case BANK_ET_FAIL:
		AppUtils_Warning("充值失败");
		break;
		
	case BANK_RECVERROR:
		AppUtils_Warning("接收数据失败");
		break;
		
	case BANK_REVERSALERROR:
		AppUtils_Warning("冲正失败");
		break;
		
	case BANK_INVALIDE:
		AppUtils_Warning("数据非法");
		break;
	}
}


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   PrnDisp_DisplayTransName
 * DESCRIPTION:     display the meaning of transcation type
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void PrnDisp_DisplayTransName(int iTransType)
{
	char cBuffer[ 56 ];
	
	memset(cBuffer, 0, sizeof(cBuffer));
	
	Trans_GetTransName(iTransType, cBuffer);
    printf("%s\n", cBuffer);
	AppUtils_DisplayCenter(cBuffer, LINE1, TRUE);
	
	return;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayWarning
 * DESCRIPTION:   显示带标题的警告提示信息
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayWarning(char *pTitle, char *pWarningMsg)
{
	if(strlen(pTitle) > 0)
	{
		AppUtils_Cls();
		AppUtils_DisplayCenter(pTitle, LINE1, TRUE);
	}
	else
		AppUtils_UserClear(LINE2);
		
	AppUtils_DisplayCenter(pWarningMsg, LINE2, FALSE);
	
	return AppUtils_WaitKey(60);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayShowLogo
 * DESCRIPTION:   显示待机界面
 * PARAMETERS
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrnDisp_DisplayShowLogo(void)
{
	char cDateTimeStr[32] = {0};
	char cVersionStr[32] = {0};
	int  iDoubleScreenFlag = 0;
	
	AppUtils_GetSysDateTime(cDateTimeStr);

	if(AppUtils_IsBigScreen() == BANK_OK)
		iDoubleScreenFlag = 1;
		
	if(AppUtils_IsColorScreen() == BANK_OK)
	{
		if(AppUtils_IsRotatedScreen() == BANK_OK)
		{
			AppUtils_DispBMP(FLASH_PREFIX "WALLPAPER675.bmp", 0, 0, 320, 240);

			if(gTermCfg.mDynPrm.BankStatus & BANK_LOCKTERM)
				AppUtils_DisplayCenter("终端已锁定", LINE3, FALSE);
			else
				AppUtils_DisplayCenter("请插卡/刷卡", LINE3, FALSE);
				
			AppUtils_DispStrByColor(cVersionStr, 96, 158, 18, 0x0000, 0xffffff);
			AppUtils_DispStrByColor(cDateTimeStr, 39, 188, 24, 0x0000, 0xffffff);
		}
		else
		{
			AppUtils_DispBMP(FLASH_PREFIX "WALLPAPER.BMP", 0, 0, 240, 320);
			AppUtils_DispBMP(FLASH_PREFIX "CUPLOGO.BMP", 0, 36, 240, 65);
			
			if(gTermCfg.mDynPrm.BankStatus & BANK_LOCKTERM)
				AppUtils_DisplayCenter("终端已锁定", LINE4, FALSE);
			else
				AppUtils_DisplayCenter("请插卡/刷卡", LINE4, FALSE);
				
			AppUtils_DispStrByColor(cVersionStr, 56, 238, 18, 0x0000, 0xffffff);
			AppUtils_DispStrByColor(cDateTimeStr, 35, 278, 18, 0x0000, 0xffffff);
		}
		
		if(AppUtils_IsBatteryTerminal() == BANK_OK)
			PrnDisp_DisplayBatteryStatus();
			
		if(AppUtils_IsGprsTerminal() == BANK_OK)
			PrnDisp_DispSignalStatus();
	}
	else
	{
		if(AppUtils_IsGprsTerminal() == BANK_OK || AppUtils_IsBigScreen() == BANK_OK)
			AppUtils_Window(1, 1, 21, 8 *(iDoubleScreenFlag + 1) - 1);
		else
			AppUtils_Window(1, 1, 21, 8 *(iDoubleScreenFlag + 1));
			
		AppUtils_Cls();
		

		if(gTermCfg.mDynPrm.BankStatus & BANK_LOCKTERM)
			AppUtils_DispChinese("终端已锁定", 7, 3 + (2 * iDoubleScreenFlag));
		else
			AppUtils_DispChinese("请插卡/刷卡", 6, 3 + (2 * iDoubleScreenFlag));
			
		AppUtils_SetFontFile("");
		AppUtils_DispEnglish(cDateTimeStr, 1, 8 + (iDoubleScreenFlag * 7));
	}
	
	return (0);
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayRespCode
 * DESCRIPTION:   显示响应码对应的错误信息
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayRespCode(DynData *pTxnData)
{
	char cDispBuf[ 128 ] = {0};
	char cRespMsg[ 128 ] = {0};
	
	if(AppUtils_IsNullData((char *)pTxnData->TransDB.RspCode, 2) == BANK_OK)
	{
		AppDebug_AppLog("PrnDisp_DisplayRespCode, RspCode is null fail");
		AppUtils_ErrorBeep();
		//PrnDisp_DisplayWarning( "", "主机无返回响应" );
		return BANK_FAIL;
	}
	
	memset(cDispBuf, 0, sizeof(cDispBuf));
	memset(cRespMsg, 0, sizeof(cRespMsg));
	
	AppUtils_UserClear(LINE2);
	
	if(memcmp(pTxnData->TransDB.RspCode, "00", 2) == 0)
	{
		if(AppUtils_IsNullData((char *)pTxnData->AddRspCode, 2) != BANK_OK)
		{
			if(AppUtils_GetFileString(RESP_FILE, "RESP_CODE", (char *)pTxnData->AddRspCode, cRespMsg) > 0)
			{
				sprintf(cDispBuf, "[%2.2s]%s", pTxnData->AddRspCode, cRespMsg);
				
				AppUtils_ErrorBeep();
				AppUtils_DisplayCenter(cDispBuf, LINE2, FALSE);
				AppUtils_WaitKey(2);
			}
		}
		
		return BANK_OK;
	}
	else
	{
		if(AppUtils_GetFileString(RESP_FILE, "RESP_CODE", (char *)pTxnData->TransDB.RspCode, cRespMsg) <= 0)
			sprintf(cDispBuf, "[%2.2s]交易失败", pTxnData->TransDB.RspCode);
		else
			sprintf(cDispBuf, "[%2.2s]%s", pTxnData->TransDB.RspCode, cRespMsg);
			
		AppUtils_ErrorBeep();
		PrnDisp_DisplayWarning("", cDispBuf);
		
		if(memcmp(pTxnData->TransDB.RspCode, "A0", 2) == 0 || memcmp(pTxnData->TransDB.RspCode, "99", 2) == 0)
		{
			// 校验错或PIN格式错，应重新签到获取工作密钥
			AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_OPENED);
			PrmMngr_SavePrmToSamDisk();
			
			return(BANK_MACERROR);
		}
		
		return BANK_FAIL;
	}
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayVersion
 * DESCRIPTION:   显示应用软件版本信息
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayVersion(void *dummy)
{
	char cBuffer[ 56 ];
	
	AppUtils_UserClear(LINE1);

	
	//sprintf( cBuffer, "终端号:%8.8s", gTermCfg.mFixPrm.TerminalID );
	//AppUtils_DisplayLine( cBuffer, LINE3 );
	
	sprintf(cBuffer, "构建时间:%s", AppUtils_GetAppBuildTimeStamp());
	AppUtils_DisplayLine(cBuffer, LINE3);
	
	AppUtils_WaitKey(60);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayBatteryStatus
 * DESCRIPTION:   显示电池状态信息
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_DisplayBatteryStatus(void)
{
#ifdef MAKE_LINUX
	return;
#else
	int status = 0, Bvalue = 0, dock = 0;
	char buf[2];
	int offset = 0 ;
	static int Bvol = 0;
	static char LastBattTime[22] = {0};
	char CurrBattTime[22] = {0};
	int bat_cap;
	char sbuf[32];
	
RETRY:
	
	if(AppUtils_IsRotatedScreen() == BANK_OK)
	{
		AppUtils_GetClock(CurrBattTime);
	
		//原电压为0 或间隔上次取电压时间超过10秒
		if(Bvol == 0 || memcmp(LastBattTime, CurrBattTime, 13))
		{

			if(Bvol > 3780) //超过3780mV 清掉虚电标志
				Batt_Flag = 1;
		}
	
		Bvalue = AppUtils_BatteryVolToValue(Bvol);
		strcpy(LastBattTime, CurrBattTime);
	}

	if(AppUtils_IsColorScreen() == BANK_OK)
	{
		if(AppUtils_IsRotatedScreen() == BANK_OK)
		{
			AppUtils_DispBMP(FLASH_PREFIX "battery.bmp", 10, 225, 24, 12);
	
			bat_cap = (Bvalue % 5) ? (Bvalue / 5 + 1) : (Bvalue / 5);
			bat_cap = (bat_cap > 18) ? 20 : bat_cap;
	
			if(bat_cap > 4)
				AppUtils_DispBMP(FLASH_PREFIX "capacity.bmp", 11, 227, bat_cap, 8);
			else
				AppUtils_DispBMP(FLASH_PREFIX "low_battery.bmp", 12, 227, bat_cap, 8);
	
			//因为VX675的电量取值问题，不显示电量值，只显示前面的ICON
			//sprintf(sbuf, "%02d", Bvalue);
			//AppUtils_DispStrByColor(sbuf, 4, 226, 12, 0x000000, 0xffffff);
	

			{
				if(Bvol < 3650)
				{
					AppUtils_ClearLine(65);
					AppUtils_DisplayCenter("电量低", 5, 0);
					AppUtils_ErrorBeep();
					AppUtils_WaitKey(2);
				}
	
				if(Bvol < 3600)
				{
					Bvol = 0;
					AppUtils_ClearLine(5);
					AppUtils_DisplayCenter("电量低 请充电", 5, 0);
					AppUtils_ErrorBeep();
					AppUtils_WaitKey(1);
					goto RETRY;
				}
			}
		}
		else
		{
			AppUtils_DispBMP(FLASH_PREFIX "battery.bmp", 5, 305, 24, 12);
	
			bat_cap = (Bvalue % 5) ? (Bvalue / 5 + 1) : (Bvalue / 5);
			bat_cap = (bat_cap > 18) ? 20 : bat_cap;
	
			if(bat_cap > 4)
				AppUtils_DispBMP(FLASH_PREFIX "capacity.bmp", 6, 307, bat_cap, 8);
			else
				AppUtils_DispBMP(FLASH_PREFIX "low_battery.bmp", 7, 307, bat_cap, 8);
	
	
			sprintf(sbuf, "%02d", Bvalue);
			AppUtils_DispStrByColor(sbuf, 4, 306, 12, 0x000000, 0xffffff);

			{
				if(Bvalue < 25)
				{
					AppUtils_ClearLine(6);
					AppUtils_DisplayCenter("电量低", 6, 0);
					AppUtils_ErrorBeep();
					AppUtils_WaitKey(2);
				}
	
				if(Bvalue < 20)
				{
					AppUtils_ClearLine(6);
					AppUtils_DisplayCenter("电量低 请充电", 6, 0);
					AppUtils_ErrorBeep();
					AppUtils_WaitKey(1);
					goto RETRY;
	
				}
			}
		}
	}
	else
	{
		if(status == -1)   //没有电池
		{
			offset = 11;
		}
		else
		{
			if((Bvalue >= 81) && (Bvalue <= 110))
			{
				offset = 10;
			}
			else if((Bvalue >= 61) && (Bvalue <= 80))
			{
				offset = 9;
			}
			else if((Bvalue >= 41) && (Bvalue <= 60))
			{
				offset = 8;
			}
			else if((Bvalue >= 21) && (Bvalue <= 40))
			{
				offset = 7;
			}
			else if((Bvalue >= 10) && (Bvalue <= 20))
			{
				offset = 6;
			}
			else
			{
				offset = 5;
			}
	
		}
	
		//电量图标
		buf[0] = offset;
		buf[1] = 0;
	


		buf[1] = 0;
		//display_at( 3 , 16 , buf , NO_CLEAR );

		//////////////////////////////////////////////////////////////////////////

		AppUtils_Window(1, 1, 21, 16);
	}
	
#endif
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DispSignalStatus
 * DESCRIPTION:   显示信号状态信息
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_DispSignalStatus(void)
{


}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayTxnDetail
 * DESCRIPTION:   显示交易明细
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayTxnDetail(DynData *pTxnData, int iTitleTranType)
{
	int iLine;
	byte bRet;
	char bNegativeFlag;
	char cBuffer[ 128 ], cTempBuf[32];
	int iPageNo = 0;

	
	if(AppUtils_IsColorScreen() == BANK_OK)
	{
		while(TRUE)
		{
			AppUtils_Cls();
			iLine = LINE1;
			
			// 交易类型
			memset(cBuffer, 0, sizeof(cBuffer));
			
			if(iTitleTranType != 0)
				Trans_GetTransName(iTitleTranType, cBuffer);
			else
				Trans_GetTransName(pTxnData->TransDB.TrType, cBuffer);
				
			if(AppUtils_CheckStatus(pTxnData->TransDB.Status[ 0 ], STATUS_VOID) == BANK_OK)
			{
				//需要显示(已撤)标志的交易，应读取短交易名称
				memset(cBuffer, 0, sizeof(cBuffer));
				
				if(Trans_GetShortTransName(pTxnData->TransDB.TrType, cBuffer) == BANK_FAIL)
					Trans_GetTransName(pTxnData->TransDB.TrType, cBuffer);
				else
					strcat(cBuffer, "(已撤)");
			}
			else if(AppUtils_CheckStatus(pTxnData->TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
				strcat(cBuffer, "(已调整)");
				
			if(pTxnData->TransDB.TrType == BTYPE_SALE)   //消费交易应根据具体情况显示
			{
				if((pTxnData->TransDB.Swipe == TRAN_QPBOC && pTxnData->TransDB.qPBOC_OnlineFlag == 0) ||
				   (pTxnData->TransDB.EC_flag == 1))
				{
					memset(cBuffer, 0, sizeof(cBuffer));
					strcpy(cBuffer, "电子现金消费");
				}
			}
			
			AppUtils_DisplayCenter(cBuffer, iLine, TRUE);
			AppUtils_UserClear(LINE2);
			iLine ++;
			
			// 卡号
			memset(cBuffer, 0, sizeof(cBuffer));
			AppUtils_ConvertPANbyMask(pTxnData, 1, cBuffer);
			
			if(AppUtils_IsBigScreen() == BANK_OK)
			{
				AppUtils_DisplayLine(cBuffer, iLine);
			}
			else
			{
				iLine = 3;
				AppUtils_DispEnglish(cBuffer, 1, iLine);
			}
			
			iLine ++;
			
			// 交易时间
			memset(cBuffer, 0, sizeof(cBuffer));
            sprintf(cBuffer, "Time:  ");
            AppUtils_ConvertTransDateTime(pTxnData, cBuffer + 7);
            qDebug("1 cBuffer:[%s]", cBuffer);
			if(AppUtils_IsBigScreen() == BANK_OK)
			{
                memset(cBuffer, 0, 5);
                memcpy(&cBuffer, "时间:  ",7);
                AppUtils_DisplayLine(cBuffer, iLine);

                 qDebug("2 cBuffer:[%s]", cBuffer);
			}
			else
				AppUtils_DispEnglish(cBuffer, 1, iLine);
				
			iLine ++;
			
			// 金额
			bNegativeFlag = AppUtils_ConvertAmountNegativeFlag(pTxnData);
			
			memset(cBuffer, 0, sizeof(cBuffer));
			
			if(AppUtils_IsBigScreen() == BANK_OK)
			{
				sprintf(cBuffer, "金额:");
				AppUtils_AmountAsc2String(pTxnData->TransDB.Amount, "", bNegativeFlag, cBuffer + 7);
				AppUtils_DisplayLine(cBuffer, iLine);
			}
			else
			{
				sprintf(cBuffer, "AMOUNT:");
				AppUtils_AmountAsc2String(pTxnData->TransDB.Amount, "", bNegativeFlag, cBuffer + 7);
				AppUtils_DispEnglish(cBuffer, 1, iLine);
			}
			
			iLine ++;
			
			// 凭证号
			memset(cBuffer, 0, sizeof(cBuffer));
			
			if(AppUtils_IsBigScreen() == BANK_OK)
			{
				sprintf(cBuffer, "凭证号:%06ld", pTxnData->TransDB.SysTrace);
				AppUtils_DisplayLine(cBuffer, iLine);
			}
			else
			{
				sprintf(cBuffer, "VOUCHER:%06ld", pTxnData->TransDB.SysTrace);
				AppUtils_DispEnglish(cBuffer, 1, iLine);
			}
			
			iLine ++;
			
			// 批次号
			memset(cBuffer, 0, sizeof(cBuffer));
			
			if(AppUtils_IsBigScreen() == BANK_OK)
			{
				AppUtils_DisplayLine((char *)"参考号:", iLine++);
				sprintf(cBuffer, "%s", pTxnData->TransDB.RefCode);
				AppUtils_DisplayLine(cBuffer, iLine);
			}
			else
			{
				sprintf(cBuffer, "REFNO: %s", pTxnData->TransDB.RefCode);
				AppUtils_DispEnglish(cBuffer, 1, iLine);
			}
			
			iLine ++;
			
#if 0
			// 批次号
			memset(cBuffer, 0, sizeof(cBuffer));
			
			if(AppUtils_IsBigScreen() == BANK_OK)
			{
				sprintf(cBuffer, "批次号:%06ld", pTxnData->TransDB.BatchNum);
				AppUtils_DisplayLine(cBuffer, iLine);
			}
			else
			{
				sprintf(cBuffer, "Batch:%06ld", pTxnData->TransDB.BatchNum);
				AppUtils_DispEnglish(cBuffer, 1, iLine);
			}
			
			iLine ++;
			
			
			// 授权码
			memset(cBuffer, 0, sizeof(cBuffer));
			
			if(AppUtils_IsBigScreen() == BANK_OK)
			{
				sprintf(cBuffer, "授权码:%6.6s", pTxnData->TransDB.AuthCode);
				AppUtils_DisplayLine(cBuffer, iLine);
			}
			else
			{
				sprintf(cBuffer, "Auth: %6.6s", pTxnData->TransDB.AuthCode);
				AppUtils_DispEnglish(cBuffer, 1, iLine);
			}
			
			iLine ++;
#endif
            bRet = AppUtils_get_WaitKey(60);
			
			if((bRet == 0) || (bRet == bESC))
			{
				return (int)bESC;
			}
			else
			{
				return (int)bRet;
			}
		}
	}
	else
	{
		while(TRUE)
		{
			if(iPageNo == 0)
			{
				AppUtils_Cls();
				
				// 交易类型
				memset(cBuffer, 0, sizeof(cBuffer));
				
				if(iTitleTranType != 0)
					Trans_GetTransName(iTitleTranType, cBuffer);
				else
					Trans_GetTransName(pTxnData->TransDB.TrType, cBuffer);
					
				if(AppUtils_CheckStatus(pTxnData->TransDB.Status[ 0 ], STATUS_VOID) == BANK_OK)
				{
					//需要显示(已撤)标志的交易，应读取短交易名称
					memset(cBuffer, 0, sizeof(cBuffer));
					
					if(Trans_GetShortTransName(pTxnData->TransDB.TrType, cBuffer) == BANK_FAIL)
						Trans_GetTransName(pTxnData->TransDB.TrType, cBuffer);
					else
						strcat(cBuffer, "(已撤)");
				}
				else if(AppUtils_CheckStatus(pTxnData->TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
					strcat(cBuffer, "(已调整)");
					
				if(pTxnData->TransDB.TrType == BTYPE_SALE)   //消费交易应根据具体情况显示
				{
					if(pTxnData->TransDB.Swipe == TRAN_QPBOC && pTxnData->TransDB.qPBOC_OnlineFlag == 0)
					{
						memset(cBuffer, 0, sizeof(cBuffer));
						strcpy(cBuffer, "快速支付");
					}
					else if(pTxnData->TransDB.EC_flag == 1)
					{
						memset(cBuffer, 0, sizeof(cBuffer));
						strcpy(cBuffer, "普通支付");
					}
					
					if(AppUtils_CheckStatus(pTxnData->TransDB.Status[ 0 ], STATUS_UPLOAD) == BANK_OK)
					{
						strcat(cBuffer, "(已上送)");
					}
				}
				
				AppUtils_DisplayCenter(cBuffer, LINE1, TRUE);
				
				// 卡号
				memset(cBuffer, 0, sizeof(cBuffer));
				AppUtils_ConvertPANbyMask(pTxnData, 1, cBuffer);
				
				if(strlen(cBuffer) > 16)
				{
					AppUtils_DispEnglish((char*)"CARD NO:", 1, 3);
					AppUtils_DispEnglish(cBuffer, 1, 4);
				}
				else
					AppUtils_DisplayLine(cBuffer, LINE2);
					
				// 凭证号
				memset(cBuffer, 0, sizeof(cBuffer));
				sprintf(cBuffer, "凭证号:%06ld", pTxnData->TransDB.SysTrace);
				AppUtils_DisplayLine(cBuffer, LINE3);
				
				// 授权号
				memset(cBuffer, 0, sizeof(cBuffer));
				sprintf(cBuffer, "授权号:%6.6s", pTxnData->TransDB.AuthCode);
				AppUtils_DisplayLine(cBuffer, LINE4);
				
				PrnDisp_DisplayDownArrow(); //显示下箭头
			}
			else
			{
				AppUtils_Cls();
				
				// 交易时间
                memset(cBuffer, 0, sizeof(cBuffer));
                AppUtils_ConvertTransDateTime(pTxnData, cBuffer);
                AppUtils_DispEnglish(cBuffer, 2, 7);
				memset(cBuffer, 0, sizeof(cBuffer));
				sprintf(cBuffer, "日期: %02X%02X-%02X-%02X",
				        pTxnData->TransDB.TranDate[0], pTxnData->TransDB.TranDate[1], pTxnData->TransDB.TranDate[2], pTxnData->TransDB.TranDate[3]);
				AppUtils_DisplayLine(cBuffer, LINE1);
				memset(cBuffer, 0, sizeof(cBuffer));
				sprintf(cBuffer, "时间: %02X:%02X:%02X",
				        pTxnData->TransDB.TranTime[0], pTxnData->TransDB.TranTime[1], pTxnData->TransDB.TranTime[2]);
				AppUtils_DisplayLine(cBuffer, LINE2);
				
				// 金额
				bNegativeFlag = AppUtils_ConvertAmountNegativeFlag(pTxnData);
				memset(cTempBuf, 0, sizeof(cTempBuf));
				AppUtils_AmountAsc2String(pTxnData->TransDB.Amount, "", bNegativeFlag, cTempBuf);
				sprintf(cBuffer, "金额:%11s", cTempBuf);
				AppUtils_DisplayLine(cBuffer, LINE3);
				
				// 参考号
				AppUtils_DisplayLine((char *)"参考号:", LINE4);
				memset(cBuffer, 0, sizeof(cBuffer));
				sprintf(cBuffer, "%s", pTxnData->TransDB.RefCode);
				AppUtils_DispEnglish(cBuffer, 10, 7);
				
				PrnDisp_DisplayUpArrow(); //显示上箭头
			}
			
            bRet = AppUtils_get_WaitKey(60);
			
			if(bRet == bKEYC)
			{
				if(iPageNo == 1)
					iPageNo = 0;
			}
			else if(bRet == bKEYD)
			{
				if(iPageNo == 0)
					iPageNo = 1;
			}
			else if((bRet == 0) || (bRet == bESC))
			{
				return (int)bESC;
			}
			else if(bRet == bENTER)
			{
				return (int)bENTER;
			}
		}
	}
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayUpArrow
 * DESCRIPTION:   显示上箭头
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayUpArrow(void)
{
#ifdef  MAKE_LINUX
	AppUtils_DispBMP((char *)"up.bmp", 91, 58, 5, 5);
#else
	
#endif
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayDownArrow
 * DESCRIPTION:   显示下箭头
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayDownArrow(void)
{
#ifdef  MAKE_LINUX
	AppUtils_DispBMP((char *)"down.bmp", 117, 59, 5, 5);
#else
	
#endif
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayTxnTotal
 * DESCRIPTION:   显示内外卡交易汇总
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayTxnTotal(TotalRec *pTotalsData)
{
	char cBuffer[ 56 ];
	char cDispBuf[32];
	byte bKey;
	
	AppUtils_Cls();
	
	AppUtils_DisplayCenter("内卡汇总查询", LINE1, TRUE);
	
	while(1)
	{
		sprintf(cDispBuf, "借记笔数:%7d", pTotalsData->CupTotalSaleCnt);
		AppUtils_DisplayLine(cDispBuf, LINE2);
		
		memset(cBuffer, 0, sizeof(cBuffer));
		AppUtils_AmountAsc2String(pTotalsData->CupTotalSaleAmt, "", '+', cBuffer);
		sprintf(cDispBuf, "金额:%11s", cBuffer);
		AppUtils_DisplayLine(cDispBuf, LINE3);
		
		bKey = AppUtils_WaitKey(30);
		
		if(bKey == bESC || bKey == 0)
			return BANK_FAIL;
		else if(bKey == bENTER)
			break;
	}
	
	while(1)
	{
		sprintf(cDispBuf, "贷记笔数:%7d", pTotalsData->CupTotalVoidCnt);
		AppUtils_DisplayLine(cDispBuf, LINE2);
		
		memset(cBuffer, 0, sizeof(cBuffer));
		AppUtils_AmountAsc2String(pTotalsData->CupTotalVoidAmt, "", '+', cBuffer);
		sprintf(cDispBuf, "金额:%11s", cBuffer);
		AppUtils_DisplayLine(cDispBuf, LINE3);
		
		bKey = AppUtils_WaitKey(30);
		
		if(bKey == bESC || bKey == 0)
			return BANK_FAIL;
		else if(bKey == bENTER)
			break;
	}
	
	AppUtils_DisplayCenter("外卡汇总查询", LINE1, TRUE);
	
	while(1)
	{
		sprintf(cDispBuf, "借记笔数:%7d", pTotalsData->FgnTotalSaleCnt);
		AppUtils_DisplayLine(cDispBuf, LINE2);
		
		memset(cBuffer, 0, sizeof(cBuffer));
		AppUtils_AmountAsc2String(pTotalsData->FgnTotalSaleAmt, "", '+', cBuffer);
		sprintf(cDispBuf, "金额:%11s", cBuffer);
		AppUtils_DisplayLine(cDispBuf, LINE3);
		
		bKey = AppUtils_WaitKey(30);
		
		if(bKey == bESC || bKey == 0)
			return BANK_FAIL;
		else if(bKey == bENTER)
			break;
	}
	
	while(1)
	{
		sprintf(cDispBuf, "贷记笔数:%7d", pTotalsData->FgnTotalVoidCnt);
		AppUtils_DisplayLine(cDispBuf, LINE2);
		
		memset(cBuffer, 0, sizeof(cBuffer));
		AppUtils_AmountAsc2String(pTotalsData->FgnTotalVoidAmt, "", '+', cBuffer);
		sprintf(cDispBuf, "金额:%11s", cBuffer);
		AppUtils_DisplayLine(cDispBuf, LINE3);
		
		bKey = AppUtils_WaitKey(30);
		
		if(bKey == bESC || bKey == 0)
			return BANK_FAIL;
		else if(bKey == bENTER)
			break;
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayECashTxnLogDetail
 * DESCRIPTION:   显示电子现金交易交易日志
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayECashTxnLogDetail(int iRecordOffset, char *pRecordData, char *pLogFormat, int iLogFormatLen)
{
	int iLen;
	int iPage;
	char *pData;
	char *pLogFormatOffset;
	char cBuffer[ 56 ];
	struct _TxnLogRec
	{
		char cDate[ 6+1 ];
		char cTime[ 6+1 ];
		char cAmount1[ 12+1 ];
		char cAmount2[ 12+1 ];
		char cCntCode[ 4+1 ];
		char cCurrCode[ 4+1 ];
		char cCustName[ 40+1 ];
		char cTxnType[ 2+1 ];
		char cATC[ 4+1];
	} stTxnLogRec;
	
	// 9A03 9F2103 9F0206 9F0306 9F1A02 5F2A02 9F4E14 9C01 9F3602
	pLogFormatOffset = pLogFormat;
	
	// 121120180017000000020000000000000000015601560000000000000000000000000000000000000000230121
	pData = pRecordData;
	memset(&stTxnLogRec, 0, sizeof(stTxnLogRec));
	
	while(TRUE)
	{
		if(pLogFormatOffset - pLogFormat >= iLogFormatLen)
			break;
			
		if(!memcmp(pLogFormatOffset, "\x9A", 1))
		{
			iLen = *(pLogFormatOffset + 1);
			
			AppUtils_Bcd2Asc((byte *)pData, (byte *)stTxnLogRec.cDate, 6);
			
			pData += iLen;
			pLogFormatOffset += 2;
		}
		else if(!memcmp(pLogFormatOffset, "\x9F\x21", 2))
		{
			iLen = *(pLogFormatOffset + 2);
			
			AppUtils_Bcd2Asc((byte *)pData, (byte *)stTxnLogRec.cTime, 6);
			
			pData += iLen;
			pLogFormatOffset += 3;
		}
		else if(!memcmp(pLogFormatOffset, "\x9F\x02", 2))
		{
			iLen = *(pLogFormatOffset + 2);
			
			AppUtils_Bcd2Asc((byte *)pData, (byte *)stTxnLogRec.cAmount1, 12);
			
			pData += iLen;
			pLogFormatOffset += 3;
			
		}
		else if(!memcmp(pLogFormatOffset, "\x9F\x03", 2))
		{
			iLen = *(pLogFormatOffset + 2);
			
			AppUtils_Bcd2Asc((byte *)pData, (byte *)stTxnLogRec.cAmount2, 12);
			
			pData += iLen;
			pLogFormatOffset += 3;
		}
		else if(!memcmp(pLogFormatOffset, "\x9F\x1A", 2))
		{
			iLen = *(pLogFormatOffset + 2);
			
			AppUtils_Bcd2Asc((byte *)pData, (byte *)stTxnLogRec.cCntCode, 4);
			
			pData += *(pLogFormatOffset + 2);
			pLogFormatOffset += 3;
			
		}
		else if(!memcmp(pLogFormatOffset, "\x5F\x2A", 2))
		{
			iLen = *(pLogFormatOffset + 2);
			
			AppUtils_Bcd2Asc((byte *)pData, (byte *)stTxnLogRec.cCurrCode, 4);
			
			pData += iLen;
			pLogFormatOffset += 3;
			
		}
		else if(!memcmp(pLogFormatOffset, "\x9F\x4E", 2))
		{
			iLen = *(pLogFormatOffset + 2);
			
			if(iLen > 40)
				memcpy(stTxnLogRec.cCustName, pData, 40);
			else
				memcpy(stTxnLogRec.cCustName, pData, iLen);
				
			pData += iLen;
			pLogFormatOffset += 3;
		}
		else if(!memcmp(pLogFormatOffset, "\x9C", 1))
		{
			iLen = *(pLogFormatOffset + 1);
			
			AppUtils_Bcd2Asc((byte *)pData, (byte *)stTxnLogRec.cTxnType, 2);
			
			pData += iLen;
			pLogFormatOffset += 2;
			
		}
		else if(!memcmp(pLogFormatOffset, "\x9F\x36", 1))
		{
			iLen = *(pLogFormatOffset + 2);
			
			AppUtils_Bcd2Asc((byte *)pData, (byte *)stTxnLogRec.cATC, 4);
			
			pData += iLen;
			pLogFormatOffset += 3;
			
		}
	}
	
	iPage = 1;
	
	while(TRUE)
	{
		AppUtils_UserClear(LINE1);
		
		if(iPage == 1)
		{
			sprintf(cBuffer, "明细记录(%d)", iRecordOffset);
			AppUtils_DisplayCenter(cBuffer, LINE1, TRUE);
			
			sprintf(cBuffer, "日期:20%s", stTxnLogRec.cDate);
			AppUtils_DisplayLine(cBuffer, LINE2);
			
			sprintf(cBuffer, "时间:%s", stTxnLogRec.cTime);
			AppUtils_DisplayLine(cBuffer, LINE3);
			
			sprintf(cBuffer, "交易类型:%s", stTxnLogRec.cTxnType);
			AppUtils_DisplayLine(cBuffer, LINE4);
		}
		else if(iPage == 2)
		{
			AppUtils_DisplayLine("授权金额:", LINE1);
			
			memset(cBuffer, 0, sizeof(cBuffer));
			AppUtils_AmountAsc2String((byte *)stTxnLogRec.cAmount1, "", '+', cBuffer);
			AppUtils_DisplayLine(cBuffer, LINE2);
			
			AppUtils_DisplayLine("其他金额:", LINE3);
			
			memset(cBuffer, 0, sizeof(cBuffer));
			AppUtils_AmountAsc2String((byte *)stTxnLogRec.cAmount2, "", '+', cBuffer);
			AppUtils_DisplayLine(cBuffer, LINE4);
		}
		else if(iPage == 3)
		{
			sprintf(cBuffer, "国家代码:%s", stTxnLogRec.cCntCode);
			AppUtils_DisplayLine(cBuffer, LINE1);
			
			sprintf(cBuffer, "货币代码:%s", stTxnLogRec.cCurrCode);
			AppUtils_DisplayLine(cBuffer, LINE2);
			
			sprintf(cBuffer, "交易计数器:%s", stTxnLogRec.cATC);
			AppUtils_DisplayLine(cBuffer, LINE3);
		}
		else if(iPage == 4)
		{
			AppUtils_DisplayLine("商户名称:", LINE1);
			AppUtils_DisplayLine(stTxnLogRec.cCustName, LINE2);
		}
		
		switch(AppUtils_WaitKey(30))
		{
		case bESC:
		case 0:
		

			return BANK_FAIL;
			
		case bBKSP:
			return(BANK_PRE);
			
		case bENTER:
			return(BANK_NEXT);
			
		case bKEYC:
			iPage --;
			
			if(iPage <= 0)
				iPage = 4;
				
			break;
			
		case bKEYD:
			iPage ++;
			
			if(iPage > 4)
				iPage = 1;
				
			break;
			
		default:
			break;
		}
	}
}


/* -----------------------------------------------------------------------------
 * print Function declare
 ---------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintString
 * DESCRIPTION:   打印字符串，通过传入可选是否换行
 * PARAMETERS:    pString: 字符串缓冲区
 *                bAttribute: 打印的字体属性(3/2)
 *                bLineFeedFlag: 是否换行(0:不换行  1:换行)
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_PrintString(char *pString, byte bAttribute, byte bLineFeedFlag)
{
    zzprinter::getInstance()->Qt_Print_Str((char *)pString, 1,0,0);

	return;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintStringCenterAlign
 * DESCRIPTION:   居中打印字符串(自动换行)
 * PARAMETERS:    pString: 字符串缓冲区
 *                bAttribute: 打印的字体属性(3/2)
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_PrintStringCenterAlign(char *pString, byte bAttribute)
{
	int iRowMax = PAPER_MAX_COLUMN;
	int iLength;
	char cTmpBuf[ 56 ];

		
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	memset(cTmpBuf, ' ', iRowMax - 1);
	
	iLength = strlen(pString);
	
	if(iLength > iRowMax)
		iLength = iRowMax;
		
	memcpy(cTmpBuf + (iRowMax - iLength) / 2, pString, iLength);

    PrnDisp_PrintString(cTmpBuf, 1, TRUE);

	return;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintStringRightAlign
 * DESCRIPTION:   右对齐打印字符串(自动换行)
 * PARAMETERS:    pString: 字符串缓冲区
 *                bAttribute: 打印的字体属性(3/2)
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_PrintStringRightAlign(char *pString, byte bAttribute)
{	

    PrnDisp_PrintString(pString, 1, TRUE);

	return;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintStringLeftRightAlign
 * DESCRIPTION:   左右对齐(两端对齐)打印字符串(自动换行)
 * PARAMETERS:    pLeftString: 左侧字符串缓冲区
 *                bAttribute: 左侧打印的字体属性(3/2)
 *                pRightString: 右侧字符串缓冲区
 *                bRightAttrib: 右侧打印的字体属性(3/2)
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_PrintStringLeftRightAlign(char *pLeftString, byte bLeftAttrib, char *pRightString, byte bRightAttrib)
{
	int iLRowMax, iRRowMax;
	int iLLength, iRLength;
	char cTmpBuf[ 56 ];
	
	iLLength = strlen(pLeftString);
	iRLength = strlen(pRightString);

		
	if(iRRowMax > PAPER_MAX_COLUMN)
		iRRowMax = PAPER_MAX_COLUMN;
		
	iLRowMax = PAPER_MAX_COLUMN - iRRowMax;

	// left
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if(iLLength < iLRowMax)
	{
		memcpy(cTmpBuf, pLeftString, iLLength);
		memset(cTmpBuf + iLLength, ' ', iLRowMax - iLLength);
	}
	else
		memcpy(cTmpBuf, pLeftString, iLRowMax);
		

	
	// right
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
	if(iRLength > iRRowMax)
		iRLength = iRRowMax;
		
	memcpy(cTmpBuf, pRightString, iRLength);

#ifdef MAKE_LINUX
    PrnDisp_PrintString("\n", 1, TRUE);
#else
    PrnDisp_PrintString("\n", 1, TRUE);
#endif

	return;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintLineFeed
 * DESCRIPTION:   换行
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_PrintLineFeed(byte bAttribute)
{
    PrnDisp_PrintString("\n", 1, TRUE);

	return;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintFormFeed
 * DESCRIPTION:   走纸以便于撕纸
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_PrintFormFeed(void)
{
    zzprinter::getInstance()->Qt_Print_Str((char *)"\n");
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_SepLine
 DESCRIPTION:   打印分隔线
 PARAMETERS:    void
 RETURN:        None
 NOTE:
 *-------------------------------------------------------------------------*/
void PrnDisp_SepLine(void)
{
	char cPrnBuf[128];
	
	memset(cPrnBuf, 0, sizeof(cPrnBuf));
	
#ifdef MAKE_LINUX
	
	if(Print_GetFontSize() == FONT_CHN24x24)
		memset(cPrnBuf, '-', PAPER_MAX_COLUMN / 2);
	else
		memset(cPrnBuf, '-', PAPER_MAX_COLUMN);
		
#else
	memset(cPrnBuf, '-', PAPER_MAX_COLUMN);
#endif
    PrnDisp_PrintString(cPrnBuf, 2, FALSE);
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_UnderLine
 DESCRIPTION:   打印下划线
 PARAMETERS:    void
 RETURN:        None
 NOTE:
 *-------------------------------------------------------------------------*/
void PrnDisp_UnderLine(void)
{
	char cPrnBuf[128];
	
	memset(cPrnBuf, 0, sizeof(cPrnBuf));
	
#ifdef MAKE_LINUX
	
	if(Print_GetFontSize() == FONT_CHN24x24)
		memset(cPrnBuf, '_', PAPER_MAX_COLUMN / 2);
	else
		memset(cPrnBuf, '_', PAPER_MAX_COLUMN);
		
#else
	memset(cPrnBuf, '_', PAPER_MAX_COLUMN);
#endif

    PrnDisp_PrintString(cPrnBuf, 2, FALSE);
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_Trail
 DESCRIPTION:   打印结束线(撕纸线)
 PARAMETERS:    void
 RETURN:        None
 NOTE:
 *-------------------------------------------------------------------------*/
void PrnDisp_Trail(void)
{
	char cPrnBuf[128];
	
	memset(cPrnBuf, 0, sizeof(cPrnBuf));
	
#ifdef MAKE_LINUX
	
	if(Print_GetFontSize() == FONT_CHN24x24)
		strcpy(cPrnBuf, "-------X-------X-------X");
	else
		strcpy(cPrnBuf, "---------------X---------------X---------------X");
		
#else
	strcpy(cPrnBuf, "X------------X--------------X------------X");
#endif
		
    PrnDisp_PrintString(cPrnBuf, 1, TRUE);
	PrnDisp_PrintFormFeed();
    PrnDisp_PrintLineFeed(1);
    PrnDisp_PrintLineFeed(1);
    PrnDisp_PrintLineFeed(1);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintTxn
 * DESCRIPTION:   交易打印
 * PARAMETERS:    pTxnData: 交易数据指针
 *                bReprintFlag: 重打印标志
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_PrintTxn(DynData *pTxnData, byte bReprintFlag)
{
	int i, iReceiptNum;
	char cTmpBuf[ 256 ], cBuffer[128];
	TransRecord *pTranRec;
	char cSignSymbol;
	byte bPrnAttrib, bTmpAttrib;
	
    bPrnAttrib = 1;
	
	pTranRec = &pTxnData->TransDB;
	
	if(memcmp(pTranRec->RspCode, "00", 2) != 0)
		return BANK_FAIL;

    gTermCfg.mFixPrm.Receipt = 1;
	iReceiptNum = gTermCfg.mFixPrm.Receipt;
	
	if(iReceiptNum > 3 || iReceiptNum < 1)
	{
		iReceiptNum = 2;
	}
	
	AppUtils_OpenPrinter();
	
	for(i = 0; i < iReceiptNum; i++)
	{
		AppUtils_UserClear(LINE2);
		
		if(bReprintFlag == 0)
			AppUtils_DisplayCenter("交易成功", LINE2, FALSE);
			
		AppUtils_DisplayCenter("正在打印", LINE3, FALSE);
		
		// CUP方式
		//签购单抬头
        sprintf(cTmpBuf, "%s签购单", gTermCfg.mFixPrm.ReceiptTitle);

        if(strlen(cTmpBuf) > 24)
            PrnDisp_PrintStringCenterAlign(cTmpBuf, 2);
        else
            PrnDisp_PrintStringCenterAlign(cTmpBuf, 3 | 2);

		if(gTermCfg.mTrSwitch.UserDefinedReceiptTitle)
		{
			//签购单抬头
            PrnDisp_PrintStringCenterAlign((char*)gTermCfg.mFixPrm.ReceiptTitle, 2 | 3);
			
			PrnDisp_SepLine();
		}

		
#ifdef MAKE_LINUX
		
		if(gTermCfg.mFixPrm.ReceiptPrintFont == 0)  //小字体
		{
            bPrnAttrib = 1;
			Print_SetFontSize(FONT_CHN16x16);
			Print_SetLineHeight(16);
		}
		else if(gTermCfg.mFixPrm.ReceiptPrintFont == 1) //中字体
		{
            bPrnAttrib = 1;
			Print_SetFontSize(FONT_CHN24x24);
			Print_SetLineHeight(24);
		}
		else if(gTermCfg.mFixPrm.ReceiptPrintFont == 2) //大字体
		{
            bPrnAttrib = 2;
			Print_SetFontSize(FONT_CHN16x16);
			Print_SetLineHeight(16);
		}
		
#endif
		
		//------
		if(gTermCfg.mTrSwitch.EnglishSlip)
			PrnDisp_PrintString("商户名称(MERCHANT NAME):", bPrnAttrib, TRUE);
		else
			PrnDisp_PrintString("商户名称:", bPrnAttrib, TRUE);
			
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%s", gTermCfg.mFixPrm.MerchantNameChn);
        PrnDisp_PrintString(cTmpBuf, 2, FALSE);
		
		if(strlen((char *)gTermCfg.mFixPrm.MerchantNameEng) > 0)
		{
			sprintf(cTmpBuf, "/%s", gTermCfg.mFixPrm.MerchantNameEng);
            PrnDisp_PrintString(cTmpBuf, 2, FALSE);
		}
		
        PrnDisp_PrintLineFeed(2);
		
		//------
		if(gTermCfg.mTrSwitch.EnglishSlip)
		{
			PrnDisp_PrintString("商户编号(MERCHANT NO): ", bPrnAttrib, FALSE);
			
			if(gTermCfg.mFixPrm.ReceiptPrintFont == 1)
				PrnDisp_PrintLineFeed(bPrnAttrib);
		}
		else
			PrnDisp_PrintString("商户编号: ", bPrnAttrib, FALSE);
			
		PrnDisp_PrintString((char *)gTermCfg.mFixPrm.MerchantID, bPrnAttrib, TRUE);
		
		//------
		if(gTermCfg.mTrSwitch.EnglishSlip)
		{
			PrnDisp_PrintString("终端编号(TERMINAL NO): ", bPrnAttrib, FALSE);
			
			if(gTermCfg.mFixPrm.ReceiptPrintFont == 1)
				PrnDisp_PrintLineFeed(bPrnAttrib);
		}
		else
			PrnDisp_PrintString("终端编号: ", bPrnAttrib, FALSE);
			
		PrnDisp_PrintString((char *)gTermCfg.mFixPrm.TerminalID, bPrnAttrib, TRUE);
		
		//------
		if(gTermCfg.mTrSwitch.EnglishSlip)
			PrnDisp_PrintString("操作员号(OPERATOR NO): ", bPrnAttrib, FALSE);
		else
			PrnDisp_PrintString("操作员号: ", bPrnAttrib, FALSE);
			
		PrnDisp_PrintString((char *)gTermCfg.mDynPrm.OperID, bPrnAttrib, TRUE);
		
		//------
		if(gTermCfg.mTrSwitch.EnglishSlip)
			PrnDisp_PrintString("发卡行(ISSUER):", bPrnAttrib, FALSE);
		else
			PrnDisp_PrintString("发卡行: ", bPrnAttrib, FALSE);
			
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		if(gTermCfg.mTrSwitch.IssueChnName)
		{
			if(PrnDisp_GetBankName(pTranRec->Issuer, cTmpBuf) == BANK_OK)
				PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
			else if(strlen((char*)gTermCfg.mFixPrm.UnknownIssuerName) > 0)
				PrnDisp_PrintString((char*)gTermCfg.mFixPrm.UnknownIssuerName, bPrnAttrib, TRUE);
			else
				PrnDisp_PrintString((char*)pTranRec->Issuer, bPrnAttrib, TRUE);
		}
		else
		{
			memcpy(cTmpBuf, pTranRec->Issuer, strlen((char *)pTranRec->Issuer));
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
		}
		
		//------
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		if(gTermCfg.mTrSwitch.EnglishSlip)
			PrnDisp_PrintString("收单行(ACQUIRER):", bPrnAttrib, FALSE);
		else
			PrnDisp_PrintString("收单行: ", bPrnAttrib, FALSE);
			
		if(gTermCfg.mTrSwitch.AccquChnName && PrnDisp_GetBankName(pTranRec->Acquirer, cTmpBuf) == BANK_OK)
		{
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
		}
		else
		{
			memcpy(cTmpBuf, pTranRec->Acquirer, strlen((char *)pTranRec->Acquirer));
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
		}
		
		//------
		//卡号
		if(pTranRec->TrType == BTYPE_ECASH_OUT_RELOAD)
		{
			if(gTermCfg.mTrSwitch.EnglishSlip)
				PrnDisp_PrintString("转出卡号(CARD NUMBER): ", bPrnAttrib, TRUE);
			else
				PrnDisp_PrintString("转出卡号: ", bPrnAttrib, TRUE);
		}
		else
		{
			if(gTermCfg.mTrSwitch.EnglishSlip)
				PrnDisp_PrintString("卡号(CARD NUMBER): ", bPrnAttrib, TRUE);
			else
				PrnDisp_PrintString("卡号: ", bPrnAttrib, TRUE);
		}
		
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_ConvertPANbyMask(pTxnData, 1, cTmpBuf);
		
		if(pTranRec->Swipe == TRAN_ICC)
			strcat(cTmpBuf, "/I");
		else if(pTranRec->Swipe == TRAN_QPBOC)
			strcat(cTmpBuf, "/C");
		else if(pTranRec->Swipe == TRAN_MCHIP)
			strcat(cTmpBuf, "/C");
		else if(pTranRec->Swipe == TRAN_NUM)
			strcat(cTmpBuf, "/M");
		else if(pTranRec->Swipe == TRAN_NOCARD)
			strcat(cTmpBuf, "/N");
		else if(AppUtils_CheckStatus(pTranRec->Status[ 0 ], STATUS_FALLBACK) == BANK_OK)
			strcat(cTmpBuf, "/F");
		else
			strcat(cTmpBuf, "/S");
			
		if(gTermCfg.mFixPrm.ReceiptPrintFont == 0)
            bTmpAttrib = 2 | 3;
		else if(gTermCfg.mFixPrm.ReceiptPrintFont == 1)
            bTmpAttrib = 2;
		else
            bTmpAttrib = 2 | 3;
			
		PrnDisp_PrintString(cTmpBuf, bTmpAttrib, TRUE);
		
		//------
		//有效期
		if(AppUtils_IsNullData((char *)pTranRec->ExpDate, 3) != BANK_OK)
		{
			memset(cBuffer, 0, sizeof(cBuffer));
			sprintf(cBuffer, "%02X%02X/%02X", pTranRec->ExpDate[0], pTranRec->ExpDate[1], pTranRec->ExpDate[2]);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			
			if(gTermCfg.mTrSwitch.EnglishSlip)
				sprintf(cTmpBuf, "有效期(EXP.DATE): %s", cBuffer);
			else
				sprintf(cTmpBuf, "有效期: %s", cBuffer);
				
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
		}
		
		//------
		//交易类型
		if(gTermCfg.mTrSwitch.EnglishSlip)
			PrnDisp_PrintString("交易类型(TXN TYPE): ", bPrnAttrib, TRUE);
		else
			PrnDisp_PrintString("交易类型: ", bPrnAttrib, TRUE);
			
		//------
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		
		switch(pTranRec->TrType)
		{
		case BTYPE_SALE:
		
			if((pTranRec->EC_flag == 1) ||
			   (pTranRec->Swipe == TRAN_QPBOC && pTranRec->qPBOC_OnlineFlag == 0))
			{
				strcat(cTmpBuf, "电子现金消费(EC SALE)");
			}
			else
			{
				strcat(cTmpBuf, "消费(SALE)");
			}
			
			break;
		case BTYPE_ORDER_SALE:
			strcat(cTmpBuf, "订购消费(MOTO SALE)");
			break;
		case BTYPE_PREAUTH:
			strcat(cTmpBuf, "预授权(AUTH)");
			break;
		case BTYPE_COMPLETE:
			strcat(cTmpBuf, "预授权完成请求(AUTH COMPLETE)");
			break;
		case BTYPE_REFUND:
			strcat(cTmpBuf, "退货(REFUND)");
			break;
		case BTYPE_ECASH_REFUND:
			strcat(cTmpBuf, "电子现金脱机退货(EC REFUND)");
			break;
		case BTYPE_SALE_VOID:
			strcat(cTmpBuf, "消费撤销(VOID)");
			break;
		case BTYPE_PREAUTH_VOID:
			strcat(cTmpBuf, "预授权撤销(PREAUTH CANCEL)");
			break;
		case BTYPE_COMPLETE_VOID:
			strcat(cTmpBuf, "预授权完成撤销(COMPLETE VOID)");
			break;
		case BTYPE_OFFLINE_SETTLE:
			strcat(cTmpBuf, "离线结算(OFFLINE)");
			break;
		case BTYPE_OFFLINE_ADJUST:
			strcat(cTmpBuf, "结算调整(ADJUST)");
			break;
		case BTYPE_OFFLINE_COMPLETE:
			strcat(cTmpBuf, "预授权完成通知(AUTH SETTLEMENT)");
			break;
		case BTYPE_ECASH_IN_RELOAD:
			strcat(cTmpBuf, "指定帐户圈存(IN LOAD)");
			break;
		case BTYPE_ECASH_OUT_RELOAD:
			strcat(cTmpBuf, "非指定帐户圈存(OUT LOAD)");
			break;
		case BTYPE_ECASH_CASH_RELOAD:
			strcat(cTmpBuf, "现金充值(CASH LOAD)");
			break;
		case BTYPE_INSTALLMENT:
			strcat(cTmpBuf, "分期付款(INSTALLMENT)");
			break;
		case BTYPE_INSTALLMENT_VOID:
			strcat(cTmpBuf, "分期付款撤销(INSTALLMENT VOID)");
			break;
		default:
			Trans_GetTransName(pTranRec->TrType, cTmpBuf);
			break;
		}
		
		if(gTermCfg.mTrSwitch.EnglishSlip == 0)
		{
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			Trans_GetTransName(pTranRec->TrType, cTmpBuf);
			
			if((pTranRec->EC_flag == 1) ||
			   (pTranRec->Swipe == TRAN_QPBOC && pTranRec->qPBOC_OnlineFlag == 0))
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				strcat(cTmpBuf, "电子现金消费");
			}
		}
		
        PrnDisp_PrintString(cTmpBuf, 2, FALSE);
        PrnDisp_PrintLineFeed(2);
		
		// ------
		if(gTermCfg.mTrSwitch.EnglishSlip)
		{
			PrnDisp_PrintString("批次号(BATCH NO):", bPrnAttrib, FALSE);
			sprintf(cTmpBuf, "%06lu", pTranRec->BatchNum);
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
			
			PrnDisp_PrintString("凭证号(VOUCHER NO):", bPrnAttrib, FALSE);
			sprintf(cTmpBuf, "%06lu", pTranRec->SysTrace);
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
			
			if(AppUtils_IsNullData((char *)pTranRec->AuthCode, sizeof(pTranRec->AuthCode)) != BANK_OK)
			{
				PrnDisp_PrintString("授权号(AUTH NO):", bPrnAttrib, FALSE);
				PrnDisp_PrintString((char *)pTranRec->AuthCode, bPrnAttrib, TRUE);
			}
			
			if(AppUtils_IsNullData((char *)pTranRec->RefCode, sizeof(pTranRec->RefCode)) != BANK_OK)
			{
				PrnDisp_PrintString("参考号(REF.NO):", bPrnAttrib, FALSE);
				PrnDisp_PrintString((char *)pTranRec->RefCode, bPrnAttrib, TRUE);
			}
		}
		else
		{
			if(gTermCfg.mFixPrm.ReceiptPrintFont == 0)
			{
				sprintf(cTmpBuf, "批次号:%06lu 参考号:%s", pTranRec->BatchNum, pTranRec->RefCode);
				PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
			}
			else
			{
				sprintf(cTmpBuf, "批次号:%06lu\n参考号:%s", pTranRec->BatchNum, pTranRec->RefCode);
                PrnDisp_PrintString(cTmpBuf, 2, FALSE);
                PrnDisp_PrintLineFeed(2);
			}
			
			sprintf(cTmpBuf, "凭证号:%06lu 授权号:%6s", pTranRec->SysTrace, pTranRec->AuthCode);
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
		}
		
		// ------
		if(memcmp(pTranRec->CardTypeName, "CUP", 3) && memcmp(pTranRec->CardTypeName, "000", 3))
		{
			memset(cBuffer, 0, sizeof(cBuffer));
			memcpy(cBuffer, pTranRec->CardTypeName, 3);
			sprintf(cTmpBuf, "国际卡组织: %s", cBuffer);
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
		}
		
		// ------
		if(gTermCfg.mTrSwitch.EnglishSlip)
			PrnDisp_PrintString("日期/时间(DATE/TIME):", bPrnAttrib, TRUE);
		else
			PrnDisp_PrintString("日期/时间:", bPrnAttrib, FALSE);
			
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_ConvertTransDateTime(pTxnData, cTmpBuf);
		PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
		
		//------
		//交易金额
		
		if(gTermCfg.mTrSwitch.NegativeSignFlag)
			cSignSymbol = inPrnDisp_GetAmountSignSymbol(pTranRec->TrType);
		else
			cSignSymbol = '+';
			
		if(gTermCfg.mTrSwitch.EnglishSlip)
		{
			if(AppUtils_IsNullData((char *)pTranRec->Fee, sizeof(pTranRec->Fee)) != BANK_OK)
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				AppUtils_AmountAsc2String(pTranRec->OrgAmount, PRNCURR_NAME, '+', cTmpBuf);
                PrnDisp_PrintStringLeftRightAlign("交易金额(AMOUNT):", bPrnAttrib, cTmpBuf, 3 | 2);
				
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				AppUtils_AmountAsc2String(pTranRec->Fee, PRNCURR_NAME, '+', cTmpBuf);
                PrnDisp_PrintStringLeftRightAlign("小费(TIP):", bPrnAttrib, cTmpBuf, 3 | 2);
				
				PrnDisp_PrintString("总金额(TOTAL):", bPrnAttrib, TRUE);
			}
			else
			{
				PrnDisp_PrintString("金额(AMOUNT):", bPrnAttrib, TRUE);
			}
		}
		else
		{
			if(AppUtils_IsNullData((char *)pTranRec->Fee, sizeof(pTranRec->Fee)) != BANK_OK)
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				AppUtils_AmountAsc2String(pTranRec->OrgAmount, PRNCURR_NAME, '+', cTmpBuf);
                PrnDisp_PrintStringLeftRightAlign("交易金额:", bPrnAttrib, cTmpBuf, 3 | 2);
				
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				AppUtils_AmountAsc2String(pTranRec->Fee, PRNCURR_NAME, '+', cTmpBuf);
                PrnDisp_PrintStringLeftRightAlign("小费:", bPrnAttrib, cTmpBuf, 3 | 2);
				
				PrnDisp_PrintString("总金额:", bPrnAttrib, TRUE);
			}
			else
			{
				PrnDisp_PrintString("金额:", bPrnAttrib, TRUE);
			}
		}
		
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_AmountAsc2String(pTranRec->Amount, PRNCURR_NAME, cSignSymbol, cTmpBuf);
        PrnDisp_PrintString(cTmpBuf, 3 | 2, TRUE);
		
		if(gTermCfg.mTrSwitch.EnglishSlip)
			PrnDisp_PrintString("备注(REFERENCE):", bPrnAttrib, TRUE);
		else
			PrnDisp_PrintString("备注:", bPrnAttrib, TRUE);
			
		// 接触式电子现金、非接触式电子现金(无论脱机还是联机)都需要打印余额
		if(pTranRec->EC_flag == 1 || pTranRec->Swipe == TRAN_QPBOC || pTranRec->Swipe == TRAN_CTLSPBOC)
		{
			if(pTranRec->TrType != BTYPE_ECASH_REFUND)
			{
				memset(cBuffer, 0, sizeof(cBuffer));
				AppUtils_AmountAsc2String(pTranRec->EC_Balance, PRNCURR_NAME, '+', cBuffer);
				sprintf(cTmpBuf, "电子现金余额: %s", cBuffer);
				PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
			}
		}
		
		if(pTranRec->TrType != BTYPE_OFFLINE_ADJUST)
		{
			if(AppUtils_IsNullData((char *)pTranRec->HostNote1, sizeof(pTranRec->HostNote1)) != BANK_OK)
				PrnDisp_PrintString((char *)pTranRec->HostNote1, bPrnAttrib, TRUE);
				
			if(AppUtils_IsNullData((char *)pTranRec->HostNote2, sizeof(pTranRec->HostNote2)) != BANK_OK)
				PrnDisp_PrintString((char *)pTranRec->HostNote2, bPrnAttrib, TRUE);
				
			if(AppUtils_IsNullData((char *)pTranRec->HostNote3, sizeof(pTranRec->HostNote3)) != BANK_OK)
				PrnDisp_PrintString((char *)pTranRec->HostNote3, bPrnAttrib, TRUE);
		}
		
		//备注信息根据交易类型判断
		if(pTranRec->TrType == BTYPE_SALE_VOID
		   || pTranRec->TrType == BTYPE_ECASH_REFUND
		   || pTranRec->TrType == BTYPE_COMPLETE_VOID
		   || pTranRec->TrType == BTYPE_INSTALLMENT_VOID
		   || pTranRec->TrType == BTYPE_ISSUER_SCORE_SALE_VOID
		   || pTranRec->TrType == BTYPE_UNION_SCORE_SALE_VOID
		   || pTranRec->TrType == BTYPE_YUYUE_SALE_VOID
		   || pTranRec->TrType == BTYPE_MCHIP_SALE_VOID
		   || pTranRec->TrType == BTYPE_MCHIP_COMPLETE_VOID
		   || pTranRec->TrType == BTYPE_ORDER_SALE_VOID
		   || pTranRec->TrType == BTYPE_ORDER_COMPLETE_VOID
		   || pTranRec->TrType == BTYPE_ECASH_CASH_RELOAD_VOID
		   || pTranRec->TrType == BTYPE_OFFLINE_ADJUST)
		{
			PrnDisp_PrintString("原凭证号:", bPrnAttrib, FALSE);
			sprintf(cTmpBuf, "%06lu", pTranRec->OrgSysTrace);
            PrnDisp_PrintString(cTmpBuf, 3, TRUE);
		}
		
		if(pTranRec->TrType == BTYPE_ECASH_REFUND)
		{
			PrnDisp_PrintString("原批次号:", bPrnAttrib, FALSE);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%06lu", pTranRec->OrgBatchNum);
            PrnDisp_PrintString(cTmpBuf, 3, TRUE);
			
			PrnDisp_PrintString("原终端号:", bPrnAttrib, FALSE);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%s", (char *)pTranRec->OrgTermID);
            PrnDisp_PrintString(cTmpBuf, 3, TRUE);
		}
		
		if(pTranRec->TrType == BTYPE_REFUND
		   || pTranRec->TrType == BTYPE_UNION_SCORE_REFUND
		   || pTranRec->TrType == BTYPE_YUYUE_REFUND
		   || pTranRec->TrType == BTYPE_MCHIP_REFUND
		   || pTranRec->TrType == BTYPE_ORDER_REFUND)
		{
			if(AppUtils_IsNullData((char *)pTranRec->OrgRefCode, sizeof(pTranRec->OrgRefCode)) != BANK_OK)
			{
				PrnDisp_PrintString("原参考号:", bPrnAttrib, FALSE);
                PrnDisp_PrintString((char *)pTranRec->OrgRefCode, 3, TRUE);
			}
		}
		
		if(pTranRec->TrType == BTYPE_REFUND
		   || pTranRec->TrType == BTYPE_OFFLINE_COMPLETE
		   || pTranRec->TrType == BTYPE_UNION_SCORE_REFUND
		   || pTranRec->TrType == BTYPE_YUYUE_REFUND
		   || pTranRec->TrType == BTYPE_MCHIP_REFUND
		   || pTranRec->TrType == BTYPE_ORDER_REFUND
		   || pTranRec->TrType == BTYPE_ECASH_REFUND)
		{
			if(AppUtils_IsNullData((char *)pTranRec->OrgTranDate, sizeof(pTranRec->OrgTranDate)) != BANK_OK)
			{
				PrnDisp_PrintString("原交易日期:", bPrnAttrib, FALSE);
                PrnDisp_PrintString((char *)pTranRec->OrgTranDate, 3, TRUE);
			}
		}
		
		if(pTranRec->TrType == BTYPE_COMPLETE
		   || pTranRec->TrType == BTYPE_OFFLINE_COMPLETE
		   || pTranRec->TrType == BTYPE_COMPLETE_VOID
		   || pTranRec->TrType == BTYPE_PREAUTH_VOID
		   || pTranRec->TrType == BTYPE_MCHIP_COMPLETE
		   || pTranRec->TrType == BTYPE_MCHIP_COMPLETE_VOID
		   || pTranRec->TrType == BTYPE_MCHIP_PREAUTH_VOID
		   || pTranRec->TrType == BTYPE_ORDER_COMPLETE
		   || pTranRec->TrType == BTYPE_ORDER_COMPLETE_VOID
		   || pTranRec->TrType == BTYPE_ORDER_PREAUTH_VOID)
		{
			if(AppUtils_IsNullData((char *)pTranRec->OrgAuthCode, sizeof(pTranRec->OrgAuthCode)) != BANK_OK)
			{
				PrnDisp_PrintString("原预授权码/AUTH:", bPrnAttrib, FALSE);
                PrnDisp_PrintString((char *)pTranRec->OrgAuthCode, 3, TRUE);
			}
		}
		
#ifdef MAKE_ADDTRANS
		
		if(pTranRec->TrType == BTYPE_INSTALLMENT
		   || pTranRec->TrType == BTYPE_INSTALLMENT_VOID)
		{
			sprintf(cBuffer, "分期数: %u", pTranRec->InstallmentPeriod);
			PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			
			//GetCurencyStr(pTranRec->InstallmentCurrency, temp);
			//sprintf(print_buf, "还款币种: %s(%s)\n", pTranRec->InstallmentCurrency, temp);
			
			sprintf(cBuffer, "还款币种: 156(RMB)");
			PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_AmountAsc2String((byte *)pTranRec->InstallmentFirstAmt, "", '+', cTmpBuf);
			sprintf(cBuffer, "首付还款金额: %s", cTmpBuf);
			PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			
			if(pTranRec->InstallmentFeePayway == 0)
			{
				//一次性支付手续费
				PrnDisp_PrintString("手续费支付方式: 一次性支付", bPrnAttrib, TRUE);
				
				if(memcmp(pTranRec->InstallmentHolderFee, "            ", 12) == 0)
				{
					PrnDisp_PrintString("持卡人手续费: ", bPrnAttrib, TRUE);
				}
				else
				{
					memset(cTmpBuf, 0, sizeof(cTmpBuf));
					AppUtils_AmountAsc2String((byte *)pTranRec->InstallmentHolderFee, "", '+', cTmpBuf);
					sprintf(cBuffer, "持卡人手续费: %s", cTmpBuf);
					PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
				}
			}
			else
			{
				//分期支付手续费
				PrnDisp_PrintString("手续费支付方式: 分期支付", bPrnAttrib, TRUE);
				
				if(memcmp(pTranRec->InstallmentFirstFee, "            ", 12) == 0 ||
				   AppUtils_IsNullData((char *)pTranRec->InstallmentFirstFee, 12) == BANK_OK)
				{
					PrnDisp_PrintString("首期手续费:", bPrnAttrib, TRUE);
				}
				else
				{
					memset(cTmpBuf, 0, sizeof(cTmpBuf));
					AppUtils_AmountAsc2String((byte *)pTranRec->InstallmentFirstFee, "", '+', cTmpBuf);
					sprintf(cBuffer, "首期手续费: %s", cTmpBuf);
					PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
				}
				
				if(memcmp(pTranRec->InstallmentPerFee, "            ", 12) == 0 ||
				   AppUtils_IsNullData((char *)pTranRec->InstallmentPerFee, 12) == BANK_OK)
				{
					PrnDisp_PrintString((char *)"每期手续费:", bPrnAttrib, TRUE);
				}
				else
				{
					memset(cTmpBuf, 0, sizeof(cTmpBuf));
					AppUtils_AmountAsc2String((byte *)pTranRec->InstallmentPerFee, "", '+', cTmpBuf);
					sprintf(cBuffer, "每期手续费: %s", cTmpBuf);
					PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
				}
			}
			
			AppUtils_AmountAsc2String((byte *)pTranRec->BonusScore, "", '+', cTmpBuf);
			sprintf(cBuffer, "奖励积分: %s", cTmpBuf);
			PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			
			if(strlen((char *)pTranRec->AddPrnInfo) > 0)      //附加信息 按照原样打印
			{
				PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			}
		}
		
		if(pTranRec->TrType == BTYPE_UNION_SCORE_SALE ||
		   pTranRec->TrType == BTYPE_UNION_SCORE_SALE_VOID ||
		   pTranRec->TrType == BTYPE_ISSUER_SCORE_SALE ||
		   pTranRec->TrType == BTYPE_ISSUER_SCORE_SALE_VOID)
		{
			sprintf(cBuffer, "商品代码:  %s", pTranRec->ScoreProductCode);
			PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			
			AppDebug_AppLog("pTranRec->ConvertedScore=%s", pTranRec->ConvertedScore);
			AppDebug_AppLog("pTranRec->ScoreBalance=%s", pTranRec->ScoreBalance);
			AppDebug_AppLog("pTranRec->ScoreSelfPaidAmt=%s", pTranRec->ScoreSelfPaidAmt);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_AmountAsc2String(pTranRec->ConvertedScore, "", '+', cTmpBuf);
			sprintf(cBuffer, "兑换积分数:%s", cTmpBuf);
			PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			
			if(pTranRec->TrType == BTYPE_UNION_SCORE_SALE ||
			   pTranRec->TrType == BTYPE_ISSUER_SCORE_SALE)
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				AppUtils_AmountAsc2String(pTranRec->ScoreBalance, "", '+', cTmpBuf);
				sprintf(cBuffer, "积分余额数:%s", cTmpBuf);
				PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			}
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_AmountAsc2String(pTranRec->ScoreSelfPaidAmt, "", '+', cTmpBuf);
			sprintf(cBuffer, "自付金额: %s", cTmpBuf);
			PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
		}
		
		if(pTranRec->TrType == BTYPE_YUYUE_SALE ||
		   pTranRec->TrType == BTYPE_YUYUE_SALE_VOID)
		{
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			memcpy(cTmpBuf, pTranRec->YuYuePhoneNum, 11);
			memset(cTmpBuf + 3, '*', 5);
			sprintf(cBuffer, "手机号码: %s", cTmpBuf);
			PrnDisp_PrintString(cBuffer, bPrnAttrib, TRUE);
			
			//预约类交易不需要打印预约号码(乃隐私信息)
			//      memset( cTmpBuf, 0, sizeof( cTmpBuf ) );
			//      memcpy( cTmpBuf, pTranRec->YuYueCode, 6);
			//      sprintf( cBuffer, "预约号码: %6s", cTmpBuf);
			//      PrnDisp_PrintString( cBuffer, bPrnAttrib, TRUE );
		}
		
#endif
		
		if(strlen(gTermCfg.mFixPrm.MerchantInfo)) //增加商户个性信息(广告信息)的打印
		{
			PrnDisp_PrintString(gTermCfg.mFixPrm.MerchantInfo, bPrnAttrib, TRUE);
		}
		
		if((pTranRec->TrType == BTYPE_MAG_ACCTLOAD) ||
		   (pTranRec->TrType == BTYPE_ECASH_OUT_RELOAD))
		{
			PrnDisp_PrintString("转入卡号(CARD NUMBER):", bPrnAttrib, TRUE);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			memcpy(cTmpBuf, pTxnData->TransDB.PAN_2, pTxnData->TransDB.PAN_2Len);
			PrnDisp_PrintString(cTmpBuf, bPrnAttrib, TRUE);
		}
		
#ifdef MAKE_LINUX
		Print_SetFontSize(FONT_CHN16x16);
		Print_SetLineHeight(16);
#endif
		
		// 走完整流程的交易  才需要打印IC卡信息
		if(((pTranRec->TrType == BTYPE_SALE) ||
		    (pTranRec->TrType == BTYPE_PREAUTH) ||
		    (pTranRec->TrType == BTYPE_ECASH_CASH_RELOAD) ||
		    (pTranRec->TrType == BTYPE_ECASH_IN_RELOAD) ||
		    (pTranRec->TrType == BTYPE_ECASH_OUT_RELOAD) ||
		    (pTranRec->TrType == BTYPE_ISSUER_SCORE_SALE) ||
		    (pTranRec->TrType == BTYPE_UNION_SCORE_SALE) ||
		    (pTranRec->TrType == BTYPE_INSTALLMENT))
		   && ((pTranRec->Swipe == TRAN_ICC) || (pTranRec->Swipe == TRAN_QPBOC)))
		{
			// 应用标签
            PrnDisp_PrintString("Appl Label:", 1, FALSE);
            PrnDisp_PrintString((char *)pTranRec->AppLabel, 1, TRUE);
			
			// 应用首选名称
            PrnDisp_PrintString("ApplPref Name:", 1, FALSE);
            PrnDisp_PrintString((char *)pTranRec->AppPrefName, 1, TRUE);
			
			// IC卡交易证书TC
			if(AppUtils_CheckStatus(pTxnData->TransDB.Status[ 0 ], STATUS_OFFLINE) == BANK_OK)
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				AppUtils_Bcd2Asc(pTranRec->TC, (byte *)cTmpBuf, 16);
                PrnDisp_PrintString("TC:", 1, FALSE);
                PrnDisp_PrintString(cTmpBuf, 1, TRUE);
			}
			else
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				AppUtils_Bcd2Asc(pTranRec->ARQC, (byte *)cTmpBuf, 16);
                PrnDisp_PrintString("ARQC:", 1, FALSE);
                PrnDisp_PrintString(cTmpBuf, 1, TRUE);
			}
			
			// AID
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(pTranRec->AID, (byte *)cTmpBuf, pTranRec->AIDLength * 2);
            PrnDisp_PrintString("AID:", 1, FALSE);
            PrnDisp_PrintString(cTmpBuf, 1, TRUE);
			
			// TVR / TSI / ATC
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(pTranRec->TVR, (byte *)cTmpBuf, 10);
            PrnDisp_PrintString("TVR:", 1, FALSE);
            PrnDisp_PrintString(cTmpBuf, 1, FALSE);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(pTranRec->TSI, (byte *)cTmpBuf, 4);
            PrnDisp_PrintString(" TSI:", 1, FALSE);
            PrnDisp_PrintString(cTmpBuf, 1, FALSE);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(pTranRec->ATC, (byte *)cTmpBuf, 4);
            PrnDisp_PrintString(" ATC:", 1, FALSE);
            PrnDisp_PrintString(cTmpBuf, 1, TRUE);
			
			// CSN / AIP / CVMR
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "CSN:%03X AIP:%02X%02X CVMR:%02X%02X%02X", pTranRec->SequenceNum,
			        pTranRec->AIP[0], pTranRec->AIP[1],
			        pTranRec->CVMResult[0], pTranRec->CVMResult[1], pTranRec->CVMResult[2]);
            PrnDisp_PrintString((char *)cTmpBuf, 1, TRUE);
			
			// IAD
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(pTranRec->IAD, (byte *)cTmpBuf, pTranRec->IADLength * 2);
			sprintf(cBuffer, "IAD:%s", cTmpBuf);
            PrnDisp_PrintString((char *)cBuffer, 1, TRUE);
			
			// Terminal Capacity
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(pTranRec->TermCAP, (byte *)cTmpBuf, 6);
			sprintf(cBuffer, "Terminal Capacity:%s ", cTmpBuf);
            PrnDisp_PrintString((char *)cBuffer, 1, FALSE);
			
			// UnPredict No
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_Bcd2Asc(pTranRec->RANDOM, (byte *)cTmpBuf, 8);
			sprintf(cBuffer, "UN:%s", cTmpBuf);
            PrnDisp_PrintString((char *)cBuffer, 1, TRUE);
		}
		
		if(bReprintFlag)
		{
			if(gTermCfg.mTrSwitch.EnglishSlip)
                PrnDisp_PrintString("***重印凭证/DUPLICATED***", 1, FALSE);
			else
                PrnDisp_PrintString("********重印凭证********", 1, FALSE);
		}
		
        PrnDisp_PrintLineFeed(1);
		
		if(i < iReceiptNum)
		{
			if(iReceiptNum >= 2 && (i == iReceiptNum - 1))
			{
				//持卡人联不需要打印签名栏
			}
			else
			{
				if(gTermCfg.mTrSwitch.EnglishSlip)
				{
                    PrnDisp_PrintString("持卡人签名(CAR2OLDER SIGNATURE):", 1, TRUE);
				}
				else
				{
                    PrnDisp_PrintString("持卡人签名:", 1, FALSE);
				}
				

				
                PrnDisp_PrintLineFeed(1);
                PrnDisp_PrintLineFeed(1);
				
				PrnDisp_UnderLine();
                PrnDisp_PrintString("本人确认以上交易,同意将其记入本卡帐户", 1, TRUE);
				
				if(gTermCfg.mTrSwitch.EnglishSlip)
				{
                    PrnDisp_PrintString("I ACKNOWLEDGE SATISFACTORY RECEIPT OF RELATIVE", 1, TRUE);
                    PrnDisp_PrintString("GOODS/SERVICES", 1, TRUE);
				}
			}
			
			if((gTermCfg.mFixPrm.HotLinePhoneNum[0] != 0) && (gTermCfg.mFixPrm.HotLinePhoneNum[0] != ' '))
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				
				if(gTermCfg.mTrSwitch.EnglishSlip)
					sprintf(cTmpBuf,  "服务热线(HOT LINE): %s", (char*)gTermCfg.mFixPrm.HotLinePhoneNum);
				else
					sprintf(cTmpBuf,  "服务热线: %s", (char*)gTermCfg.mFixPrm.HotLinePhoneNum);
					
                PrnDisp_PrintString(cTmpBuf, 1, TRUE);
			}
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));

			if(gTermCfg.mTrSwitch.EnglishSlip)
			{
				if(i == 0)
					strcat(cTmpBuf, " 商户存根/MERCHANT COPY");
				else if((iReceiptNum >= 2) && (i == iReceiptNum - 1))
					strcat(cTmpBuf, "持卡人存根/CUSTOMER COPY");
				else
					strcat(cTmpBuf, "     银行存根/BANK COPY");
			}
			else
			{
				if(i == 0)
					strcat(cTmpBuf, "               商户存根");
				else if((iReceiptNum >= 2) && (i == iReceiptNum - 1))
					strcat(cTmpBuf, "             持卡人存根");
				else
					strcat(cTmpBuf, "               银行存根");
			}
			
            PrnDisp_PrintString(cTmpBuf, 1, TRUE);
			PrnDisp_Trail();
			
			Print_CheckPrintFinish();
            zzprinter::getInstance()->Qt_Printer_CutPaper();

			if(i < gTermCfg.mFixPrm.Receipt - 1)
			{

				AppUtils_UserClear(LINE2);

				if(AppUtils_IsBigScreen() == BANK_OK)
                    AppUtils_DisplayCenter("按任意键继续", LINE5, 0);
				else
                    AppUtils_DisplayCenter("按任意键继续", LINE5, 0);
                qDebug("print info");
                AppUtils_get_WaitKey(10);
			}

		}
		else
		{
			PrnDisp_SepLine();
			
			PrnDisp_PrintFormFeed();
			
			Print_CheckPrintFinish();
		}
	}
	
#ifdef MAKE_LINUX
	Print_SetFontSize(FONT_CHN16x16);
	Print_SetLineHeight(16);
#endif
	
	AppUtils_ClosePrinter();
	
	return BANK_OK;
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_RePrintLast
 DESCRIPTION:   重打印最后一笔
 PARAMETERS:    dummy
 RETURN:        BANK_OK / BANK_FAIL
 NOTE:
 *-------------------------------------------------------------------------*/
int PrnDisp_RePrintLast(void *dummy)
{
	DynData TransData;
	
	AppUtils_UserClear(LINE1);
	AppUtils_DisplayCenter("重打印最后一笔", LINE1, TRUE);
	
	memset(&TransData, 0, sizeof(TransData));
	
	if(AppUtils_JournalGetRecordLast(&TransData) != BANK_OK)
		return BANK_FAIL;
		
	PrnDisp_PrintTxn(&TransData, TRUE);
	
	return BANK_OK;
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrnDisp_RePrintTxn
 DESCRIPTION:   重打印任意一笔交易(根据凭证号搜索)
 PARAMETERS:    dummy
 RETURN:        BANK_OK / BANK_FAIL
 NOTE:
 *-------------------------------------------------------------------------*/
int PrnDisp_RePrintTxn(void *dummy)
{
	DynData TransData;
	char cBuffer[ 7 ];
	
	AppUtils_UserClear(LINE1);
	AppUtils_DisplayCenter("重打印任意一笔", LINE1, TRUE);
	
	// 获取原交易数据
	while(TRUE)
	{
		memset(cBuffer, 0, sizeof(cBuffer));
		
		if(AppUtils_GetNum(LINE2, "请输入凭证号:", cBuffer, 1, 6, 60) <= 0)
			return BANK_FAIL;
			
		memset(&TransData, 0, sizeof(TransData));
		
		if(AppUtils_JournalGetRecord(atol(cBuffer), &TransData) >= 0)
			break;
		else
			PrnDisp_DisplayWarning("", "原交易不存在!");
	}
	
	PrnDisp_PrintTxn(&TransData, TRUE);
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_RePrintSettlement
 * DESCRIPTION:   重打印结算单
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_RePrintSettlement(void *dummy)
{
	TotalRec TotalsData;
	
	AppUtils_Cls();
	AppUtils_DisplayCenter((char*)"重打印结算单", LINE1, TRUE);
	memset(&TotalsData, 0, sizeof(TotalRec));
	
	if(Trans_ReadTotalData(&TotalsData) == BANK_OK)
		PrnDisp_PrintSettlement(&TotalsData, TRUE);
	else
		AppUtils_Warning((char*)"无结算单信息");
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintJournal
 * DESCRIPTION:   显示应用软件版本信息
 * PARAMETERS:    dummy =
 *                (void*)1: 打印离线上送失败交易明细
 *                (void*)2: 打印离线上送拒绝交易明细
 *                (void*)3: 结算后的交易明细打印
 *                (void*)4: "打印"菜单中的"打印交易明细"
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_PrintJournal(void *dummy)
{
	int i;
	int iRecNum;
	int iPrintTitleFlag = 1;
	byte bNegativeFlag;
	char cLTmpBuf[ 56 ];
	char cRTmpBuf[ 56 ];
	DynData TransData;
	
	AppUtils_UserClear(LINE1);
	iRecNum = AppUtils_GetNumberOfRecords((char *)TRAN_DB, sizeof(TransRecord));
	
	if(iRecNum <= 0)
	{
		if(dummy == (void *)4 || dummy == (void *)3)
		{
			AppUtils_UserClear(LINE2);
			AppUtils_Warning("无交易明细");
		}
		
		return BANK_OK;
	}
	
	AppUtils_OpenPrinter();
	
	if(dummy == (void *)4)
	{
		AppUtils_DisplayTitle("打印交易明细");
		AppUtils_DisplayCenter("正在打印", LINE3, FALSE);
		
        PrnDisp_PrintStringCenterAlign("交易明细/TXN LIST", 3);
		
		// ------
		PrnDisp_SepLine();
		
		// ------
        PrnDisp_PrintString("商户名称(MERCHANT NAME):", 1, TRUE);
		
		memset(cLTmpBuf, 0, sizeof(cLTmpBuf));
		sprintf(cLTmpBuf, "%s", gTermCfg.mFixPrm.MerchantNameChn);
        PrnDisp_PrintString(cLTmpBuf, 2, TRUE);
		
		if(strlen((char *)gTermCfg.mFixPrm.MerchantNameEng) > 0)
		{
			sprintf(cLTmpBuf, "%s", gTermCfg.mFixPrm.MerchantNameEng);
            PrnDisp_PrintString(cLTmpBuf, 2, TRUE);
		}
		
		// ------
        PrnDisp_PrintString("商户编号(MERCHANT NO):", 1, TRUE);
		
        PrnDisp_PrintString((char *)gTermCfg.mFixPrm.MerchantID, 3, TRUE);
		
		// ------
        PrnDisp_PrintString("终端编号(TERMINAL NO):", 1, TRUE);
		
        PrnDisp_PrintString((char *)gTermCfg.mFixPrm.TerminalID, 3, TRUE);
		
		// ------
        PrnDisp_PrintString("操作员号(OPERATOR NO):", 1, FALSE);
        PrnDisp_PrintString((char *)gTermCfg.mDynPrm.OperID, 3, TRUE);
		
		// ------
        PrnDisp_PrintString("批次号(BATCH NO):", 1, FALSE);
		sprintf(cLTmpBuf, "%06lu", gTermCfg.mDynPrm.BatchNum);
        PrnDisp_PrintString(cLTmpBuf, 3, TRUE);
		
		// ------
        PrnDisp_PrintString("日期/时间(DATE/TIME):", 1, TRUE);
		memset(cLTmpBuf, 0, sizeof(cLTmpBuf));
		AppUtils_GetClock(cLTmpBuf);
		AppUtils_Asc2Bcd((byte *)cLTmpBuf, TransData.TransDB.TranDate, 8);
		AppUtils_Asc2Bcd((byte *)cLTmpBuf + 8, TransData.TransDB.TranTime, 6);
		
		memset(cLTmpBuf, 0, sizeof(cLTmpBuf));
		AppUtils_ConvertTransDateTime(&TransData, cLTmpBuf);
        PrnDisp_PrintString(cLTmpBuf, 3, TRUE);
	}
	
	// ------
	iRecNum = AppUtils_GetNumberOfRecords((char *)TRAN_DB, sizeof(TransRecord));
	
	if(iRecNum <= 0)
	{
		AppUtils_ClosePrinter();
		return BANK_FAIL;
	}
	
	
	for(i = 0; i < iRecNum; i ++)
	{
		memset(&TransData, 0, sizeof(TransData));
		
		if(AppUtils_GetTransRecByIdx(&TransData, i) != BANK_OK)
			break;
			
		// 打印离线未上送成功交易明细
		if(dummy == (void *)1)
		{
			//必须是离线交易
			if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_OFFLINE) != BANK_OK)
				continue;
				
			//必须是上送失败交易
			if(AppUtils_CheckStatus(TransData.TransDB.Status[ 1 ], STATUS_UPLFAIL_TO) != BANK_OK)
				continue;
		}
		
		// 打印离线主机拒绝交易明细
		else if(dummy == (void *)2)
		{
			//必须是离线交易
			if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_OFFLINE) != BANK_OK)
				continue;
				
			//必须是上送失败交易
			if(AppUtils_CheckStatus(TransData.TransDB.Status[ 1 ], STATUS_UPLFAIL_ER) != BANK_OK)
				continue;
		}
		
		//结算后的交易明细
		else if(dummy == (void *)3)
		{
			//不打印全部交易明细时, 需要过滤掉撤销类交易和已撤交易
			if(gTermCfg.mTrSwitch.AllTransDetailFlag == 0)
			{
				//已撤销的交易不打印
				if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_VOID) == BANK_OK)
				{
					continue;
				}
				
				switch(TransData.TransDB.TrType)
				{
				case BTYPE_BALANCE:
				case BTYPE_SALE_VOID:
				case BTYPE_COMPLETE_VOID:
				case BTYPE_INSTALLMENT_VOID:
				case BTYPE_ISSUER_SCORE_SALE_VOID:
				case BTYPE_MCHIP_COMPLETE_VOID:
				case BTYPE_MCHIP_SALE_VOID:
				case BTYPE_ORDER_SALE_VOID:
				case BTYPE_ORDER_COMPLETE_VOID:
				case BTYPE_UNION_SCORE_SALE_VOID:
				case BTYPE_ECASH_OUT_RELOAD:
				case BTYPE_ECASH_IN_RELOAD:
				case BTYPE_ECASH_CASH_RELOAD_VOID:
				case BTYPE_YUYUE_SALE_VOID:
				case BTYPE_MAG_ACCTLOAD:
					continue;
				}
				
				//已调整的离线结算不打印
				if(TransData.TransDB.TrType == BTYPE_OFFLINE_SETTLE
				   && AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
				{
					continue;
				}
			}
			
			//预授权和预授权撤销，不在结算后的交易明细里打印
			switch(TransData.TransDB.TrType)
			{
			case BTYPE_PREAUTH:
			case BTYPE_PREAUTH_VOID:
			case BTYPE_MCHIP_PREAUTH:
			case BTYPE_MCHIP_PREAUTH_VOID:
			case BTYPE_ORDER_PREAUTH:
			case BTYPE_ORDER_PREAUTH_VOID:
				continue;
			}
		}
		
		// 结算明细中: 如果该笔交易已经被调整,不打印该笔交易明细,把该笔交易的调整明细交易类型修改为原类型打印出来
		if(dummy == (void *)3)
		{
			if(AppUtils_CheckStatus(TransData.TransDB.Status[ 0 ], STATUS_ADJUST) == BANK_OK)
			{
				if((TransData.TransDB.TrType == BTYPE_SALE) || (TransData.TransDB.TrType == BTYPE_OFFLINE_SETTLE))
					continue;
			}
			else if(TransData.TransDB.TrType == BTYPE_OFFLINE_ADJUST)
				TransData.TransDB.TrType = TransData.TransDB.OrgTranType;
		}
		
		// "打印"菜单中的"打印交易明细"
		else if(dummy == (void *)4)
		{
			;
		}
		
		if(iPrintTitleFlag == 1)
		{
			iPrintTitleFlag = 2;
			
			AppUtils_DisplayTitle("打印交易明细");
			AppUtils_DisplayCenter("正在打印", LINE3, FALSE);
			
			if(dummy == (void *)1)
                PrnDisp_PrintStringCenterAlign("未成功上送离线明细单", 2);
			else if(dummy == (void *)2)
                PrnDisp_PrintStringCenterAlign("上送被拒离线明细单", 2);
			else if(dummy == (void *)3)
                PrnDisp_PrintStringCenterAlign("结算交易明细/SETTLEMENT TXN LIST", 2);
				
			// ------
			PrnDisp_SepLine();
			
            PrnDisp_PrintStringLeftRightAlign("卡号/CARD NUMBER", 1, "类型/TYPE", 1);
			
			if(dummy == (void *)3 || dummy == (void *)4)
                PrnDisp_PrintStringLeftRightAlign("凭证号/VOUCHER 授权号/AUTH ID", 1, " 金额/AMOUNT", 1);
			else
                PrnDisp_PrintStringLeftRightAlign("凭证号/VOUCHER", 1, " 金额/AMOUNT", 1);
				
			PrnDisp_SepLine();
		}
		
		//转换卡号(根据交易类型决定是否卡号屏蔽)
		memset(cLTmpBuf, 0, sizeof(cLTmpBuf));
		AppUtils_ConvertPANbyMask(&TransData, 1, cLTmpBuf);
		
		//默认为正号
		bNegativeFlag = '+';
		memset(cRTmpBuf, 0, sizeof(cRTmpBuf));
		
		if(dummy == (void *)4)
		{
			switch(TransData.TransDB.TrType)
			{
			case BTYPE_SALE:
			
				if((TransData.TransDB.Swipe == TRAN_QPBOC && TransData.TransDB.qPBOC_OnlineFlag == 0) ||
				   (TransData.TransDB.EC_flag == 1))
					sprintf(cRTmpBuf, "电子现金消费");
				else
					sprintf(cRTmpBuf, "消费");
					
				break;
			case BTYPE_SALE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "撤销");
				break;
			case BTYPE_COMPLETE:
				sprintf(cRTmpBuf, "预授权完成(请求)");
				break;
			case BTYPE_OFFLINE_COMPLETE:
				sprintf(cRTmpBuf, "预授权完成(通知)");
				break;
			case BTYPE_COMPLETE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "预授权完成撤销");
				break;
			case BTYPE_PREAUTH:
				sprintf(cRTmpBuf, "预授权");
				break;
			case BTYPE_PREAUTH_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "预授权撤销");
				break;
			case BTYPE_REFUND:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "退货");
				break;
			case BTYPE_ECASH_REFUND:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "电子现金退货");
				break;
			case BTYPE_OFFLINE_SETTLE:
				sprintf(cRTmpBuf, "离线结算");
				break;
			case BTYPE_OFFLINE_ADJUST:
				sprintf(cRTmpBuf, "结算调整");
				break;
			case BTYPE_ECASH_CASH_RELOAD:
				sprintf(cRTmpBuf, "现金充值");
				break;
			case BTYPE_ECASH_CASH_RELOAD_VOID:
				sprintf(cRTmpBuf, "现金充值撤销");
				break;
			case BTYPE_ECASH_IN_RELOAD:
				sprintf(cRTmpBuf, "指定账户圈存");
				break;
			case BTYPE_ECASH_OUT_RELOAD:
				sprintf(cRTmpBuf, "非指定账户圈存");
				break;
			case BTYPE_INSTALLMENT:
				sprintf(cRTmpBuf, "分期付款");
				break;
			case BTYPE_INSTALLMENT_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "分期付款撤销");
				break;
			case BTYPE_YUYUE_SALE:
				sprintf(cRTmpBuf, "预约消费");
				break;
			case BTYPE_YUYUE_SALE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "预约消费撤销");
				break;
			case BTYPE_ISSUER_SCORE_SALE:
				sprintf(cRTmpBuf, "发卡行积分消费");
				break;
			case BTYPE_ISSUER_SCORE_SALE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "发卡行积分撤销");
				break;
			case BTYPE_UNION_SCORE_SALE:
				sprintf(cRTmpBuf, "联盟积分消费");
				break;
			case BTYPE_UNION_SCORE_SALE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "联盟积分撤销");
				break;
			case BTYPE_UNION_SCORE_REFUND:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "联盟积分退货");
				break;
			case BTYPE_MAG_CASHLOAD:
				sprintf(cRTmpBuf, "磁条卡现金充值");
				break;
			case BTYPE_MAG_ACCTLOAD:
				sprintf(cRTmpBuf, "磁条卡账户充值");
				break;
			case BTYPE_MCHIP_SALE:
				sprintf(cRTmpBuf, "手机芯片消费");
				break;
			case BTYPE_MCHIP_SALE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "手机芯片消费撤销");
				break;
			case BTYPE_MCHIP_REFUND:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "手机芯片退货");
				break;
			case BTYPE_MCHIP_PREAUTH:
				sprintf(cRTmpBuf, "手机芯片预授权");
				break;
			case BTYPE_MCHIP_PREAUTH_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "手机芯片预授权撤销");
				break;
			case BTYPE_MCHIP_COMPLETE:
				sprintf(cRTmpBuf, "手机芯片预授权完成请求");
				break;
			case BTYPE_MCHIP_COMPLETE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "手机芯片预授权完成撤销");
				break;
			case BTYPE_MCHIP_OFFLINE_COMPLETE:
				sprintf(cRTmpBuf, "手机芯片预授权完成通知");
				break;
			case BTYPE_ORDER_SALE:
				sprintf(cRTmpBuf, "订购消费");
				break;
			case BTYPE_ORDER_SALE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "订购消费撤销");
				break;
			case BTYPE_ORDER_REFUND:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "订购退货");
				break;
			case BTYPE_ORDER_PREAUTH:
				sprintf(cRTmpBuf, "订购预授权");
				break;
			case BTYPE_ORDER_PREAUTH_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "订购预授权撤销");
				break;
			case BTYPE_ORDER_COMPLETE:
				sprintf(cRTmpBuf, "订购预授权完成请求");
				break;
			case BTYPE_ORDER_COMPLETE_VOID:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "订购预授权完成撤销");
				break;
			case BTYPE_ORDER_OFFLINE_COMPLETE:
				sprintf(cRTmpBuf, "订购预授权完成通知");
				break;
			default:
				//sprintf( cRTmpBuf, "未知交易" );
				break;
			}
		}
		else
		{
			switch(TransData.TransDB.TrType)
			{
			case BTYPE_SALE:
			case BTYPE_ORDER_SALE:
			case BTYPE_YUYUE_SALE:
			case BTYPE_MCHIP_SALE:
			
				if((TransData.TransDB.Swipe == TRAN_QPBOC && TransData.TransDB.qPBOC_OnlineFlag == 0) ||
				   (TransData.TransDB.EC_flag == 1))
					sprintf(cRTmpBuf, "E");
				else
					sprintf(cRTmpBuf, "S");
					
				break;
			case BTYPE_COMPLETE:
			case BTYPE_ORDER_COMPLETE:
			case BTYPE_MCHIP_COMPLETE:
				sprintf(cRTmpBuf, "P");
				break;
			case BTYPE_INSTALLMENT:
				sprintf(cRTmpBuf, "T");
				break;
			case BTYPE_UNION_SCORE_SALE:
			case BTYPE_ISSUER_SCORE_SALE:
				sprintf(cRTmpBuf, "B");
				break;
			case BTYPE_REFUND:
			case BTYPE_UNION_SCORE_REFUND:
			case BTYPE_YUYUE_REFUND:
			case BTYPE_ECASH_REFUND:
			case BTYPE_ORDER_REFUND:
			case BTYPE_MCHIP_REFUND:
				bNegativeFlag = '-';
				sprintf(cRTmpBuf, "R");
				break;
			case BTYPE_OFFLINE_COMPLETE:
			case BTYPE_ORDER_OFFLINE_COMPLETE:
			case BTYPE_MCHIP_OFFLINE_COMPLETE:
				sprintf(cRTmpBuf, "C");
				break;
			case BTYPE_OFFLINE_SETTLE:
			case BTYPE_OFFLINE_ADJUST:
				sprintf(cRTmpBuf, "L");
				break;
			case BTYPE_ECASH_CASH_RELOAD:
			case BTYPE_MAG_CASHLOAD:
				sprintf(cRTmpBuf, "Q");
				break;
			case BTYPE_SALE_VOID:
			case BTYPE_INSTALLMENT_VOID:
			case BTYPE_YUYUE_SALE_VOID:
			case BTYPE_ISSUER_SCORE_SALE_VOID:
			case BTYPE_UNION_SCORE_SALE_VOID:
			case BTYPE_COMPLETE_VOID:
			case BTYPE_MCHIP_SALE_VOID:
			case BTYPE_MCHIP_COMPLETE_VOID:
			case BTYPE_ORDER_SALE_VOID:
			case BTYPE_ORDER_COMPLETE_VOID:
				bNegativeFlag = '-';
				break;
				
			default:
				sprintf(cRTmpBuf, " ");
				break;
			}
		}
		
        PrnDisp_PrintStringLeftRightAlign(cLTmpBuf, 1, cRTmpBuf, 1);
		
		// 流水号、授权码
		if(dummy == (void *)3 || dummy == (void *)4)
			sprintf(cLTmpBuf, "%06lu  %6s", TransData.TransDB.SysTrace, TransData.TransDB.AuthCode);
		else
			sprintf(cLTmpBuf, "%06lu", TransData.TransDB.SysTrace);
			
		// 此处再根据参数决定是否设置成正号 by XC 2014/12/18 11:34:51
		if(!gTermCfg.mTrSwitch.NegativeSignFlag)
			bNegativeFlag = '+';
			
		memset(cRTmpBuf, 0, sizeof(cRTmpBuf));
		AppUtils_AmountAsc2String(TransData.TransDB.Amount, "", bNegativeFlag, cRTmpBuf);
		
        PrnDisp_PrintStringLeftRightAlign(cLTmpBuf, 1, cRTmpBuf, 1);
	}
	
	
	// ------
	if(iPrintTitleFlag == 2)
	{
		PrnDisp_Trail();
		Print_CheckPrintFinish();
	}
	
	AppUtils_ClosePrinter();
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintSettlement
 * DESCRIPTION:   打印(重打印)结算单
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
void PrnDisp_PrintSettlement(TotalRec *pTotalsData, byte bReprintFlag)
{
	int i;
	char cTmpBuf[ 126 ];
	byte PositiveAmount[12+1], NegativeAmount[12+1], TotalAmount[12+1];
	int TotalNum;
	byte bSignFlag;
	
	AppUtils_UserClear(LINE1);
	
	if(bReprintFlag)
	{
		AppUtils_DisplayCenter("重打印结算单", LINE1, TRUE);
		AppUtils_DisplayCenter("正在打印", LINE3, FALSE);
	}
	else
	{
		AppUtils_DisplayCenter("打印结算单", LINE1, TRUE);
		AppUtils_DisplayCenter("正在打印", LINE3, FALSE);
	}
	
	if(gTermCfg.mTrSwitch.NegativeSignFlag)
		bSignFlag = '-';
	else
		bSignFlag = '+';
		
	// ------
	AppUtils_OpenPrinter();
	
	PrnDisp_SepLine();
	
    PrnDisp_PrintStringCenterAlign("POS结算总计单", 3);
	
	if(gTermCfg.mTrSwitch.EnglishSlip)
        PrnDisp_PrintStringCenterAlign("POS SETTLEMENT SLIP", 3);
		
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
	// ------
    PrnDisp_PrintString("商户名称(MERCHANT NAME):", 1, TRUE);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%s", pTotalsData->CnName);
    PrnDisp_PrintString(cTmpBuf, 2, TRUE);
	
	if(strlen((char *)pTotalsData->EnName) > 0)
	{
		sprintf(cTmpBuf, "%s", pTotalsData->EnName);
        PrnDisp_PrintString(cTmpBuf, 2, TRUE);
	}
	
	// ------
    PrnDisp_PrintString("商户编号(MERCHANT NO):", 1, TRUE);
    PrnDisp_PrintString((char *)pTotalsData->MechID, 3, TRUE);
	
	// ------
    PrnDisp_PrintString("终端编号(TERMINAL NO):", 1, TRUE);
    PrnDisp_PrintString((char *)pTotalsData->TermID, 3, TRUE);
	
	// ------
    PrnDisp_PrintString("操作员号(OPERATOR NO):", 1, FALSE);
    PrnDisp_PrintString((char *)pTotalsData->OperID, 3, TRUE);
	
	// ------
    PrnDisp_PrintString("批次号(BATCH NO):", 1, FALSE);
	sprintf(cTmpBuf, "%06lu", pTotalsData->BatchNo);
    PrnDisp_PrintString(cTmpBuf, 3, TRUE);
	
	// ------
    PrnDisp_PrintString("日期/时间(DATE/TIME):", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%02X%02X-%02X-%02X  %02X:%02X:%02X",
	        pTotalsData->DateTime[0], pTotalsData->DateTime[1], pTotalsData->DateTime[2], pTotalsData->DateTime[3],
	        pTotalsData->DateTime[4], pTotalsData->DateTime[5], pTotalsData->DateTime[6]);
    PrnDisp_PrintString(cTmpBuf, 3, TRUE);
	
	// ------
	PrnDisp_SepLine();
    PrnDisp_PrintString("类型/TYPE", 1, TRUE);
    PrnDisp_PrintStringLeftRightAlign("笔数/SUM", 1, " 金额/AMOUNT", 1);
	
	// ------
	if(pTotalsData->CupSettleFlag)
        PrnDisp_PrintStringCenterAlign("内卡对帐不平", 3);
	else
        PrnDisp_PrintStringCenterAlign("内卡对帐平", 3);
		
	// 清除相关缓冲区
	TotalNum = 0;
	
	memset(PositiveAmount, 0, sizeof(PositiveAmount));
	memset(PositiveAmount, '0', 12);
	
	memset(NegativeAmount, 0, sizeof(NegativeAmount));
	memset(NegativeAmount, '0', 12);
	
	memset(TotalAmount, 0, sizeof(TotalAmount));
	memset(TotalAmount, '0', 12);
	
	// ------
    PrnDisp_PrintString("电子现金消费/EC SALE", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->CupPrtECSaleCnt);
	AppUtils_AmountAsc2String(pTotalsData->CupPrtECSaleAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->CupPrtECSaleCnt;
	AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->CupPrtECSaleAmt);
	
	if(gTermCfg.mTrSwitch.AllTransDetailFlag)
	{
		// ------
        PrnDisp_PrintString("消费/SALE", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->CupAllSaleCnt);
		AppUtils_AmountAsc2String(pTotalsData->CupAllSaleAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->CupAllSaleCnt;
		AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->CupAllSaleAmt);
		
		// ------
        PrnDisp_PrintString("撤销/VOID", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->CupAllVoidCnt);
		AppUtils_AmountAsc2String(pTotalsData->CupAllVoidAmt, CURRENCY_NAME, bSignFlag,  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->CupAllVoidCnt;
		AppUtils_AmountAdd(NegativeAmount, NegativeAmount, pTotalsData->CupAllVoidAmt);
		
		// ------
        PrnDisp_PrintString("预授权完成请求/COMPLETE", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->CupAllCompCnt);
		AppUtils_AmountAsc2String(pTotalsData->CupAllCompAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->CupAllCompCnt;
		AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->CupAllCompAmt);
		
		// ------
        PrnDisp_PrintString("预授权完成撤销/COMPLETE VOID", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->CupAllCompVoidCnt);
		AppUtils_AmountAsc2String(pTotalsData->CupAllCompVoidAmt, CURRENCY_NAME, bSignFlag,  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->CupAllCompVoidCnt;
		AppUtils_AmountAdd(NegativeAmount, NegativeAmount, pTotalsData->CupAllCompVoidAmt);
	}
	else
	{
		// ------
        PrnDisp_PrintString("消费/SALE", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		i = pTotalsData->CupPrtSaleCnt - pTotalsData->CupPrtECSaleCnt;
		
		if(i < 0)
			i = 0;
			
		sprintf(cTmpBuf, "%03d", i);
		//此处减法要注意是否有问题 by XC 2014/12/20 19:52:59
		AppUtils_AmountSub(pTotalsData->CupPrtSaleAmt, pTotalsData->CupPrtSaleAmt, pTotalsData->CupPrtECSaleAmt);
		AppUtils_AmountAsc2String(pTotalsData->CupPrtSaleAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += i;
		AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->CupPrtSaleAmt);
		
		// ------
        PrnDisp_PrintString("预授权完成请求/COMPLETE", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->CupPrtCompCnt);
		AppUtils_AmountAsc2String(pTotalsData->CupPrtCompAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->CupPrtCompCnt;
		AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->CupPrtCompAmt);
	}
	
	
	// ------
    PrnDisp_PrintString("退货/REFUND", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->CupPrtRefundCnt);
	AppUtils_AmountAsc2String(pTotalsData->CupPrtRefundAmt, CURRENCY_NAME, bSignFlag,  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->CupPrtRefundCnt;
	AppUtils_AmountAdd(NegativeAmount, NegativeAmount, pTotalsData->CupPrtRefundAmt);
	
	// ------
    PrnDisp_PrintString("离线结算/OFFLINE", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->CupPrtOfflineCnt);
	AppUtils_AmountAsc2String(pTotalsData->CupPrtOfflineAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->CupPrtOfflineCnt;
	AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->CupPrtOfflineAmt);
	
	// ------
    PrnDisp_PrintString("预授权完成通知SETTLEMENT", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->CupPrtCompOfflineCnt);
	AppUtils_AmountAsc2String(pTotalsData->CupPrtCompOfflineAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->CupPrtCompOfflineCnt;
	AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->CupPrtCompOfflineAmt);
	
	// ------
    PrnDisp_PrintString("圈存/LOAD", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->CupPrtLoadCnt);
	AppUtils_AmountAsc2String(pTotalsData->CupPrtLoadAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->CupPrtLoadCnt;
	//现金圈存对于商户来说应计入贷记
	AppUtils_AmountAdd(NegativeAmount, NegativeAmount, pTotalsData->CupPrtLoadAmt);
	
	
	if(gTermCfg.mTrSwitch.AllTransDetailFlag)
	{
		AppDebug_AppLog("PositiveAmount=[%s]", PositiveAmount);
		AppDebug_AppLog("NegativeAmount=[%s]", NegativeAmount);
		
		if(memcmp(PositiveAmount, NegativeAmount, 12) >= 0)
		{
			bSignFlag = '+';
			AppUtils_AmountSub(TotalAmount, PositiveAmount, NegativeAmount);
			AppDebug_AppLog("TotalAmount=[%s]", TotalAmount);
		}
		else
		{
			bSignFlag = '-';
			AppUtils_AmountSub(TotalAmount, NegativeAmount, PositiveAmount);
			AppDebug_AppLog("TotalAmount=[%s]", TotalAmount);
		}
		
        PrnDisp_PrintString("总计/TOTAL", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", TotalNum);
		AppUtils_AmountAsc2String(TotalAmount, CURRENCY_NAME, bSignFlag, cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	}
	
	// ------
    PrnDisp_PrintLineFeed(3);
	
	if(gTermCfg.mTrSwitch.NegativeSignFlag)
		bSignFlag = '-';
	else
		bSignFlag = '+';
		
	// ------
	if(pTotalsData->FgnSettleFlag)
        PrnDisp_PrintStringCenterAlign("外卡对帐不平", 3);
	else
        PrnDisp_PrintStringCenterAlign("外卡对帐平", 3);
		
		
	// 清除相关缓冲区
	TotalNum = 0;
	
	memset(PositiveAmount, 0, sizeof(PositiveAmount));
	memset(PositiveAmount, '0', 12);
	
	memset(NegativeAmount, 0, sizeof(NegativeAmount));
	memset(NegativeAmount, '0', 12);
	
	memset(TotalAmount, 0, sizeof(TotalAmount));
	memset(TotalAmount, '0', 12);
	
	if(gTermCfg.mTrSwitch.AllTransDetailFlag)
	{
		// ------
        PrnDisp_PrintString("消费/SALE", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->FgnAllSaleCnt);
		AppUtils_AmountAsc2String(pTotalsData->FgnAllSaleAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->FgnAllSaleCnt;
		AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->FgnAllSaleAmt);
		
		// ------
        PrnDisp_PrintString("撤销/VOID", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->FgnAllVoidCnt);
		AppUtils_AmountAsc2String(pTotalsData->FgnAllVoidAmt, CURRENCY_NAME, bSignFlag,  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->FgnAllVoidCnt;
		AppUtils_AmountAdd(NegativeAmount, NegativeAmount, pTotalsData->FgnAllVoidAmt);
		
		// ------
        PrnDisp_PrintString("预授权完成请求/COMPLETE", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->FgnAllCompCnt);
		AppUtils_AmountAsc2String(pTotalsData->FgnAllCompAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->FgnAllCompCnt;
		AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->FgnAllCompAmt);
		
		
		// ------
        PrnDisp_PrintString("预授权完成撤销/COMPLETE VOID", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->FgnAllCompVoidCnt);
		AppUtils_AmountAsc2String(pTotalsData->FgnAllCompVoidAmt, CURRENCY_NAME, bSignFlag,  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->FgnAllCompVoidCnt;
		AppUtils_AmountAdd(NegativeAmount, NegativeAmount, pTotalsData->FgnAllCompVoidAmt);
	}
	else
	{
		// ------
        PrnDisp_PrintString("消费/SALE", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->FgnPrtSaleCnt);
		AppUtils_AmountAsc2String(pTotalsData->FgnPrtSaleAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->FgnPrtSaleCnt;
		AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->FgnPrtSaleAmt);
		
		// ------
        PrnDisp_PrintString("预授权完成请求/COMPLETE", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", pTotalsData->FgnPrtCompCnt);
		AppUtils_AmountAsc2String(pTotalsData->FgnPrtCompAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
		TotalNum += pTotalsData->FgnPrtCompCnt;
		AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->FgnPrtCompAmt);
	}
	
	// ------
    PrnDisp_PrintString("退货/REFUND", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->FgnPrtRefundCnt);
	AppUtils_AmountAsc2String(pTotalsData->FgnPrtRefundAmt, CURRENCY_NAME, bSignFlag,  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->FgnPrtRefundCnt;
	AppUtils_AmountAdd(NegativeAmount, NegativeAmount, pTotalsData->FgnPrtRefundAmt);
	
	// ------
    PrnDisp_PrintString("离线结算/OFFLINE", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->FgnPrtOfflineCnt);
	AppUtils_AmountAsc2String(pTotalsData->FgnPrtOfflineAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->FgnPrtOfflineCnt;
	AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->FgnPrtOfflineAmt);
	
	// ------
    PrnDisp_PrintString("预授权完成通知SETTLEMENT", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->FgnPrtCompOfflineCnt);
	AppUtils_AmountAsc2String(pTotalsData->FgnPrtCompOfflineAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->FgnPrtCompOfflineCnt;
	AppUtils_AmountAdd(PositiveAmount, PositiveAmount, pTotalsData->FgnPrtCompOfflineAmt);
	
	// ------
    PrnDisp_PrintString("圈存/LOAD", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", pTotalsData->FgnPrtLoadCnt);
	AppUtils_AmountAsc2String(pTotalsData->FgnPrtLoadAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	TotalNum += pTotalsData->FgnPrtLoadCnt;
	AppUtils_AmountAdd(NegativeAmount, NegativeAmount, pTotalsData->FgnPrtLoadAmt);
	
	if(gTermCfg.mTrSwitch.AllTransDetailFlag)
	{
		if(memcmp(PositiveAmount, NegativeAmount, 12) >= 0)
		{
			bSignFlag = '+';
			AppUtils_AmountSub(TotalAmount, PositiveAmount, NegativeAmount);
		}
		else
		{
			bSignFlag = '-';
			AppUtils_AmountSub(TotalAmount, NegativeAmount, PositiveAmount);
		}
		
        PrnDisp_PrintString("总计/TOTAL", 1, TRUE);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%03d", TotalNum);
		AppUtils_AmountAsc2String(TotalAmount, CURRENCY_NAME, bSignFlag, cTmpBuf + 10);
        PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 3, cTmpBuf + 10, 3);
	}
	
	// ------
	if(bReprintFlag == 1)
        PrnDisp_PrintStringCenterAlign("*** 重印凭证/DUPLICATED ***", 1);
		
	// ------
	PrnDisp_Trail();
	
	Print_CheckPrintFinish();
	
	AppUtils_ClosePrinter();
	
	return;
}


int PrnDisp_PrintTotal(void *dummy)
{
	char cTmpBuf[128], cLTmpBuf[128], cRTmpBuf[128];
	TotalRec TotalsData;
	
	AppUtils_UserClear(LINE1);
	AppUtils_DisplayCenter("打印交易汇总", LINE1, TRUE);
	AppUtils_DisplayCenter("正在打印", LINE3, FALSE);
	
	// 交易统计
	memset(&TotalsData, 0, sizeof(TotalsData));
	ASSERT_FAIL(Trans_CalculateTotals(&TotalsData));
	
	AppUtils_OpenPrinter();
	
	// ------
	PrnDisp_SepLine();
	
    PrnDisp_PrintStringCenterAlign("POS交易汇总单", 3);
	
	if(gTermCfg.mTrSwitch.EnglishSlip)
        PrnDisp_PrintStringCenterAlign("POS TOTAL SLIP", 3);
		
	PrnDisp_SepLine();
	
	// ------
    PrnDisp_PrintString("商户名称(MERCHANT NAME):", 1, TRUE);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%s", gTermCfg.mFixPrm.MerchantNameChn);
    PrnDisp_PrintString(cTmpBuf, 2, TRUE);
	
	if(strlen((char *)gTermCfg.mFixPrm.MerchantNameEng) > 0)
	{
		sprintf(cTmpBuf, "%s", gTermCfg.mFixPrm.MerchantNameEng);
        PrnDisp_PrintString(cTmpBuf, 2, TRUE);
	}
	
	// ------
    PrnDisp_PrintString("商户编号(MERCHANT NO):", 1, TRUE);
	
    PrnDisp_PrintString((char *)gTermCfg.mFixPrm.MerchantID, 3, TRUE);
	
	// ------
    PrnDisp_PrintString("终端编号(TERMINAL NO):", 1, TRUE);
	
    PrnDisp_PrintString((char *)gTermCfg.mFixPrm.TerminalID, 3, TRUE);
	
	// ------
    PrnDisp_PrintString("操作员号(OPERATOR NO):", 1, FALSE);
    PrnDisp_PrintString((char *)gTermCfg.mDynPrm.OperID, 3, TRUE);
	
	// ------
    PrnDisp_PrintString("批次号(BATCH NO):", 1, FALSE);
	sprintf(cTmpBuf, "%06lu", gTermCfg.mDynPrm.BatchNum);
    PrnDisp_PrintString(cTmpBuf, 3, TRUE);
	
	// ------
    PrnDisp_PrintString("日期/时间(DATE/TIME):", 1, TRUE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_GetClock(cTmpBuf);
	memset(cRTmpBuf, 0, sizeof(cRTmpBuf));
	AppUtils_Asc2Bcd((byte *)cTmpBuf, (byte *)cRTmpBuf, 14);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%02X%02X-%02X-%02X  %02X:%02X:%02X",
	        cRTmpBuf[0], cRTmpBuf[1], cRTmpBuf[2], cRTmpBuf[3],
	        cRTmpBuf[4], cRTmpBuf[5], cRTmpBuf[6]);
    PrnDisp_PrintString(cTmpBuf, 3, TRUE);
	
	// ------
	PrnDisp_SepLine();
	
#if 0
	// ------
    PrnDisp_PrintStringLeftRightAlign("内卡借记 笔数", 2, "金额", 2);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", TotalsData.CupTotalSaleCnt);
	AppUtils_AmountAsc2String(TotalsData.CupTotalSaleAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 2, cTmpBuf + 10, 2);
	
	// ------
    PrnDisp_PrintStringLeftRightAlign("内卡贷记 笔数", 2, "金额", 2);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", TotalsData.CupTotalVoidCnt);
	AppUtils_AmountAsc2String(TotalsData.CupTotalVoidAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 2, cTmpBuf + 10, 2);
	
	// ------
    PrnDisp_PrintStringLeftRightAlign("外卡借记 笔数", 2, "金额", 2);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", TotalsData.FgnTotalSaleCnt);
	AppUtils_AmountAsc2String(TotalsData.FgnTotalSaleAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 2, cTmpBuf + 10, 2);
	
	// ------
    PrnDisp_PrintStringLeftRightAlign("外卡贷记 笔数", 2, "金额", 2);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%03d", TotalsData.FgnTotalVoidCnt);
	AppUtils_AmountAsc2String(TotalsData.FgnTotalVoidAmt, CURRENCY_NAME, '+',  cTmpBuf + 10);
    PrnDisp_PrintStringLeftRightAlign(cTmpBuf, 2, cTmpBuf + 10, 2);
#endif
	
#if 1
	// ------
    PrnDisp_PrintString("内卡总计:", 3, TRUE); \
	
	memset(cLTmpBuf, 0, sizeof(cLTmpBuf));
	sprintf(cLTmpBuf, "借记笔数%3d", TotalsData.CupTotalSaleCnt);
	memset(cRTmpBuf, 0, sizeof(cRTmpBuf));
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_AmountAsc2String(TotalsData.CupTotalSaleAmt, CURRENCY_NAME, '+',  cTmpBuf);
	sprintf(cRTmpBuf, "借记金额%11s", cTmpBuf);
    PrnDisp_PrintStringLeftRightAlign(cLTmpBuf, 2, cRTmpBuf, 2);
	
	// ------
	memset(cLTmpBuf, 0, sizeof(cLTmpBuf));
	sprintf(cLTmpBuf, "贷记笔数%3d", TotalsData.CupTotalVoidCnt);
	memset(cRTmpBuf, 0, sizeof(cRTmpBuf));
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_AmountAsc2String(TotalsData.CupTotalVoidAmt, CURRENCY_NAME, '+',  cTmpBuf);
	sprintf(cRTmpBuf, "贷记金额%11s", cTmpBuf);
    PrnDisp_PrintStringLeftRightAlign(cLTmpBuf, 2, cRTmpBuf, 2);
	
	
	// ------
    PrnDisp_PrintString("外卡总计:", 3, TRUE);
	
	memset(cLTmpBuf, 0, sizeof(cLTmpBuf));
	sprintf(cLTmpBuf, "借记笔数%3d", TotalsData.FgnTotalSaleCnt);
	memset(cRTmpBuf, 0, sizeof(cRTmpBuf));
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_AmountAsc2String(TotalsData.FgnTotalSaleAmt, CURRENCY_NAME, '+',  cTmpBuf);
	sprintf(cRTmpBuf, "借记金额%11s", cTmpBuf);
    PrnDisp_PrintStringLeftRightAlign(cLTmpBuf, 2, cRTmpBuf, 2);
	
	// ------
	memset(cLTmpBuf, 0, sizeof(cLTmpBuf));
	sprintf(cLTmpBuf, "贷记笔数%3d", TotalsData.FgnTotalVoidCnt);
	memset(cRTmpBuf, 0, sizeof(cRTmpBuf));
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_AmountAsc2String(TotalsData.FgnTotalVoidAmt, CURRENCY_NAME, '+',  cTmpBuf);
	sprintf(cRTmpBuf, "贷记金额%11s", cTmpBuf);
    PrnDisp_PrintStringLeftRightAlign(cLTmpBuf, 2, cRTmpBuf, 2);
#endif
	
	PrnDisp_Trail();
	
	Print_CheckPrintFinish();
	
	AppUtils_ClosePrinter();
	
	return BANK_OK;
}


int PrnDisp_PrintSettleDetail(void *dummy)
{
//  int iChoice = 1;
	byte bKey;
//  if(AppUtils_YesOrNo("是否打印明细", "      0-否  1-是", &iChoice) != BANK_OK)
//    return BANK_OK;

//  if(iChoice != 1)
//    return BANK_OK;

	AppUtils_UserClear(LINE2);
	AppUtils_DisplayCenter("是否打印明细", LINE2, FALSE);
	AppUtils_DisplayCenter("0-否    1-是", LINE3, FALSE);
	
	while(TRUE)
	{

		
		if(bKey == 0 || bKey ==  bESC || bKey == bKEY0)
			return BANK_FAIL;
		else if(bKey == bKEY1 || bKey == bENTER)
			break;
	}
	
	PrnDisp_PrintJournal((void *) 3);
	
	Print_CheckPrintFinish();
	
	return BANK_OK;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   PrnDisp_PrintUserData
 * DESCRIPTION:     Print data by user need
 *                  bStringMode =PRINTMODE_STRING, print by orignial buffer string
 *                              =PRINTMODE_HEX, print by string converted to hex mode
 * RETURN: none
 ---------------------------------------------------------------------------- */
void PrnDisp_PrintUserData(byte bStringMode, char *pPrintBuf, int iPrintLength)
{
	;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   PrnDisp_PrintTermCfg
 * DESCRIPTION:     打印终端参数
 *
 *
 * RETURN:
 ---------------------------------------------------------------------------- */
int PrnDisp_PrintTermCfg(void *dummy)
{
	char cTmpBuf[ 56 ];
	int iRet;
	unsigned char  bMenuOption;
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "商户信息",   NULL, (void *) 0 },
		{ NULL,    2, "交易控制",   NULL, (void *) 0 },
		{ NULL,    3, "系统控制",   NULL, (void *) 0 },
		{ NULL,    4, "通讯参数",   NULL, (void *) 0 },
		{ NULL,    5, "版本信息",   NULL, (void *) 0 },
		{ NULL,    6, "其他",       NULL, (void *) 0 },
	};
	
	bMenuOption = MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	
	AppUtils_OpenPrinter();
	
	while(1)
	{
		iRet = PrnDisp_DisplayMenu(LINE1, "", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
		
		if(iRet >= 1 && iRet <= 6)
		{
			AppUtils_Cls();
			AppUtils_DisplayCenter("打印终端参数", LINE1 , TRUE);
			AppUtils_DisplayCenter("正在打印...", LINE3, FALSE);
			
			PrnDisp_SepLine();
            PrnDisp_PrintStringCenterAlign("打印终端参数", 2 | 3);
		}
		
		switch(iRet)
		{
		case 1: //商户参数
            PrnDisp_PrintStringCenterAlign("******商户参数******", 1);
			
            PrnDisp_PrintString("商户名称:", 1, TRUE);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%s", gTermCfg.mFixPrm.MerchantNameChn);
            PrnDisp_PrintString(cTmpBuf, 1, TRUE);
			
			if(strlen((char *)gTermCfg.mFixPrm.MerchantNameEng) > 0)
			{
				sprintf(cTmpBuf, "%s", gTermCfg.mFixPrm.MerchantNameEng);
                PrnDisp_PrintString(cTmpBuf, 1, TRUE);
			}
			
            PrnDisp_PrintStringLeftRightAlign("商户编号:", 1, (char *)gTermCfg.mFixPrm.MerchantID, 1);
			
            PrnDisp_PrintStringLeftRightAlign("终端编号:", 1, (char *)gTermCfg.mFixPrm.TerminalID, 1);
			

//        memset(cTmpBuf, 0, sizeof(cTmpBuf));
//        sprintf(cTmpBuf, "%s-%s", AppUtils_GetTermModuleName(), MultHead_GetReleaseVersion());
//        PrnDisp_PrintStringLeftRightAlign("程序版本号:", 1, cTmpBuf, 1);
			break;
		case 2: //交易控制参数
            PrnDisp_PrintStringCenterAlign("******交易控制参数******", 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.Sale_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持消费:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.Void_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.Balance_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持查询余额:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.Refund_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持退货:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.PreAuth_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持预授权:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.PreAuthVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持预授权撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.CompeleteReq_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持预授权完成请求:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.CompeleteNote_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持预授权完成通知:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.CompeleteVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持预授权完成撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OfflineSettle_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持离线结算:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.SettleAdjust_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持结算调整:", 1, cTmpBuf, 1);
			
			//电子现金类
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.ECash_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子现金:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EcashInLoad_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子现金指定账户圈存:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EcashOutLoad_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子现金非指定账户圈存:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EcashLoad_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子现金现金充值:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EcashLoadVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子现金现金充值撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EcRefund_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子现金脱机退货:", 1, cTmpBuf, 1);
			
			//电子钱包类(本应用其实不支持)
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EPSale, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子钱包消费:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EPInLoad, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子钱包指定账户圈存:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EPOutLoad, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子钱包非指定账户圈存:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EPCashLoad, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持电子钱包现金圈存:", 1, cTmpBuf, 1);
			
			//分期类
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.IntallSale_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持分期付款消费:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.IntallVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持分期付款消费撤销:", 1, cTmpBuf, 1);
			
			//积分类
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.UnionScoreSale_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持联盟积分消费:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.IssuerScoreSale_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持发卡行积分消费:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.UnionScoreVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持联盟积分消费撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.IssuerScoreVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持发卡行积分消费撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.UnionScoreBalance_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持联盟积分查询:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.UnionScoreRefund_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持联盟积分退货:", 1, cTmpBuf, 1);
			
			//手机芯片类
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipSale_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机消费:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机消费撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipRefund_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机退货:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipPreAuth_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机预授权:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipPreAuthVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机预授权撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipCompleteReq_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机预授权完成请求:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipCompleteNote_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机预授权完成通知:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipCompleteVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机预授权完成撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MChipBalance_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手机余额查询:", 1, cTmpBuf, 1);
			
			//预约类
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.YuyueSale_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持预约消费:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.YuyueVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持预约消费撤销:", 1, cTmpBuf, 1);
			
			//订购类
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OrderSale_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持订购消费:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OrderVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持订购消费撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OrderRefund_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持订购退货:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OrderPreAuth_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持订购预授权:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OrderPreAuthVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持订购预授权撤销:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OrderCompleteReq_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持订购预授权完成请求:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OrderCompleteNote_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持订购预授权完成通知:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OrderCompleteVoid_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持订购预授权完成撤销:", 1, cTmpBuf, 1);
			
			//这是2014送检程序新增交易

			//磁条卡充值
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MagCashLoad_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持磁条现金充值:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MagAccountLoad_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持磁条账户充值:", 1, cTmpBuf, 1);
			
			//其他业务开关
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.AutoLogoutFlag, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("结算是否自动签退:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.DetailPrintFlag, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("结算是否提示打印明细:", 1, cTmpBuf, 1);
			
			//离线上送方式目前都改为了联机后
			//if( gTermCfg.mFixPrm.OfflineUploadMode == 0 )
            PrnDisp_PrintStringLeftRightAlign("离线上送方式:", 1, "联机后", 1);
			//else
            //  PrnDisp_PrintStringLeftRightAlign( "离线上送方式:", 1, "结算前", 1 );
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.OffUploadNumLimit);
            PrnDisp_PrintStringLeftRightAlign("离线自动上送笔数:", 1, cTmpBuf, 1);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.UploadMaxTimes);
            PrnDisp_PrintStringLeftRightAlign("离线上送次数:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.MngPwdRequired, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否输入主管密码:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.ManualFlag, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持手输卡号:", 1, cTmpBuf, 1);
			
#if 1   //银商暂时没要求改
			
			if(gTermCfg.mFixPrm.DefaultTransType == 1)
                PrnDisp_PrintStringLeftRightAlign("默认刷卡交易:", 1, "消费", 1);
			else
                PrnDisp_PrintStringLeftRightAlign("默认刷卡交易:", 1, "预授权", 1);
				
#else //Modify on 2015-07-17 for 银联改为1表示消费 2表示预授权
				
			if(gTermCfg.mFixPrm.DefaultTransType == 2)
                PrnDisp_PrintStringLeftRightAlign("默认刷卡交易:", 1, "预授权", 1);
			else
                PrnDisp_PrintStringLeftRightAlign("默认刷卡交易:", 1, "消费", 1);
				
#endif
				
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			AppUtils_AmountAsc2String(gTermCfg.mFixPrm.RefundAmt, CURRENCY_NAME, '+', cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("退货限额:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.SmallAmountStandinAuth, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持小额代授权:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.CTLS_Support, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持非接卡:", 1, cTmpBuf, 1);
			break;
			
		case 3: //系统控制参数
            PrnDisp_PrintStringCenterAlign("******系统控制参数******", 1);
			sprintf(cTmpBuf, "%06lu", gTermCfg.mDynPrm.SysTrace);
            PrnDisp_PrintStringLeftRightAlign("当前流水号:", 1, cTmpBuf, 1);
			
			sprintf(cTmpBuf, "%06lu", gTermCfg.mDynPrm.BatchNum);
            PrnDisp_PrintStringLeftRightAlign("当前批次号:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.AccquChnName, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否打印中文收单行:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.IssueChnName, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否打印中文发卡行:", 1, cTmpBuf, 1);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.Receipt);
            PrnDisp_PrintStringLeftRightAlign("打印单据联数:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.EnglishSlip, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("签购单是否打英文:", 1, cTmpBuf, 1);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.ReversalRetryTimes);
            PrnDisp_PrintStringLeftRightAlign("冲正重试次数:", 1, cTmpBuf, 1);
			
			sprintf(cTmpBuf, "%u", gTermCfg.mFixPrm.MaxTranNumber);
            PrnDisp_PrintStringLeftRightAlign("最大交易笔数:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.FeeFlag, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持小费:", 1, cTmpBuf, 1);
			
			if(gTermCfg.mTrSwitch.FeeFlag)
			{
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				AppUtils_Bcd2Asc(&gTermCfg.mFixPrm.FeePercent, (byte *)cTmpBuf, 2);
                PrnDisp_PrintStringLeftRightAlign("小费百分比:", 1, cTmpBuf, 1);
			}
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.TrackEncryptFlag, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持磁道加密:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.PartPurchaseFlag, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("是否支持部分承兑:", 1, cTmpBuf, 1);
//        memset(cTmpBuf, 0, sizeof(cTmpBuf));
//        AppUtils_Bcd2Asc(gTermCfg.mFixPrm.TerminalCap, (byte *)cTmpBuf, 6);
//        PrnDisp_PrintStringLeftRightAlign("终端能力:", 1, cTmpBuf, 1);
//
//        memset(cTmpBuf, 0, sizeof(cTmpBuf));
//        AppUtils_Bcd2Asc(gTermCfg.mFixPrm.AddTerminalCap, (byte *)cTmpBuf, 10);
//        PrnDisp_PrintStringLeftRightAlign("终端附加能力:", 1, cTmpBuf, 1);

//        memset(cTmpBuf, 0, sizeof(cTmpBuf));
//          AppUtils_AmountAsc2String(gTermCfg.mFixPrm.EC_Limit, CURRENCY_NAME, '+', cTmpBuf);
//          PrnDisp_PrintStringLeftRightAlign("电子现金限额:", 1, cTmpBuf, 1);
//
//          memset(cTmpBuf, 0, sizeof(cTmpBuf));
//          AppUtils_AmountAsc2String(gTermCfg.mFixPrm.OffLine_CTLS_Limit, CURRENCY_NAME, '+', cTmpBuf);
//          PrnDisp_PrintStringLeftRightAlign("脱机非接限额:", 1, cTmpBuf, 1);
//
//          memset(cTmpBuf, 0, sizeof(cTmpBuf));
//          AppUtils_AmountAsc2String(gTermCfg.mFixPrm.CTLS_Limit, CURRENCY_NAME, '+', cTmpBuf);
//          PrnDisp_PrintStringLeftRightAlign("非接限额:", 1, cTmpBuf, 1);
//
//          memset(cTmpBuf, 0, sizeof(cTmpBuf));
//          AppUtils_AmountAsc2String(gTermCfg.mFixPrm.CVM_Limit, CURRENCY_NAME, '+', cTmpBuf);
//          PrnDisp_PrintStringLeftRightAlign("CVM限额:", 1, cTmpBuf, 1);
			break;
			
		case 4:
            PrnDisp_PrintStringCenterAlign("******通讯控制参数******", 1);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			
			switch(gTermCfg.mCommPrm.CurrentCommMode)
			{
			case COMM_TYPE_SDLC:
				strcat(cTmpBuf, "拨号");
				break;
			case COMM_TYPE_RS232:
				strcat(cTmpBuf, "串口");
				break;
			case COMM_TYPE_GPRS:
				strcat(cTmpBuf, "GPRS");
				break;
			case COMM_TYPE_ETHERNET:
				strcat(cTmpBuf, "以太网");
				break;
#ifdef TRIDENT
			case COMM_TYPE_CDMA:
				strcat(cTmpBuf, "CDMA");
				break;
			case COMM_TYPE_WIFI:
				strcat(cTmpBuf, "WIFI");
				break;
#endif
			}
			
            PrnDisp_PrintStringLeftRightAlign("通讯方式:", 1, cTmpBuf, 1);
			
			AppUtils_Bcd2Asc(gTermCfg.mFixPrm.TPDU, (byte *)cTmpBuf, 10);
            PrnDisp_PrintStringLeftRightAlign("TPDU:", 1, cTmpBuf, 1);
			
			AppUtils_Bcd2Asc(gTermCfg.mFixPrm.Header, (byte *)cTmpBuf, 12);
            PrnDisp_PrintStringLeftRightAlign("HEADER:", 1, cTmpBuf, 1);
			
			switch(gTermCfg.mCommPrm.CurrentCommMode)
			{
			case COMM_TYPE_GPRS:
			case COMM_TYPE_ETHERNET:
#ifdef TRIDENT
			case COMM_TYPE_WIFI:
			case COMM_TYPE_CDMA:
#endif
                PrnDisp_PrintStringLeftRightAlign("本机IP地址:", 1, (char *)gTermCfg.mCommPrm.LocalIP, 1);
                PrnDisp_PrintStringLeftRightAlign("子网掩码:", 1, (char *)gTermCfg.mCommPrm.SubNetMask, 1);
                PrnDisp_PrintStringLeftRightAlign("网关:", 1, (char *)gTermCfg.mCommPrm.GateWay, 1);
                PrnDisp_PrintStringLeftRightAlign("DNS1:", 1, (char *)gTermCfg.mCommPrm.PriDNS, 1);
                PrnDisp_PrintStringLeftRightAlign("DNS2:", 1, (char *)gTermCfg.mCommPrm.SecDNS, 1);
				break;
			default:
				break;
			}
			
			switch(gTermCfg.mCommPrm.CurrentCommMode)
			{
			case COMM_TYPE_SDLC:
				inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.IsPreDialFlag, cTmpBuf);
                PrnDisp_PrintStringLeftRightAlign("是否支持预拨号:", 1, cTmpBuf, 1);
				
                PrnDisp_PrintStringLeftRightAlign("分机前缀:", 1, (char *)gTermCfg.mCommPrm.TelPrefix, 1);
				
                PrnDisp_PrintStringLeftRightAlign("电话号码1:", 1, (char *)gTermCfg.mCommPrm.TelPhone[ 0 ], 1);
				
                PrnDisp_PrintStringLeftRightAlign("电话号码2:", 1, (char *)gTermCfg.mCommPrm.TelPhone[ 1 ], 1);
				
                PrnDisp_PrintStringLeftRightAlign("电话号码3:", 1, (char *)gTermCfg.mCommPrm.TelPhone[ 2 ], 1);
				break;
			case COMM_TYPE_ETHERNET:
                PrnDisp_PrintStringLeftRightAlign("主机IP地址1:", 1, (char *)gTermCfg.mCommPrm.EthHostIP1, 1);
				
				sprintf(cTmpBuf, "%u", gTermCfg.mCommPrm.EthHostPort1);
                PrnDisp_PrintStringLeftRightAlign("主机端口1:", 1, cTmpBuf, 1);
				
                PrnDisp_PrintStringLeftRightAlign("主机IP地址2:", 1, (char *)gTermCfg.mCommPrm.EthHostIP2, 1);
				
				sprintf(cTmpBuf, "%u", gTermCfg.mCommPrm.EthHostPort2);
                PrnDisp_PrintStringLeftRightAlign("主机端口2:", 1, cTmpBuf, 1);
				break;
			case COMM_TYPE_GPRS:
                PrnDisp_PrintStringLeftRightAlign("GPRS拨入号码:", 1, (char *)gTermCfg.mCommPrm.GprsPhoneNo, 1);
                PrnDisp_PrintStringLeftRightAlign("GPRS接入点:", 1, (char *)gTermCfg.mCommPrm.GprsAPN, 1);
                PrnDisp_PrintStringLeftRightAlign("GPRS用户名:", 1, (char *)gTermCfg.mCommPrm.GprsUserName, 1);
                PrnDisp_PrintStringLeftRightAlign("GPRS密码:", 1, (char *)gTermCfg.mCommPrm.GprsUserPwd, 1);
				
                PrnDisp_PrintStringLeftRightAlign("主机IP地址1:", 1, (char *)gTermCfg.mCommPrm.GprsHostIP1, 1);
				
				sprintf(cTmpBuf, "%u", gTermCfg.mCommPrm.GprsHostPort1);
                PrnDisp_PrintStringLeftRightAlign("主机端口1:", 1, cTmpBuf, 1);
				
                PrnDisp_PrintStringLeftRightAlign("主机IP地址2:", 1, (char *)gTermCfg.mCommPrm.GprsHostIP2, 1);
				
				sprintf(cTmpBuf, "%u", gTermCfg.mCommPrm.GprsHostPort2);
                PrnDisp_PrintStringLeftRightAlign("主机端口2:", 1, cTmpBuf, 1);
				break;
#ifdef TRIDENT
			case COMM_TYPE_CDMA:
                PrnDisp_PrintStringLeftRightAlign("CDMA拨入号码:", 1, (char *)gTermCfg.mCommPrm.CdmaPhoneNo, 1);
                PrnDisp_PrintStringLeftRightAlign("CDMA用户名:", 1, (char *)gTermCfg.mCommPrm.CdmaUserName, 1);
                PrnDisp_PrintStringLeftRightAlign("CDMA密码:", 1, (char *)gTermCfg.mCommPrm.CdmaUserPwd, 1);
				sprintf(cTmpBuf, "%u", gTermCfg.mCommPrm.CdmaAuthType);
                PrnDisp_PrintStringLeftRightAlign("CDMA认证方式:", 1, cTmpBuf, 1);
				
                PrnDisp_PrintStringLeftRightAlign("主机IP地址1:", 1, (char *)gTermCfg.mCommPrm.CdmaHostIP1, 1);
				
				sprintf(cTmpBuf, "%d", gTermCfg.mCommPrm.CdmaHostPort1);
                PrnDisp_PrintStringLeftRightAlign("主机端口1:", 1, cTmpBuf, 1);
				
                PrnDisp_PrintStringLeftRightAlign("主机IP地址2:", 1, (char *)gTermCfg.mCommPrm.CdmaHostIP2, 1);
				
				sprintf(cTmpBuf, "%d", gTermCfg.mCommPrm.CdmaHostPort2);
                PrnDisp_PrintStringLeftRightAlign("主机端口2:", 1, cTmpBuf, 1);
				break;
			case COMM_TYPE_WIFI:
                PrnDisp_PrintStringLeftRightAlign("主机IP地址1:", 1, (char *)gTermCfg.mCommPrm.WifiHostIP1, 1);
				
				sprintf(cTmpBuf, "%d", gTermCfg.mCommPrm.WifiHostPort1);
                PrnDisp_PrintStringLeftRightAlign("主机端口1:", 1, cTmpBuf, 1);
				
                PrnDisp_PrintStringLeftRightAlign("主机IP地址2:", 1, (char *)gTermCfg.mCommPrm.WifiHostIP2, 1);
				
				sprintf(cTmpBuf, "%d", gTermCfg.mCommPrm.WifiHostPort2);
                PrnDisp_PrintStringLeftRightAlign("主机端口2:", 1, cTmpBuf, 1);
#endif
			default:
				break;
			}
			
			// 公网参数
			if(Comm_IsTCPIPCommMode(gTermCfg.mCommPrm.CurrentCommMode))
			{
				inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.SSLCommFlag, cTmpBuf);
                PrnDisp_PrintStringLeftRightAlign("是否支持公网接入:", 1, cTmpBuf, 1);
				
				if(gTermCfg.mTrSwitch.SSLCommFlag)
				{
					inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.OnewayAuthSSL, cTmpBuf);
                    PrnDisp_PrintStringLeftRightAlign("是否支持单向认证:", 1, cTmpBuf, 1);
					

					inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.UseDNSFlag, cTmpBuf);
                    PrnDisp_PrintStringLeftRightAlign("是否支持DNS域名解析:", 1, cTmpBuf, 1);
					
					if(gTermCfg.mTrSwitch.UseDNSFlag)
					{
                        PrnDisp_PrintStringLeftRightAlign("SSL域名:", 1, gTermCfg.mCommPrm.SSLSvrDomainName1, 1);
                        PrnDisp_PrintStringLeftRightAlign("SSL备用域名:", 1, gTermCfg.mCommPrm.SSLSvrDomainName2, 1);
                        PrnDisp_PrintStringLeftRightAlign("SSL主DNS:", 1, gTermCfg.mCommPrm.SSLDnsIP1, 1);
                        PrnDisp_PrintStringLeftRightAlign("SSL从DNS:", 1, gTermCfg.mCommPrm.SSLDnsIP2, 1);
						
						memset(cTmpBuf, 0, sizeof(cTmpBuf));
						sprintf(cTmpBuf, "%d", gTermCfg.mCommPrm.SSLHostPort1);
                        PrnDisp_PrintStringLeftRightAlign("SSL端口1:", 1, cTmpBuf, 1);
						
						memset(cTmpBuf, 0, sizeof(cTmpBuf));
						sprintf(cTmpBuf, "%d", gTermCfg.mCommPrm.SSLHostPort2);
                        PrnDisp_PrintStringLeftRightAlign("SSL端口2:", 1, cTmpBuf, 1);
					}
				}
			}
			
			break;
		case 5: //版本号信息
            PrnDisp_PrintStringCenterAlign("******版本号信息******", 1);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));


		case 6:
			//刷卡输密控制
            PrnDisp_PrintStringCenterAlign("******刷卡输密控制******", 1);
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.VoidSwipCard, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("消费撤销是否刷卡:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.VoidPreauthEndSwipCard, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("预授授权完成撤销是否刷卡:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.VoidPIN, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("消费撤销是否输密:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.VoidPreAuthPIN, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("预授权撤销输密码:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.VoidPreAuthEndPIN, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("授权完成撤销输密码:", 1, cTmpBuf, 1);
			
			inPrnDisp_GetTransSupportName(gTermCfg.mTrSwitch.PreAuthEndReqPIN, cTmpBuf);
            PrnDisp_PrintStringLeftRightAlign("授权完成请求输密码:", 1, cTmpBuf, 1);
			
			//密钥管理
            PrnDisp_PrintStringCenterAlign("******密钥管理******", 1);
			
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.KeyIndex);
            PrnDisp_PrintStringLeftRightAlign("主密钥索引号:", 1, cTmpBuf, 1);
			
            if(gTermCfg.mFixPrm.PP_DesType == 3)
                PrnDisp_PrintStringLeftRightAlign("密钥算法:", 1, "3DES", 1);
			else
                PrnDisp_PrintStringLeftRightAlign("密钥算法:", 1, "DES", 1);
				
			if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
                PrnDisp_PrintStringLeftRightAlign("密码键盘:", 1, "内置", 1);
			else
                PrnDisp_PrintStringLeftRightAlign("密码键盘:", 1, "外置", 1);
				
			break;
		default:
			AppUtils_ClosePrinter();
			return BANK_FAIL;
		}
		
		PrnDisp_Trail();
		Print_CheckPrintFinish();
	}
	
//  return BANK_OK;
}


void PrnDisp_PrintAIDParameter(void)
{
    return ;
}
void PrnDisp_PrintCAKeyParameter(void)
{
    return ;
}
int PrnDisp_PrintPbocParameter(void *dummy)
{
	if(dummy == (void *)1)
		PrnDisp_PrintAIDParameter();
	else
		PrnDisp_PrintCAKeyParameter();
		
	return BANK_OK;
}


void PrnDisp_PrintICDebugData(DynData *pTxnData)
{
	char cTmpBuf[ 256 ];
	
	if(AppDebug_CheckAppDebugStatus(APPDEBUGMODE_EMVTAGDATA) != BANK_OK)
		return;
		
	AppUtils_OpenPrinter();
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));

    PrnDisp_PrintString("EMVToolsVersion:", 1, FALSE);
    PrnDisp_PrintString((char *)cTmpBuf + 100, 1, TRUE);
	
    PrnDisp_PrintString("------------------------", 1, TRUE);
	
    PrnDisp_PrintString("终端性能:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.TermCAP, (byte *)cTmpBuf, 6);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("终端附加性能:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.AddTermCAP, (byte *)cTmpBuf, 10);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "卡序号:%02x", pTxnData->TransDB.SequenceNum);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("专用文件名称AID:", 1, FALSE);
	AppUtils_Bcd2Asc(pTxnData->TransDB.AID, (byte *)cTmpBuf, pTxnData->TransDB.AIDLength * 2);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
	sprintf(cTmpBuf, "卡密钥索引:%02x", pTxnData->TransDB.PKindex);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("应用交互特征AIP:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.AIP, (byte *)cTmpBuf, 4);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("随机数:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.RANDOM, (byte *)cTmpBuf, 8);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("应用标签:", 1, FALSE);
    PrnDisp_PrintString((char *)pTxnData->TransDB.AppLabel, 1, TRUE);
	
    PrnDisp_PrintString("应用首选名称:", 1, FALSE);
    PrnDisp_PrintString((char *)pTxnData->TransDB.AppPrefName, 1, TRUE);
	
    PrnDisp_PrintString("二磁道数据:", 1, FALSE);
    PrnDisp_PrintString((char *)pTxnData->TransDB.Track2Data, 1, TRUE);
	
    PrnDisp_PrintString("卡号:", 1, FALSE);
    PrnDisp_PrintString((char *)pTxnData->TransDB.PAN, 1, TRUE);
	
 PrnDisp_PrintString("应用交易计数器ATC:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.ATC, (byte *)cTmpBuf, 4);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("货币代码:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.CurrCode, (byte *)cTmpBuf, 4);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("国家代码:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.CountryCode, (byte *)cTmpBuf, 4);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("应用版本号:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.AppVer, (byte *)cTmpBuf, 4);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
	if(pTxnData->EC_9F7AFlag == 'A')
        PrnDisp_PrintString("9F7A标识:未获取", 1, TRUE);
	else if(pTxnData->EC_9F7AFlag == 1)
        PrnDisp_PrintString("9F7A标识:1", 1, TRUE);
	else
        PrnDisp_PrintString("9F7A标识:0", 1, TRUE);
		
	if(pTxnData->TransDB.EC_flag == 1)
	{
        PrnDisp_PrintString("EC发卡行授权码:", 1, FALSE);
        PrnDisp_PrintString((char *)pTxnData->TransDB.EC_IssAuthID, 1, TRUE);
		
        PrnDisp_PrintString("EC电子现金余额:", 1, FALSE);
		AppUtils_AmountAsc2String(pTxnData->TransDB.EC_Balance, "", '+', cTmpBuf);
        PrnDisp_PrintString(cTmpBuf, 1, TRUE);
		
        PrnDisp_PrintString("EC重置阈值:", 1, FALSE);
		AppUtils_AmountAsc2String(pTxnData->TransDB.EC_Reset_Threshold, "", '+', cTmpBuf);
        PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	}
	
	// -----------------------------------------------
    PrnDisp_PrintString("交易证书TC:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.TC, (byte *)cTmpBuf, 16);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("应用信息数据CI:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(&pTxnData->TransDB.CryptInfo, (byte *)cTmpBuf, 2);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("终端验证结果TVR:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.TVR, (byte *)cTmpBuf, 10);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("TSI:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.TSI, (byte *)cTmpBuf, 4);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("发卡行应用数据:", 1, FALSE);
	
	if(pTxnData->TransDB.IADLength > 0)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		memcpy(cTmpBuf, pTxnData->TransDB.IAD, pTxnData->TransDB.IADLength);
		AppUtils_Bcd2Asc(pTxnData->TransDB.IAD, (byte *)cTmpBuf, pTxnData->TransDB.IADLength * 2);
        PrnDisp_PrintString(cTmpBuf, 1, FALSE);
	}
	
    PrnDisp_PrintString("\n", 1, FALSE);
	
    PrnDisp_PrintString("持卡人验证结果:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	AppUtils_Bcd2Asc(pTxnData->TransDB.CVMResult, (byte *)cTmpBuf, 6);
    PrnDisp_PrintString(cTmpBuf, 1, TRUE);
	
    PrnDisp_PrintString("发卡行认证数据91:", 1, FALSE);
	
	if(pTxnData->TransDB.IssAuthDataLength > 0)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_Bcd2Asc(pTxnData->TransDB.IssAuthData, (byte *)cTmpBuf, pTxnData->TransDB.IssAuthDataLength * 2);
        PrnDisp_PrintString(cTmpBuf, 1, FALSE);
	}
	
    PrnDisp_PrintString("\n", 1, FALSE);
	
    PrnDisp_PrintString("脚本执行结果:", 1, FALSE);
	
	if(pTxnData->TransDB.IssSriptResultDataLength > 0)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		AppUtils_Bcd2Asc(pTxnData->TransDB.IssSriptResultData, (byte *)cTmpBuf, pTxnData->TransDB.IssSriptResultDataLength * 2);
        PrnDisp_PrintString(cTmpBuf, 1, FALSE);
	}
	
    PrnDisp_PrintString("\n", 1, FALSE);
	
    PrnDisp_PrintString("IAC Default:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
     PrnDisp_PrintString(cTmpBuf + 10, 1, TRUE);
	
    PrnDisp_PrintString("IAC Denial:", 1, FALSE);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));

	PrnDisp_PrintFormFeed();
	
	AppUtils_ClosePrinter();
	return;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_PrintWidthHEXASC
 * DESCRIPTION:   打印hex-asc信息
 * PARAMETERS:    header: title of the data
 *                buf: data to be printed
 *                len: length of the data
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_PrintWidthHEXASC(char *header, unsigned char *buf, int len)
{
	unsigned char *p, *end, HexPos, AscPos;
	char PrntBuf[60], TempBuf[12];
	unsigned short Cycle, j, col;
	const unsigned short Position[8] = {0x001F, 0x0320, 0x0621, 0x0922, 0x0E23, 0x1124, 0x1425, 0x1726};
	
	memset(PrntBuf, 0, sizeof(PrntBuf));
	
	/* 计算该批数据占用的行数 */
	if(len % 8 == 0)
	{
		col = len / 8;
	}
	else
	{
		col = (len / 8) + 1;
	}
	
	/* 在标题中加上行数 */
	sprintf(PrntBuf, "%s (%d Columns) len=%d \n", header, col, len);
	
	AppUtils_OpenPrinter();
	
	/* 打印数据的标题 */
	if(header != NULL)
	{
        PrnDisp_PrintString(PrntBuf, 1, FALSE);
	}
	
	p = buf;
	end = p + len;
	
	while(1)
	{
		memset(PrntBuf, 0x20, sizeof(PrntBuf));
		PrntBuf[40] = 0;
		
		/* 处理待打印数据不是8的整数倍的情况 */
		if((p + 8) > end)
		{
			Cycle = len % 8;
		}
		else
		{
			Cycle = 8;
		}
		
		for(j = 0; j < Cycle; j++)
		{
			memset(TempBuf, 0, sizeof(TempBuf));
			sprintf(TempBuf, "%02X", p[j]);
			HexPos = (unsigned char)((Position[j] & 0xFF00) >> 8);
			AscPos = (unsigned char)(Position[j] & 0x00FF);
			memcpy(&PrntBuf[HexPos], TempBuf, 2);
			
			/* 判断是否可打印字符 */
			if(!(p[j] & 0x80) && (p[j] & 0x60))
			{
				PrntBuf[AscPos] = p[j];
			}
			else
			{
				PrntBuf[AscPos] = '.';
			}
		}
		
        PrnDisp_PrintString(PrntBuf, 1, TRUE);
		
		p += 8;
		
		/* 已经到达或超过待打印数据的末端,退出循环 */
		if(p >= end)
		{
			break;
		}
	}
	
	PrnDisp_PrintFormFeed();
	AppUtils_ClosePrinter();
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_DisplayAdInfo
 * DESCRIPTION:   显示广告信息，每次调用都加一个汉字
 *                四行机型把时间接到广告信息后，
 *                大屏机型与时间不显示在同一行，不需要拼接时间
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_DisplayAdInfo(void)
{
	char szAdverPrompt[200 + 1], szCurrDateTime[32];
	char szDispBuffer[20 + 1], szTempBuf[16 + 1];
	static int iDispOffset;
	

	
	memset(szAdverPrompt, 0, sizeof(szAdverPrompt));
	strcpy(szAdverPrompt, gTermCfg.mFixPrm.AdverInfo);
	
	if(AppUtils_IsBigScreen() != BANK_OK && AppUtils_IsRotatedScreen() != BANK_OK)
	{
		memset(szCurrDateTime, 0, sizeof(szCurrDateTime));
		AppUtils_GetSysDateTime(szCurrDateTime);
		strcat(szAdverPrompt, szCurrDateTime);
	}
	
	if(iDispOffset >= strlen(szAdverPrompt))
		iDispOffset = 0;
		
	//拷贝本次显示的信息
//  if(AppUtils_IsRotatedScreen() == BANK_OK)
//  {
//    memset(szDispBuffer, 0, sizeof(szDispBuffer));
//    BankUtil_CStrlcpy(szDispBuffer, szAdverPrompt + iDispOffset, 22 + 1);
//  }
//  else
//  {
//    memset(szDispBuffer, 0, sizeof(szDispBuffer));
//    BankUtil_CStrlcpy(szDispBuffer, szAdverPrompt + iDispOffset, 16 + 1);
//  }
	//向下偏移一个汉字的位置供下次拷贝显示信息使用, 使用带保护的复制函数
	memset(szTempBuf, 0, sizeof(szTempBuf));
    if(AppUtils_IsColorScreen() == BANK_OK)
	{
		if(AppUtils_IsRotatedScreen() == BANK_OK)
		{
			AppUtils_ClearLine(LINE4);
			AppUtils_DisplayLine(szDispBuffer, LINE4);
		}
		else
		{
			AppUtils_ClearLine(LINE6);
			AppUtils_DisplayLine(szDispBuffer, LINE6);
		}
	}
	else
	{
		if(AppUtils_IsBigScreen() == BANK_OK)
		{
			//八行大屏显示在第六行，不影响原时间日期的显示
			AppUtils_ClearLine(LINE6);
			AppUtils_DisplayLine(szDispBuffer, LINE6);
		}
		else
		{
			//四行屏幕显示在第四行，原来的时间日期被覆盖

			AppUtils_ClearLine(LINE4);
			AppUtils_DisplayLine(szDispBuffer, LINE4);
		}
	}
	
	return 0;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_CheckAdInfo
 * DESCRIPTION:   检测是否存在广告信息
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_CheckAdInfo(void)
{
	if(strlen(gTermCfg.mFixPrm.AdverInfo) > 0)
	{
		return 1;
	}
	
	return 0;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrnDisp_GetBankName
 * DESCRIPTION:   根据银行代码得到银行名称
 * PARAMETERS:    pBankCode: 银行代码   pBankName: 银行名称
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrnDisp_GetBankName(byte *pBankCode, char *pBankName)
{
	char szBankCode[4 + 1];
	
	if(strlen((char *)pBankCode) < 4)
		return BANK_FAIL;
		
	memset(szBankCode, 0, sizeof(szBankCode));
	memcpy(szBankCode, pBankCode, 4);
	
	//从配置文件中读取银行代码对应的银行名称
	if(AppUtils_GetFileString(BANK_FILE, "BANK_NAME", szBankCode, pBankName) > 0)
	{
		return BANK_OK;
	}
	else if(memcmp(gTermCfg.mFixPrm.LocalBank, szBankCode, 4) == 0)
	{
		//如果配置文件中未找到，则匹配本地银行代码，对应银行名称为"商业银行"
		strcpy(pBankName, "商业银行");
		return BANK_OK;
	}
	else
	{
		//没有匹配到中文名字，直接复制发卡行代码(暂不用)
		//strcpy(pBankName, (char*)pBankCode);
		return BANK_FAIL;
	}
}


int PrnDisp_InitShowTimeCount(int TimeSec)
{

	return 0;
}

/**<阻塞时显示的界面信息*/
void PrnDisp_CommShowTimeCount(void)
{
	char cDispBuf[32] = {0};
	unsigned long ulTimeInterval;
	static int iShowTime = -1;
	

	if(iShowTime != (giTimeOut - ulTimeInterval / 1000))
	{
		iShowTime = (giTimeOut - ulTimeInterval / 1000);
		
		if(iShowTime > 0)
		{
			sprintf(cDispBuf, "请稍候... %d", iShowTime - 1);
			AppUtils_DisplayLine(cDispBuf, LINE3);
			AppUtils_DisplayLine("接收数据包", LINE4);
		}
	}
	
//    nShowTime = Sys_TimerCheck(gnTimerHdl);
//    if((nShowTime/1000) != (nOldTime/1000)){
//      sprintf(szBuf, "请稍候...  %d", nShowTime / 1000);
//    if (nShowTime > 0)
//    {
//      Disp_ClrLine(3, 0);
//      Disp_Str(szBuf, 3, 0, 1);
//    }
//    }
//  nOldTime = nShowTime;
}

#if 0       //暂未使用
/*
　检查是否银联，还是本地
  当发卡行代码的第5，6两位不等于该参数前两位,
  且前4位为15家全国性商业银行的代码时，签购单
  打印完发卡行简称后(简称见附录2,考虑到发卡行位置长度有限)，
  再空一格打印“银联”，表示银行卡受理机构为银联。
  例如:该参数为“8810”,而发卡行代码为“01027900”,
  则签购单上发卡行名称打印为”工行 银联”.
  该参数默认值为“0000”，
  当该参数为默认值时，直接打印发卡行名称(中文名称见附录2)
  ,不论发卡行代码第5，6两位为何值,都不打印”银联”字样
*/
int inPrnDisp_CheckLocalArea(byte *IssuerCode)
{
	//小于6位的银行代码，不处理
	if(strlen((char *)IssuerCode) < 6)
		return BANK_FAIL;
		
	//本地区域码为0000，不处理
	if(!memcmp(gTermCfg.mFixPrm.LocalArea, "0000", 4))
		return BANK_FAIL;
		
	if(!memcmp((char *)IssuerCode + 4, gTermCfg.mFixPrm.LocalArea, 2))
		return BANK_FAIL;
		
	return BANK_OK;
}
#endif

/* -----------------------------------------------------------------------------
 * Private Function define
 ---------------------------------------------------------------------------- */
static char *inPrnDisp_GetTransSupportName(int iSupportFlag, char *pRetName)
{
	if(iSupportFlag == 1)
		sprintf(pRetName, "支持");
	else
		sprintf(pRetName, "不支持");
		
	return pRetName;
}

static char inPrnDisp_GetAmountSignSymbol(byte TransType)
{
	switch(TransType)
	{
	case BTYPE_PREAUTH_VOID:
	case BTYPE_MCHIP_PREAUTH_VOID:
	case BTYPE_ORDER_PREAUTH_VOID:
	case BTYPE_REFUND:
	case BTYPE_ECASH_REFUND:
//  case BTYPE_ECASH_CASH_RELOAD: //虽然属于贷记，但是打印负号有误导
	case BTYPE_SALE_VOID:
	case BTYPE_COMPLETE_VOID:
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
		return '-';
		
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
	default:
		break;
	}
	
	return '+';
}

