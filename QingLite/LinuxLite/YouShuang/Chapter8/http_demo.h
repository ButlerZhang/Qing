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



enum CHECK_STATE                                    //��״̬�������ֿ���״̬
{
    CHECK_STATE_REQUESTLINE = 0,                    //��ǰ���ڷ���������
    CHECK_STATE_HEADER                              //��ǰ���ڷ���ͷ���ֶ�
};

enum LINE_STATUS                                    //��״̬�������ֿ���״̬�����еĶ�ȡ״̬
{
    LINE_OK = 0,                                    //��ȡ��һ����������
    LINE_BAD,                                       //�г���
    LINE_OPEN                                       //���������Ҳ�����
};

enum HTTP_CODE                                      //����������HTTP����Ľ��
{
    NO_REQUEST,                                     //������������Ҫ������ȡ�ͻ�����
    GET_REQUEST,                                    //�����һ�������Ŀͻ�����
    BAD_REQUEST,                                    //�ͻ��������﷨����
    FORBIDDEN_REQUEST,                              //�ͻ�����Դû���㹻�ķ���Ȩ��
    INTERNAL_ERROR,                                 //�������ڲ�����
    CLOSED_CONNECTION                               //�ͻ����Ѿ��ر�������
};

//�����⣬�����ͻ�����������HTTPӦ���ģ�ֻ�Ǹ��ݷ������Ĵ���������������Ϣ
static const char* szret[] = { "I get a corrent result\n", "Something wrong\n" };

//��״̬�������ڽ�����һ������
LINE_STATUS parse_line(char *buffer, int &checked_index, int &read_index)
{
    //checked_indexָ��buffer(Ӧ�ó���Ķ�������)�е�ǰ���ڷ������ֽڣ�read_indexָ��
    //buffer�пͻ����ݵ�β������һ�ֽڡ�buffer�е�0~checked_index�ֽڶ��Ѿ�������ϣ�
    //��checked_index~(read_index-1)�ֽ��������ѭ���������
    for (char temp; checked_index < read_index; ++checked_index) {

        //��õ�ǰҪ�������ֽ�
        temp = buffer[checked_index];

        //�����ǰ���ֽ���"\r"�����س�������˵�����ܶ�ȡ��һ����������
        if (temp == '\r') {

            //���"\r"�ַ�ǡ����Ŀǰbuffer�е����һ���Ѿ�������Ŀͻ����ݣ�
            //��ô��η���û�ж�ȡ��һ���������У�����LINE_OPEN��ʾ����Ҫ����
            //��ȡ�ͻ����ݲ��ܽ�һ������
            if ((checked_index + 1) == read_index) {
                return LINE_OPEN;
            }
            //�����һ���ַ���'\n'����˵���ɹ���ȡ��һ����������
            else if (buffer[checked_index + 1] == '\n') {
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }

            //˵���ͻ����͵�HTTP��������﷨����
            return LINE_BAD;
        }
        //�����ǰ���ֽ���"\n"�������з�����Ҳ˵�����ܶ�ȡ��һ����������
        else if (temp == '\n') {
            if ((checked_index > 1) && buffer[checked_index - 1] == '\r') {
                buffer[checked_index-1] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }

            return LINE_BAD;
        }
    }

    //����������ݶ��������Ҳû������"\r"�ַ����򷵻�LINE_OPEN����ʾ
    //����Ҫ������ȡ�ͻ����ݲ��ܽ�һ������
    return LINE_OPEN;
}

