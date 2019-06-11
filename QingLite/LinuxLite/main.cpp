#include <iostream>
#include "../../Common/Tool/Common.h"
//#include "../../Common/Algorithm/AlgorithmTest.h"

//#include "UNIX/Chapter1/list_file_name.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_nobuffer.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_standard.h"

//#include "Open/libevent/demo1_client_read_write.h"
//#include "Open/libevent/demo1_server_read_write.h"
//#include "Open/libevent/demo2_client_bufferevent.h"
//#include "Open/libevent/demo2_server_bufferevent.h"
//#include "Open/libevent/demo3_client_evconnlistener.h"
//#include "Open/libevent/demo3_server_evconnlistener.h"
//#include "Open/libevent/demo4_client_udp_timer.h"
//#include "Open/libevent/demo4_server_udp_timer.h"
//#include "Open/libevent/demo5_client_evhttp.h"
//#include "Open/libevent/demo5_server_evhttp.h"
//#include "Open/libevent/demo5_server_evconnlistener_http.h"
//#include "Open/libevent/demo6_client_bufferevent_openssl.h"
//#include "Open/libevent/demo6_server_bufferevent_openssl.h"

#include "Open/project/SingleEventServer.h"
#include "Open/project/SingleEventClient.h"



int main(int argc, char *argv[])
{
    //UNIX
    //list_file_name(argc, argv);
    //copy_stdin_stdout_nobuffer();
    //copy_stdin_stdout_standard();

    const char *ServerIP = "192.168.3.126";
    const int ServerPort = 12345;

    //libevent
    if (atoi(argv[1]) == 0)
    {
        //demo1_server_read_write(ServerIP, ServerPort);
        //demo2_server_bufferevent(ServerIP, ServerPort);
        //demo3_server_evconnlistener(ServerIP, ServerPort);
        //demo4_server_udp_timer();
        //demo5_server_evhttp(ServerIP, ServerPort);
        //demo6_server_bufferevent_openssl(ServerIP, ServerPort);

        SingleEventServer Server;
        if (Server.Initialize(ServerIP, ServerPort))
        {
            Server.Start();
        }
    }
    else
    {
        //demo1_client_read_write(ServerIP, ServerPort);
        //demo2_client_bufferevent(ServerIP, ServerPort);
        //demo3_client_evconnlistener(ServerIP, ServerPort);
        //demo4_client_udp_timer();
        //demo5_client_evhttp(ServerIP, ServerPort);
        //demo6_client_bufferevent_openssl(ServerIP, ServerPort);

        SingleEventClient Client;
        Client.Start(ServerPort);
    }

    std::cout << std::endl;
    return 0;
}
