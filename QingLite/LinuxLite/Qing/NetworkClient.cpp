#include "NetworkClient.h"
#include <sys/socket.h>
#include <netinet/in.h>



NetworkClient::NetworkClient()
{
    m_IsConnected = false;
}

NetworkClient::~NetworkClient()
{
}

bool NetworkClient::StartClient(const std::string &IP, int Port)
{
    if (!CreateSocket(PF_INET, SOCK_STREAM))
    {
        //ADD LOG
        return false;
    }

    ConnectServer(IP, Port);
    return true;
}

bool NetworkClient::ConnectServer(const std::string & IP, int Port)
{
    if (!m_IsConnected)
    {
        struct sockaddr_in Address;
        InitSockAddress(&Address, IP, Port);

        if (connect(                                    //�ͻ��˷���Է�����������
            m_Socket,                                   //�ͻ��˵�socket
            (struct sockaddr*)&Address,                 //����˵�ַ
            sizeof(Address)) < 0)                       //����˵�ַ����
        {
            //TODO
            m_IsConnected = false;
        }

        m_IsConnected = true;
    }

    return m_IsConnected;
}
