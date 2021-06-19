#include "PaiGowGame/HouseWayFactory.h"
#include "LibeventServer/libevent_server_main.cpp"


int main(int argc, char *argv[])
{
    //PG::HouseWayFactory factory;
    //factory.unitTest();

    libevent_server_main(argc, argv);
    system("pause");
    return 0;
}
