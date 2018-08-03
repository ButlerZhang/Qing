#pragma once
#include "QingBase.h"
#include <WinSock2.h>
#include <string>

QING_NAMESPACE_BEGIN



bool QING_DLL StartupNetwork();
bool QING_DLL ShutdownNetwork();



class QING_DLL Network
{
public:

    Network();
    virtual ~Network();

    SOCKET GetSocket() const { return m_Socket; }
    bool RecvData(SOCKET Socket, char *Buffer, int BufferSize);
    bool SendData(SOCKET Socket, const char *Buffer, int BufferSize);

protected:

    bool CreateSocket();
    bool BindSocket(const std::string &IP, int Port);
    bool StartListen(int backlog = SOMAXCONN);
    bool Connect(const std::string &IP, int Port);
    SOCKET AcceptConnect();

protected:

    SOCKET  m_Socket;
};

QING_NAMESPACE_END