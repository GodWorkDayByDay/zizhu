/* 
 * ISO8583组包、解包等工具函数;
 * 2007-08-10
 */
#include "cupms.h"
#include "pos/Proc8583.h"
#include <QDebug>

struct ISO_8583 iso8583[128] = 
{
    {  8,  8,  0},  //1
    { 19,  4,  1},  //2
    {  6,  2,  0},  //3
    { 12,  2,  0},  //4
    { 12,  2,  0},  /* 5 field */
    { 12,  2,  0}, 
    { 10,  2,  0}, 
    {  8,  2,  0}, 
    {  8,  2,  0}, 
    {  8,  2,  0},  /* 10 field */
    {  6,  2,  0},
    {  6,  2,  0}, 
    {  4,  2,  0}, 
    {  4,  2,  0}, 
    {  4,  2,  0},  /* 15 field */
    {  4,  2,  0}, 
    {  4,  2,  0}, 
    {  4,  2,  0}, 
    {  3,  2,  0}, 
    {  6,  2,  0},  /* 20 field */
    {  6,  2,  0},
    {  3,  2,  0},
    {  4,  2,  0},
    {  20, 0,  0},
    {  2,  2,  0},  /* 25 field */
    {  2,  2,  0}, 
    {  1,  2,  0}, 
    {  8,  3,  0}, 
    {  8,  3,  0}, 
    {  8,  3,  0},  /* 30 field */
    {  8,  3,  0}, 
    { 11,  2,  1},
    { 11,  2,  1}, 
    { 28,  0,  1}, 
    { 37,  4,  1},  /* 35 field */
    {104,  4,  2},
    { 12,  0,  0}, 
    {  6,  0,  0}, 
    {  2,  0,  0}, 
    {  3,  0,  0},  /* 40 field */
    {  8,  0,  0}, 
    { 15,  0,  0}, 
    { 40,  0,  0}, 
    { 25,  0,  1},
    {256,  8,  2},  /* 45 field */
    {256,  8,  2}, 
    {256,  8,  2}, 
    { 999, 4,  2}, 
    {  3,  0,  0}, 
    {  3,  0,  0},  /* 50 field */
    {  7,  0,  0}, 
    {  8,  8,  0}, 
    { 16,  2,  0}, 
    {999,  2,  2},  
    {999,  0,  2},  /* 55 field */
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  0},
    {999,  4,  0},  /* 60 field */
    {999,  2,  2},
    {999,  8,  2},
    {999,  0,  2},
    { 64,  8,  0},  /* 64 field */
    {  8,  8,  0}, 
    {  1,  2,  0}, 
    {  2,  2,  0}, 
    {  3,  2,  0}, 
    {  3,  2,  0}, 
    {  3,  2,  0}, 
    {  4,  2,  0}, 
    {  4,  2,  0}, 
    {  6,  2,  0}, 
    { 10,  2,  0}, 
    { 10,  2,  0}, 
    { 10,  2,  0}, 
    { 10,  2,  0}, 
    { 10,  2,  0}, 
    { 10,  2,  0}, 
    { 10,  2,  0}, 
    { 10,  2,  0}, 
    { 12,  2,  0}, 
    { 12,  2,  0}, 
    { 12,  2,  0}, 
    { 12,  2,  0}, 
    { 16,  2,  0}, 
    { 16,  2,  0}, 
    { 16,  2,  0}, 
    { 16,  2,  0}, 
    { 42,  2,  0}, 
    {  1,  0,  0}, 
    {  2,  0,  0}, 
    {  5,  0,  0}, 
    {  7,  0,  0}, 
    { 42,  0,  0}, 
    {  8,  8,  0}, 
    { 16,  3,  0}, 
    { 25,  0,  0}, 
    { 11,  2,  1}, 
    { 11,  2,  1}, 
    { 17,  0,  1}, 
    { 28,  0,  1}, 
    { 28,  0,  1}, 
    {100,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {999,  0,  2}, 
    {  8,  8,  0}  /* 128 field */
};

