
#ifndef __KEYMNGR_H
#define __KEYMNGR_H

/* ��IC��������Կ */
int KeyMngr_ImportKeyICCard(void *dummy);

/* ���ⲿ�˿ڵ�����Կ(COM1/COM2/USB DEV�ȵ�) */
int KeyMngr_ImportKeyExtPort(void *dummy);

/* ������Կ */
int KeyMngr_ManualLoadKey(void *dummy);

/* ����MAC */
int KeyMngr_CalMac(byte *pPacketBuffer, int iPacketLen, byte *pRetMac);

/* ��ԿУ�飭���ݳ������ж���Կ��������㷨���ͣ�
   ����24Ϊ������MACKEY��PINKEY
   ����40Ϊ˫����MACKEY��PINKEY
   ����60Ϊ˫����MACKEY��PINKEY��TRKKEY
   �������ݺͳ��Ƚ�Ϊת�����ASCII�볤�� */
int KeyMngr_CheckVal(char *pKeyBuffer, int iKeyDataLen);

int KeyMngr_SetTestKey( void *dummy );

/* --------------------------------------------------------------------------
 * FUNCTION NAME: KeyMngr_EncryptTrackData
 * DESCRIPTION:   ���ܴŵ���Ϣ
 * PARAMETERS     OrgTrackData: ASCII���ʽ�Ĵŵ���Ϣ
 *                EncryptTrackData: ���ܺ�Ĵŵ���Ϣ
 * RETURN:
 * NOTES:
 * ------------------------------------------------------------------------ */
int KeyMngr_EncryptTrackData( byte *OrgTrackData, byte *EncryptTrackData );

#endif
