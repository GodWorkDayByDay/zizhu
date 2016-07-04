/****************************************************************************
 * FILE NAME:   PRNMNGR.C                                                   *
 * MODULE NAME: PRNMNGR                                                     *
 * PROGRAMMER:  XC                                                          *
 * DESCRIPTION: 参数管理模块: 所有的参数修改都仅针对gTermCfg全局变量;       *
 *              存入FLASH的时机由外部调用方控制                             *
 * REVISION:                                                                *
 ****************************************************************************/
#include "cupms.h"
#include <QDebug>


/******************************************************************************
* External Variables
******************************************************************************/
Term_Config gTermCfg;

/******************************************************************************
* Module Internal Variables & Definitions
******************************************************************************/
//参数已初始化的标志
static int s_init_flag = 0;
#define AppDebug_AppLog printf
//模块内结构变量，保存一份参数文件中的完整数据，回写前做比较，如果没有变化则跳过写入的动作
static Term_Config sTermCfg;


/******************************************************************************
 * Private Function Declarations
 ******************************************************************************/
static int inPrmMngr_GetAllPrm(Term_Config *T_config);
static int inPrmMngr_SaveAllPrm(Term_Config *T_config);
static int inPrmMngr_OperAdd(void *dumy);
static int inPrmMngr_OperDelete(void *dumy);
static int inPrmMngr_OperInquire(void *dumy);

#define DEV_COM1 "dfa"
#define DEV_COM2  "dfa"
#define DEV_USBSER "dfa"

/******************************************************************************
 * Private Const Variables
 ******************************************************************************/
char *PpDevName[MAX_PP_IF_TYPE] = {"", (char *)DEV_COM1, (char *)DEV_COM2, (char *)DEV_USBSER};


/******************************************************************************
 * Public Function Definitions
 ******************************************************************************/
/*--------------------------------------------------------------------------
 FUNCTION NAME: File_AppendRecord.
 DESCRIPTION:   保存记录
 PARAMETERS:    none.
 RETURN:        void.
 config.ini->param_file
 *-------------------------------------------------------------------------*/