struct len_str 
{
	short  len;
	char   *str;
};

struct element_struc 
{
	short bitno;           /* element no */
	short type;            /* 0--default value, 1,2--process function */
	short len;             /* when type=0, len=length of defaule value */
	void  *pointer;        /* pointer of value or process function */
};

struct  trans_bitmap_struc 
{
	short trans_type;                /* transaction type */
	char  message_id[10];
	short element_number;            /* number of elememts */
	struct element_struc *element;   /* transaction element ally */

};

ISO_data    Iso, Iso1;
extern struct ISO_8583 iso8583[128];

/*
 * 函数：convert
 * 功能：转化十六字符为十进制数字
 * 入口：ch:     需要转换的字符
 * 出口： 转换后的十进制数
 * 返回：-1 失败
 */
 
int convert(char   ch)
{
	
	if((ch>='0')&&(ch<='9'))   
   return   ch-0x30;
  else   if((ch>='A')&&(ch<='F'))   
   return   ch-'A'+10;
  else   if((ch>='a')&&(ch<='f'))   
   return   ch-'a'+10;
  else
	 return  (-1);   

}
/*
 * 函数：SetBit
 * 功能：打包ISO8583结构指定的域，同时设置Bitmap相应位;
 * 入口：n:     要打包的域；
 *       str:   要打包的数据；
 *       len:   要打包数据的长度；
 * 出口：iso:   打包之后的ISO8583结构指针；
 * 返回：-1 失败；0 成功；
 */
int SetBit(ISO_data *iso, int n, UCHAR *str, int len)
{
    int i, l;
    UCHAR *pt, tmp[1024];

    if (len == 0)
        return(0);

    if (n == 0)
    {
        memcpy(iso->message_id, str, 4);
        iso->message_id[4] = '\0';
        return(0);
    }
    
    if (n <= 1 || n > 128)
        return(-1);

    if (n == 64)
        n = 128;
        
    n--;
    if (len > iso8583[n].len)
        len = iso8583[n].len;
        
    iso->f[n].bitf = 1;
    l = len;
    
    if (iso8583[n].flag == 0)
        len = iso8583[n].len;
        
    iso->f[n].len = len;
    pt = (UCHAR *)&(iso->dbuf[iso->off]);
    iso->f[n].dbuf_addr = iso->off;

    if (len + iso->off >= 1000)
        return(-1);

    iso->off += len;
    if (iso8583[n].type & 0x01)
    {                   /* x */
        *(pt ++)= *(str ++);
       (iso->off)++;
    }
    
    i = 0;
    if (iso8583[n].type & 0x03) 
    {                   /* n */ 
        for (; i < len- l; i ++)
            tmp[i] = '0';
    }
    
    memcpy(tmp + i, str, l);
    i += l;
    if (iso8583[n].type & 0x08)
    {                    /* b */
        for (; i < len; i ++)
            tmp[i] = 0;
    }
    else
    {
        for (; i < len; i ++)
            tmp[i] = ' ';         /* z or others */
    }

    if (iso8583[n].type & 0x07)
        AscToBcd(pt, tmp, len, iso8583[n].type & 0x03);
    else
        memcpy(pt, tmp, len);
        
    return 0;
}

/*
 * 函数：GetBit
 * 功能：解包ISO8583结构的指定域数据;
 * 入口：iso:   ISO8583数据的指针；
 *       n:     要解包的域； 
 * 出口：str:   该域解出来的数据；
 * 返回：解包出来数据的长度；
 */
