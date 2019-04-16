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

    if (connect(                                    //客户端发起对服务器的连接
        m_Socket,                                   //客户端的socket
        (struct sockaddr*)&Address,                 //服务端地址
        sizeof(Address)) < 0)                       //服务端地址长度
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

    int connfd = accept(                            //接受客户端的连接
        m_Socket,                                   //侦听socket
        (struct sockaddr*)&client_address,          //远端socket地址
        &client_addrlength);                        //远端socket地址的长度
                                                    //成功时返回新连接的socket，失败返回-1并设置errno

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
    int old_option = fcntl(fd, F_GETFL);                //获取文件描述符旧的状态标志
    int new_option = old_option | O_NONBLOCK;           //设置非阻塞状态
    fcntl(fd, F_SETFL, new_option);                     //设置新的状态
    return old_option;                                  //返回旧的状态标志，以便后续恢复使用
}

bool NetworkBase::CreateSocket(int Domain, int Type)
{
    m_Socket = socket(                                  //创建socket
        Domain,                                         //底层协议族，PF_INET表示IPv4，PF_INET6表示IPv6，PF_UNIX表示UNIX本地域协议
        Type,                                           //服务类型，SOCK_STREAM表示TCP，SOCK_DGRAM表示UDP
        0);                                             //在前两个参数构成的协议集合下，再选择一个具体的协议，通常设为0，表示默认协议
    assert(m_Socket >= 0);                              //创建socket失败返回-1被设置errno

    return m_Socket > 0;
}

bool NetworkBase::BindSocket(const std::string &IP, int Port)
{
    struct sockaddr_in Address;
    InitSockAddress(&Address, IP, Port);

    int Result = bind(                                  //绑定socket，也称为命名socket，仅用于服务端
        m_Socket,                                       //要绑定的socket
        (struct sockaddr*)&Address,                     //要绑定的地址
        sizeof(Address));                               //地址长度
    assert(Result != -1);                               //绑定成功返回0，失败返回-1被设置errno

    return Result != -1;
}

bool NetworkBase::ListenSocket()
{
    int Result = listen(                                //侦听连接请求
        m_Socket,                                       //要侦听的socket
        5);                                             //内核监听队列的最大长度，经验值为5
    assert(Result != -1);                               //成功返回0，失败返回-1并设置errno

    return Result != -1;
}

void NetworkBase::InitSockAddress(void *Address, const std::string & IP, int Port)
{
    uint16_t TempPort = static_cast<uint16_t>(Port);
    struct sockaddr_in *TempAddress = (sockaddr_in*)Address;

    //struct sockaddr_in Address;                             //sockaddr_in专用于IPv4地址，sockaddr_in6专用于IPv6地址
    bzero(Address, sizeof(sockaddr_in));                      //将Address初始化为0
    TempAddress->sin_family = AF_INET;                        //地址族，AF_INET=PF_INET，AF_INET6=PF_INET6，AF_UNIX=PF_UNIX
    inet_pton(AF_INET, IP.c_str(), &(TempAddress->sin_addr)); //将字符串IP地址转换成用网络字节序整数表示的IP地址
    TempAddress->sin_port = htons(TempPort);                  //将主机字节序转换成网络字节序
}
