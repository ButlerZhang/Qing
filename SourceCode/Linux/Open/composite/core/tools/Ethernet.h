#pragma once
#include <string>
#include <vector>



class Ethernet
{
public:

    struct EthernetNode
    {
        std::string                 m_IP;
        std::string                 m_MAC;
        std::string                 m_Mask;
        std::string                 m_Name;
    };

public:

    Ethernet();
    ~Ethernet();

    std::string GetLocalIP() const;
    std::string GetLocalMAC() const;
    std::string GetLocalMask() const;
    std::string GetMAC(const std::string &EthernetName) const;

    bool Initialize();
    bool IsExisted(const std::string &IP, const std::string &MAC) const;

    bool SetLocalIP(const std::string &IP);
    bool SetLocalMask(const std::string &Mask);
    bool SetIPandMask(const std::string &IP, const std::string &Mask);

    const std::vector<EthernetNode>& GetNodeVector() const { return m_NodeVector; }

private:

    std::vector<EthernetNode>       m_NodeVector;
};
