#pragma once
#include "Network/SingleEventBaseClient.h"



class Client : public SingleEventBaseClient
{
public:

    Client();
    virtual ~Client();

    virtual bool ProcessConnected();
    virtual bool ProcessDisconnected();
    virtual bool ProcessMessage(void *Data, size_t Size);
};
