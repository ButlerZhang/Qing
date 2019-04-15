#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>



void accept_exception(int argc, char* argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n",
            basename(argv[0]));                 //basename的头文件是<string.h>，获取路径中最后一个/后面的内容
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    int sock = socket(                          //创建socket
        PF_INET,                                //底层协议族，PF_INET表示IPv4，PF_INET6表示IPv6，PF_UNIX表示UNIX本地域协议
        SOCK_STREAM,                            //服务类型，SOCK_STREAM表示TCP，SOCK_DGRAM表示UDP
        0);                                     //在前两个参数构成的协议集合下，再选择一个具体的协议，通常设为0，表示默认协议
    assert(sock >= 0);                          //创建socket失败返回-1被设置errno

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET,
        SO_REUSEADDR,                           //重用本地地址，避免在TIME_WAIT状态下无法使用同一个端口号，仅用于服务端
        &reuse, sizeof(reuse));

    struct sockaddr_in address;                 //sockaddr_in专用于IPv4地址，sockaddr_in6专用于IPv6地址
    bzero(&address, sizeof(address));           //将address初始化为0
    address.sin_family = AF_INET;               //地址族，AF_INET=PF_INET，AF_INET6=PF_INET6，AF_UNIX=PF_UNIX
    inet_pton(AF_INET, ip, &address.sin_addr);  //将字符串IP地址转换成用网络字节序整数表示的IP地址
    address.sin_port = htons(port);             //将主机字节序转换成网络字节序

    int ret = bind(                             //绑定socket，也称为命名socket，仅用于服务端
        sock,                                   //要绑定的socket
        (struct sockaddr*)&address,             //要绑定的地址
        sizeof(address));                       //地址长度
    assert(ret != -1);                          //绑定成功返回0，失败返回-1被设置errno

    ret = listen(                               //侦听连接请求
        sock,                                   //要侦听的socket
        5);                                     //内核监听队列的最大长度，经验值为5
    assert(ret != -1);                          //成功返回0，失败返回-1并设置errno

    sleep(20);                                  //暂停20秒以等待客户端连接和相关操作(掉线或退出)完成

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(                        //接受客户端的连接
        sock,                                   //侦听socket
        (struct sockaddr*)&client,              //远端socket地址
        &client_addrlength);                    //远端socket地址的长度
                                                //成功时返回新连接的socket，失败返回-1并设置errno

    if (connfd < 0) {
        printf("errno is : %d\n", errno);       //打印出错的原因
    }
    else {
        char remote[INET_ADDRSTRLEN];
        printf("connected with ip = %s and port = %d\n",
            inet_ntop(AF_INET,                 //网络字节序IP转换成字符串IP
                &client.sin_addr, remote, INET_ADDRSTRLEN),
            ntohs(client.sin_port));           //网络字节序转换成主机字节序，端口值

        close(connfd);
    }

    close(sock);                                //关闭socket，不是立即关闭，而是减少引用计数
                                                //如果想要立即关闭，可以调用shutdown函数
}