int File_AppendRecord(char * FileName,void * SrcData, unsigned int SrcLen)
{
  int w_fh;
  int iRet;

  if((w_fh = open(FileName, O_WRONLY)) < 0)
  {
    if((w_fh = open(FileName, O_RDWR | O_CREAT  | O_EXCL, 0777)) < 0)
    {
      return BANK_FAIL;
    }
  }
  qDebug("lseek:%d", lseek(w_fh, 0L, SEEK_END));
  //读取文件
  qDebug("SrcLen[%d]", SrcLen);
  print_qdebug((unsigned char *)"write:", (unsigned char *)SrcData, 8 );
  iRet = write(w_fh, (void*)SrcData, SrcLen);
  qDebug("iRet[%d]", iRet);
  if(iRet != SrcLen)
  {
    close(w_fh);
    qDebug("write fail");
    return BANK_FAIL;
  }

  close(w_fh);

  return FILE_SUCCESS;
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: File_AppendRecord.
 DESCRIPTION:   保存记录
 PARAMETERS:    none.
 RETURN:        void.
 config.ini->param_file
 *-------------------------------------------------------------------------*/
int File_AppendRecord_bak(char * FileName,void * SrcData, unsigned int SrcLen)
{
  FILE * w_fh;

  if((w_fh = fopen(FileName, "r+")) == NULL)
  {
    if((w_fh = fopen(FileName, "a+")) == NULL)
    {
      return BANK_FAIL;
    }
  }

  qDebug("fseek:%d", fseek(w_fh, 0L, SEEK_END));
  //读取文件
  qDebug("SrcLen[%d]", SrcLen);
  print_qdebug((unsigned char *)"write:", (unsigned char *)SrcData, 8 );
  if(fwrite((void*)SrcData, SrcLen, 1, w_fh) < 0)
  {
      fclose(w_fh);

    return BANK_FAIL;
  }

  fclose(w_fh);

  return FILE_SUCCESS;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_SysManage
 * DESCRIPTION:   系统管理菜单
 * PARAMETERS:    (void*)0:需要密码验证;  (void*)1:不需要密码验证;
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_SysManage(void *dummy)
{
	int iRet;
	unsigned char  bMenuOption;
	char cTitleBuffer[32+1];
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "商户参数设置",  PrmMngr_MerchantPrmSettings, (void *) 0 },
		{ NULL,    2, "交易管理设置",  PrmMngr_TranMngSettings, (void *) 0 },
		{ NULL,    3, "系统参数设置",  PrmMngr_SysPrmSettings, (void *) 2 },
		{ NULL,    4, "通讯参数设置",  PrmMngr_CommPrmSettings, (void *) 0 },
		{ NULL,    5, "终端密钥管理",  PrmMngr_Menu_KeyManage, (void *) 0 },
		{ NULL,    6, "密码管理",      PrmMngr_Menu_ChangePwd, (void *) 0 },
		{ NULL,    7, "其他功能设置",  PrmMngr_Menu_Others, (void *) 0 },
	};
	
	AppUtils_Cls();
	
	if(dummy == (void *) 0)
	{
		AppUtils_DisplayCenter("系统管理", LINE1, TRUE);
		
		if(PrmMngr_InputPwd((void *) 99) != BANK_OK)
			return BANK_FAIL;
	}
	
	if(PrmMngr_GetSubAppModiFlag() == 0)
	{
		aMenuItem[0].Func = PrmMngr_UnsupportSettings;  //商户参数设置-不支持
		aMenuItem[1].Parameter = (void*)1;              //交易管理设置
		
		aMenuItem[2].Parameter = (void*)0;              //系统参数设置
		aMenuItem[3].Parameter = (void*)1;              //通讯参数设置
		aMenuItem[4].Parameter = (void*)1;              //终端密钥管理
		
		aMenuItem[5].Func = PrmMngr_UnsupportSettings;  //密码管理-不支持
//    aMenuItem[6].Parameter = (void*)1;              //其他功能设置
	}
	else if(PrmMngr_GetSubAppModiFlag() == 1)
	{
		aMenuItem[0].Func = PrmMngr_SetMerchantName;    //商户参数设置-仅支持修改商户名称
		aMenuItem[1].Parameter = (void*)1;              //交易管理设置
		
		aMenuItem[2].Parameter = (void*)1;              //系统参数设置
		aMenuItem[4].Parameter = (void*)1;              //终端密钥管理
		
		aMenuItem[5].Func = PrmMngr_UnsupportSettings;  //密码管理-不支持
//    aMenuItem[6].Parameter = (void*)1;              //其他功能设置
	}
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsRotatedScreen() == BANK_OK)
		sprintf(cTitleBuffer, "系统管理");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	iRet = PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
	
	PrmMngr_SavePrmToSamDisk();
	return iRet;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_TranMngSettings
 * DESCRIPTION:   交易管理设置菜单
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_TranMngSettings(void *dummy)
{
	int iRet, iChoice, iNum;
	unsigned char  bMenuOption;
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "交易开关控制",  NULL, (void *) 0 },
		{ NULL,    2, "交易输密控制",  NULL, (void *) 0 },
		{ NULL,    3, "交易刷卡控制",  NULL, (void *) 0 },
		{ NULL,    4, "结算交易控制",  NULL, (void *) 0 },
		{ NULL,    5, "离线交易控制",  NULL, (void *) 0 },
		{ NULL,    6, "其它交易控制",  NULL, (void *) 0 },
	};
	char cTmpBuf[32+1];
	
	AppUtils_Cls();
	bMenuOption = MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	
	while(1)
	{
		iRet = PrnDisp_DisplayMenu(LINE1, "", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
		
		//修改级别0,1时，不支持修改，所以直接continue
		if(dummy == (void*)1)
		{
			if(iRet == 1 || iRet == 2 || iRet == 3 || iRet == 6)
				continue;
		}
		
		switch(iRet)
		{
		case 1:
			PrmMngr_TranSwitchSettings((void *)0);
			break;
		case 2:
			AppUtils_DisplayTitle((char *)"交易输密控制");
			
			//消费撤销是否输密码
			iChoice = gTermCfg.mTrSwitch.VoidPIN;
			
			if(AppUtils_YesOrNo("消费撤销是否输密", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.VoidPIN = iChoice;
			
			//预授权撤销是否输密码
			iChoice = gTermCfg.mTrSwitch.VoidPreAuthPIN;
			
			if(AppUtils_YesOrNo("预授撤销是否输密", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.VoidPreAuthPIN = iChoice;
			
			//预授权完成撤销是否输密码
			iChoice = gTermCfg.mTrSwitch.VoidPreAuthEndPIN;
			
			if(AppUtils_YesOrNo("预授完成撤销输密", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.VoidPreAuthEndPIN = iChoice;
			
			//预授权完成请求是否输密码
			iChoice = gTermCfg.mTrSwitch.PreAuthEndReqPIN;
			
			if(AppUtils_YesOrNo("预授完成请求输密", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.PreAuthEndReqPIN = iChoice;
			break;
		case 3:
			AppUtils_DisplayTitle((char *)"交易刷卡控制");
			
			//消费撤销是否刷卡
			iChoice = gTermCfg.mTrSwitch.VoidSwipCard;
			
			if(AppUtils_YesOrNo("消费撤销是否刷卡", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.VoidSwipCard = iChoice;
			
			//预授权完成撤销是否刷卡
			iChoice = gTermCfg.mTrSwitch.VoidPreauthEndSwipCard;
			
			if(AppUtils_YesOrNo("预授完成撤销刷卡", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.VoidPreauthEndSwipCard = iChoice;
			break;
		case 4:
			AppUtils_DisplayTitle((char *)"结算交易控制");
			
			if(dummy == (void*)0)
			{
				//结算完成是否自动签退
				iChoice = gTermCfg.mTrSwitch.AutoLogoutFlag;
				
				if(AppUtils_YesOrNo("结算是否自动签退", "0-否  1-是", &iChoice) != BANK_OK)
					return BANK_FAIL;
					
				gTermCfg.mTrSwitch.AutoLogoutFlag = iChoice;
				
				//结算完成是否提示打印明细
				iChoice = gTermCfg.mTrSwitch.DetailPrintFlag;
				
				if(AppUtils_YesOrNo("结算是否打印明细", "0-否  1-是", &iChoice) != BANK_OK)
					return BANK_FAIL;
					
				gTermCfg.mTrSwitch.DetailPrintFlag = iChoice;
			}
			
			//是否打印失败明细
			iChoice = gTermCfg.mTrSwitch.FailDetailPrint;
			
			if(AppUtils_YesOrNo("是否打印失败明细", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.FailDetailPrint = iChoice;
			break;
		case 5:
			AppUtils_DisplayTitle((char *)"离线交易控制");
			
			if(dummy == (void*)0)
			{
				//离线上送方式
				iChoice = gTermCfg.mFixPrm.OfflineUploadMode;
				
				if(AppUtils_YesOrNo("离线上送方式", "0-结算前1-联机前", &iChoice) != BANK_OK)
					return BANK_FAIL;
					
				gTermCfg.mFixPrm.OfflineUploadMode = iChoice;
				
				
				//离线上送次数
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.UploadMaxTimes);
				
				if(AppUtils_GetNum(LINE2, "离线上送次数", cTmpBuf, 1, 1, 60) <= 0)
					return BANK_FAIL;
					
				iNum = atoi((char *)cTmpBuf);
				
				if(iNum > 0)
					gTermCfg.mFixPrm.UploadMaxTimes = iNum;
			}
			
			//自动上送累计笔数
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%u", gTermCfg.mFixPrm.OffUploadNumLimit);
			
			if(AppUtils_GetNum(LINE2, "自动上送累计笔数", cTmpBuf, 1, 2, 60) < 0)
				return BANK_FAIL;
				
			iNum = atoi((char *)cTmpBuf);
			
			if(iNum > 0)
				gTermCfg.mFixPrm.OffUploadNumLimit = iNum;
				
			break;
		case 6:
			AppUtils_DisplayTitle((char *)"其它交易控制");
			
			//是否输入主管密码
			iChoice = gTermCfg.mTrSwitch.MngPwdRequired;
			
			if(AppUtils_YesOrNo("是否输入主管密码", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MngPwdRequired = iChoice;
			
			//是否允许手输卡号
			iChoice = gTermCfg.mTrSwitch.ManualFlag;
			
			if(AppUtils_YesOrNo("是否允许手输卡号", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.ManualFlag = iChoice;
			
#if 1 //银商暂时没要求改
			//默认刷卡交易
			iChoice = gTermCfg.mFixPrm.DefaultTransType;
			
			if(AppUtils_YesOrNo("默认刷卡交易", "0-预授权  1-消费", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mFixPrm.DefaultTransType = iChoice;
#else
			//Modify on 2015-07-17 for 银联改为1表示消费 2表示预授权
			//默认刷卡交易
			iChoice = gTermCfg.mFixPrm.DefaultTransType;
			
			if(AppUtils_YesOrNoEx("默认刷卡交易", "1-消费 2-预授权", &iChoice) != BANK_OK)
				return BANK_FAIL;
			
			gTermCfg.mFixPrm.DefaultTransType = iChoice;
#endif
			
			//最大退货金额
			if(PrmMngr_SetMaxRefundAmt((void *) 0) != BANK_OK)
				return BANK_FAIL;
				
			break;
		default:
			PrmMngr_SavePrmToSamDisk();
			return iRet;
		}
	}
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_TranSwitchSettings
 * DESCRIPTION:   交易开关控制
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_TranSwitchSettings(void *dummy)
{
	int iRet, iChoice;
	unsigned char  bMenuOption;
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "传统类交易",     NULL, (void *) 0 },
		{ NULL,    2, "电子现金类交易", NULL, (void *) 0 },
		{ NULL,    3, "电子钱包类交易", NULL, (void *) 0 },
		{ NULL,    4, "分期付款类交易", NULL, (void *) 0 },
		{ NULL,    5, "积分类交易",     NULL, (void *) 0 },
		{ NULL,    6, "手机芯片类交易", NULL, (void *) 0 },
		{ NULL,    7, "预约类交易",     NULL, (void *) 0 },
		{ NULL,    8, "订购类交易",     NULL, (void *) 0 },
		{ NULL,    9, "其他类交易",     NULL, (void *) 0 },
	};
	
	AppUtils_Cls();
	
	bMenuOption = MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	
	while(1)
	{
		iRet = PrnDisp_DisplayMenu(LINE1, "", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
		
		if(iRet >= 1 && (iRet <= sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem)))
		{
			AppUtils_DisplayTitle((char *)aMenuItem[iRet-1].pMenuTitle);
		}
		
		switch(iRet)
		{
		case 1: //传统类
			iChoice = gTermCfg.mTrSwitch.Sale_Support;
			
			if(AppUtils_YesOrNo("消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.Sale_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.Void_Support;
			
			if(AppUtils_YesOrNo("消费撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.Void_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.Refund_Support;
			
			if(AppUtils_YesOrNo("退货", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.Refund_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.Balance_Support;
			
			if(AppUtils_YesOrNo("余额查询", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.Balance_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.PreAuth_Support;
			
			if(AppUtils_YesOrNo("预授权", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.PreAuth_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.PreAuthVoid_Support;
			
			if(AppUtils_YesOrNo("预授权撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.PreAuthVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.CompeleteReq_Support;
			
			if(AppUtils_YesOrNo("预授权完成请求", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.CompeleteReq_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.CompeleteNote_Support;
			
			if(AppUtils_YesOrNo("预授权完成通知", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.CompeleteNote_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.CompeleteVoid_Support;
			
			if(AppUtils_YesOrNo("预授权完成撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.CompeleteVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.OfflineSettle_Support;
			
			if(AppUtils_YesOrNo("离线结算", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OfflineSettle_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.SettleAdjust_Support;
			
			if(AppUtils_YesOrNo("结算调整", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.SettleAdjust_Support = iChoice;
			break;
			
		case 2: //电子现金类
			iChoice = gTermCfg.mTrSwitch.ECash_Support;
			
			if(AppUtils_YesOrNo("接触电子现金消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.ECash_Support = iChoice;
			
			//是否支持非接卡
			iChoice = gTermCfg.mTrSwitch.CTLS_Support;
			
			if(AppUtils_YesOrNo("非接电子现金消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.CTLS_Support = iChoice;
			
			
			iChoice = gTermCfg.mTrSwitch.EcashLoad_Support;
			
			if(AppUtils_YesOrNo("现金圈存", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EcashLoad_Support = iChoice;
			
			
			iChoice = gTermCfg.mTrSwitch.EcashInLoad_Support;
			
			if(AppUtils_YesOrNo("指定账户圈存", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EcashInLoad_Support = iChoice;
			
			
			iChoice = gTermCfg.mTrSwitch.EcashOutLoad_Support;
			
			if(AppUtils_YesOrNo("非指定账户圈存", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EcashOutLoad_Support = iChoice;
			
			
			iChoice = gTermCfg.mTrSwitch.EcashLoadVoid_Support;
			
			if(AppUtils_YesOrNo("现金充值撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EcashLoadVoid_Support = iChoice;
			
			
			iChoice = gTermCfg.mTrSwitch.EcRefund_Support;
			
			if(AppUtils_YesOrNo("电子现金脱机退货", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EcRefund_Support = iChoice;
			break;
		case 3: //电子钱包类
			iChoice = gTermCfg.mTrSwitch.EPSale;
			
			if(AppUtils_YesOrNo("电子钱包消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EPSale = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.EPInLoad;
			
			if(AppUtils_YesOrNo("EP指定账户圈存", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EPInLoad = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.EPOutLoad;
			
			if(AppUtils_YesOrNo("EP非指定账户圈存", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EPOutLoad = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.EPCashLoad;
			
			if(AppUtils_YesOrNo("电子钱包现金充值", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.EPCashLoad = iChoice;
			break;
			
		case 4: //分期类
			iChoice = gTermCfg.mTrSwitch.IntallSale_Support;
			
			if(AppUtils_YesOrNo("分期付款消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.IntallSale_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.IntallVoid_Support;
			
			if(AppUtils_YesOrNo("分期付款撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.IntallVoid_Support = iChoice;
			break;
			
		case 5: //积分类
			iChoice = gTermCfg.mTrSwitch.UnionScoreSale_Support;
			
			if(AppUtils_YesOrNo("联盟积分消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.UnionScoreSale_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.IssuerScoreSale_Support;
			
			if(AppUtils_YesOrNo("发卡行积分消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.IssuerScoreSale_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.UnionScoreVoid_Support;
			
			if(AppUtils_YesOrNo("联盟积分撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.UnionScoreVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.IssuerScoreVoid_Support;
			
			if(AppUtils_YesOrNo("发卡行积分撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.IssuerScoreVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.UnionScoreBalance_Support;
			
			if(AppUtils_YesOrNo("联盟积分查询", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.UnionScoreBalance_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.UnionScoreRefund_Support;
			
			if(AppUtils_YesOrNo("联盟积分退货", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.UnionScoreRefund_Support = iChoice;
			break;
			
		case 6: //手机芯片类
			iChoice = gTermCfg.mTrSwitch.MChipSale_Support;
			
			if(AppUtils_YesOrNo("手机芯片消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipSale_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MChipVoid_Support;
			
			if(AppUtils_YesOrNo("手机芯片消费撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MChipRefund_Support;
			
			if(AppUtils_YesOrNo("手机芯片退货", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipRefund_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MChipPreAuth_Support;
			
			if(AppUtils_YesOrNo("手机芯片预授权", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipPreAuth_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MChipPreAuthVoid_Support;
			
			if(AppUtils_YesOrNo("手机预授权撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipPreAuthVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MChipCompleteReq_Support;
			
			if(AppUtils_YesOrNo("预授权完成请求", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipCompleteReq_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MChipCompleteNote_Support;
			
			if(AppUtils_YesOrNo("预授权完成通知", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipCompleteNote_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MChipCompleteVoid_Support;
			
			if(AppUtils_YesOrNo("预授权完成撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipCompleteVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MChipBalance_Support;
			
			if(AppUtils_YesOrNo("手机芯片查余额", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MChipBalance_Support = iChoice;
			break;
			
		case 7: //预约类
			iChoice = gTermCfg.mTrSwitch.YuyueSale_Support;
			
			if(AppUtils_YesOrNo("预约消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.YuyueSale_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.YuyueVoid_Support;
			
			if(AppUtils_YesOrNo("预约消费撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.YuyueVoid_Support = iChoice;
			break;
			
		case 8: //订购类
			iChoice = gTermCfg.mTrSwitch.OrderSale_Support;
			
			if(AppUtils_YesOrNo("订购消费", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OrderSale_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.OrderVoid_Support;
			
			if(AppUtils_YesOrNo("订购消费撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OrderVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.OrderRefund_Support;
			
			if(AppUtils_YesOrNo("订购退货", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OrderRefund_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.OrderPreAuth_Support;
			
			if(AppUtils_YesOrNo("订购预授权", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OrderPreAuth_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.OrderPreAuthVoid_Support;
			
			if(AppUtils_YesOrNo("订购预授权撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OrderPreAuthVoid_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.OrderCompleteReq_Support;
			
			if(AppUtils_YesOrNo("预授权完成请求", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OrderCompleteReq_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.OrderCompleteNote_Support;
			
			if(AppUtils_YesOrNo("预授权完成通知", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OrderCompleteNote_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.OrderCompleteVoid_Support;
			
			if(AppUtils_YesOrNo("预授权完成撤销", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.OrderCompleteVoid_Support = iChoice;
			break;
			
		case 9: //其它类
			iChoice = gTermCfg.mTrSwitch.MagCashLoad_Support;
			
			if(AppUtils_YesOrNo("磁条卡现金充值", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MagCashLoad_Support = iChoice;
			
			iChoice = gTermCfg.mTrSwitch.MagAccountLoad_Support;
			
			if(AppUtils_YesOrNo("磁条卡账户充值", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MagAccountLoad_Support = iChoice;
			break;
		default:
			PrmMngr_SavePrmToSamDisk();
			return iRet;
		}
	}
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_KeyManage
 * DESCRIPTION:   终端密钥管理(菜单)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_KeyManage(void *dummy)
{
	int iRet;
	unsigned char  bMenuOption;
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "IC卡导入密钥",  KeyMngr_ImportKeyICCard, (void *) 0 },
		{ NULL,    2, "母POS导入密钥", KeyMngr_ImportKeyExtPort, (void *) 0 },
		{ NULL,    3, "设置主密钥索引", PrmMngr_SetKeyIndex, (void *) 0 },
		{ NULL,    4, "设置算法类型",  PrmMngr_SetDesType, (void *) 0 },
		{ NULL,    5, "手工输入密钥",  KeyMngr_ManualLoadKey, (void *) 0 },
#ifdef MAKE_APPDEBUG
		{ NULL,    6, "设置测试密钥",  KeyMngr_SetTestKey, (void *) 0 },
#endif
	};
	
	if(dummy == (void*)1)
	{
		aMenuItem[2].Func = PrmMngr_UnsupportSettings;
		aMenuItem[3].Func = PrmMngr_UnsupportSettings;
	}
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	iRet = PrnDisp_DisplayMenu(LINE1, "密钥管理", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
	
	return iRet;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_ChangePwd
 * DESCRIPTION:   修改密码(菜单)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_ChangePwd(void *dummy)
{
	int   iRet;
	unsigned char  bMenuOption;
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "系统管理密码",   PrmMngr_ChangUserPwd, (void *) 99 },
		{ NULL,    2, "安全密码",       PrmMngr_ChangUserPwd, (void *) 98 },
	};
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	iRet = PrnDisp_DisplayMenu(LINE1, "修改密码", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
	
	return iRet;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_Others
 * DESCRIPTION:   其它功能(菜单)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_Others(void *dummy)
{
	int iRet;
	unsigned char  bMenuOption;
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "清除交易流水",   PrmMngr_Menu_Clear, (void *) 0 },
		{ NULL,    2, "签购单打印",     PrmMngr_ReceiptSettings, (void *) 0 },
		{ NULL,    3, "下载功能",       Trans_DownloadFuncMenu, (void *) 0 },
		{ NULL,    4, "参数打印",       PrnDisp_PrintTermCfg, (void *) 0 },
	};
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	iRet = PrnDisp_DisplayMenu(LINE1, "其他功能设置", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
	
	PrmMngr_SavePrmToSamDisk();
	return iRet;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ManufacturerOthers
 * DESCRIPTION:   制造商参数设置的其他参数设置
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ManufacturerOthers(void *dummy)
{
	unsigned char bMenuOption;
	char cTitleBuffer[ 32 + 1 ];
	PrnDisp_DisplayMenuItem OtherMenuItem[] =
	{
		{ NULL, 1, "重置公钥",      Trans_ClearCAKey, (void *) 0 },
        //{ NULL, 2, "重置AID参数",   Trans_ClearAIDPrm, (void *) 0 },
		{ NULL, 3, "清除脚本结果",  Trans_ClearScriptResult, (void *) 0 },
		{ NULL, 4, "脚本结果上送",  Trans_UploadScriptResult, (void *) 0 },
		//{ NULL, 5, "POS状态传递",   Trans_UploadPOSStatus, (void *) 0 },
		{ NULL, 5, "电子现金上送",  Trans_UploadOfflineTrans, (void *) 0 },
		{ NULL, 6, "黑名单下载",    Trans_BlackListDownload, (void *) 0 },
		{ NULL, 7, "设置主管",      PrmMngr_ChangUserPwd, (void *) 0 },
		{ NULL, 8, "打印所有交易",  PrmMngr_SetAllTransPrnFlag, (void *) 0 },
		{ NULL, 9, "证书文件",      PrmMngr_SetSSLCertFile, (void *) 0 },
		//{ NULL, 7, "黑名单打印",    NULL, (void *) 0 },
	};
	
	memset(cTitleBuffer, 0, sizeof(cTitleBuffer));
	
	if(AppUtils_IsBigScreen() == BANK_OK)
		sprintf(cTitleBuffer, "%s", "下载功能菜单");
		
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	return PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(OtherMenuItem) / sizeof(PrnDisp_DisplayMenuItem), OtherMenuItem, 60);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_Clear
 * DESCRIPTION:   清除流水(菜单)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_Clear(void *dummy)
{
	int iRet;
	unsigned char  bMenuOption;
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "清除冲正流水",   PrmMngr_ClearReversal, (void *) 2 },
		{ NULL,    2, "清除交易流水",   PrmMngr_ClearAllTrans, (void *) 0 },
	};
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	iRet = PrnDisp_DisplayMenu(LINE1, "清除流水", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
	
	return iRet;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_MerchantPrmSettings
 * DESCRIPTION:   商户参数设置
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_MerchantPrmSettings(void *dummy)
{
//  int iChoice;

	//商户号
	if(PrmMngr_SetMerchantID((void *)0) != BANK_OK)
		return BANK_FAIL;
		
	//商户名称(中文、英文)
	if(PrmMngr_SetMerchantName((void *)0) != BANK_OK)
		return BANK_FAIL;
		
	//终端号
	if(PrmMngr_SetTerminalID((void *)0) != BANK_OK)
		return BANK_FAIL;
		
	//子应用名称
	if(PrmMngr_SetAppDispName((void *)0) != BANK_OK)
		return BANK_FAIL;
		
//  AppUtils_DisplayCenter("商户参数设置", LINE1, TRUE);
//
//  iChoice = 0;
//
//  if(strcmp((char *)gTermCfg.mFixPrm.ReceiptTitle, (char *)"银联POS") == 0)
//    iChoice = 1;
//
//  if(AppUtils_YesOrNo("抬头内容选择", "0-输入    1-默认", &iChoice) != BANK_OK)
//    return BANK_FAIL;
//
//  if(iChoice == 0)
//  {
//    //签购单抬头设置
//    if(PrmMngr_SetReceiptTitle((void *) 0) != BANK_OK)
//      return BANK_FAIL;
//  }
//  else if(iChoice == 1)
//  {
//    strcpy((char *)gTermCfg.mFixPrm.ReceiptTitle, "银联POS");
//  }
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SysPrmSettings
 * DESCRIPTION:   系统参数设置
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SysPrmSettings(void *dummy)
{
	int iChoice, iFee;
	char cTmpBuf[32];
	byte bRet, PP_InterfaceTmp;
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("系统参数设置", LINE1, TRUE);
	
	if(dummy == (void*)1 || dummy == (void*)2)
	{
		//流水号
		if(PrmMngr_SetSystemTrace((void *) 0) != BANK_OK)
			return BANK_FAIL;
	}
	
	if(dummy == (void*)2)
	{
		//批次号
		if(PrmMngr_SetBatchNo((void *) 0) != BANK_OK)
			return BANK_FAIL;
			
		//是否打印中文收单行
		iChoice = gTermCfg.mTrSwitch.AccquChnName;
		
		if(AppUtils_YesOrNo("打印中文收单行", "0-否    1-是", &iChoice) != BANK_OK)
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.AccquChnName = iChoice;
		
		//是否打印中文发卡行
		iChoice = gTermCfg.mTrSwitch.IssueChnName;
		
		if(AppUtils_YesOrNo("打印中文发卡行", "0-否    1-是", &iChoice) != BANK_OK)
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.IssueChnName = iChoice;
		
		//热敏打印联数
		if(PrmMngr_SetReceiptNum((void *)0) != BANK_OK)
			return BANK_FAIL;
			
		//签购单是否打英文
		iChoice = gTermCfg.mTrSwitch.EnglishSlip;
		
		if(AppUtils_YesOrNo("签购单打印英文", "0-否    1-是", &iChoice) != BANK_OK)
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.EnglishSlip = iChoice;
		
		
		//冲正重发次数
		if(PrmMngr_SetReversalTimes(dummy) != BANK_OK)
			return BANK_FAIL;
			
		//最大交易笔数
		if(PrmMngr_SetMaxTrans((void *) 0) != BANK_OK)
			return BANK_FAIL;
	}
	
	PrmMngr_SavePrmToSamDisk();
	
	//设置密码键盘接口
	while(1)
	{
		sprintf(cTmpBuf, "密码键盘连接[%d]", gTermCfg.mFixPrm.PP_Interface);
		AppUtils_DisplayLine(cTmpBuf, LINE2);
		AppUtils_DisplayLine("0.内置 1.COM1", LINE3);
#ifdef MAKE_LINUX
		AppUtils_DisplayLine("2.COM3 3.USB", LINE4);
#else
		AppUtils_DisplayLine("2.COM2 3.USB", LINE4);
#endif
		bRet = AppUtils_WaitKey(60);
		
		if(bRet >= '0' && bRet <= '3')
		{
			PP_InterfaceTmp = gTermCfg.mFixPrm.PP_Interface;
			gTermCfg.mFixPrm.PP_Interface = bRet - '0';
			AppUtils_WaitKeyMill(200);
			
			//密码键盘连接方式有修改需要重检测
			if(PP_InterfaceTmp != gTermCfg.mFixPrm.PP_Interface)
			{
				if(PrmMngr_DevInitPinpad((void *) 1) ==  BANK_OK)
				{
					AppUtils_DisplayCenter((char*)"检测成功", LINE3, FALSE);
					AppUtils_WaitKey(1);
				}
			}
			
			break;
		}
		else if(bRet == 0 || bRet == bESC)
		{
			return BANK_FAIL;
		}
		else if(bRet == bENTER)
		{
			if(PrmMngr_DevInitPinpad((void *) 1) ==  BANK_OK)
			{
				AppUtils_DisplayCenter((char*)"检测成功", LINE3, FALSE);
				AppUtils_WaitKey(1);
			}
			
			break;
		}
	}
	
	
	//设置非接设备接口
	if(gTermCfg.mTrSwitch.CTLS_Support == 1)
	{
		while(1)
		{
			sprintf(cTmpBuf, "非接连接方式[%d]", gTermCfg.mFixPrm.CtlsInterface);
			AppUtils_DisplayLine(cTmpBuf, LINE2);
			AppUtils_DisplayLine("0.内置 1.COM1", LINE3);
#ifdef MAKE_LINUX
			AppUtils_DisplayLine("2.COM3 3.USB", LINE4);
#else
			AppUtils_DisplayLine("2.COM2 3.USB", LINE4);
#endif
			bRet = AppUtils_WaitKey(60);
			
			if(bRet >= '0' && bRet <= '3')
			{
				gTermCfg.mFixPrm.CtlsInterface = bRet - '0';
			}
			else if(bRet == 0 || bRet == bESC)
			{
				return BANK_FAIL;
			}
			else if(bRet == bENTER)
				break;
		}
		
		iChoice = gTermCfg.mTrSwitch.CtlsTransChannelFlag;
		
		if(AppUtils_YesOrNo("非接电子现金优先", "0-否  1-是", &iChoice) != BANK_OK) //非接受理通道设置  0-联机借贷记优先 1-电子现金优先
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.CtlsTransChannelFlag = iChoice;
		
		iChoice = gTermCfg.mTrSwitch.GetPanSupportCTLS;
		
		if(AppUtils_YesOrNo("消费界面挥卡", "0-不支持  1-支持", &iChoice) != BANK_OK)
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.GetPanSupportCTLS = iChoice;
		
		// 测试非接设备是否OK
		AppUtils_UserClear(LINE2);
		AppUtils_DisplayLine("连接非接读卡器", LINE2);
        //CTLS_ConnectReader((void *)0);
        //CTLS_Switch2PPMode();
	}
	
	
	if(dummy == (void*)2)
	{
		//是否支持小费
		iChoice = gTermCfg.mTrSwitch.FeeFlag;
		
		if(AppUtils_YesOrNo("是否支持小费", "0-否    1-是", &iChoice) != BANK_OK)
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.FeeFlag = iChoice;
		
		//小费百分比
		if(gTermCfg.mTrSwitch.FeeFlag)
		{
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%x", gTermCfg.mFixPrm.FeePercent);
			
			if(AppUtils_GetNum(LINE2, "小费百分比(%):", cTmpBuf, 1, 2, 60) > 0)
			{
				iFee = atoi(cTmpBuf);
				
				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				sprintf(cTmpBuf, "%02d", iFee);
				AppUtils_Asc2Bcd((byte *)cTmpBuf, &gTermCfg.mFixPrm.FeePercent, 2);
			}
			else
				return BANK_FAIL;
		}
		
		//金额是否打印负号
		iChoice = gTermCfg.mTrSwitch.NegativeSignFlag;
		
		if(AppUtils_YesOrNo("金额是否打印负号", "0-否    1-是", &iChoice) != BANK_OK)
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.NegativeSignFlag = iChoice;
		
		//强制需要屏蔽卡号 by XC 2014/12/13
		gTermCfg.mTrSwitch.MaskPanFlag = 1;
		
		//支持屏蔽卡号的情况下，预授权屏蔽卡号的设置才有意义
		if(gTermCfg.mTrSwitch.MaskPanFlag)
		{
			iChoice = gTermCfg.mTrSwitch.PreAuthCardNoMask;
			
			if(AppUtils_YesOrNo("预授权屏蔽卡号", "0-否    1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.PreAuthCardNoMask = iChoice;
		}
	}
	
	PrmMngr_SavePrmToSamDisk();
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetCommMode
 * DESCRIPTION:   选择通讯类型
 * PARAMETERS:    dummy: (void*)1 需要验证主管密码
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetCommMode(void *dummy)
{
	int iRet;
	unsigned char  bMenuOption;
	char cTitleBuffer[ 32 + 1 ];
	PrnDisp_DisplayMenuItem aCommMenuItem[] =
	{
		{ NULL,    1, "拨号",   NULL, (void *) 0 },
		{ NULL,    2, "GPRS",   NULL, (void *) 0 },
		{ NULL,    3, "CDMA",   NULL, (void *) 0 },
		{ NULL,    4, "以太网", NULL, (void *) 0 },
		{ NULL,    5, "串口",   NULL, (void *) 0 },
		{ NULL,    6, "WIFI",   NULL, (void *) 0 },
	};
	
	if(dummy == (void*)1)
	{
		AppUtils_DisplayCenter("设置通讯类型", LINE1, TRUE);
		
		// 主管密码
		if(PrmMngr_InputPwd((void *)0) != BANK_OK)
			return BANK_FAIL;
	}
	
	
	while(1)
	{
		AppUtils_Cls();
		
		switch(gTermCfg.mCommPrm.CurrentCommMode)
		{
		case COMM_TYPE_SDLC:
			iRet = 1;
			break;
		case COMM_TYPE_GPRS:
			iRet = 2;
			break;
		case COMM_TYPE_ETHERNET:
			iRet = 4;
			break;
		case COMM_TYPE_RS232:
			iRet = 5;
			break;
#ifdef TRIDENT
		case COMM_TYPE_CDMA:
			iRet = 3;
			break;
		case COMM_TYPE_WIFI:
			iRet = 6;
			break;
#endif
		default:
			iRet = 1;
			break;
		}
		
		sprintf(cTitleBuffer, "选择通讯类型:[%d]", iRet);
		
		bMenuOption = MENUOPT_1LINE2MENU | MENUOPT_NOI_NVERT_TITLE | MENUOPT_ENTER_RETURN;
		iRet = PrnDisp_DisplayMenu(LINE1, cTitleBuffer, bMenuOption, sizeof(aCommMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aCommMenuItem, 60);
		
		switch(iRet)
		{
		case 1:
			gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_SDLC;
			break;
		case 2:
			gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_GPRS;
			break;
		case 4:
			gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_ETHERNET;
			break;
		case 5:
			gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_RS232;
			break;
#ifdef TRIDENT
		case 3:
			gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_CDMA;
			break;
		case 6:
			gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_WIFI;
			break;
#else
		case 3:
		case 6:
			break;
#endif
		default:
			//AppDebug_AppLog( "Choose comm type iRet[%d]", iRet);
			PrmMngr_SavePrmToSamDisk();
			return BANK_OK;
		}
	}
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_CommPrmSettings
 * DESCRIPTION:   通讯参数设置
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_CommPrmSettings(void *dummy)
{
	int iPage;
	byte bKeyPressed;
	
	iPage = 0;
	
	while(1)
	{
		AppUtils_Cls();
		
		if(AppUtils_IsBigScreen() == BANK_OK || AppUtils_IsColorScreen() == BANK_OK)
		{
			AppUtils_DisplayLine("1TPDU  2通讯类型", LINE1);
			AppUtils_DisplayLine("3拨号  4GPRS", LINE2);
			AppUtils_DisplayLine("5CDMA  6以太网", LINE3);
			AppUtils_DisplayLine("7串口  8WIFI", LINE4);
			AppUtils_DisplayLine("9其它", LINE5);
		}
		else
		{
			if(iPage == 0)//当前第一页，显示向下箭头
			{
				AppUtils_DisplayLine("1TPDU  2通讯类型", LINE1);
				AppUtils_DisplayLine("3拨号  4GPRS", LINE2);
				AppUtils_DisplayLine("5CDMA  6以太网", LINE3);
				AppUtils_DisplayLine("7串口  8WIFI", LINE4);
				PrnDisp_DisplayDownArrow();
			}
			else//不是第一屏时显示向上的箭头
			{
				AppUtils_DisplayLine("9其它", LINE1);
				PrnDisp_DisplayUpArrow();
			}
		}
		
		bKeyPressed = AppUtils_WaitKey(60);
		
		//只响应部分按键
		if(dummy == (void*)1)
		{
			if(bKeyPressed == bKEY1 || bKeyPressed == bKEY3 || bKeyPressed == bKEY7)
			{
				continue;
			}
		}
		
		switch(bKeyPressed)
		{
		case bKEY1: //TPDU
			AppUtils_Cls();
			AppUtils_DisplayCenter("通讯参数设置", LINE1, TRUE);
			PrmMngr_SetTPDU((void *)0);
			break;
		case bKEY2: //通讯类型
			PrmMngr_SetCommMode((void *)0);
			break;
		case bKEY3: //拨号
			PrmMngr_Comm_SetTel((void *)0);
			break;
		case bKEY4: //GPRS
			PrmMngr_Comm_SetGprs((void *)dummy);
			break;
		case bKEY5: //CDMA
			PrmMngr_Comm_SetCdma((void *)dummy);
			break;
		case bKEY6: //ETHERNET
			PrmMngr_Comm_SetEth((void *)dummy);
			break;
		case bKEY7: //RS232
			PrmMngr_Comm_RS232((void *)0);
			break;
		case bKEY8: //WIFI
			PrmMngr_Comm_SetWifi((void *)dummy);
			break;
		case bKEY9: //其他
			AppUtils_Cls();
			AppUtils_DisplayCenter("通讯参数设置", LINE1, TRUE);
			
			if(dummy == (void*) 0)
			{
				//交易超时时间
				if(PrmMngr_Comm_Timeout(gTermCfg.mCommPrm.CurrentCommMode) != BANK_OK)
					break;
					
				//交易重拨次数
				if(PrmMngr_SetRetryTimes((void *)0) != BANK_OK)
					break;
			}
			
			//设置公网参数
			if(PrmMngr_Comm_InternetPrm(dummy) != BANK_OK)
				break;
				
			break;
		case bKEYC:
		
			if(iPage == 1)
				iPage = 0;
				
			break;
		case bKEYD:
		
			if(iPage == 0)
				iPage = 1;
				
			break;
		case bENTER:
			iPage = !iPage;
			break;
		case bESC:
		case 0:
			PrmMngr_SavePrmToSamDisk();
			return BANK_OK;
		default:
			break;
		}
		
		PrmMngr_SavePrmToSamDisk();
	}
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ManufacturerSettings
 * DESCRIPTION:   制造商参数设置菜单
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ManufacturerSettings(void *dummy)
{
	int iRet, iChoice;
	unsigned char  bMenuOption;
	byte bRet;
	char cTmpBuf[32 + 1];
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "终端参数设置",  NULL, (void *) 0 },
		{ NULL,    2, "其它参数设置",  NULL, (void *) 0 },
		{ NULL,    3, "修改密码",      NULL, (void *) 0 },
		{ NULL,    4, "EMV参数打印",   NULL, (void *) 0 },
		{ NULL,    5, "重新保存AID",   NULL, (void *) 0 },
		{ NULL,    6, "重新保存CAKEY",   NULL, (void *) 0 },
	};
	
	AppUtils_Cls();
	
	bMenuOption = MENUOPT_UP_DOWN_ARROW | MENUOPT_INORDER;
	
	while(1)
	{
		iRet = PrnDisp_DisplayMenu(LINE1, "制造商参数设置", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
		
		if(iRet >= 1 && (iRet <= sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem)))
		{
			AppUtils_DisplayTitle((char *)aMenuItem[iRet-1].pMenuTitle);
		}
		
		AppUtils_UserClear(LINE2);
		
		switch(iRet)
		{
		case 1:
			//当前年份
			AppUtils_UserClear(LINE2);
			
			if(PrmMngr_SetCurrentYear((void *) 0) != BANK_OK)
				break;
				
			//商行代码
			AppUtils_UserClear(LINE2);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			memcpy(cTmpBuf, gTermCfg.mFixPrm.LocalBank, 4);
			
			if(AppUtils_GetNum(LINE2, "请输入商行代码", (char *) cTmpBuf, 4, 4, 60) <= 0)
			{
				break;
			}
			
			memcpy(gTermCfg.mFixPrm.LocalBank, cTmpBuf, 4);
			
			//本地区域码
			AppUtils_UserClear(LINE2);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			memcpy(cTmpBuf, gTermCfg.mFixPrm.LocalArea, 4);
			
			if(AppUtils_GetNum(LINE2, "请输入本地地区码", (char *) cTmpBuf, 4, 4, 60) <= 0)
			{
				break;
			}
			
			memcpy(gTermCfg.mFixPrm.LocalArea, cTmpBuf, 4);
			
			//是否屏蔽卡号
			iChoice = gTermCfg.mTrSwitch.MaskPanFlag;
			
			if(AppUtils_YesOrNo("是否支持屏蔽卡号", "0-否    1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.MaskPanFlag = iChoice;
			
			
			//是否支持部分承兑（部分扣款）
			iChoice = gTermCfg.mTrSwitch.PartPurchaseFlag;
			
			if(AppUtils_YesOrNo("是否支持部分承兑", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.PartPurchaseFlag = iChoice;
			
			
			//是否支持磁道加密
			iChoice = gTermCfg.mTrSwitch.TrackEncryptFlag;
			
			if(AppUtils_YesOrNo("是否支持磁道加密", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.TrackEncryptFlag = iChoice;
			
			
			//是否支持磁道加密
			iChoice = gTermCfg.mTrSwitch.CheckBlackListCard;
			
			if(AppUtils_YesOrNo("是否支持黑名单", "0-否  1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.CheckBlackListCard = iChoice;
			
			
			//是否支持小额代授权
			iChoice = gTermCfg.mTrSwitch.SmallAmountStandinAuth;
			
			if(AppUtils_YesOrNo("小额代授权", "0-不支持  1-支持", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.SmallAmountStandinAuth = iChoice;
			
			//是否支持非接卡
			iChoice = gTermCfg.mTrSwitch.CTLS_Support;
			
			if(AppUtils_YesOrNo("是否支持非接", "0-否    1-是", &iChoice) != BANK_OK)
				return BANK_FAIL;
				
			gTermCfg.mTrSwitch.CTLS_Support = iChoice;
			
			if(gTermCfg.mTrSwitch.CTLS_Support)
			{
				while(1)
				{
					sprintf(cTmpBuf, "非接连接方式[%d]", gTermCfg.mFixPrm.CtlsInterface);
					AppUtils_DisplayLine(cTmpBuf, LINE2);
					AppUtils_DisplayLine("0.内置 1.COM1", LINE3);
#ifdef MAKE_LINUX
					AppUtils_DisplayLine("2.COM3 3.USB", LINE4);
#else
					AppUtils_DisplayLine("2.COM2 3.USB", LINE4);
#endif
					
					bRet = AppUtils_WaitKey(60);
					
					if(bRet >= '0' && bRet <= '3')
					{
						gTermCfg.mFixPrm.CtlsInterface = bRet - '0';
					}
					else if(bRet == 0 || bRet == bESC)
					{
						return BANK_FAIL;
					}
					else if(bRet == bENTER)
						break;
				}
				
				// 测试非接设备是否OK
				AppUtils_UserClear(LINE2);
				AppUtils_DisplayLine("连接非接读卡器", LINE2);

				
                //CTLS_Switch2PPMode();
			}
			
			//设置打印深度
			PrmMngr_SetPrinterDark((void *)0);
			break;
		case 2:
			PrmMngr_ManufacturerOthers((void *)0);
			break;
		case 3:
		
			if(PrmMngr_ChangUserPwd((void*)99) != BANK_OK)
				break;
				
			break;
		case 4:
			Trans_PbocHandingMenu((void*)0);
			break;
		case 5:
			Trans_AIDSave2Kernel((void*)0);
			break;
		case 6:
			Trans_CAKeySave2Kernal((void*)0);
			break;
		default:
			PrmMngr_SavePrmToSamDisk();
			return iRet;
		}
	}
}


/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_CheckPrmInitFlag
* DESCRIPTION:   检测参数初始化标记(是否从文件中取出数据赋值到gTermCfg)
* PARAMETERS:    None
* RETURN:  　　　1: 参数已初始化;  0: 参数未初始化;
* NOTES:
* -------------------------------------------------------------------------*/
int PrmMngr_CheckPrmInitFlag(void)
{
	return(s_init_flag);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_AddSystrace
 * DESCRIPTION:   增加系统流水号
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_AddSystrace(void)
{
	if(gTermCfg.mDynPrm.SysTrace < 999999)
		gTermCfg.mDynPrm.SysTrace ++;
	else
		gTermCfg.mDynPrm.SysTrace = 1;
		
	PrmMngr_SavePrmToSamDisk();
	
	AppDebug_AppLog("PrmMngr_AddSystrace ok........");
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetTerminalID
 * DESCRIPTION:   设置终端号
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetTerminalID(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("终端参数", LINE1, TRUE);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	memcpy(cTmpBuf, gTermCfg.mFixPrm.TerminalID, 8);
	
	if(AppUtils_GetNum(LINE2, "请输入终端编号", (char *) cTmpBuf, 8, 8, 60) <= 0)
	{
		return(BANK_ABORT);
	}
	
	if(strcmp(cTmpBuf, (char *) gTermCfg.mFixPrm.TerminalID))
	{
		// 判断是否有冲正
		if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL) == BANK_OK)
		{
			AppUtils_Warning((char *)"有冲正不允许修改");
			return BANK_FAIL;
		}
		
		//判断是否有交易记录
		if(AppUtils_GetNumberOfRecords((char *)TRAN_DB, sizeof(TransRecord)) > 0)
		{
			AppUtils_Warning((char *)"有流水不允许修改");
			return BANK_FAIL;
		}
		
		if(PrmMngr_InputPwd((void *) 98) == BANK_OK)
			memcpy(gTermCfg.mFixPrm.TerminalID, cTmpBuf, 8);
		else
			return BANK_FAIL;
			
		AppUtils_ResetStatus(&gTermCfg.mDynPrm.EMVPrmStatus, STATUS_AIDDOWN);
		AppUtils_ResetStatus(&gTermCfg.mDynPrm.EMVPrmStatus, STATUS_CAKEYDOWN);
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetReceiptTitle
 * DESCRIPTION:   设置签购单抬头
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetReceiptTitle(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	if(dummy == (void*)0)
	{
		AppUtils_Cls();
		AppUtils_DisplayCenter("签购单抬头设置", LINE1, TRUE);
	}
	else
		AppUtils_UserClear(LINE2);
		
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	strcpy(cTmpBuf, (char *)gTermCfg.mFixPrm.ReceiptTitle);
	

	if(strlen(cTmpBuf) > 0)
	{
		strcpy((char *)gTermCfg.mFixPrm.ReceiptTitle, cTmpBuf);
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetMerchantID
 * DESCRIPTION:   设置商户号
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetMerchantID(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("终端参数", LINE1, TRUE);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	strcpy(cTmpBuf, (char *) gTermCfg.mFixPrm.MerchantID);
	
	if(AppUtils_GetNum(LINE2, "请输入商户编号", (char *) cTmpBuf, 15, 15, 60) <= 0)
	{
		return(BANK_ABORT);
	}
	
	if(strcmp(cTmpBuf, (char *) gTermCfg.mFixPrm.MerchantID))
	{
		// 判断是否有冲正
		if(AppUtils_CheckStatus(gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL) == BANK_OK)
		{
			AppUtils_Warning((char *)"有冲正不允许修改");
			return BANK_FAIL;
		}
		
		//判断是否有交易记录
		if(AppUtils_GetNumberOfRecords((char *)TRAN_DB, sizeof(TransRecord)) > 0)
		{
			AppUtils_Warning((char *)"有流水不允许修改");
			return BANK_FAIL;
		}
		
		if(PrmMngr_InputPwd((void *) 98) == BANK_OK)
			strcpy((char *) gTermCfg.mFixPrm.MerchantID, cTmpBuf);
		else
			return BANK_FAIL;
			
		AppUtils_ResetStatus(&gTermCfg.mDynPrm.EMVPrmStatus, STATUS_AIDDOWN);
		AppUtils_ResetStatus(&gTermCfg.mDynPrm.EMVPrmStatus, STATUS_CAKEYDOWN);
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetMerchantName
 * DESCRIPTION:   设置商户名称
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetMerchantName(void *dummy)
{
	byte bRet;
	char cTmpBuf[40 + 1];
	
	//修改中文商户名
	while(1)
	{
		AppUtils_Cls();
		AppUtils_DisplayCenter("商户中文名称", LINE1, TRUE);
		AppUtils_DisplayRight((char *)gTermCfg.mFixPrm.MerchantNameChn, LINE2);
		AppUtils_DisplayLine("按#键修改", LINE4);
		
		bRet = AppUtils_WaitKey(60);
		
		if(bRet == '#')
		{
			AppUtils_Cls();
			

		}
		else if(bRet == 0 || bRet == bESC)
		{
			return BANK_FAIL;
		}
		else if(bRet == bENTER)
		{
			break;
		}
	}
	
	//修改英文商户名
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	strcpy(cTmpBuf, (char *) gTermCfg.mFixPrm.MerchantNameEng);
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("商户英文名称", LINE1, TRUE);
	
	if(AppUtils_GetText(LINE2, "", (char *) cTmpBuf, 0, 40, 0, 60) < 0)
		return(BANK_ABORT);
		
	if(strlen(cTmpBuf) > 0)
		strcpy((char *) gTermCfg.mFixPrm.MerchantNameEng, cTmpBuf);
	else
		memset(gTermCfg.mFixPrm.MerchantNameEng, 0, sizeof(gTermCfg.mFixPrm.MerchantNameEng));
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetAppDispName
 * DESCRIPTION:   设置应用显示名称(子应用名称)
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetAppDispName(void *dummy)
{
	byte bRet;
	char cTmpBuf[128 + 1];
	
	while(1)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		

		
		AppUtils_Cls();
		AppUtils_DisplayCenter("子应用名称", LINE1, TRUE);
		AppUtils_DisplayRight((char *)cTmpBuf, LINE2);
		AppUtils_DisplayLine("按#键修改", LINE4);
		bRet = AppUtils_WaitKey(60);
		
		if(bRet == '#')
		{
			AppUtils_Cls();
			

		}
		else if(bRet == 0 || bRet == bESC)
		{
			return BANK_FAIL;
		}
		else if(bRet == bENTER)
		{
			break;
		}
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetUnknownIssuerName
 * DESCRIPTION:   设置未知发卡行名称
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetUnknownIssuerName(void *dummy)
{
	byte bRet;
	char cTmpBuf[128 + 1];
	
	while(1)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		strcpy(cTmpBuf, gTermCfg.mFixPrm.UnknownIssuerName);
		
		AppUtils_Cls();
		AppUtils_DisplayCenter("未知发卡行名称", LINE1, TRUE);
		AppUtils_DisplayRight((char *)cTmpBuf, LINE2);
		AppUtils_DisplayLine("按#键修改", LINE4);
		
		bRet = AppUtils_WaitKey(60);
		
		if(bRet == '#')
		{
			AppUtils_Cls();
			

		}
		else if(bRet == 0 || bRet == bESC)
		{
			return BANK_FAIL;
		}
		else if(bRet == bENTER)
		{
			break;
		}
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetSystemTrace
 * DESCRIPTION:   设置流水号
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetSystemTrace(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	AppUtils_UserClear(LINE2);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%06lu", gTermCfg.mDynPrm.SysTrace);
	
	if(AppUtils_GetNum(LINE2, "当前流水号", (char *) cTmpBuf, 1, 6, 60) <= 0)
	{
		return(BANK_ABORT);
	}
	
	gTermCfg.mDynPrm.SysTrace = (ulint) atol(cTmpBuf);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetBatchNo
 * DESCRIPTION:   设置批次号
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetBatchNo(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	AppUtils_UserClear(LINE2);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%06lu", gTermCfg.mDynPrm.BatchNum);
	
	if(AppUtils_GetNum(LINE2, "当前批次号", (char *) cTmpBuf, 1, 6, 60) <= 0)
	{
		return(BANK_ABORT);
	}
	
	gTermCfg.mDynPrm.BatchNum = (ulint) atol(cTmpBuf);
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetMaxRefundAmt
 * DESCRIPTION:   设置最大退货金额
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetMaxRefundAmt(void *dummy)
{
	byte bAmtBuf[12 + 1];
	
	AppUtils_UserClear(LINE2);
	memset(bAmtBuf, 0, sizeof(bAmtBuf));
	memcpy(bAmtBuf, gTermCfg.mFixPrm.RefundAmt, 12);
	
	if(AppUtils_GetAmout(LINE2, "最大退货金额:", 0, CURRENCY_NAME, TRUE, bAmtBuf) != BANK_OK)
	{
		return BANK_FAIL;
	}
	
	memcpy(gTermCfg.mFixPrm.RefundAmt, bAmtBuf, 12);
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_SetGprs
 * DESCRIPTION:   设置GPRS通讯参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_Comm_SetGprs(void *dummy)
{
	char  cTemp[256];
#ifdef TRIDENT
	int iChoice;
#endif
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("GPRS设置", LINE1, TRUE);
	
	//gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_GPRS;
	
	if(dummy == (void*)0)
	{
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.GprsPhoneNo);
		
		if(AppUtils_GetText(LINE2, "GPRS接入号码:", cTemp, 0, 31, 0, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.GprsPhoneNo, (char *) cTemp);
			
			
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.GprsAPN);
		
		if(AppUtils_GetText(LINE2, "GPRS接入点(APN):", cTemp, 0, 31, 0, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.GprsAPN, (char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.GprsHostIP1);
		
		if(AppUtils_GetIPAddress("主机地址:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.GprsHostIP1, (char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		sprintf(cTemp, "%u", gTermCfg.mCommPrm.GprsHostPort1);
		
		if(AppUtils_GetNum(LINE2, "主机端口:", cTemp, 1, 5, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			gTermCfg.mCommPrm.GprsHostPort1 = (unsigned short) atoi((char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.GprsHostIP2);
		
		if(AppUtils_GetIPAddress("备份主机地址:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.GprsHostIP2, (char *) cTemp);
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		sprintf(cTemp, "%u", gTermCfg.mCommPrm.GprsHostPort2);
		
		if(AppUtils_GetNum(LINE2, "备份主机端口:", cTemp, 1, 5, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			gTermCfg.mCommPrm.GprsHostPort2 = (unsigned short) atoi((char *) cTemp);
			
		//GPRS用户名
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.GprsUserName);
		
		if(AppUtils_GetText(LINE2, "GPRS登录用户名:", cTemp, 0, 31, 0, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		strcpy((char *) gTermCfg.mCommPrm.GprsUserName, (char *) cTemp);
		
		//GPRS登陆密码
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.GprsUserPwd);
		
		if(AppUtils_GetText(LINE2, "GPRS登录密码:", cTemp, 0, 31, 0, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		strcpy((char *) gTermCfg.mCommPrm.GprsUserPwd, (char *) cTemp);
	}
	
#ifdef TRIDENT
	iChoice = 1;
	
	if(AppUtils_YesOrNo("是否开机自动注册", "0-否    1-是", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	if(iChoice)
	{
		VComm_SetAutoMode(COMM_TYPE_GPRS);
	}
	
	if(AppUtils_GetTerminalType() == _VX675)
		iChoice = 0; //VX675的GPRS模块不同，不建议重置SIM卡，此处默认值改为0
	else
		iChoice = 1;
		
	if(AppUtils_YesOrNo("是否重置SIM卡", "0-否    1-是", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	if(iChoice)
	{
		VComm_ResetSIM(COMM_TYPE_GPRS);
	}
	
	Comm_RegisterNetwork(1);
#endif
	
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_SetCdma
 * DESCRIPTION:   设置CDMA通讯参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_Comm_SetCdma(void *dummy)
{
#ifdef TRIDENT
	char  cTemp[256];
	int iChoice;
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("CDMA设置", LINE1, TRUE);
	
	//gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_CDMA;
	
	if(dummy == (void*)0)
	{
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.CdmaPhoneNo);
		
		if(AppUtils_GetText(LINE2, "CDMA接入号码", cTemp, 0, 31, 0, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.CdmaPhoneNo, (char *) cTemp);
			
			
		//用户名
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.CdmaUserName);
		
		if(AppUtils_GetText(LINE2, "CDMA登录用户名:", cTemp, 0, 31, 0, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		strcpy((char *) gTermCfg.mCommPrm.CdmaUserName, (char *) cTemp);
		
		
		//密码
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.CdmaUserPwd);
		
		if(AppUtils_GetText(LINE2, "CDMA登录密码:", cTemp, 0, 31, 0, 60) < 0)
		{
			return BANK_FAIL;
		}
		
		strcpy((char *) gTermCfg.mCommPrm.CdmaUserPwd, (char *) cTemp);
		
		
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.CdmaHostIP1);
		
		if(AppUtils_GetIPAddress("主机地址:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.CdmaHostIP1, (char *) cTemp);
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		sprintf(cTemp, "%u", gTermCfg.mCommPrm.CdmaHostPort1);
		
		if(AppUtils_GetNum(LINE2, "主机端口:", cTemp, 1, 5, 30) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			gTermCfg.mCommPrm.CdmaHostPort1 = atoi((char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.CdmaHostIP2);
		
		if(AppUtils_GetIPAddress("备份主机地址:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.CdmaHostIP2, (char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		sprintf(cTemp, "%u", gTermCfg.mCommPrm.CdmaHostPort2);
		
		if(AppUtils_GetNum(LINE2, "备份主机端口:", cTemp, 1, 5, 30) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			gTermCfg.mCommPrm.CdmaHostPort2 = atoi((char *) cTemp);
	}
	
	//CDMA登录认证方式
	AppUtils_UserClear(2);
	memset(cTemp, 0, sizeof(cTemp));
	sprintf(cTemp, "%d", gTermCfg.mCommPrm.CdmaAuthType);
	
	if(AppUtils_GetNum(LINE2, "CDMA登录认证方式", cTemp, 1, 1, 30) < 0)
	{
		return BANK_FAIL;
	}
	
	gTermCfg.mCommPrm.CdmaAuthType = atoi(cTemp);
	
	if(gTermCfg.mCommPrm.CdmaAuthType > MSCHAP_PPP || gTermCfg.mCommPrm.CdmaAuthType < NONE_PPP)
	{
		gTermCfg.mCommPrm.CdmaAuthType = PAP_PPP;
	}
	
	
	iChoice = 1;
	
	if(AppUtils_YesOrNo("是否开机自动注册", "0-否    1-是", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	if(iChoice)
	{
		VComm_SetAutoMode(COMM_TYPE_GPRS);
	}
	
	Comm_RegisterNetwork(1);
	
	return BANK_OK;
#else
	AppUtils_Cls();
	AppUtils_Warning((char *) "暂不支持此类型");
	return BANK_FAIL;
#endif
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_SetWifi
 * DESCRIPTION:   设置Wifi通讯参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_Comm_SetWifi(void *dummy)
{
	//gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_WIFI;
	AppUtils_Cls();
	AppUtils_Warning((char *) "暂不支持此类型");
	return BANK_FAIL;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_InternetPrm
 * DESCRIPTION:   设置公网参数
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Comm_InternetPrm(void *dummy)
{
	int iChoice;
	char cTmpBuf[128];
	
	AppUtils_UserClear(LINE2);
	
	
	if(dummy == (void*)0)
	{
		//是否公网接入
		iChoice = gTermCfg.mTrSwitch.SSLCommFlag;
		
		if(AppUtils_YesOrNo("是否公网接入", "0-否    1-是", &iChoice) != BANK_OK)
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.SSLCommFlag = iChoice;
	}
	
	// 公网接入的情况下，后面的设置才有意义
	// by XC 2014/12/14
	if(!gTermCfg.mTrSwitch.SSLCommFlag)
		return BANK_OK;
		
	//是否打印中文发卡行
	iChoice = gTermCfg.mTrSwitch.OnewayAuthSSL;
	
	if(AppUtils_YesOrNo("是否单向认证", "0-否    1-是", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.OnewayAuthSSL = iChoice;
	
	if(dummy == (void*)0)
	{
		//是否启用DNS
		iChoice = gTermCfg.mTrSwitch.UseDNSFlag;
		
		if(AppUtils_YesOrNo("是否启用DNS", "0-否    1-是", &iChoice) != BANK_OK)
			return BANK_FAIL;
			
		gTermCfg.mTrSwitch.UseDNSFlag = iChoice;
	}
	
	if(gTermCfg.mTrSwitch.UseDNSFlag)
	{
		if(dummy == (void*)0)
		{
			//主机域名
			AppUtils_UserClear(LINE2);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			strcpy(cTmpBuf, (char *) gTermCfg.mCommPrm.SSLSvrDomainName1);
			
			if(AppUtils_GetText(LINE2, "公网主机域名", (char *) cTmpBuf, 0, 40, 0, 60) < 0)
				return(BANK_ABORT);
				
			if(strlen(cTmpBuf) > 0)
				strcpy((char *) gTermCfg.mCommPrm.SSLSvrDomainName1, cTmpBuf);
			else
				memset(gTermCfg.mCommPrm.SSLSvrDomainName1, 0, sizeof(gTermCfg.mCommPrm.SSLSvrDomainName1));
				
			//主机端口
			AppUtils_UserClear(LINE2);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%u", gTermCfg.mCommPrm.SSLHostPort1);
			
			if(AppUtils_GetNum(LINE2, "公网主机端口:", cTmpBuf, 1, 5, 60) < 0)
			{
				return BANK_FAIL;
			}
			
			if(strlen((char *) cTmpBuf) > 0)
				gTermCfg.mCommPrm.SSLHostPort1 = (unsigned short) atoi((char *) cTmpBuf);
				
				
			//备份主机域名
			AppUtils_UserClear(LINE2);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			strcpy(cTmpBuf, (char *) gTermCfg.mCommPrm.SSLSvrDomainName2);
			
			if(AppUtils_GetText(LINE2, "公网备份主机域名", (char *) cTmpBuf, 0, 40, 0, 60) < 0)
				return(BANK_ABORT);
				
			if(strlen(cTmpBuf) > 0)
				strcpy((char *) gTermCfg.mCommPrm.SSLSvrDomainName2, cTmpBuf);
			else
				memset(gTermCfg.mCommPrm.SSLSvrDomainName2, 0, sizeof(gTermCfg.mCommPrm.SSLSvrDomainName2));
				
			//备份主机端口
			AppUtils_UserClear(LINE2);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			sprintf(cTmpBuf, "%u", gTermCfg.mCommPrm.SSLHostPort2);
			
			if(AppUtils_GetNum(LINE2, "公网主机端口:", cTmpBuf, 1, 5, 60) < 0)
			{
				return BANK_FAIL;
			}
			
			if(strlen((char *) cTmpBuf) > 0)
				gTermCfg.mCommPrm.SSLHostPort2 = (unsigned short) atoi((char *) cTmpBuf);
		}
		
		
		//不使用DHCP的情况，应设置DNS服务器地址(SSL公网接入的DNS服务器存在专门的字段)
		if(gTermCfg.mTrSwitch.UseDHCPFlag == 0)
		{
			AppUtils_UserClear(2);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			strcpy((char *) cTmpBuf, (char *) gTermCfg.mCommPrm.SSLDnsIP1);
			
			if(AppUtils_IntelligentInputIP(LINE2, "首选DNS:", LINE3, cTmpBuf, 60) < 0)
			{
				return BANK_FAIL;
			}
			
			if(strlen((char *) cTmpBuf) > 0)
				strcpy((char *) gTermCfg.mCommPrm.SSLDnsIP1, (char *) cTmpBuf);
				
				
			AppUtils_UserClear(2);
			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			strcpy((char *) cTmpBuf, (char *) gTermCfg.mCommPrm.SSLDnsIP2);
			
			if(AppUtils_IntelligentInputIP(LINE2, "备选DNS:", LINE3, cTmpBuf, 60) < 0)
			{
				return BANK_FAIL;
			}
			
			if(strlen((char *) cTmpBuf) > 0)
				strcpy((char *) gTermCfg.mCommPrm.SSLDnsIP2, (char *) cTmpBuf);
		}
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_SetTel
 * DESCRIPTION:   设置拨号通讯参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_Comm_SetTel(void *dummy)
{
	char  cBuffer[32+1], cDispBuf[32+1];
	int   iFlag = 0, i;
	
	//设置电话号码
	AppUtils_Cls();
	AppUtils_DisplayCenter("拨号设置", LINE1, TRUE);
	
	//gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_SDLC;
	
	for(i = 0; i < 3; i++)
	{
		memset(cDispBuf, 0, sizeof(cDispBuf));
		sprintf(cDispBuf, "交易电话号码%d", i + 1);
		
		memset(cBuffer, 0, sizeof(cBuffer));
		strcpy((char *) cBuffer, (char *) gTermCfg.mCommPrm.TelPhone[i]);
		
		if(AppUtils_GetNum(LINE2, cDispBuf, cBuffer, 0, 19, 60) < 0)
			return BANK_FAIL;
			
		if(strlen(cBuffer) > 0)
			strcpy((char *)gTermCfg.mCommPrm.TelPhone[ i ], cBuffer);
	}
	
	//设置外线号
	ASSERT_FAIL(PrmMngr_SetPABX((void *)1));
	
	
	//是否预拨号
	iFlag = gTermCfg.mTrSwitch.IsPreDialFlag;
	
	if(AppUtils_YesOrNo("是否预拨号", "0-否  1-是", &iFlag) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.IsPreDialFlag = iFlag;
	
	
	//是否检测拨号音  by XC 2015/4/16 8:22:39
	iFlag = gTermCfg.mTrSwitch.CheckDialTone;
	
	if(AppUtils_YesOrNo("是否检测拨号音", "0-否  1-是", &iFlag) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.CheckDialTone = iFlag;
	
	
#ifdef MAKE_LINUX
	// S91寄存器取值
	memset(cDispBuf, 0, sizeof(cDispBuf));
	sprintf(cDispBuf, "S91(%u):", gTermCfg.mCommPrm.ModemRegS91);
	
	memset(cBuffer, 0, sizeof(cBuffer));
	sprintf(cBuffer, "%u", gTermCfg.mCommPrm.ModemRegS91);
	
	if(AppUtils_GetNum(LINE2, cDispBuf, cBuffer, 1, 2, 60) > 0)
	{
		gTermCfg.mCommPrm.ModemRegS91 = (byte)atoi(cBuffer);
	}
	else
		return BANK_FAIL;
		
#endif
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_SetEth
 * DESCRIPTION:   设置以太网通讯参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_Comm_SetEth(void *dummy)
{
	char cTemp[256];
	int iChoice;
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("以太网设置", LINE1, TRUE);
	
	//gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_ETHERNET;
	
	//是否公网接入
	iChoice = gTermCfg.mTrSwitch.UseDHCPFlag;
	
	if(AppUtils_YesOrNo("是否使用DHCP", "0-否    1-是", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.UseDHCPFlag = iChoice;
	
	
	if(gTermCfg.mTrSwitch.UseDHCPFlag)
	{
//    strcpy((char*)gTermCfg.mCommPrm.LocalIP, "0.0.0.0");
//    strcpy((char*)gTermCfg.mCommPrm.SubNetMask, "0.0.0.0");
//    strcpy((char*)gTermCfg.mCommPrm.GateWay, "0.0.0.0");
//    strcpy((char*)gTermCfg.mCommPrm.PriDNS, "0.0.0.0");
//    strcpy((char*)gTermCfg.mCommPrm.SecDNS, "0.0.0.0");
	}
	else
	{
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.LocalIP);
		
		if(AppUtils_GetIPAddress("本机地址:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
		{
			strcpy((char *) gTermCfg.mCommPrm.LocalIP, (char *) cTemp);
		}
		
		
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.SubNetMask);
		
		if(AppUtils_GetIPAddress("子网掩码:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.SubNetMask, (char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.GateWay);
		
		if(AppUtils_GetIPAddress("网关地址:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.GateWay, (char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.PriDNS);
		
		if(AppUtils_GetIPAddress("主DNS:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.PriDNS, (char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.SecDNS);
		
		if(AppUtils_GetIPAddress("次DNS:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.SecDNS, (char *) cTemp);
	}
	
	//注销网络
	Comm_LogoutNetwork(1);
	
	if(dummy == (void*)0)
	{
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.EthHostIP1);
		
		if(AppUtils_GetIPAddress("主机地址:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.EthHostIP1, (char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		sprintf(cTemp, "%u", gTermCfg.mCommPrm.EthHostPort1);
		
		if(AppUtils_GetNum(LINE2, "主机端口:", cTemp, 1, 5, 30) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen(cTemp) > 0)
			gTermCfg.mCommPrm.EthHostPort1 = atoi((char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		strcpy((char *) cTemp, (char *) gTermCfg.mCommPrm.EthHostIP2);
		
		if(AppUtils_GetIPAddress("备份主机地址:", cTemp) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen((char *) cTemp) > 0)
			strcpy((char *) gTermCfg.mCommPrm.EthHostIP2, (char *) cTemp);
			
			
		AppUtils_UserClear(2);
		memset(cTemp, 0, sizeof(cTemp));
		sprintf(cTemp, "%u", gTermCfg.mCommPrm.EthHostPort2);
		
		if(AppUtils_GetNum(LINE2, "备份主机端口:", cTemp, 1, 5, 30) < 0)
		{
			return BANK_FAIL;
		}
		
		if(strlen(cTemp) > 0)
			gTermCfg.mCommPrm.EthHostPort2 = atoi((char *) cTemp);
	}
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_RS232
 * DESCRIPTION:   设置RS232通讯参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_Comm_RS232(void *dummy)
{
	char cDispBuf[32], cTmpBuf[16];
	byte bRet;
	
	//gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_RS232;
	AppUtils_Cls();
	AppUtils_DisplayCenter("通讯参数设置", LINE1, TRUE);
	
	while(1)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));

		
		if(cTmpBuf[0] != '1' && cTmpBuf[0] != '3')
			cTmpBuf[0] = '1';
			
		AppUtils_DisplayLine("请选择串口:", LINE2);
		AppUtils_DisplayLine(" 0-COM1  1-COM3", LINE3);
		
		sprintf(cDispBuf, "当前[COM%c]", cTmpBuf[0]);
		AppUtils_DisplayRight(cDispBuf, LINE4);
		
		bRet = AppUtils_WaitKey(30);
		

	}
	
	PrnDisp_DisplayWarning("通讯参数设置", "串口设置成功");
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_Timeout
 * DESCRIPTION:   设置通讯超时
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_Comm_Timeout(int CommMode)
{
	int iTimeout;
	char cBuffer[ 56 ];
	
	switch(gTermCfg.mCommPrm.CurrentCommMode)
	{
	case COMM_TYPE_SDLC:
		iTimeout = gTermCfg.mCommPrm.TelTimeout;
		break;
	case COMM_TYPE_ETHERNET:
		iTimeout = gTermCfg.mCommPrm.EthTimeout;
		break;
	case COMM_TYPE_GPRS:
		iTimeout = gTermCfg.mCommPrm.GprsTimeout;
		break;
#ifdef TRIDENT
	case COMM_TYPE_CDMA:
		iTimeout = gTermCfg.mCommPrm.CdmaTimeout;
		break;
	case COMM_TYPE_WIFI:
		iTimeout = gTermCfg.mCommPrm.WifiTimeout;
		break;
#endif
	default:
		return BANK_OK;
	}
	
	AppUtils_UserClear(LINE2);
	memset(cBuffer, 0, sizeof(cBuffer));
	sprintf(cBuffer, "%d", iTimeout);
	
	if(AppUtils_GetNum(LINE2, "通讯超时时间:", cBuffer, 0, 2, 60) <= 0)
		return BANK_FAIL;
		
	iTimeout = atoi(cBuffer);
	
	switch(gTermCfg.mCommPrm.CurrentCommMode)
	{
	case COMM_TYPE_SDLC:
		gTermCfg.mCommPrm.TelTimeout = iTimeout;
		break;
	case COMM_TYPE_ETHERNET:
		gTermCfg.mCommPrm.EthTimeout = iTimeout;
		break;
	case COMM_TYPE_GPRS:
		gTermCfg.mCommPrm.GprsTimeout = iTimeout;
		break;
#ifdef TRIDENT
	case COMM_TYPE_CDMA:
		gTermCfg.mCommPrm.CdmaTimeout = iTimeout;
		break;
	case COMM_TYPE_WIFI:
		gTermCfg.mCommPrm.WifiTimeout = iTimeout;
		break;
#endif
	default:
		return BANK_OK;
	}
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InitPrmFromTMS
 * DESCRIPTION:   从TMS参数文件中获取参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_InitPrmFromTMS(void *dummy)
{
	Bank_Operator Oper;
	char    ParamFileName[40 + 1], RamFileName[40 + 1];
	int     ParaHd, AppHd;
	int     iRet, i, j, iTmp;
	int     ParaNum;
	AppParaMsg  mAppPara;
	char    cTempBuf[512];
	unsigned long ulTagv;
	
	memset(ParamFileName, 0, sizeof(ParamFileName));
	strcpy(ParamFileName, "/");
	strcat(ParamFileName, APPNAME);
	strcat(ParamFileName, "PR.para");
	
	AppDebug_AppLog("ParamFileName=%s", ParamFileName);
	

	
	
	//有应用程序ZIP包 不能更新参数(应该等应用程序包解压缩安装完成之后才能更新参数)!!!
	memset(RamFileName, 0, sizeof(RamFileName));
	strcpy(RamFileName, "/");
	strcat(RamFileName, APPNAME);
	strcat(RamFileName, ".zip");
    AppHd = open(RamFileName, O_RDONLY);


	ParaNum = 0;
//	iRet = TmsMngr_GetAppParaCount((unsigned char *)ParamFileName, &ParaNum);
	
	if(iRet == BANK_FAIL)
	{
		AppUtils_FormatWarning((char *) "%s参数为空", APPNAME);
		return BANK_OK;
	}
	
	
    //if(TmsMngr_CheckAppSettle(APPNAME) == BANK_OK)
	{
		AppUtils_Warning((char *) "请先结算");
		return BANK_OK;
	}
	
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("正在处理CUPMS参数", LINE1, FALSE);
	AppUtils_DisplayCenter("请稍候...", LINE2, FALSE);
	
	
	/*读取参数各项*/
	for(j = 0; j < ParaNum; j++)
	{
		memset((char *) &mAppPara, 0, sizeof(AppParaMsg));
//		iRet = TmsMngr_GetAppPara((byte *)ParamFileName, j, &mAppPara);
		
		if(iRet == BANK_FAIL)
		{
			AppUtils_FormatWarning((char *) "读取TAG失败");
			
			if(j == 0)
				return BANK_FAIL;
			else
				break;
		}
		
		if(j == 0)
		{
			AppUtils_Cls();
			AppUtils_DisplayCenter("重建CUPMS参数..", LINE1, FALSE);
			AppUtils_DisplayCenter("请稍候..", LINE2, FALSE);
			
			PrmMngr_SetDefaultPrms((void *) 0);
			PrmMngr_ReadPrmFromFlashDisk();
			//*InitPrmFlag = 1;
			
			AppUtils_Cls();
			AppUtils_DisplayLine("读取CUPMS参数..", LINE1);
			AppUtils_DisplayLine("请稍候......", LINE2);
		}
		
		memset(cTempBuf, 0x00, sizeof(cTempBuf));
		memcpy(cTempBuf, mAppPara.TagValue, mAppPara.TagLen);
		
		ulTagv = atol((char *)cTempBuf);
		
		switch(ulTagv)
		{
		case 1000001: /*商户号*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 15)
				memcpy(cTempBuf, mAppPara.ParaValue, 15);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mFixPrm.MerchantID, cTempBuf);
			break;
			
		case 1000002: /*商户名*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 40)
				memcpy(cTempBuf, mAppPara.ParaValue, 40);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mFixPrm.MerchantNameChn, cTempBuf);
			break;
			
		case 1000003: /*英文名*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 40)
				memcpy(cTempBuf, mAppPara.ParaValue, 40);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mFixPrm.MerchantNameEng, cTempBuf);
			break;
			
		case 1000004: /*商户类型*/
			break;
			
		case 1000005: /*终端号*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 8)
				memcpy(cTempBuf, mAppPara.ParaValue, 8);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mFixPrm.TerminalID, cTempBuf);
			break;
			
		case 1000006: /*子应用名称*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 32)
				memcpy(cTempBuf, mAppPara.ParaValue, 32);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			PrmMngr_SaveShowName(cTempBuf);
			break;
		case 2000001: /* 交易控制开关 */
			/*交易控制参数说明
			交易控制参数；先用计算器把前 6 位16 进制转换为 2 进制数，如：87ff80
			转换为2 进制数是100001111111111110000000，数字1表示支持，0表示不支
			持，从左到右位数分别表示:
			第1 位  消费
			第2 位  消费撤销
			第3 位  退货
			第4 位  余额查询
			第5 位  预授权
			第6 位  预授权撤销
			第7 位  预授权完成请求
			第8 位  预授权完成通知
			第9 位  预授权完成撤销
			第10 位  离线结算
			第11 位  结算调整
			第12 位  接触式电子现金消费
			第13 位  快速支付（非接电子现金消费）
			第14 位  电子现金指定账户圈存
			第15 位  电子现金非指定账户圈存
			第16 位  电子现金现金充值
			第17 位  电子钱包消费
			第18 位  电子钱包指定账户圈存
			第19 位  电子钱包非指定账户圈存
			第20 位  电子钱包现金充值
			第21 位  分期付款消费
			第22 位  分期付款消费撤销
			第23 位  联盟积分消费
			第24 位  发卡行积分消费
			第25 位  联盟积分消费撤销
			第26 位  发卡行积分消费撤销
			第27 位  手机消费
			第28 位  手机消费撤销*/
			
			memset(cTempBuf, 0, sizeof(cTempBuf));
			

			//第1字节
			gTermCfg.mTrSwitch.Sale_Support         = ((cTempBuf[0] & 0x80) == 0x80);
			gTermCfg.mTrSwitch.Void_Support         = ((cTempBuf[0] & 0x40) == 0x40);
			gTermCfg.mTrSwitch.Refund_Support           = ((cTempBuf[0] & 0x20) == 0x20);
			gTermCfg.mTrSwitch.Balance_Support      = ((cTempBuf[0] & 0x10) == 0x10);
			gTermCfg.mTrSwitch.PreAuth_Support      = ((cTempBuf[0] & 0x08) == 0x08);
			gTermCfg.mTrSwitch.PreAuthVoid_Support  = ((cTempBuf[0] & 0x04) == 0x04);
			gTermCfg.mTrSwitch.CompeleteReq_Support = ((cTempBuf[0] & 0x02) == 0x02);
			gTermCfg.mTrSwitch.CompeleteNote_Support    = ((cTempBuf[0] & 0x01) == 0x01);
			
			//第2字节
			gTermCfg.mTrSwitch.CompeleteVoid_Support    = ((cTempBuf[1] & 0x80) == 0x80);
			gTermCfg.mTrSwitch.OfflineSettle_Support    = ((cTempBuf[1] & 0x40) == 0x40);
			gTermCfg.mTrSwitch.SettleAdjust_Support     = ((cTempBuf[1] & 0x20) == 0x20);
			
			gTermCfg.mTrSwitch.ECash_Support        = ((cTempBuf[1] & 0x10) == 0x10);
			//gTermCfg.mTrSwitch.CTLS_Support           = ((cTempBuf[1] & 0x08) == 0x08);
			
			if((cTempBuf[1] & 0x08) == 0)  //快速支付(非接电子现金)
			{
				gTermCfg.mTrSwitch.CTLS_Support = 0;
			}
			else
			{
                int iTermType = 1;

				//_VX670无法支持非接, _VX680 _VX820若有则必定是内置非接, 其它机型则外置非接
				if(iTermType == _VX670)
				{
					gTermCfg.mTrSwitch.CTLS_Support = 0;
					gTermCfg.mFixPrm.CtlsInterface = CTLS_IF_INTERNAL;
					gTermCfg.mTrSwitch.GetPanSupportCTLS = 0;
				}
				else if(iTermType == _VX820 || iTermType == _VX680 || iTermType == _VX680C || iTermType == _VX675)
				{
					gTermCfg.mFixPrm.CtlsInterface = CTLS_IF_INTERNAL;
					//gTermCfg.mTrSwitch.GetPanSupportCTLS = 1; //内置非接不支持消费界面挥卡(刷卡时会误挥卡)
				}
				else
				{
					gTermCfg.mFixPrm.CtlsInterface = CTLS_IF_COM1;
				}
			}
			
			gTermCfg.mTrSwitch.EcashLoad_Support            = ((cTempBuf[1] & 0x04) == 0x04);
			gTermCfg.mTrSwitch.EcashInLoad_Support          = ((cTempBuf[1] & 0x02) == 0x02);
			gTermCfg.mTrSwitch.EcashOutLoad_Support         = ((cTempBuf[1] & 0x01) == 0x01);
			
			//第3字节
			gTermCfg.mTrSwitch.EPSale                       = ((cTempBuf[2] & 0x80) == 0x80);
			gTermCfg.mTrSwitch.EPCashLoad                   = ((cTempBuf[2] & 0x40) == 0x40);
			gTermCfg.mTrSwitch.EPInLoad                     = ((cTempBuf[2] & 0x20) == 0x20);
			gTermCfg.mTrSwitch.EPOutLoad                    = ((cTempBuf[2] & 0x10) == 0x10);
			gTermCfg.mTrSwitch.IntallSale_Support           = ((cTempBuf[2] & 0x08) == 0x08);
			gTermCfg.mTrSwitch.IntallVoid_Support           = ((cTempBuf[2] & 0x04) == 0x04);
			gTermCfg.mTrSwitch.UnionScoreSale_Support       = ((cTempBuf[2] & 0x02) == 0x02);
			gTermCfg.mTrSwitch.IssuerScoreSale_Support      = ((cTempBuf[2] & 0x01) == 0x01);
			
			//第4字节
			gTermCfg.mTrSwitch.UnionScoreVoid_Support       = ((cTempBuf[3] & 0x80) == 0x80);
			gTermCfg.mTrSwitch.IssuerScoreVoid_Support      = ((cTempBuf[3] & 0x40) == 0x40);
			gTermCfg.mTrSwitch.MChipSale_Support            = ((cTempBuf[3] & 0x20) == 0x20);   //手机消费
			gTermCfg.mTrSwitch.MChipVoid_Support            = ((cTempBuf[3] & 0x10) == 0x10);   //手机消费撤销
			gTermCfg.mTrSwitch.MChipRefund_Support          = ((cTempBuf[3] & 0x08) == 0x08);   //手机芯片退货
			gTermCfg.mTrSwitch.MChipPreAuth_Support         = ((cTempBuf[3] & 0x04) == 0x04);   //手机芯片预授权
			gTermCfg.mTrSwitch.MChipPreAuthVoid_Support     = ((cTempBuf[3] & 0x02) == 0x02);   //手机芯片预授权撤销
			gTermCfg.mTrSwitch.MChipCompleteReq_Support     = ((cTempBuf[3] & 0x01) == 0x01);   //手机芯片预授权完成请求
			
			//第5字节
			gTermCfg.mTrSwitch.MChipCompleteNote_Support    = ((cTempBuf[4] & 0x80) == 0x80);   //手机芯片预授权完成通知
			gTermCfg.mTrSwitch.MChipCompleteVoid_Support    = ((cTempBuf[4] & 0x40) == 0x40);   //手机芯片预授权完成撤销
			gTermCfg.mTrSwitch.MChipBalance_Support         = ((cTempBuf[4] & 0x20) == 0x20);   //手机芯片余额查询
			
			gTermCfg.mTrSwitch.YuyueSale_Support            = ((cTempBuf[4] & 0x10) == 0x10);   //预约消费
			gTermCfg.mTrSwitch.YuyueVoid_Support            = ((cTempBuf[4] & 0x08) == 0x08);   //预约消费撤销
			gTermCfg.mTrSwitch.OrderSale_Support            = ((cTempBuf[4] & 0x04) == 0x04);   //订购消费
			gTermCfg.mTrSwitch.OrderVoid_Support            = ((cTempBuf[4] & 0x02) == 0x02);   //订购消费撤销
			gTermCfg.mTrSwitch.OrderRefund_Support          = ((cTempBuf[4] & 0x01) == 0x01);   //订购退货
			
			//第6字节
			gTermCfg.mTrSwitch.OrderPreAuth_Support         = ((cTempBuf[5] & 0x80) == 0x80);   //订购预授权
			gTermCfg.mTrSwitch.OrderPreAuthVoid_Support     = ((cTempBuf[5] & 0x40) == 0x40);   //订购预授权撤销
			gTermCfg.mTrSwitch.OrderCompleteReq_Support     = ((cTempBuf[5] & 0x20) == 0x20);   //订购预授权完成请求
			gTermCfg.mTrSwitch.OrderCompleteVoid_Support    = ((cTempBuf[5] & 0x10) == 0x10);   //订购预授权完成撤销
			gTermCfg.mTrSwitch.OrderCompleteNote_Support    = ((cTempBuf[5] & 0x08) == 0x08);   //订购预授权完成通知
			gTermCfg.mTrSwitch.MagCashLoad_Support          = ((cTempBuf[5] & 0x04) == 0x04);   //磁条卡现金充值
			gTermCfg.mTrSwitch.MagAccountLoad_Support       = ((cTempBuf[5] & 0x02) == 0x02);   //磁条卡帐户充值
			gTermCfg.mTrSwitch.EcashLoadVoid_Support        = ((cTempBuf[5] & 0x01) == 0x01);   //电子现金现金充值撤销
			
			//第7字节
			gTermCfg.mTrSwitch.EcRefund_Support             = ((cTempBuf[6] & 0x80) == 0x80);   //电子现金退货
			gTermCfg.mTrSwitch.UnionScoreBalance_Support    = ((cTempBuf[6] & 0x40) == 0x40);   //联盟积分查询
			gTermCfg.mTrSwitch.UnionScoreRefund_Support     = ((cTempBuf[6] & 0x20) == 0x20);   //联盟积分退货
			
			//预留……
			
			break;
			
		case 2000002: /* 交易是否输入卡密码 */
		
			/*消费撤销是否密码*/
			if(mAppPara.ParaValue[0] == '0')
				gTermCfg.mTrSwitch.VoidPIN = 0;
			else
				gTermCfg.mTrSwitch.VoidPIN = 1;
				
			/*预授权撤销是否密码*/
			if(mAppPara.ParaValue[1] == '0')
				gTermCfg.mTrSwitch.VoidPreAuthPIN = 0;
			else
				gTermCfg.mTrSwitch.VoidPreAuthPIN = 1;
				
				
			/*预授权完成撤销是否密码*/
			if(mAppPara.ParaValue[2] == '0')
				gTermCfg.mTrSwitch.VoidPreAuthEndPIN = 0;
			else
				gTermCfg.mTrSwitch.VoidPreAuthEndPIN = 1;
				
				
			/*预授权完成(请求)是否密码*/
			if(mAppPara.ParaValue[3] == '0')
				gTermCfg.mTrSwitch.PreAuthEndReqPIN = 0;
			else
				gTermCfg.mTrSwitch.PreAuthEndReqPIN = 1;
				
			break;
			
		case 2000003: /* 是否输入主管密码 */
		
			if(mAppPara.ParaValue[0] == '0')
				gTermCfg.mTrSwitch.MngPwdRequired = 0;
			else
				gTermCfg.mTrSwitch.MngPwdRequired = 1;
				
			break;
			
		case 2000004: /* 消费撤销，预授权完成撤销是否需要刷卡 */
		
			if(mAppPara.ParaValue[0] == '0')
				gTermCfg.mTrSwitch.VoidSwipCard = 0;
			else
				gTermCfg.mTrSwitch.VoidSwipCard = 1;
				
			if(mAppPara.ParaValue[1] == '0')
				gTermCfg.mTrSwitch.VoidPreauthEndSwipCard = 0;
			else
				gTermCfg.mTrSwitch.VoidPreauthEndSwipCard = 1;
				
			break;
			
		case 2000005: /*是否手输卡号*/
		
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mTrSwitch.ManualFlag = 1;
			else
				gTermCfg.mTrSwitch.ManualFlag = 0;
				
			break;
			
		case 2000006:
		
			/* 是否自动签退 */
			if(mAppPara.ParaValue[0] == '0')
				gTermCfg.mTrSwitch.AutoLogoutFlag = 0;
			else
				gTermCfg.mTrSwitch.AutoLogoutFlag = 1;
				
			/*是否打印明细*/
			if(mAppPara.ParaValue[1] == '0')
				gTermCfg.mTrSwitch.DetailPrintFlag = 0;
			else
				gTermCfg.mTrSwitch.DetailPrintFlag = 1;
				
			break;
			
		case 2000007: /* 离线交易相关控制 */
		
			if(mAppPara.ParaValue[0] == '0')
				gTermCfg.mFixPrm.OfflineUploadMode = 0; //0-结算前
			else
				gTermCfg.mFixPrm.OfflineUploadMode = 1; //1-(下笔交易)联机前
				
			if(mAppPara.ParaValue[1] < '1' || mAppPara.ParaValue[1] > '9')
			{
				gTermCfg.mFixPrm.UploadMaxTimes = 3;
			}
			else if(mAppPara.ParaLen > 0)
			{
				memset(cTempBuf, 0, sizeof(cTempBuf));
				cTempBuf[0] = mAppPara.ParaValue[1];
				
				gTermCfg.mFixPrm.UploadMaxTimes = atoi(cTempBuf);
			}
			
			break;
			
		case 2000008: /* 默认刷卡交易 1表示消费 0表示预授权 */
		
#if 1 //银商暂时没要求改
		
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mFixPrm.DefaultTransType = 1;
			else if(mAppPara.ParaValue[0] == '0')
				gTermCfg.mFixPrm.DefaultTransType = 0;
				
#else
				
			//Modify on 2015-07-17 for 银联改为1表示消费 2表示预授权
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mFixPrm.DefaultTransType = 1;
			else if(mAppPara.ParaValue[0] == '2')
				gTermCfg.mFixPrm.DefaultTransType = 2;
				
#endif
			break;
			
		case 2000009: /* 退货限额 单位为分 */
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			sprintf((char*)gTermCfg.mFixPrm.RefundAmt, "%012lu", (unsigned long)atol(cTempBuf));
			break;
			
		case 2000010: /* 是否支持磁道加密 */
		
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mTrSwitch.TrackEncryptFlag = 1;
			else
				gTermCfg.mTrSwitch.TrackEncryptFlag = 0;
				
			break;
			
		case 2000011: /* 预授权是否屏蔽卡号 */
		
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mTrSwitch.PreAuthCardNoMask = 1;
			else
				gTermCfg.mTrSwitch.PreAuthCardNoMask = 0;
				
			break;
		case 2000012: /* 是否打印失败明细 */
		
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mTrSwitch.FailDetailPrint = 1;
			else
				gTermCfg.mTrSwitch.FailDetailPrint = 0;
				
			break;
		case 2000014: /* 报文头处理重发次数 */
			break;
		case 3000001: /*交易流水号*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 6)
				memcpy(cTempBuf, mAppPara.ParaValue, 6);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			else
				memcpy(cTempBuf, "1", 1);
				
			gTermCfg.mDynPrm.SysTrace = atoi(cTempBuf);
			break;
			
		case 3000002: /*当前批次号*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 6)
				memcpy(cTempBuf, mAppPara.ParaValue, 6);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			else
				memcpy(cTempBuf, "1", 1);
				
			gTermCfg.mDynPrm.BatchNum = atoi(cTempBuf);
			break;
			
		case 3000003: /*终端主密钥号*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen <= 2 && mAppPara.ParaLen > 0)
			{
				memset(cTempBuf, 0, sizeof(cTempBuf));
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				gTermCfg.mFixPrm.KeyIndex = atoi(cTempBuf);
			}
			
			break;
			
		case 3000004: /*DES算法*/
		

			break;
			
		case 3000005: /*参数设置密码*/
		
			if(mAppPara.ParaLen == 8)
			{
				memset(&Oper, 0, sizeof(Bank_Operator));
				strcpy((char*)Oper.OperID, (char*)"99");
				Oper.IDLen = strlen((char*)Oper.OperID);
				
				if(AppUtils_OperFind(&Oper) == BANK_OK)
				{
					//此处必须复制一遍
					memcpy(Oper.OperPwd, mAppPara.ParaValue, mAppPara.ParaLen);
					Oper.PwdLen = mAppPara.ParaLen;
					
					AppUtils_OperUpdate(&Oper);
				}
				else
				{
					memcpy(Oper.OperPwd, mAppPara.ParaValue, mAppPara.ParaLen);
					Oper.PwdLen = mAppPara.ParaLen;
					Oper.OperType = OPER_SUPERVISOR;
					AppUtils_OperAdd(&Oper);
				}
			}
			
			break;
			
		case 3000006: /*安全密码 */
		
			if(mAppPara.ParaLen == 6)
			{
				memset(&Oper, 0, sizeof(Bank_Operator));
				strcpy((char*)Oper.OperID, (char*)"98");
				Oper.IDLen = strlen((char*)Oper.OperID);
				
				if(AppUtils_OperFind(&Oper) == BANK_OK)
				{
					//此处必须复制一遍
					memcpy(Oper.OperPwd, mAppPara.ParaValue, mAppPara.ParaLen);
					Oper.PwdLen = mAppPara.ParaLen;
					
					AppUtils_OperUpdate(&Oper);
				}
				else
				{
					memcpy(Oper.OperPwd, mAppPara.ParaValue, mAppPara.ParaLen);
					Oper.PwdLen = mAppPara.ParaLen;
					Oper.OperType = OPER_SUPERVISOR;
					AppUtils_OperAdd(&Oper);
				}
			}
			
			break;
			
		case 3000007: /* 打印中文收单行，发卡行(第一位:收单行;第二位:发卡行)(1:是，2:否) */
		
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mTrSwitch.AccquChnName = 1;
			else
				gTermCfg.mTrSwitch.AccquChnName = 0;
				
			if(mAppPara.ParaValue[1] == '1')
				gTermCfg.mTrSwitch.IssueChnName = 1;
			else
				gTermCfg.mTrSwitch.IssueChnName = 0;
				
			break;
			
		case 3000008: /* 套打签购单样式1表示旧  0表示新 */
			break;
			
		case 3000009: /*打印联数*/
		
			if(mAppPara.ParaValue[0] < '1' || mAppPara.ParaValue[0] > '3')
			{
				//越界判段，默认为2联
				gTermCfg.mFixPrm.Receipt = 2;
			}
			else if(mAppPara.ParaLen > 0)
			{
				gTermCfg.mFixPrm.Receipt = mAppPara.ParaValue[0] - '0';
			}
			
			break;
			
		case 3000010: /* 签购单是否打印英文 */
		
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mTrSwitch.EnglishSlip = 1;
			else
				gTermCfg.mTrSwitch.EnglishSlip = 0;
				
			break;
			
		case 3000011: /*冲正最大重发次数*/
		
			if(mAppPara.ParaLen > 0)
			{
				memset(cTempBuf, 0, sizeof(cTempBuf));
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
				iTmp = atoi(cTempBuf);
				
				if(iTmp <= 0 || iTmp > 9)
				{
					gTermCfg.mFixPrm.ReversalRetryTimes = 3;
				}
				else
				{
					gTermCfg.mFixPrm.ReversalRetryTimes = iTmp;
				}
			}
			
			break;
			
		case 3000012: /*最大交易笔数*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			iTmp = atoi(cTempBuf);
			
			if(iTmp > 999 || iTmp < 0)
			{
				gTermCfg.mFixPrm.MaxTranNumber = 300;
			}
			else
			{
				gTermCfg.mFixPrm.MaxTranNumber = iTmp;
			}
			
			break;
			
		case 3000013: /*内置或外置密码键盘*/
		
			if(AppUtils_IsOnlyBuiltInPinpad() == BANK_OK)
			{
				gTermCfg.mFixPrm.PP_Interface = PP_IF_INTERNAL;
			}
			else
			{
				if(mAppPara.ParaValue[0] == '0')
				{
					gTermCfg.mFixPrm.PP_Interface = PP_IF_INTERNAL;
				}
				else
				{
					//外置密码键盘，默认使用COM1口
					gTermCfg.mFixPrm.PP_Interface = PP_IF_COM1;
				}
			}
			
			break;
			
		case 3000018: /* 小费比例: 0表示关闭小费 */
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			
			iTmp = atoi(cTempBuf);
			
			if(iTmp == 0)
			{
				gTermCfg.mTrSwitch.FeeFlag = 0; //是否支持小费交易，0－不支持，1－支持
			}
			else if(iTmp > 0)
			{
				gTermCfg.mTrSwitch.FeeFlag = 1; //是否支持小费交易，0－不支持，1－支持
				
				memset(cTempBuf, 0, sizeof(cTempBuf));
				sprintf(cTempBuf, "%02d", iTmp);
				
				AppUtils_Asc2Bcd((byte *)cTempBuf, &gTermCfg.mFixPrm.FeePercent, 2); //小费百分比
			}
			
			break;
			
		case 3000020: /* 累计离线交易笔数 */
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			
			iTmp = atoi(cTempBuf);
			
			if(iTmp > 0)
			{
				gTermCfg.mFixPrm.OffUploadNumLimit = iTmp;
			}
			else
			{
				gTermCfg.mFixPrm.OffUploadNumLimit = 10;
			}
			
			break;
			
		case 3000021: /* 预留标签(包含多个参数) */
			/*|(签购单抬头类别:1打印LOGO，0打印中文)|(签购单抬头中文：默认“银联商务签购单”)|服务热线(默认为空) */
			for(i = 1; i < 9; i++)
			{
				memset(cTempBuf, 0, sizeof(cTempBuf));
				
				if(PrmMngr_GetAdditionPara(i + 1, (char*)mAppPara.ParaValue, mAppPara.ParaLen, (char*)cTempBuf) != BANK_OK)
				{
					continue;
				}
				
				switch(i)
				{
				case 1: //签购单抬头类别：1-打印LOGO  0-打印中文
				
					if(cTempBuf[1] == '0')
						gTermCfg.mTrSwitch.UserDefinedReceiptTitle = 1;
					else
						gTermCfg.mTrSwitch.UserDefinedReceiptTitle = 0;
						
					break;
					
				case 2: //抢购单抬头中文: (默认: 银联商务签购单)
				

					break;
					
				case 3: //服务热线，默认为空

				case 4: //撤销类交易签购单金额打印负号: 1-打印  0-不打印
				
					if(cTempBuf[0] == '1')
						gTermCfg.mTrSwitch.NegativeSignFlag = 1;
					else
						gTermCfg.mTrSwitch.NegativeSignFlag = 0;
						
					break;
					
				case 5: //结算单明细打印方式  1-打印所有交易  0-仅打印有效交易
				
					if(cTempBuf[0] == '1')
						gTermCfg.mTrSwitch.AllTransDetailFlag = 1;
					else
						gTermCfg.mTrSwitch.AllTransDetailFlag = 0;
						
					break;
					
				case 6: //未知发卡行代码打印方式: 为空时打印代码，否则打印本字段内容
					memset(gTermCfg.mFixPrm.UnknownIssuerName, 0, sizeof(gTermCfg.mFixPrm.UnknownIssuerName));
                //	BankUtil_CStrlcpy(gTermCfg.mFixPrm.UnknownIssuerName, (char*)cTempBuf, 32 + 1);
					break;
					
				case 7: //签购单字体: 0-小  1-中  2-大
				
					if(cTempBuf[0] == '1')
						gTermCfg.mFixPrm.ReceiptPrintFont = 1;
					else if(cTempBuf[0] == '2')
						gTermCfg.mFixPrm.ReceiptPrintFont = 2;
					else
						gTermCfg.mFixPrm.ReceiptPrintFont = 0;
						
					break;
					
				case 8: //签购单备注栏打印持卡人姓名拼音: 1-打印  0-不打印
				
					if(cTempBuf[0] == '1')
						gTermCfg.mTrSwitch.ReceiptCardHolderName = 1;
					else
						gTermCfg.mTrSwitch.ReceiptCardHolderName = 0;
						
					break;
					
				default:
					break;
				}
			}
			
			break;
		case 3000023: /* 商户个性化信息(仅针对热敏机型) */
			memset(gTermCfg.mFixPrm.MerchantInfo, 0, sizeof(gTermCfg.mFixPrm.MerchantInfo));
			
			if(mAppPara.ParaLen <= 128)
				memcpy(gTermCfg.mFixPrm.MerchantInfo, mAppPara.ParaValue, mAppPara.ParaLen);
			else
				memcpy(gTermCfg.mFixPrm.MerchantInfo, mAppPara.ParaValue, 128);
				
			break;
			
		case 3000036: /* 非接交易通道标志 1-联机 0-电子现金 默认非接联机*/
		
			if(mAppPara.ParaValue[0] == '0')
				gTermCfg.mTrSwitch.CtlsTransChannelFlag = 1;  //和我们程序定义的反的
			else
				gTermCfg.mTrSwitch.CtlsTransChannelFlag = 0;
				
			break;
			
		case 4000001: /*通讯方式 0nac 1gprs 2cdma 3rs232 4以太网*/
		
			if(mAppPara.ParaValue[0] == 0x33)    /*串口*/
			{
				gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_RS232;
			}
			else if(mAppPara.ParaValue[0] == 0x30)    /*拨号*/
			{
				gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_SDLC;
			}
			else if(mAppPara.ParaValue[0] == 0x31)    /*Gprs*/
			{
				gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_GPRS;
			}
			
#ifdef TRIDENT
			else if(mAppPara.ParaValue[0] == 0x32)    /*cdma*/
			{
				gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_CDMA;
			}
			
#endif
			else if(mAppPara.ParaValue[0] == 0x34)    /*以太网*/
			{
				gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_ETHERNET;
			}
			
			break;
			
		case 4000002: /*TPDU*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen == 4)   //4位时
			{
				strcpy(cTempBuf, "6000030000");
				memcpy(cTempBuf + 2, mAppPara.ParaValue, 4);
			}
			else if(mAppPara.ParaLen == 10) //10位时
			{
				memcpy(cTempBuf, mAppPara.ParaValue, 10);
			}
			
			AppUtils_Asc2Bcd((byte *)cTempBuf, (byte *) gTermCfg.mFixPrm.TPDU, 10);
			break;
			
		case 4000003: /*通讯超时时间*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			
			iTmp = atoi(cTempBuf);
			
			//是否应该根据通讯类型来保存？
			gTermCfg.mCommPrm.GprsTimeout = iTmp;      // Gprs超时时间
			gTermCfg.mCommPrm.CdmaTimeout = iTmp;      // Cdma超时时间
			gTermCfg.mCommPrm.EthTimeout = iTmp;       // Eth超时时间
			gTermCfg.mCommPrm.WifiTimeout = iTmp;      // WIFI超时时间
			gTermCfg.mCommPrm.TelTimeout = iTmp;       // Tel超时时间
			break;
			
		case 4000004: /*重拨次数*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			gTermCfg.mCommPrm.Retrys = atoi(cTempBuf);
			break;
			
		case 4000006: /*主机IP*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 15)
				memcpy(cTempBuf, mAppPara.ParaValue, 15);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.GprsHostIP1, cTempBuf);
			strcpy((char*)gTermCfg.mCommPrm.CdmaHostIP1, cTempBuf);
			strcpy((char*)gTermCfg.mCommPrm.WifiHostIP1, cTempBuf);
			strcpy((char*)gTermCfg.mCommPrm.EthHostIP1, cTempBuf);
			break;
			
		case 4000007: /*主机端口*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 5)
				memcpy(cTempBuf, mAppPara.ParaValue, 5);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			iTmp = atoi(cTempBuf);
			
			gTermCfg.mCommPrm.GprsHostPort1 = (unsigned short)iTmp;    // GPRS主机端口
			gTermCfg.mCommPrm.CdmaHostPort1 = (unsigned short)iTmp;    // CDMA主机端口
			gTermCfg.mCommPrm.EthHostPort1 = (unsigned short)iTmp;     // 以太网主机端口
			gTermCfg.mCommPrm.WifiHostPort1 = (unsigned short)iTmp;    // WIFI主机端口
			break;
			
		case 4000008: /*备份主机IP*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 15)
				memcpy(cTempBuf, mAppPara.ParaValue, 15);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.GprsHostIP2, cTempBuf);
			strcpy((char*)gTermCfg.mCommPrm.CdmaHostIP2, cTempBuf);
			strcpy((char*)gTermCfg.mCommPrm.WifiHostIP2, cTempBuf);
			strcpy((char*)gTermCfg.mCommPrm.EthHostIP2, cTempBuf);
			break;
			
		case 4000009: /*备份主机端口*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 5)
				memcpy(cTempBuf, mAppPara.ParaValue, 5);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			iTmp = atoi(cTempBuf);
			
			gTermCfg.mCommPrm.GprsHostPort2 = (unsigned short)iTmp;    // GPRS备份主机端口
			gTermCfg.mCommPrm.CdmaHostPort2 = (unsigned short)iTmp;    // CDMA备份主机端口
			gTermCfg.mCommPrm.EthHostPort2 = (unsigned short)iTmp;     // 以太网备份主机端口
			gTermCfg.mCommPrm.WifiHostPort2 = (unsigned short)iTmp;    // WIFI备份主机端口
			break;
			
		case 4000010: /*GPRS呼叫中心号码*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			/* 因为与其它厂家格式可能会有很多不同 暂不处理 我们默认是*99***1# */
			if(mAppPara.ParaLen > 3 && (memcmp(mAppPara.ParaValue, "ATD", 3) == 0))
			{
				memcpy(cTempBuf, mAppPara.ParaValue + 3, mAppPara.ParaLen - 3);
			}
			else if(mAppPara.ParaLen > 0)
			{
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			}
			
			strcpy((char*)gTermCfg.mCommPrm.GprsPhoneNo, cTempBuf);
			break;
			
		case 4000011: /*是否预拨*/
		
			if(mAppPara.ParaValue[0] == '1')
				gTermCfg.mTrSwitch.IsPreDialFlag = 1;
			else
				gTermCfg.mTrSwitch.IsPreDialFlag = 0;
				
			break;
			
		case 4000012: /*电话一*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen >= 20)
				memcpy(cTempBuf, mAppPara.ParaValue, 19);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.TelPhone[0], cTempBuf);
			break;
			
		case 4000013: /*电话二*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen >= 20)
				memcpy(cTempBuf, mAppPara.ParaValue, 19);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.TelPhone[1], cTempBuf);
			break;
			
		case 4000014: /*电话三*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen >= 20)
				memcpy(cTempBuf, mAppPara.ParaValue, 19);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.TelPhone[2], cTempBuf);
			break;
			
		case 4000015: /*GPRS业务参数 APN */
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 63)
				memcpy(cTempBuf, mAppPara.ParaValue, 63);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.GprsAPN, cTempBuf);
			break;
			
		case 4000016: /*CDMA登录名*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 31)
				memcpy(cTempBuf, mAppPara.ParaValue, 31);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.CdmaUserName, cTempBuf);
			break;
			
		case 4000017: /*CDMA登录口令*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 31)
				memcpy(cTempBuf, mAppPara.ParaValue, 31);
			else
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.CdmaUserPwd, cTempBuf);
			break;
			
		case 4000018: /*本机IP*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 15)
				memcpy(cTempBuf, mAppPara.ParaValue, 15);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.LocalIP, cTempBuf);
			break;
			
		case 4000019: /*掩码*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 15)
				memcpy(cTempBuf, mAppPara.ParaValue, 15);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.SubNetMask, cTempBuf);
			break;
			
		case 4000020: /*网关*/
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 15)
				memcpy(cTempBuf, mAppPara.ParaValue, 15);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			strcpy((char*)gTermCfg.mCommPrm.GateWay, cTempBuf);
			break;
			
		case 4000021: /* DNS服务器地址 */
		
			if(mAppPara.ParaLen >= 7 && mAppPara.ParaLen <= 15)
			{
				memset(cTempBuf, 0, sizeof(cTempBuf));
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				strcpy((char*)gTermCfg.mCommPrm.PriDNS, cTempBuf);
				
				memset(cTempBuf, 0, sizeof(cTempBuf));
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				strcpy((char*)gTermCfg.mCommPrm.SecDNS, cTempBuf);
			}
			
			break;
#if 0   //WIFI相关
		case 4000022: //WIFI用户名(SSID)
			memset(buffer, 0, sizeof(buffer));
			memset(TermPrm.mfixPrm.WiFiSSID, 0, sizeof(TermPrm.mfixPrm.WiFiSSID));
			
			memcpy(buffer, mAppPara.ParaValue, mAppPara.ParaLen);
			strncpy((char*)TermPrm.mfixPrm.WiFiSSID, buffer, (sizeof(TermPrm.mfixPrm.WiFiSSID) - 1));
			break;
			
		case 4000023: //WIFI密码
			memset(buffer, 0, sizeof(buffer));
			memset(TermPrm.mfixPrm.WiFiPwd, 0, sizeof(TermPrm.mfixPrm.WiFiPwd));
			
			memcpy(buffer, mAppPara.ParaValue, mAppPara.ParaLen);
			strncpy((char*)TermPrm.mfixPrm.WiFiPwd, buffer, (sizeof(TermPrm.mfixPrm.WiFiPwd) - 1));
			break;
			
		case 4000024: //WIFI加密方式
			memset(buffer, 0, sizeof(buffer));
			memcpy(buffer, mAppPara.ParaValue, mAppPara.ParaLen);
			
			memset(tmp, 0, sizeof(tmp));
			memset(temp, 0, sizeof(temp));
			
			//tms下发的转成大写字母
			for(i = 0; buffer[i] != 0; i++)
			{
				tmp[i] = toupper(buffer[i]);
			}
			
			//本地的也转成大写字母
			for(i = 0; i < VCOMM_WIFI_AUTHTYPE_MAX; i++)
			{
				for(iTemp = 0; g_VComm_Wifi_AuthType[i][iTemp] != 0; iTemp++)
				{
					temp[i][iTemp] = toupper(g_VComm_Wifi_AuthType[i][iTemp]);
				}
			}
			
			//比较
			for(i = 0; i < VCOMM_WIFI_AUTHTYPE_MAX; i++)
			{
				if(!strcmp(tmp, temp[i]))
					break;
			}
			
			if(i < VCOMM_WIFI_AUTHTYPE_MAX)
			{
				TermPrm.mfixPrm.WiFiAuthType = i;
			}
			
			break;
			
		case 4000025: //WIFI加密算法
			memset(buffer, 0, sizeof(buffer));
			memcpy(buffer, mAppPara.ParaValue, mAppPara.ParaLen);
			
			memset(tmp, 0, sizeof(tmp));
			memset(temp, 0, sizeof(temp));
			
			//tms下发的转成大写字母
			for(i = 0; buffer[i] != 0; i++)
			{
				tmp[i] = toupper(buffer[i]);
			}
			
			//本地的也转成大写字母
			for(i = 0; i < VCOMM_WIFI_EPTTYPE_MAX; i++)
			{
				for(iTemp = 0; g_VComm_Wifi_WsecType[i][iTemp] != 0; iTemp++)
				{
					temp[i][iTemp] = toupper(g_VComm_Wifi_WsecType[i][iTemp]);
				}
			}
			
			//比较
			for(i = 0; i < VCOMM_WIFI_EPTTYPE_MAX; i++)
			{
				if(!strcmp(tmp, temp[i]))
					break;
			}
			
			if(i < VCOMM_WIFI_EPTTYPE_MAX)
			{
				TermPrm.mfixPrm.WiFiAlgorithm = i;
			}
			
			break;
#endif
		case 4000026: //是否公网接入(是否开启SSL)
		
		
			if(mAppPara.ParaLen > 0)
			{
//        AppUtils_FormatWarning( (char*)"Len[%d]-[%02X, %c]", mAppPara.ParaLen, mAppPara.ParaValue[0], mAppPara.ParaValue[0] );
				if(mAppPara.ParaValue[0] == '1')
					gTermCfg.mTrSwitch.SSLCommFlag = 1;
				else
					gTermCfg.mTrSwitch.SSLCommFlag = 0;
			}
			else
			{
				gTermCfg.mTrSwitch.SSLCommFlag = 0;
			}
			
			break;
			
		case 4000027: //是否单向认证
		
			if(mAppPara.ParaLen > 0)
			{
				if(mAppPara.ParaValue[0] == '0')
					gTermCfg.mTrSwitch.OnewayAuthSSL = 0;
				else
					gTermCfg.mTrSwitch.OnewayAuthSSL = 1;
			}
			else
			{
				gTermCfg.mTrSwitch.OnewayAuthSSL = 0;
			}
			
			break;
			
		case 4000028: //是否使用DNS
		
			if(mAppPara.ParaLen > 0)
			{
				if(mAppPara.ParaValue[0] == '0')
					gTermCfg.mTrSwitch.UseDNSFlag = 0;
				else
					gTermCfg.mTrSwitch.UseDNSFlag = 1;
			}
			else
			{
				gTermCfg.mTrSwitch.UseDNSFlag = 0;
			}
			
			break;
			
		case 4000029: //SSL主机域名
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			
			memset(gTermCfg.mCommPrm.SSLSvrDomainName1, 0, sizeof(gTermCfg.mCommPrm.SSLSvrDomainName1));
			strncpy((char*)gTermCfg.mCommPrm.SSLSvrDomainName1, cTempBuf, sizeof(gTermCfg.mCommPrm.SSLSvrDomainName1) - 1);
			break;
			
		case 4000030: //SSL主机端口
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			
			gTermCfg.mCommPrm.SSLHostPort1 = (unsigned short)atoi((char*)cTempBuf);
			break;
			
		case 4000031: //SSL备份主机域名
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			
			memset(gTermCfg.mCommPrm.SSLSvrDomainName2, 0, sizeof(gTermCfg.mCommPrm.SSLSvrDomainName2));
			strncpy((char*)gTermCfg.mCommPrm.SSLSvrDomainName2, cTempBuf, sizeof(gTermCfg.mCommPrm.SSLSvrDomainName2) - 1);
			break;
			
		case 4000032: //备份主机端口
			memset(cTempBuf, 0, sizeof(cTempBuf));
			memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
			
			gTermCfg.mCommPrm.SSLHostPort2 = (unsigned short)atoi((char*)cTempBuf);
			break;
			
		case 4000033: //主DNS服务器地址(SSL通讯用)
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 15)
				memcpy(cTempBuf, mAppPara.ParaValue, 15);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			if(strlen(cTempBuf) > 0)
				strcpy(gTermCfg.mCommPrm.SSLDnsIP1, cTempBuf);
				
			break;
			
		case 4000034: //备DNS服务器地址(SSL通讯用)
			memset(cTempBuf, 0, sizeof(cTempBuf));
			
			if(mAppPara.ParaLen > 15)
				memcpy(cTempBuf, mAppPara.ParaValue, 15);
			else if(mAppPara.ParaLen > 0)
				memcpy(cTempBuf, mAppPara.ParaValue, mAppPara.ParaLen);
				
			if(strlen(cTempBuf) > 0)
				strcpy(gTermCfg.mCommPrm.SSLDnsIP2, cTempBuf);
				
			break;
			
		case 4000035: //是否使用DHCP
		
			if(mAppPara.ParaValue[0] == '0')
				gTermCfg.mTrSwitch.UseDHCPFlag = 0;
			else
				gTermCfg.mTrSwitch.UseDHCPFlag = 1;
				
			break;
			
		case 4000048: //LBS主机TPDU
			break;
			
		case 4000049: //LBS主机IP
			break;
			
		case 4000050: //LBS主机端口
			break;
			
		case 4000051: //LBS通讯超时时间
			break;
			
		default:
			break;
		}
		
	}
	
	PrmMngr_SavePrmToSamDisk();
	
	//删除参数文件
	File_Remove(ParamFileName);
	
	//删除参数文件对应的列表信息
	//(此处是否应考虑在主控中处理？列表文件的增删如果在多个应用中处理会有共享问题)
	memset(cTempBuf, 0, sizeof(cTempBuf));
	sprintf(cTempBuf, "%sPR", APPNAME);
//	TmsMngr_DelListInfo(cTempBuf);
	
	AppUtils_Cls();
	AppUtils_DisplayLine("CUPMS参数", LINE1);
	AppUtils_DisplayLine("更新完成..", LINE2);
	
	return BANK_OK;
}
//字符串的查找
int FindString(unsigned char *Buffer, int BufLen, char *Target, int TargetLen)
{
    int  i;
    int  cut;
    cut = BufLen - TargetLen + 1;

    for(i = 0; i < cut; i ++)
    {
        if(memcmp(Buffer + i, Target, TargetLen) == 0)
        {
            return i;
        }
    }

    return -1;
}


int app_read_line_data(char *fileName, char *Section, char *key, char *value)
{
    int  fp;
    char buff[4096];
    int nRet, iRet;
    long    Recs;
    char szTemp[4096];
    char szBuff[2024];
    int i;
    int num;

    fp = open(fileName, O_RDWR);
    if(fp < 0)
        return -1;

    Recs = lseek(fp, 0L, SEEK_END);
    lseek(fp, 0L, SEEK_SET);
    //读取文件
    memset(buff, 0x00, sizeof(buff));

    Recs = read(fp, (void*)buff, Recs);
    if(Recs <= 0)
    {
        qDebug("read error");

        close(fp);
        return -1;
    }

    close(fp);
    if(FindString((unsigned char *)buff, strlen(buff), Section, strlen(Section)) == -1)
    {
        return -1;
    }

    i = 0;

    while(1)
    {
        //qDebug("read buff[%s]Section[%s]", buff, Section);
        memset(szTemp, 0, sizeof(szTemp));
        nRet = FindString((unsigned char *)buff, Recs, "\x0d\x0a", 2);
        num = 2;


        if(nRet == -1)
        {
            nRet = FindString((unsigned char *)buff, Recs, "\x0a", 1);
            num = 1;
        }

        if(nRet != -1)
        {
            memset(szBuff, 0, sizeof(szBuff));
            memcpy(szBuff, buff, nRet);
            memcpy(szTemp, buff + nRet + num, Recs - nRet - num);
            memset(buff, 0, sizeof(buff));
            memcpy(buff, szTemp, Recs - nRet - num);
            Recs = Recs - nRet - num;
            i++;

        }
        else
            return 0 ;

        //PrnDisp_PrintWidthHEXASC("szBuff", (byte *)szBuff, strlen(szBuff));//test

        iRet = FindString((unsigned char *)szBuff, strlen(szBuff), key, strlen(key));

        if(iRet != -1)
        {
            memcpy(value, szBuff + iRet + 1 + strlen(key), strlen(szBuff) - 1 - iRet - strlen(key));

            return strlen(value);
        }

    }

    return 0;
}
int File_GetKey(char *Key, char *cTmpBuf, int BufLen,char *FileName)
{
    int iRet;
    //qDebug("FileName[%s]Key[%s]", FileName, Key);
    iRet = app_read_line_data(FileName, "POS", Key, cTmpBuf);
    return iRet;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InitPrmFromDLD
 * DESCRIPTION:   从DLD参数文件中获取参数(目前VXDT支持的方式)
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_InitPrmFromDLD(void *dummy)
{
    int fp;
    long lOffset;
    int iLength;
    char cAppName[ 56 ];
    char cTmpBuf[ 126 ];
    char cAscData[ 56 ];
    Bank_Operator Oper;


    // findout the dld file to check whether it is exists or not
    memset(cAppName, 0, sizeof(cAppName));
    sprintf(cAppName, "%s.INI", (char *)APPNAME);
    fp = open(cAppName, O_RDWR);

    if(fp <= 0)
    {
        memset(cAppName, 0, sizeof(cAppName));
        sprintf(cAppName, "%s.ini", (char *)APPNAME);

        fp = open(cAppName, O_RDWR);
    }

    if(fp > 0)
    {
        lOffset = lseek(fp, 0, SEEK_END);
        if(lOffset <= 0)
        {
            close(fp);
            return BANK_OK;
        }
    }
    else
    {
        return BANK_OK;
    }

    if(fp)
        close(fp);

    AppUtils_DisplayCenter("检查DLD配置文件", LINE2, FALSE);
    AppUtils_WaitKeyMill(2000);//稍作延时以便看到上述提示信息

    // found it, ok, go to  init the parameter value first
    PrmMngr_SetDefaultPrms((void *) 0);


    // get global terminal varible buffer
    PrmMngr_ReadPrmFromFlashDisk();


    //gTermCfg.mTrSwitch.Sale_Support = 1;//test
    qDebug("init DLD");

    // terminal base option
    memset(cTmpBuf, 0, sizeof(cTmpBuf));

    iLength = File_GetKey("TerminalID", cTmpBuf, 8, cAppName);
    if((iLength > 0) && (iLength <= 8))
    {
        qDebug("TerminalID[%s]", cTmpBuf);
        memset(gTermCfg.mFixPrm.TerminalID, 0, sizeof(gTermCfg.mFixPrm.TerminalID));
        memcpy(gTermCfg.mFixPrm.TerminalID, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("MerchantID", cTmpBuf, 15, cAppName);

    if((iLength > 0) && (iLength <= 15))
    {
        memset(gTermCfg.mFixPrm.MerchantID, 0, sizeof(gTermCfg.mFixPrm.MerchantID));
        memcpy(gTermCfg.mFixPrm.MerchantID, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("MerchantName", cTmpBuf, 40, cAppName);

    if((iLength > 0) && (iLength <= 40))
    {
        memset(gTermCfg.mFixPrm.MerchantNameChn, 0, sizeof(gTermCfg.mFixPrm.MerchantNameChn));
        memcpy(gTermCfg.mFixPrm.MerchantNameChn, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("MerchantName1", cTmpBuf, 40, cAppName);

    if((iLength > 0) && (iLength <= 40))
    {
        memset(gTermCfg.mFixPrm.MerchantNameEng, 0, sizeof(gTermCfg.mFixPrm.MerchantNameEng));
        memcpy(gTermCfg.mFixPrm.MerchantNameEng, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("RefundAmt", cTmpBuf, 12, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mFixPrm.RefundAmt, 0, sizeof(gTermCfg.mFixPrm.RefundAmt));
        sprintf((char *)gTermCfg.mFixPrm.RefundAmt, "%012lu", atol(cTmpBuf));
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("EC_Limit", cTmpBuf, 12, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mFixPrm.EC_Limit, 0, sizeof(gTermCfg.mFixPrm.EC_Limit));
        sprintf((char *)gTermCfg.mFixPrm.EC_Limit, "%012lu", atol(cTmpBuf));
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("OffLine_CTLS_Limit", cTmpBuf, 12, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mFixPrm.OffLine_CTLS_Limit, 0, sizeof(gTermCfg.mFixPrm.OffLine_CTLS_Limit));
        sprintf((char *)gTermCfg.mFixPrm.OffLine_CTLS_Limit, "%012lu", atol(cTmpBuf));
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CTLS_Limit", cTmpBuf, 12, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mFixPrm.CTLS_Limit, 0, sizeof(gTermCfg.mFixPrm.CTLS_Limit));
        sprintf((char *)gTermCfg.mFixPrm.CTLS_Limit, "%012lu", atol(cTmpBuf));
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CVM_Limit", cTmpBuf, 12, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mFixPrm.CVM_Limit, 0, sizeof(gTermCfg.mFixPrm.CVM_Limit));
        sprintf((char *)gTermCfg.mFixPrm.CVM_Limit, "%012lu", atol(cTmpBuf));
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("FeePercent", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        sprintf(cAscData, "%02d", atoi(cTmpBuf));
        AppUtils_Asc2Bcd((byte *)cAscData, &gTermCfg.mFixPrm.FeePercent, 2);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Receipt", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mFixPrm.Receipt = cTmpBuf[ 0 ] - 0x30;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("ReversalRetryTimes", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mFixPrm.ReversalRetryTimes = cTmpBuf[ 0 ] - 0x30;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("ContactlessTimeout", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mFixPrm.ContactlessTimeout = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("MaxTranNumber", cTmpBuf, 3, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mFixPrm.MaxTranNumber = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("OffUploadNumLimit", cTmpBuf, 3, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mFixPrm.OffUploadNumLimit = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TPDU", cTmpBuf, 4, cAppName);

    if(iLength > 0)
    {
        AppUtils_Asc2Bcd((byte *)cTmpBuf, gTermCfg.mFixPrm.TPDU + 1, 4);
    }

    // 不从参数文件更新报文头
//  memset(cTmpBuf, 0, sizeof(cTmpBuf));
//  iLength = File_GetKey("Header", cTmpBuf, 12, cAppName);
//
//  if(iLength > 0)
//  {
//    memset(gTermCfg.mFixPrm.Header, 0, sizeof(gTermCfg.mFixPrm.Header));
//    AppUtils_Asc2Bcd((byte *)cTmpBuf, gTermCfg.mFixPrm.Header, 12);
//  }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("LocalBank", cTmpBuf, 4, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mFixPrm.LocalBank, 0, sizeof(gTermCfg.mFixPrm.LocalBank));
        memcpy(gTermCfg.mFixPrm.LocalBank, cTmpBuf, 4);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("LocalArea", cTmpBuf, 4, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mFixPrm.LocalArea, 0, sizeof(gTermCfg.mFixPrm.LocalArea));
        memcpy(gTermCfg.mFixPrm.LocalArea, cTmpBuf, 4);
    }

    // pinpad option
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("KeyIndex", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mFixPrm.KeyIndex = atoi(cTmpBuf);
        AppDebug_AppLog("get master key index=[%d] from dld", gTermCfg.mFixPrm.KeyIndex);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("PP_InExtType", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '0')
            gTermCfg.mFixPrm.PP_Interface = PP_IF_INTERNAL;
        else
        {
            memset(cTmpBuf, 0, sizeof(cTmpBuf));
            iLength = File_GetKey("PP_ComType", cTmpBuf, 1, cAppName);

            if(iLength > 0)
            {
                if(cTmpBuf[ 0 ] == '0')
                    gTermCfg.mFixPrm.PP_Interface = PP_IF_COM1;
                else if(cTmpBuf[ 0 ] == '1')
                    gTermCfg.mFixPrm.PP_Interface = PP_IF_COM2;
                else if(cTmpBuf[ 0 ] == '2')
                    gTermCfg.mFixPrm.PP_Interface = PP_IF_USB;

            }
        }
    }

    //只支持内置键盘的机型，参数强制设为内置键盘  by XC 2015/3/26
    if(AppUtils_IsOnlyBuiltInPinpad() == BANK_OK)
        gTermCfg.mFixPrm.PP_Interface = PP_IF_INTERNAL;

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("PP_DesType", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '0')
            gTermCfg.mFixPrm.PP_DesType = PP222_DES_MODE;
        else
            gTermCfg.mFixPrm.PP_DesType = PP222_3DES_MODE;
    }

    // EMV option
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TerminalType", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        AppUtils_Asc2Bcd((byte *)cTmpBuf, &gTermCfg.mFixPrm.TerminalType, 2);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TerminalCap", cTmpBuf, 6, cAppName);

    if(iLength > 0)
    {
        AppUtils_Asc2Bcd((byte *)cTmpBuf, gTermCfg.mFixPrm.TerminalCap, 6);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("AddTerminalCap", cTmpBuf, 10, cAppName);

    if(iLength > 0)
    {
        AppUtils_Asc2Bcd((byte *)cTmpBuf, gTermCfg.mFixPrm.AddTerminalCap, 10);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Appl_Type", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        AppUtils_Asc2Bcd((byte *)cTmpBuf, &gTermCfg.mFixPrm.Appl_Type, 2);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("KernelOptions", cTmpBuf, 16, cAppName);

    if(iLength > 0)
    {
        AppUtils_Asc2Bcd((byte *)cTmpBuf, gTermCfg.mFixPrm.KernelOptions, 16);
    }

    // communication option
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CurrentCommMode", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        switch(cTmpBuf[ 0 ])
        {
        case '1':
            gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_RS232;
            break;
        case '2':
            gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_ETHERNET;
            break;
        case '3':
            gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_GPRS;
            break;
#ifdef TRIDENT
        case '4':
            gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_CDMA;
            break;
        case '5':
            gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_WIFI;
            break;
#endif
        case '0':
        default:
            gTermCfg.mCommPrm.CurrentCommMode = COMM_TYPE_SDLC;
            break;
        }
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Retrys", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.Retrys = atoi(cTmpBuf);
    }

    // pos ip setting
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("LocalIP", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.LocalIP, 0, sizeof(gTermCfg.mCommPrm.LocalIP));
        memcpy(gTermCfg.mCommPrm.LocalIP, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SubNetMask", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.SubNetMask, 0, sizeof(gTermCfg.mCommPrm.SubNetMask));
        memcpy(gTermCfg.mCommPrm.SubNetMask, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GateWay", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.GateWay, 0, sizeof(gTermCfg.mCommPrm.GateWay));
        memcpy(gTermCfg.mCommPrm.GateWay, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("PriDNS", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.PriDNS, 0, sizeof(gTermCfg.mCommPrm.PriDNS));
        memcpy(gTermCfg.mCommPrm.PriDNS, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SecDNS", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.SecDNS, 0, sizeof(gTermCfg.mCommPrm.SecDNS));
        memcpy(gTermCfg.mCommPrm.SecDNS, cTmpBuf, iLength);
    }

    // telephone
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TelPrefix", cTmpBuf, 9, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.TelPrefix, 0, sizeof(gTermCfg.mCommPrm.TelPrefix));
        memcpy(gTermCfg.mCommPrm.TelPrefix, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TelPhone1", cTmpBuf, 19, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.TelPhone[ 0 ], 0, sizeof(gTermCfg.mCommPrm.TelPhone[ 0 ]));
        memcpy(gTermCfg.mCommPrm.TelPhone[ 0 ], cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TelPhone2", cTmpBuf, 19, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.TelPhone[ 1 ], 0, sizeof(gTermCfg.mCommPrm.TelPhone[ 1 ]));
        memcpy(gTermCfg.mCommPrm.TelPhone[ 1 ], cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TelPhone3", cTmpBuf, 19, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.TelPhone[ 2 ], 0, sizeof(gTermCfg.mCommPrm.TelPhone[ 2 ]));
        memcpy(gTermCfg.mCommPrm.TelPhone[ 2 ], cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TelTimeout", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.TelTimeout = atoi(cTmpBuf);
    }

    // gprs
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsPhoneNo", cTmpBuf, 32, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.GprsPhoneNo, 0, sizeof(gTermCfg.mCommPrm.GprsPhoneNo));
        memcpy(gTermCfg.mCommPrm.GprsPhoneNo, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsAPN", cTmpBuf, 32, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.GprsAPN, 0, sizeof(gTermCfg.mCommPrm.GprsAPN));
        memcpy(gTermCfg.mCommPrm.GprsAPN, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsUserName", cTmpBuf, 32, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.GprsUserName, 0, sizeof(gTermCfg.mCommPrm.GprsUserName));
        memcpy(gTermCfg.mCommPrm.GprsUserName, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsUserPwd", cTmpBuf, 32, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.GprsUserPwd, 0, sizeof(gTermCfg.mCommPrm.GprsUserPwd));
        memcpy(gTermCfg.mCommPrm.GprsUserPwd, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsHostIP1", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.GprsHostIP1, 0, sizeof(gTermCfg.mCommPrm.GprsHostIP1));
        memcpy(gTermCfg.mCommPrm.GprsHostIP1, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsHostPort1", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.GprsHostPort1 = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsHostIP2", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.GprsHostIP2, 0, sizeof(gTermCfg.mCommPrm.GprsHostIP2));
        memcpy(gTermCfg.mCommPrm.GprsHostIP2, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsHostPort2", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.GprsHostPort2 = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GprsTimeout", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.GprsTimeout = atoi(cTmpBuf);
    }

    // cdma
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaPhoneNo", cTmpBuf, 32, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.CdmaPhoneNo, 0, sizeof(gTermCfg.mCommPrm.CdmaPhoneNo));
        memcpy(gTermCfg.mCommPrm.CdmaPhoneNo, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaUserName", cTmpBuf, 32, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.CdmaUserName, 0, sizeof(gTermCfg.mCommPrm.CdmaUserName));
        memcpy(gTermCfg.mCommPrm.CdmaUserName, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaUserPwd", cTmpBuf, 32, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.CdmaUserPwd, 0, sizeof(gTermCfg.mCommPrm.CdmaUserPwd));
        memcpy(gTermCfg.mCommPrm.CdmaUserPwd, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaAuthType", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.CdmaAuthType =  cTmpBuf[ 0 ] - 0x30;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaHostIP1", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.CdmaHostIP1, 0, sizeof(gTermCfg.mCommPrm.CdmaHostIP1));
        memcpy(gTermCfg.mCommPrm.CdmaHostIP1, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaHostPort1", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.CdmaHostPort1 = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaHostIP2", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.CdmaHostIP2, 0, sizeof(gTermCfg.mCommPrm.CdmaHostIP2));
        memcpy(gTermCfg.mCommPrm.CdmaHostIP2, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaHostPort2", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.CdmaHostPort2 = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CdmaTimeout", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.CdmaTimeout = atoi(cTmpBuf);
    }

    // ethernet
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("EthHostIP1", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.EthHostIP1, 0, sizeof(gTermCfg.mCommPrm.EthHostIP1));
        memcpy(gTermCfg.mCommPrm.EthHostIP1, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("EthHostPort1", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.EthHostPort1 = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("EthHostIP2", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.EthHostIP2, 0, sizeof(gTermCfg.mCommPrm.EthHostIP2));
        memcpy(gTermCfg.mCommPrm.EthHostIP2, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("EthHostPort2", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.EthHostPort2 = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("EthTimeout", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.EthTimeout = atoi(cTmpBuf);
    }

    // wifi
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("NetworkType", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.NetworkType = cTmpBuf[ 0 ] - 0x30;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("ApSSID", cTmpBuf, 16, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.ApSSID, 0, sizeof(gTermCfg.mCommPrm.ApSSID));
        memcpy(gTermCfg.mCommPrm.ApSSID , cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("ChannelID", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.ChannelID = cTmpBuf[ 0 ] - 0x30;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SecuType", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.SecuType = cTmpBuf[ 0 ] - 0x30;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("AuthType", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.AuthType = cTmpBuf[ 0 ] - 0x30;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("WepKey", cTmpBuf, 12, cAppName);

    if(iLength > 0)
    {
        memset(&gTermCfg.mCommPrm.WepKey, 0, sizeof(gTermCfg.mCommPrm.WepKey));
        memcpy(gTermCfg.mCommPrm.WepKey[ 0 ] , cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("DefaultKeyIdx", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.DefaultKeyIdx = cTmpBuf[ 0 ] - 0x30;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("WifiHostIP1", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.WifiHostIP1, 0, sizeof(gTermCfg.mCommPrm.WifiHostIP1));
        memcpy(gTermCfg.mCommPrm.WifiHostIP1, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("WifiHostPort1", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.WifiHostPort1 = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("WifiHostIP2", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.WifiHostIP2, 0, sizeof(gTermCfg.mCommPrm.WifiHostIP2));
        memcpy(gTermCfg.mCommPrm.WifiHostIP2, cTmpBuf, iLength);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("WifiHostPort2", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.WifiHostPort2 = atoi(cTmpBuf);
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("WifiTimeout", cTmpBuf, 2, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.WifiTimeout = atoi(cTmpBuf);
    }

    //公网接入相关参数
    //是否公网接入
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SSLCommFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.SSLCommFlag = 1;
        else
            gTermCfg.mTrSwitch.SSLCommFlag = 0;
    }

    //是否单向认证
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("OnewayAuthSSL", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.OnewayAuthSSL = 1;
        else
            gTermCfg.mTrSwitch.OnewayAuthSSL = 0;
    }

    //是否启用DNS
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("UseDNSFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.UseDNSFlag = 1;
        else
            gTermCfg.mTrSwitch.UseDNSFlag = 0;
    }

    //公网主机域名
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SSLSvrDomainName1", cTmpBuf, 64, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.SSLSvrDomainName1, 0, sizeof(gTermCfg.mCommPrm.SSLSvrDomainName1));
        strcpy((char *) gTermCfg.mCommPrm.SSLSvrDomainName1, cTmpBuf);
    }

    //主机端口
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SSLHostPort1", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.SSLHostPort1 = atoi(cTmpBuf);
    }

    //备份公网主机域名
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SSLSvrDomainName2", cTmpBuf, 64, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.SSLSvrDomainName2, 0, sizeof(gTermCfg.mCommPrm.SSLSvrDomainName2));
        strcpy((char *) gTermCfg.mCommPrm.SSLSvrDomainName2, cTmpBuf);
    }

    //备份主机端口
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SSLHostPort2", cTmpBuf, 5, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mCommPrm.SSLHostPort2 = atoi(cTmpBuf);
    }

    //SSL DNS1
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SSLDnsIP1", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.SSLDnsIP1, 0, sizeof(gTermCfg.mCommPrm.SSLDnsIP1));
        memcpy(gTermCfg.mCommPrm.SSLDnsIP1, cTmpBuf, iLength);
    }

    //SSL DNS2
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SSLDnsIP2", cTmpBuf, 15, cAppName);

    if(iLength > 0)
    {
        memset(gTermCfg.mCommPrm.SSLDnsIP2, 0, sizeof(gTermCfg.mCommPrm.SSLDnsIP2));
        memcpy(gTermCfg.mCommPrm.SSLDnsIP2, cTmpBuf, iLength);
    }

    // switch parameter
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("DefaultTransType", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
#if 1 //银商暂时没要求改

        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mFixPrm.DefaultTransType = 1;
        else
            gTermCfg.mFixPrm.DefaultTransType = 0;

#else

        //Modify on 2015-07-17 for 银联改为1表示消费 2表示预授权
        if(cTmpBuf[ 0 ] == '2')
            gTermCfg.mFixPrm.DefaultTransType = 2;
        else
            gTermCfg.mFixPrm.DefaultTransType = 1;

#endif
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Sale_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.Sale_Support = 1;
        else
            gTermCfg.mTrSwitch.Sale_Support = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Balance_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.Balance_Support = 1;
        else
            gTermCfg.mTrSwitch.Balance_Support = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Void_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.Void_Support = 1;
        else
            gTermCfg.mTrSwitch.Void_Support = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Refund_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.Refund_Support = 1;
        else
            gTermCfg.mTrSwitch.Refund_Support = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("PreAuth_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
        {
            gTermCfg.mTrSwitch.PreAuth_Support = 1;
            gTermCfg.mTrSwitch.PreAuth_Support = 1;
            gTermCfg.mTrSwitch.CompeleteReq_Support = 1;
            gTermCfg.mTrSwitch.CompeleteNote_Support = 1;
            gTermCfg.mTrSwitch.PreAuthVoid_Support = 1;
            gTermCfg.mTrSwitch.CompeleteVoid_Support = 1;
        }
        else
        {
            gTermCfg.mTrSwitch.PreAuth_Support = 0;
            gTermCfg.mTrSwitch.PreAuth_Support = 0;
            gTermCfg.mTrSwitch.CompeleteReq_Support = 0;
            gTermCfg.mTrSwitch.CompeleteNote_Support = 0;
            gTermCfg.mTrSwitch.PreAuthVoid_Support = 0;
            gTermCfg.mTrSwitch.CompeleteVoid_Support = 0;
        }
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Offline_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.OfflineSettle_Support = gTermCfg.mTrSwitch.SettleAdjust_Support = 1;
        else
            gTermCfg.mTrSwitch.OfflineSettle_Support = gTermCfg.mTrSwitch.SettleAdjust_Support = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("ECash_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
        {
            gTermCfg.mTrSwitch.ECash_Support = 1;
            gTermCfg.mTrSwitch.EcashLoad_Support = 1;
            gTermCfg.mTrSwitch.EcashLoadVoid_Support = 1;
            gTermCfg.mTrSwitch.EcashInLoad_Support = 1;
            gTermCfg.mTrSwitch.EcashOutLoad_Support = 1;
        }
        else
        {
            gTermCfg.mTrSwitch.ECash_Support = 0;
            gTermCfg.mTrSwitch.EcashLoad_Support = 0;
            gTermCfg.mTrSwitch.EcashLoadVoid_Support = 0;
            gTermCfg.mTrSwitch.EcashInLoad_Support = 0;
            gTermCfg.mTrSwitch.EcashOutLoad_Support = 0;
        }
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CTLS_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.CTLS_Support = 1;
        else
            gTermCfg.mTrSwitch.CTLS_Support = 0;
    }

    //非彩屏的仅支持内置键盘的机型，不支持非接  by XC 2015/3/26
    if(AppUtils_IsColorScreen() == BANK_FAIL && AppUtils_IsOnlyBuiltInPinpad() == BANK_OK)
        gTermCfg.mTrSwitch.CTLS_Support = 0;

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CTLS_ComType", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '0')
            gTermCfg.mFixPrm.CtlsInterface = CTLS_IF_INTERNAL;
        else if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mFixPrm.CtlsInterface = CTLS_IF_COM1;
        else if(cTmpBuf[ 0 ] == '2')
            gTermCfg.mFixPrm.CtlsInterface = CTLS_IF_COM2;
        else if(cTmpBuf[ 0 ] == '3')
            gTermCfg.mFixPrm.CtlsInterface = CTLS_IF_USB;
    }

    //只支持内置键盘的机型，设置为内置非接  by XC 2015/3/26
    if(AppUtils_IsOnlyBuiltInPinpad() == BANK_OK)
        gTermCfg.mFixPrm.CtlsInterface = CTLS_IF_INTERNAL;



    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("GetPanSupportCTLS", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.GetPanSupportCTLS = 1;
        else
            gTermCfg.mTrSwitch.GetPanSupportCTLS = 0;
    }

    //非接交易通道标志
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("CtlsTransChannelFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.CtlsTransChannelFlag = 1;
        else
            gTermCfg.mTrSwitch.CtlsTransChannelFlag = 0;
    }

    //内置非接，消费界面不允许挥卡(读卡器和刷卡槽相隔太近，刷卡会导致误挥卡)  by XC 2015/3/26
    //if(gTermCfg.mFixPrm.PP_Interface == CTLS_IF_INTERNAL)
    //  gTermCfg.mTrSwitch.GetPanSupportCTLS = 0;

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Wallet_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.Wallet_Support = 1;
        else
            gTermCfg.mTrSwitch.Wallet_Support = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Intall_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
        {
            gTermCfg.mTrSwitch.IntallSale_Support = 1;
            gTermCfg.mTrSwitch.IntallVoid_Support = 1;
        }
        else
        {
            gTermCfg.mTrSwitch.IntallSale_Support = 0;
            gTermCfg.mTrSwitch.IntallVoid_Support = 0;
        }
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Score_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
        {
            gTermCfg.mTrSwitch.IssuerScoreSale_Support      = 1;        // 发卡行积分消费
            gTermCfg.mTrSwitch.IssuerScoreVoid_Support      = 1;        // 发卡行积分消费撤销
            gTermCfg.mTrSwitch.UnionScoreSale_Support       = 1;        // 联盟积分消费
            gTermCfg.mTrSwitch.UnionScoreVoid_Support       = 1;        // 联盟积分撤销
            gTermCfg.mTrSwitch.UnionScoreBalance_Support    = 1;        // 联盟积分查询
            gTermCfg.mTrSwitch.UnionScoreRefund_Support     = 1;        // 联盟积分退货
        }
        else
        {
            gTermCfg.mTrSwitch.IssuerScoreSale_Support      = 0;        // 发卡行积分消
            gTermCfg.mTrSwitch.IssuerScoreVoid_Support      = 0;        // 发卡行积分消
            gTermCfg.mTrSwitch.UnionScoreSale_Support       = 0;        // 联盟积分消费
            gTermCfg.mTrSwitch.UnionScoreVoid_Support       = 0;        // 联盟积分撤销
            gTermCfg.mTrSwitch.UnionScoreBalance_Support    = 0;        // 联盟积分查询
            gTermCfg.mTrSwitch.UnionScoreRefund_Support     = 0;        // 联盟积分退货
        }
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("MChip_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
        {
            gTermCfg.mTrSwitch.MChipBalance_Support     = 1;
            gTermCfg.mTrSwitch.MChipSale_Support        = 1;
            gTermCfg.mTrSwitch.MChipVoid_Support        = 1;
            gTermCfg.mTrSwitch.MChipRefund_Support      = 1;
            gTermCfg.mTrSwitch.MChipPreAuth_Support     = 1;
            gTermCfg.mTrSwitch.MChipPreAuthVoid_Support = 1;
            gTermCfg.mTrSwitch.MChipCompleteReq_Support = 1;
            gTermCfg.mTrSwitch.MChipCompleteNote_Support = 1;
            gTermCfg.mTrSwitch.MChipCompleteVoid_Support = 1;
        }
        else
        {
            gTermCfg.mTrSwitch.MChipBalance_Support     = 0;
            gTermCfg.mTrSwitch.MChipSale_Support        = 0;
            gTermCfg.mTrSwitch.MChipVoid_Support        = 0;
            gTermCfg.mTrSwitch.MChipRefund_Support      = 0;
            gTermCfg.mTrSwitch.MChipPreAuth_Support     = 0;
            gTermCfg.mTrSwitch.MChipPreAuthVoid_Support = 0;
            gTermCfg.mTrSwitch.MChipCompleteReq_Support = 0;
            gTermCfg.mTrSwitch.MChipCompleteNote_Support = 0;
            gTermCfg.mTrSwitch.MChipCompleteVoid_Support = 0;
        }
    }

    //预约类交易
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Yuyue_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
        {
            gTermCfg.mTrSwitch.YuyueSale_Support = 1;
            gTermCfg.mTrSwitch.YuyueVoid_Support = 1;
        }
        else
        {
            gTermCfg.mTrSwitch.YuyueSale_Support = 0;
            gTermCfg.mTrSwitch.YuyueVoid_Support = 0;
        }
    }

    //订购类交易
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Order_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
        {
            gTermCfg.mTrSwitch.OrderSale_Support = 1;
            gTermCfg.mTrSwitch.OrderVoid_Support = 1;
            gTermCfg.mTrSwitch.OrderRefund_Support = 1;
            gTermCfg.mTrSwitch.OrderPreAuth_Support = 1;
            gTermCfg.mTrSwitch.OrderPreAuthVoid_Support = 1;
            gTermCfg.mTrSwitch.OrderCompleteReq_Support = 1;
            gTermCfg.mTrSwitch.OrderCompleteNote_Support = 1;
            gTermCfg.mTrSwitch.OrderCompleteVoid_Support = 1;
            gTermCfg.mTrSwitch.OrderVerifyCardHolder = 1;
        }
        else
        {
            gTermCfg.mTrSwitch.OrderSale_Support = 0;
            gTermCfg.mTrSwitch.OrderVoid_Support = 0;
            gTermCfg.mTrSwitch.OrderRefund_Support = 0;
            gTermCfg.mTrSwitch.OrderPreAuth_Support = 0;
            gTermCfg.mTrSwitch.OrderPreAuthVoid_Support = 0;
            gTermCfg.mTrSwitch.OrderCompleteReq_Support = 0;
            gTermCfg.mTrSwitch.OrderCompleteNote_Support = 0;
            gTermCfg.mTrSwitch.OrderCompleteVoid_Support = 0;
            gTermCfg.mTrSwitch.OrderVerifyCardHolder = 0;
        }
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("MagLoad_Support", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
        {
            gTermCfg.mTrSwitch.MagCashLoad_Support = 1;
            gTermCfg.mTrSwitch.MagAccountLoad_Support = 1;
        }
        else
        {
            gTermCfg.mTrSwitch.MagCashLoad_Support = 0;
            gTermCfg.mTrSwitch.MagAccountLoad_Support = 0;
        }
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("OfflineUploadMode", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mFixPrm.OfflineUploadMode = 1;
        else
            gTermCfg.mFixPrm.OfflineUploadMode = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("UploadTimes", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        gTermCfg.mFixPrm.UploadMaxTimes = atoi(cTmpBuf);
    }


    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("IsPreDialFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.IsPreDialFlag = 1;
        else
            gTermCfg.mTrSwitch.IsPreDialFlag = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("ManualFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.ManualFlag = 1;
        else
            gTermCfg.mTrSwitch.ManualFlag = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("MaskPanFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.MaskPanFlag = 1;
        else
            gTermCfg.mTrSwitch.MaskPanFlag = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("VoidSwipCard", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.VoidSwipCard = 1;
        else
            gTermCfg.mTrSwitch.VoidSwipCard = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("VoidPIN", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.VoidPIN = 1;
        else
            gTermCfg.mTrSwitch.VoidPIN = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("VoidPreauthEndSwipCard", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.VoidPreauthEndSwipCard = 1;
        else
            gTermCfg.mTrSwitch.VoidPreauthEndSwipCard = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("VoidPreAuthPIN", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.VoidPreAuthPIN = 1;
        else
            gTermCfg.mTrSwitch.VoidPreAuthPIN = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("VoidPreAuthEndPIN", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.VoidPreAuthEndPIN = 1;
        else
            gTermCfg.mTrSwitch.VoidPreAuthEndPIN = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("PreAuthEndReqPIN", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.PreAuthEndReqPIN = 1;
        else
            gTermCfg.mTrSwitch.PreAuthEndReqPIN = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("DetailPrintFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.DetailPrintFlag = 1;
        else
            gTermCfg.mTrSwitch.DetailPrintFlag = 0;
    }

    //货币代码(应用里暂未用到)
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("Currency", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        ;
    }


    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("TrackEncryptFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.TrackEncryptFlag = 1;
        else
            gTermCfg.mTrSwitch.TrackEncryptFlag = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("PartPurchaseFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.PartPurchaseFlag = 1;
        else
            gTermCfg.mTrSwitch.PartPurchaseFlag = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("EnglishSlip", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.EnglishSlip = 1;
        else
            gTermCfg.mTrSwitch.EnglishSlip = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("FeeFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.FeeFlag = 1;
        else
            gTermCfg.mTrSwitch.FeeFlag = 0;
    }

    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("AutoLogoutFlag", cTmpBuf, 1, cAppName);

    if(iLength > 0)
    {
        if(cTmpBuf[ 0 ] == '1')
            gTermCfg.mTrSwitch.AutoLogoutFlag = 1;
        else
            gTermCfg.mTrSwitch.AutoLogoutFlag = 0;
    }

    // save the new parameter
    PrmMngr_SavePrmToSamDisk();


    // 更新安全密码
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SafePWD", cTmpBuf, 6, cAppName);

    if(iLength == 6)
    {
        memset(&Oper, 0, sizeof(Bank_Operator));
        strcpy((char*)Oper.OperID, (char*)"98");
        Oper.IDLen = strlen((char*)Oper.OperID);

        if(AppUtils_OperFind(&Oper) == BANK_OK)
        {
            //此处必须复制一遍
            memcpy(Oper.OperPwd, cTmpBuf, iLength);
            Oper.PwdLen = iLength;

            AppUtils_OperUpdate(&Oper);
        }
        else
        {
            memcpy(Oper.OperPwd, cTmpBuf, iLength);
            Oper.PwdLen = iLength;
            Oper.OperType = OPER_SUPERVISOR;
            AppUtils_OperAdd(&Oper);
        }
    }


    // 更新安全密码
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("SystemMngPWD", cTmpBuf, 8, cAppName);

    if(iLength == 8)
    {
        memset(&Oper, 0, sizeof(Bank_Operator));
        strcpy((char*)Oper.OperID, (char*)"99");
        Oper.IDLen = strlen((char*)Oper.OperID);

        if(AppUtils_OperFind(&Oper) == BANK_OK)
        {
            //此处必须复制一遍
            memcpy(Oper.OperPwd, cTmpBuf, iLength);
            Oper.PwdLen = iLength;

            AppUtils_OperUpdate(&Oper);
        }
        else
        {
            memcpy(Oper.OperPwd, cTmpBuf, iLength);
            Oper.PwdLen = iLength;
            Oper.OperType = OPER_SUPERVISOR;
            AppUtils_OperAdd(&Oper);
        }
    }

#if 0  //从参数文件中读取密钥并下载到密码键盘
    memset(cTmpBuf, 0, sizeof(cTmpBuf));
    iLength = File_GetKey("MKEY", cTmpBuf, 32, cAppName);

    if(iLength == 32)
    {
        byte Buffer[32];

        memset(Buffer, 0, sizeof(Buffer));
        AppUtils_Asc2Bcd(cTmpBuf, Buffer, 32);

        if(PrmMngr_DevInitPinpad((void *)1) == BANK_OK)
        {
            if(Pinpad_LoadMasterKey(gTermCfg.mFixPrm.KeyIndex, (byte *)Buffer) != VS_SUCCESS)
            {
                return BANK_FAIL;
            }
        }
    }

#endif

    // deleted the dld file
    //AppUtils_RemoveFile();

    return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InitPrmFromINI
 * DESCRIPTION:   从INI参数文件中获取参数(预留的方式，暂未使用)
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_InitPrmFromINI(void *dummy)
{
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDefaultPrms
 * DESCRIPTION:   设置默认参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetDefaultPrms(void *dummy)
{
	int fd;
	int iParaFileLen;
	int iTermType;
	Term_Config  T_Config;
	char cTmpBuf[ 16 ];
	
    qDebug("PrmMngr_SetDefaultPrms\n");

	//打得开说明有参数文件
    if((fd = open(PARAM_FILE, O_RDONLY)) > 0)
	{
        iParaFileLen = lseek(fd, 0, SEEK_END);
        close(fd);
		
		if(iParaFileLen > 0)
		{
			if(iParaFileLen == sizeof(Term_Config))
			{
				//参数文件长度与参数结构相同，直接返回
                qDebug("OK");
                return BANK_OK;
			}
			else
			{
				//参数文件长度与参数结构不一致，需要重建参数文件，直接往后走
                //File_Remove((char*)PARAM_FILE);
			}
		}
	}
    qDebug("setdefault");
	memset(&T_Config, 0, sizeof(Term_Config));
	
    iTermType = AppUtils_GetTerminalType();

    // 按照终端型号设置默认通讯类型和密码键盘类型 by XC 2010-10-21 12:22:38
    if(iTermType == _VX680 || iTermType == _VX510G || iTermType == _VX670 || iTermType == _VX610 || iTermType == _VX675)
    {
        T_Config.mCommPrm.CurrentCommMode = COMM_TYPE_GPRS;
        T_Config.mFixPrm.PP_Interface = PP_IF_INTERNAL;
        T_Config.mFixPrm.CtlsInterface = CTLS_IF_INTERNAL;
    }
    else
    {
        T_Config.mCommPrm.CurrentCommMode = COMM_TYPE_SDLC;

        if(iTermType == _VX520 || iTermType == _VX520S)
        {
            //根据销售部门的要求，2012银商入围机型VX520热敏配置COM口键盘
            //因此默认密码键盘设备类型为COM1  by XC 2011-12-30 10:22:52
            T_Config.mFixPrm.PP_Interface = PP_IF_COM1;
            T_Config.mFixPrm.CtlsInterface = CTLS_IF_COM1;
        }
        else if(iTermType == _VX510)
        {
            T_Config.mFixPrm.PP_Interface = PP_IF_COM2;
            T_Config.mFixPrm.CtlsInterface = CTLS_IF_COM2;
        }
        else
        {
            T_Config.mFixPrm.PP_Interface = PP_IF_COM2;
            T_Config.mFixPrm.CtlsInterface = CTLS_IF_COM2;
        }
    }

    T_Config.mFixPrm.PP_DesType = 2;  // 算法类型
	T_Config.mFixPrm.KeyIndex = 0;                  // 主密钥索引
	T_Config.mFixPrm.Receipt = 2;                   // 单据联数
	T_Config.mFixPrm.ReversalRetryTimes = 3;        // 冲正次数
	T_Config.mFixPrm.UploadMaxTimes = 3;            // 离线上送次数
	T_Config.mFixPrm.OfflineUploadMode = 0;         // 离线上送模式 (0:结算前, 1:联机前)
	T_Config.mFixPrm.MaxTranNumber = 300;           // 最大交易数
	T_Config.mFixPrm.ContactlessTimeout = 60;       // 非接触卡等待时间
	T_Config.mFixPrm.OffUploadNumLimit = 10;        // 脱机交易上送笔数限制
	
	T_Config.mFixPrm.TPDU[0] = 0x60;
	T_Config.mFixPrm.TPDU[1] = 0x04;
	T_Config.mFixPrm.TPDU[2] = 0x01;
	T_Config.mFixPrm.TPDU[3] = 0x00;
	T_Config.mFixPrm.TPDU[4] = 0x00;
	
	T_Config.mFixPrm.Header[0] = 0x60;
	T_Config.mFixPrm.Header[1] = 0x31;
	T_Config.mFixPrm.Header[2] = 0x00;
	T_Config.mFixPrm.Header[3] = 0x31;
	T_Config.mFixPrm.Header[4] = 0x05;
	T_Config.mFixPrm.Header[5] = 0x00;
	
	

	
	T_Config.mFixPrm.TerminalType = 0x22;
#ifdef  MAKE_LINUX
	memcpy(T_Config.mFixPrm.TerminalCap, "\xE0\xE9\xC8", 3);
	memcpy(T_Config.mFixPrm.AddTerminalCap, "\xF0\x00\xF0\xA0\x01", 5);
#else
	memcpy(T_Config.mFixPrm.TerminalCap, "\xE0\xF9\xC8", 3);
	memcpy(T_Config.mFixPrm.AddTerminalCap, "\x60\x00\xB0\xB0\x01", 5);
#endif
	strcpy((char *)T_Config.mFixPrm.ReceiptTitle, (char *)"POS签购单");
	
	T_Config.mFixPrm.DefaultTransType  = 1;       // 默认交易类型: 1-消费; 0-预授权   //Modify on 2015-07-17 for 银联改为1表示消费 2表示预授权  //银商暂时没要求改
	strcpy((char*)T_Config.mFixPrm.LocalArea, "0000");
	strcpy((char*)T_Config.mFixPrm.LocalBank, "0000");
	T_Config.mFixPrm.ReceiptPrintDark = 2;       // 默认灰度为2
	
	//交易开关
	T_Config.mTrSwitch.Sale_Support = 1;            // 消费

	T_Config.mTrSwitch.Balance_Support = 1;         // 查询余额
	T_Config.mTrSwitch.Void_Support = 1;            // 撤销交易
	T_Config.mTrSwitch.Refund_Support = 1;          // 退货交易
	T_Config.mTrSwitch.PreAuth_Support = 1;         // 预授权交易
	T_Config.mTrSwitch.CompeleteReq_Support = 1;    // 预授权完成请求
	T_Config.mTrSwitch.CompeleteNote_Support = 1;   // 预授权完成通知
	T_Config.mTrSwitch.PreAuthVoid_Support = 1;     // 预授权撤销
	T_Config.mTrSwitch.CompeleteVoid_Support = 1;   // 预授权完成撤销
	
	T_Config.mTrSwitch.OfflineSettle_Support = 1;   // 离线结算交易
	T_Config.mTrSwitch.SettleAdjust_Support = 1;    // 结算调整交易
	
	T_Config.mTrSwitch.ECash_Support = 1;       // 是否支持电子现金
	T_Config.mTrSwitch.CTLS_Support = 1;        // 是否支持CTLS
	T_Config.mTrSwitch.Wallet_Support = 1;      // 钱包
	
	T_Config.mTrSwitch.IntallSale_Support  = 1;     // 分期
	T_Config.mTrSwitch.IntallVoid_Support = 1;
	
	T_Config.mTrSwitch.MagCashLoad_Support = 1;     // 磁条卡现金充值
	T_Config.mTrSwitch.MagAccountLoad_Support = 1;  // 磁条卡账户充值
	
	T_Config.mTrSwitch.Script_Support = 1;
	
	T_Config.mTrSwitch.EcashLoad_Support = 1;
	T_Config.mTrSwitch.EcashLoadVoid_Support = 1;
	T_Config.mTrSwitch.EcashInLoad_Support = 1;
	T_Config.mTrSwitch.EcashOutLoad_Support = 1;
	
	T_Config.mTrSwitch.YuyueSale_Support = 1;       // 预约消费
	T_Config.mTrSwitch.YuyueVoid_Support = 1;
	
	T_Config.mTrSwitch.IssuerScoreSale_Support = 1;             // 发卡行积分消费
	T_Config.mTrSwitch.IssuerScoreVoid_Support = 1;         // 发卡行积分撤销
	T_Config.mTrSwitch.UnionScoreSale_Support = 1;      // 联盟积分消费
	T_Config.mTrSwitch.UnionScoreVoid_Support = 1;      // 联盟积分撤销
	T_Config.mTrSwitch.UnionScoreBalance_Support = 1;   // 联盟积分查询
	T_Config.mTrSwitch.UnionScoreRefund_Support = 1;    // 联盟积分退货
	
	T_Config.mTrSwitch.MChipBalance_Support = 1;
	T_Config.mTrSwitch.MChipSale_Support        = 1;
	T_Config.mTrSwitch.MChipVoid_Support        = 1;
	T_Config.mTrSwitch.MChipRefund_Support  = 1;
	T_Config.mTrSwitch.MChipPreAuth_Support = 1;
	T_Config.mTrSwitch.MChipPreAuthVoid_Support = 1;
	T_Config.mTrSwitch.MChipCompleteReq_Support = 1;
	T_Config.mTrSwitch.MChipCompleteNote_Support = 1;
	T_Config.mTrSwitch.MChipCompleteVoid_Support = 1;
	
	T_Config.mTrSwitch.OrderSale_Support        = 1;
	T_Config.mTrSwitch.OrderVoid_Support        = 1;
	T_Config.mTrSwitch.OrderRefund_Support  = 1;
	T_Config.mTrSwitch.OrderPreAuth_Support = 1;
	T_Config.mTrSwitch.OrderPreAuthVoid_Support = 1;
	T_Config.mTrSwitch.OrderCompleteReq_Support = 1;
	T_Config.mTrSwitch.OrderCompleteNote_Support = 1;
	T_Config.mTrSwitch.OrderCompleteVoid_Support = 1;
	T_Config.mTrSwitch.OrderVerifyCardHolder = 1;   // 持卡人信息验证
	
	// 参数开关
	T_Config.mTrSwitch.IsPreDialFlag  = 1;      // 是否支持预拨号
	T_Config.mTrSwitch.CheckDialTone  = 1;      // 是否检测拨号音
	T_Config.mTrSwitch.ManualFlag  = 0;         // 是否手输
	T_Config.mTrSwitch.MaskPanFlag  = 1;        // 屏蔽卡号
	T_Config.mTrSwitch.VoidSwipCard  = 1;       // 撤销是否刷卡
	T_Config.mTrSwitch.VoidPIN  = 1;            // 撤销是否输入密码
	T_Config.mTrSwitch.VoidPreAuthPIN  = 1;     // 预授权撤销是否输密
	T_Config.mTrSwitch.PreAuthEndReqPIN  = 1;   // 预授权完成请求是否输密
	T_Config.mTrSwitch.VoidPreauthEndSwipCard  = 0; // 预授权完成是否刷卡
	T_Config.mTrSwitch.VoidPreAuthEndPIN  = 0;  // 预授权完成撤销是否输密
	T_Config.mTrSwitch.DetailPrintFlag = 1;     // 打印明细
	T_Config.mTrSwitch.FailDetailPrint = 0;     // 打印失败交易明细
	T_Config.mTrSwitch.TrackEncryptFlag  = 0;   // 磁道加密开关
	T_Config.mTrSwitch.PartPurchaseFlag  = 0;   // 部分承兑开关
	T_Config.mTrSwitch.EnglishSlip  = 0;        // 签购单是否打英文
	T_Config.mTrSwitch.FeeFlag  = 1;            // 是否支持小费
	T_Config.mTrSwitch.AutoLogoutFlag  = 1;     // 是否自动签退
	T_Config.mTrSwitch.MngPwdRequired  = 1;
	T_Config.mTrSwitch.CheckBlackListCard = 1;  // 检查黑名单卡
	
	T_Config.mTrSwitch.SmallAmountStandinAuth = 0;  //小额代授权 默认关闭
	T_Config.mTrSwitch.NegativeSignFlag = 0;        //负号打印开关(对于撤销类交易打印负号)
	T_Config.mTrSwitch.GetPanSupportCTLS = 1;   //默认消费界面支持挥卡操作
	T_Config.mTrSwitch.PreAuthCardNoMask = 0;
	
	T_Config.mTrSwitch.Sale_Support = 1;
	T_Config.mTrSwitch.IsPreDialFlag = 1;
	T_Config.mTrSwitch.Balance_Support = 1;
	T_Config.mTrSwitch.Void_Support = 1;
	
	T_Config.mTrSwitch.VoidSwipCard = 1;
	T_Config.mTrSwitch.VoidPIN = 1;
	T_Config.mTrSwitch.DetailPrintFlag = 0;
	
	T_Config.mTrSwitch.AccquChnName = 0;    //收单行默认打印代码
	T_Config.mTrSwitch.IssueChnName = 1;    //发卡行默认打印中文
	
	T_Config.mTrSwitch.PacketPrintDebugFlag = 0;
	
	T_Config.mTrSwitch.UserDefinedReceiptTitle = 0;
	
	T_Config.mTrSwitch.CtlsTransChannelFlag = 0;  //默认优先走联机账户
	
	// 通讯参数
	strcpy((char *) T_Config.mCommPrm.TelPrefix, "");
	strcpy((char *) T_Config.mCommPrm.TelPhone[0], (char *) "");
	strcpy((char *) T_Config.mCommPrm.TelPhone[1], (char *) "");
	strcpy((char *) T_Config.mCommPrm.TelPhone[2], (char *) "");
	T_Config.mCommPrm.ModemRegS91 = 11; //缺省值11
	
	strcpy((char *) T_Config.mCommPrm.LocalIP, "0.0.0.0");
	strcpy((char *) T_Config.mCommPrm.SubNetMask, "0.0.0.0");
	strcpy((char *) T_Config.mCommPrm.GateWay, "0.0.0.0");
	strcpy((char *) T_Config.mCommPrm.PriDNS, "0.0.0.0");
	strcpy((char *) T_Config.mCommPrm.SecDNS, "0.0.0.0");
	
	strcpy((char *) T_Config.mCommPrm.GprsPhoneNo, "*99***1#");       // GPRS拨入号码
	strcpy((char *) T_Config.mCommPrm.GprsAPN, "cmnet");            // GPRS接入点
	strcpy((char *) T_Config.mCommPrm.GprsUserName, "");            // GPRS用户名
	strcpy((char *) T_Config.mCommPrm.GprsUserPwd, "");             // GPRS密码
	
	strcpy((char *) T_Config.mCommPrm.CdmaPhoneNo, "#777");         // CDMA拨入号码
	strcpy((char *) T_Config.mCommPrm.CdmaUserName, "card");        // CDMA用户名
	strcpy((char *) T_Config.mCommPrm.CdmaUserPwd, "card");         // CDMA密码
	
	T_Config.mCommPrm.CdmaAuthType = 2;                             // 认证类型, 默认PAP_PPP
	
	T_Config.mDynPrm.SysTrace = 1;
	T_Config.mDynPrm.InvoiceNum = 1;
	T_Config.mDynPrm.BatchNum = 1;
	T_Config.mDynPrm.OfflineRecNum = 0;
	T_Config.mDynPrm.DialTimes = 0;
	T_Config.mDynPrm.DialSuccTimes = 0;
	
	T_Config.mDynPrm.BankStatus = BANK_CLOSED;
	
	strcpy(T_Config.mFixPrm.SSLCertFileName, DEFAULT_SSL_CERT_FILE_NAME);
	strcpy((char*)T_Config.mFixPrm.RefundAmt, "000000010000");
	
	T_Config.mTrSwitch.SSLCommFlag  = 0; // SSL 通讯方式支持
	T_Config.mTrSwitch.OnewayAuthSSL = 0; // 是否单向认证
	T_Config.mTrSwitch.UseDNSFlag   = 0; // 使用DNS标志
	T_Config.mTrSwitch.UseDHCPFlag  = 0; // 使用DHCP动态获取地址标志
	T_Config.mTrSwitch.UseLBSFlag   = 0; // 使用LBS基站定位服务标志
	
	memset(T_Config.mDynPrm.PINKey, 0, sizeof(T_Config.mDynPrm.PINKey));
	memset(T_Config.mDynPrm.MACKey, 0, sizeof(T_Config.mDynPrm.MACKey));
	memset(T_Config.mDynPrm.TRACKKey, 0, sizeof(T_Config.mDynPrm.TRACKKey));
	
	memset(T_Config.mDynPrm.StartUsingTime, 0, sizeof(T_Config.mDynPrm.StartUsingTime));
	
	if(AppUtils_GetClock(cTmpBuf) == BANK_OK)
		memcpy(T_Config.mDynPrm.StartUsingTime, cTmpBuf, 10);

    //qDebug("Sale_Support 121[%d]", T_Config.mTrSwitch.Sale_Support);
	//删除原来的终端参数文件
	File_Remove((char *) PARAM_FILE);

	inPrmMngr_SaveAllPrm(&T_Config);
	
	//初始化操作员文件
	PrmMngr_SetDefaultOperators((void *) 0);

	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDefaultOperators
 * DESCRIPTION:   设置默认操作员
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetDefaultOperators(void *dummy)
{
	int i;
	Bank_Operator Oper;
	char cTempBuf[32 + 1];
	
	//删除操作员文件
	File_Remove((char *) OPER_FILE);
	
	//默认主管操作员00，密码123456
	memset(&Oper, 0, sizeof(Bank_Operator));
	Oper.OperType = OPER_MANAGER;
	strcpy(Oper.OperID, "00");
	Oper.IDLen = 2;
	strcpy(Oper.OperPwd, "123456");
	Oper.PwdLen = 6;
	
	if(File_AppendRecord((char *) OPER_FILE, (char *) & Oper, sizeof(Bank_Operator)) != FILE_SUCCESS)
		return(-1);
		
	//默认系统管理员99，密码12345678
	memset(&Oper, 0, sizeof(Bank_Operator));
	Oper.OperType = OPER_SUPERVISOR;
	strcpy(Oper.OperID, "99");
	Oper.IDLen = 2;
	strcpy(Oper.OperPwd, "12345678");
	Oper.PwdLen = 8;
	
	if(File_AppendRecord((char *) OPER_FILE, (char *) & Oper, sizeof(Bank_Operator)) != FILE_SUCCESS)
		return(-2);
		
	//默认安全密码操作员98，密码888888
	memset(&Oper, 0, sizeof(Bank_Operator));
	Oper.OperType = OPER_SUPERVISOR;
	strcpy(Oper.OperID, "98");
	Oper.IDLen = 2;
	strcpy(Oper.OperPwd, "888888");
	Oper.PwdLen = 6;
	
	if(File_AppendRecord((char *) OPER_FILE, (char *) & Oper, sizeof(Bank_Operator)) != FILE_SUCCESS)
		return(-3);
		
	//默认操作员01~05，密码0000
	for(i = 1; i <= 5; i++)
	{
		memset(&Oper, 0, sizeof(Bank_Operator));
		Oper.OperType = OPER_OPERATOR;
		
		sprintf(cTempBuf, "%02d", i);
		strcpy(Oper.OperID, cTempBuf);
		Oper.IDLen = 2;
		
		strcpy(Oper.OperPwd, "0000");
		Oper.PwdLen = 4;
		
		if(File_AppendRecord((char *) OPER_FILE, (char *) & Oper, sizeof(Bank_Operator)) != FILE_SUCCESS)
			return(-4);
	}
	
	return(0);
}

/* ---------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ReadPrmFromFlashDisk
 * DESCRIPTION:   读取参数文件中的数据到全局结构gTermCfg
 * PARAMETERS     None
 * RETURN:
 * NOTES:
 * ---------------------------------------------------------------------------*/
int PrmMngr_ReadPrmFromFlashDisk(void)
{
	memset(&sTermCfg, 0, sizeof(Term_Config));

	if(inPrmMngr_GetAllPrm(&sTermCfg) != BANK_OK)
	{
		PrmMngr_SetDefaultPrms((void *) 0);

        AppUtils_Warning("读取参数失败,请重启");

		return(-1);
	}

	memset(&gTermCfg, 0, sizeof(Term_Config));
	memcpy(&gTermCfg, &sTermCfg, sizeof(Term_Config));
	
	s_init_flag = 1;
	
	return(0);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDefaultPrms
 * DESCRIPTION:   保存全局参数结构数据gTermCfg到文件
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SavePrmToSamDisk(void)
{
	//参数结构未初始化，不能回写
	if(!s_init_flag)
	{
        qDebug("gTermCfg not initiated, Save prm structure err");
		return(-1);
	}
	
	//如果参数结构和上次读出来的一样，不回写(节省FLASH寿命)
	if(memcmp(&sTermCfg, &gTermCfg, sizeof(Term_Config)) == 0)
	{
		return(0);
	}
	
	memcpy(&sTermCfg, &gTermCfg, sizeof(Term_Config));
	
	if(inPrmMngr_SaveAllPrm(&sTermCfg) != BANK_OK)
		return(-3);
		
	return(0);
}
/* ---------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InitFile
 * DESCRIPTION:   文件初始化
 * PARAMETERS     None
 * RETURN:
 * NOTES:
 * ---------------------------------------------------------------------------*/
int PrmMngr_InitFile(void)
{

    AppUtils_Cls();

    PrmMngr_InitPrmFromDLD((void *) 0);


    // get global terminal varible buffer
    if(PrmMngr_ReadPrmFromFlashDisk() != BANK_OK)
        return -1;

    SetIsoStruct();

    return 0;
}
/* ---------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetTerminalClosed
 * DESCRIPTION:   清终端已签到标志(不清除主密钥),
 *                主要用于断电重启后让终端需要重新签到
 * PARAMETERS     None
 * RETURN:
 * NOTES:
 * ---------------------------------------------------------------------------*/
int PrmMngr_SetTerminalClosed(void)
{
	memset(&sTermCfg, 0, sizeof(Term_Config));
	
	if(inPrmMngr_GetAllPrm(&sTermCfg) != BANK_OK)
	{
        printf("inPrmMngr_GetAllPrm Fail\n");
		return BANK_FAIL;
	}
	
	AppUtils_ResetStatus(&sTermCfg.mDynPrm.BankStatus, BANK_OPENED);

	if(inPrmMngr_SaveAllPrm(&sTermCfg) != BANK_OK)
	{
		return BANK_FAIL;
	}
	
	return(BANK_OK);
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetRetryTimes
 * DESCRIPTION:   设置交易重拨次数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetRetryTimes(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	AppUtils_UserClear(2);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%d", gTermCfg.mCommPrm.Retrys);
	
	if(AppUtils_GetNum(LINE2, "交易重拨次数", (char *) cTmpBuf, 1, 2, 60) <= 0)
		return(BANK_ABORT);
		
	gTermCfg.mCommPrm.Retrys = (byte)atoi(cTmpBuf);
	
	PrmMngr_SavePrmToSamDisk();
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetKeyIndex
 * DESCRIPTION:   设置主密钥索引
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetKeyIndex(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	AppUtils_UserClear(LINE2);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.KeyIndex);
	
	if(AppUtils_GetNum(LINE2, "密钥索引(0-9):", (char *) cTmpBuf, 1, 2, 60) <= 0)
	{
		return(BANK_ABORT);
	}
	
	gTermCfg.mFixPrm.KeyIndex = atoi(cTmpBuf);
	PrmMngr_SavePrmToSamDisk();
	
	//检测主密钥是否存在
//	if(Pinpad_CheckMKey(gTermCfg.mFixPrm.KeyIndex) != VS_SUCCESS)
	{
		AppUtils_FormatWarning("主密钥%d未找到", gTermCfg.mFixPrm.KeyIndex);
//		PrmMngr_DevInitPinpad((void *)1);
		
		return BANK_FAIL;
	}
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetReversalTimes
 * DESCRIPTION:   设置冲正次数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetReversalTimes(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	AppUtils_UserClear(LINE2);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.ReversalRetryTimes);
	
	if(AppUtils_GetNum(LINE2, "冲正重发次数", cTmpBuf, 1, 1, 60) <= 0)
	{
		return BANK_FAIL;
	}
	
	if(strlen(cTmpBuf) > 0)
		gTermCfg.mFixPrm.ReversalRetryTimes = atoi((char *) cTmpBuf);
		
	PrmMngr_SavePrmToSamDisk();
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetReceiptNum
 * DESCRIPTION:   热敏打印次数
 * PARAMETERS     dummy
 * RETURN:        none
 * NOTES:         none
 * ------------------------------------------------------------------------ */
int PrmMngr_SetReceiptNum(void *dummy)
{
	char cTmpBuf[32 + 1];
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.Receipt);
	
	AppUtils_UserClear(LINE2);
	
	if(AppUtils_GetNum(LINE2, "热敏打印联数", cTmpBuf, 1, 1, 60) <= 0)
	{
		return BANK_FAIL;
	}
	
	if(strlen(cTmpBuf) > 0)
		gTermCfg.mFixPrm.Receipt = atoi((char *) cTmpBuf);
		
	if(gTermCfg.mFixPrm.Receipt > 3 || gTermCfg.mFixPrm.Receipt < 1)
		gTermCfg.mFixPrm.Receipt = 2;
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetMaxTrans
 * DESCRIPTION:   设置最大交易笔数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetMaxTrans(void *dummy)
{
	char cTmpBuf[32 + 1];
	int iMaxTranNum;
	
	AppUtils_UserClear(LINE2);
	
	while(1)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "%d", gTermCfg.mFixPrm.MaxTranNumber);
		
		if(AppUtils_GetNum(LINE2, "最大交易笔数", cTmpBuf, 1, 3, 60) <= 0)
		{
			return BANK_FAIL;
		}
		
		iMaxTranNum = atoi((char *) cTmpBuf);
		
		if(iMaxTranNum > 0 && iMaxTranNum <= 500)
		{
			gTermCfg.mFixPrm.MaxTranNumber = iMaxTranNum;
			break;
		}
		else
			AppUtils_FormatWarning("输入无效, 请重输");
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetPABX
 * DESCRIPTION:   设置外线号
 * PARAMETERS     dummy: (void*)0 显示自己的标题, 并且需要验证主管密码
 *                       (void*)1 不显示标题，保留调用前的标题显示
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetPABX(void *dummy)
{
	char cTmpBuf[32 + 1];
#if defined(MAKE_LINUX)
	char cDispBuf[32+1];
#endif
#if defined(MAKE_LINUX) || defined(TRIDENT)
	int iFlag;
#endif
	
	if(dummy == (void*)0)
	{
		AppUtils_DisplayTitle("外线号码");
		
		//子应用修改级别为0
		if(PrmMngr_GetSubAppModiFlag() == 0)
		{
			AppUtils_Warning("请从主控中修改");
			return BANK_FAIL;
		}
		
		// 输入主管密码
		if(PrmMngr_InputPwd((void *)0) != BANK_OK)
			return BANK_FAIL;
	}
	
	AppUtils_UserClear(2);
	
	//设置外线号
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	
//	if(TmsMngr_GetComPrmPrePabx(cTmpBuf) != BANK_OK)
	{
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		strcpy((char *) cTmpBuf, (char *) gTermCfg.mCommPrm.TelPrefix);
		
		if(AppUtils_GetText(LINE2, "设置外线号:", cTmpBuf, 0, 9, 0, 30) < 0)
		{
			return BANK_FAIL;
		}
		
		strcpy((char *) gTermCfg.mCommPrm.TelPrefix, (char *) cTmpBuf);
	}
    //else
	{
		strcpy((char *) gTermCfg.mCommPrm.TelPrefix, (char *) cTmpBuf);
		memset(cTmpBuf, 0, sizeof(cTmpBuf));
		sprintf(cTmpBuf, "当前外线号%s", (char *) gTermCfg.mCommPrm.TelPrefix);
		AppUtils_DisplayLine((char *) cTmpBuf, LINE2);
		AppUtils_DisplayLine((char *) "若要修改外线号", LINE3);
		AppUtils_DisplayLine((char *) "请进入主控菜单", LINE4);
		AppUtils_WaitKey(30);
	}
	
#if defined(MAKE_LINUX) || defined(TRIDENT)
	//是否检测拨号音  by XC 2015/4/16 8:22:39
	iFlag = gTermCfg.mTrSwitch.CheckDialTone;
	
	if(AppUtils_YesOrNo("是否检测拨号音", "0-否  1-是", &iFlag) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.CheckDialTone = iFlag;
#endif
	
	
#ifdef MAKE_LINUX
	// S91寄存器取值
	memset(cDispBuf, 0, sizeof(cDispBuf));
	sprintf(cDispBuf, "S91(范围6~15):");
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%u", gTermCfg.mCommPrm.ModemRegS91);
	
	if(AppUtils_GetNum(LINE2, cDispBuf, cTmpBuf, 1, 2, 60) <= 0)
		return BANK_FAIL;
		
	gTermCfg.mCommPrm.ModemRegS91 = (byte)atoi(cTmpBuf);
#endif
	
	Comm_SetDialAtCmd();
	
	return BANK_OK;
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrmMngr_GetMutualPrm.
 DESCRIPTION:   设置终端参数，包括mfixPrm.mDynPrm.mtrswitch
 PARAMETERS:    none.
 RETURN:        void.
 config.ini->param_file
 *-------------------------------------------------------------------------*/
int PrmMngr_GetMutualPrm(MUTUAL_PRM *MutuPara)
{
	int     r_fh;
	MUTUAL_PRM MutuPrm;
	
	r_fh = 0;
    r_fh = open(TMS_MUTUAL_PARA_FILE, O_RDONLY);
	
	if(r_fh <= 0)
	{
		return BANK_FAIL;
	}
	
    lseek(r_fh, 0L, F_SEEK_SET);
	
	//读取文件
	memset(&MutuPrm, 0, sizeof(MUTUAL_PRM));
	
    if(read(r_fh, (void *) &MutuPrm, sizeof(MUTUAL_PRM)) != sizeof(MUTUAL_PRM))
	{
		if(r_fh)
            close(r_fh);
			
		return BANK_FAIL;
	}
	
	if(r_fh)
        close(r_fh);
		
	//读取终端参数
	memcpy(MutuPara, &MutuPrm, sizeof(MUTUAL_PRM));
	
	return BANK_OK;
}

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrmMngr_RetrieveMutualPrm
 DESCRIPTION:   获取共性参数
 PARAMETERS:
 RETURN:
 *-------------------------------------------------------------------------*/
MUTUAL_PRM *PrmMngr_RetrieveMutualPrm(void)
{
	int     r_fh;
	static  MUTUAL_PRM MutuPrm;
	
	r_fh = 0;
	
    r_fh = open(TMS_MUTUAL_PARA_FILE, O_RDONLY);
	
	if(r_fh <= 0)
	{
		return NULL;
	}
	
    lseek(r_fh, 0L, F_SEEK_SET);
	
	//读取文件
	memset(&MutuPrm, 0, sizeof(MUTUAL_PRM));
	
    if(read(r_fh, (void *) &MutuPrm, sizeof(MUTUAL_PRM)) != sizeof(MUTUAL_PRM))
	{
		if(r_fh)
            close(r_fh);
			
		return NULL;
	}
	
	if(r_fh)
        close(r_fh);
		
	return &MutuPrm;
}


/*--------------------------------------------------------------------------
 FUNCTION NAME: PrmMngr_SaveMutualPrm
 DESCRIPTION:   保存共性参数
 PARAMETERS:
 RETURN:
 *-------------------------------------------------------------------------*/
int PrmMngr_SaveMutualPrm(MUTUAL_PRM *Mutu_Para)
{
    FILE * w_fh;
	
    if((w_fh = fopen(TMS_MUTUAL_PARA_FILE, "r+")) == NULL)
	{
        if((w_fh = fopen(TMS_MUTUAL_PARA_FILE, "w+")) == NULL)
		{
			return BANK_FAIL;
		}
	}
	
    fseek(w_fh, 0L, F_SEEK_SET);
	
	//读取文件
    if(fwrite((void *) Mutu_Para, sizeof(MUTUAL_PRM), 1, w_fh) == NULL)
	{

        fclose(w_fh);
			
		return BANK_FAIL;
	}

    fclose(w_fh);
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDebug
 * DESCRIPTION:   设置调试开关
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetDebug(void *dummy)
{
	int iRet = 0, iChoice;
	byte AppDebugFlag = 0;
	char cTmpBuf[ 3 ];
	
	iChoice = gTermCfg.mTrSwitch.PacketPrintDebugFlag;
	
	if(AppUtils_YesOrNo("是否打印调试报文", "0-否    1-是", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.PacketPrintDebugFlag = iChoice;
	PrmMngr_SavePrmToSamDisk();
	
#ifdef MAKE_LINUX
	
	if(AppUtils_YesOrNo("是否开启AMGR输出", "0-否    1-是", &iRet) != BANK_OK)
		return BANK_FAIL;
		
	ApplMngr_SetLogFlag(iRet);
#endif
	

	AppUtils_Asc2Bcd((byte *)cTmpBuf, &AppDebugFlag, 2);
	
	if(AppDebugFlag & APPDEBUGMODE_PRINT)
		iRet = 1;
	else
		iRet = 0;
		
	if(AppUtils_YesOrNo("是否开启打印输出", "0-否    1-是", &iRet) != BANK_OK)
		return BANK_FAIL;
		
	if(iRet == 1)
		AppDebugFlag |= APPDEBUGMODE_PRINT;
	else
		AppDebugFlag &= ~APPDEBUGMODE_PRINT;
		
	if(AppDebugFlag & APPDEBUGMODE_COMOUTPUT)
		iRet = 1;
	else
		iRet = 0;
		
	if(AppUtils_YesOrNo("是否开启串口输出", "0-否    1-是", &iRet) != BANK_OK)
		return BANK_FAIL;
		
	if(iRet == 1)
		AppDebugFlag |= APPDEBUGMODE_COMOUTPUT;
	else
		AppDebugFlag &= ~APPDEBUGMODE_COMOUTPUT;
		
	if(AppDebugFlag & APPDEBUGMODE_DISPLAY)
		iRet = 1;
	else
		iRet = 0;
		
	if(AppUtils_YesOrNo("是否开启显示输出", "0-否    1-是", &iRet) != BANK_OK)
		return BANK_FAIL;
		
	if(iRet == 1)
		AppDebugFlag |= APPDEBUGMODE_DISPLAY;
	else
		AppDebugFlag &= ~APPDEBUGMODE_DISPLAY;
		
	if(AppDebugFlag & APPDEBUGMODE_EMVTAGDATA)
		iRet = 1;
	else
		iRet = 0;
		
	if(AppUtils_YesOrNo("是否开启ICC DEBUG", "0-否    1-是", &iRet) != BANK_OK)
		return BANK_FAIL;
		
	if(iRet == 1)
		AppDebugFlag |= APPDEBUGMODE_EMVTAGDATA;
	else
		AppDebugFlag &= ~APPDEBUGMODE_EMVTAGDATA;
		
	if(AppDebugFlag & APPDEBUGMODE_SAVEFILE)
		iRet = 1;
	else
		iRet = 0;
		
	if(AppUtils_YesOrNo("是否开启保存文件", "0-否    1-是", &iRet) != BANK_OK)
		return BANK_FAIL;
		
	if(iRet == 1)
		AppDebugFlag |= APPDEBUGMODE_SAVEFILE;
	else
		AppDebugFlag &= ~APPDEBUGMODE_SAVEFILE;
		
	if(AppDebugFlag & (APPDEBUGMODE_PRINT | APPDEBUGMODE_COMOUTPUT | APPDEBUGMODE_DISPLAY | APPDEBUGMODE_SAVEFILE))
		AppDebugFlag |= APPDEBUGMODE_PACKETDATA;
	else
		AppDebugFlag &= ~APPDEBUGMODE_PACKETDATA;
		
	memset(cTmpBuf, 0, sizeof(cTmpBuf));

	return BANK_OK;
}


/* -------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InputPwd
 * DESCRIPTION:  (void*)99: 系统管理员密码
 *               (void*)98: 安全密码
 *               (void*)0: 主管密码
 *               (void*)1: 操作员密码，验证完成把操作员号保存到gTermCfg.mDynPrm
 *               其它: 操作员密码
 * ------------------------------------------------------------------------*/
int PrmMngr_InputPwd(void *dummy)
{
	Bank_Operator   Oper;
	char cInputBuf[32 + 1];

	
	//请输入主管密码
	if(dummy == (void *)0)
	{
        if(!gTermCfg.mTrSwitch.MngPwdRequired)   //不需要输入主管密码，直接返回成功
        {
          return BANK_OK;
        }

		AppUtils_UserClear(2);
		
		//获取00-主管信息
		memset(&Oper, 0, sizeof(Bank_Operator));
		strcpy(Oper.OperID, "00");
		

        qDebug("Oper.PwdLen[%d]", Oper.PwdLen);
		memset(cInputBuf, 0, sizeof(cInputBuf));
        if(AppUtils_GetPassWord(LINE2, "请输入主管密码:", cInputBuf, 6, 6, 60) <= 0 )
        //if(AppUtils_GetPassWord(LINE2, "请输入主管密码:", cInputBuf, Oper.PwdLen, Oper.PwdLen, RIGHT_JST) < 0)
			return BANK_FAIL;
			
		if(memcmp(Oper.OperPwd, cInputBuf, Oper.PwdLen))
		{
			AppUtils_Warning((char *) "密码错误");
			return BANK_FAIL;
		}
	}
	else if(dummy == (void *) 99)
	{
		AppUtils_UserClear(2);
		
		//获取99-系统管理员信息
		memset(&Oper, 0, sizeof(Bank_Operator));
		strcpy(Oper.OperID, "99");
		

		
		//请输入系统管理员密码
		memset(cInputBuf, 0, sizeof(cInputBuf));
		
		if(AppUtils_GetPassWord(LINE2, "系统管理员密码:", cInputBuf, Oper.PwdLen, Oper.PwdLen, RIGHT_JST) < 0)
			return BANK_FAIL;
			
		//厂商参数设置
		if(memcmp(cInputBuf, "20100322", Oper.PwdLen) == 0)
		{
			PrmMngr_ManufacturerSettings((void*)0);
			return BANK_FAIL;
		}
		//调试界面开关
		else if(memcmp(cInputBuf, "19491911", Oper.PwdLen) == 0)
		{
			PrmMngr_SetDebug((void *) 0);
			return BANK_FAIL;
		}
		// 输出调试保存的文档
		else if(memcmp(cInputBuf, "19491912", Oper.PwdLen) == 0)
		{
			AppDebug_OutputFileLog();
			return BANK_FAIL;
		}
		// 手工写入测试的PBOC参数
//    else if(memcmp(cInputBuf, "19491914", Oper.PwdLen) == 0)
//    {
//      Trans_AIDDownload_Test();
//      Trans_CAKeyDownload_Test();
//      return BANK_FAIL;
//    }
		// 重新把EMV参数密钥写入到KERNEL里头
		else if(memcmp(cInputBuf, "19491915", Oper.PwdLen) == 0)
		{
			Trans_AIDSave2Kernel(dummy);
			return BANK_FAIL;
		}
		else if(memcmp(cInputBuf, "19491916", Oper.PwdLen) == 0)
		{
			Trans_CAKeySave2Kernal(dummy);
			return BANK_FAIL;
		}
		else if(memcmp(cInputBuf, "19491917", Oper.PwdLen) == 0)
		{

			return BANK_FAIL;
		}
		else if(memcmp(cInputBuf, "19491918", Oper.PwdLen) == 0)
		{
			// 是否打印明细单
			PrnDisp_PrintSettleDetail(dummy);
			
			// 打印上送失败的离线明细单
			PrnDisp_PrintJournal((void *) 1);
			PrnDisp_PrintJournal((void *) 2);
			return BANK_FAIL;
		}
		else
		{
			if(memcmp(Oper.OperPwd, cInputBuf, Oper.PwdLen))
			{
				AppUtils_Warning((char *) "密码错误");
				return BANK_FAIL;
			}
		}
	}
	else if(dummy == (void *) 98)
	{
		AppUtils_UserClear(2);
		
		//获取98-系统管理员(安全密码)信息
		memset(&Oper, 0, sizeof(Bank_Operator));
		strcpy(Oper.OperID, "98");
		

		//请输入系统管理员密码
		memset(cInputBuf, 0, sizeof(cInputBuf));
		
		if(AppUtils_GetPassWord(LINE2, "安全密码:", cInputBuf, Oper.PwdLen, Oper.PwdLen, RIGHT_JST) < 0)
			return BANK_FAIL;
			
		if(memcmp(Oper.OperPwd, cInputBuf, Oper.PwdLen))
		{
			AppUtils_Warning((char *) "密码错误");
			return BANK_FAIL;
		}
	}
	else
	{
		AppUtils_UserClear(2);
		
		//输入普通操作员ID+密码
		memset(cInputBuf, 0, sizeof(cInputBuf));
		
		if(AppUtils_GetNum(LINE2, "输入操作员号:", cInputBuf, 2, 2, 60) < 0)
			return BANK_FAIL;
			
		memset(&Oper, 0, sizeof(Bank_Operator));
		strcpy(Oper.OperID, cInputBuf);
		

		
		//请输入操作员密码
		memset(cInputBuf, 0, sizeof(cInputBuf));
		
		if(AppUtils_GetPassWord(LINE2, "输入操作员密码:", cInputBuf, Oper.PwdLen, Oper.PwdLen, RIGHT_JST) < 0)
			return BANK_FAIL;
			
		if(memcmp(Oper.OperPwd, cInputBuf, Oper.PwdLen))
		{
			AppUtils_Warning((char *) "密码错误");
			return BANK_FAIL;
		}
		
		if(dummy == (void *) 1)
		{
			strcpy((char *)gTermCfg.mDynPrm.OperID, Oper.OperID);
			PrmMngr_SavePrmToSamDisk();
		}
	}
	
	return BANK_OK;
}


/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_ChangUserPwd
* DESCRIPTION:   修改用户密码
* PARAMETERS    (void*)99: 系统管理员密码
*               (void*)98: 安全密码
*               (void*)0: 主管密码
*               其它: 操作员密码
* RETURN:
* NOTES:
* -------------------------------------------------------------------------*/
int PrmMngr_ChangUserPwd(void *dummy)
{
    return 0;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_DevInitPinpad
 * DESCRIPTION:   根据参数初始化密码键盘
 * PARAMETERS:    (void*)0: 无提示信息; (void*)1: 有提示信息;
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_DevInitPinpad(void *dummy)
{

	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetCurrentYear
 * DESCRIPTION:   设置当前年份
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetCurrentYear(void *dummy)
{

	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetTPDU
 * DESCRIPTION:   设置TPDU
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetTPDU(void *dummy)
{
	char cBuffer[ 32 ];
	
	AppUtils_UserClear(2);
	memset(cBuffer, 0, sizeof(cBuffer));
	AppUtils_Bcd2Asc((byte *) gTermCfg.mFixPrm.TPDU + 1, (byte *)cBuffer, 4);
	
	if(AppUtils_GetNum(LINE2, "TPDU", cBuffer, 4, 10, 60) < 0)
		return BANK_FAIL;
		
	if(strlen(cBuffer) == 4)
		AppUtils_Asc2Bcd((byte *) cBuffer, gTermCfg.mFixPrm.TPDU + 1, 4);
	else if(strlen(cBuffer) == 10)
		AppUtils_Asc2Bcd((byte *) cBuffer, gTermCfg.mFixPrm.TPDU, 10);
	else
		AppUtils_Warning((char *)"无效的TPDU");
		
#if 0  //Header暂时不可修改
	AppUtils_UserClear(2);
	memset(cBuffer, 0, sizeof(cBuffer));
	AppUtils_Bcd2Asc((byte *) gTermCfg.mFixPrm.Header, (byte *) cBuffer, 12);
	
	// Header: 应用类别定义2 软件总版本号2 终端状态1 处理要求1 软件分版本号6
	if(AppUtils_GetNum(LINE2, "Header:", cBuffer, 0, 12, 60) < 0)
		return BANK_FAIL;
		
	if(strlen(cBuffer) == 12)
		AppUtils_Asc2Bcd((byte *)cBuffer, gTermCfg.mFixPrm.Header, 12);
		
#endif
		
	return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetAllTransPrnFlag
 * DESCRIPTION:   设置打印全部交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetAllTransPrnFlag(void *dummy)
{
	int iChoice;
	
	AppUtils_Cls();
	iChoice = gTermCfg.mTrSwitch.AllTransDetailFlag;
	
	if(AppUtils_YesOrNo("是否打印全部交易", "0-否    1-是", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.AllTransDetailFlag = iChoice;
	
	PrmMngr_SavePrmToSamDisk();
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ClearAllTrans
 * DESCRIPTION:   清除终端流水
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ClearAllTrans(void *dummy)
{
	int iFlag = 0;
	
	if(dummy == (void *)0)
	{
		if(AppUtils_YesOrNo("是否清除交易流水", "0-否    1-是", &iFlag) != BANK_OK)
			return BANK_FAIL;
	}
	
	if(iFlag == 1 || dummy == (void *)1)
	{
		PrmMngr_ClearReversal((void *)0);
		
		AppUtils_JournalClearAll();
		
//		TmsMngr_CheckAndUpdateSettleFlag();
		
		//离线未上送笔数置为0
		gTermCfg.mDynPrm.OfflineRecNum = 0;
		PrmMngr_SavePrmToSamDisk();
		
		if(dummy == (void *)0)
		{
			PrnDisp_DisplayWarning("提示", "交易记录已清除!");
		}
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ClearReversal
 * DESCRIPTION:   手工清除冲正数据
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ClearReversal(void *dummy)
{
	int iRet;
	int iFlag = 1;
	char cTmpBuf[ 16 ];
	
	if(dummy == (void *)2)
	{
		iFlag = 0;
		
		if(AppUtils_YesOrNo("是否清除冲正流水", "0-否    1-是", &iFlag) != BANK_OK)
			return BANK_FAIL;
	}
	
	if(iFlag != 1)
		return BANK_FAIL;
		
	AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_REVERSAL);
	
	iRet = PrmMngr_SavePrmToSamDisk();
	
	if(iRet < 0)
	{
		sprintf(cTmpBuf, "清空冲正标志失败[%d]!", iRet);
		AppUtils_Warning(cTmpBuf);
		return BANK_FAIL;
	}
	
////	TmsMngr_CheckAndUpdateSettleFlag();
	
	ASSERT_FAIL(AppUtils_RemoveFile(REVERSAL_FILE));
	
	if(dummy == (void *) 1 || dummy == (void *)2)
		PrnDisp_DisplayWarning("提示", "清空冲正记录成功!");
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetPinpadType
 * DESCRIPTION:   设置密码键盘类型
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetPinpadType(void *dummy)
{
	char cBuffer[ 20 ];
	char bRet;
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("密码键盘类型", LINE1, TRUE);
	AppUtils_DisplayLine("1.外接  2.内置", LINE2);
	
	memset(cBuffer, ' ', 16);
	
	if(gTermCfg.mFixPrm.PP_Interface == PP_IF_INTERNAL)
		sprintf(cBuffer + 6, "内置(当前)");
	else
		sprintf(cBuffer + 6, "外接(当前)");
		
	AppUtils_DisplayLine(cBuffer, LINE3);
	
	while(TRUE)
	{
		bRet = AppUtils_WaitKey(30);
		
		if(bRet == '1')
		{
			if(AppUtils_GetTerminalType() == _VX520
			   || AppUtils_GetTerminalType() == _VX520S)
				gTermCfg.mFixPrm.PP_Interface = PP_IF_COM1;
			else if(AppUtils_GetTerminalType() == _VX510)
				gTermCfg.mFixPrm.PP_Interface = PP_IF_COM2;
			else
				gTermCfg.mFixPrm.PP_Interface = PP_IF_COM2;
				
			break;
		}
		else if(bRet == '2')
		{
			gTermCfg.mFixPrm.PP_Interface = PP_IF_INTERNAL;
			break;
		}
		else if(bRet == bENTER)
			break;
		else if(bRet == bESC)
			return BANK_FAIL;
	}
	
	PrmMngr_SavePrmToSamDisk();
	
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("加密算法", LINE1, TRUE);
	AppUtils_DisplayLine((char *)"1.DES     2.3DES", LINE2);
	

	
	if(PrmMngr_DevInitPinpad((void *) 1) == BANK_OK)
	{
		AppUtils_DisplayCenter((char*)"检测成功", LINE3, FALSE);
		AppUtils_WaitKey(1);
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDesType
 * DESCRIPTION:   设置加密算法类型
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetDesType(void *dummy)
{
	char cBuffer[ 20 ];
	char bRet;
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("加密算法类型", LINE1, TRUE);
	AppUtils_DisplayLine((char *)" 1.单倍长", LINE2);
	AppUtils_DisplayLine((char *)" 2.双倍长", LINE3);

	PrmMngr_SavePrmToSamDisk();
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetBankStatus
 * DESCRIPTION:   设置应用状态
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetBankStatus(byte bStatus)
{
	gTermCfg.mDynPrm.BankStatus = bStatus;
	PrmMngr_SavePrmToSamDisk();
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetPrinterDark
 * DESCRIPTION:   设置打印机深度(C520)
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetPrinterDark(void *dummy)
{
	char cTmpBuf[ 16 ];
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("打印机深度设置", LINE1, TRUE);
	
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	sprintf(cTmpBuf, "%x", gTermCfg.mFixPrm.ReceiptPrintDark);
	
	if(AppUtils_GetNum(LINE2, "深度值(0~2)", (char *) cTmpBuf, 1, 1, 60) <= 0)
		return(BANK_ABORT);
		

	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_ReceiptSettings
* DESCRIPTION:   设置签购单有关内容：小票抬头、打印字体等
* PARAMETER:
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int PrmMngr_ReceiptSettings(void *dummy)
{
	int  iChoice;
	char cDispBuf[128], cBuffer[128];
	byte bKey;
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("打印签购单设置", LINE1, TRUE);
	
	//签购单字体选择（选择签购单字体0小，1中，2大）
	AppUtils_UserClear(LINE2);
	
	memset(cDispBuf, 0, sizeof(cDispBuf));
	sprintf(cDispBuf, "选择打印字体[%u]", gTermCfg.mFixPrm.ReceiptPrintFont);
	AppUtils_DisplayLine(cDispBuf, LINE2);
	AppUtils_DisplayLine("0.小  1.中  2.大", LINE3);
	
	while(1)
	{
		bKey = AppUtils_WaitKey(60);
		
		if(bKey == bESC || bKey == 0)
			return BANK_FAIL;
		else if(bKey >= bKEY0 && bKey <= bKEY2)
		{
			gTermCfg.mFixPrm.ReceiptPrintFont = bKey - '0';
			break;
		}
		else if(bKey == bENTER)
			break;
	}
	
	PrmMngr_SavePrmToSamDisk();
	
	if(PrmMngr_GetSubAppModiFlag() <= 1)  //后面的内容不允许修改, 直接到修改打印持卡人拼音处
	{
		goto CardHolderPinyin;
	}
	
	
	//签购单抬头选择（1LOGO，0中文）
	iChoice = !gTermCfg.mTrSwitch.UserDefinedReceiptTitle;
	
	if(AppUtils_YesOrNo("请选择签购单抬头", "1.LOGO   0.中文 ", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.UserDefinedReceiptTitle = !iChoice;
	
	
	//签购单抬头名称（设置签购单抬头中文名）
	if(gTermCfg.mTrSwitch.UserDefinedReceiptTitle)
	{
		//签购单抬头设置
		if(PrmMngr_SetReceiptTitle((void *) 1) != BANK_OK)
			return BANK_FAIL;
	}
	
	
	//服务热线设置（设置服务热线号码并在签购单中打印，如果参数为空或0，则签购单不打印服务热线）
	AppUtils_Cls();
	AppUtils_DisplayCenter("打印签购单设置", LINE1, TRUE);
	
	memset(cBuffer, 0, sizeof(cBuffer));
	strcpy(cBuffer, (char*)gTermCfg.mFixPrm.HotLinePhoneNum);
	
	if(AppUtils_GetNum(LINE2, "请输入服务热线:", cBuffer, 0, 16, 60) < 0)
	{
		return BANK_FAIL;
	}
	
	if(strlen(cBuffer) > 0)
		strcpy((char*)gTermCfg.mFixPrm.HotLinePhoneNum, cBuffer);
	else
		memset(gTermCfg.mFixPrm.HotLinePhoneNum, 0, sizeof(gTermCfg.mFixPrm.HotLinePhoneNum));
		
	PrmMngr_SavePrmToSamDisk();
	
	//未知发卡行代码打印(如果为空则按原规则处理,不为空则发卡行打印本参数内容)
	if(PrmMngr_SetUnknownIssuerName((void *)0) != BANK_OK)
		return BANK_FAIL;
		
CardHolderPinyin:
	AppUtils_Cls();
	AppUtils_DisplayCenter("打印签购单设置", LINE1, TRUE);
	
	//备注栏打印持卡人姓名拼音()
	iChoice = gTermCfg.mTrSwitch.ReceiptCardHolderName;
	
	if(AppUtils_YesOrNo("打印持卡人拼音名", "0-不支持  1-支持", &iChoice) != BANK_OK)
		return BANK_FAIL;
		
	gTermCfg.mTrSwitch.ReceiptCardHolderName = iChoice;
	PrmMngr_SavePrmToSamDisk();
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_SetSSLCertFile
* DESCRIPTION:   设置SSL证书文件名
* PARAMETER:
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int PrmMngr_SetSSLCertFile(void *dummy)
{
	char cTmpBuf[32+1];
	
	//SSL证书文件名
	AppUtils_UserClear(LINE1);
	memset(cTmpBuf, 0, sizeof(cTmpBuf));
	strcpy(cTmpBuf, (char *) gTermCfg.mFixPrm.SSLCertFileName);
	
	if(AppUtils_GetText(LINE2, "SSL证书文件名", (char *) cTmpBuf, 0, 32, 0, 60) < 0)
		return(BANK_FAIL);
		
	if(strlen(cTmpBuf) > 0)
		strcpy((char *) gTermCfg.mFixPrm.SSLCertFileName, cTmpBuf);
	else
		strcpy((char *) gTermCfg.mFixPrm.SSLCertFileName, DEFAULT_SSL_CERT_FILE_NAME);
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_OperManageMenu
 * DESCRIPTION:   柜员管理菜单
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_OperManageMenu(void *dummy)
{
	unsigned char  bMenuOption;
	PrnDisp_DisplayMenuItem aMenuItem[] =
	{
		{ NULL,    1, "主管改密",   PrmMngr_ChangUserPwd, (void *) 0 },
		{ NULL,    2, "增加",       inPrmMngr_OperAdd, (void *) 0 },
		{ NULL,    3, "删    除",   inPrmMngr_OperDelete, (void *) 0 },
		{ NULL,    4, "查询",       inPrmMngr_OperInquire, (void *) 0 },
		{ NULL,    5, "柜员改密",   PrmMngr_ChangUserPwd, (void *) 1 },
		{ NULL,    6, "签退",       Trans_OperatorLogout, (void *) 1 },
	};
	
	AppUtils_Cls();
	
	if(dummy == (void *) 1)
	{
		AppUtils_Cls();
		AppUtils_DisplayCenter("柜员管理", LINE1, TRUE);
		
		// 主管密码
		if(PrmMngr_InputPwd((void *)0) != BANK_OK)
			return BANK_FAIL;
	}
	
	bMenuOption = MENUOPT_EXEC_FUNC | MENUOPT_AUTOCHECKLEN;
	PrnDisp_DisplayMenu(LINE1, "柜员管理", bMenuOption, sizeof(aMenuItem) / sizeof(PrnDisp_DisplayMenuItem), aMenuItem, 60);
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_GetMainAppMkeyIndex
 * DESCRIPTION:   获取主控的主密钥索引
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_GetMainAppMkeyIndex(void)
{
	MUTUAL_PRM MutuPrm;
	
	memset(&MutuPrm, 0, sizeof(MUTUAL_PRM));
	
	if(PrmMngr_GetMutualPrm(&MutuPrm) != BANK_OK)
		return BANK_FAIL;
		
	return MutuPrm.MainAppMKeyIndex;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_GetSubAppModiFlag
 * DESCRIPTION:   获取应用参数修改级别
 * PARAMETERS:    None
 * RETURN:        2:可以修改所有参数 1:可以修改大部分参数 0:仅可以修改部分参数
 * NOTES:         具体可修改的参数和不可修改的参数以银商规范为准
 * -------------------------------------------------------------------------*/
int PrmMngr_GetSubAppModiFlag(void)
{
	MUTUAL_PRM * ptMutu;
	int iPrmModiFlag;
	
	ptMutu = PrmMngr_RetrieveMutualPrm();
	
	if(ptMutu == NULL)  //无共性参数或单应用-认为可以修改所有内容
		return 2;
		
	if(ptMutu->MainAppOption == 1)
		return 2;
		
	iPrmModiFlag = ptMutu->SubAppPrmModifiableFlag;
	
	if(iPrmModiFlag >= 0)
		return iPrmModiFlag;
	else
		return 2;
}

/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_AddSettCount
* DESCRIPTION:   增加结算计数器(在共性参数中)
* PARAMETER:
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int PrmMngr_AddSettCount(void)
{
	MUTUAL_PRM MutuPrm;
	
	if(PrmMngr_GetMutualPrm(&MutuPrm) != BANK_OK)
	{
		return BANK_FAIL;
	}
	
	MutuPrm.TotalSettCount++;
	
	PrmMngr_SaveMutualPrm(&MutuPrm);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_CheckSettCount
 * DESCRIPTION:   检测结算计数器
 * PARAMETERS:
 * RETURN:        BANK_OK / BANK_FAIL
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_CheckSettCount(void *dummy)
{
	MUTUAL_PRM MutuPrm;
	
	if(PrmMngr_GetMutualPrm(&MutuPrm) != BANK_OK)
	{
		return BANK_FAIL;
	}
	
	if((MutuPrm.TotalSettTimes > 0) && (MutuPrm.TotalSettCount >= MutuPrm.TotalSettTimes))
	{
		return BANK_OK;
	}
	
	return BANK_FAIL;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ResetLogonData
 * DESCRIPTION:   重置签到数据
 * PARAMETERS:
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ResetLogonData(void * dummy)
{
	//清掉已签到标志
	AppUtils_ResetStatus(&gTermCfg.mDynPrm.BankStatus, BANK_OPENED);
	
	memset(gTermCfg.mDynPrm.PINKey, 0, sizeof(gTermCfg.mDynPrm.PINKey));
	memset(gTermCfg.mDynPrm.MACKey, 0, sizeof(gTermCfg.mDynPrm.MACKey));
	memset(gTermCfg.mDynPrm.TRACKKey, 0, sizeof(gTermCfg.mDynPrm.TRACKKey));
	PrmMngr_SavePrmToSamDisk();
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SaveShowName
 * DESCRIPTION:   保存应用显示名称
 * PARAMETERS:
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SaveShowName(char *AppShowName)
{

	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_GetShowName
 * DESCRIPTION:   获取应用显示名称
 * PARAMETERS:
 * RETURN:        BANK_OK: 成功
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_GetShowName(char *AppShowName)
{

	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_GetAdditionPara
 * DESCRIPTION:   解析附加参数(用中杠|隔开的参数)
 * PARAMETERS
 * RETURN:
 * NOTES:         BANK_OK: 成功
 * -------------------------------------------------------------------------*/
int PrmMngr_GetAdditionPara(int count, char *str, int strlen, char *retfield)
{
	int i, j;
	int sp, ep;
	
	j = 1;
	sp = 0;
	ep = 0;  /*起始 , 结束*/
	
	for(i = 0 ; i < strlen + 1; i++)
	{
		if(str[i] == '|' || str[i] == '\0')
		{
			sp = ep;
			ep = i;
			
			if(j == count)
			{
				if(sp == 0)
					memcpy(retfield , str, ep - sp); /*ep=2 sp=0*/
				else
					memcpy(retfield , str + sp + 1, ep - sp - 1);
					
				return BANK_OK;
			}
			
			j++;
		}
	}
	
	return BANK_FAIL;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_UnsupportSettings
 * DESCRIPTION:   不支持
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_UnsupportSettings(void *dummy)
{
	return BANK_OK;
}

/******************************************************************************
 * Private Function Definitions
 ******************************************************************************/
/* --------------------------------------------------------------------------
 * FUNCTION NAME: inPrmMngr_OperAdd
 * DESCRIPTION:   增加操作员
 * PARAMETERS
 * RETURN:
 * NOTES:         BANK_OK: 成功
 * -------------------------------------------------------------------------*/
static int inPrmMngr_OperAdd(void *dumy)
{
	Bank_Operator tmpOper;
	int iLen;
	char cPwd[32];
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("增加操作员", LINE1, TRUE);
	
	memset(&tmpOper, 0, sizeof(tmpOper));
	iLen = AppUtils_GetNum(LINE2, "请输入操作员号:", tmpOper.OperID, 2, 2, 60);
	
	if(iLen <= 0)
		return BANK_FAIL;
		
	tmpOper.IDLen = iLen;
	
	if(memcmp(tmpOper.OperID, "00", 2) == 0 ||
	   memcmp(tmpOper.OperID, "99", 2) == 0 ||
	   memcmp(tmpOper.OperID, "98", 2) == 0)
	{
		AppUtils_Warning("此编号为主管操作员,不能增加");
		return BANK_OK;
	}
	
	if(AppUtils_OperFind(&tmpOper) == BANK_OK)
	{
		AppUtils_Warning("操作员已存在!");
		return BANK_OK;
	}
	
	memset(tmpOper.OperPwd, 0, sizeof(tmpOper.OperPwd));
	iLen = AppUtils_GetPassWord(LINE2, "请输入操作员密码:", tmpOper.OperPwd, 4, 4, RIGHT_JST);
	
	if(iLen <= 0)
		return BANK_FAIL;
		
	memset(cPwd, 0, sizeof(cPwd));
	iLen = AppUtils_GetPassWord(LINE2, "请再输入一次密码:", cPwd, 4, 4, RIGHT_JST);
	
	if(iLen <= 0)
		return BANK_FAIL;
		
	if(strcmp(tmpOper.OperPwd, cPwd))
	{
		AppUtils_Warning("两次密码不一致");
		return BANK_FAIL;
	}
	
	tmpOper.PwdLen = iLen;
	tmpOper.OperType = OPER_OPERATOR;
	
	if(AppUtils_OperAdd(&tmpOper) == BANK_OK)
		AppUtils_Warning("增加操作员成功");
	else
		AppUtils_Warning("增加操作员失败");
		
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: inPrmMngr_OperDelete
 * DESCRIPTION:   删除操作员
 * PARAMETERS
 * RETURN:
 * NOTES:         BANK_OK: 成功
 * -------------------------------------------------------------------------*/
static int inPrmMngr_OperDelete(void *dumy)
{
	Bank_Operator tmpOper;
	int iLen;
	char DispBuf[32+1];
	
	AppUtils_Cls();
	AppUtils_DisplayCenter("删除操作员", LINE1, TRUE);
	
	memset(&tmpOper, 0, sizeof(tmpOper));
	iLen = AppUtils_GetNum(LINE2, "请输入操作员号:", tmpOper.OperID, 2, 2, 60);
	
	if(iLen <= 0)
		return BANK_FAIL;
		
	tmpOper.IDLen = iLen;
	
	if(memcmp(tmpOper.OperID, "00", 2) == 0)
	{
		AppUtils_Warning("主管操作员不能删除");
		return BANK_OK;
	}
	else if(memcmp(tmpOper.OperID, "99", 2) == 0 ||
	        memcmp(tmpOper.OperID, "98", 2) == 0)
	{
		AppUtils_Warning("系统管理员不能删除");
		return BANK_OK;
	}
	else if(memcmp(tmpOper.OperID, gTermCfg.mDynPrm.OperID, 2) == 0)
	{
		AppUtils_Warning("不允许删除当前操作员");
		return BANK_OK;
	}
	
	if(AppUtils_OperFind(&tmpOper) != BANK_OK)
	{
		AppUtils_Warning("此操作员不存在!");
		return BANK_OK;
	}
	
	AppUtils_FlushKbd();
	AppUtils_UserClear(LINE2);
	AppUtils_DisplayCenter("是否确认删除", LINE2, FALSE);
	sprintf(DispBuf, "操作员%s", (char*)tmpOper.OperID);
	AppUtils_DisplayCenter(DispBuf, LINE3, FALSE);
	
	if(AppUtils_WaitKey(60) == bENTER)
	{
		if(AppUtils_OperDelete(&tmpOper) == BANK_OK)
			AppUtils_Warning("删除操作员成功");
		else
			AppUtils_Warning("删除操作员失败");
	}
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: inPrmMngr_OperInquire
 * DESCRIPTION:   查询操作员
 * PARAMETERS
 * RETURN:
 * NOTES:         BANK_OK: 成功
 * -------------------------------------------------------------------------*/
static int inPrmMngr_OperInquire(void *dumy)
{
	Bank_Operator tmpOper;
	int i;
	int iCnt;
	int RecNum;
	char cBuffer[ 256 ];
	char *p;
	
	RecNum = File_GetNumberOfRecords((char *)OPER_FILE, sizeof(Bank_Operator));
	
	if(RecNum > 0)
	{
		memset(cBuffer, 0, sizeof(cBuffer));
		p = cBuffer;
		
		iCnt = 0;
		
		for(i = 0; i < RecNum; i ++)
		{
			memset(&tmpOper, 0, sizeof(Bank_Operator));
			
			if(AppUtils_GetRecordByIndex((char *)OPER_FILE, &tmpOper, sizeof(Bank_Operator), i) != BANK_OK)
				break;
				
			if(memcmp(tmpOper.OperID, "00", 2) == 0 ||
			   memcmp(tmpOper.OperID, "99", 2) == 0 ||
			   memcmp(tmpOper.OperID, "98", 2) == 0)
				continue;
				
			iCnt ++;
			
			memcpy(p, tmpOper.OperID, tmpOper.IDLen);
			p += tmpOper.IDLen;
			
			if(iCnt % 5 == 0)
			{
				memcpy(p, "  ", 2);
				p += 2;
			}
			else
			{
				memcpy(p, " ", 1);
				p ++;
			}
		}
	}
	
	AppUtils_Cls();
	
	if(strlen(cBuffer) > 0)
	{
	
		AppUtils_DisplayLine("操作员编号:", LINE1);
		AppUtils_DisplayLine(cBuffer, LINE2);
		
		if(strlen(cBuffer) > 3 * 16)
		{
			AppUtils_WaitKey(60);
			
			AppUtils_Cls();
			AppUtils_DisplayLine(cBuffer + 3 * 16, LINE1);
		}
	}
	else
	{
		AppUtils_DisplayCenter("操作员查询", LINE1, TRUE);
		AppUtils_DisplayCenter("无操作员", LINE2, FALSE);
	}
	
	AppUtils_WaitKey(60);
	
	return BANK_OK;
}

/* --------------------------------------------------------------------------
 * FUNCTION NAME: inPrmMngr_GetAllPrm
 * DESCRIPTION:   终端参数结构获取-保存到传入的指针中,返回值为成功/失败信息
 * PARAMETERS
 * RETURN:
 * NOTES:         BANK_OK: 成功
 * -------------------------------------------------------------------------*/
static int inPrmMngr_GetAllPrm(Term_Config *T_config)
{
    FILE * r_fh;
    Term_Config termCfg;
    int iRet;


    r_fh = 0;
    r_fh = fopen(PARAM_FILE, "r+");

    if(r_fh == NULL)
    {
        qDebug("Open para file[%s] failed.", PARAM_FILE);
        fclose(r_fh);
        return BANK_FAIL;
    }

    fseek(r_fh, 0, SEEK_SET);

    //读取文件
    memset(&termCfg, 0, sizeof(Term_Config));

    iRet = fread( (void *) &termCfg, sizeof(Term_Config), 1, r_fh);
    qDebug(" read iRet[%d]size[%d]",iRet , sizeof(Term_Config));
    if(iRet <= 0)
    {
        if(r_fh)
           fclose(r_fh);

        return BANK_FAIL;
    }

    //if(r_fh)
    fclose(r_fh);

    memcpy(T_config, &termCfg, sizeof(Term_Config));

    return BANK_OK;
}


/* --------------------------------------------------------------------------
 * FUNCTION NAME: inPrmMngr_GetAllPrm
 * DESCRIPTION:   终端参数结构保存
 * PARAMETERS
 * RETURN:
 * NOTES:         BANK_OK: 成功
 * -------------------------------------------------------------------------*/
static int inPrmMngr_SaveAllPrm(Term_Config *T_config)
{
    FILE * fd;
    int iRet;

    fd = fopen(PARAM_FILE, "w+");

    if(fd == NULL)
    {
        qDebug("1 open fd[%d]", fd);
        fd = fopen(PARAM_FILE, "a+");

        if(fd == NULL)
            return BANK_FAIL;
        qDebug("2 open fd[%d]", fd);
    }
    qDebug("save ok");
    //指向到文件头
    fseek(fd, 0L, SEEK_SET);

    //写入文件
    iRet = fwrite((void *) T_config, sizeof(Term_Config), 1, fd);
    if(iRet <= 0)
    {
        qDebug("write iRet[%d]", iRet);
        fclose(fd);
        return BANK_FAIL;
    }
    fclose(fd);

	return BANK_OK;
}


/*--------------------------------------------------------------------------
 FUNCTION NAME: Oil_GetAllPrm.
 DESCRIPTION:   设置终端参数，包括mfixPrm.mdynPrm.mtrswitch
 PARAMETERS:    none.  为了减少读文件次数用T_Config_Flag控制
 RETURN:        void.
 config.ini->param_file
 *-------------------------------------------------------------------------*/
int Oil_GetAllPrm(char * FileName,void * SrcData, unsigned int SrcLen)
{
    FILE * r_fh;
  //Term_Config t_config;

    //判断文件权限
    //if( access( FileName, 0 ) != 0 )
    //{
    //    return BANK_FAIL;
    //}

    //sysdebug(__FILE__,__func__,__LINE__," 打开失败%s", FileName );
    if((r_fh = fopen(FileName, "r+")) < 0)
    {
        return BANK_FAIL;
    }

    fseek(r_fh, 0L, SEEK_SET);
    //读取文件

    if(fread( (void*)SrcData, SrcLen, 1, r_fh) == 0)
    {
        fclose(r_fh);

      return BANK_FAIL;
    }

      fclose(r_fh);


  return FILE_SUCCESS;
}
int  File_GetNumberOfRecords(const char * data, int siz)
{
    int iRet;
    int r_fh;
    //sysdebug(__FILE__,__func__,__LINE__," 打开失败%s", FileName );
    if((r_fh = open(data, O_RDONLY)) < 0)
    {
        return BANK_FAIL;
    }

    iRet = lseek(r_fh, 0L, SEEK_END);

    close(r_fh);

    return iRet/siz;
}


/*--------------------------------------------------------------------------
 FUNCTION NAME: Oil_SaveIndex_Str.
 DESCRIPTION:   保存流水
 PARAMETERS:    .
 RETURN:        .
 config.ini->param_file
 *-------------------------------------------------------------------------*/
int Oil_SaveIndex_Str(char * FileName,void * buff, unsigned int buff_size, unsigned int Index)
{
  int w_fh;

  if((w_fh = open(FileName, O_WRONLY)) < 0)
  {
    if((w_fh = open(FileName, O_RDWR | O_CREAT  | O_EXCL, 0777)) < 0)
    {
      return BANK_FAIL;
    }
  }

  lseek(w_fh, buff_size*Index, SEEK_SET);
  //读取文件
  if(write(w_fh, (void*)buff, buff_size) < 0)
  {
    if(w_fh)
      close(w_fh);

    return BANK_FAIL;
  }

  if(w_fh)
    close(w_fh);

  return FILE_SUCCESS;
}
/*--------------------------------------------------------------------------
 FUNCTION NAME: Oil_GetIndex_Str.
 DESCRIPTION:   按索引读取记录
 PARAMETERS:
 RETURN:
 config.ini->param_file
 *-------------------------------------------------------------------------*/
int Oil_GetIndex_Str(char * FileName,void * buff, unsigned int buff_size, unsigned int Index)
{
    FILE   * r_fh;
    int    nret;

    if((r_fh = fopen(FileName, "r+")) == NULL)
    {
      qDebug("open error");
      return BANK_FAIL;
    }
    //qDebug("Index[%d][%d]", Index, Index*buff_size);
    fseek(r_fh, Index*buff_size, SEEK_SET);
    //读取文件

    //memset(buff, 0, sizeof(buff));
    nret = fread((void*)buff, buff_size, 1,r_fh);
    qDebug("read  nret[%d]buff_size[%d]", nret, buff_size);
    if( nret <= 0)
    {
        fclose(r_fh);

      return BANK_FAIL;
    }

    fclose(r_fh);


  return FILE_SUCCESS;
}
int File_GetRecordByIndex(char * data, void * pRetTxnRecord, int siz, int num)
{
    return Oil_GetIndex_Str(data, pRetTxnRecord, siz, num);
}
int File_UpdateRecordByIndex(char *OPLE, void* dadfadf, int num, int i)
{
    return Oil_SaveIndex_Str(OPLE, dadfadf, num, i);
}
