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
    int old_option = fcntl(fd, F_GETFL);                //获取文件描述符旧的状态标志
    int new_option = old_option | O_NONBLOCK;           //设置非阻塞状态
    fcntl(fd, F_SETFL, new_option);                     //设置新的状态
    return old_option;                                  //返回旧的状态标志，以便后续恢复使用
}

bool QNetworkBase::CreateSocket(int Domain, int Type)
{
    m_Socket = socket(                                  //创建socket
        Domain,                                         //底层协议族，PF_INET表示IPv4，PF_INET6表示IPv6，PF_UNIX表示UNIX本地域协议
        Type,                                           //服务类型，SOCK_STREAM表示TCP，SOCK_DGRAM表示UDP
        0);                                             //在前两个参数构成的协议集合下，再选择一个具体的协议，通常设为0，表示默认协议
    assert(m_Socket >= 0);                              //创建socket失败返回-1被设置errno

    return m_Socket > 0;
}

void QNetworkBase::InitSockAddress(void *Address, const std::string & IP, int Port)
{
    uint16_t TempPort = static_cast<uint16_t>(Port);
    struct sockaddr_in *TempAddress = (sockaddr_in*)Address;

    //struct sockaddr_in Address;                             //sockaddr_in专用于IPv4地址，sockaddr_in6专用于IPv6地址
    bzero(Address, sizeof(sockaddr_in));                      //将Address初始化为0
    TempAddress->sin_family = AF_INET;                        //地址族，AF_INET=PF_INET，AF_INET6=PF_INET6，AF_UNIX=PF_UNIX
    inet_pton(AF_INET, IP.c_str(), &(TempAddress->sin_addr)); //将字符串IP地址转换成用网络字节序整数表示的IP地址
    TempAddress->sin_port = htons(TempPort);                  //将主机字节序转换成网络字节序
}
