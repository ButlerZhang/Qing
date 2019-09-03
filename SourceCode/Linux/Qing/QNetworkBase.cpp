#include "QNetworkBase.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>



QNetworkBase::QNetworkBase()
{
    m_Socket = -1;
}

QNetworkBase::~QNetworkBase()
{
    close(m_Socket);
}

int QNetworkBase::SetNonBlocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);                //��ȡ�ļ��������ɵ�״̬��־
    int new_option = old_option | O_NONBLOCK;           //���÷�����״̬
    fcntl(fd, F_SETFL, new_option);                     //�����µ�״̬
    return old_option;                                  //���ؾɵ�״̬��־���Ա�����ָ�ʹ��
}

bool QNetworkBase::CreateSocket(int Domain, int Type)
{
    m_Socket = socket(                                  //����socket
        Domain,                                         //�ײ�Э���壬PF_INET��ʾIPv4��PF_INET6��ʾIPv6��PF_UNIX��ʾUNIX������Э��
        Type,                                           //�������ͣ�SOCK_STREAM��ʾTCP��SOCK_DGRAM��ʾUDP
        0);                                             //��ǰ�����������ɵ�Э�鼯���£���ѡ��һ�������Э�飬ͨ����Ϊ0����ʾĬ��Э��
    assert(m_Socket >= 0);                              //����socketʧ�ܷ���-1������errno

    return m_Socket > 0;
}

void QNetworkBase::InitSockAddress(void *Address, const std::string & IP, int Port)
{
    uint16_t TempPort = static_cast<uint16_t>(Port);
    struct sockaddr_in *TempAddress = (sockaddr_in*)Address;

    //struct sockaddr_in Address;                             //sockaddr_inר����IPv4��ַ��sockaddr_in6ר����IPv6��ַ
    bzero(Address, sizeof(sockaddr_in));                      //��Address��ʼ��Ϊ0
    TempAddress->sin_family = AF_INET;                        //��ַ�壬AF_INET=PF_INET��AF_INET6=PF_INET6��AF_UNIX=PF_UNIX
    inet_pton(AF_INET, IP.c_str(), &(TempAddress->sin_addr)); //���ַ���IP��ַת�����������ֽ���������ʾ��IP��ַ
    TempAddress->sin_port = htons(TempPort);                  //�������ֽ���ת���������ֽ���
}
