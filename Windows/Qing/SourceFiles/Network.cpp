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
        m_Socket = socket(AF_INET,              //Socket����ȡֵΪAF_INET
            SOCK_STREAM,                        //Socket������Ϊ��Socket
            IPPROTO_TCP);                       //ʹ�õ�Э����TCPЭ��

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
