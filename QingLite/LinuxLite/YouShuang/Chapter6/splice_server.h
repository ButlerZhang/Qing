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
#include <fcntl.h>



void splice_server(int argc, char* argv[])
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
        0);                                     //在前两个参数构成的协议集合下，再选择一个具体的协议，通常设为0，表示默认
    assert(sock >= 0);                          //创建socket失败返回-1被设置errno

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

        printf("connect succeed\n");

        int pipefd[2];
        assert(ret != -1);

        ret = pipe(pipefd);                     //创建管道
        assert(ret != -1);
        printf("pipe: pipefd[0] = %d, pipefd[1] = %d\n", pipefd[0], pipefd[1]);

        while (true)
        {
            //将connfd上流入的客户数据定向到管道中
            __ssize_t splice_size = splice(         //用于在两个文件描述符之间移动数据，也是零拷贝操作
                connfd,                             //待输入数据的文件描述符
                NULL,                               //输入数据的流从何处开始读数据，对于管道文件描述符设置为NULL
                pipefd[1],                          //待输出数据的文件描述符
                NULL,                               //输出数据的流从何处开始写数据，对于管道文件描述符设置为NULL
                32768,                              //移动数据的长度
                SPLICE_F_MORE | SPLICE_F_MOVE);     //控制数据如何移动
            assert(splice_size != -1);

            //将管道的输出定向到connfd客户连接文件描述符
            splice_size = splice(pipefd[0], NULL, connfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
            assert(splice_size != -1);
        }

        close(connfd);
    }

    close(sock);                                //关闭socket，不是立即关闭，而是减少引用计数
                                                //如果想要立即关闭，可以调用shutdown函数
}
