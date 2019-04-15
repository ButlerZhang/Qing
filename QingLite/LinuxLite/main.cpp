#include <iostream>
//#include "../../Common/Tool/Common.h"
//#include "../../Common/Algorithm/AlgorithmTest.h"

//#include "UNIX/Chapter1/list_file_name.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_nobuffer.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_standard.h"

#include "YouShuang/Chapter7/switch_to_user.h"
#include "YouShuang/Chapter7/daemonize_demo.h"



int main(int argc, char *argv[])
{
    //UNIX
    //list_file_name(argc, argv);
    //copy_stdin_stdout_nobuffer();
    //copy_stdin_stdout_standard();

    //YouShuang
    daemonize_demo();
    switch_to_user(0, 0);

    std::cout << std::endl;
    return 0;
}
