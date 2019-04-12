#include <iostream>
//#include "../../Common/Tool/Common.h"
//#include "../../Common/Algorithm/AlgorithmTest.h"

//#include "UNIX/Chapter1/list_file_name.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_nobuffer.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_standard.h"

#include "YouShuang/Chapter5/backlog_test.h"
#include "YouShuang/Chapter5/accept_exception.h"
#include "YouShuang/Chapter5/oob_data_client.h"
#include "YouShuang/Chapter5/oob_data_server.h"
#include "YouShuang/Chapter5/change_tcp_buffer_size_client.h"
#include "YouShuang/Chapter5/change_tcp_buffer_size_server.h"
#include "YouShuang/Chapter5/daytime.h"


int main(int argc, char *argv[])
{
    //UNIX
    //list_file_name(argc, argv);
    //copy_stdin_stdout_nobuffer();
    //copy_stdin_stdout_standard();


    //YouShuang
    //blacklog_test(argc, argv);
    //accept_exception(argc, argv);
    //oob_data_client(argc, argv);
    //oob_data_server(argc, argv);
    //change_tcp_buffer_size_client(argc, argv);
    //change_tcp_buffer_size_server(argc, argv);
    daytime(argc, argv);

    std::cout << std::endl;
    return 0;
}
