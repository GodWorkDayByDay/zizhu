/****************************************************************************
 * FILE NAME:   define.h                                                    *
 * MODULE NAME: DEFINE                                                      *
 * PROGRAMMER:  Chuan Xi / Laikey                                           *
 *
 ***************************************************************************/
#ifndef __DEFINE_H
#define __DEFINE_H


/////////////////////////////////////////////////////////////////////////////////
//类型定义
#ifndef COMM_TYPE_RS232
#define COMM_TYPE_RS232 (COMM_TYPE_GPRS+3)
#endif

#ifndef STX
#define STX (0x02)
#endif

#ifndef ETX
#define ETX (0x03)
#endif

////////////////////////////////////////////////////////////////////////////
//定义应用名称－非常重要!!! 应用移植时需要修改 需要与应用.out名称手工匹配
#define APPNAME   "CUPMS"

//  C520平台的文件路径有差异  使用LINUX宏定义区分
#define RAM_PREFIX        "I:"
#define FLASH_PREFIX      "F:"

//  文件名与应用名称关联，以尽量减少不同应用在同一个GROUP可能造成的冲突 
//  FLASH存储文件－仅供读取使用 
#define RESP_FILE		"F:" APPNAME "_CODE.INI"   	//错误返回码文件
#define BANK_FILE		"F:" APPNAME "_BANK.INI"   	//银行名称文件

//  RAMDISK文件－供读写用 
#define PARAM_FILE		"I" APPNAME "_PRMFILE"    	//终端存储参数文件
#define OPER_FILE		"I" APPNAME "_OPERFILE"   	//操作员文件
#define REVERSAL_FILE	"I" APPNAME "_REVDATA"    	//冲正数据文件
#define TRAN_DB			"I" APPNAME "_TRANDB"     	//交易数据库文件
#define TRAN_DB_FAIL	"I" APPNAME "_TRANDB2"		//失败脱机交易数据库文件
#define TRAN_DB_SCRIPT	"I" APPNAME "_TRANDB3"		//脚本文件数据库文件
#define TRAN_TOTAL		"I" APPNAME "_TRANTOTAL"  	//交易汇总数据库文件

#define AIDtmp_FILE		"I" APPNAME "_AIDtmp"    	// EMV/PBOC temporary AID parameter
#define CTLSAIDtmp_FILE "I" APPNAME "_CTLSAIDtmp"	// EMV/PBOC temporary AID parameter for CTLS reader
#define CAKEYtmp_FILE	"I" APPNAME "_CAKEYtmp"   	// EMV/PBOC temporary CAKEY parameter
#define AID_FILE		"I" APPNAME "_AID"    		// EMV/PBOC AID parameter
#define CTLSAID_FILE 	"I" APPNAME "_CTLSAID"     // EMV/PBOC AID parameter
#define CAKEY_FILE		"I" APPNAME "_CAKEY"      	// EMV/PBOC CAKEY parameter
#define CTLSAIDLIST_FILE "I" APPNAME "_CTLSAIDINF"   // EMV/PBOC AID list parameter for CTLS
#define APPDEBUG_FILE	"I" APPNAME "AppDebug.log"	// App Debug file

#define BLACK_LIST_CARD "IBlackListedCards.txt"	// 黑名单文件列表
#define APP_DISPNAME_FILE "I" APPNAME "_SHOWNAME"	// 应用显示名称文件

#define SLIP_PRN_LOGO 	"FSlipHeader.LGO"
#define SLIP_PRN_BMP	"./SlipHeader.bmp"

#define DEFAULT_SSL_CERT_FILE_NAME	"umscert.pem"

#define	MENU_VxBUTTON	"IA8X16.vft"				// 非彩屏类菜单上一屏下一屏按钮
#define MENU_NEXTBUTTON	"Fnext_button.bmp"			// 菜单下一屏按钮
#define MENU_PREBUTTON	"Fpre_button.bmp"			// 菜单上一屏按钮

#define SIGNALBMP       "F:CS.bmp"					// 信号量

#define REVERSAL_CLEAR_STATUS   0x01
#define REVERSAL_CLEAR_DATA     0x02

// 终端状态标志，采用字节位方式，供动态参数中BankStatus使用
#define BANK_CLOSED     	0x00
#define BANK_OPENED     	0x01
#define BANK_LOCKTERM     	0x02
#define BANK_REVERSAL   	0x04
#define BANK_UPLOAD_BFST   	0x08
#define BANK_UPLOAD_CUP 	0x10
#define BANK_UPLOAD_FGN 	0x20
#define BANK_UPLOAD_SCRIPT	0x40
#define	BANK_UPLOAD_TC		0x80


// 交易状态定义――采用字节位方式，方便进行条件组合判断，减小存储空间
#define STATUS_UPLOAD   	0x01    // 已上传 
#define STATUS_ADJUST   	0x02    // 已调整 
#define STATUS_BATCH    	0x04    // 已批上送 
#define STATUS_OFFLINE  	0x08    // 脱机 
#define STATUS_VOID     	0x10    // 已经撤销 
#define STATUS_TIP			0x20	// 有小费
#define STATUS_TCUPLOAD		0x40	// TC上送
#define STATUS_FALLBACK		0x80    // FALLBACK

#define STATUS_SCRIPT		0x01	// 脚本上送
#define	STATUS_UPLFAIL_TO	0x02	// 结算前上送失败,接收超时
#define STATUS_UPLFAIL_ER	0X04	// 结算前上送失败,主机返回错误

#define	STATUS_AIDDOWN		0x01	// AID参数已经下载
#define	STATUS_CAKEYDOWN 	0x02	// CAKEY参数已经下载
#define STATUS_HMDDOWN		0x04	// 黑名单已经下载

