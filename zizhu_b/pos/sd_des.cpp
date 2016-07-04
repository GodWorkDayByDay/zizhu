/******************************************************************************
 * des.c
 *
 *  Created on: 2013-3-24
 *      Author: LiuZe
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sd_des.h"
#include "apputils.h"

extern int print_qdebug(unsigned char * Title,unsigned char * buff, int len);
static const unsigned char pc_1_c[28] = { 57, 49, 41, 33, 25, 17, 9, 1, 58, 50,
        42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36 };

static const unsigned char pc_1_d[28] = { 63, 55, 47, 39, 31, 23, 15, 7, 62, 54,
        46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4 };
static const unsigned char pc_2[48] = { 14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21,
        10, 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55,
        30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 };

static const unsigned char ls_count[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2,
        2, 2, 2, 1 };

static const unsigned char ip_tab[64] = { 58, 50, 42, 34, 26, 18, 10, 2, 60, 52,
        44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40,
        32, 24, 16, 8, 57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11,
        3, 61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7 };

static const unsigned char _ip_tab[64] = { 40, 8, 48, 16, 56, 24, 64, 32, 39, 7,
        47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13,
        53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19,
        59, 27, 34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25 };

static const unsigned char e_r[48] = { 32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8,
        9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20,
        21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1 };

static const unsigned char P[32] = { 16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23,
        26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4,
        25 };
static const unsigned char SSS[8][4][16] = { { { 14, 4, 13, 1, 2, 15, 11, 8, 3,
        10, 6, 12, 5, 9, 0, 7 }, { 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9,
        5, 3, 8 }, //* err on
        { 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 }, { 15, 12, 8,
                2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 } },

{ { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 }, { 3, 13, 4, 7, 15,
        2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 }, { 0, 14, 7, 11, 10, 4, 13, 1, 5,
        8, 12, 6, 9, 3, 2, 15 }, { 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0,
        5, 14, 9 } },

{ { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 }, { 13, 7, 0, 9, 3, 4,
        6, 10, 2, 8, 5, 14, 12, 11, 15, 1 }, { 13, 6, 4, 9, 8, 15, 3, 0, 11, 1,
        2, 12, 5, 10, 14, 7 }, { 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5,
        2, 12 } },

{ { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 }, { 13, 8, 11, 5, 6,
        15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 }, { 10, 6, 9, 0, 12, 11, 7, 13, 15,
        1, 3, 14, 5, 2, 8, 4 }, { 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7,
        2, 14 } }, // err on

        { { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 }, { 14, 11, 2,
                12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 }, //* err on
                { 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 }, { 11,
                        8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 } },

        { { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 }, { 10, 15, 4,
                2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 }, { 9, 14, 15, 5, 2,
                8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 }, { 4, 3, 2, 12, 9, 5, 15,
                10, 11, 14, 1, 7, 6, 0, 8, 13 } },

        { { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 }, { 13, 0, 11,
                7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 }, { 1, 4, 11, 13, 12,
                3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 }, { 6, 11, 13, 8, 1, 4, 10,
                7, 9, 5, 0, 15, 14, 2, 3, 12 } },

        { { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 }, { 1, 15, 13,
                8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 }, { 7, 11, 4, 1, 9,
                12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 }, { 2, 1, 14, 7, 4, 10, 8,
                13, 15, 12, 9, 0, 3, 5, 6, 11 } } };

/**************************************************************************
 *                       Tools
 **************************************************************************/
//extern unsigned char C[17][28],D[17][28],K[17][48];
static unsigned char C[17][28], D[17][28], K[17][48];

static void s_box(char *aa, char *bb) {
    unsigned char i, j, k, m;
    unsigned char y, z;
    char ss[8];
    m = 0;
    for (i = 0; i < 8; i++) {
        j = 6 * i;
        y = aa[j] * 2 + aa[j + 5];
        z = aa[j + 1] * 8 + aa[j + 2] * 4 + aa[j + 3] * 2 + aa[j + 4];
        ss[i] = SSS[i][y][z];
        y = 0x08;
        for (k = 0; k < 4; k++) {
            bb[m++] = (ss[i] / y) & 1;
            y /= 2;
        }

    }
}