int GetBit(ISO_data *iso, int n, UCHAR *str)
{
    int j;
    UCHAR *pt;

//    if (n == 0)
//    {
//        memcpy(str, iso->message_id, 4);
//        str[4] = '\0';
//        return(4);
//    }
//    
//    if (n <= 1 || n > 128)
//        return(0);
        
    n--;
    if (iso->f[n].bitf == 0)
    {
        if (n == 127)
        {
            if (iso->f[63].bitf == 0)
                return 0;
        }
                        
        str[0] = 0;
        return(0);
    }
    
    pt = (UCHAR *)&iso->dbuf[iso->f[n].dbuf_addr];
    j = iso->f[n].len;
    if (iso8583[n].type & 0x01)
        * str ++ = * pt ++;
        
    if (iso8583[n].type & 0x07)
        BcdToAsc(str, pt, j, iso8583[n].type & 0x03);
    else
        memcpy(str, pt, j);
        
    str[j] = '\0';

    return(j);
}

/*
 * 函数：StrToIso
 * 功能：将字符串转存为ISO8583结构数据;
 * 入口：dstr-需要转换的字符串数据；
 * 出口：iso-转换之后的ISO8583结构数据；
 * 返回：-1 失败；0 成功；
 */
int StrToIso_bak(UCHAR *dstr, ISO_data *iso)
{
    unsigned char   *rpt, bitmask;
    unsigned char   i, j, n, bitnum;
    int             off, len, k;
    int message_id_len;

    /* Initialize the iso->f[].bitf */
    for (i = 0; i < 128; i ++)
        iso->f[i].bitf = 0;

    off = 0;

    /* Get Message ID */
    BcdToAsc((UCHAR *)iso->message_id, dstr, 4, 0);
    message_id_len = 2;
    iso->message_id[4] = '\0';

    /* Get Bitmap bytes */
    if (dstr[message_id_len] & 0x80)
        bitnum = 16;
    else
        bitnum = 8;

    /* initialize <rpt> point to data elements */
    rpt = dstr + message_id_len + bitnum;
    /* copy dstr elements to iso */
    for (i = 0; i < bitnum; i ++)
    {
        bitmask = 0x80;
        for (j = 0; j < 8; j ++, bitmask >>= 1)
        {
            if (i == 0 && bitmask == 0x80)
                continue;

            if ((dstr[i + message_id_len] & bitmask)== 0)
                continue;

            n =(i << 3)+ j;
            qDebug("n:%d", n);
            /* process variable length data elements */
            if (iso8583[n].flag > 0)
            {
                qDebug("rpt:%02x", *rpt);
                len =(* rpt)-((* rpt)>> 4)* 6;
                rpt ++;
                qDebug("flag:%dlen:%d", iso8583[n].flag, len);
                if (iso8583[n].flag == 2)
                {
                    len = len * 100 +(* rpt)-(* rpt >> 4)* 6;
                    rpt ++;
                }

                if (len > iso8583[n].len)
                {
                    qDebug("1 proc8583 item [%d]len[%d]iso8583[n].len[%d]\n", n, len, iso8583[n].len);
                    return(-1);
                }
            }
            else
            {
                len = iso8583[n].len;

            }

            iso->f[n].len = len;
            iso->f[n].dbuf_addr = off;

            /* Credit or Debit Char no include in the len */
            if (iso8583[n].type & 0x01)
                iso->dbuf[off ++] = * rpt ++;

            /* BCD field Bytes=len/2 */
            if (iso8583[n].type & 0x07)
            {
                len ++;
                len >>= 1;
            }

            /* Copy to iso buffer */
            if (len + off >= 1000)
            {
                qDebug("2 proc8583 item [%d]\n", n);
                return(-1);
            }

            for (k = 0; k < len; k ++)
            {
                iso->dbuf[off ++] = * rpt ++;
            }

            iso->f[n].bitf = 1;
        }
    }

    iso->off = off;
    return(0);
}