// 当前卡类型
#define  TRAN_ESC       (unsigned short)0x0000    // 无效类型
#define  TRAN_NUM       (unsigned short)0x0001   // 手输卡号
#define  TRAN_SWIP      (unsigned short)0x0002   // 刷卡交易
#define  TRAN_ICC       (unsigned short)0x0004   // IC卡交易
#define  TRAN_QPBOC     (unsigned short)0x0008   // QPBOC transaction

//回退交易状态 以STATUS_FALLBACK为准  不使用TRAN_FALLBACK
//#define  TRAN_FALLBACK  (unsigned short)0x0010   // IC卡回退交易

#define  TRAN_ECASH     (unsigned short)0x0020   // 电子现金IC卡交易
#define	 TRAN_IDLECARD  (unsigned short)0x0040	 // 待机界面已经刷卡或者插卡
#define	 TRAN_MCHIP     (unsigned short)0x0080	 // 手机芯片卡
#define	 TRAN_NOCARD    (unsigned short)0x0100	 // 无卡支付类型(预约交易用)
#define	 TRAN_CTLSPBOC  (unsigned short)0x0200	 // 非接PBOC
#define	 TRAN_ICFORLOG	(unsigned short)0x0400	 // 应用选择forlog模式,适用于C520

#define  CURRENCY_NAME    ""    // "RMB"
#define  PRNCURR_NAME     "RMB "
#define  CURRENCY_CODE    "156" // 货币代码


//#define  byte unsigned char
//#define  ulint unsigned long int
//#define  usint unsigned short int

typedef unsigned long       ulint;
typedef long                lint;
typedef unsigned short      usint;
typedef short               sint;
//typedef short               boolean;
typedef unsigned char       byte;


////////////////////////////////////////////////////////////////////////////

typedef enum
{
	PP_IF_INTERNAL,			//内置密码键盘
	PP_IF_COM1,				//COM1接口键盘
	PP_IF_COM2,				//COM2接口键盘(对于C520来说是/dev/ttys3)
	PP_IF_USB,				//USB接口键盘
	MAX_PP_IF_TYPE
} PpInterfaceType;

typedef enum
{
	CTLS_IF_INTERNAL,		//内置非接
	CTLS_IF_COM1,			//COM1接口外置非接
	CTLS_IF_COM2,			//COM2接口外置非接(对于C520来说是/dev/ttys3)
	CTLS_IF_USB,			//USB接口外置非接
	MAX_CTLS_IF_TYPE
} CtlsInterfaceType;

// 终端固定参数
typedef struct __FixParams
{
	unsigned short ContactlessTimeout;	// 非接触超时时间 
	unsigned short MaxTranNumber;		// 最大交易流水数量 
	unsigned short OffUploadNumLimit;	// 累计脱机交易笔数上限 
	unsigned short KeyIndex;			// 主密钥索引号 
	
    byte RefundAmt[12 + 1];				// 退货限制金额
    byte EC_Limit[12 + 1];				// 电子现金额度
    byte OffLine_CTLS_Limit[12 + 1];	// 脱机非接触限额
    byte CTLS_Limit[12 + 1];			// 非接触限额
    byte CVM_Limit[12 + 1];				// 终端执行CVM限额CVM_Limit
    byte DefaultTransType;				// 待机界面默认刷卡交易类型	//Modify on 2015-07-17 for 银联改为1表示消费 2表示预授权
    byte FeePercent;					// 小费百分比
    byte Receipt;						// 单据联数
    byte ReversalRetryTimes;			// 冲正重发次数
    byte OfflineUploadMode;  			// 离线上送方式 0-联机前  1-结算前
    byte UploadMaxTimes;        		// 离线交易上送次数
    byte UploadCurTimes;				// 批结后目前已上送次数
    byte UploadCurNote;					// 批结后当前上送批次类型
	
    byte ManageTelNo[20];				// 管理电话
    byte TPDU[5 + 1];					// NAC-TPDU
    byte Header[6 + 1];					// CUP HEADER
    byte TerminalID[8 + 1];				// 终端号
    byte MerchantID[15 + 1];			// 商户号
    byte MerchantNameChn[40 + 1];		// 商户名称-CHINESE
    byte MerchantNameEng[40 + 1];		// 商户名称-ENGLISH
    byte LocalBank[4 + 1];				// 银行代码
    byte LocalArea[4 + 1];				// 本地区代码
    byte ReceiptTitle[64 + 1];  		// 签购单抬头信息
	
	//EMV 终端参数,配置内核用
    byte TerminalType;					// 终端类型
    byte TerminalCap[3 + 1];			// 终端能力--CCB不支持ONLINE PIN
    byte AddTerminalCap[5 + 1];			// 终端符加能力
    byte Appl_Type;						// 默认主应用
    byte KernelOptions[8 + 1];			// 内核选项
	
    byte PP_DesType;					// 密码键盘加密类型PP222_DES_MODE /PP222_3DES_MODE
    byte PP_Interface;					// 密码键盘接口类型 参看 PpInterfaceType 定义
    byte CtlsInterface;					// 非接读卡器接口类型 参看 CtlsInterfaceType 定义
	
	char AdverInfo[128 + 1]; 			// 广告信息 
	char SSLCertFileName[32 + 1];		// SSL证书文件名称
	char HotLinePhoneNum[20 + 1];			// 服务热线电话号码
	
    byte ReceiptPrintFont;				// 签购单打印字体 0:小字体 1:中字体 2:大字体
	char UnknownIssuerName[32+1];		// 未知发卡行名称
	char MerchantInfo[128+1];			// 商户个性化信息
    byte ReceiptPrintDark;				// 设置打印灰度0-2
    byte Reserved[101];
} FixParams;

