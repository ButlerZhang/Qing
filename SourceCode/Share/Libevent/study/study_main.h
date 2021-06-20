#include "study1_timer.h"
#include <event2/event.h>


int study_main(int argc, char *argv[])
{
#ifdef _WIN32

    if (argc != 2)
    {
        WSADATA WSAData;
        WSAStartup(0x101, &WSAData);
    }
#endif

    printf("=======================================\n");
    printf("display system supported backend method\n");
    const char** SystemSupportedMethod = event_get_supported_methods();
    while (SystemSupportedMethod && *SystemSupportedMethod)
    {
        printf("%s\t", *SystemSupportedMethod++);
    }
    printf("\n=======================================\n");

    //study
    study1_timer(argc, argv);


#ifdef _WIN32
    if (argc != 2)
    {
        WSACleanup();
    }
#endif

    return 0;
}