int StrToIso(unsigned char *dstr,ISO_data *iso)
{
    unsigned char   *rpt,bitmask;
    unsigned char   i,j,n,bitnum;
    int             off_,len,k;
      int message_id_len=0;
//	  char buff[2];
//	  char buff2[4];

    /*Initialize the iso->f[].bitf */
    for(i=0;i<128;i++)
        iso->f[i].bitf=0;

    off_=0;

    /*Get Message ID */
    BcdToAsc((UCHAR *)iso->message_id,dstr,4,0);
    message_id_len=2;
    iso->message_id[4]='\0';

    /*Get Bitmap bytes */
    if(dstr[message_id_len]&0x80)
        bitnum=16;
    else
        bitnum=8;

    /*initialize <rpt> point to data elements */
    rpt=dstr+message_id_len+bitnum;

    /*copy dstr elements to iso */
    for(i=0;i<bitnum;i++)
    {
        bitmask=0x80;
        for(j=0;j<8;j++,bitmask>>=1)
        {
            //if(i==0&&bitmask==0x80)
             //  continue;

            if((dstr[i+message_id_len]&bitmask)==0)
                continue;

            n=(i<<3)+j;

            /*process variable length data elements */
            if(iso8583[n].flag>0)
            {

                len=(*rpt)-((*rpt)>>4)*6;
                rpt++;

                if(iso8583[n].flag==2)
                {

                    len=len*100+(*rpt)-(*rpt>>4)*6;
                    rpt++;
                }

               //buff[0]=*rpt++;
               //buff[1]=*rpt++;

        //				   BcdToAsc((unsigned char *)buff2,(unsigned char *)buff,4,0);


        //				   len=(convert(buff2[1]))*16*16+convert(buff2[2])*16+convert(buff2[3]);



                //printf("variable length :  %d\n" , len);

                if(len>iso8583[n].len)
                {
                    qDebug("proc8583 item [%d]\n",n);
                    return(-1);
                }
            }
            else
                len=iso8583[n].len;

            iso->f[n].len=len;
            iso->f[n].dbuf_addr=off_;

            /*Credit or Debit Char no include in the len */
            if(iso8583[n].type&0x01)
                iso->dbuf[off_++]=*rpt++;

            /*BCD field Bytes=len/2 */
            if(iso8583[n].type&0x07)
            {
                len++;
                len>>=1;
            }

            /*Copy to iso buffer */
            if(len+off_>=1000)
            {
                //printf("proc8583 item [%d]\n",n);
                return(-1);
            }

            for(k=0;k<len;k++)
                iso->dbuf[off_++]=*rpt++;

            iso->f[n].bitf=1;
        }
    }

    iso->off=off_;
    return(0);
}

/*
 * 函数：IsoToStr
 * 功能：将ISO8583结构数据转换为字符串数据;
 * 入口：iso-转换之后的ISO8583结构数据；
 * 出口：dstr-需要转换的字符串数据；
 * 返回：转换后字符串数据的长度；
 */
