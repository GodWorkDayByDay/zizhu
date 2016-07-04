/* ************************************************************************* */
/*  FILE NAME   : zontalkap.c                                                */
/*  PROGRAMMER  : Derya ENGIN                                                */
/*  DESCRIPTION : implementation of zontalkap protocol host part             */
/* ************************************************************************* */

/* ......................................................................... */
/* ... INCLUDES ............................................................ */
/* ......................................................................... */

#include "cupms.h"
#define ZONTALK_NOK 1
#define ZONTALK_OK 0
/* -----------------------------------------------------------------------------
* External variables / declarations
---------------------------------------------------------------------------- */
extern Term_Config gTermCfg;

extern char *CtlsDevName[MAX_CTLS_IF_TYPE];
char env_ver[64] = {0};

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_CHECKFILE                                      */
/*  DESCRIPTION :   查询基础文件是否存在(FLASHCLR.TXT/BASE.TXT/HANDLER.TXT)  */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_ERR_FILFIND/ZONTALK_OK                           */
/* ......................................................................... */
int  ZONTALKAP_CHECKFILE(char(*lpFileList)[64], int *lpCount)
{

	
    return 0;
}

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_CHECKUPDATA                                    */
/*  DESCRIPTION :   根据FW版本判断是否升级                                   */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_NOK/ZONTALK_OK                                   */
/* ......................................................................... */
int ZONTALKAP_CHECKUPDATA(void *dummy)
{

    return 0;
}

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_DLOADFILE                                       */
/*  DESCRIPTION :   FW下载                                                   */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_NOK/ZONTALK_OK                                   */
/* ......................................................................... */
int ZONTALKAP_DLOADFILE(char *lpFileName)
{

	

    return 0;
}

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_CallBack                                       */
/*  DESCRIPTION :   FW下载                                                   */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   none                                                     */
/* ......................................................................... */
short ZONTALKAP_CallBack(short  i_Act, void *i_Val)
{
	char *lpTile = (char *)i_Val;
	
	if(NULL != lpTile)
	{
		AppUtils_DisplayLine(lpTile, LINE2);
	}
	
	return 0;
}

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_RemoveFWFiles                                  */
/*  DESCRIPTION :   移除固件文件包                                           */
/*  INPUT       :   dummy                                                    */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_OK / ZONTALK_NOK                                 */
/* ......................................................................... */
int ZONTALKAP_RemoveFWFiles(void *dummy)
{
	char cFileList[16][64] = {0};
	int  iFileCount = 0;
	
	//
	// 查看基础文件是否存在
	//

	return ZONTALK_NOK;
}


