#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>



void daytime(int argc, char *argv[])
{
    assert(argc == 2);
    char *host = argv[1];

    //获取目标主机地址信息
    //gethostbyname根据主机名称获取主机的完整信息
    //gethostbyaddr根据IP地址获取主机的完整信息
    struct hostent *hostinfo = gethostbyname(host);
    assert(hostinfo);

    //获取daytime服务器信息
    //getservbyname根据名称获取某个服务的完整信息
    //getservbyport根据端口号获取某个服务的完整信息
    struct servent *servinfo = getservbyname(
        "daytime",                              //服务的名称
        "tcp");                                 //服务的类型
    assert(servinfo);

    printf("daytime port is %d\n", ntohs(static_cast<uint16_t>(servinfo->s_port)));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = static_cast<uint16_t>(servinfo->s_port);
    //h_addr_list本身是使用网络字节序的地址列表，所以无需转换
    address.sin_addr = *(struct in_addr*)*hostinfo->h_addr_list;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int result = connect(sockfd, (struct sockaddr*)&address, sizeof(address));
    assert(result != -1);

    char buffer[128];
    ssize_t read_result = read(sockfd, buffer, sizeof(buffer));
    assert(read_result > 0);
    buffer[read_result] = '\0';
    printf("day time is :%s", buffer);
    close(sockfd);
}
