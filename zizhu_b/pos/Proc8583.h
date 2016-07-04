#ifndef __PROC_8583_
#define __PROC_8583_
#include "cupms.h"

typedef  unsigned char UCHAR;
void SetIsoStruct();

/* ISO 8583 Message Structure Definitions */
struct  ISO_8583 
{
	int    len;             /* data element max length */
	unsigned char    type;  /* bit 0--x, bit 1--n左对齐, bit 2--z bit 3--b*/
	unsigned char    flag;  /* length field length: 1--LLVAR型 2--LLLVAR型*/
};

struct data_element_flag 
{
	short bitf;
	short len;
	int   dbuf_addr;
};

typedef struct  
{
	struct  data_element_flag f[128];
	short   off;
	char    dbuf[1024];
	char    message_id[10];
} ISO_data;

extern ISO_data    Iso, Iso1;
extern struct ISO_8583 iso8583[128];


/*
 * function	:	将普通字符串转换成ISO8583格式的结构体
 * param1		:	普通字符串缓冲区
 * param2		:	ISO8583结构的指针
 * return		:	0		- 成功
 *				其它	- 失败
 */
int StrToIso(UCHAR *, ISO_data *);

/*
 * function	:	将ISO8583格式的结构体转换成普通字符串
 * param1		:	普通字符串缓冲区
 * param2		:	ISO8583结构的指针
 * return		:	0		- 成功
 *				其它	- 失败
 */
int IsoToStr(UCHAR *, ISO_data *);

/*
 * function	:	解包ISO8583结构的指定域数据
 * param1		:	ISO8583结构的指针
 * param2		:	要解包的域
 * param3		:	该域解出来的数据
 * return		:	解包出来数据的长度
 */
int GetBit(ISO_data *, int, UCHAR *);

/*
 * function	:	打包ISO8583结构指定的域，同时设置Bitmap相应位
 * param1		:	ISO8583结构的指针
 * param2		:	要解包的域
 * param3		:	该域解出来的数据
 * param4		:	要打包数据的长度
 * return		:	解包出来数据的长度
 */
int SetBit(ISO_data *, int, UCHAR *, int);

/*
 * function	:	清空ISO8583结构全部域的数据，同时清空Bitmap相应位
 * param1		:	ISO8583数据的指针
 * return		:	无
 */
void ClearBit(ISO_data *);

/*
 * function	:	清空ISO8583结构Bitmap指定位
 * param1		:	ISO8583结构的指针
 * param2		:	指定域
 * return		:	无
 */
void ClearBit1(ISO_data *,int);

/*
 * function	:	将ASC数据转换为BCD数据
 * param1		:	ASC数据串
 * param2		:	BCD数据缓存
 * param3		:	ASC数据串的长度
 * param4		:	转换模式
 * return		:	无
 */
void AscToBcd(UCHAR *, UCHAR *, int, UCHAR);

/*
 * function	:	将BCD数据转换为ASC数据
 * param1		:	ASC数据串
 * param2		:	BCD数据缓存
 * param3		:	ASC数据串的长度
 * param4		:	转换模式
 * return		:	无
 */
void BcdToAsc(UCHAR *, UCHAR *, int, UCHAR);

/*
 * function	:	使能65-128位
 * param1		:	ISO8583结构的指针
 * return		:	无
 */
void EnableSecBitmap(ISO_data *iso);


int SetIso(int bit, int len, unsigned char type, unsigned char flag);
/*
 * 函数：convert
 * 功能：转化十六字符为十进制数字
 * 入口：ch:     需要转换的字符
 * 出口： 转换后的十进制数
 * 返回：-1 失败
 */
int convert(char   ch);
#endif


