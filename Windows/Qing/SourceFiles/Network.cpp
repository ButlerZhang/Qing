#include "..\HeaderFiles\Network.h"
#include "..\HeaderFiles\QingLog.h"
#include <WS2tcpip.h>

QING_NAMESPACE_BEGIN



long g_NetworkInitCounter = 0;

bool QING_DLL StartupNetwork()
{
    //ԭ�Ӽӣ���ֻ֤����һ��WSAStartup
    if (InterlockedIncrement(&g_NetworkInitCounter) == 1)
    {
        WSAData wsaData;
        //WSAStartup������ʼ��Windows Sockets DLL���ɹ�����ܵ�������API
        //MAKEWORD(2,2)��ʾʹ�õ���WinSocket2.2�汾
        int Result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (Result == NO_ERROR)
        {
            return true;
        }

        QingLog::Write(LL_ERROR, "WSAStartup error = %d.", Result);
    }

    return false;
}

bool QING_DLL ShutdownNetwork()
{
    //ԭ�Ӽ�����ֻ֤����һ��WSACleanup
    if (InterlockedDecrement(&g_NetworkInitCounter) == 0)
    {
        //��ֹWinsock 2 DLL��ʹ��
        int Result = ::WSACleanup();
        if (Result == NO_ERROR)
        {
            return true;
        }

        QingLog::Write(LL_ERROR, "WSACleanup error = %d.", Result);
    }

    return false;
}



Network::Network()
{
    m_Socket = INVALID_SOCKET;
}

Network::~Network()
{
    if (m_Socket != INVALID_SOCKET)
    {
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
    }
}

bool Network::CreateSocket()
{
    bool Result = true;

    //Socket����ȡֵΪAF_INET
    //Socket������Ϊ��Socket
    //Socketʹ�õ�Э����TCPЭ��
    m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_Socket == INVALID_SOCKET)
    {
        int WSAError = WSAGetLastError();
        Result = false;
    }

    return Result;
}

bool Network::BindSocket(const std::string & IP, int Port)
{
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Port);
    inet_pton(AF_INET, IP.c_str(), &(addr.sin_addr.S_un.S_addr));

    bool Result = true;
    int BindResult = bind(m_Socket, (LPSOCKADDR)&addr, sizeof(addr));
    if (BindResult == SOCKET_ERROR)
    {
        int WSAError = WSAGetLastError();
        Result = false;
    }

    return Result;
}

bool Network::StartListen(int backlog)
{
    bool Result = true;
    int ListenError = listen(m_Socket, backlog) == 0;
    if (ListenError == SOCKET_ERROR)
    {
        int WSAError = WSAGetLastError();
        Result = false;
    }

    return Result;
}

SOCKET Network::AcceptConnect()
{
    fd_set readset;
    timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 1000;
    FD_ZERO(&readset);
    FD_SET(m_Socket, &readset);

    SOCKET NewSocket = INVALID_SOCKET;
    int ret = select(FD_SETSIZE, &readset, NULL, NULL, &timeout);
    if (ret > 0 && FD_ISSET(m_Socket, &readset))
    {
        SOCKADDR addr;
        int len = sizeof(addr);
        NewSocket = accept(m_Socket, (SOCKADDR*)&addr, (int*)&len);
        if (NewSocket == INVALID_SOCKET)
        {
            int WSAError = WSAGetLastError();
        }
    }

    return NewSocket;
}

bool Network::Connect(const std::string & IP, int Port)
{
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(Port);
    inet_pton(AF_INET, IP.c_str(), &(addr.sin_addr.S_un.S_addr));

    bool Result = true;
    int ConnectError = connect(m_Socket, (SOCKADDR*)&addr, sizeof(addr));
    if (ConnectError == SOCKET_ERROR)
    {
        int WSAError = WSAGetLastError();
        Result = false;
    }

    return Result;
}

bool Network::SendData(SOCKET Socket, const char *Buffer, int BufferSize)
{
    if (Socket == INVALID_SOCKET)
    {
        return false;
    }

    int SendError = send(Socket, Buffer, BufferSize, 0);
    if (SendError == SOCKET_ERROR)
    {
        int WSAError = WSAGetLastError();
        return false;
    }

    return true;
}

bool Network::RecvData(SOCKET Socket, char *Buffer, int BufferSize)
{
    if (Socket == INVALID_SOCKET)
    {
        return false;
    }

    int RecvError = recv(Socket, Buffer, BufferSize, 0);
    if (RecvError = SOCKET_ERROR)
    {
        int WSAError = WSAGetLastError();
        return false;
    }

    return true;
}

QING_NAMESPACE_END