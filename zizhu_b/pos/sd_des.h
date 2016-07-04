/*******************************************************************************
 * ds_des.h
 *
 *  Created on: 
 *      Author: zfs
 ******************************************************************************/
#ifndef _SD_DES_H_
#define _SD_DES_H_
//#include "define.h"

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef U8
#define U8 unsigned char
#endif

#ifndef LPBYTE
#define LPBYTE BYTE *
#endif

extern void DES(U8 * key,U8 * text,U8 * mtext);

extern void DES_1(U8 * key,U8 * text,U8 * mtext);

extern void DES_3  (U8 * key16,U8 * text,U8 * mtext);

extern void DES_31(U8 * key16,U8 * text,U8 * mtext);

void Des1_ECBMac(unsigned char *key, const unsigned char *text, int textsize,
             unsigned char *rtn_mac);
#endif /* _SD_DES_H_                                                          */
