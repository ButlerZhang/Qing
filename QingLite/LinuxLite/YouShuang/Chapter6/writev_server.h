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
            basename(argv[0]));                 //basename��ͷ�ļ���<string.h>����ȡ·�������һ��/���������
        return;
    }

    const char *ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    const char *file_name = argv[3];
    const char* status_line[2] = { "200 OK", "500 Internal server error" };

    int sock = socket(                          //����socket
        PF_INET,                                //�ײ�Э���壬PF_INET��ʾIPv4��PF_INET6��ʾIPv6��PF_UNIX��ʾUNIX������Э��
        SOCK_STREAM,                            //�������ͣ�SOCK_STREAM��ʾTCP��SOCK_DGRAM��ʾUDP
        0);                                     //��ǰ�����������ɵ�Э�鼯���£���ѡ��һ�������Э�飬ͨ����Ϊ0����ʾĬ��Э��
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
        printf("connect succeed\n");

        const int BUFFER_SIZE = 1024;
        char header_buf[BUFFER_SIZE];           //���ڱ���HTTPӦ���״̬�С�ͷ���ֶκ�һ�����еĻ�����
        memset(header_buf, '\0', BUFFER_SIZE);  //��ʼ��

        char *file_buf = NULL;                  //���ڴ��Ŀ���ļ����ݵ�Ӧ�ó��򻺴�
        struct stat file_stat;                  //���ڻ�ȡĿ���ļ������ԣ������Ƿ�ΪĿ¼���ļ��д�С��
        bool valid = true;                      //��¼Ŀ���ļ��Ƿ�����Ч�ļ�
        int len = 0;                            //������header_bufĿǰ�Ѿ�ʹ���˶����ֽڵĿռ�

        if (stat(file_name, &file_stat) < 0) {  //Ŀ���ļ�������
            valid = false;
        }
        else {
            if (S_ISDIR(file_stat.st_mode)) {   //Ŀ���ļ���һ��Ŀ¼
                valid = false;
            }
            else if (file_stat.st_mode & S_IROTH) { //��ǰ�û��ж�ȡĿ���ļ���Ȩ��

                //��̬���仺����file_buf����ָ�����СΪĿ���ļ��Ĵ�С
                //file_stat.st_size��1��Ȼ��Ŀ���ļ����뻺����file_buf��
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

            //���Ŀ���ļ���Ч������������HTTPӦ��
            if (valid) {

                //�����ⲿ�����ݽ�HTTPӦ���״̬�С�Content-Lengthͷ���ֶκ�һ���������μ���header_buf��
                ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[0]);

                len += ret;
                ret = snprintf(header_buf, BUFFER_SIZE - 1 - len, "Content-Length: %d\r\n", file_stat.st_size);

                len += ret;
                ret = snprintf(header_buf, BUFFER_SIZE - 1 - len, "%s", "\r\n");

                //����writev��header_buf��file_buf������һ��д��
                struct iovec iv[2];
                iv[0].iov_base = header_buf;
                iv[0].iov_len = strlen(header_buf);
                iv[1].iov_base = file_buf;
                iv[1].iov_len = file_stat.st_size;
                ssize_t writev_size = writev(connfd, iv, 2);
                printf("writev size = %d\n", writev_size);
            }
            else {                              //���Ŀ���ļ���Ч����֪ͨ�ͻ��˷������������ڲ�����

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

    close(sock);                                //�ر�socket�����������رգ����Ǽ������ü���
                                                //�����Ҫ�����رգ����Ե���shutdown����
}
