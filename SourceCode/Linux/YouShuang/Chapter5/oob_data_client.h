#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>



void oob_data_client(int argc, char* argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n",
            basename(argv[0]));                 //basename的头文件是<string.h>，获取路径中最后一个/后面的内容
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    struct sockaddr_in server_address;                 //sockaddr_in专用于IPv4地址，sockaddr_in6专用于IPv6地址
    bzero(&server_address, sizeof(server_address));    //将server_address初始化为0
    server_address.sin_family = AF_INET;               //地址族，AF_INET=PF_INET，AF_INET6=PF_INET6，AF_UNIX=PF_UNIX
    inet_pton(AF_INET, ip, &server_address.sin_addr);  //将字符串IP地址转换成用网络字节序整数表示的IP地址
    server_address.sin_port = htons(port);             //将主机字节序转换成网络字节序

    int sockfd = socket(                        //创建socket
        PF_INET,                                //底层协议族，PF_INET表示IPv4，PF_INET6表示IPv6，PF_UNIX表示UNIX本地域协议
        SOCK_STREAM,                            //服务类型，SOCK_STREAM表示TCP，SOCK_DGRAM表示UDP
        0);                                     //在前两个参数构成的协议集合下，再选择一个具体的协议，通常设为0，表示默认协议
    assert(sockfd >= 0);                        //创建socket失败返回-1被设置errno

    if (connect(                                //客户端发起对服务器的连接
        sockfd,                                 //客户端的socket
        (struct sockaddr*)&server_address,      //服务端地址
        sizeof(server_address)) < 0) {          //服务端地址长度
        printf("Connection failed\n");          //成功返回0，失败返回-1并设置errno
    }
    else {
        const char* oob_data = "abc";
        const char* normal_data = "123";
        send(sockfd, normal_data, strlen(normal_data), 0);

        send(                                   //往sockfd上写数据，即发送数据
            sockfd,                             //客户端的socket
            oob_data,                           //数据缓冲区
            strlen(oob_data),                   //缓冲区大小
            MSG_OOB);                           //提供额外的控制，MSG_OOB表示紧急数据

        send(sockfd, normal_data, strlen(normal_data), 0);
    }

    close(sockfd);                              //关闭socket，不是立即关闭，而是减少引用计数
                                                //如果想要立即关闭，可以调用shutdown函数
}