// 动态参数
typedef struct
{
    // 7 x 4 = 28 bytes
	ulint   SysTrace;           // 交易流水号
	ulint   InvoiceNum;         // 交易票据号
	ulint   BatchNum;           // 交易批次号
	ulint   UploadTotalCnt;		// 批上送总笔数
	ulint	OfflineRecNum;		// 离线交易未上市笔数
	ulint 	DialTimes;			// 5位数字表示的总拨号次数
	ulint 	DialSuccTimes;		// 3位数字表示的拨通率(百分比)
	
    // 4 + 99 + 34 + 11 = 148 bytes
    byte    BankStatus;			// 终端状态
    byte	NextEvent;			// 后台Header返回要求终端处理事件
    byte	ReversalCount;		// 冲正冲了几次
    byte	EMVPrmStatus;		// EMV参数开关
    byte    PINKey[32 + 1];  	// 终端PIN工作密钥
    byte    MACKey[32 + 1];  	// 终端MAC工作密钥
    byte    TRACKKey[32 + 1];  	// 终端TRK工作密钥
    byte    OperID[16 + 1];     // 操作员代码
    byte    OperPwd[16 + 1];    // 操作员密码
    byte	StartUsingTime[11];	// 4位数字从上次成功统计到本次统计的时间段(小时)
	
    byte	Reserved[80];
} DynParams;

typedef struct __CommParams
{
    // 13 x 2 bytes
	unsigned short  GprsTimeout;      // Gprs超时时间 
	unsigned short  CdmaTimeout;      // Cdma超时时间 
	unsigned short  EthTimeout;       // Eth超时时间 
	unsigned short  WifiTimeout;      // WIFI超时时间
	unsigned short  TelTimeout;       // Tel超时时间 
	
	unsigned short  GprsHostPort1;    // GPRS主机端口 
	unsigned short  GprsHostPort2;    // GPRS备份主机端口 
	unsigned short  CdmaHostPort1;    // CDMA主机端口 
	unsigned short  CdmaHostPort2;    // CDMA备份主机端口 
	unsigned short  EthHostPort1;     // 以太网主机端口 
	unsigned short  EthHostPort2;     // 以太网备份主机端口 
	unsigned short  WifiHostPort1;    // WIFI主机端口 
	unsigned short  WifiHostPort2;    // WIFI备份主机端口 
	unsigned short  SSLHostPort1;     // SSL HostPort1
	unsigned short  SSLHostPort2;     // SSL HostPort2
	
	// COMM_TYPE_SDLC/COMM_TYPE_ETHERNET/COMM_TYPE_ASYNCMDM/COMM_TYPE_GPRS
    byte  CurrentCommMode;        	// 当前通讯模式
    byte  Retrys;                 	// 重试次数

	//以下参数为本地POS(共用部分)
    byte  LocalIP[16];              // 本机地址
    byte  SubNetMask[16];           // 网络MASK
    byte  GateWay[16];              // 网关
    byte  PriDNS[16];               // DNS1
    byte  SecDNS[16];               // DNS2
	
	// GPRS
    byte  GprsPhoneNo[32];        	// GPRS拨入号码
    byte  GprsAPN[64];            	// GPRS接入点
    byte  GprsUserName[32];       	// GPRS用户名
    byte  GprsUserPwd[32];        	// GPRS密码
    byte  GprsHostIP1[16];        	// GPRS主机地址1
    byte  GprsHostIP2[16];        	// GPRS主机地址2

	// CDMA
    byte  CdmaPhoneNo[32];        	// CDMA拨入号码
    byte  CdmaUserName[32];       	// CDMA用户名
    byte  CdmaUserPwd[32];        	// CDMA密码
    byte  CdmaAuthType;           	// CDMA认证类型
    byte  CdmaHostIP1[16];        	// CDMA主机地址1
    byte  CdmaHostIP2[16];        	// CDMA主机地址2

	// DIALUP
    byte  TelPrefix[10];            // 外线号
    byte  TelPhone[3][20];          // 电话号码
    byte  ModemRegS91;				// Modem寄存器S91取值
	
	//WIFI
    byte  NetworkType;      		// 网络类型 WIFI_ACCESS_POINT/WIFI_PEER_TO_PEER
	char  ApSSID[ 17 ];     		// APN : WIFI_ACCESS_POINT 时使用  否则为0xFF 
    byte  ChannelID;        		// Channel ID: WIFI_PEER_TO_PEER 时使用 否则为0xFF
    byte  SecuType;         		// WIFI_NO_SECURITY/WIFI_64_BIT_SECURITY/WIFI_128_BIT_SECURITY
    byte  AuthType;         		// WIFI认证类型
    byte  WepKey[4][32];        	// 网络密钥
    byte  DefaultKeyIdx;        	// 默认使用的网络密钥索引
    byte  WifiHostIP1[16];      	// WIFI主机地址1
    byte  WifiHostIP2[16];      	// WIFI主机地址2
	
	// ETHERNET
    byte  EthHostIP1[16];           // 以太网主机地址1
    byte  EthHostIP2[16];           // 以太网主机地址2
	
	// SSL相关
	char  SSLSvrDomainName1[64+1];		// SSL服务器域名1
	char  SSLSvrDomainName2[64+1];		// SSL服务器域名2
	char  SSLDnsIP1[16];
	char  SSLDnsIP2[16];
	
    byte  Reserved[128];
} CommParams;


