/**********************************************************************************************************************
Copyright (C) 2009-2010, 山东新北洋信息技术股份有限公司 
文件名: TKIOSKDLL.h
版本 : V1.14
日期: 2010-12-15
描述:  TKIOSK动态库接口及宏定义
函数列表:	1.	TKIOSK_OpenCom()
			2.	TKIOSK_CloseCom()
			3.	TKIOSK_OpenLptByDrv()
			4.	TKIOSK_CloseDrvLPT()
			5.	TKIOSK_OpenUsb()
			6.	TKIOSK_OpenUsbByID()
			7.	TKIOSK_CloseUsb()
			8.	TKIOSK_OpenDrv()
			9.	TKIOSK_CloseDrv()
			10.	TKIOSK_OpenNibblePar()
			11.	TKIOSK_CloseNibblePar()
			12.	TKIOSK_OpenNet()
			13.	TKIOSK_CloseNet()
			14.	TKIOSK_StartDoc()
			15.	TKIOSK_EndDoc()
			16.	TKIOSK_SetComTimeOuts()
			17.	TKIOSK_SetUsbTimeOuts()
			18.	TKIOSK_SetNibbleParTimeOuts()
			19.	TKIOSK_NibbleParPrintToMemory()
			20.	TKIOSK_NibbleParFlushMemory()
			21.	TKIOSK_WriteData()
			22.	TKIOSK_ReadData()
			23.	TKIOSK_SendFile()
			24.	TKIOSK_BeginSaveToFile()
			25.	TKIOSK_EndSaveToFile()
			26.	TKIOSK_QueryStatus()
			27.	TKIOSK_RTQueryStatus()
			28.	TKIOSK_QueryASB()
			29.	TKIOSK_GetVersionInfo()
			30.	TKIOSK_Reset()
			31.	TKIOSK_SetPaperMode()
			32.	TKIOSK_SetMode()
			33.	TKIOSK_SetLineSpacing()
			34.	TKIOSK_SetRightSpacing()
			35.	TKIOSK_SetOppositePosition()
			36.	TKIOSK_SetTabs()
			37.	TKIOSK_ExecuteTabs()
			38.	TKIOSK_PreDownloadBmpToRAM()
			39.	TKIOSK_PreDownloadBmpToFlash()
			40.	TKIOSK_SetCharSetAndCodePage()
			41.	TKIOSK_FeedLine()
			42.	TKIOSK_FeedLines()
			43.	TKIOSK_MarkerFeedPaper()
			44.	TKIOSK_CutPaper()
			45.	TKIOSK_S_SetLeftMarginAndAreaWidth()
			46.	TKIOSK_S_SetAlignMode()
			47.	TKIOSK_S_Textout()
			48.	TKIOSK_S_DownloadPrintBmp()
			49.	TKIOSK_S_PrintBmpInRAM()
			50.	TKIOSK_S_PrintBmpInFlash()
			51.	TKIOSK_S_PrintBarcode()
			52.	TKIOSK_P_SetAreaAndDirection()
			53.	TKIOSK_P_Textout()
			54.	TKIOSK_P_DownloadPrintBmp()
			55. TKIOSK_P_PrintBmpInRAM()
			56.	TKIOSK_P_PrintBmpInFlash()
			57.	TKIOSK_P_PrintBarcode()
			58.	TKIOSK_P_Print()
			59.	TKIOSK_P_Clear()
			60.	TKIOSK_P_Clear()
			61.	TKIOSK_S_TestPrint()
			62.	TKIOSK_SetPrintFromStart()
			63. TKIOSK_SetRaster()
			64.	TKIOSK_SetChineseFont()
			65.	TKIOSK_EnumNetPrinter()
			66.	TKIOSK_EnumDrvPrinter()
**********************************************************************************************************************/
#ifndef __TKIOSKDLL__H__
#define __TKIOSKDLL__H__

/**********************************************************************************************************************
*端口类型定义
**********************************************************************************************************************/
#define		TKIOSK_COM				0x00					//串口
#define		TKIOSK_LPT				(TKIOSK_COM + 0x01)		//并口
#define		TKIOSK_USB				(TKIOSK_COM + 0x02)		//USB接口
#define		TKIOSK_DRV				(TKIOSK_COM + 0x03)		//驱动接口
#define		TKIOSK_NIBBLEPAR		(TKIOSK_COM + 0x04)		//字节并口
#define		TKIOSK_NET				(TKIOSK_COM + 0x05)		//网络接口

