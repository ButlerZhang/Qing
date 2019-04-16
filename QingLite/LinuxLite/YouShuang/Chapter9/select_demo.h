#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>



void select_demo(int argc, char* argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n",
            basename(argv[0]));                 //basename的头文件是<string.h>，获取路径中最后一个/后面的内容
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    struct sockaddr_in address;                 //sockaddr_in专用于IPv4地址，sockaddr_in6专用于IPv6地址
    bzero(&address, sizeof(address));           //将address初始化为0
    address.sin_family = AF_INET;               //地址族，AF_INET=PF_INET，AF_INET6=PF_INET6，AF_UNIX=PF_UNIX
    inet_pton(AF_INET, ip, &address.sin_addr);  //将字符串IP地址转换成用网络字节序整数表示的IP地址
    address.sin_port = htons(port);             //将主机字节序转换成网络字节序

    int listenfd = socket(                      //创建socket
        PF_INET,                                //底层协议族，PF_INET表示IPv4，PF_INET6表示IPv6，PF_UNIX表示UNIX本地域协议
        SOCK_STREAM,                            //服务类型，SOCK_STREAM表示TCP，SOCK_DGRAM表示UDP
        0);                                     //在前两个参数构成的协议集合下，再选择一个具体的协议，通常设为0，表示默认协议
    assert(listenfd >= 0);                      //创建socket失败返回-1被设置errno

    int ret = bind(                             //绑定socket，也称为命名socket，仅用于服务端
        listenfd,                               //要绑定的socket
        (struct sockaddr*)&address,             //要绑定的地址
        sizeof(address));                       //地址长度
    assert(ret != -1);                          //绑定成功返回0，失败返回-1被设置errno

    ret = listen(                               //侦听连接请求
        listenfd,                               //要侦听的socket
        5);                                     //内核监听队列的最大长度，经验值为5
    assert(ret != -1);                          //成功返回0，失败返回-1并设置errno

    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    int connfd = accept(                        //接受客户端的连接
        listenfd,                               //侦听socket
        (struct sockaddr*)&client_address,      //远端socket地址
        &client_addrlength);                    //远端socket地址的长度
                                                //成功时返回新连接的socket，失败返回-1并设置errno

    if (connfd < 0) {
        printf("errno is : %d\n", errno);       //打印出错的原因
    }
    else {

        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];

        fd_set read_fds;
        fd_set exception_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&exception_fds);

        while (true)
        {
            memset(buffer, '\0', BUFFER_SIZE);

            FD_SET(connfd, &read_fds);
            FD_SET(connfd, &exception_fds);

            int ret = select(                   //在指定时间内侦听用户感兴趣的文件描述符上的可读可写和异常事件
                connfd + 1,                     //指定被监听的文件描述符的总数，通常设置为所有文件描述符中的最大值加1
                &read_fds,                      //可读文件描述符集合
                NULL,                           //可写文件描述符集合
                &exception_fds,                 //异常事件文件描述符集合
                NULL);                          //超时时间，NULL表示一直阻塞，直到某个文件描述符就绪
            if (ret < 0) {
                printf("selection failure\n");
                break;
            }
            else {
                printf("current socket connfd = %d\n", connfd);
            }

            if (FD_ISSET(connfd, &read_fds)) {                              //对于可读事件，采用普通的recv函数读取数据
                ssize_t recv_ret = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
                if (recv_ret <= 0) {
                    break;
                }
                else {
                    printf("get %d bytes of normal data: %s\n", recv_ret, buffer);
                }
            }
            else if (FD_ISSET(connfd, &exception_fds)) {                    //对于异常事件，采用带MSG_OOB标志的recv函数读取带外数据
                ssize_t recv_ret = recv(connfd, buffer, BUFFER_SIZE - 1, MSG_OOB);
                if (recv_ret <= 0) {
                    break;
                }
                else {
                    printf("get %d bytes of oob data: %s\n", recv_ret, buffer);
                }
            }
        }

        close(connfd);
    }

    close(listenfd);                            //关闭socket，不是立即关闭，而是减少引用计数
                                                //如果想要立即关闭，可以调用shutdown函数
}
