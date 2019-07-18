//#include "UNIX/unix_main.h"
//#include "Open/openssl/demo_test.h"
#include "Open/project/controller/Client.h"
#include "Open/project/controller/SingleServer.h"
#include "Open/project/controller/MultiServer.h"
#include "Open/project/core/tools/BoostLog.h"
#include <iostream>



int main(int argc, char *argv[])
{
    //return unix_chapter1_main(argc, argv);
    //OpenSSL_EncryptAndDecryptTest();
    //return 0;

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


