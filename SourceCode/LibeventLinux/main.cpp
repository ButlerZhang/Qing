#include <iostream>
#include "src/demo1_client_read_write.h"
#include "src/demo1_server_read_write.h"



int main(int argc, char *argv[])
{
#ifdef _WIN32
    if (argc != 2)
    {
        WSADATA WSAData;
        WSAStartup(0x101, &WSAData);
    }
#endif

    std::cout << "Choose: " << std::endl;
    std::cout << "1.Server" << std::endl;
    std::cout << "2.Client" << std::endl;

    int Choose = 1;
    std::cin >> Choose;

    const char *ServerIP = "127.0.0.1";
    const int ServerPort = 9000;

    if (Choose == 1)
    {
        demo1_server_read_write(ServerIP, ServerPort);
    }
    else
    {
        demo1_client_read_write(ServerIP, ServerPort);
    }

#ifdef _WIN32
    if (argc != 2)
    {
        WSACleanup();
    }
#endif

    return 0;
}
