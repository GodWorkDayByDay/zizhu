
/***************************************************************************
* FILE NAME:    AppDebug.h                                                 *
* MODULE NAME:  APPDEBUG                                                   *
* PROGRAMMER:   Laikey                                                     *
* DESCRIPTION:                                                             *
* REVISION:                                                                *
****************************************************************************/

#ifndef __APPDEBUG_H__
#define __APPDEBUG_H__

#define APPDEBUGMODE_PRINT          0x01	// 打印模式输出调试
#define APPDEBUGMODE_COMOUTPUT      0x02	// 串口模式输出调试
#define APPDEBUGMODE_DISPLAY        0x04	// 显示模式输出调试
#define APPDEBUGMODE_PACKETDATA     0x08	// 通讯报文输出调试
#define APPDEBUGMODE_EMVTAGDATA     0x10	// 打印EMV信息输出模式
#define APPDEBUGMODE_SAVEFILE       0x20	// 保存调试信息输出模式

#ifdef MAKE_APPDEBUG
	#ifdef MAKE_LINUX
		#define AppDebug_AppLog( pString, ...) \
		{ \
			printf( "[%s-%s] T:%lu|F:%s|L:%d| ", MultHead_GetAppName(), MultHead_GetManageVersion(), (unsigned long)read_ticks(), strrchr(__FILE__,0x2f)+1, __LINE__ ); \
			printf( pString, ##__VA_ARGS__); \
			printf( "\n" ); \
		}
	#else
		#define AppDebug_AppLog( pString, ...) \
		{ \
			AppDebug_OutputLog( "[%s-%s] T:%lu|F:%s|%s()|L:%d|", MultHead_GetAppName(), MultHead_GetManageVersion(), (unsigned long)read_ticks(), __MODULE__, __FUNCTION__, __LINE__ ); \
		    AppDebug_OutputLog( (char *)pString, ##__VA_ARGS__ ); \
		    AppDebug_OutputLog( "\n" ); \
		}
	#endif
#else
    #define AppDebug_AppLog( pString, ...)
#endif

#define ASSERT(x)	(x)

#define ASSERT_FAIL(x) \
	if (ASSERT(x) != BANK_OK)\
	{\
		return(BANK_FAIL);\
	}

#define ASSERT_USERCANCEL(x) \
	if (ASSERT(x) == BANK_USERCANCEL)\
	{\
		Comm_Disconnect();\
		return(BANK_FAIL);\
	}
	
#define ASSERT_HANGUP_FAIL(x) \
	if (ASSERT(x) != BANK_OK)\
	{\
		Comm_Disconnect();\
		return(BANK_FAIL);\
	}
	

	



/* -----------------------------------------------------------------------------
 * FUNCTION NAME: Debug_AppLog
 * DESCRIPTION:   Display/Output/Print the data according environment variable define
 * RETURN: none
 ---------------------------------------------------------------------------- */
void AppDebug_OutputLog(char *pString, ...);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppDebug_OutputDataByHex
 * DESCRIPTION:     Send out the data via COM, all data will convert to hex mode,
                    1 times is 32bytes data
 * RETURN: none
 ---------------------------------------------------------------------------- */
void AppDebug_OutputDataByHex(char *pOutputData, int iDataLength);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   AppDebug_OutputFileLog
 * DESCRIPTION:     Send out the data saved by file mode
 * RETURN: none
 ---------------------------------------------------------------------------- */
void AppDebug_OutputFileLog(void);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME: Debug_PrintISOPacketData
 * DESCRIPTION:   Print the data sent to bank host or received from bank host
 * RETURN: none
 ---------------------------------------------------------------------------- */
void AppDebug_OutputHexData(char *pMsgtitle, char *pPacketData, int iPacketDataLength);

/* -----------------------------------------------------------------------------
 * FUNCTION NAME: AppDebug_CheckAppDebugStatus
 * DESCRIPTION:   Check which mode can be used for debug
 * RETURN:        BANK_OK: bit match, BANK_FAIL: fail
 ---------------------------------------------------------------------------- */
int AppDebug_CheckAppDebugStatus(byte bCheckMaskStatus);


byte AppDebug_CheckReturnStatus( void );

void AppDebug_SetReturnStatus( byte bStatus );

char *AppDebug_GetCOMPortName( char *pRetComPortName );

#endif