static void LS(unsigned char *bits, unsigned char *buffer, unsigned char count) {
    unsigned char i;
    for (i = 0; i < 28; i++) {
        buffer[i] = bits[(i + count) % 28];
    }
}

static void F(unsigned char n, char *ll, char *rr, char *LL, char *RR) {
    unsigned char i;
    char buffer[64], tmp[64];
    for (i = 0; i < 48; i++)
        buffer[i] = rr[e_r[i] - 1];
    for (i = 0; i < 48; i++)
        buffer[i] = (buffer[i] + K[n][i]) & 1;

    s_box(buffer, tmp);

    for (i = 0; i < 32; i++)
        buffer[i] = tmp[P[i] - 1];

    for (i = 0; i < 32; i++)
        RR[i] = (buffer[i] + ll[i]) & 1;

    for (i = 0; i < 32; i++)
        LL[i] = rr[i];

}

static void ip_1(LPBYTE text,char *ll,char *rr)
{
    unsigned char i;
    char tmp[64];
    for (i=0;i<32;i++)
    tmp[i]=ll[i];
    for (i=32;i<64;i++)
    tmp[i]=rr[i-32];
    for (i=0;i<64;i++)
    text[i]=tmp[_ip_tab[i]-1];
}

static void expand0(LPBYTE in,char *out)
{
    unsigned char divide;
    unsigned char i,j;

    for (i=0;i<8;i++)
    {
        divide=0x80;
        for (j=0;j<8;j++)
        {
            *out++=(in[i]/divide)&1;
            divide/=2;
        }
    }
}

static void son(unsigned char *cc, unsigned char *dd, unsigned char *kk) {
    unsigned char i;
    char buffer[56];
    for (i = 0; i < 28; i++)
        buffer[i] = *cc++;

    for (i = 28; i < 56; i++)
        buffer[i] = *dd++;

    for (i = 0; i < 48; i++)
        *kk++ = buffer[pc_2[i] - 1];
}

static void compress0(char *out, LPBYTE in)
{
    unsigned char times;
    unsigned char i,j;

    for (i=0;i<8;i++)
    {
        times=0x80;
        in[i]=0;
        for (j=0;j<8;j++)
        {
            in[i]+=(*out++)*times;
            times/=2;
        }
    }
}
/*static void compress016(char *out,LPBYTE in)
 {
 unsigned char times;
 unsigned char i,j;

 for (i=0;i<16;i++)
 {
 times=0x8;
 in[i]='0';
 for (j=0;j<4;j++)
 {
 in[i]+=(*out++)*times;
 times/=2;
 }
 }
 }
 */

static void setkeystar(unsigned char bits[64]) {
    unsigned char i, j;

    for (i = 0; i < 28; i++)
        C[0][i] = bits[pc_1_c[i] - 1];
    for (i = 0; i < 28; i++)
        D[0][i] = bits[pc_1_d[i] - 1];
    for (j = 0; j < 16; j++) {
        LS(C[j], C[j + 1], ls_count[j]);
        LS(D[j], D[j + 1], ls_count[j]);
        son(C[j + 1], D[j + 1], K[j + 1]);
    }
}

static void ip(LPBYTE text,char *ll,char *rr)
{
    unsigned char i;
    char buffer[64];
    //char tmp[64];
        expand0(text,buffer);

        for (i=0;i<32;i++)
        ll[i]=buffer[ip_tab[i]-1];

        for (i=0;i<32;i++)
        rr[i]=buffer[ip_tab[i+32]-1];
    }

static void encrypt0(LPBYTE text,LPBYTE mtext)
{
    char ll[64],rr[64],LL[64],RR[64];
    char tmp[64];
    unsigned char i,j;
    ip(text,ll,rr);

    for (i=1;i<17;i++)
    {
        F(i,ll,rr,LL,RR);
        for (j=0;j<32;j++)
        {
            ll[j]=LL[j];
            rr[j]=RR[j];
        }
    }

    ip_1((unsigned char*)tmp,rr,ll);

    compress0(tmp,mtext);
}

