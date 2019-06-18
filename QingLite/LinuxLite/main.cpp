#include "LinuxTools.h"
#include "Open/protobuf/user.pb.h"
#include "Open/project/network/MultiEventBaseServer.h"
#include "Open/project/network/SingleEventBaseClient.h"

#include <iostream>
#include <fstream>



int main(int argc, char *argv[])
{
    Open::User LoginUser;
    LoginUser.set_id(1000);
    LoginUser.set_name("Butler");
    LoginUser.set_password("1234567890");
    LoginUser.set_authority("1001000100");
    LoginUser.add_nickname("Sky");
    LoginUser.add_nickname("Sea");

    Open::MessageHeader *Header = LoginUser.mutable_header();
    Header->set_type(Open::MessageType::MT_LOGIN);
    Header->set_transmissionid(GetUUID());

    printf("Login user information:\n");
    LoginUser.PrintDebugString();

    std::ofstream fout("test.txt");
    if (!LoginUser.SerializeToOstream(&fout))
    {
        printf("ERROR: serialize to ostream failed.\n");
        return 0;
    }

    fout.close();

    Open::User CheckUser;
    std::ifstream fin("test.txt");

    if (!CheckUser.ParseFromIstream(&fin))
    {
        printf("ERROR: parse from istream failed.\n");
        return 0;
    }

    fin.close();

    printf("\nCheck user information:\n");
    CheckUser.PrintDebugString();

    const char *ServerIP = "192.168.3.126";
    const int ServerPort = 12345;

    //libevent
    if (atoi(argv[1]) == 0)
    {
        MultiEventBaseServer Server;
        Server.Start(ServerIP, ServerPort, 2);
    }
    else
    {
        SingleEventBaseClient Client;
        //Client.Start(ServerPort);
        Client.Start(ServerIP, ServerPort);
    }

    std::cout << std::endl;
    return 0;
}
