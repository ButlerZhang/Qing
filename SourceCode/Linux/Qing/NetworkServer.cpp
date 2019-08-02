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

    int connfd = accept(                            //接受客户端的连接
        m_Socket,                                   //侦听socket
        (struct sockaddr*)&client_address,          //远端socket地址
        &client_addrlength);                        //远端socket地址的长度
                                                    //成功时返回新连接的socket，失败返回-1并设置errno

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

    int Result = bind(                                  //绑定socket，也称为命名socket，仅用于服务端
        m_Socket,                                       //要绑定的socket
        (struct sockaddr*)&Address,                     //要绑定的地址
        sizeof(Address));                               //地址长度
    assert(Result != -1);                               //绑定成功返回0，失败返回-1被设置errno

    return Result != -1;
}

bool NetworkServer::ListenSocket()
{
    int Result = listen(                                //侦听连接请求
        m_Socket,                                       //要侦听的socket
        5);                                             //内核监听队列的最大长度，经验值为5
    assert(Result != -1);                               //成功返回0，失败返回-1并设置errno

    return Result != -1;
}
