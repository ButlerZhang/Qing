#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ֽ���
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <netinet/in.h>
//unsigned long int htonl(unsigned long int hostlong);    //�������͵������ֽ�������ת���������ֽ�������
//unsigned short int htons(unsigned short int hostshort); //�������͵������ֽ�������ת���������ֽ�������
//unsigned long int ntohl(unsigned long int netlong);     //�������ֽ�������ת���ɳ����͵������ֽ�������
//unsigned short int ntohs(unsigned short int netshort);  //�������ֽ�������ת���ɶ����͵������ֽ�������

//����˵����
//1.htonl��ʾ"host to network long"��s��ʾshort��
//2.�����ͺ���ͨ������ת��IP��ַ�������ͺ���ͨ������ת���˿ںš�
//3.�κθ�ʽ��������ͨ�����紫��ʱ����Ӧ��ʹ����Щ������ת���ֽ���

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//socket��ַ
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <bits/socket.h>
//struct sockaddr                                     //ͨ��socket��ַ
//{
//    sa_family_t sa_family;                          //��ַ�����ͣ�AF_UNIX/AF_INET/AF_INET6
//    char sa_data[14];                               //socket��ֵַ����ͬ��Э�����ֵַ����ͳ��Ⱦ�����ͬ
//};

//struct sockaddr_storage                             //sockaddr��ǿ�棬�����ɸ����ַ
//{
//    sa_family_t sa_family;                          //��ַ������
//    unsigned long int __ss_align;                   //���ڶ���
//    char __ss_padding[128 - sizeof(__ss_align)];    //socket��ֵַ
//};

//#include <sys/un.h>
//struct sockaddr_un                                  //UNIX������ר��socket��ַ�ṹ��
//{
//    sa_family_t sin_family;                         //��ַ�壺AF_UNIX
//    char sun_path[108];                             //�ļ�·����
//};

//struct sockaddr_in                                  //IPv4ר��
//{
//    sa_family_t sin_family;                         //��ַ�壬AF_INET
//    u_int16_t   sin_port;                           //�˿ںţ�Ҫ�������ֽ����ʾ
//    struct in_addr sin_addr;                        //IPv4��ַ�ṹ��
//};

//struct in_addr { u_int32_t s_addr; };               //IPv4��ַ��Ҫ�������ֽ����ʾ

//struct sockaddr_in6                                 //IPv6ר��
//{
//    sa_family_t sin6_family;                        //��ַ�壬AF_INET6
//    u_int16_t   sin_port;                           //�˿ںţ�Ҫ�������ֽ����ʾ
//    u_int32_t   sin6_flowinfo;                      //��Ϣ��������Ϊ0
//    struct in6_addr sin6_addr;                      //IPv6��ַ�ṹ��
//    u_int32_t   sin6_scope_id;                      //scope ID������׶�
//};

//struct in6_addr { unsigned char sa_addr[16]; };     //IPv6��ַ��Ҫ�������ֽ����ʾ

//����˵����
//����ר��socket��ַ��Ҫת����ͨ��socket��ַsockaddr��ǿ��ת�����ɣ�����Ϊsocket��̽ӿ�ʹ��ͨ�õ�ַ��

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//IP��ַת��
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <arpa/inet.h>

//���ܣ������ʮ�����ַ�����ʾ��IPv4��ַת��Ϊ�����ֽ���������ʾ��IPv4��ַ��
//���أ�ʧ��ʱ����INADDR_NONE��
//in_addr_t inet_addr(const char* strptr);

//���ܣ���inet_addrһ�������ǽ�����洢��inpָ��ĵ�ַ�ṹ�С�
//���أ�ʧ�ܷ���0���ɹ�����1��
//int inet_aton(const char* cp, struct in_addr* inp);

//���ܣ��������ֽ���������ʾ��IPv4��ַת��Ϊ���ʮ�����ַ�����ʾ��IPv4��ַ��
//ȱ�㣺�ڲ����þ�̬�����洢ת���������˲������롣
//char* inet_ntoa(struct in_addr in);

//���ܣ����ַ�����ʾ��IP��ַsrcת���������ֽ���������ʾ��IP��ַ������ת������洢��dst�ڴ��С�
//      ���У�afָ����ַ�壬������AF_INET��AF_INET6��
//���أ��ɹ�ʱ����1��ʧ�ܷ���0��������errno��
//int inet_pton(int af, const char* src, void *dst);

//���ܣ��������ֽ���������ʾ��IP��ַת�����ַ�����ʾ��IP��ַ��
//const char* inet_ntop(int af, const void* src, char *dst, socklen_t cnt);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UDP���ݶ�д
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <sys/types.h>
//#include <sys/socket.h>
//
//ssize_t recvfrom(                       //��ȡsockfd�ϵ�����
//    int sockfd,                         //Ҫ��ȡ��socket
//    void *buf,                          //������
//    size_t len,                         //��������С
//    int flags,                          //��socket��recv��ͬ���ṩ����Ŀ��ƣ�Ĭ��Ϊ0
//    struct sockaddr* src_addr,          //���Ͷ˵�socket��ַ
//    socklen_t *addrlen);                //���Ͷ�socket��ַ�ĳ���
//
//ssize_t sendto(                         //��sockfdд������
//    int sockfd,                         //Ҫд���socket
//    void *buf,                          //������
//    size_t len,                         //��������С
//    int flags,                          //��socket��send��ͬ���ṩ����Ŀ��ƣ�Ĭ��Ϊ0
//    const struct sockaddr* dest_addr,   //���ն�socket��ַ
//    socklen_t addrlen);                 //���ն�socket��ַ�ĳ���

//����˵����
//���socket��ַ��ΪNULL��������Ϊ0����recvfrom��Ϊrecv��sendto��Ϊsend��

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ͨ�����ݶ�д���ȿ�������UDPҲ��������TCP
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <sys/types.h>
//#include <sys/socket.h>

//ssize_t recvmsg(                          //��������
//    int sockfd,                           //�������ݵ�socket
//    struct msghdr *msg,                   //����ʵ�ַ�ɢ�������ݽṹ
//    int flags);                           //��recv��flags����һ��

//ssize_t sendmsg(                          //��������
//    int sockfd,                           //�������ݵ�socket
//    struct msghdr* msg,                   //����ʵ�ּ���д�����ݽṹ
//    int flags);                           //��send��flags����һ��

//struct msghdr
//{
//    void *msg_name;                       //ͨ�ŶԷ���socket��ַ�������TCP��ΪNULL
//    socklen_t msg_namelen;                //socket��ַ�ĳ���
//    struct iovec* msg_iov;                //��ɢ���ڴ��
//    int msg_iovlen;                       //��ɢ���ڴ�������
//    void* msg_control;                    //ָ�������ݵ���ʼλ��
//    socklen_t msg_controllen;             //�������ݵĴ�С
//    int msg_flags;                        //���ƺ����е�flags���������ڵ��ù����и���
//};

//struct iovec
//{
//    void *iov_base;                       //�ڴ���ʼ��ַ
//    size_t iov_len;                       //����ڴ�ĳ���
//};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//������
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <sys/socket.h>
//���ܣ��ж�sockfd�Ƿ��ڴ����ǣ�����һ������ȡ���������Ƿ��Ǵ������ݣ�OOB����
//���أ������OOB������1�����򷵻�0��
//int sockatmark(int sockfd);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��ַ��Ϣ����
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <sys/socket.h>
//
//int getsockname(                          //��ȡsockfd��Ӧ�ı���socket��ַ
//    int sockfd,                           //Ŀ��socket
//    struct sockaddr* address,             //socket��ַ
//    socklen_t* address_len);              //��ַ����
//                                          //�ɹ�����0��ʧ�ܷ���-1������errno
//
//int getpeername(                          //��ȡsockfd��Ӧ��Զ��socket��ַ
//    int sockfd,                           //Ŀ��socket
//    struct sockaddr* address,             //socket��ַ
//    socklen_t* address_len);              //��ַ����
//                                          //�ɹ�����0��ʧ�ܷ���-1������errno

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//������ϢAPI
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <netdb.h>
//struct hostent* gethostbyname(          //�����������ƻ�ȡ������������Ϣ
//    const char* name);                  //��������

//struct hostent* gethostbyaddr(          //����IP��ַ��ȡ������������Ϣ
//    const void* addr,                   //IP��ַ
//    size_t len,                         //IP��ַ�ĳ���
//    int type);                          //AF_INET����AF_INET6

//struct hostent                          //������������Ϣ
//{
//    char *h_name;                       //������
//    char **h_aliases;                   //���������б������ж��
//    int h_addrtype;                     //��ַ���ͣ���ַ�壩
//    int h_length;                       //��ַ����
//    char **h_addr_list;                 //�������ֽ����г�������IP��ַ�б�
//};

//struct servent* getservbyname(          //�������ƻ�ȡĳ�������������Ϣ
//    const char* name,                   //Ŀ����������
//    const char* proto);                 //�������ͣ�����"tcp"��"udp"
//
//struct servent* getservbyport(          //���ݶ˿ںŻ�ȡĳ�������������Ϣ
//    int port,                           //�˿ں�
//    const char* proto);                 //�������ͣ�����"tcp"��"udp"

//struct servent
//{
//    char *s_name;                       //��������
//    char **s_aliases;                   //����ı����б������ж��
//    int s_port;                         //�˿ں�
//    char *s_proto;                      //�������ͣ�ͨ����tcp��udp
//};

//����˵����
//�����ĸ��������ǲ�������汾���ں�����ǰ�����_r�����õľ��ǿ�����汾��

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ܵ�
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>

//���ܣ������ܵ���ʵ�ֽ��̼�ͨ�š�f[0]ֻ����f[1]ֻд���ܵ������Ĵ�СĬ����65536�ֽڡ�
//���أ������ɹ�����0�����򷵻�-1������errno��
//int pipe(int fd[2]);
//
//#include <sys/types.h>
//#include <sys/socket.h>
//int socketpair(                 //����˫��ܵ�
//    int domain,                 //ֻ�ܴ���AF_UNIX
//    int type,                   //�������ͣ���socketһ��
//    int protocol,               //����0����socketһ��
//    int fd[2]);                 //�ܵ�

