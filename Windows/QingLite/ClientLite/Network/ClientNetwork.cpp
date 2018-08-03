#include "ClientNetwork.h"
#include <iostream>
#include "..\..\..\Qing\HeaderFiles\Utility.h"



ClientNetwork::ClientNetwork() : Network()
{
    m_IsConnected = false;
}

ClientNetwork::~ClientNetwork()
{
    m_IsConnected = false;
}

bool ClientNetwork::ConnectServer(const std::string & IP, int Port)
{
    if (!CreateSocket())
    {
        return false;
    }

    if (!Connect(IP, Port))
    {
        return false;
    }

    m_IsConnected = true;
    return m_IsConnected;
}

bool ClientNetwork::RecvData(char * Buffer, int BufferSize)
{
    return Network::RecvData(GetSocket(), Buffer, BufferSize);
}

bool ClientNetwork::SendData(const char * Buffer, int BufferSize)
{
    return Network::SendData(GetSocket(), Buffer, BufferSize);
}

void TestClientNetwork()
{
    if (Qing::StartupNetwork())
    {
        std::string Data = "I am client, GUID = " + Qing::GetGUID();

        while (true)
        {
            ClientNetwork Client;
            if (Client.ConnectServer("127.0.0.1", 1989))
            {
                Client.SendData(Data.c_str(), static_cast<int>(Data.size()));
                std::cout << "SEND: Socket = " << Client.GetSocket() << "\tData = " << Data << std::endl;

                char Buffer[1024];
                memset(Buffer, 0, sizeof(Buffer));
                Client.RecvData(Buffer, sizeof(Buffer));
                std::cout << "RECV: = " << Buffer << std::endl << std::endl;
            }

            Sleep(5000);
        }
    }
}
