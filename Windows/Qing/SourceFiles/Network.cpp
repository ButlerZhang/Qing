#include "..\HeaderFiles\Network.h"



namespace Qing
{
    Network::Network()
    {
        m_Socket = INVALID_SOCKET;
    }

    Network::~Network()
    {
        if (m_Socket != INVALID_SOCKET)
        {
            closesocket(m_Socket);
        }
    }

    bool Network::CreateSocket()
    {
        m_Socket = socket(AF_INET,              //Socket家族取值为AF_INET
            SOCK_STREAM,                        //Socket的种类为流Socket
            IPPROTO_TCP);                       //使用的协议是TCP协议

        return m_Socket != INVALID_SOCKET;
    }

    bool Network::SendData()
    {
        if (m_Socket != INVALID_SOCKET)
        {
            char buf[1024];
            int ret = send(m_Socket, buf, sizeof(buf), 0);
            return (ret == 0);
        }

        return false;
    }

    bool Network::RecvData()
    {
        if (m_Socket != INVALID_SOCKET)
        {
            char buf[1024];
            int ret = recv(m_Socket, buf, sizeof(buf), 0);
            return (ret == 0);
        }

        return false;
    }
}