// 开关参数
typedef struct
{
	// 交易开关 22 bits + 5 bits + 6 bits + 9 bits + 9 bits + 4 bits
    byte Sale_Support       	: 1;        // 消费
    byte Balance_Support     	: 1;        // 查询余额
    byte Void_Support       	: 1;        // 撤销
    byte Refund_Support			: 1;		// 退货
    byte PreAuth_Support		: 1;		// 授权类
    byte CompeleteReq_Support	: 1;		// 预授权完成请求
    byte CompeleteNote_Support	: 1;		// 预授权完成通知
    byte PreAuthVoid_Support	: 1;		// 预授权撤销
    byte CompeleteVoid_Support	: 1;		// 预授权完成撤销
    byte OfflineSettle_Support	: 1;		// 离线结算
    byte SettleAdjust_Support	: 1;		// 离线调整
    byte Offline_Support		: 1;		// 离线交易
    byte ECash_Support			: 1;		// 是否支持电子现金
    byte CTLS_Support			: 1;		// 是否支持CTLS
    byte Wallet_Support  		: 1;        // 钱包
    byte IntallSale_Support  	: 1;        // 分期
    byte IntallVoid_Support		: 1;		// 分期撤销
    byte YuyueSale_Support  	: 1;        // 预约消费
    byte YuyueVoid_Support		: 1;		// 预约消费撤销
    byte MagCashLoad_Support	: 1;        // 磁条卡现金充值
    byte MagAccountLoad_Support	: 1;		// 磁条卡账户充值
    byte Script_Support			: 1;		// 是否支持脚本上送
	
    byte EcashLoad_Support		: 1;		// 现金圈存
    byte EcashInLoad_Support	: 1;		// 指定账户圈存
    byte EcashOutLoad_Support	: 1;		// 非指定账户圈存
    byte EcashLoadVoid_Support	: 1;		// 现金充值撤销
    byte EcRefund_Support		: 1;		// 电子现金脱机退货
	
    byte IssuerScoreSale_Support: 1;        // 发卡行积分消费
    byte IssuerScoreVoid_Support: 1;		// 发卡行积分消费撤销
    byte UnionScoreSale_Support : 1;		// 联盟积分消费
    byte UnionScoreVoid_Support : 1;		// 联盟积分撤销
    byte UnionScoreBalance_Support : 1;		// 联盟积分查询
    byte UnionScoreRefund_Support: 1;		// 联盟积分退货
	
    byte MChipBalance_Support	: 1;
    byte MChipSale_Support		: 1;
    byte MChipVoid_Support		: 1;
    byte MChipRefund_Support	: 1;
    byte MChipPreAuth_Support	: 1;
    byte MChipPreAuthVoid_Support: 1;
    byte MChipCompleteReq_Support: 1;
    byte MChipCompleteNote_Support: 1;
    byte MChipCompleteVoid_Support: 1;
	
    byte OrderSale_Support		: 1;
    byte OrderVoid_Support		: 1;
    byte OrderRefund_Support	: 1;
    byte OrderPreAuth_Support	: 1;
    byte OrderPreAuthVoid_Support: 1;
    byte OrderCompleteReq_Support: 1;
    byte OrderCompleteNote_Support: 1;
    byte OrderCompleteVoid_Support: 1;
    byte OrderVerifyCardHolder	: 1;
	
	// 电子钱包开关(应用中并不支持这些交易，但可以设置相应交易开关)
    byte EPSale					: 1;		// 电子钱包消费
    byte EPCashLoad				: 1;		// 电子钱包现金圈存
    byte EPInLoad				: 1;		// 电子钱包指定账户圈存
    byte EPOutLoad				: 1;		// 电子钱包非正定账户圈存
	
	// 参数开关 29 bits
    byte IsPreDialFlag  		: 1;        // 是否支持预拨号
    byte CheckDialTone  		: 1;        // 是否检测拨号音
    byte ManualFlag            	: 1;        // 是否手输
    byte MaskPanFlag           	: 1;        // 屏蔽卡号
    byte VoidSwipCard          	: 1;        // 撤销是否刷卡
    byte VoidPIN               	: 1;        // 撤销是否输入密码
    byte VoidPreAuthPIN        	: 1;        // 预授权撤销是否输密
    byte PreAuthEndReqPIN      	: 1;        // 预授权完成请求是否输密
    byte VoidPreauthEndSwipCard	: 1;        // 预授权完成撤销是否刷卡
    byte VoidPreAuthEndPIN     	: 1;        // 预授权完成撤销是否输密
    byte AllTransDetailFlag		: 1;		// 打印所有交易明细(包括失败交易)
    byte DetailPrintFlag       	: 1;        // 打印明细
    byte FailDetailPrint       	: 1;        // 打印失败交易明细
    byte ReceiptCardHolderName	: 1;		// 签购单打印持卡人姓名
    byte TrackEncryptFlag      	: 1;        // 磁道加密开关
    byte PartPurchaseFlag      	: 1;        // 部分承兑开关
    byte EnglishSlip           	: 1;        // 签购单是否打英文
    byte Default_Tran          	: 1;        // 默认交易类型: 1-消费; 0-预授权
    byte FeeFlag               	: 1;        // 是否支持小费
    byte AutoLogoutFlag        	: 1;        // 是否自动签退
    byte IssueChnName           : 1;        // 发卡行中文名称
    byte AccquChnName           : 1;        // 收单行中文名称
    byte MngPwdRequired         : 1;        // 是否需要输主管密码开关
    byte SmallAmountStandinAuth	: 1;		// 小额代授权开关
    byte NegativeSignFlag		: 1;		// 负号打印开关
    byte GetPanSupportCTLS		: 1;		// 消费刷卡界面是否支持挥卡
    byte PreAuthCardNoMask		: 1;		// 预授权交易是否屏蔽卡号
    byte UserDefinedReceiptTitle: 1;		// 是否自定义抬头
    byte CheckBlackListCard		: 1;		// 是否支持黑名单卡
	
	// For SSL 通讯 5 bits
    byte SSLCommFlag			: 1;		// SSL 通讯方式支持
    byte OnewayAuthSSL			: 1;		// 是否单向认证
    byte UseDNSFlag				: 1;		// 使用DNS标志
    byte UseDHCPFlag			: 1;		// 使用DHCP动态获取地址标志
    byte UseLBSFlag				: 1;		// 使用LBS基站定位服务标志
	
    byte PacketPrintDebugFlag	: 1;		// 报文打印调试开关
	
    byte CtlsTransChannelFlag  	: 1;		// 非接交易通道标志: 0-非接交易走qPBOC联机借贷记账户 1-优先走qPBOC电子现金账户，允许转联机

	// Reserved for future using
	unsigned long UnUsed1		: 5;        // 保留
	unsigned long Reserved1		: 32;       // 保留
} TranSwitchParams;


