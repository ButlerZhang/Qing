#include "UNIX/unix_main.h"

#include "Open/project/Client.h"
#include "Open/project/SingleServer.h"
#include <iostream>



int main(int argc, char *argv[])
{
    return unix_chapter1_main(argc, argv);


    const char *ServerIP = "192.168.3.126";
    const int ServerPort = 12345;

    if (atoi(argv[1]) == 0)
    {
        SingleServer Server;
        Server.Start(ServerIP, ServerPort);

        //MultiEventBaseServer Server;
        //Server.Start(ServerIP, ServerPort, 2);
    }
    else
    {
        Client MyClient;
        //MyClient.Start(ServerPort);
        MyClient.Start(ServerIP, ServerPort);
    }

    std::cout << std::endl;
    return 0;
}
