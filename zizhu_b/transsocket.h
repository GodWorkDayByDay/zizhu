#ifndef TRANSSOCKET_H
#define TRANSSOCKET_H

#include <QObject>
#include <QTcpSocket>

#define SUCCESS 0
#define FAIL -1

class transSocket : public QObject
{
    Q_OBJECT
public:
    explicit transSocket(QObject *parent = 0);

    /*
     *功能:连接socket
     *参数:ip port timeout
     *返回值:0:成功， -1 失败
     */
    int connectSocket(QString ip, int port, int timeout);

    /*
     * 功能：socket发送数据
     * 返回值: 0：成功 ， -1 失败
    */
    int writeSocket(unsigned char * sendMsg, int sendLen,int timeout);

    /*
    *功能：读socket
    * 返回值: 0：成功 ， -1 失败
    */
    int readSocket(unsigned char * recvMsg, int timeout, int *len);

    /*
     * 功能:关闭socket
    */
    int closeSocket();


     static transSocket *getInstance()
     {
         if(msocket == NULL)
         {
            msocket = new transSocket();
         }

        return msocket;
     }
private:
     int startConnect(char * ip, int port, int timeout, char * sendMsg, int sendLen, char *recvMsg);

private:
     QTcpSocket *m_socket;

     static transSocket * msocket;
};

#endif // TRANSSOCKET_H