int IsoToStr(UCHAR *dstr, ISO_data *iso)
{
    UCHAR *wpt, bitmask, bitmap;
    int k, lc;
    UCHAR i, j, n, bitnum;
    int message_id_len;

    AscToBcd(dstr, (UCHAR *)iso->message_id, 4, 0);
    message_id_len = 2;

    for (lc = 64, bitnum = 8; lc < 127; lc ++) /*扩展bitmap*/
    {
        if (iso->f[lc].bitf)
        {
            bitnum = 16;
            break;
        }
    }

    if(iso->f[0].bitf)
        bitnum = 16;

    //qDebug("*******************bitnum[%d]*********************************", bitnum);
    wpt = dstr + message_id_len + bitnum;    /* wpt是指向bitmap后具体内容的指针 */
    //qDebug("1bufferlen[%d]", wpt - dstr);
    /* copy iso elements to dstr */
    for (i = 0; i < bitnum; i ++)
    {
        bitmap = 0;
        bitmask = 0x80; /*变量bitmap表示8个位图域*/

        for (j = 0; j < 8; j ++, bitmask >>= 1)
        {
            n =(i << 3)+ j;
            if (iso->f[n].bitf == 0)
                continue;

            bitmap |= bitmask;

            lc = iso->f[n].len;
            //qDebug("len[%d]n[%d]", lc, n);
            //qDebug("bufferlen[%d]", wpt - dstr);
            if (iso8583[n].flag == 1)
                (* wpt ++)=(unsigned char)((lc % 10)+(lc / 10)* 16);
            else if (iso8583[n].flag == 2)
            {
                (* wpt ++)=(unsigned char)(lc / 100);
                (* wpt ++)=(unsigned char)(((lc % 100)/ 10)* 16 +(lc % 100)%10);
            }       /* 存放VARII,VARIII型长度 */

            k = 0;
            if (iso8583[n].type & 0x01)
            {                 /* x ASCII型数据 */
               (* wpt ++)= iso->dbuf[iso->f[n].dbuf_addr + k];    /* C or D */
                k ++;
                lc ++;
            }
            if (iso8583[n].type & 0x07)
            {                /* n or z BCD型数据*/
                lc ++;
                lc >>= 1;           /* lc =(lc+1)/2 */
            }

            for (; k < lc; k ++)
            {
               (* wpt ++)= iso->dbuf[iso->f[n].dbuf_addr + k];    /* C or D */
            }
        }

        dstr[i + message_id_len] = bitmap;
    }
    //qDebug("2bufferlen[%d]", wpt - dstr);
    //if (iso->f[127].bitf && bitnum == 8)
    if (iso->f[127].bitf && bitnum == 16)
    {   /* 存放 MAC */
        qDebug("**********************not print is ok****************");
        dstr[bitnum + 1] |= 0x01;
        memcpy(wpt, &iso->dbuf[iso->f[127].dbuf_addr], 8);
        wpt += 8;
    }
    //qDebug("3bufferlen[%d]", wpt - dstr);
    if (bitnum == 16)
    {
        dstr[message_id_len] |= 0x80;        /* 置MAC的bitmap */
    }
    //qDebug("4bufferlen[%d]", wpt - dstr);
    return (wpt - dstr);
}

/*
 * 函数：ClearBit
 * 功能：清空ISO8583结构全部域的数据，同时清空Bitmap相应位;
 * 入口：iso:   ISO8583结构的指针； 
 * 返回：无；
 */
void ClearBit(ISO_data *iso)
{
    int     i;

    for (i = 0; i < 128; i ++)
        iso->f[i].bitf = 0;
        
    iso->off = 0;

    return;
}

/*
 * 函数：ClearBit1
 * 功能：清空ISO8583结构Bitmap指定位;
 * 入口：iso:   ISO8583结构的指针； 
 *       set:   指定域；
 * 返回：无；
 */
void ClearBit1(ISO_data *iso, int set)
{
    iso->f[set- 1].bitf = 0;

    return;
}

/*
 * 函数：AscToBcd
 * 功能：将ASC数据转换为BCD数据;
 * 出口：bcd_buf:   BCD数据缓存；
 * 入口：ascii_buf: ASC数据串；
 *       conv_len:  ASC数据串的长度；
 *       type:      转换模式；
 * 返回：无；
 */
void AscToBcd(UCHAR *bcd_buf, UCHAR *ascii_buf, int conv_len, UCHAR type)
{
    int cnt;
    char ch, ch1;

    if (conv_len & 0x01 && type)
        ch1 = 0;
    else
        ch1 = 0x55;

    for (cnt = 0; cnt < conv_len; ascii_buf ++, cnt ++)
    {
        if (*ascii_buf >= 'a')
            ch = *ascii_buf- 'a' + 10;
        else if (*ascii_buf >= 'A')
            ch = *ascii_buf- 'A' + 10;
        else if (*ascii_buf >= '0')
            ch = *ascii_buf-'0';
        else
            ch = 0;
            
        if (ch1 == 0x55)
            ch1 = ch;
        else 
        {
            *bcd_buf ++ =(ch1 << 4)| ch;
            ch1 = 0x55;
        }
    }
    
    if (ch1 != 0x55)
        *bcd_buf = ch1 << 4;

    return;
}

