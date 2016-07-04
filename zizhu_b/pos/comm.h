#ifndef __COMM_H
#define __COMM_H
#include "pos/define.h"

#define MAX_COMM_BUF_SIZE           (8192)

#define COMM_RET_OK                 1        //通讯正常
#define COMM_RET_SEND_OK            2        //发送成功
#define COMM_RET_RECV_OK            3        //接收成功  

#define COMM_RET_TYPE_FAIL          -1        //通讯类型错误
#define COMM_RET_REG_CANCEL         -2        //网络注册取消
#define COMM_RET_REG_FAIL           -3        //网络注册失败
#define COMM_RET_USER_CANCEL        -4        //用户取消

#define COMM_RET_FAIL               -5        //通信失败
#define COMM_RET_SEND_FAIL          -6        //发送失败
#define COMM_RET_RECV_TIMEOUT       -7        //接收超时
#define COMM_RET_RECV_FAIL          -8        //接收失败

//#ifndef byte
//#define byte unsigned char
//#endif
#define BOOL unsigned int

void Comm_AppInitComm( void );

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_RegisterNetwork
 * DESCRIPTION:     仅对于TCP类通讯方式－注册网络
 * PARAMETERS:      BackGroundFlag: 1为背景方式注册; 0为前景方式注册
 * RETURN:          COMM_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_RegisterNetwork(int BackGroundFlag);

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_LogoutNetwork
 * DESCRIPTION:     仅对于TCP类通讯方式登出网络
 *
 * RETURN:          COMM_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_LogoutNetwork(int BackGroundFlag);

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_CheckPreConnect
 * DESCRIPTION:     检查预拨号：如果终端当前为拨号通讯方式，且预拨号开关打开，
 *                  则进行预拨号
 * RETURN:          COMM_RET_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_CheckPreConnect(void);

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_CheckConnect
 * DESCRIPTION:     通讯连接：对于拨号方式为拨号；对于TCP通讯方式为建立socket
 *                  对于TCP类的通讯方式如果尚未注册网络本函数会先调用注册网络。
 *                  在发送和接收数据之前应调用本函数并判返回值
 * RETURN:          COMM_RET_OK
 *                  COMM_RET_FAIL
 *-----------------------------------------------------------------------------*/
int Comm_CheckConnect(void);

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_Disconnect
 * DESCRIPTION:     通讯断开：对于拨号方式为挂机；对于TCP通讯方式为释放socket
 *
 * RETURN:          COMM_OK
 *
 *-----------------------------------------------------------------------------*/
int Comm_Disconnect(void);


/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_SendPacket
 * DESCRIPTION:     发送数据
 *
 * RETURN:          COMM_RET_SEND_OK: 发送成功
 *                  COMM_RET_SEND_FAIL: 发送失败
 *-----------------------------------------------------------------------------*/
int Comm_SendPacket(byte *Data, int DataLen);

/*-----------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_RecvPacket
 * DESCRIPTION:     接收数据
 *
 * RETURN:          COMM_RET_RECV_OK: 接收成功
 *                  COMM_RET_RECV_FAIL: 接收失败
 *                  COMM_RET_RECV_TIMEOUT: 接收超时
 *-----------------------------------------------------------------------------*/
int Comm_RecvPacket(byte *Data, int *DataLen);

/*--------------------------------------------------------------------------
 * FUNCTION NAME:   COMM_Get_ErrCode.
 * DESCRIPTION:     显示通讯库错误代码含义
 * RETURN:
 * NOTE:
 *
 *-------------------------------------------------------------------------- */
int Comm_GetErrCode(int ErrCode, int Comm_Mode);

/*--------------------------------------------------------------------------
 * FUNCTION NAME:   Comm_IsTCPIPCommMode
 * DESCRIPTION:     是否TCPIP通讯方式(以支持SSL)
 *                  
 * RETURN:
 * NOTE:
 *
 *-------------------------------------------------------------------------- */
int Comm_IsTCPIPCommMode(int CommMode);

int Comm_CheckLineStatus( void );

int Commu_RS232Send( byte *Data, usint DataLen );
int Commu_RS232Recv( byte *Data, int iSizeofData, int *DataLen, usint Rs232Timeout);


/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_Connect
 DESCRIPTION:   SSL连接主机
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
//int Comm_SSL_Connect(int CommType, CommCnt_Param *ConnectParam, BOOL ManualCancel);

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_DisConnect
 DESCRIPTION:   SSL断开
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_DisConnect(int CommType, BOOL BackGroundFlag);

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_HostVerifyPos
 DESCRIPTION:   SSL双向认证方式，平台验证终端
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_HostVerifyPos(int CommType);

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_Recv
 DESCRIPTION:   SSL接收数据
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_Recv(int CommType, byte *RecvBuf, int MaxRecvBufSize, int *RcvLen, int RecvDataFmt, int TimeOutSec);

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SSL_Send
 DESCRIPTION:   SSL发送数据
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SSL_Send(int CommType, byte *SndData, int SndLen, int Fmt);


/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_SetDialAtCmd
 DESCRIPTION:   设置拨号AT指令
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_SetDialAtCmd(void);

/*--------------------------------------------------------------------------
 FUNCTION NAME: Comm_TCP_Disconnect
 DESCRIPTION:   TCP通讯方式挂线
 PARAMETERS:
 RETURN:        BANK_OK
 *-------------------------------------------------------------------------*/
int Comm_TCP_Disconnect(void);
#endif