// 终端参数配置结构 2048 bytes
// 增删参数注意调整预留字段以便保证各个结构的大小保持不变！
typedef struct
{
	FixParams   mFixPrm;        // 存储固定参数
	DynParams   mDynPrm;        // 存储动态参数
	TranSwitchParams mTrSwitch; // 存储交易开关
	CommParams  mCommPrm;       // 存储通讯参数
} Term_Config;


//-------------------------------------------------------------------------

typedef struct __TransRecord
{
	ulint SysTrace;       		// 交易流水
	ulint OrgSysTrace;    		// 原交易流水
	ulint BatchNum;       		// 批次号
	ulint OrgBatchNum;    		// 原批次号
	ulint InvoiceNum;     		// 凭证号
	usint Swipe;     			// 刷卡模式
	
    byte Status[4];         	// 交易状态定义
	
    byte Amount[12 + 1];     	// 交易金额
    byte Fee[12 + 1];        	// 小费
    byte OrgAmount[12 + 1];  	// 原始金额
    byte TrType;             	// 交易类型
    byte OrgTranType;        	// 原交易类型
    byte ProcessCode[6+1];		// 交易处理码
    byte PAN[20];            	// 账号
    byte PANLen;             	// 账号长度
    byte PAN_2[20];          	// 账号2(转入卡号)
    byte PAN_2Len;           	// 账号2长度
    byte TranDate[4 + 1];    	// 交易日期YYYYMMDD(BCD)
    byte OrgTranDate[4 + 1]; 	// 原始交易日期MMDD
    byte TranTime[3 + 1];    	// 交易时间HHMMSS(BCD)
    byte ExpDate[3 + 1];     	// 卡有效期YYYYMM(BCD)
    byte SettleDate[2+1];       // 清算日期MMDD(BCD)
    byte RefCode[12 + 1];   	// 参考号
    byte OrgRefCode[12 + 1]; 	// 原始参考号
    byte AuthCode[6 + 1];      	// 授权号
    byte OrgAuthCode[6 + 1];   	// 原始授权号
    byte RspCode[2 + 1];     	// 响应码
    byte POSEntry[2 + 1];    	// ISO #Fld22
    byte Issuer[11 + 1];     	// 发卡行
    byte Acquirer[11 + 1];   	// 收单行
    byte IssuerCode[11 + 1]; 	// 受理方标识码
    byte AuthType;           	// 授权类型,电话授权或POS授权
    byte AuthInst[11 + 1];   	// 原授权机构
    byte CardTypeName[3 + 1]; 	// 卡类别名称
    byte HostNote1[20 + 1];  	// 主机附加信息1  63.2.1域  发卡方保留域
    byte HostNote2[20 + 1];  	// 主机附加信息2  63.2.2域  中国银联保留域
    byte HostNote3[20 + 1];  	// 主机附加信息3  63.2.3域  受理机构保留域
    byte HostNote4[60 + 1];  	// 主机附加信息4  63.2.4域  POS终端保留域
    byte OrgTermID[8 + 1];    	// 原终端号(电子现金退货交易用)
    byte BeforeStlUploadCount;	// 结算前离线交易上送次数计数器
    byte AfterStlUploadCount;	// 结算后的交易上送次数计数器
    byte ServiceCode[2+1];		// 服务点条件码 25域
    byte TransTypeCode[2+1];	// 交易类型码 60.1域
    byte EncryptedTrkFlag;  	// 1: 磁道信息已加密  0:磁道信息未加密
    byte Track2Data[40];		// 二磁信息（ASCII）
    byte Track3Data[108];		// 三磁信息（ASCII）

	//分期
    byte InstallmentPeriod;             //期数
    byte InstallmentProjectNo[30 + 1];  //项目编码
    byte InstallmentFeePayway;          //手续费支付方式

    byte InstallmentFirstAmt[12+1];   //首期还款金额
    byte InstallmentCurrency[3+1];    //还款币种
    byte InstallmentHolderFee[12+1];  //持卡人手续费
    byte InstallmentFirstFee[12+1];   //首期手续费
    byte InstallmentPerFee[12+1];     //每期手续费
  
    byte BonusScore[12+1];
    byte AddPrnInfo[13+1];

	//积分
    byte ScoreProductCode[30+1]; //积分商品代码
    byte ConvertedScore[12+1];   //兑换积分
    byte ScoreBalance[12+1];     //积分余额
    byte ScoreSelfPaidAmt[12+1]; //自付金额

	//预约号
    byte YuYuePhoneNum[11+1];  /* 预约手机号(11位) */
    byte YuYueCode[6+1];       /* 预约号(6位) */

	//EMV IC卡数据
    byte CardHolderName[30 + 1];   	// 持卡人信息
    byte AppLabel[16 + 1]; 		// 应用标签
    byte AppPrefName[16 + 1];   // 应用首选名称
    byte AID[32 + 1];  			// 专用文件名称 84
    byte AIDLength;				// 84 length
    byte SequenceNum;   		// 应用主帐号序列号
    byte SeqNumFlag;   			// 卡序号存在标志0: 不存在, 1: 存在
    byte EmvTransType;			// 交易类型 9C
    byte ARQC[8 + 1];  			  // 应用密文
    byte TC[8 + 1];  			// 应用密文
    byte TVR[5 + 1];  			// 终端验证结果 95
    byte TSI[2 + 1];  			// 9B
    byte ATC[2 + 1];            // 应用交易计数器 9F36
    byte RANDOM[4 + 1]; 		// 不可预知数 9F37
    byte IAD[32 + 1];  			// 发卡行应用数据 9F10
    byte IADLength;				// 9F10 length
    byte AIP[2 + 1];  			// 应用交互特征 82
    byte CryptInfo;				// 应用信息数据 9F27
    byte CurrCode[2 + 1];		// 交易货币代码 5F2A
    byte CountryCode[2 + 1];	// 交易国家代码 9F1A
    byte TermCAP[3 + 1];		// 终端性能 9F33
    byte AddTermCAP[5+1];		// 终端附加性能 9F40
    byte CVMResult[3 + 1];		// 持卡人验证结果 9F34
    byte TIDSerial[8 + 1];		// 接口设备序列号 9F1E
    byte AppVer[2 + 1];			// 应用版本号 9F09
    byte CPI[32+1];         	// 9F63
    byte CPILength;         	// 9F63 Length
    byte IssAuthData[16+1];		// 发卡行认证数据 91
    byte IssAuthDataLength;		// 91 length
    byte IssSript71Data[128+1];	// 发卡行71脚本数据 71
    byte IssSript71DataLength;	// 71 length
    byte IssSript72Data[128+1];	// 发卡行72脚本数据 72
    byte IssSript72DataLength;	// 72 length
    byte IssSriptResultData[48+1];	// 发卡行脚本结果 DF31
    byte IssSriptResultDataLength;
    byte PKindex;				// 密钥索引 8F
    byte EMVRspCode[2 + 1];		// EMV响应码
    byte EC_flag;    			// 电子现金交易标志
    byte EC_IssAuthID[ 6 + 1];	// 电子现金发卡行认证数据
    byte EC_Balance[12 + 1]; 	// 电子现金余额
    byte EC_Reset_Threshold[12 + 1];	// 电子现金重置阈值
    byte ARPCFlag;    			// 0 - ARPC校验成功 1- ARPC校验失败
    byte qPBOC_OnlineFlag;    	//qPBOC联机标志(0:不需要联机  1:需要联机)

    byte EC_Balance2[12 + 1]; 	//第二币种电子现金余额
    byte CTLSCurrencyName1[3+1];//第一币种货币名称
    byte CTLSCurrencyName2[3+1];//第二币种货币名称
    byte desc[32];              //备用
} TransRecord;

// 交易数据－这是每次交易时候采集的数据，该结构中包含前面的流水结构
typedef struct __TransData
{
	//交易流水结构，交易成功后这部分将被保存
	TransRecord  TransDB;
	
	//不允许保存的数据，交易完成后，局部变量被释放，数据被丢弃
    byte MsgID[ 4+1 ];
    byte Track1Data[80];		// 一磁信息（ASCII）
    byte PinExists;				// 是否有联机PIN
    byte PinRetryFlag;  		// 是否重输密码
    byte PINdata[8];			// PIN block
    byte AddRspCode[2 + 1];     // 附加响应码用于交易成功后非00的信息提示
	
	int AddBufLen;
    byte AddBuf[ 256 ];
	
	// IC卡临时数据
    byte EC_PaybyMenuFlag;		// 是否为小额支付交易
    byte EC_9F7AFlag;			// 获取9F7A的值

    byte EC_CTLSPriOpt;			//电子现金默认优先标志 0: qPBOC强制走联机 1: qPBOC优先走脱机 2: 非接PBOC 其它: 无定义
	
} DynData;


// 交易类型--注意同Msg相一致
typedef enum __TransactionType
{
	BTYPE_LOGON=1,                    // POS 签到
	BTYPE_LOGOUT,                     // POS 签退
	BTYPE_LOGON_CASHIER,              // 收银员积分签到
	BTYPE_SETTLE,                     // 结算 
	BTYPE_SETTLE_END,                 // 结算完成 
	BTYPE_BATCH_UPLOAD,               // 批上送
	BTYPE_SCRIPTADVICE,               // 发卡行处理脚本结果通知
	BTYPE_ICOFFLINE_UPLOAD,           // IC卡脱机交易上送 
	BTYPE_TC_UPLOAD,                  // IC卡联机交易
	BTYPE_TC_UPLOAD_END,              // IC卡联机交易上送完成
	BTYPE_DOWNLOAD_PARAMETERS,        // 参数下载(传递)
	BTYPE_UPLOAD_STATUS,              // 状态上送(亦可作参数查询) 
	BTYPE_ECHO_TESTING,				  // 回响测试
	BTYPE_DOWNLOAD_END,               // 参数下载结束 

	BTYPE_BALANCE,                    // 余额查询 
	BTYPE_SALE,                       // 消费 
	BTYPE_SALE_VOID,                  // 消费撤销 
	BTYPE_REFUND,                     // 退货 

	BTYPE_PREAUTH,                    // 预授权 
	BTYPE_PREAUTH_VOID,               // 预授权撤销 
	BTYPE_COMPLETE,                   // 预授权完成请求(联机) 
	BTYPE_COMPLETE_VOID,              // 预授权完成撤销 
	BTYPE_OFFLINE_COMPLETE,           // 预授权完成通知(联机) 

	BTYPE_OFFLINE_SETTLE,             // 离线结算(脱机) 
	BTYPE_OFFLINE_ADJUST,             // 结算调整(脱机)
	
	BTYPE_ECASH_BALANCE,              // 电子现金查余额
	BTYPE_ECASH_CASH_RELOAD,          // 电子现金现金充值 
	BTYPE_ECASH_CASH_RELOAD_VOID,     // 电子现金现金充值撤销 
	BTYPE_ECASH_IN_RELOAD,            // 电子现金指定账户圈存 
	BTYPE_ECASH_OUT_RELOAD,           // 电子现金非指定账户圈存 
	BTYPE_ECASH_REFUND,               // 电子现金脱机交易退货
	
	BTYPE_MAG_ACCT_VERIFY,            // 磁条卡现金充值账户验证 
	BTYPE_MAG_CASHLOAD,               // 磁条卡现金充值 
	BTYPE_MAG_CASHLOAD_CONFIRM,       // 磁条卡现金充值确认 
	BTYPE_MAG_ACCTLOAD,               // 磁条卡账户充值 
	
	BTYPE_INSTALLMENT,                // 分期付款 
	BTYPE_INSTALLMENT_VOID,           // 分期付款撤销 
	
	BTYPE_YUYUE_SALE,                 // 预约消费 
	BTYPE_YUYUE_SALE_VOID,            // 预约消费撤销 
	BTYPE_YUYUE_REFUND,               // 预约退货

	BTYPE_ISSUER_SCORE_SALE,          // 发卡行积分消费 
	BTYPE_ISSUER_SCORE_SALE_VOID,     // 发卡行积分撤销 

	BTYPE_UNION_SCORE_SALE,           // 联盟积分消费 
	BTYPE_UNION_SCORE_SALE_VOID,      // 联盟积分撤销 
	BTYPE_UNION_SCORE_BALANCE,        // 联盟积分查询 
	BTYPE_UNION_SCORE_REFUND,         // 联盟积分退货 

	BTYPE_MCHIP_BALANCE,              // 手机芯片余额查询 
	BTYPE_MCHIP_SALE,                 // 手机芯片消费 
	BTYPE_MCHIP_SALE_VOID,            // 手机芯片消费撤销 
	BTYPE_MCHIP_REFUND,               // 手机芯片退货 
	BTYPE_MCHIP_PREAUTH,              // 手机芯片预授权 
	BTYPE_MCHIP_PREAUTH_VOID,         // 手机芯片预授权撤销 
	BTYPE_MCHIP_COMPLETE,             // 手机芯片预授权完成请求(联机) 
	BTYPE_MCHIP_COMPLETE_VOID,        // 手机芯片预授权完成撤销 
	BTYPE_MCHIP_OFFLINE_COMPLETE,     // 手机芯片预授权完成通知(联机) 

	BTYPE_ORDER_CARDHOLDER_VERIFY,    // 订购交易持卡人身份验证
	BTYPE_ORDER_SALE,                 // 订购消费 
	BTYPE_ORDER_SALE_VOID,            // 订购消费撤销 
	BTYPE_ORDER_REFUND,               // 订购退货 
	BTYPE_ORDER_PREAUTH,              // 订购预授权 
	BTYPE_ORDER_PREAUTH_VOID,         // 订购预授权撤销 
	BTYPE_ORDER_COMPLETE,             // 订购预授权完成请求(联机) 
	BTYPE_ORDER_COMPLETE_VOID,        // 订购预授权完成撤销 
	BTYPE_ORDER_OFFLINE_COMPLETE,     // 订购预授权完成通知(联机) 

	BTYPE_AID_INQUERY,                // 查询AID INDEX LIST
	BTYPE_AID_UPDATE,                 // 更新AID参数
	BTYPE_AID_END,                    // 更新AID完成
	BTYPE_CAKEY_INQUERY,              // 查询CAPK INDEX LIST
	BTYPE_CAKEY_UPDATE,               // 更新公钥
	BTYPE_CAKEY_END,                  // 更新公钥完成
	BTYPE_BLACK_UPDATE,               // 更新黑名单参数
	BTYPE_BLACK_END,                  // 更新黑名单完成
	BTYPE_TMS_UPDATE_PARAMETER,       // 更新TMS
	
	BTYPE_REVERSAL,                   // 冲正
	BTYPE_MAX_TRANTYPE,
} TransactionType;


