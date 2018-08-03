#include "ServerNetwork.h"
#include <iostream>
#include "..\..\..\Qing\HeaderFiles\Utility.h"



ServerNetwork::ServerNetwork() : Network()
{
}

ServerNetwork::~ServerNetwork()
{
}

bool ServerNetwork::Start(const std::string & IP, int Port)
{
    if (!CreateSocket())
    {
        return false;
    }

    if (!BindSocket(IP, Port))
    {
        return false;
    }

    if (!StartListen())
    {
        return false;
    }

    return true;
}

SOCKET ServerNetwork::GetConnectedClient()
{
    return Network::AcceptConnect();
}

void TestServerNetwork()
{
    if (Qing::StartupNetwork())
    {
        ServerNetwork Server;
        Server.Start("127.0.0.1", 1989);
        std::string ResponsedString = "I am Server, GUID = " + Qing::GetGUID();

        while (true)
        {
            Sleep(1);
            SOCKET NewSocket = Server.GetConnectedClient();
            if (NewSocket != INVALID_SOCKET)
            {
                char Buffer[1024];
                memset(Buffer, 0, sizeof(Buffer));
                Server.RecvData(NewSocket, Buffer, sizeof(Buffer));
                std::cout << "RECV: " << "Client Socket = " << NewSocket << "\tData = " << Buffer << std::endl;

                Server.SendData(NewSocket, ResponsedString.c_str(), static_cast<int>(ResponsedString.size()));
                std::cout << "SEND: " << ResponsedString << std::endl << std::endl;
            }
        }
    }
}
