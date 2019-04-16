#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>



void select_demo(int argc, char* argv[])
{
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n",
            basename(argv[0]));                 //basename��ͷ�ļ���<string.h>����ȡ·�������һ��/���������
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    struct sockaddr_in address;                 //sockaddr_inר����IPv4��ַ��sockaddr_in6ר����IPv6��ַ
    bzero(&address, sizeof(address));           //��address��ʼ��Ϊ0
    address.sin_family = AF_INET;               //��ַ�壬AF_INET=PF_INET��AF_INET6=PF_INET6��AF_UNIX=PF_UNIX
    inet_pton(AF_INET, ip, &address.sin_addr);  //���ַ���IP��ַת�����������ֽ���������ʾ��IP��ַ
    address.sin_port = htons(port);             //�������ֽ���ת���������ֽ���

    int listenfd = socket(                      //����socket
        PF_INET,                                //�ײ�Э���壬PF_INET��ʾIPv4��PF_INET6��ʾIPv6��PF_UNIX��ʾUNIX������Э��
        SOCK_STREAM,                            //�������ͣ�SOCK_STREAM��ʾTCP��SOCK_DGRAM��ʾUDP
        0);                                     //��ǰ�����������ɵ�Э�鼯���£���ѡ��һ�������Э�飬ͨ����Ϊ0����ʾĬ��Э��
    assert(listenfd >= 0);                      //����socketʧ�ܷ���-1������errno

    int ret = bind(                             //��socket��Ҳ��Ϊ����socket�������ڷ����
        listenfd,                               //Ҫ�󶨵�socket
        (struct sockaddr*)&address,             //Ҫ�󶨵ĵ�ַ
        sizeof(address));                       //��ַ����
    assert(ret != -1);                          //�󶨳ɹ�����0��ʧ�ܷ���-1������errno

    ret = listen(                               //������������
        listenfd,                               //Ҫ������socket
        5);                                     //�ں˼������е���󳤶ȣ�����ֵΪ5
    assert(ret != -1);                          //�ɹ�����0��ʧ�ܷ���-1������errno

    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);
    int connfd = accept(                        //���ܿͻ��˵�����
        listenfd,                               //����socket
        (struct sockaddr*)&client_address,      //Զ��socket��ַ
        &client_addrlength);                    //Զ��socket��ַ�ĳ���
                                                //�ɹ�ʱ���������ӵ�socket��ʧ�ܷ���-1������errno

    if (connfd < 0) {
        printf("errno is : %d\n", errno);       //��ӡ�����ԭ��
    }
    else {

        const int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];

        fd_set read_fds;
        fd_set exception_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&exception_fds);

        while (true)
        {
            memset(buffer, '\0', BUFFER_SIZE);

            FD_SET(connfd, &read_fds);
            FD_SET(connfd, &exception_fds);

            int ret = select(                   //��ָ��ʱ���������û�����Ȥ���ļ��������ϵĿɶ���д���쳣�¼�
                connfd + 1,                     //ָ�����������ļ���������������ͨ������Ϊ�����ļ��������е����ֵ��1
                &read_fds,                      //�ɶ��ļ�����������
                NULL,                           //��д�ļ�����������
                &exception_fds,                 //�쳣�¼��ļ�����������
                NULL);                          //��ʱʱ�䣬NULL��ʾһֱ������ֱ��ĳ���ļ�����������
            if (ret < 0) {
                printf("selection failure\n");
                break;
            }
            else {
                printf("current socket connfd = %d\n", connfd);
            }

            if (FD_ISSET(connfd, &read_fds)) {                              //���ڿɶ��¼���������ͨ��recv������ȡ����
                ssize_t recv_ret = recv(connfd, buffer, BUFFER_SIZE - 1, 0);
                if (recv_ret <= 0) {
                    break;
                }
                else {
                    printf("get %d bytes of normal data: %s\n", recv_ret, buffer);
                }
            }
            else if (FD_ISSET(connfd, &exception_fds)) {                    //�����쳣�¼������ô�MSG_OOB��־��recv������ȡ��������
                ssize_t recv_ret = recv(connfd, buffer, BUFFER_SIZE - 1, MSG_OOB);
                if (recv_ret <= 0) {
                    break;
                }
                else {
                    printf("get %d bytes of oob data: %s\n", recv_ret, buffer);
                }
            }
        }

        close(connfd);
    }

    close(listenfd);                            //�ر�socket�����������رգ����Ǽ������ü���
                                                //�����Ҫ�����رգ����Ե���shutdown����
}
