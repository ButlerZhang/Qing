#include <iostream>
//#include "../../Common/Tool/Common.h"
//#include "../../Common/Algorithm/AlgorithmTest.h"

//#include "UNIX/Chapter1/list_file_name.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_nobuffer.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_standard.h"

#include "YouShuang/Chapter5/backlog_test.h"
#include "YouShuang/Chapter5/accept_exception.h"
#include "YouShuang/Chapter5/send_oob_data.h"
#include "YouShuang/Chapter5/recv_oob_data.h"


int main(int argc, char *argv[])
{
    //UNIX
    //list_file_name(argc, argv);
    //copy_stdin_stdout_nobuffer();
    //copy_stdin_stdout_standard();


    //YouShuang
    //blacklog_test(argc, argv);
    //accept_exception(argc, argv);
    //send_oob_data(argc, argv);
    recv_oob_data(argc, argv);


    std::cout << std::endl;
    return 0;
}