//����������
HTTP_CODE parse_requestline(char *temp, CHECK_STATE &checkstate)
{
    char *url = strpbrk(temp, " \t");               //��s1���ҳ����Ⱥ��������ַ���s2����һ�ַ���λ�ò�����
    if (!url) {                                     //�����������û�пհ��ַ���"\t"�ַ�����HTTP�����������
        return BAD_REQUEST;
    }

    *url++ = '\0';
    char *method = temp;
    if (strcasecmp(method, "GET") == 0) {           //��֧��GET����
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
    if (strcasecmp(version, "HTTP/1.1") != 0) {      //��֧��HTTP/1.1
        return BAD_REQUEST;
    }

    if (strncasecmp(url, "http://", 7) == 0) {      //���URL�Ƿ�Ϸ�
        url += 7;
        url = strchr(url, '/');
    }

    if (!url || url[0] != '/') {
        return BAD_REQUEST;
    }

    printf("The request URL is : %s\n", url);
    checkstate = CHECK_STATE_HEADER;                //HTTP�����д�����ϣ�״̬ת�Ƶ�ͷ���ֶεķ���
    return NO_REQUEST;
}

//����ͷ���ֶ�
HTTP_CODE parse_headers(char *temp)
{
    if (temp[0] == '\0') {                          //����һ�����У�˵���õ���һ����ȷ��HTTP����
        return GET_REQUEST;
    }
    else if (strncasecmp(temp, "Host:", 5) == 0) {  //����HOSTͷ���ֶ�
        temp += 5;
        temp += strspn(temp, " \t");
        printf("the request host is: %s\n", temp);
    }
    else {
        printf("I can not handle this header\n");
    }

    return NO_REQUEST;
}

//����HTTP�������ں���
HTTP_CODE parse_content(char *buffer, int &checked_index, CHECK_STATE &checkstate, int &read_index, int &start_line)
{
    LINE_STATUS linestatus = LINE_OK;                       //��¼��ǰ�еĶ�ȡ״̬
    HTTP_CODE retcode = NO_REQUEST;                         //��¼HTTP����Ĵ�����

    //��״̬�������ڴ�buffer��ȡ��������������
    while ((linestatus = parse_line(buffer, checked_index, read_index)) == LINE_OK) {
        char *temp = buffer + start_line;                   //start_line������buffer�е���ʼλ��
        start_line = checked_index;                         //��¼��һ�е���ʼλ��

        switch (checkstate)                                 //checkstate��¼��״̬����ǰ��״̬
        {
        case CHECK_STATE_REQUESTLINE:                       //��һ��״̬������������
        {
            retcode = parse_requestline(temp, checkstate);
            if (retcode == BAD_REQUEST) {
                return BAD_REQUEST;
            }
            break;
        }

        case CHECK_STATE_HEADER:                            //�ڶ���״̬������ͷ���ֶ�
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

    if (linestatus == LINE_OPEN) {      //��û�ж�ȡ��һ���������У����ʾ����Ҫ������ȡ�ͻ����ݲ��ܽ�һ������
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
    assert(listenfd >= 0);                          //����socketʧ�ܷ���-1������errno

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
        printf("errno is : %d\n", errno);
    }
    else {

        const int BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];                           //��������
        memset(buffer, '\0', BUFFER_SIZE);

        int data_read = 0;                              //��ǰ������������
        int read_index = 0;                                 //��ǰ�Ѿ���ȡ�˶����ֽڵĿͻ�����
        int checked_index = 0;                              //��ǰ�Ѿ��������˶����ֽڵĿͻ�����
        int start_line = 0;                                 //����buffer�е���ʼλ��

        CHECK_STATE checkstate = CHECK_STATE_REQUESTLINE;   //������״̬���ĳ�ʼ״̬
        while (true) {                                      //ѭ����ȡ�ͻ����ݲ�����֮

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

            if (result == NO_REQUEST) {                     //��δ�õ�һ��������HTTP����
                continue;
            }
            else if (result == GET_REQUEST) {               //�õ�һ����������ȷ��HTTP����
                send(connfd, szret[0], strlen(szret[0]), 0);
                break;
            }
            else {                                          //��������
                send(connfd, szret[1], strlen(szret[1]), 0);
                break;
            }
        }

        close(connfd);
    }

    close(listenfd);
}
