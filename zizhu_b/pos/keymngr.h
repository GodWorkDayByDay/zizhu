
#ifndef __KEYMNGR_H
#define __KEYMNGR_H

/* 从IC卡导入密钥 */
int KeyMngr_ImportKeyICCard(void *dummy);

/* 从外部端口导入密钥(COM1/COM2/USB DEV等等) */
int KeyMngr_ImportKeyExtPort(void *dummy);

/* 手输密钥 */
int KeyMngr_ManualLoadKey(void *dummy);

/* 计算MAC */
int KeyMngr_CalMac(byte *pPacketBuffer, int iPacketLen, byte *pRetMac);

/* 密钥校验－根据长度来判断密钥的种类和算法类型：
   长度24为单倍长MACKEY和PINKEY
   长度40为双倍长MACKEY和PINKEY
   长度60为双倍长MACKEY和PINKEY和TRKKEY
   传入数据和长度皆为转换后的ASCII码长度 */
int KeyMngr_CheckVal(char *pKeyBuffer, int iKeyDataLen);

int KeyMngr_SetTestKey( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_EncryptTrackData
 * DESCRIPTION:   加密磁道信息
 * PARAMETERS     OrgTrackData: ASCII码格式的磁道信息
 *                EncryptTrackData: 加密后的磁道信息
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_EncryptTrackData( byte *OrgTrackData, byte *EncryptTrackData );

#endif
