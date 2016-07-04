#ifndef  ISO8583_H
#define ISO8583_H
/*
本文件基本无用，稍后修改删除
*/
#include "define.h"
#define ISO8583_MAXFIELD		128
#define ISO8583_MAXLENTH  		2048

#define ISO8583TYPE_FIX			0x01    // type fix length
#define ISO8583TYPE_VAR			0x02    // type Variable length - 99/999

#define ISO8583TYPE_BIN			0x04    // type Binary - 'b','h'
#define ISO8583TYPE_ASC			0x08    // type ASCII  - 'a','an','ans'
#define ISO8583TYPE_BCD			0x10    // type BCD    - 'n','z'

#define F_RDONLY O_RDONLY
#define F_WRONLY O_WRONLY
#define F_SEEK_SET  SEEK_SET
#define F_RDWR O_RDWR
#define F_CREAT O_CREAT
//8583报文引擎: 最大报文域数量枚举
typedef enum
{
    ISO8583_BITMAP64 = 64,
    ISO8583_BITMAP128=128,
}ISO8583_BitMode;


//8583报文引擎: 域数据类型结构
typedef struct
{
    unsigned char bType;		// (Fix or Var) | (BIN or ASC or BCD)
    unsigned int iMaxLength;	// data max length
}ISO8583_FieldFormat;


typedef struct
{
    unsigned int iOffset;
    unsigned char cBitMap[ ISO8583_MAXFIELD ];
    unsigned char cData[ ISO8583_MAXLENTH ];
}ISO8583_Rec;


/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_SetFieldFormat
 * DESCRIPTION:     Set ISO8583 each field type and format
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void ISOEngine_SetFieldFormat( ISO8583_BitMode bBitMode, const ISO8583_FieldFormat *pFieldFormat );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_ClearAllBits
 * DESCRIPTION:     Clear the struct all bit flag
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void ISOEngine_ClearAllBits( ISO8583_Rec *cpIsoRec );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_ClearOneBit
 * DESCRIPTION:     Clear one field data
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void ISOEngine_ClearOneBit( ISO8583_Rec *cpIsoRec, int iFieldNo );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_SetBit
 * DESCRIPTION:     Set ISO8583 field data
                    pFieldData must be the ASC format
 * RETURN:          if successful, RETURN 0; else
 *                  -1: not set iso8583 field format
 *                  -2: iFieldNo > APPISO8583_MAXFIELD or iFieldNo <= 1
 *                  -3: iDataLength > 999
 *                  -4: iso8583 string total length already > ISO8583_MAXLENTH
 ---------------------------------------------------------------------------- */
int ISOEngine_SetBit( ISO8583_Rec *pIsoRec, int iFieldNo, unsigned char *pFieldData, int iDataLength );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_GetBit
 * DESCRIPTION:     Get ISO8583 field data
                    pRetFieldData must be the ASC format
 * RETURN:          Return FieldData length has gotten
 ---------------------------------------------------------------------------- */
int ISOEngine_GetBit( ISO8583_Rec *cpIsoRec, int iFieldNo, unsigned char *pRetFieldData, int iSizeofRetFieldData );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_StringToIso
 * DESCRIPTION:     Convert string to ISO8583 format
 * RETURN:          if success, RETURN 0, else RETURN -1/-2
 ---------------------------------------------------------------------------- */
int ISOEngine_StringToIso( ISO8583_Rec *cpIsoRec, unsigned char *pString );

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_IsoToString
 * DESCRIPTION:     Convert ISO8583 format to string
 * RETURN:          if success, RETURN 0, else RETURN -1/-2
 ---------------------------------------------------------------------------- */
int ISOEngine_IsoToString( ISO8583_Rec *cpIsoRec, unsigned char *pRetString, int iSizeofRetString );


int Trans_SetField53_Data(ISO8583_Rec *pReqIsoRec, DynData *pstTransData);

int File_DeleteRecordByIndex(char *data, int siz, int i);
int File_Remove(char *data);


int TimeUtil_MillSec();
int MmiUtil_ClearLine(int num);
int MmiUtil_Beep();
int File_UpdateRecordByIndex(char *OPLE, void* dadfadf, int num, int i);


int MmiUtil_CheckKey();
#endif
