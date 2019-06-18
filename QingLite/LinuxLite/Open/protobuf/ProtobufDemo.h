#pragma once
#include "../../LinuxTools.h"
#include "user.pb.h"
#include <fstream>



void ProtobufDemo()
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
        return;
    }

    fout.close();

    Open::User CheckUser;
    std::ifstream fin("test.txt");

    if (!CheckUser.ParseFromIstream(&fin))
    {
        printf("ERROR: parse from istream failed.\n");
        return;
    }

    fin.close();

    printf("\nCheck user information:\n");
    CheckUser.PrintDebugString();
}
