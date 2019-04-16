#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>



enum CHECK_STATE                                    //主状态机的两种可能状态
{
    CHECK_STATE_REQUESTLINE = 0,                    //当前正在分析请求行
    CHECK_STATE_HEADER                              //当前正在分析头部字段
};

enum LINE_STATUS                                    //从状态机的三种可能状态，即行的读取状态
{
    LINE_OK = 0,                                    //读取到一个完整的行
    LINE_BAD,                                       //行出错
    LINE_OPEN                                       //行数据尚且不完整
};

enum HTTP_CODE                                      //服务器处理HTTP请求的结果
{
    NO_REQUEST,                                     //请求不完整，需要继续读取客户数据
    GET_REQUEST,                                    //获得了一个完整的客户请求
    BAD_REQUEST,                                    //客户请求有语法错误
    FORBIDDEN_REQUEST,                              //客户对资源没有足够的访问权限
    INTERNAL_ERROR,                                 //服务器内部错误
    CLOSED_CONNECTION                               //客户端已经关闭连接了
};

//简化问题，不给客户发送完整的HTTP应答报文，只是根据服务器的处理结果发送如下信息
static const char* szret[] = { "I get a corrent result\n", "Something wrong\n" };

//从状态机，用于解析出一行内容
LINE_STATUS parse_line(char *buffer, int &checked_index, int &read_index)
{
    //checked_index指向buffer(应用程序的读缓冲区)中当前正在分析的字节，read_index指向
    //buffer中客户数据的尾部的下一字节。buffer中第0~checked_index字节都已经分析完毕，
    //第checked_index~(read_index-1)字节由下面的循环逐个分析
    for (char temp; checked_index < read_index; ++checked_index) {

        //获得当前要分析的字节
        temp = buffer[checked_index];

        //如果当前的字节是"\r"，即回车符，则说明可能读取到一个完整的行
        if (temp == '\r') {

            //如果"\r"字符恰巧是目前buffer中的最后一个已经被读入的客户数据，
            //那么这次分析没有读取到一个完整的行，返回LINE_OPEN表示还需要继续
            //读取客户数据才能进一步分析
            if ((checked_index + 1) == read_index) {
                return LINE_OPEN;
            }
            //如果下一个字符是'\n'，则说明成功读取到一个完整的行
            else if (buffer[checked_index + 1] == '\n') {
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }

            //说明客户发送的HTTP请求存在语法问题
            return LINE_BAD;
        }
        //如果当前的字节是"\n"，即换行符，则也说明可能读取到一个完整的行
        else if (temp == '\n') {
            if ((checked_index > 1) && buffer[checked_index - 1] == '\r') {
                buffer[checked_index-1] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }

            return LINE_BAD;
        }
    }

    //如果所有内容都分析完毕也没有遇到"\r"字符，则返回LINE_OPEN，表示
    //还需要继续读取客户数据才能进一步分析
    return LINE_OPEN;
}

//分析请求行
HTTP_CODE parse_requestline(char *temp, CHECK_STATE &checkstate)
{
    char *url = strpbrk(temp, " \t");               //从s1中找出最先含有搜索字符串s2中任一字符的位置并返回
    if (!url) {                                     //如果请求行中没有空白字符或"\t"字符，则HTTP请求必有问题
        return BAD_REQUEST;
    }

    *url++ = '\0';
    char *method = temp;
    if (strcasecmp(method, "GET") == 0) {           //仅支持GET方法
        printf("The request method is GET\n");
    }
    else {
        return BAD_REQUEST;
    }

    url += strspn(url, " \t");
    char *version = strpbrk(url, " \t");
    if (!version) {
        return BAD_REQUEST;
    }

    *version++ = '\0';
    version += strspn(version, " \t");
    if (strcasecmp(version, "HTTP/1.1") != 0) {      //仅支持HTTP/1.1
        return BAD_REQUEST;
    }

    if (strncasecmp(url, "http://", 7) == 0) {      //检查URL是否合法
        url += 7;
        url = strchr(url, '/');
    }

    if (!url || url[0] != '/') {
        return BAD_REQUEST;
    }

    printf("The request URL is : %s\n", url);
    checkstate = CHECK_STATE_HEADER;                //HTTP请求行处理完毕，状态转移到头部字段的分析
    return NO_REQUEST;
}

