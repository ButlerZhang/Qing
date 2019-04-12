#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>



static bool g_stop = false;

//SIGTERM信号处理函数，触发时结束主程序中的循环
static void handle_term(int sig)
{
    g_stop = true;
}

void blacklog_test(int argc, char* argv[])
{
    signal(SIGTERM, handle_term);               //注册信号处理函数

    if (argc < 3) {
        printf("usage: %s ip_address port_number backlog\n",
            basename(argv[0]));                 //basename的头文件是<string.h>，获取路径中最后一个/后面的内容
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    int backlog = atoi(argv[3]);

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
        backlog);                               //内核监听队列的最大长度，超过backlog的侦听请求服务器不再受理
    assert(ret != -1);                          //成功返回0，失败返回-1并设置errno

    while (!g_stop) {
        sleep(1);
    }

    close(sock);                                //关闭socket，不是立即关闭，而是减少引用计数
                                                //如果想要立即关闭，可以调用shutdown函数
}
