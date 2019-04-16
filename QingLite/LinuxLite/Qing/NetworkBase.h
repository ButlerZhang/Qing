#pragma once
#include <string>



class NetworkBase
{
public:

    NetworkBase();
    virtual ~NetworkBase();

    int  SetNonBlocking(int fd);
    int  GetSocket() const { return m_Socket; }

public:

    int  Accept();
    bool StartServer(const std::string &IP, int Port);

public:

    bool StartClient();
    bool ConnectServer(const std::string &IP, int Port);

protected:

    bool CreateSocket(int Domain, int Type);
    bool BindSocket(const std::string &IP, int Port);
    bool ListenSocket();
    void InitSockAddress(void *Address, const std::string &IP, int Port);

protected:

    int m_Socket;
};
