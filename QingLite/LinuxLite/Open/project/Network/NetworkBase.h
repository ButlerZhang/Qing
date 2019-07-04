#pragma once
#include <string>



const int NETWORK_BUFFER_SIZE = 2048;



struct NetworkMessage
{
    int                                      m_Socket;
    std::string                              m_Message;
    struct bufferevent                      *m_Bufferevent;

    NetworkMessage() : m_Socket(0), m_Bufferevent(NULL) {}
};
