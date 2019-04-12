#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>



void change_tcp_buffer_size_client(int argc, char* argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip_address port_number send buffer size\n",
            basename(argv[0]));                 //basename��ͷ�ļ���<string.h>����ȡ·�������һ��/���������
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    struct sockaddr_in server_address;                 //sockaddr_inר����IPv4��ַ��sockaddr_in6ר����IPv6��ַ
    bzero(&server_address, sizeof(server_address));    //��server_address��ʼ��Ϊ0
    server_address.sin_family = AF_INET;               //��ַ�壬AF_INET=PF_INET��AF_INET6=PF_INET6��AF_UNIX=PF_UNIX
    inet_pton(AF_INET, ip, &server_address.sin_addr);  //���ַ���IP��ַת�����������ֽ���������ʾ��IP��ַ
    server_address.sin_port = htons(port);             //�������ֽ���ת���������ֽ���

    int sock = socket(                          //����socket
        PF_INET,                                //�ײ�Э���壬PF_INET��ʾIPv4��PF_INET6��ʾIPv6��PF_UNIX��ʾUNIX������Э��
        SOCK_STREAM,                            //�������ͣ�SOCK_STREAM��ʾTCP��SOCK_DGRAM��ʾUDP
        0);                                     //��ǰ�����������ɵ�Э�鼯���£���ѡ��һ�������Э�飬ͨ����Ϊ0����ʾĬ��
    assert(sock >= 0);                          //����socketʧ�ܷ���-1������errno

    int sendbuf = atoi(argv[3]);
    int len = sizeof(sendbuf);

    setsockopt(                                 //����socket����
        sock,                                   //Ҫ���õ�socket
        SOL_SOCKET,                             //Ҫ�����ĸ�Э��(SOL_SOCKET/IPPROTO_IP/IPPROTO_IPV6/IPPROTO_TCP)������
        SO_SNDBUF,                              //���Ե�����
        &sendbuf,                               //���Ե���ֵ
        sizeof(sendbuf));                       //������ֵ�ĳ���

    getsockopt(                                 //��ȡsocket����
        sock,                                   //Ҫ��ȡ��socket
        SOL_SOCKET,                             //Ҫ�����ĸ�Э��(SOL_SOCKET/IPPROTO_IP/IPPROTO_IPV6/IPPROTO_TCP)������
        SO_SNDBUF,                              //���Ե�����
        &sendbuf,                               //���Ե�ֵ
        (socklen_t*)&len);                      //���Ե�ֵ�ĳ���

    printf("TCP send buffer size after setting is %d\n", sendbuf);

    if (connect(                                //�ͻ��˷���Է�����������
        sock,                                   //�ͻ��˵�socket
        (struct sockaddr*)&server_address,      //����˵�ַ
        sizeof(server_address)) != -1) {        //����˵�ַ����

        const int BUFFER_SIZE = 512;
        char buffer[BUFFER_SIZE];
        memset(buffer, 'a', BUFFER_SIZE);
        send(sock, buffer, BUFFER_SIZE, 0);
    }

    close(sock);                                //�ر�socket�����������رգ����Ǽ������ü���
                                                //�����Ҫ�����رգ����Ե���shutdown����
}
