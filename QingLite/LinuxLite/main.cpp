#include <iostream>
#include "../../Common/Tool/Common.h"
//#include "../../Common/Algorithm/AlgorithmTest.h"

//#include "UNIX/Chapter1/list_file_name.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_nobuffer.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_standard.h"

//#include "YouShuang/Chapter7/switch_to_user.h"
//#include "YouShuang/Chapter7/daemonize_demo.h"
//#include "YouShuang/Chapter8/http_demo.h"
//#include "YouShuang/Chapter9/select_demo.h"
//#include "YouShuang/Chapter5/oob_data_client.h"
#include "YouShuang/Chapter9/select_server.h"
#include "YouShuang/Chapter9/select_client.h"


int main(int argc, char *argv[])
{
    //UNIX
    //list_file_name(argc, argv);
    //copy_stdin_stdout_nobuffer();
    //copy_stdin_stdout_standard();

    //YouShuang
    //daemonize_demo();
    //switch_to_user(0, 0);
    //http_demo(argc, argv);
    //select_demo(argc, argv);
    //oob_data_client(argc, argv);
    select_server();
    //select_client();

    std::cout << std::endl;
    return 0;
}