/* ......................................................................... */
/*  NAME        :   ZONTALKAP_DOWNLOAD                                       */
/*  DESCRIPTION :   FW下载                                                   */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_NOK/ZONTALK_OK                                   */
/* ......................................................................... */
int ZONTALKAP_DOWNLOAD(void *dummy)
{
	int iRet = ZONTALK_NOK;
	int i = 0;
	int rv = 0;
	int TerminalType = 0;
//  byte bt = 0;
	char cFileList[16][64] = {0};
	int  iFileCount = 0;
	//终端型号
	TerminalType = AppUtils_GetTerminalType();
	
	switch(TerminalType)
	{
	case _VX510:
	case _VX510G:
	case _VX520:
	case _VX520S:
		break;
		
	default :
	
		if((void *)0 != dummy)
		{
			AppUtils_Warning("机型不支持下载");
		}
		
		return ZONTALK_NOK;
	}
	
	//
	// 查看基础文件是否存在
	//
	if(ZONTALKAP_CHECKFILE(cFileList, &iFileCount) != ZONTALK_OK)
	{
		if((void *)0 != dummy)
		{
			AppUtils_Warning("检查索引文件失败");
		}
		
//		return ZONTALK_ERR_FILFIND;
	}
	
	if(dummy == (void*)2)
		goto BeginDownload;
		
	//
	// 连接读卡器
	//
	AppUtils_Cls();
	AppUtils_DisplayLine("正在检测读卡器 ", LINE1);
	AppUtils_DisplayLine("请稍候 ", LINE2);
	

	
	i = 0;
	
	while(1)
	{
		i++;
		
		if(i > 3)
		{

			
			AppUtils_Cls();
			AppUtils_DisplayLine("请检查读卡器",  LINE2);
			AppUtils_DisplayLine("或者设置",  LINE3);
			AppUtils_WaitKey(5);
			return ZONTALK_NOK;
		}
		
		if(ZONTALK_PingReader(30) == ZONTALK_OK)
		{
			break;
		}
	}
	
	//
	// 根据FW版本判断是否升级
	//
	if(ZONTALKAP_CHECKUPDATA((void*)1) != ZONTALK_OK)
	{
		if((void *)0 != dummy)
		{
			AppUtils_Cls();
			AppUtils_DisplayLine("读卡器已是高版本", LINE2);
			AppUtils_DisplayLine("无需升级", LINE3);
			AppUtils_WaitKey(5);
		}
		

		
		return ZONTALK_NOK;
	}
	
BeginDownload:

	//
	// 开始升级
	//
	for(i = 0; i < iFileCount; i++)
	{
		if((dummy == (void*)2 && i == 0) || ZONTALK_PingReader(60) == ZONTALK_OK)
		{
			char cTmp[32] = {0};
			AppUtils_Cls();
			
			sprintf(cTmp, "开始下载%s", &cFileList[i][0]);
			iRet = ZONTALKAP_DLOADFILE(&cFileList[i][0]);
			
			if(iRet != ZONTALK_OK)
			{
				AppUtils_Cls();
				AppUtils_DisplayCenter("固件更新失败", LINE2, FALSE);
				AppUtils_DisplayCenter("请联系客服人员", LINE3, FALSE);
				
				AppUtils_SVCWAIT(2000);
				AppUtils_FlushKbd();
				AppUtils_WaitKey(3);
				return ZONTALK_NOK;
			}
			
			if(0 == i)
			{
				//
				// 升级FW需要重新下载AID参数和公钥
				//
				AppUtils_ResetStatus(&gTermCfg.mDynPrm.EMVPrmStatus, STATUS_AIDDOWN);
				AppUtils_ResetStatus(&gTermCfg.mDynPrm.EMVPrmStatus, STATUS_CAKEYDOWN);
				PrmMngr_SavePrmToSamDisk();
			}
			
			AppUtils_SVCWAIT(10000);
		}
	}
	
	
//#ifndef TRIDENT
//  //Trident平台不需要删除本地文件
//  ZONTALK_RemoveAllFile();
//#endif
//  {
//    AppUtils_Cls();
//    AppUtils_DisplayCenter("固件更新完成", LINE1, FALSE);
//    AppUtils_DisplayCenter("是否删除FW包", LINE2, FALSE);
//    AppUtils_DisplayLine((char*)"0-不删除  1-删除", LINE3);
//
//    do
//    {
//      bt = AppUtils_WaitKey(30);
//    }
//    while(KEY0 != bt && KEY1 != bt);
//
//    if(KEY1 == bt)
//    {
//      ZONTALK_RemoveAllFile(); //删除FW包
//    }
//  }

	AppUtils_Cls();
	AppUtils_DisplayCenter("固件更新完成", LINE1, FALSE);
	AppUtils_DisplayCenter("正在删除固件包", LINE2, FALSE);
	ZONTALK_RemoveAllFile(cFileList, iFileCount);

    while(1)
	{
		if(AppUtils_WaitKeyMill(300) == bESC)
		{
			return ZONTALK_NOK;
		}
		
		AppUtils_DisplayLine("未连接PP1000! ", LINE3);
		AppUtils_DisplayLine("请勿断电 ", LINE4);
		rv = ZONTALK_PingReader(5);
	}
	
	AppUtils_SVCWAIT(1000);
	AppUtils_DisplayLine("切换PP1000 ", LINE3);

	if(rv != BANK_OK)
	{
		AppUtils_DisplayLine("再次切换PP1000 ", LINE3);
		AppUtils_SVCWAIT(2000);

	}
	
	return iRet;
}

void ZONTALK_RemoveAllFile(char(*FileList)[64], int FileCount)
{
	int iCurrGroup = 0;
	int k;
	char szLocalFileName[128];
	

	
	for(k = 0; k < FileCount; k++)
	{
		memset(szLocalFileName, 0, sizeof(szLocalFileName));
		
		sprintf(szLocalFileName, "%s.TXT", FileList[k]);
		ZONTALK_RemoveLocalFile(szLocalFileName);
	}
	
//  ZONTALK_RemoveLocalFile("FLASHCLR.TXT");
//  ZONTALK_RemoveLocalFile("BASE.TXT");
//  ZONTALK_RemoveLocalFile("HANDLER1.TXT");
//  ZONTALK_RemoveLocalFile("HANDLER2.TXT");


}

void ZONTALK_RemoveLocalFile(char *i_Fil)
{
	char buffer[150] = {0};
	char filename[66] = {0};
	int  l_File = -1;
	int  l_Rv = 0;
	
    l_File = open(i_Fil, F_RDWR);
	
	if(l_File < 0)
	{
		return ;
	}
	

	
    close(l_File);
	memset(filename, 0, sizeof(filename));
	memcpy(filename, buffer, 64);
	
    //File_Remove(filename);
    //File_Remove(i_Fil);
}

int ZONTALK_PingReader(int iTimeOut)
{

    return 0;
}
