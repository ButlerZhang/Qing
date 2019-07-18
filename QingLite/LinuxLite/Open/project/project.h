#include "controller/Client.h"
#include "controller/SingleServer.h"
#include "controller/MultiServer.h"
#include "core/tools/BoostLog.h"
#include <iostream>



int project(int argc, char *argv[])
{
    BoostLog::DefaultInit();

    const char *ServerIP = "192.168.3.126";
    const int ServerPort = 12345;

    if (argc <= 1 || atoi(argv[1]) == 0)
    {
        SingleServer Server;
        Server.Start(ServerIP, ServerPort);

        //MultiServer Server;
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
