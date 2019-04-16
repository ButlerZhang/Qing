#include "NetworkBase.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>



NetworkBase::NetworkBase()
{
    m_Socket = -1;
}

NetworkBase::~NetworkBase()
{
    close(m_Socket);
}

bool NetworkBase::StartClient()
{
    if (!CreateSocket(PF_INET, SOCK_STREAM))
    {
        //ADD LOG
        return false;
    }

    return true;
}

bool NetworkBase::ConnectServer(const std::string & IP, int Port)
{
    struct sockaddr_in Address;
    InitSockAddress(&Address, IP, Port);

    if (connect(                                    //�ͻ��˷���Է�����������
        m_Socket,                                   //�ͻ��˵�socket
        (struct sockaddr*)&Address,                 //����˵�ַ
        sizeof(Address)) < 0)                       //����˵�ַ����
    {
        //TODO
        return false;
    }

    return true;
}

int NetworkBase::Accept()
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

bool NetworkBase::StartServer(const std::string & IP, int port)
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

int NetworkBase::SetNonBlocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);                //��ȡ�ļ��������ɵ�״̬��־
    int new_option = old_option | O_NONBLOCK;           //���÷�����״̬
    fcntl(fd, F_SETFL, new_option);                     //�����µ�״̬
    return old_option;                                  //���ؾɵ�״̬��־���Ա�����ָ�ʹ��
}

bool NetworkBase::CreateSocket(int Domain, int Type)
{
    m_Socket = socket(                                  //����socket
        Domain,                                         //�ײ�Э���壬PF_INET��ʾIPv4��PF_INET6��ʾIPv6��PF_UNIX��ʾUNIX������Э��
        Type,                                           //�������ͣ�SOCK_STREAM��ʾTCP��SOCK_DGRAM��ʾUDP
        0);                                             //��ǰ�����������ɵ�Э�鼯���£���ѡ��һ�������Э�飬ͨ����Ϊ0����ʾĬ��Э��
    assert(m_Socket >= 0);                              //����socketʧ�ܷ���-1������errno

    return m_Socket > 0;
}

bool NetworkBase::BindSocket(const std::string &IP, int Port)
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

bool NetworkBase::ListenSocket()
{
    int Result = listen(                                //������������
        m_Socket,                                       //Ҫ������socket
        5);                                             //�ں˼������е���󳤶ȣ�����ֵΪ5
    assert(Result != -1);                               //�ɹ�����0��ʧ�ܷ���-1������errno

    return Result != -1;
}

void NetworkBase::InitSockAddress(void *Address, const std::string & IP, int Port)
{
    uint16_t TempPort = static_cast<uint16_t>(Port);
    struct sockaddr_in *TempAddress = (sockaddr_in*)Address;

    //struct sockaddr_in Address;                             //sockaddr_inר����IPv4��ַ��sockaddr_in6ר����IPv6��ַ
    bzero(Address, sizeof(sockaddr_in));                      //��Address��ʼ��Ϊ0
    TempAddress->sin_family = AF_INET;                        //��ַ�壬AF_INET=PF_INET��AF_INET6=PF_INET6��AF_UNIX=PF_UNIX
    inet_pton(AF_INET, IP.c_str(), &(TempAddress->sin_addr)); //���ַ���IP��ַת�����������ֽ���������ʾ��IP��ַ
    TempAddress->sin_port = htons(TempPort);                  //�������ֽ���ת���������ֽ���
}
