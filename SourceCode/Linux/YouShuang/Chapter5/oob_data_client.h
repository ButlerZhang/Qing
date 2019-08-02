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

    int sockfd = socket(                        //����socket
        PF_INET,                                //�ײ�Э���壬PF_INET��ʾIPv4��PF_INET6��ʾIPv6��PF_UNIX��ʾUNIX������Э��
        SOCK_STREAM,                            //�������ͣ�SOCK_STREAM��ʾTCP��SOCK_DGRAM��ʾUDP
        0);                                     //��ǰ�����������ɵ�Э�鼯���£���ѡ��һ�������Э�飬ͨ����Ϊ0����ʾĬ��Э��
    assert(sockfd >= 0);                        //����socketʧ�ܷ���-1������errno

    if (connect(                                //�ͻ��˷���Է�����������
        sockfd,                                 //�ͻ��˵�socket
        (struct sockaddr*)&server_address,      //����˵�ַ
        sizeof(server_address)) < 0) {          //����˵�ַ����
        printf("Connection failed\n");          //�ɹ�����0��ʧ�ܷ���-1������errno
    }
    else {
        const char* oob_data = "abc";
        const char* normal_data = "123";
        send(sockfd, normal_data, strlen(normal_data), 0);

        send(                                   //��sockfd��д���ݣ�����������
            sockfd,                             //�ͻ��˵�socket
            oob_data,                           //���ݻ�����
            strlen(oob_data),                   //��������С
            MSG_OOB);                           //�ṩ����Ŀ��ƣ�MSG_OOB��ʾ��������

        send(sockfd, normal_data, strlen(normal_data), 0);
    }

    close(sockfd);                              //�ر�socket�����������رգ����Ǽ������ü���
                                                //�����Ҫ�����رգ����Ե���shutdown����
}
