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
    //demo1_client();
    //demo1_server();
    //demo2_client();
    //demo2_server();
    demo3_client();
    //demo3_server();

    std::cout << std::endl;
    return 0;
}
