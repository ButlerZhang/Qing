#include <iostream>
#include "../../Common/Tool/Common.h"
//#include "../../Common/Algorithm/AlgorithmTest.h"

//#include "UNIX/Chapter1/list_file_name.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_nobuffer.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_standard.h"

//#include "Open/libevent/Demo/demo1_client.h"
//#include "Open/libevent/Demo/demo1_server.h"
//#include "Open/libevent/Demo/demo2_client.h"
//#include "Open/libevent/Demo/demo2_server.h"
//#include "Open/libevent/Demo/demo3_client.h"
//#include "Open/libevent/Demo/demo3_server.h"
//#include "Open/libevent/Demo/demo4_client.h"
//#include "Open/libevent/Demo/demo4_server.h"

#include "Open/libevent/SingleThread/SingleThreadServerLite.h"
#include "Open/libevent/SingleThread/SingleThreadClientLite.h"



int main(int argc, char *argv[])
{
    //UNIX
    //list_file_name(argc, argv);
    //copy_stdin_stdout_nobuffer();
    //copy_stdin_stdout_standard();

    //libevent
    //demo1_client("192.168.3.126", 12345);
    //demo1_server("192.168.3.126", 12345);
    //demo2_client("192.168.3.126", 12345);
    //demo2_server("192.168.3.126", 12345);
    //demo3_client("192.168.3.126", 12345);
    //demo3_server("192.168.3.126", 12345);
    //demo4_client();

    if (atoi(argv[1]) == 0)
    {
        SingleThreadServerLite Server;
        if (Server.Initialize("192.168.3.126", 12345))
        {
            Server.Start();
        }
    }
    else
    {
        SingleThreadClientLite Client;
        Client.Start();
    }

    std::cout << std::endl;
    return 0;
}
