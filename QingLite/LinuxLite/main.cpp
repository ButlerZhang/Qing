#include <iostream>
#include "../../Common/Tool/Common.h"
//#include "../../Common/Algorithm/AlgorithmTest.h"

//#include "UNIX/Chapter1/list_file_name.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_nobuffer.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_standard.h"

//#include "Open/libevent/demo1_client.h"
//#include "Open/libevent/demo1_server.h"
//#include "Open/libevent/demo2_client.h"
//#include "Open/libevent/demo2_server.h"
#include "Open/libevent/demo3_client.h"
#include "Open/libevent/demo3_server.h"



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
    demo3_client("192.168.3.126", 12345);
    //demo3_server("192.168.3.126", 12345);

    std::cout << std::endl;
    return 0;
}
