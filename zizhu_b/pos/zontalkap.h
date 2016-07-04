/* ************************************************************************* */
/*  FILE NAME   : zontalkap.h                                                */
/*  PROGRAMMER  : Derya ENGIN                                                */
/*  DESCRIPTION : implementation of zontalkap protocol host part             */
/* ************************************************************************* */

#ifndef   _ZONTALK_AP_H_
#define   _ZONTALK_AP_H_

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_CHECKFILE                                      */
/*  DESCRIPTION :   查询基础文件是否存在(FLASHCLR.TXT/BASE.TXT/HANDLER.TXT)  */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_NOK/ZONTALK_OK                                   */
/* ......................................................................... */
int  ZONTALKAP_CHECKFILE(char (*lpFileList)[64], int *lpCount);

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_CHECKUPDATA                                    */
/*  DESCRIPTION :   根据FW版本判断是否升级                                   */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_NOK/ZONTALK_OK                                   */
/* ......................................................................... */
int ZONTALKAP_CHECKUPDATA(void *dummy);

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_DLOADFILE                                      */
/*  DESCRIPTION :   文件下载                                                 */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_NOK/ZONTALK_OK                                   */
/* ......................................................................... */
int ZONTALKAP_DLOADFILE(char *lpFileName);

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_CallBack                                       */
/*  DESCRIPTION :   FW下载                                                   */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   none                                                     */
/* ......................................................................... */
short ZONTALKAP_CallBack(short  i_Act, void *i_Val);

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_DOWNLOAD                                       */
/*  DESCRIPTION :   FW下载                                                   */
/*  INPUT       :   none                                                     */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_NOK/ZONTALK_OK                                   */
/* ......................................................................... */
int ZONTALKAP_DOWNLOAD(void *dummy);

/* ......................................................................... */
/*  NAME        :   ZONTALKAP_RemoveFWFiles                                  */
/*  DESCRIPTION :   移除固件文件包                                           */
/*  INPUT       :   dummy                                                    */
/*  OUTPUT      :   none                                                     */
/*  RETURN      :   ZONTALK_OK / ZONTALK_NOK                                 */
/* ......................................................................... */
int ZONTALKAP_RemoveFWFiles(void *dummy);


int ZONTALK_PingReader(int iTimeOut);

void ZONTALK_RemoveAllFile(char(*FileList)[64], int FileCount);

void ZONTALK_RemoveLocalFile(char *i_Fil);

#endif /* _ZONTALK_AP_H_ */