static void discrypt0(LPBYTE mtext,LPBYTE text)
{
    char ll[64],rr[64],LL[64],RR[64];
    char tmp[64];
    unsigned char i,j;
    ip(mtext,ll,rr);

    for (i=16;i>0;i--)
    {
        F(i,ll,rr,LL,RR);
        for (j=0;j<32;j++)
        {
            ll[j]=LL[j];
            rr[j]=RR[j];
        }
    }

    ip_1((unsigned char*)tmp,rr,ll);

    compress0(tmp,text);
}

/*******************************************************************************
 * Date: :
 * Author:
 * Function: 加密DES
 ******************************************************************************/
void DES(BYTE *key, BYTE *text, BYTE *mtext) {
    char tmp[64];
    expand0(key, tmp);
    setkeystar((unsigned char*) tmp);
    encrypt0(text, mtext);
}

/*******************************************************************************
 * Date: :
 * Author:
 * Function: 解密单DES
 ******************************************************************************/
void DES_1(BYTE *key, BYTE *text, BYTE *mtext) {
    char tmp[64];
    expand0(key, tmp);
    setkeystar((unsigned char*) tmp);
    discrypt0(text, mtext);
}

/*******************************************************************************
 * Date: :
 * Author:
 * Function: 加密3DES
 ******************************************************************************/
void DES_3(BYTE *key16, BYTE *text, BYTE *mtext) {
    BYTE i;
    BYTE keyL[8];
    BYTE keyR[8];
    BYTE dump1[8];
    BYTE dump2[8];
    for (i = 0; i < 8; i++) {
        dump1[i] = 0x00;
        dump2[i] = 0x00;
    }
    for (i = 0; i < 8; i++) {
        keyL[i] = key16[i];
        keyR[i] = key16[i + 8];
    }
    DES(keyL, text, dump1);
    DES_1(keyR, dump1, dump2);
    DES(keyL, dump2, mtext);
}

/*******************************************************************************
 * Date: :
 * Author:
 * Function: 解密3DES
 ******************************************************************************/
void DES_31(BYTE *key16, BYTE *text, BYTE *mtext)
{
    BYTE i;
    BYTE keyL[8];
    BYTE keyR[8];
    BYTE dump1[8];
    BYTE dump2[8];
    for (i = 0; i < 8; i++) {
        dump1[i] = 0x00;
        dump2[i] = 0x00;
    }
    for (i = 0; i < 8; i++) {
        keyL[i] = key16[i];
        keyR[i] = key16[i + 8];
    }
    DES_1(keyL, text, dump1);
    DES(keyR, dump1, dump2);
    DES_1(keyL, dump2, mtext);
}

/*******************************************************************************
 * Date: :
 * Author:
 * Function: 计算DesMac
 ******************************************************************************/
void Des1_X99CBCMac(unsigned char *key, const unsigned char *text, int textsize,
             unsigned char *rtn_mac)
{
    unsigned char tmp[1024] = { 0 };
    memcpy(tmp, text, textsize);
    int st = textsize % 8 == 0 ? textsize / 8 : textsize / 8 + 1;
    unsigned char d0[8] = { 0 };
    unsigned char d1[8] = { 0 };
    unsigned char dd[8] = { 0 };
    int i = 0;
    int ii = 0;

    memcpy(d0, tmp + i * 8, 8);
    for (i = 0; i < st; i++) {
        DES(key, d0, dd);
        if (i + 1 < st) {
            memcpy(d1, tmp + (i + 1) * 8, 8);
            for (ii = 0; ii < 8; ii++) {
                d0[ii] = dd[ii] ^ d1[ii];
            }
        }
    }
    
    memcpy( rtn_mac, dd, 8 );
}

/*******************************************************************************
 * Date: :
 * Author:
 * Function: 计算Des3Mac
 ******************************************************************************/
void Des3_X99CBCMac(unsigned char *key, const unsigned char *text, int textsize,
             unsigned char *rtn_mac)
{
    unsigned char tmp[1024] = { 0 };
    memcpy(tmp, text, textsize);
    int st = textsize % 8 == 0 ? textsize / 8 : textsize / 8 + 1;
    unsigned char d0[17] = { 0 };
    unsigned char d1[17] = { 0 };
    unsigned char dd[17] = { 0 };
    int i = 0;
    int ii = 0;
    memcpy(d0, tmp + i * 8, 8);
    for (i = 0; i < st; i++) {

        DES_3(key, d0, dd);

        DES_3(key, d0+8, dd+8);

        if (i + 1 < st) {
            memcpy(d1, tmp + (i + 1) * 8, 8);

            for (ii = 0; ii < 8; ii++) {
                d0[ii] = dd[ii] ^ d1[ii];
            }
        }
    }
    
    memcpy( rtn_mac, dd, 8 );
}

