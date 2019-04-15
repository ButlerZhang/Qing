#pragma once



//#include <netinet/in.h>
//unsigned long int htonl(unsigned long int hostlong);    //将长整型的主机字节序数据转换成网络字节序数据
//unsigned short int htons(unsigned short int hostshort); //将短整型的主机字节序数据转换成网络字节序数据
//unsigned long int ntohl(unsigned long int netlong);     //将网络字节序数据转换成长整型的主机字节序数据
//unsigned short int ntohs(unsigned short int netshort);  //将网络字节序数据转换成短整型的主机字节序数据

//其它说明：
//1.htonl表示"host to network long"。s表示short。
//2.长整型函数通常用来转换IP地址，短整型函数通常用来转换端口号。
//3.任何格式化的数据通过网络传输时，都应该使用这些函数来转换字节序。