//分析头部字段
HTTP_CODE parse_headers(char *temp)
{
    if (temp[0] == '\0') {                          //遇到一个空行，说明得到了一个正确的HTTP请求
        return GET_REQUEST;
    }
    else if (strncasecmp(temp, "Host:", 5) == 0) {  //处理HOST头部字段
        temp += 5;
        temp += strspn(temp, " \t");
        printf("the request host is: %s\n", temp);
    }
    else {
        printf("I can not handle this header\n");
    }

    return NO_REQUEST;
}

//分析HTTP请求的入口函数
HTTP_CODE parse_content(char *buffer, int &checked_index, CHECK_STATE &checkstate, int &read_index, int &start_line)
{
    LINE_STATUS linestatus = LINE_OK;                       //记录当前行的读取状态
    HTTP_CODE retcode = NO_REQUEST;                         //记录HTTP请求的处理结果

    //主状态机，用于从buffer中取出所有完整的行
    while ((linestatus = parse_line(buffer, checked_index, read_index)) == LINE_OK) {
        char *temp = buffer + start_line;                   //start_line是行在buffer中的起始位置
        start_line = checked_index;                         //记录下一行的起始位置

        switch (checkstate)                                 //checkstate记录主状态机当前的状态
        {
        case CHECK_STATE_REQUESTLINE:                       //第一个状态，分析请求行
        {
            retcode = parse_requestline(temp, checkstate);
            if (retcode == BAD_REQUEST) {
                return BAD_REQUEST;
            }
            break;
        }

        case CHECK_STATE_HEADER:                            //第二个状态，分析头部字段
        {
            retcode = parse_headers(temp);
            if (retcode == BAD_REQUEST) {
                return BAD_REQUEST;
            }
            else if (retcode == GET_REQUEST) {
                return GET_REQUEST;
            }
            break;
        }

        default:
            return INTERNAL_ERROR;
        }
    }

    if (linestatus == LINE_OPEN) {      //若没有读取到一个完整的行，则表示还需要继续读取客户数据才能进一步分析
        return NO_REQUEST;
    }
    else {
        return BAD_REQUEST;
    }
}

void http_demo(int argc, char* argv[])
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
    assert(listenfd >= 0);                          //创建socket失败返回-1被设置errno

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
        printf("errno is : %d\n", errno);
    }
    else {

        const int BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];                           //读缓冲区
        memset(buffer, '\0', BUFFER_SIZE);

        int data_read = 0;                              //当前读到的数据量
        int read_index = 0;                                 //当前已经读取了多少字节的客户数据
        int checked_index = 0;                              //当前已经分析完了多少字节的客户数据
        int start_line = 0;                                 //行在buffer中的起始位置

        CHECK_STATE checkstate = CHECK_STATE_REQUESTLINE;   //设置主状态机的初始状态
        while (true) {                                      //循环读取客户数据并分析之

            data_read = static_cast<int>(recv(connfd, buffer + read_index, BUFFER_SIZE - read_index, 0));
            if (data_read == -1) {
                printf("reading failed\n");
                break;
            }
            else if (data_read == 0) {
                printf("remote client has closed the connection\n");
                break;
            }

            read_index += data_read;
            HTTP_CODE result = parse_content(buffer, checked_index, checkstate, read_index, start_line);

            if (result == NO_REQUEST) {                     //尚未得到一个完整的HTTP请求
                continue;
            }
            else if (result == GET_REQUEST) {               //得到一个完整的正确的HTTP请求
                send(connfd, szret[0], strlen(szret[0]), 0);
                break;
            }
            else {                                          //发生错误
                send(connfd, szret[1], strlen(szret[1]), 0);
                break;
            }
        }

        close(connfd);
    }

    close(listenfd);
}