BYTE a_to_b(BYTE bchar)
{
    bchar &= 0xFF;
    if ((bchar >='0')&&(bchar <='9'))
        return(bchar-'0');
    if ((bchar >='A')&&(bchar <='F'))
        return(bchar-'A'+10);
    if ((bchar >='a')&&(bchar <='f'))
        return(bchar-'a'+10);
    else
        return(0xf);
}

int BankUtil_Bcd2Asc(BYTE *BcdBuf, BYTE *AscBuf, unsigned short AscLen )
{
    int i, j;
    unsigned char bchar;
    int BcdLen = AscLen/2;

    for(i=0,j=0; i<BcdLen; i++)
    {
        bchar = ((BcdBuf[i]>>4)&0xf);
        AscBuf[j++] = (bchar<=9)?('0'+bchar):(bchar-10+'A');
        bchar = BcdBuf[i]&0x0f;
        AscBuf[j++] = (bchar<=9)?('0'+bchar):(bchar-10+'A');
    }
    return 0;
}

int BankUtil_Asc2Bcd(BYTE *AscBuf, BYTE *BcdBuf, unsigned short AscLen)
{
    int i,j;
    BYTE bchar;

    for (i=0,j=0; j<AscLen; i++)
    {
        bchar = a_to_b(AscBuf[j++]);
        if(j == AscLen)
            BcdBuf[i] = ((bchar<<4)&0xf0) + 0x0F;
        else
            BcdBuf[i] = ((bchar<<4)&0xf0) + a_to_b(AscBuf[j++]);

    }
    return 0;
}


/*******************************************************************************
 * Date: :
 * Author:
 * Function: 计算DesMac
 ******************************************************************************/
void Des1_ECBMac(unsigned char *key, const unsigned char *text, int textsize,
             unsigned char *rtn_mac)
{
    unsigned char tmp[1024] = { 0 };
    memcpy(tmp, text, textsize);
    int st = textsize % 8 == 0 ? textsize / 8 : textsize / 8 + 1;
    unsigned char d0[8] = { 0 };
    unsigned char d1[8] = { 0 };
    unsigned char dd[8] = { 0 };
    
    unsigned char hexm1[17] = { 0 };
    unsigned char hexm2[17] = { 0 };
    unsigned char tmp1[9] = { 0 };
    unsigned char tmp2[9] = { 0 };
    
    unsigned char tmp3[9] = { 0 };
    unsigned char tmp4[9] = { 0 };
    int i = 0;
    int ii = 0;

    memcpy( d0, tmp + i * 8, 8 );
    //b
    for (i = 0; i < st; i++)                
    {
        //DES( key, d0, dd );
        memcpy( dd, d0, 8 );
        if ( i + 1 < st )
        {
            memcpy( d1, tmp + (i + 1) * 8, 8 );
            for (ii = 0; ii < 8; ii++)
            {
                d0[ii] = dd[ii] ^ d1[ii];
            }
        }
    }

    //c
   // print_qdebug((unsigned char *)"^d0", d0, 16);
   BankUtil_Bcd2Asc( d0, hexm1, 16 );
   //print_qdebug((unsigned char *)"^res", hexm1, 16);
   //("");
   //d
   memcpy( tmp1, hexm1, 8 );
   memcpy( tmp2, hexm1 + 8, 8 );

   DES( key, tmp1, tmp3 );

   //e
   for( i= 0; i < 8; i++ ){
       tmp4[i] = tmp3[i] ^ tmp2[i];
    }

   //f
   memset( tmp1, 0x00, sizeof( tmp1 ) );
   DES( key, tmp4, tmp1 );

   //g
   BankUtil_Bcd2Asc( tmp1, hexm2, 16 );

   memcpy( rtn_mac, hexm2, 8 );  
}

