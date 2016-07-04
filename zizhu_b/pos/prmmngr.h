#ifndef __PRMMNGR_H
#define __PRMMNGR_H


int FindString(unsigned char *Buffer, int BufLen, char *Target, int TargetLen);
int app_read_line_data(char *fileName, char *Section, char *key, char *value);
int File_GetKey(char *Key, char *cTmpBuf, int BufLen,char *FileName);
/*--------------------------------------------------------------------------
 FUNCTION NAME: File_AppendRecord.
 DESCRIPTION:   保存记录
 PARAMETERS:    none.
 RETURN:        void.
 config.ini->param_file
 *-------------------------------------------------------------------------*/
int File_AppendRecord(char * FileName,void * SrcData, unsigned int SrcLen);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDefaultPrms
 * DESCRIPTION:   保存全局参数结构数据gTermCfg到文件
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SavePrmToSamDisk(void);

/* ---------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ReadPrmFromFlashDisk
 * DESCRIPTION:   读取参数文件中的数据到全局结构gTermCfg
 * PARAMETERS     None
 * RETURN:
 * NOTES:
 * ---------------------------------------------------------------------------*/
int PrmMngr_ReadPrmFromFlashDisk(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDefaultPrms
 * DESCRIPTION:   设置默认参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetDefaultPrms(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_TranMngSettings
 * DESCRIPTION:   交易管理设置菜单
 * PARAMETERS:    
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_TranMngSettings( void *dummy );

/* 共性参数操作函数 */
/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_GetMutualPrm
 * DESCRIPTION:   获取共性参数
 * PARAMETERS:    MutuPara: 得到的共性参数结构数据
 * RETURN:        
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_GetMutualPrm(MUTUAL_PRM *MutuPara);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_RetrieveMutualPrm
 * DESCRIPTION:   得到共性参数
 * PARAMETERS:    None
 * RETURN:        返回共性参数结构指针
 * NOTES:
 * --------------------------------------------------------------------------*/
MUTUAL_PRM * PrmMngr_RetrieveMutualPrm(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SaveMutualPrm
 * DESCRIPTION:   保存共性参数
 * PARAMETERS:    MutuPara: 待保存的共性参数结构数据
 * RETURN:        
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SaveMutualPrm(MUTUAL_PRM *Mutu_Para);


/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDefaultOperators
 * DESCRIPTION:   设置默认操作员
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetDefaultOperators(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_DevInitPinpad
 * DESCRIPTION:   根据参数初始化密码键盘
 * PARAMETERS:    (void*)0: 无提示信息; (void*)1: 有提示信息;
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_DevInitPinpad(void *dummy);


int PrmMngr_CheckPrmInitFlag(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_SysManage
 * DESCRIPTION:   系统管理菜单
 * PARAMETERS:    (void*)0:需要密码验证;  (void*)1:不需要密码验证;
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_SysManage(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetMerchantName
 * DESCRIPTION:   设置商户名称
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetMerchantName(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetCurrentYear
 * DESCRIPTION:   设置当前年份
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetCurrentYear(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetMaxRefundAmt
 * DESCRIPTION:   设置最大退货金额
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetMaxRefundAmt(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_MerchantPrmSettings
 * DESCRIPTION:   商户参数设置
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_MerchantPrmSettings(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_CommPrmSettings
 * DESCRIPTION:   通讯参数设置
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_CommPrmSettings(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_KeyManage
 * DESCRIPTION:   终端密钥管理(菜单)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_KeyManage(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_ChangePwd
 * DESCRIPTION:   修改密码(菜单)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_ChangePwd(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_Others
 * DESCRIPTION:   其它功能(菜单)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_Others(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetPABX
 * DESCRIPTION:   设置外线号
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetPABX(void *dummy);

/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_ChangUserPwd
* DESCRIPTION:   修改用户密码
* PARAMETERS    (void*)99: 系统管理员密码
*               (void*)98: 安全密码
*               (void*)0: 主管密码
*				其它: 操作员密码
* RETURN:
* NOTES:
* -------------------------------------------------------------------------*/
int PrmMngr_ChangUserPwd(void *dummy);

/* -------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InputPwd
 * DESCRIPTION:  (void*)99: 系统管理员密码
 *               (void*)98: 安全密码
 *               (void*)0: 主管密码
 *				 其它: 操作员密码
 * ------------------------------------------------------------------------*/
int PrmMngr_InputPwd(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InitPrmFromDLD
 * DESCRIPTION:   从DLD参数文件中获取参数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_InitPrmFromDLD(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InitPrmFromTMS
 * DESCRIPTION:   从TMS参数文件中获取参数
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_InitPrmFromTMS(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_AddSystrace
 * DESCRIPTION:   增加系统流水号
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_AddSystrace(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ClearAllTrans
 * DESCRIPTION:   清除终端流水
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ClearAllTrans( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ClearReversal
 * DESCRIPTION:   清除冲正标记
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ClearReversal( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetPinpadType
 * DESCRIPTION:   设置密码键盘类型
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetPinpadType( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetKeyIndex
 * DESCRIPTION:   设置主密钥索引
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetKeyIndex( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SysPrmSettings
 * DESCRIPTION:   系统参数设置
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SysPrmSettings( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetMaxTrans
 * DESCRIPTION:   设置最大交易笔数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetMaxTrans( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_TranSwitchSettings
 * DESCRIPTION:   交易开关控制
 * PARAMETERS:    
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_TranSwitchSettings( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetTPDU
 * DESCRIPTION:   设置TPDU
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetTPDU(void *dummy);

int PrmMngr_SetReversalTimes( void *dummy );
int PrmMngr_SetTerminalID( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetReceiptTitle
 * DESCRIPTION:   设置签购单抬头
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetReceiptTitle( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetReceiptNum
 * DESCRIPTION:   热敏打印次数
 * PARAMETERS     dummy
 * RETURN:        none
 * NOTES:         none
 * ------------------------------------------------------------------------ */
int PrmMngr_SetReceiptNum( void *dummy );

int PrmMngr_SetMerchantID( void *dummy );
int PrmMngr_SetMerchantName( void *dummy );
int PrmMngr_SetSystemTrace( void *dummy );
int PrmMngr_SetBatchNo( void *dummy );
int PrmMngr_SetMaxRefundAmt( void *dummy );
int PrmMngr_Comm_SetGprs( void *dummy );
int PrmMngr_Comm_SetCdma( void *dummy );
int PrmMngr_Comm_SetWifi( void *dummy );
int PrmMngr_Comm_SetTel( void *dummy );
int PrmMngr_Comm_SetEth( void *dummy );
int PrmMngr_Comm_RS232( void *dummy );
int PrmMngr_Comm_Timeout( int CommMode );

/*
  柜员管理菜单
*/
int PrmMngr_OperManageMenu(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDesType
 * DESCRIPTION:   设置加密算法类型
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetDesType( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetRetryTimes
 * DESCRIPTION:   设置交易重拨次数
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetRetryTimes( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetPrinterDark
 * DESCRIPTION:   设置热敏打印机深度
 * PARAMETERS:
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetPrinterDark( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_GetMainAppMkeyIndex
 * DESCRIPTION:   获取主控的主密钥索引
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_GetMainAppMkeyIndex(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Menu_Clear
 * DESCRIPTION:   清除流水(菜单)
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Menu_Clear(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_GetSubAppModiFlag
 * DESCRIPTION:   获取应用参数修改级别
 * PARAMETERS:    None
 * RETURN:        2:可以修改所有参数 1:可以修改大部分参数 0:仅可以修改部分参数
 * NOTES:         具体可修改的参数和不可修改的参数以银商规范为准
 * -------------------------------------------------------------------------*/
int PrmMngr_GetSubAppModiFlag(void);

/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_ReceiptSettings
* DESCRIPTION:   设置签购单有关内容：小票抬头、打印字体等
* PARAMETER:
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int PrmMngr_ReceiptSettings(void *dummy);

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrmMngr_SaveShowName
 DESCRIPTION:   保存应用显示名称
 PARAMETERS:
 RETURN:
 *-------------------------------------------------------------------------*/
int PrmMngr_SaveShowName(char *AppShowName);

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrmMngr_GetShowName.
 DESCRIPTION:   获取应用显示名称
 PARAMETERS:    
 RETURN:        BANK_OK: 成功
 *-------------------------------------------------------------------------*/
int PrmMngr_GetShowName(char *AppShowName);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetAppDispName
 * DESCRIPTION:   设置应用显示名称(子应用名称)
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetAppDispName(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetCommMode
 * DESCRIPTION:   选择通讯类型
 * PARAMETERS:    dummy: (void*)1 需要验证主管密码
 * RETURN:        BANK_OK
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetCommMode(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_Comm_InternetPrm
 * DESCRIPTION:   设置公网参数
 * PARAMETERS:    None
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_Comm_InternetPrm(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetUnknownIssuerName
 * DESCRIPTION:   设置未知发卡行名称
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * -------------------------------------------------------------------------*/
int PrmMngr_SetUnknownIssuerName(void *dummy);

/*--------------------------------------------------------------------------
 FUNCTION NAME: PrmMngr_ResetLogonData
 DESCRIPTION:   重置签到数据
 PARAMETERS:
 RETURN:
 *-------------------------------------------------------------------------*/
int PrmMngr_ResetLogonData( void * dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ManufacturerSettings
 * DESCRIPTION:   厂商参数设置菜单
 * PARAMETERS:    
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ManufacturerSettings(void *dummy);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_ManufacturerOthers
 * DESCRIPTION:   厂商参数设置的其他参数设置
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_ManufacturerOthers(void *dummy);

/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_SetSSLCertFile
* DESCRIPTION:   设置SSL证书文件名
* PARAMETER:
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int PrmMngr_SetSSLCertFile( void *dummy );

/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_AddSettCount
* DESCRIPTION:   增加结算计数器(在共性参数中)
* PARAMETER:
* NOTES:
* RETURN:
* ------------------------------------------------------------------------ */
int PrmMngr_AddSettCount(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetDebug
 * DESCRIPTION:   设置调试开关
 * PARAMETERS     dummy
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int PrmMngr_SetDebug(void *dummy);

/* --------------------------------------------------------------------------
* FUNCTION NAME: PrmMngr_GetAdditionPara
* DESCRIPTION:   解析附加参数(用中杠|隔开的参数)
* PARAMETERS     
* RETURN:
* NOTES:         BANK_OK: 成功
* -------------------------------------------------------------------------*/
int PrmMngr_GetAdditionPara(int count, char *str, int strlen, char *retfield);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetAllTransPrnFlag
 * DESCRIPTION:   设置打印全部交易菜单
 * PARAMETERS:    dummy
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_SetAllTransPrnFlag(void *dummy);

/* ---------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_InitFile
 * DESCRIPTION:   文件初始化
 * PARAMETERS     None
 * RETURN:
 * NOTES:
 * ---------------------------------------------------------------------------*/
int PrmMngr_InitFile(void);
/* ---------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_SetTerminalClosed
 * DESCRIPTION:   清终端已签到标志(不清除主密钥), 
 *                主要用于断电重启后让终端需要重新签到
 * PARAMETERS     None
 * RETURN:
 * NOTES:
 * ---------------------------------------------------------------------------*/
int PrmMngr_SetTerminalClosed(void);

/* --------------------------------------------------------------------------
 * FUNCTION NAME: PrmMngr_UnsupportSettings
 * DESCRIPTION:   不支持
 * PARAMETERS:    None.
 * RETURN:
 * NOTES:
 * --------------------------------------------------------------------------*/
int PrmMngr_UnsupportSettings(void *dummy);
int File_GetNumberOfRecords(const char * data, int siz);

int File_GetRecordByIndex(char *data,void *pRetTxnRecord, int siz, int num);
#endif
