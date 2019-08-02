#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//字节序
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <netinet/in.h>
//unsigned long int htonl(unsigned long int hostlong);    //将长整型的主机字节序数据转换成网络字节序数据
//unsigned short int htons(unsigned short int hostshort); //将短整型的主机字节序数据转换成网络字节序数据
//unsigned long int ntohl(unsigned long int netlong);     //将网络字节序数据转换成长整型的主机字节序数据
//unsigned short int ntohs(unsigned short int netshort);  //将网络字节序数据转换成短整型的主机字节序数据

//其它说明：
//1.htonl表示"host to network long"。s表示short。
//2.长整型函数通常用来转换IP地址，短整型函数通常用来转换端口号。
//3.任何格式化的数据通过网络传输时，都应该使用这些函数来转换字节序。

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//socket地址
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <bits/socket.h>
//struct sockaddr                                     //通用socket地址
//{
//    sa_family_t sa_family;                          //地址族类型，AF_UNIX/AF_INET/AF_INET6
//    char sa_data[14];                               //socket地址值，不同的协议族地址值含义和长度均不相同
//};

//struct sockaddr_storage                             //sockaddr增强版，能容纳更多地址
//{
//    sa_family_t sa_family;                          //地址族类型
//    unsigned long int __ss_align;                   //用于对齐
//    char __ss_padding[128 - sizeof(__ss_align)];    //socket地址值
//};

//#include <sys/un.h>
//struct sockaddr_un                                  //UNIX本地域专用socket地址结构体
//{
//    sa_family_t sin_family;                         //地址族：AF_UNIX
//    char sun_path[108];                             //文件路径名
//};

//struct sockaddr_in                                  //IPv4专用
//{
//    sa_family_t sin_family;                         //地址族，AF_INET
//    u_int16_t   sin_port;                           //端口号，要用网络字节序表示
//    struct in_addr sin_addr;                        //IPv4地址结构体
//};

//struct in_addr { u_int32_t s_addr; };               //IPv4地址，要用网络字节序表示

//struct sockaddr_in6                                 //IPv6专用
//{
//    sa_family_t sin6_family;                        //地址族，AF_INET6
//    u_int16_t   sin_port;                           //端口号，要用网络字节序表示
//    u_int32_t   sin6_flowinfo;                      //信息流，设置为0
//    struct in6_addr sin6_addr;                      //IPv6地址结构体
//    u_int32_t   sin6_scope_id;                      //scope ID，试验阶段
//};

//struct in6_addr { unsigned char sa_addr[16]; };     //IPv6地址，要用网络字节序表示

//其它说明：
//所有专用socket地址都要转化成通用socket地址sockaddr（强制转换即可），因为socket编程接口使用通用地址。

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//IP地址转换
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <arpa/inet.h>

//功能：将点分十进制字符串表示的IPv4地址转换为网络字节序整数表示的IPv4地址。
//返回：失败时返回INADDR_NONE。
//in_addr_t inet_addr(const char* strptr);

//功能：与inet_addr一样，但是将结果存储在inp指向的地址结构中。
//返回：失败返回0，成功返回1。
//int inet_aton(const char* cp, struct in_addr* inp);

//功能：将网络字节序整数表示的IPv4地址转换为点分十进制字符串表示的IPv4地址。
//缺点：内部采用静态变量存储转化结果，因此不可重入。
//char* inet_ntoa(struct in_addr in);

//功能：将字符串表示的IP地址src转换成网络字节序整数表示的IP地址，并把转换结果存储在dst内存中。
//      其中，af指定地址族，可以是AF_INET或AF_INET6。
//返回：成功时返回1，失败返回0，并设置errno。
//int inet_pton(int af, const char* src, void *dst);

//功能：将网络字节序整数表示的IP地址转换成字符串表示的IP地址。
//const char* inet_ntop(int af, const void* src, char *dst, socklen_t cnt);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UDP数据读写
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <sys/types.h>
//#include <sys/socket.h>
//
//ssize_t recvfrom(                       //读取sockfd上的数据
//    int sockfd,                         //要读取的socket
//    void *buf,                          //缓冲区
//    size_t len,                         //缓冲区大小
//    int flags,                          //与socket的recv相同，提供额外的控制，默认为0
//    struct sockaddr* src_addr,          //发送端的socket地址
//    socklen_t *addrlen);                //发送端socket地址的长度
//
//ssize_t sendto(                         //向sockfd写入数据
//    int sockfd,                         //要写入的socket
//    void *buf,                          //缓冲区
//    size_t len,                         //缓冲区大小
//    int flags,                          //与socket的send相同，提供额外的控制，默认为0
//    const struct sockaddr* dest_addr,   //接收端socket地址
//    socklen_t addrlen);                 //接收端socket地址的长度