//交易失败的银行返回错误信息
typedef enum
{
	BANK_ABORT = -200,         // 用户取消
	BANK_TIMEOUT,     	// 超时
	BANK_FAIL,        	// 失败
	BANK_DB_FOUND,    	// 找到
	BANK_DB_NOTFOUND, 	// 未找到
	BANK_DB_FAIL,     	// 数据库失败
	BANK_DB_EMPTY,    	// 数据库为空
	BANK_DB_NOFILE,   	// 没有文件
	BANK_NOCONNECT,   	// 未连接
	BANK_PACKERROR,   	// 打包错误
	BANK_SENDERROR,   	// 发送错误
	BANK_COMMERROR,   	// 通讯错误
	BANK_MACERROR,    	// MAC错误
	BANK_UNPACKERROR,   // 解包错误
	BANK_HOSTERROR,   	// 主机错误
	BANK_CONTINUEDIAL,  // 重拨
	BANK_COMM_FAIL,   	// 通讯错误
	BANK_ET_FAIL,       // 充值失败
	BANK_RECVERROR,   	// 接收数据失败
	BANK_REVERSALERROR,	// 冲正失败
	BANK_INVALIDE,    	// 数据校验错误
	BANK_PRE,			// 上一条
	BANK_NEXT,			// 下一条
	BANK_NOAPP,         // 无应用 by XC
	BANK_CONTINUE,      // 继续   by XC
	BANK_USERCANCEL,	// 用户取消
	
	BANK_EMV_ONLINE = -100,	// EMV IC卡联机交易
	BANK_EMV_BYPASS,	// EMV BYPASS
	BANK_EMV_APPROVE,	// EMV IC卡批准交易
	BANK_EMV_DECLINE,	// EMV IC卡拒绝交易
	BANK_EMV_FALLBACK,	// EMV 其他交易转磁条卡或手输卡号
	BANK_EMV_ICC,		// EMV 其他交易转IC卡
	
  	BANK_OK = 0,    	  // OK
	MAX_ERR_RET_CODE,
} ERR_RET_CODE;


