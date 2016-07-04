#ifndef __MULTDEF_H
  #define __MULTDEF_H

#define MTMSAPP               "MAINAPP"
#define MTMSAPP_TMSPARA_FILE  "MAINAPPPR"

#define TMS_FILELIST          "/TMSFIL_DAT_FILE"      /*文件列表文件*/
#define TMS_FILELISTBAK       "/TMSFLB_DAT_FILE"      /*文件列表备份文件*/
#define TMS_CTRLFILE          "/TMSCTL_DAT_FILE"      /*FPD控制文件*/
#define TMS_APPONPORT         "/TMSAOP_DAT_FILE"      /*子应用联机报到策略*/

#define TMS_MUTUAL_PARA_FILE  "/MUTUAL_PRM_FILE"      /* 共性参数文件 */


#define MAX_APPLIST		    30
#define MAX_RECSIZE_LIST    300       /*列表文件记录长度*/
#define MAX_RECSIZE_FILE    1024      /*文件内容的记录长度*/
#define MAX_PACKFINISH      70
#define MAX_APPONPORT_FILE  100

#define	MAX_EVENTDATA_SIZE	256		    // 事件最大消息长度
#define MAX_APP_DATA_SIZE   512       // 消息传递的最大应用数据长度


typedef enum{
  APP_NEED_SETTLMENT  = 0x01,
  APP_NEED_UPDATE     = 0x02,
  APP_STATUS_RFU1     = 0x04,
  APP_STATUS_RFU2     = 0x08,
  APP_STATUS_RFU3     = 0x10,
  APP_STATUS_RFU4     = 0x20,
  APP_STATUS_RFU5     = 0x40,
  APP_STATUS_RFU6     = 0x80,
}App_Status;


typedef struct _File_List{

	short EventId;	//激活事件ID //未使用－可复用 2012-9-3 22:26:39
	short CallFuncEventId;	//调用事件ID //未使用－可复用 2012-9-3 22:26:39
	char  MenuText[16];
  
  /*Vx-40*/
	char  VxFileName[10+1]; /*应用名称*/
	char  VxFileVersion[20+1]; /*应用版本*/
	char  VxLength[4];         /*长度*/
	char  VxGroupID[2+1];      /*GROUP*/
	char  VxMemType;           /*RAM OR FLASH*/

  /*15*/
	char  VxFileType;                 /*dll set 文件类型 ’A’： 标识应用程序  ‘P’ ：标识参数文件  ‘D’ ：标识数据文件  O： 标识其他文件 'L' 动态库文件*/
	char  VxAPPDealFlag;	//0x0000--不更新，0x0001--更新，0x0002--新增，0x0004--删除
    char  VxDownload[4];
	char  VxUmsSeriesNum[2];		  /*序列号*/
	char  VxUmsUndateType;            /*更新时机*/
	char  VxAdvancedCompressFlag;     /* 更改为高级压缩标志 by XC 2012-9-3 22:31:03 */
	char  VxUmsDownPosFlag;           //TMS下载或其它方式
	char  VxUmsMkeyIndex;             //各个应用密钥索引 //未使用－可复用 2012-9-3 22:24:41
	char  VxUmsAPPEnableDown;         /*UMS*/ /*周期下载应用状态*/
	char  VxUmsSeriesNumDown[2];      /*UMS*/ /*周期下载序列号*/  
  
  /*32*/
  char  VxUpdateDate[14+1];   //UMS 更新日期
  char  VxDownDate[16+1]; //下载时间
  
  /*4*/
  char  VxIsNeedFlag; 
  char  VxAPPLoadokFlag;
  char  VxAPPUpdateokFlag;
  char  VxAPPEnable;
  
  /*1*/
  //unsigned char VxAppStatus; //使用enum: App_Status来保存终端应用状态
  char  SettleFlag;     //0-不需要结算 1-需要结算  
  /*end*/
  
}File_List;

typedef enum {
  INFORM_NOTHING_TO_DO = 0,     //无含义
  INFORM_RETURN_TO_MAINAPP,     //提示是否回到主控
  INFORM_SETTLEMENT,            //通知应用提示结算
  INFORM_UPDATE_PARAMETERS,     //通知更新参数
  INFORM_SHOW_PROMPT,           //通知应用显示Data中的信息，如果按确定键则切换到主控，其他键不处理
  MAX_INFORM_STATUS
}InformType;