//其它说明：
//如果socket地址设为NULL，长度设为0，则recvfrom变为recv，sendto变为send。

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//通用数据读写，既可以用于UDP也可以用于TCP
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <sys/types.h>
//#include <sys/socket.h>

//ssize_t recvmsg(                          //接收数据
//    int sockfd,                           //接收数据的socket
//    struct msghdr *msg,                   //可以实现分散读的数据结构
//    int flags);                           //与recv的flags含义一致

//ssize_t sendmsg(                          //发送数据
//    int sockfd,                           //发送数据的socket
//    struct msghdr* msg,                   //可以实现集中写的数据结构
//    int flags);                           //与send的flags含义一致

//struct msghdr
//{
//    void *msg_name;                       //通信对方的socket地址，如果是TCP设为NULL
//    socklen_t msg_namelen;                //socket地址的长度
//    struct iovec* msg_iov;                //分散的内存块
//    int msg_iovlen;                       //分散的内存块的数量
//    void* msg_control;                    //指向辅助数据的起始位置
//    socklen_t msg_controllen;             //辅助数据的大小
//    int msg_flags;                        //复制函数中的flags参数，并在调用过程中更新
//};

//struct iovec
//{
//    void *iov_base;                       //内存起始地址
//    size_t iov_len;                       //这块内存的长度
//};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//带外标记
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <sys/socket.h>
//功能：判断sockfd是否处于带外标记，即下一个被读取到的数据是否是带外数据（OOB）。
//返回：如果是OOB，返回1；否则返回0。
//int sockatmark(int sockfd);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//地址信息函数
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <sys/socket.h>
//
//int getsockname(                          //获取sockfd对应的本端socket地址
//    int sockfd,                           //目标socket
//    struct sockaddr* address,             //socket地址
//    socklen_t* address_len);              //地址长度
//                                          //成功返回0，失败返回-1并设置errno
//
//int getpeername(                          //获取sockfd对应的远端socket地址
//    int sockfd,                           //目标socket
//    struct sockaddr* address,             //socket地址
//    socklen_t* address_len);              //地址长度
//                                          //成功返回0，失败返回-1并设置errno

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//网络信息API
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <netdb.h>
//struct hostent* gethostbyname(          //根据主机名称获取主机的完整信息
//    const char* name);                  //主机名称

//struct hostent* gethostbyaddr(          //根据IP地址获取主机的完整信息
//    const void* addr,                   //IP地址
//    size_t len,                         //IP地址的长度
//    int type);                          //AF_INET还是AF_INET6

//struct hostent                          //主机的完整信息
//{
//    char *h_name;                       //主机名
//    char **h_aliases;                   //主机别名列表，可能有多个
//    int h_addrtype;                     //地址类型（地址族）
//    int h_length;                       //地址长度
//    char **h_addr_list;                 //按网络字节序列出的主机IP地址列表
//};

//struct servent* getservbyname(          //根据名称获取某个服务的完整信息
//    const char* name,                   //目标服务的名字
//    const char* proto);                 //服务类型，例如"tcp"、"udp"
//
//struct servent* getservbyport(          //根据端口号获取某个服务的完整信息
//    int port,                           //端口号
//    const char* proto);                 //服务类型，例如"tcp"、"udp"

//struct servent
//{
//    char *s_name;                       //服务名称
//    char **s_aliases;                   //服务的别名列表，可能有多个
//    int s_port;                         //端口号
//    char *s_proto;                      //服务类型，通常是tcp或udp
//};

//其它说明：
//上面四个函数都是不可重入版本，在函数名前面添加_r，调用的就是可重入版本。

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//管道
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <unistd.h>

//功能：创建管道，实现进程间通信。f[0]只读，f[1]只写。管道容量的大小默认是65536字节。
//返回：函数成功返回0，否则返回-1并设置errno。
//int pipe(int fd[2]);
//
//#include <sys/types.h>
//#include <sys/socket.h>
//int socketpair(                 //创建双向管道
//    int domain,                 //只能传入AF_UNIX
//    int type,                   //服务类型，跟socket一致
//    int protocol,               //传入0，跟socket一致
//    int fd[2]);                 //管道