/**********************************************************************************************************************
*返回值宏定义
**********************************************************************************************************************/
#define		TKIOSK_SUCCESS						1001	//正常完成
#define		TKIOSK_FAIL							1002	//操作错误
#define		TKIOSK_ERROR_INVALID_HANDLE			1101	//内部句柄错误或者有错误发生
#define		TKIOSK_ERROR_INVALID_PARAMETER		1102	//参数错误
#define		TKIOSK_ERROR_LOADLIBRARY			1103	//加载动态库失败
#define		TKIOSK_ERROR_INVALID_PATH			1201	//路径错误
#define		TKIOSK_ERROR_NOT_BITMAP				1202	//文件不是位图
#define		TKIOSK_ERROR_NOT_MONO_BITMAP		1203	//位图不是单色的
#define		TKIOSK_ERROR_BEYOND_AREA			1204	//位图过大不能打印

/**********************************************************************************************************************
*并口，字节并口句柄定义
**********************************************************************************************************************/
#define		TKIOSK_LPT_HANDLE					-1		//正常完成
#define		TKIOSK_NIBBLEPAR_HANDLE				-2		//操作错误

/**********************************************************************************************************************
*串口停止位选项
**********************************************************************************************************************/
#define		TKIOSK_COM_ONESTOPBIT				0x00							//停止位为1
#define		TKIOSK_COM_TWOSTOPBITS				(TKIOSK_COM_ONESTOPBIT + 0x01)	//停止位为2

/**********************************************************************************************************************
*串口校验位选项
**********************************************************************************************************************/
#define		TKIOSK_COM_NOPARITY					0x00							//无校验
#define		TKIOSK_COM_ODDPARITY				(TKIOSK_COM_NOPARITY + 0x01)	//奇校验
#define		TKIOSK_COM_EVENPARITY				(TKIOSK_COM_NOPARITY + 0x02)	//偶校验
#define		TKIOSK_COM_MARKPARITY				(TKIOSK_COM_NOPARITY + 0x03)	//MARK校验
#define		TKIOSK_COM_SPACEPARITY				(TKIOSK_COM_NOPARITY + 0x04)	//SPACE校验

/**********************************************************************************************************************
*串口数据流控制位选项
**********************************************************************************************************************/
#define		TKIOSK_COM_DTR_DSR					0x00							//DTR_DSR
#define		TKIOSK_COM_RTS_CTS					(TKIOSK_COM_DTR_DSR + 0x01)		//RTS_CTS	
#define		TKIOSK_COM_XON_XOFF					(TKIOSK_COM_DTR_DSR + 0x02)		//XON_XOFF
#define		TKIOSK_COM_NO_HANDSHAKE				(TKIOSK_COM_DTR_DSR + 0x03)		//无握手

/**********************************************************************************************************************
*打印模式
**********************************************************************************************************************/	
#define		TKIOSK_PRINT_MODE_STANDARD			0x00								//标准模式	
#define		TKIOSK_PRINT_MODE_PAGE				(TKIOSK_PRINT_MODE_STANDARD + 0x01)	//页模式

/**********************************************************************************************************************
*纸张类型
**********************************************************************************************************************/	
#define		TKIOSK_PAPER_SERIAL					0x00							//连续纸
#define		TKIOSK_PAPER_SIGN					(TKIOSK_PAPER_SERIAL + 0x01)	//标记纸

/**********************************************************************************************************************
*字符类型
**********************************************************************************************************************/
#define		TKIOSK_FONT_TYPE_STANDARD			0x00								//标准字体
#define		TKIOSK_FONT_TYPE_COMPRESSED			(TKIOSK_FONT_TYPE_STANDARD + 0x01)	//压缩字体
#define		TKIOSK_FONT_TYPE_UDC				(TKIOSK_FONT_TYPE_STANDARD + 0x02)	//自定义字体
#define		TKIOSK_FONT_TYPE_CHINESE			(TKIOSK_FONT_TYPE_STANDARD + 0x03)	//汉字

/**********************************************************************************************************************
*字符的风格
**********************************************************************************************************************/
#define		TKIOSK_FONT_STYLE_NORMAL			0x00	//正常字体
#define		TKIOSK_FONT_STYLE_BOLD				0x08	//加粗字体	
#define		TKIOSK_FONT_STYLE_THIN_UNDERLINE	0x80	//单下划线
#define		TKIOSK_FONT_STYLE_THICK_UNDERLINE	0x100	//双下划线
#define		TKIOSK_FONT_STYLE_UPSIDEDOWN		0x200	//倒置字体
#define		TKIOSK_FONT_STYLE_REVERSE			0x400	//反显字体
#define		TKIOSK_FONT_STYLE_CLOCKWISE_90		0x1000	//旋转字体

/**********************************************************************************************************************
*位图打印模式
**********************************************************************************************************************/
#define		TKIOSK_BITMAP_MODE_8SINGLE_DENSITY		0x00	//8点单密度
#define		TKIOSK_BITMAP_MODE_8DOUBLE_DENSITY		0x01	//8点双密度
#define		TKIOSK_BITMAP_MODE_24SINGLE_DENSITY		0x20	//24点单密度
#define		TKIOSK_BITMAP_MODE_24DOUBLE_DENSITY		0x21	//24点双密度

/**********************************************************************************************************************
*位图打印模式
**********************************************************************************************************************/
#define		TKIOSK_BITMAP_PRINT_NORMAL			0x00								//正常
#define		TKIOSK_BITMAP_PRINT_DOUBLE_WIDTH	(TKIOSK_BITMAP_PRINT_NORMAL + 0x01)	//倍宽
#define		TKIOSK_BITMAP_PRINT_DOUBLE_HEIGHT	(TKIOSK_BITMAP_PRINT_NORMAL + 0x02)	//倍高
#define		TKIOSK_BITMAP_PRINT_QUADRUPLE		(TKIOSK_BITMAP_PRINT_NORMAL + 0x03)	//倍宽倍高

/**********************************************************************************************************************
*条码类型
**********************************************************************************************************************/
#define		TKIOSK_BARCODE_TYPE_UPC_A           0x41								//UPC_A
#define		TKIOSK_BARCODE_TYPE_UPC_E           (TKIOSK_BARCODE_TYPE_UPC_A + 0x01)	//UPC_E
#define		TKIOSK_BARCODE_TYPE_JAN13           (TKIOSK_BARCODE_TYPE_UPC_A + 0x02)	//JAN13
#define		TKIOSK_BARCODE_TYPE_JAN8            (TKIOSK_BARCODE_TYPE_UPC_A + 0x03)	//JAN8 
#define		TKIOSK_BARCODE_TYPE_CODE39          (TKIOSK_BARCODE_TYPE_UPC_A + 0x04)	//CODE39
#define		TKIOSK_BARCODE_TYPE_ITF             (TKIOSK_BARCODE_TYPE_UPC_A + 0x05)	//ITF 
#define		TKIOSK_BARCODE_TYPE_CODEBAR         (TKIOSK_BARCODE_TYPE_UPC_A + 0x06)	//CODEBAR
#define		TKIOSK_BARCODE_TYPE_CODE93          (TKIOSK_BARCODE_TYPE_UPC_A + 0x07)	//CODE93
#define		TKIOSK_BARCODE_TYPE_CODE128         (TKIOSK_BARCODE_TYPE_UPC_A + 0x08)	//CODE128

/**********************************************************************************************************************
*Hri字体的位置
**********************************************************************************************************************/
#define		TKIOSK_HRI_POSITION_NONE			0x00								//无
#define		TKIOSK_HRI_POSITION_ABOVE			(TKIOSK_HRI_POSITION_NONE + 0x01)	//上侧
#define		TKIOSK_HRI_POSITION_BELOW			(TKIOSK_HRI_POSITION_NONE + 0x02)	//下侧
#define		TKIOSK_HRI_POSITION_BOTH			(TKIOSK_HRI_POSITION_NONE + 0x03)	//上下两侧

/**********************************************************************************************************************
*页模式打印区域的方向
**********************************************************************************************************************/
#define		TKIOSK_AREA_LEFT_TO_RIGHT			0x00								//左到右
#define		TKIOSK_AREA_BOTTOM_TO_TOP			(TKIOSK_AREA_LEFT_TO_RIGHT + 0x01)	//下到上
#define		TKIOSK_AREA_RIGHT_TO_LEFT			(TKIOSK_AREA_LEFT_TO_RIGHT + 0x02)	//右到左
#define		TKIOSK_AREA_TOP_TO_BOTTOM			(TKIOSK_AREA_LEFT_TO_RIGHT + 0x03)	//上到下

