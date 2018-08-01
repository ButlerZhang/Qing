#pragma once
#include "QingBase.h"
#include <winsock.h>



namespace Qing
{
    class QING_DLL Network
    {
    public:

        Network();
        virtual ~Network();

    protected:

        bool CreateSocket();

        bool SendData();
        bool RecvData();

    protected:

        SOCKET m_Socket;
    };
}
