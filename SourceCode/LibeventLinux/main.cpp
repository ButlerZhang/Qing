#include <iostream>
#include "src/demo1_client_read_write.h"
#include "src/demo1_server_read_write.h"



int main()
{
    demo1_server_read_write("127.0.0.1", 9000);
    return 0;
}
