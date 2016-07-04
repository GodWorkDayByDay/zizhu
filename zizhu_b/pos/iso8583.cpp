#include "define.h"
#include "cupms.h"

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_SetFieldFormat
 * DESCRIPTION:     Set ISO8583 each field type and format
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void ISOEngine_SetFieldFormat( ISO8583_BitMode bBitMode, const ISO8583_FieldFormat *pFieldFormat )
{

        return ;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_ClearAllBits
 * DESCRIPTION:     Clear the struct all bit flag
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void ISOEngine_ClearAllBits( ISO8583_Rec *cpIsoRec ){

    return ;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_ClearOneBit
 * DESCRIPTION:     Clear one field data
 * RETURN:          None
 ---------------------------------------------------------------------------- */
void ISOEngine_ClearOneBit( ISO8583_Rec *cpIsoRec, int iFieldNo )
{

    return ;
}

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
int ISOEngine_SetBit( ISO8583_Rec *pIsoRec, int iFieldNo, unsigned char *pFieldData, int iDataLength )
{

    return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_GetBit
 * DESCRIPTION:     Get ISO8583 field data
                    pRetFieldData must be the ASC format
 * RETURN:          Return FieldData length has gotten
 ---------------------------------------------------------------------------- */
int ISOEngine_GetBit( ISO8583_Rec *cpIsoRec, int iFieldNo, unsigned char *pRetFieldData, int iSizeofRetFieldData )

{

    return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_StringToIso
 * DESCRIPTION:     Convert string to ISO8583 format
 * RETURN:          if success, RETURN 0, else RETURN -1/-2
 ---------------------------------------------------------------------------- */
int ISOEngine_StringToIso( ISO8583_Rec *cpIsoRec, unsigned char *pString )
{

return 0;
}

/* -----------------------------------------------------------------------------
 * FUNCTION NAME:   ISOEngine_IsoToString
 * DESCRIPTION:     Convert ISO8583 format to string
 * RETURN:          if success, RETURN 0, else RETURN -1/-2
 ---------------------------------------------------------------------------- */
int ISOEngine_IsoToString( ISO8583_Rec *cpIsoRec, unsigned char *pRetString, int iSizeofRetString ){

    return 0;
}


int File_DeleteRecordByIndex(char *data, int siz, int i)
{
    return 0;
}

int File_Remove(char *data)
{
    return remove(data);
}



int TimeUtil_MillSec()
{
    return 0;
}
int MmiUtil_Beep()
{
    return 0;
}
int MmiUtil_ClearLine(int num)
{
    return 0;
}


int MmiUtil_CheckKey()
{return 0;}
