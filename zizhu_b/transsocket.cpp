#include "transsocket.h"
#include <QTime>
#include <QCoreApplication>

transSocket * transSocket::msocket = NULL;

transSocket::transSocket(QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket(this);
}

/*
    int connectSocket(QString *ip,int port);
    int writeSocket(unsigned char * sendMsg);
    int readSocket(unsigned char * recvMsg);
    int closeSocket();
*/

int transSocket::connectSocket(QString ip, int port,int timeout)
{
    bool result;
    m_socket->connectToHost(ip,port);

    result = m_socket->waitForConnected(timeout);
    if(result == false)
    {
        m_socket->close();
        return FAIL;
    }

    return SUCCESS;
}

int transSocket::writeSocket(unsigned char *sendMsg,int sendLen,int timeout)
{
    int byte;
    byte = m_socket->write((const char *)sendMsg,sendLen);
    if(byte != sendLen)
    {
        return FAIL;
    }

    return SUCCESS;
}

int transSocket::readSocket(unsigned char *recvMsg,int timeout,int * len)
{
    bool result;
    QByteArray recvData;

    //延时2s后去检测socket
    //QTime dieTime = QTime::currentTime().addMSecs(2000);
    //while( QTime::currentTime() < dieTime )
    //QCoreApplication::processEvents(QEventLoop::AllEvents, 2000);

    result = m_socket->waitForReadyRead(timeout);
    if(result == false)
    {
        m_socket->close();
        return FAIL;
    }
    recvData =  m_socket->readAll();

//    qDebug()<<recvData;
    *len = recvData.length();
    memcpy(recvMsg,recvData.data(),recvData.length());
    return SUCCESS;
}


int transSocket::closeSocket()
{
     m_socket->close();
     return SUCCESS;
}

int transSocket::startConnect(char *ip, int port, int timeout, char *sendMsg, int sendLen, char  *recvMsg)
{
    int byte;
    QByteArray recvData;
    bool result;

    m_socket = new QTcpSocket(this);

    m_socket->connectToHost(ip,port);

    result = m_socket->waitForConnected(timeout);
    if(result == false)
    {
        m_socket->close();
        return -1;
    }

    byte = m_socket->write((const char *)sendMsg,sendLen);
    if(byte != sendLen)
    {
        m_socket->close();
        return -3;
    }


    //延时2s后去检测socket
    QTime dieTime = QTime::currentTime().addMSecs(2000);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 2000);

    result = m_socket->waitForReadyRead(10000);
    if(result == false)
    {
        m_socket->close();
        return -4;
    }

    recvData =  m_socket->readAll();

     qDebug()<<recvData;

     memcpy(recvMsg,recvData.data(),recvData.length());
     m_socket->close();

     return 0;
}

