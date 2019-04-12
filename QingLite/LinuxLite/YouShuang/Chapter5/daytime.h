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

    //��ȡĿ��������ַ��Ϣ
    //gethostbyname�����������ƻ�ȡ������������Ϣ
    //gethostbyaddr����IP��ַ��ȡ������������Ϣ
    struct hostent *hostinfo = gethostbyname(host);
    assert(hostinfo);

    //��ȡdaytime��������Ϣ
    //getservbyname�������ƻ�ȡĳ�������������Ϣ
    //getservbyport���ݶ˿ںŻ�ȡĳ�������������Ϣ
    struct servent *servinfo = getservbyname(
        "daytime",                              //���������
        "tcp");                                 //���������
    assert(servinfo);

    printf("daytime port is %d\n", ntohs(static_cast<uint16_t>(servinfo->s_port)));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = static_cast<uint16_t>(servinfo->s_port);
    //h_addr_list������ʹ�������ֽ���ĵ�ַ�б���������ת��
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
