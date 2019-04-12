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

//SIGTERM�źŴ�����������ʱ�����������е�ѭ��
static void handle_term(int sig)
{
    g_stop = true;
}

void blacklog_test(int argc, char* argv[])
{
    signal(SIGTERM, handle_term);               //ע���źŴ�����

    if (argc < 3) {
        printf("usage: %s ip_address port_number backlog\n",
            basename(argv[0]));                 //basename��ͷ�ļ���<string.h>����ȡ·�������һ��/���������
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    int backlog = atoi(argv[3]);

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
        backlog);                               //�ں˼������е���󳤶ȣ�����backlog�����������������������
    assert(ret != -1);                          //�ɹ�����0��ʧ�ܷ���-1������errno

    while (!g_stop) {
        sleep(1);
    }

    close(sock);                                //�ر�socket�����������رգ����Ǽ������ü���
                                                //�����Ҫ�����رգ����Ե���shutdown����
}