/*
 * 函数：BcdToAsc
 * 功能：将BCD数据转换为ASC数据;
 * 入口：bcd_buf:   BCD数据缓存；
 *       conv_len:  ASC数据串的长度；
 *       type:      转换模式；
 * 出口：ascii_buf: ASC数据串；
 * 返回：无；
 */
void BcdToAsc(UCHAR *ascii_buf, UCHAR *bcd_buf, int conv_len, UCHAR type)
{
    int cnt;

    if (conv_len & 0x01 && type)
    {
        cnt = 1;
        conv_len ++;
    }
    else
        cnt = 0;
        
    for (; cnt < conv_len; cnt ++, ascii_buf ++)
    {
        *ascii_buf =((cnt & 0x01)?(*bcd_buf ++ & 0x0f):(*bcd_buf >> 4));
        *ascii_buf +=((*ascii_buf > 9)?('A'- 10): '0');
    }

    return;
}

void EnableSecBitmap(ISO_data *iso)
{
    iso->f[0].bitf = 1;
}

int SetIso(int bit, int len, unsigned char type, unsigned char flag)
{
	if(bit < 0 || bit > 127)
		return -1;
	if(len < 0 || len > 999)
		return -1;
	if(flag != 0 && flag != 1 && flag != 2)
		return -1;

	iso8583[bit-1].len = len;
	iso8583[bit-1].type = type;
	iso8583[bit-1].flag = flag;

	return 0;
}



//初始化报文结构
//int SetIso(int bit, int len, unsigned char type, unsigned char flag);
void SetIsoStruct()
{
	//BIT图
	SetIso(0,4,1,0);	
    SetIso(1,8,8,0);
    SetIso(2,19,4,1);
    SetIso(3,6,2,0);
    SetIso(4,12,2,0);
    SetIso(5,20,2,0);
    SetIso(6,37,2,0);
    SetIso(7,10,2,0);
    SetIso(8,6,2,0);
    SetIso(9,12,2,0);
    SetIso(10,12,2,0);
    SetIso(11,6,2,0);
    SetIso(12,6,2,0);
    SetIso(13,4,2,0);
    SetIso(14,4,2,0);
    SetIso(15,4,2,0);
    SetIso(16,6,2,0);
    SetIso(17,6,2,0);
    SetIso(18,12,2,0);
    SetIso(19,6,2,0);
    SetIso(20,6,2,0);
    SetIso(21,12,2,0);
    SetIso(22,3,2,0);
    SetIso(23,4,2,0);
	SetIso(24,6,0,0);
    SetIso(25,2,2,0);
    SetIso(26,2,2,0);
	SetIso(27,20,0,0);
	SetIso(28,12,0,0);
	SetIso(29,8,0,0);
	SetIso(30,20,0,0);
	SetIso(31,20,0,0);
    SetIso(32,11,2,1);
	SetIso(33,6,0,0);
    SetIso(34,25,0,0);
    SetIso(35,38,4,1);
    SetIso(36,104,4,2);
    SetIso(37,12,0,0);
    SetIso(38,6,0,0);
    SetIso(39,2,0,0);
	SetIso(40,12,0,0);
    SetIso(41,8,0,0);
    SetIso(42,15,0,0);
	SetIso(43,12,0,0);
    SetIso(44,25,0,1);
    SetIso(45,1,0,0);
    SetIso(46,20,0,0);
    SetIso(47,20,0,0);
    SetIso(48,32,0,0);
    SetIso(49,3,0,0);
    SetIso(50,12,0,0);
    SetIso(51, 1,0,0);
    SetIso(52, 8,8,0);
    SetIso(53,16,2,0);
    SetIso(54,20,0,0);
    SetIso(55,255,0,0);
    SetIso(58,999,0,2);
    SetIso(59,999,0,0);
    SetIso(60,999,4,2);
    SetIso(61,29,2,2);
    SetIso(62,72,8,2);
    SetIso(63,63,0,2);
	//MAC
	SetIso(64,8,8,0);
}