typedef struct {  //256 bytes
	unsigned char TrType; //See enum InformType
	char  Data[255];
}INFORM_DATA;


//应用共用参数/标志结构 Edit by XC 2012-12-20 //384 bytes
typedef struct {
	//76 bytes
	char TelPrefix[10];     /* 外线号 */
	char CurrAPN[64];       /* 当前APN */
	char SubAppPrmModifiableFlag;     /* 子应用参数可被修改标志 取值范围(0,1) */
	char MainAppOption;               /* 主控应用选项: 1-本应用为普通主控 2-本应用为银商TMS主控 */
	
	//4 bytes
	unsigned char DialToneCheckFlag;
	unsigned char DefCommMode;        /* 默认通讯方式(用于主控一键同步子应用的通讯方式) */
	unsigned char dummy[2];
	
	//8 bytes
	unsigned int  TotalSettCount;     /* 主应用结算累计计数器 */
	unsigned int  TotalSettTimes;     /* 主应用主动发起联机报道的结算累计次数参数 */
	
	//1 byte
	unsigned char MainAppMKeyIndex;   /* 主控主密钥索引 */
	
	//11 bytes
	char DefaultAppName[10+1];        /* 预留 */
	
	//28 bytes
	unsigned char Reserved[28];       /* 预留 */
	
	//256 bytes
	INFORM_DATA InformData;
}MUTUAL_PRM;


//应用间传送共享结构
typedef struct {
	int   TransferType;
	int   ReturnVal;
	char  CallerName[10+1];
	char  Data[MAX_APP_DATA_SIZE];
}TransferData;


typedef struct
{
  char  AppOnportFlag;     /*0-不起作用 1-子应用触发 2-子应用触发完成*/
  char 	AppReportBeginData[8+1] ;  /*开始报到日期*/  /*如果全为0x30 立即发起联机*/
  char 	AppReportBeginTime[6+1];  /*开始报到时间*/   
  char	AppReportEndData[8+1] ;  /*截止报到日期*/ 
  char	AppReportEndTime[6+1];  /*截止报到时间*/  
  char	AppReportTimes;   /*次数*/ 
  char	AppReportDistance[2+1];	  /*间隔天数*/
  char  AppReportDisSec[4+1];  /*4位的自动报到重拨间隔时间[1-9999]单位秒  不足右补0x20*/
  char  APPFileName[8];           /*dll set 文件名 6*/
  char  APPName[22];              /*UMS*/ /*应用名称*/
}AppOnportInfo; /*POSP通知只有一条记录*/


typedef enum
{
  TMSSETTLEMENT = 1,    /*TMS*/
  TMSINFOSETTLEMENT,    /*TMS*/
  CALLSETTLEMENT,       /*TMS*/
  TMSENABLEAPP,         /* 通知启用*/
  TMSDISABLEAPP,        /* 应用是否有流水、通知禁用 */
  GET_MERTER_ID,
  PUT_DIALFIXNO,		      /*同步拨号前缀*/
  TRANSMIT_ADVER_INFO,    /*传递广告信息 by XC 2011-6-30 15:34:08*/
  CLEAR_ADVER_INFO,        /*清除广告信息 by XC 2011-10-19 */
  INFORM_APP_EXIT,         /* 通知应用退出进程 by XC 2015/3/19 */
  GET_ADD_PRM_DATA         /* 获取附加参数信息 by XC 2015/3/19 TMS终端信息上送用的，除终端号和商户号之外的数据 */
}TmsTransferType;


typedef struct 
{
  char ParaFileVerInfo[32];	     /*参数文件的版本信息*/
  char ParaCount[2];	           /*参数个数*/
}AppParaFormat;/*34*/

typedef struct
{
  unsigned char  TagLen;          /*标签长度*/
  unsigned char  TagValue[255];   /*标签值*/
  unsigned char  ParaLen;         /*参数长度*/
  unsigned char  ParaValue[255];  /*参数值*/
}AppParaMsg;

#define S_FPD_NO          0
#define S_FPD_YES         1

#endif