// 操作员类型
typedef enum
{
	OPER_OPERATOR = 0,    //普通操作员
	OPER_MANAGER,         //主管操作员
	OPER_SUPERVISOR,      //系统管理员
	OPER_MAXTYPE
} Bank_OperType;

// 操作员结构
typedef struct
{
	char  OperID[16 + 1]; 	// 操作员ID(字符串最大16位)
    byte  IDLen;          	// 操作员ID长度
	char  OperPwd[16 + 1]; 	// 操作员密码(字符串最大16位)
    byte  PwdLen;         	// 操作员密码长度
    byte  OperType;       	// 操作员类型
} Bank_Operator;


//手机芯片卡信息结构
typedef struct
{
  unsigned char CardName[20];   /*卡名称*/
  unsigned char CardNum[10];    /*卡号*/
  unsigned char Card2Buf[20];   /*2磁道BCD*/
  unsigned char Card3Buf[54];   /*3磁道BCD*/
  unsigned char CardExpDate[2]; /*有效期*/
  unsigned char RFU[2];         /*保留*/
  unsigned char ProcessKey[8];  /*过程密钥*/
} CUPMobile_CardInfo;



typedef struct __TotalRec
{
	unsigned long BatchNo;	//批次号
    byte CnName[40 + 1];	// 商户名称-CHINESE
    byte EnName[40 + 1];	// 商户名称-ENGLISH
    byte MechID[15+1];		//商户号
    byte TermID[8+1];		//终端号
    byte OperID[16+1];		//操作员号
    byte DateTime[7];		//YYYYMMDDHHMMSS
	
	// total for settlement
	// 内卡借贷记
	int  CupTotalSaleCnt;
    byte CupTotalSaleAmt[ 12+1 ];
	int  CupTotalVoidCnt;
    byte CupTotalVoidAmt[ 12+1 ];
	
	
	// 外卡借贷记
	int  FgnTotalSaleCnt;
    byte FgnTotalSaleAmt[ 12+1 ];
	int  FgnTotalVoidCnt;
    byte FgnTotalVoidAmt[ 12+1 ];
	
	
    byte CupSettleFlag;		//0=内卡对账平  1=内卡对账不平
    byte FgnSettleFlag;		//0=外卡对账平  1=外卡对账不平
	
	// total for print
	int  CupPrtECSaleCnt;  //电子现金交易笔数
    byte CupPrtECSaleAmt[ 12+1 ]; //电子现金交易金额
	int  CupPrtSaleCnt;
    byte CupPrtSaleAmt[ 12+1 ];
	
	int  CupPrtCompCnt;
    byte CupPrtCompAmt[ 12+1 ];
	int  CupPrtRefundCnt;
    byte CupPrtRefundAmt[ 12+1 ];
	int  CupPrtOfflineCnt;
    byte CupPrtOfflineAmt[ 12+1 ];
	int  CupPrtCompOfflineCnt;
    byte CupPrtCompOfflineAmt[ 12+1 ];
	int  CupPrtLoadCnt;
    byte CupPrtLoadAmt[ 12+1 ];
	
	int  CupAllSaleCnt;
    byte CupAllSaleAmt[ 12+1 ];
	int  CupAllVoidCnt;
    byte CupAllVoidAmt[ 12+1 ];
	int  CupAllCompCnt;
    byte CupAllCompAmt[ 12+1 ];
	int  CupAllCompVoidCnt;
    byte CupAllCompVoidAmt[ 12+1 ];
	
	// 外卡
	int  FgnPrtSaleCnt;
    byte FgnPrtSaleAmt[ 12+1 ];
	int  FgnPrtCompCnt;
    byte FgnPrtCompAmt[ 12+1 ];
	int  FgnPrtRefundCnt;
    byte FgnPrtRefundAmt[ 12+1 ];
	int  FgnPrtOfflineCnt;
    byte FgnPrtOfflineAmt[ 12+1 ];
	int  FgnPrtCompOfflineCnt;
    byte FgnPrtCompOfflineAmt[ 12+1 ];
	int  FgnPrtLoadCnt;
    byte FgnPrtLoadAmt[ 12+1 ];
	
	int  FgnAllSaleCnt;
    byte FgnAllSaleAmt[ 12+1 ];
	int  FgnAllVoidCnt;
    byte FgnAllVoidAmt[ 12+1 ];
	int  FgnAllCompCnt;
    byte FgnAllCompAmt[ 12+1 ];
	int  FgnAllCompVoidCnt;
    byte FgnAllCompVoidAmt[ 12+1 ];
	
	int PreAuthCnt;	
} TotalRec;

typedef struct
{
  char *eng;
  char *chn;
}Prn_Type;

#endif
