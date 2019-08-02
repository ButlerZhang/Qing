#include "NetworkServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>



NetworkServer::NetworkServer()
{
}

NetworkServer::~NetworkServer()
{
}

int NetworkServer::Accept()
{
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);

    int connfd = accept(                            //���ܿͻ��˵�����
        m_Socket,                                   //����socket
        (struct sockaddr*)&client_address,          //Զ��socket��ַ
        &client_addrlength);                        //Զ��socket��ַ�ĳ���
                                                    //�ɹ�ʱ���������ӵ�socket��ʧ�ܷ���-1������errno

    return connfd;
}

bool NetworkServer::StartServer(const std::string & IP, int port)
{
    if (!CreateSocket(PF_INET, SOCK_STREAM))
    {
        //ADD LOG
        return false;
    }

    if (!BindSocket(IP, port))
    {
        //ADD LOG
        return false;
    }

    if (!ListenSocket())
    {
        //ADD LOG
        return false;
    }

    return true;
}

bool NetworkServer::BindSocket(const std::string &IP, int Port)
{
    struct sockaddr_in Address;
    InitSockAddress(&Address, IP, Port);

    int Result = bind(                                  //��socket��Ҳ��Ϊ����socket�������ڷ����
        m_Socket,                                       //Ҫ�󶨵�socket
        (struct sockaddr*)&Address,                     //Ҫ�󶨵ĵ�ַ
        sizeof(Address));                               //��ַ����
    assert(Result != -1);                               //�󶨳ɹ�����0��ʧ�ܷ���-1������errno

    return Result != -1;
}

bool NetworkServer::ListenSocket()
{
    int Result = listen(                                //������������
        m_Socket,                                       //Ҫ������socket
        5);                                             //�ں˼������е���󳤶ȣ�����ֵΪ5
    assert(Result != -1);                               //�ɹ�����0��ʧ�ܷ���-1������errno

    return Result != -1;
}