/**********************************************************************************************************************
* 外部接口函数定义
**********************************************************************************************************************/
#ifdef	__cplusplus
extern "C" {
#endif
/**********************************************************************************************************************
* 功能:		打开串口
* 入口参数:	lpName: 串口名，不能为空
*			nComBaudrate: 每秒位数, 2400 - 115200
*			nComDataBits: 数据位, 7-8
*			nComStopBits: 停止位, TKIOSK_COM_ONESTOPBIT, TKIOSK_COM_TWOSTOPBITS
*			nComParity: 校验位，TKIOSK_COM_NOPARITY - TKIOSK_COM_SPACEPARITY
*			nFlowControl: 数据流控制, TKIOSK_COM_DTR_DSR - TKIOSK_COM_NO_HANDSHAKE
* 出口参数:	无
* 返回值: 成功返回打开端口句柄, 失败返回INVALID_HANDLE_VALUE
**********************************************************************************************************************/
	HANDLE WINAPI TKIOSK_OpenCom(LPCTSTR lpName, int nComBaudrate, int nComDataBits, int nComStopBits, int nComParity, int nFlowControl);

/**********************************************************************************************************************
* 功能:		关闭串口
* 入口参数:	hPort: 句柄，不能为非本动态库打开句柄
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
**********************************************************************************************************************/
	int WINAPI TKIOSK_CloseCom(HANDLE hPort);

/**********************************************************************************************************************
* 功能:		打开驱动并口
* 入口参数:	LPTAddress: 并口地址, >0
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_OpenLptByDrv(WORD LPTAddress);

/**********************************************************************************************************************
* 功能:		关闭驱动并口
* 入口参数:	nPortType: 端口类型, 固定为TKIOSK_LPT
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_CloseDrvLPT(int nPortType);

/**********************************************************************************************************************
* 功能:		打开USB接口
* 入口参数:	无
* 出口参数:	无
* 返回值:	成功返回打开端口句柄, 失败返回INVALID_HANDLE_VALUE
**********************************************************************************************************************/
	HANDLE WINAPI TKIOSK_OpenUsb(void);

/**********************************************************************************************************************
* 功能:		打开USB接口
* 入口参数:	nID: 设备内部ID号, >0
* 出口参数:	无
* 返回值:	成功返回打开端口句柄, 失败返回INVALID_HANDLE_VALUE
**********************************************************************************************************************/
	HANDLE WINAPI TKIOSK_OpenUsbByID(int nID);

/**********************************************************************************************************************
* 功能:		关闭USB口
* 入口参数:	hPort: 句柄，不能为非本动态库打开句柄
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
**********************************************************************************************************************/
	int WINAPI TKIOSK_CloseUsb(HANDLE hPort);

/**********************************************************************************************************************
* 功能:		打开驱动程序
* 入口参数:	hPort: 句柄，不能为非本动态库打开句柄
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
**********************************************************************************************************************/
	HANDLE WINAPI TKIOSK_OpenDrv(char *drivername);

/**********************************************************************************************************************
* 功能:		关闭驱动程序
* 入口参数:	hPort: 句柄，不能为非本动态库打开句柄
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
**********************************************************************************************************************/
	int WINAPI TKIOSK_CloseDrv(HANDLE hPort);

/**********************************************************************************************************************
* 功能:		打开字节并口
* 入口参数:	PortNumber: 端口号，>0
*			DeviceNumber: 设备号（用于级联），取值范围：1-4
*			DriverPath: 驱动文件所在目录, 不能为空
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
			TKIOSK_ERROR_LOADLIBRARY 加载动态库失败
**********************************************************************************************************************/
	int	WINAPI TKIOSK_OpenNibblePar(int PortNumber, int DeviceNumber, char *DriverPath);

/**********************************************************************************************************************
* 功能:		关闭字节并口
* 入口参数:	无
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
**********************************************************************************************************************/
	int	WINAPI TKIOSK_CloseNibblePar();

/**********************************************************************************************************************
* 功能:		新建一个打印作业
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
**********************************************************************************************************************/
	int WINAPI TKIOSK_StartDoc(HANDLE hPort);

/**********************************************************************************************************************
* 功能:		结束打印作业
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
**********************************************************************************************************************/
	int WINAPI TKIOSK_EndDoc(HANDLE hPort);

/**********************************************************************************************************************
* 功能:		设置串口超时
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nWriteTimeoutMul: 写超时系数, >=0
*			nWriteTimeoutCon: 写超时常量, >=0
*			nReadTimeoutMul: 读超时系数, >=0
*			nReadTimeoutCon: 读超时常量, >=0
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetComTimeOuts(HANDLE hPort, int nWriteTimeoutMul, int nWriteTimeoutCon, int nReadTimeoutMul, int nReadTimeoutCon);

/**********************************************************************************************************************
* 功能:		设置USB超时
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			wReadTimeouts: 写超时, >=0
*			wWriteTimeouts: 读超时, >=0
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetUsbTimeOuts(HANDLE hPort,WORD wReadTimeouts, WORD wWriteTimeouts);

/**********************************************************************************************************************
* 功能:		设置字节并口超时
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			wReadTimeouts: 写超时, >=0
*			wWriteTimeouts: 读超时, >=0
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetNibbleParTimeOuts(HANDLE hPort, WORD wReadTimeouts, WORD wWriteTimeouts);

/**********************************************************************************************************************
* 功能:		字节并口开始累加数据到内存
* 入口参数:	无
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
**********************************************************************************************************************/
	int	WINAPI TKIOSK_NibbleParPrintToMemory();

/**********************************************************************************************************************
* 功能:		字节并口结束累加数据，并打印内存数据
* 入口参数:	无
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
**********************************************************************************************************************/
	int	WINAPI TKIOSK_NibbleParFlushMemory();

/**********************************************************************************************************************
* 功能:		发送数据到端口或文件
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM - TKIOSK_NET
*			pszData: 数据缓冲区, >=0
*			nBytesToWrite: 写数据长度
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_WriteData(HANDLE hPort, int nPortType, char *pszData, int nBytesToWrite);

/**********************************************************************************************************************
* 功能:		从指定端口读取数据
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM - TKIOSK_NET
*			pszData: 数据缓冲区, >=0
*			nBytesToWrite: 写数据长度
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_ReadData(HANDLE hPort, int nPortType, int nStatus, char *pszBuffer,int nBytesToRead);

/**********************************************************************************************************************
* 功能:		发送指令文件
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM - TKIOSK_NET
*			filename: 发送文件路径, 不能为空
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
			TKIOSK_ERROR_INVALID_PATH 路径错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SendFile(HANDLE hPort, int nPortType, char *filename);

/**********************************************************************************************************************
* 功能:		开始把发送的数据保存到指定的文件
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			bToPrinter: 是否发送到打印机标志
*			lpFileName: 发送文件路径, 不能为空
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_BeginSaveToFile(HANDLE hPort, LPCTSTR lpFileName, BOOL bToPrinter);

/**********************************************************************************************************************
* 功能:		结束保存数据到文件
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
**********************************************************************************************************************/
	int WINAPI TKIOSK_EndSaveToFile(HANDLE hPort);

/**********************************************************************************************************************
* 功能:		查询打印机的状态
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM, TKIOSK_USB, TKIOSK_PARALLEL_LPT, TKIOSK_NET
*			pszStatus: 接收数据缓冲区, 不能为空
*			nTimeouts: 超时时间, >=0
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_QueryStatus(HANDLE hPort, int nPortType, char *pszStatus, int nTimeouts);

/**********************************************************************************************************************
* 功能:		实时查询打印机的状态
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM, TKIOSK_USB, TKIOSK_PARALLEL_LPT, TKIOSK_NET
*			pszStatus: 接收数据缓冲区, 不能为空
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_RTQueryStatus(HANDLE hPort, int nPortType, char *pszStatus);

/**********************************************************************************************************************
* 功能:		自动状态返回
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM, TKIOSK_USB, TKIOSK_PARALLEL_LPT, TKIOSK_NET
*			Enable: 1-开启自动状态返回，0-关闭自动状态返回
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_QueryASB(HANDLE hPort, int nPortType, int Enable);

/**********************************************************************************************************************
* 功能:		获取当前 dll 的发布版本号
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			pnMajor: 主版本
*			pnMinor: 小版本
* 出口参数:	无
* 返回值:	成功返回日期, 失败返回TKIOSK_ERROR_INVALID_PARAMETER
**********************************************************************************************************************/
	int WINAPI TKIOSK_GetVersionInfo(int *pnMajor, int *pnMinor);

/**********************************************************************************************************************
* 功能:		复位打印机
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_Reset(HANDLE hPort, int nPortType);

/**********************************************************************************************************************
* 功能:		选择纸张类型
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nMode: TKIOSK_PAPER_SERIAL为连续纸, TKIOSK_PAPER_SIGN为标记纸
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetPaperMode(HANDLE hPort,int nPortType,int nMode);

/**********************************************************************************************************************
* 功能:		选择纸张类型
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nPrintMode: KIOSK_PRINT_MODE_STANDARD-标准模式, KIOSK_PRINT_MODE_PAGE-页模式
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetMode(HANDLE hPort,int nPortType,int nPrintMode);

/**********************************************************************************************************************
* 功能:		设置字符的行高
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nDistance: 行高，0-255
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetLineSpacing(HANDLE hPort,int nPortType, int nDistance);

/**********************************************************************************************************************
* 功能:		设置字符的右间距
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nDistance: 右间距，0-255
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetRightSpacing(HANDLE hPort,int nPortType,int nDistance);

/**********************************************************************************************************************
* 功能:		设置相对打印位置
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nHorizontalDist: 纵向相对位置，0-65535
*			nVerticalDist: 横向相对位置，0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetOppositePosition(HANDLE hPort,int nPortType,int nPrintMode,int nHorizontalDist,int nVerticalDist);

/**********************************************************************************************************************
* 功能:		设置跳格位置
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszPosition: 位置信息，不能为空
*			nCount: 数量，0 <= nCount <= 32
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetTabs(HANDLE hPort, int nPortType, char *pszPosition, int nCount);

/**********************************************************************************************************************
* 功能:		执行跳格操作
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_ExecuteTabs(HANDLE hPort,int nPortType);

/**********************************************************************************************************************
* 功能:		下载位图到RAM
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszPaths: 位图文件路径, 不能为空
*			nTranslateMode：转换方式，1-2
*			nID：存储ID, 0-7
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
			TKIOSK_ERROR_INVALID_PATH 路径错误
			TKIOSK_ERROR_NOT_BITMAP	不是位图
			TKIOSK_ERROR_BEYOND_AREA 位图过大
**********************************************************************************************************************/
	int WINAPI TKIOSK_PreDownloadBmpToRAM(HANDLE hPort,int nPortType, char *pszPaths, int nTranslateMode, int nID);

/**********************************************************************************************************************
* 功能:		下载位图到RAM
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszPaths[]: 位图文件路径数组, 不能为空
*			nTranslateMode：转换方式，1-2
*			nCount: 下载数量, 1 <= nCount <= 255
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
			TKIOSK_ERROR_INVALID_PATH 路径错误
			TKIOSK_ERROR_NOT_BITMAP	不是位图
			TKIOSK_ERROR_BEYOND_AREA 位图过大
**********************************************************************************************************************/
	int WINAPI TKIOSK_PreDownloadBmpToFlash(HANDLE hPort, int nPortType, char *pszPaths[], int nTranslateMode, int nCount);

/**********************************************************************************************************************
* 功能:		选择国际字符集和代码页
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nCharSet: 选择字符集, 0 <= nCharSet <= 13
*			nCodePage: 选择代码页, 0 <= nCodePage <= 5或16 <= nCodePage <= 19
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetCharSetAndCodePage(HANDLE hPort,int nPortType,int nCharSet,int nCodePage);

/**********************************************************************************************************************
* 功能:		向前走纸一行
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_FeedLine(HANDLE hPort, int nPortType);

/**********************************************************************************************************************
* 功能:		打印并向前走纸n行
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nLines: 走纸行数, 0-255
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_FeedLines(HANDLE hPort,int nPortType,int nLines);

/**********************************************************************************************************************
* 功能:		将下一标记送到打印位置
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nLines: 走纸行数, 0-255
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_MarkerFeedPaper(HANDLE hPort, int nPortType);

/**********************************************************************************************************************
* 功能:		切纸
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nMode: 0-使用驱动程序打印时的页面大小来设定走纸距离, 1-走纸距离距离nDistance确定
*			nDistance: 0-255
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_CutPaper(HANDLE hPort,int nPortType, int nMode, int nDistance);

/**********************************************************************************************************************
* 功能:		标准模式下设置左边距和打印区域宽度
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nDistance: 左边距，0 <= nDistance <= 65535
*			nWidth: 打印区域宽度，0 <= nWidth <= 65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_S_SetLeftMarginAndAreaWidth(HANDLE hPort,int nPortType,int nDistance,int nWidth);

/**********************************************************************************************************************
* 功能:		标准模式下设置字符的对齐模式
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nMode: 左边距，0-2
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_S_SetAlignMode(HANDLE hPort, int nPortType, int nMode);

/**********************************************************************************************************************
* 功能:		把将要打印的字符串数据发送到打印缓冲区中
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszData: 数据, 不能为空
*			nWidthTimes: 横向放大倍数, 1-6
*			nHeightTimes: 纵向放大倍数，1-6
*			nFontType: 字体类型风格，0-3
*			nFontStyle：字体风格，0-1F88
*			nOrgx:横向位置0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_S_Textout(HANDLE hPort, int nPortType,char *pszData, int nOrgx, int nWidthTimes,     
								int nHeightTimes, int nFontType, int nFontStyle);

/**********************************************************************************************************************
* 功能:		下载并打印位图
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszPath: 位图路径, 不能为空
*			nTranslateMode: 处理模式, 1-2
*			nMode: 打印模式，TKIOSK_BITMAP_MODE_8SINGLE_DENSITY，TKIOSK_BITMAP_MODE_8DOUBLE_DENSITY
*							 TKIOSK_BITMAP_MODE_24SINGLE_DENSITY，TKIOSK_BITMAP_MODE_24DOUBLE_DENSITY
*			nOrgx:横向位置0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_S_DownloadPrintBmp(HANDLE hPort, int nPortType, char *pszPath, 
										int nTranslateMode, int nOrgx, int nMode);

/**********************************************************************************************************************
* 功能:		打印已下载在RAM中的位图
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nID: 内部ID, 0-7
*			nMode: 打印模式，TKIOSK_BITMAP_PRINT_NORMAL- TKIOSK_BITMAP_PRINT_QUADRUPLE
*			nOrgx:横向位置0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int  WINAPI TKIOSK_S_PrintBmpInRAM(HANDLE hPort,int nPortType,int nID, int nOrgx, int nMode);

/**********************************************************************************************************************
* 功能:		打印已下载在Flash中的位图
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nID: 内部ID, 1-255
*			nMode: 打印模式，TKIOSK_BITMAP_PRINT_NORMAL- TKIOSK_BITMAP_PRINT_QUADRUPLE
*			nOrgx:横向位置0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_S_PrintBmpInFlash(HANDLE hPort, int nPortType, int nID, int nOrgx, int nMode);

/**********************************************************************************************************************
* 功能:		设置打印条码
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszBuffer:条码数据, 不能为空
*			nOrgx:条码横起始点横坐标，0<=nOrgx<=65535
*			nType:条码类型，TKIOSK_BARCODE_TYPE_UPC_A<=nType<=TKIOSK_BARCODE_TYPE_CODE128
*			nWidth:基本元素的宽度，2<=nWidth<=6
*			nHeight:基本元素的高度，1<=nWidth<=255
*			nHriFontType:Hri字体的模式，nHriFontType=0、1
*			nHriFontPosition:Hri字体位置，0<=nHriFontPosition<=3
*			nBytesOfBuffer:条码数据个数, 与条码类型有关
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_S_PrintBarcode(HANDLE hPort, int nPortType, char *pszBuffer, int nOrgx, int nType, 
									int nWidth, int nHeight, int nHriFontType, int nHriFontPosition, int nBytesOfBuffer);

/**********************************************************************************************************************
* 功能:		设置页面的打印区域及起始方向
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nOrgx: 起始点横坐标，0<=nOrgx<=65535
*			nOrgy: 起始点纵坐标，0<=nOrgy<=65535
*			nWidth: 打印区域的宽度，0<=nWidth<=65535
*			nHeight: 打印区域的高度，0<=nHeight<=65535
*			nDirection: 打印方向，TKIOSK_AREA_LEFT_TO_RIGHT-TKIOSK_AREA_TOP_TO_BOTTOM
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_P_SetAreaAndDirection(HANDLE hPort,int nPortType,int nOrgx, int nOrgy,        
											int nWidth, int nHeight, int nDirection);

/**********************************************************************************************************************
* 功能:		把将要打印的字符串数据发送到打印缓冲区中
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszData: 数据, 不能为空
*			nWidthTimes: 横向放大倍数, 1-6
*			nHeightTimes: 纵向放大倍数，1-6
*			nFontType: 字体类型风格，TKIOSK_FONT_TYPE_STANDARD - TKIOSK_FONT_TYPE_CHINESE
*			nFontStyle：字体风格，0-1F88
*			nOrgx:横向位置0-65535
*			nOrgy:纵向位置0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_P_Textout(HANDLE hPort,int nPortType, char *pszData, int nOrgx, int nOrgy,          
								int nWidthTimes, int nHeightTimes, int nFontType, int nFontStyle);

/**********************************************************************************************************************
* 功能:		下载并打印位图
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszPath: 位图路径, 不能为空
*			nTranslateMode: 处理模式, 1-2
*			nMode: 打印模式，TKIOSK_BITMAP_MODE_8SINGLE_DENSITY，TKIOSK_BITMAP_MODE_8DOUBLE_DENSITY
*							 TKIOSK_BITMAP_MODE_24SINGLE_DENSITY，TKIOSK_BITMAP_MODE_24DOUBLE_DENSITY
*			nOrgx:横向位置0-65535
*			nOrgy:纵向位置0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_P_DownloadPrintBmp(HANDLE hPort, int nPortType,char *pszPath,
								int nTranslateMode, int nOrgx,int nOrgy, int nMode);

/**********************************************************************************************************************
* 功能:		打印已下载在RAM中的位图
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nID: 内部ID, 0-7
*			nMode: 打印模式，TKIOSK_BITMAP_PRINT_NORMAL- TKIOSK_BITMAP_PRINT_QUADRUPLE
*			nOrgx:横向位置0-65535
*			nOrgy:纵向位置0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_P_PrintBmpInRAM(HANDLE hPort,int nPortType, int nID, int nOrgx, int nOrgy, int nMode);

/**********************************************************************************************************************
* 功能:		打印已下载在Flash中的位图
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nID: 内部ID, 1-255
*			nMode: 打印模式，TKIOSK_BITMAP_PRINT_NORMAL- TKIOSK_BITMAP_PRINT_QUADRUPLE
*			nOrgx:横向位置0-65535.
*			nOrgy:纵向位置0-65535
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_P_PrintBmpInFlash(HANDLE hPort, int nPortType, int nID, int nOrgx, int nOrgy, int nMode);

/**********************************************************************************************************************
* 功能:		设置打印条码
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszBuffer:条码数据, 不能为空
*			nOrgx:条码横起始点横坐标，0<=nOrgx<=65535
*			nOrgy:条码横起始点横坐标，0<=nOrgy<=65535
*			nType:条码类型，TKIOSK_BARCODE_TYPE_UPC_A<=nType<=TKIOSK_BARCODE_TYPE_CODE128
*			nWidth:基本元素的宽度，2<=nWidth<=6
*			nHeight:基本元素的高度，1<=nWidth<=255
*			nHriFontType:Hri字体的模式，nHriFontType=0、1
*			nHriFontPosition:Hri字体位置，0<=nHriFontPosition<=3
*			nBytesOfBuffer:条码数据个数, 与条码类型有关
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_P_PrintBarcode(HANDLE hPort, int nPortType, char *pszBuffer, int nOrgx, int nOrgy, int nType, 
									int nWidth, int nHeight, int nHriFontType, int nHriFontPosition, int nBytesOfBuffer);

/**********************************************************************************************************************
* 功能:		打印页缓冲区内的内容
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_P_Print(HANDLE hPort,int nPortType);

/**********************************************************************************************************************
* 功能:		页模式下清空缓冲区
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_P_Clear(HANDLE hPort,int nPortType);

/**********************************************************************************************************************
* 功能:		执行测试打印
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_S_TestPrint(HANDLE hPort, int nPortType);

/**********************************************************************************************************************
* 功能:		设置打印起始位置在行首(标准模式下有效)
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nMode: 0-清除掉打印缓冲区的所有数据后，设置打印起始位置在行首
*				   1-打印完缓冲区的所有数据后，设置打印起始位置在行首
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetPrintFromStart(HANDLE hPort, int nPortType, int nMode);

/**********************************************************************************************************************
* 功能:		打印光栅
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			pszBmpPath: 光栅路径，不能为空
*			nTranslateMode: 1-2
*			nMode: TKIOSK_BITMAP_PRINT_NORMAL-TKIOSK_BITMAP_PRINT_QUADRUPLE
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetRaster(HANDLE hPort,int nPortType, char *pszBmpPath, int nTranslateMode, int nMode);

/**********************************************************************************************************************
* 功能:		设置汉字模式
* 入口参数:	hPort:句柄，不能为非本动态库打开句柄
*			nPortType: 端口类型, TKIOSK_COM-TKIOSK_NET
*			nEnable设置选择/取消汉字模式，nEnable= 0，1
*			nBigger=0，正常大小；nBigger=1，汉字放大
*			nLSpacing: 汉字左间距，0<=nLSpacing<=255
*			nRSpacing: 汉字右间距，0<=nRSpacing<=255
*			nUnderLine=0，无下划线；nUnderLine=1，一点宽下划线；nUnderLine=2点宽下划线
* 出口参数:	无
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL  操作失败
			TKIOSK_ERROR_INVALID_HANDLE 句柄无效
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_SetChineseFont(HANDLE hPort,int nPortType,char *pszBuffer,int nEnable,
								int nBigger,int nLSpacing,int nRSpacing,int nUnderLine);

/**********************************************************************************************************************
* 功能:		枚举网络打印机
* 入口参数:	无 
* 出口参数:	iplist: 网络中可搜索打印机IP列表，不同IP间用'@'分割，不能为空，长度足够长
*			Number: 搜索打印机数量， 不能为空
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL 操作失败
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_EnumNetPrinter(char *iplist, int *Number);

/**********************************************************************************************************************
* 功能:		枚举驱动打印机
* 入口参数:	无 
* 出口参数:	Drivername: PC安装驱动名称，不同名称间用'@'分割，不能为空，长度足够长
*			Number: PC安装驱动数量，不能为空
*			KeyWord: 过滤信息，不能为空
* 返回值:	成功返回TKIOSK_SUCCESS, 失败返回如下值：
			TKIOSK_FAIL 操作失败
			TKIOSK_ERROR_INVALID_PARAMETER 参数错误
**********************************************************************************************************************/
	int WINAPI TKIOSK_EnumDrvPrinter(char *Drivername, int *Number, char *KeyWord);
#ifdef	__cplusplus
}
#endif
#endif