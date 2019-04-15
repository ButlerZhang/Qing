#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>



void writev_server(int argc, char* argv[])
{
    if (argc <= 3) {
        printf("usage: %s ip_address port_number filename\n",
            basename(argv[0]));                 //basename的头文件是<string.h>，获取路径中最后一个/后面的内容
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    const char *file_name = argv[3];
    const char* status_line[2] = { "200 OK", "500 Internal server error" };

    int sock = socket(                          //创建socket
        PF_INET,                                //底层协议族，PF_INET表示IPv4，PF_INET6表示IPv6，PF_UNIX表示UNIX本地域协议
        SOCK_STREAM,                            //服务类型，SOCK_STREAM表示TCP，SOCK_DGRAM表示UDP
        0);                                     //在前两个参数构成的协议集合下，再选择一个具体的协议，通常设为0，表示默认协议
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

        const int BUFFER_SIZE = 1024;
        char header_buf[BUFFER_SIZE];           //用于保存HTTP应答的状态行、头部字段和一个空行的缓存区
        memset(header_buf, '\0', BUFFER_SIZE);  //初始化

        char *file_buf = NULL;                  //用于存放目标文件内容的应用程序缓存
        struct stat file_stat;                  //用于获取目标文件的属性，比如是否为目录、文件夹大小等
        bool valid = true;                      //记录目标文件是否是有效文件
        int len = 0;                            //缓存区header_buf目前已经使用了多少字节的空间

        if (stat(file_name, &file_stat) < 0) {  //目标文件不存在
            valid = false;
        }
        else {
            if (S_ISDIR(file_stat.st_mode)) {   //目标文件是一个目录
                valid = false;
            }
            else if (file_stat.st_mode & S_IROTH) { //当前用户有读取目标文件的权限

                //动态分配缓存区file_buf，并指定其大小为目标文件的大小
                //file_stat.st_size加1，然后将目标文件读入缓存区file_buf中
                int fd = open(file_name, O_RDONLY);
                file_buf = new char[file_stat.st_size + 1];
                memset(file_buf, '\0', file_stat.st_size + 1);

                if (read(fd, file_buf, file_stat.st_size) < 0) {
                    valid = false;
                }
            }
            else {
                valid = false;
            }

            //如果目标文件有效，则发送正常的HTTP应答
            if (valid) {

                //下面这部分内容将HTTP应答的状态行、Content-Length头部字段和一个空行依次加入header_buf中
                ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[0]);

                len += ret;
                ret = snprintf(header_buf, BUFFER_SIZE - 1 - len, "Content-Length: %d\r\n", file_stat.st_size);

                len += ret;
                ret = snprintf(header_buf, BUFFER_SIZE - 1 - len, "%s", "\r\n");

                //利用writev将header_buf和file_buf的内容一并写出
                struct iovec iv[2];
                iv[0].iov_base = header_buf;
                iv[0].iov_len = strlen(header_buf);
                iv[1].iov_base = file_buf;
                iv[1].iov_len = file_stat.st_size;
                ssize_t writev_size = writev(connfd, iv, 2);
                printf("writev size = %d\n", writev_size);
            }
            else {                              //如果目标文件无效，则通知客户端服务器发生了内部错误

                ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[1]);
                len += ret;

                ret = snprintf(header_buf, BUFFER_SIZE - 1 - len, "%s", "\r\n");
                ssize_t send_size = send(connfd, header_buf, strlen(header_buf), 0);
                printf("send size = %d\n", send_size);
            }

            close(connfd);

            if (file_buf != NULL) {
                delete[] file_buf;
            }
        }
    }

    close(sock);                                //关闭socket，不是立即关闭，而是减少引用计数
                                                //如果想要立即关闭，可以调用shutdown函数
}
