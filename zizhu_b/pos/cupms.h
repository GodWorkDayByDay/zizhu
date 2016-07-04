/****************************************************************************
 *
 * FILE NAME:   cupms.h                                                     *
 * MODULE NAME: DEFINE                                                      *
 * PROGRAMMER:  Laikey                                                      *
 *
 ***************************************************************************/

#ifndef __CUPMS_APP_H
#define __CUPMS_APP_H

// include c header
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include "iso8583.h"
#include "Proc8583.h"

#ifndef MAKE_LINUX
//#include "svc.h"
#endif

#define PP222_DES_MODE  1
#define PP222_3DES_MODE 3

#define FILE_SUCCESS 0

#define TRUE 1
#define FALSE 0
#define RIGHT_JST 0

#include "define.h"
#include "addtrans.h"
#include "apputils.h"
#include "comm.h"
#include "keymngr.h"
#include "multdef.h"

#include "appdebug.h"
#include "prmmngr.h"
#include "prndisp.h"
#include "trans.h"

#include "iccmd.h"
#include "zontalkap.h"

//#include "ctls.h"
//#include "ctlscall.h"

//#include "vfiemvif.h"
//#include "vfiemvmmi.h"

extern Term_Config gTermCfg;

#define     bENTER 0x0D
#define     bESC   0x1B
#define     bBKSP  0x08
typedef enum{
    bKEY0,
    bKEYC,
    bKEY1,
    bKEY2,
    bKEY3,
    bKEY4,
    bKEY5,
    bKEY6,
    bKEY7,
    bKEY8,
    bKEY9,
    bKEYD,
    bDOWN,
}key;
typedef enum{
    LINE0 = 0,
    LINE1,
    LINE2,
    LINE3,
    LINE4,
    LINE5,
    LINE6,
    LINE7,
    LINE8,
    LINE9,
    COMM_TYPE_SDLC,
    COMM_TYPE_GPRS,
    COMM_TYPE_ETHERNET,

}line;

typedef enum
{
    bALPHAKEY,
    INPUT_FAILED,
    LEFT_JST,
    INPUT_CANCEL,
    CENTER_JST,
    VS_SUCCESS,
    bUP,
}other;

typedef enum
{
    _VX510 = 0,
    _VX510G ,
    _VX610 ,
    _VX670 ,
    _VX810 ,
    _VX700 ,
    _VX520 ,
    _VX520S ,
    _VX680 ,
    _VX820 ,
    _VX680C ,
    _VX805 ,
    _VX825 ,
    _VX675 ,
    _VX600
}TerminalType;
#if defined(TRIDENT) || defined(MAKE_LINUX)
#include "ssl_interface.h"
#endif


#ifdef MAKE_LINUX

// for linux environment -->

//#include "emvTags.h"
//#include "emvcall.h"

typedef enum
{
	_VX510 = 0,
	_VX510G ,
	_VX610 ,
	_VX670 ,
	_VX810 ,
	_VX700 ,
	_VX520 ,
	_VX520S ,
	_VX680 ,
	_VX820 ,
	_VX680C ,
	_VX805 ,
	_VX825 ,
	_VX675 ,
	_VX600
}enumTerminalType;

typedef enum _FONT_SIZE
{
	FONT_16X16 = 16,
	FONT_24X24 = 24,
	FONT_28X28 = 28
}FONT_SIZE;

#define Rt_1200     2
#define Rt_2400     3
#define Rt_4800     4
#define Rt_9600     5
#define Rt_19200    6
#define Rt_38400    7
#define Rt_57600    8
#define Rt_115200   9
#define HANDHELD_DOCKED				0
#define SWITCH_DOWN					0
#define	DEV_CTLS					""
#define CHNFONT						""
#define	E_SERVICE_ALARM_SET			33

#define	DEV_USBSER					"/dev/ttyUSB0"
#define	DEV_USBD					"/dev/usbd"
#define	DEV_COM1					"/dev/ttyS0"
#define	DEV_COM2					"/dev/ttyS3"
#define	DEV_COM3					"/dev/ttyS3"
#define	DEV_COM4					"/dev/ttyS4"
#define	DEV_COM5					"/dev/ttyS5"
#define	DEV_COM6					"/dev/ttyS6"
	
static __inline int SVC_INFO_MODELNO( char *a )
{
	sprintf( a, "C520" );
	return( 0 );
}

static __inline int SVC_CLOCK( int a, char *b, int c )
{
//	return TimeUtil_RtcClock( a, b, c );
}

static __inline int get_touchscreen( int *x, int *y)
{
	return( 0 );
}

static __inline int get_powerswitch_status()
{
	return( 0 );
}
// <-- for linux

#else

// for vx environment -->
//#include "zontalk.h"

#define	ApplMngr_ClearEvent()
//#define	Print_CheckPrintFinish()
#define MmiUtil_StartTrack()
#define usleep(a)
#define EmvCall_ResetTxnAmount()
#define EmvCall_SetTxnAmount(a)

// <-- for vx
#endif

/*--------------------------------------------------------------------------
 FUNCTION NAME: File_AppendRecord.
 DESCRIPTION:   保存记录
 PARAMETERS:    none.
 RETURN:        void.
 config.ini->param_file
 *-------------------------------------------------------------------------*/
//int File_AppendRecord(char * FileName,void * SrcData, unsigned int SrcLen);
int print_qdebug(unsigned char * Title,unsigned char * buff, int len);

#endif

