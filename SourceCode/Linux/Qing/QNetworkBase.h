#pragma once
#include <string>



class QNetworkBase
{
public:

    QNetworkBase();
    virtual ~QNetworkBase();

    int  SetNonBlocking(int fd);
    int  GetSocket() const { return m_Socket; }

protected:

    bool CreateSocket(int Domain, int Type);
    void InitSockAddress(void *Address, const std::string &IP, int Port);

protected:

    int m_Socket;
};
