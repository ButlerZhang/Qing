#include "study1_timer.h"



int study_main(int argc, char *argv[])
{
#ifdef _WIN32

    if (argc != 2)
    {
        WSADATA WSAData;
        WSAStartup(0x101, &WSAData);
    }
#endif

    study1_timer(argc, argv);


#ifdef _WIN32
    if (argc != 2)
    {
        WSACleanup();
    }
#endif

    return 0;
}
