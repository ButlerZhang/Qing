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



void oob_data_server(int argc, char* argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n",
            basename(argv[0]));                 //basename��ͷ�ļ���<string.h>����ȡ·�������һ��/���������
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    int sock = socket(                          //����socket
        PF_INET,                                //�ײ�Э���壬PF_INET��ʾIPv4��PF_INET6��ʾIPv6��PF_UNIX��ʾUNIX������Э��
        SOCK_STREAM,                            //�������ͣ�SOCK_STREAM��ʾTCP��SOCK_DGRAM��ʾUDP
        0);                                     //��ǰ�����������ɵ�Э�鼯���£���ѡ��һ�������Э�飬ͨ����Ϊ0����ʾĬ��
    assert(sock >= 0);                          //����socketʧ�ܷ���-1������errno

    struct sockaddr_in address;                 //sockaddr_inר����IPv4��ַ��sockaddr_in6ר����IPv6��ַ
    bzero(&address, sizeof(address));           //��address��ʼ��Ϊ0
    address.sin_family = AF_INET;               //��ַ�壬AF_INET=PF_INET��AF_INET6=PF_INET6��AF_UNIX=PF_UNIX
    inet_pton(AF_INET, ip, &address.sin_addr);  //���ַ���IP��ַת�����������ֽ���������ʾ��IP��ַ
    address.sin_port = htons(port);             //�������ֽ���ת���������ֽ���

    int ret = bind(                             //��socket��Ҳ��Ϊ����socket�������ڷ����
        sock,                                   //Ҫ�󶨵�socket
        (struct sockaddr*)&address,             //Ҫ�󶨵ĵ�ַ
        sizeof(address));                       //��ַ����
    assert(ret != -1);                          //�󶨳ɹ�����0��ʧ�ܷ���-1������errno

    ret = listen(                               //������������
        sock,                                   //Ҫ������socket
        5);                                     //�ں˼������е���󳤶ȣ�����ֵΪ5
    assert(ret != -1);                          //�ɹ�����0��ʧ�ܷ���-1������errno

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(                        //���ܿͻ��˵�����
        sock,                                   //����socket
        (struct sockaddr*)&client,              //Զ��socket��ַ
        &client_addrlength);                    //Զ��socket��ַ�ĳ���
                                                //�ɹ�ʱ���������ӵ�socket��ʧ�ܷ���-1������errno

    if (connfd < 0) {
        printf("errno is : %d\n", errno);       //��ӡ�����ԭ��
    }
    else {

        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        size_t rece_ret;

        memset(buffer, '\0', BUFFER_SIZE);
        rece_ret = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
        printf("got %d bytes of normal data '%s'\n", rece_ret, buffer);

        memset(buffer, '\0', BUFFER_SIZE);
        rece_ret = recv(                        //��ȡsocket�ϵ����ݣ�����������
            connfd,                             //��ͻ������ӵ�socket
            buffer,                             //������
            BUFFER_SIZE - 1,                    //��������С
            MSG_OOB);                           //�ṩ����Ŀ��ƣ�MSG_OOB��ʾ��������
        printf("got %d bytes of oob data '%s'\n", rece_ret, buffer);

        memset(buffer, '\0', BUFFER_SIZE);
        rece_ret = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
        printf("got %d bytes of normal data '%s'\n", rece_ret, buffer);

        close(connfd);
    }

    close(sock);                                //�ر�socket�����������رգ����Ǽ������ü���
                                                //�����Ҫ�����رգ����Ե���shutdown����
}
