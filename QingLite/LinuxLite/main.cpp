#include <iostream>
//#include "../../Common/Tool/Common.h"
//#include "../../Common/Algorithm/AlgorithmTest.h"

//#include "UNIX/Chapter1/list_file_name.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_nobuffer.h"
//#include "UNIX/Chapter1/copy_stdin_stdout_standard.h"

#include "YouShuang/Chapter6/cgi_server.h"
#include "YouShuang/Chapter6/writev_server.h"
#include "YouShuang/Chapter6/sendfile_server.h"
#include "YouShuang/Chapter6/splice_server.h"
#include "YouShuang/Chapter6/tee_server.h"


int main(int argc, char *argv[])
{
    //UNIX
    //list_file_name(argc, argv);
    //copy_stdin_stdout_nobuffer();
    //copy_stdin_stdout_standard();

    //YouShuang
    //cgi_server(argc, argv);
    //writev_server(argc, argv);
    //sendfile_server(argc, argv);
    //splice_server(argc, argv);
    tee_server(argc, argv);

    std::cout << std::endl;
    return 0;
}
