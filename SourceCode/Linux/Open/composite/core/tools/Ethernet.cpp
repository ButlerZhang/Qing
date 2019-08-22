#include "Ethernet.h"
#include "../../../../../Common/Boost/BoostLog.h"
#include <net/if.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>



Ethernet::Ethernet()
{
}

Ethernet::~Ethernet()
{
    m_NodeVector.clear();
}

std::string Ethernet::GetLocalIP() const
{
    if (!m_NodeVector.empty())
    {
        return m_NodeVector[0].m_IP;
    }

    return std::string();
}

std::string Ethernet::GetLocalMAC() const
{
    if (!m_NodeVector.empty())
    {
        return m_NodeVector[0].m_MAC;
    }

    return std::string();
}

std::string Ethernet::GetLocalMask() const
{
    if (!m_NodeVector.empty())
    {
        return m_NodeVector[0].m_Mask;
    }

    return std::string();
}

std::string Ethernet::GetMAC(const std::string &EthernetName) const
{
    int TemporySocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (TemporySocket < 0)
    {
        g_Log.WriteError(BoostFormat("Ethernet can not create tempory socket, error = %s.", strerror(errno)));
        return std::string();
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, EthernetName.c_str());
    if (ioctl(TemporySocket, SIOCGIFHWADDR, &ifr) < 0)
    {
        g_Log.WriteError(BoostFormat("Ethernet can not ioctl SIOCGIFHWADDR, name = %s, error = %s.", EthernetName.c_str(), strerror(errno)));
        close(TemporySocket);
        return std::string();
    }

    char MacAddress[64];
    sprintf(MacAddress, "%02x-%02x-%02x-%02x-%02x-%02x",
        (unsigned char)ifr.ifr_hwaddr.sa_data[0],
        (unsigned char)ifr.ifr_hwaddr.sa_data[1],
        (unsigned char)ifr.ifr_hwaddr.sa_data[2],
        (unsigned char)ifr.ifr_hwaddr.sa_data[3],
        (unsigned char)ifr.ifr_hwaddr.sa_data[4],
        (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

    close(TemporySocket);
    return std::string(MacAddress, MacAddress + strlen(MacAddress));
}

bool Ethernet::Initialize()
{
    if (!m_NodeVector.empty())
    {
        return true;
    }

    struct ifaddrs *ifaddrsList;
    if (getifaddrs(&ifaddrsList) != 0)
    {
        g_Log.WriteError("Ethernet get ifaddrs failed.");
        return false;
    }

    std::vector<char> IP(64, 0);
    std::vector<char> Mask(64, 0);
    for (struct ifaddrs *CurrentAddress = ifaddrsList; CurrentAddress != NULL; CurrentAddress = CurrentAddress->ifa_next)
    {
        if (CurrentAddress->ifa_addr == NULL)
            continue;

        if (!(CurrentAddress->ifa_flags & IFF_UP))
            continue;

        if (CurrentAddress->ifa_addr->sa_family != AF_INET)
            continue;

        struct sockaddr_in *IPV4 = (struct sockaddr_in *)CurrentAddress->ifa_addr;
        if (inet_ntop(AF_INET, &(IPV4->sin_addr), &IP[0], (socklen_t)IP.size()) == NULL)
            continue;

        if (std::string(IP.begin(), IP.end()).find("127") != std::string::npos) //ignore 127.XXX.XXX.XXX
            continue;

        struct sockaddr_in *MaskAddr = (struct sockaddr_in *)CurrentAddress->ifa_netmask;
        if (inet_ntop(AF_INET, &(MaskAddr->sin_addr), &Mask[0], (socklen_t)Mask.size()) == NULL)
            continue;

        m_NodeVector.push_back(EthernetNode());
        std::vector<EthernetNode>::size_type CurrentIndex = m_NodeVector.size() - 1;

        m_NodeVector[CurrentIndex].m_Name = CurrentAddress->ifa_name;
        m_NodeVector[CurrentIndex].m_MAC = GetMAC(m_NodeVector[CurrentIndex].m_Name);
        m_NodeVector[CurrentIndex].m_IP.assign(IP.begin(), IP.begin() + strlen(&IP[0]));
        m_NodeVector[CurrentIndex].m_Mask.assign(Mask.begin(), Mask.begin() + strlen(&Mask[0]));

        g_Log.WriteDebug(BoostFormat("Ethernet initialize, index = %d, name = %s, mac = %s, ip = %s, mask = %s", CurrentIndex,
            m_NodeVector[CurrentIndex].m_Name.c_str(), m_NodeVector[CurrentIndex].m_MAC.c_str(),
            m_NodeVector[CurrentIndex].m_IP.c_str(), m_NodeVector[CurrentIndex].m_Mask.c_str()));
    }

    freeifaddrs(ifaddrsList);
    return !m_NodeVector.empty();
}

bool Ethernet::IsExisted(const std::string &IP, const std::string &MAC) const
{
    for (std::vector<EthernetNode>::const_iterator it = m_NodeVector.begin(); it != m_NodeVector.end(); it++)
    {
        if (it->m_IP == IP && it->m_MAC == MAC)
        {
            return true;
        }
    }

    g_Log.WriteDebug(BoostFormat("Ethernet can not find ip = %s and mac = %s", IP.c_str(), MAC.c_str()));
    return false;
}

bool Ethernet::SetLocalIP(const std::string &IP)
{
    if (IP.empty())
    {
        g_Log.WriteError("Ethernet set local ip error, IP is empty.");
        return false;
    }

    if (m_NodeVector.empty())
    {
        g_Log.WriteError(BoostFormat("Ethernet set local ip = %s can not find ethernet name.", IP.c_str()));
        return false;
    }

    int SetIPSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (SetIPSocket < 0)
    {
        g_Log.WriteError(BoostFormat("Ethernet set local ip = %s can not create socket.", IP.c_str()));
        return false;
    }

    struct sockaddr_in IPAddress;
    memset(&IPAddress, 0, sizeof(IPAddress));
    IPAddress.sin_family = AF_INET;
    IPAddress.sin_addr.s_addr = inet_addr(IP.c_str());

    struct ifreq ifr;
    bzero(&ifr, sizeof(ifr));
    memcpy(&ifr.ifr_addr, &IPAddress, sizeof(IPAddress));
    strncpy(ifr.ifr_name, m_NodeVector[0].m_Name.c_str(), sizeof(ifr.ifr_name) - 1);

    if (ioctl(SetIPSocket, SIOCSIFADDR, &ifr) < 0)
    {
        g_Log.WriteError(BoostFormat("Ethernet set local ip = %s error = %s.", IP.c_str(), strerror(errno)));
        close(SetIPSocket);
        return false;
    }

    //check code
    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if (ioctl(SetIPSocket, SIOCSIFFLAGS, &ifr) < 0)
    {
        g_Log.WriteError(BoostFormat("Ethernet set local ip = %s check error = %s.", IP.c_str(), strerror(errno)));
    }

    close(SetIPSocket);
    return true;
}

bool Ethernet::SetLocalMask(const std::string &Mask)
{
    if (Mask.empty())
    {
        g_Log.WriteError("Ethernet set local Mask error, Mask is empty.");
        return false;
    }

    if (m_NodeVector.empty())
    {
        g_Log.WriteError(BoostFormat("Ethernet set local mask = %s can not find ethernet name.", Mask.c_str()));
        return false;
    }

    int SetMaskSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (SetMaskSocket < 0)
    {
        g_Log.WriteError(BoostFormat("Ethernet set local mask = %s can not create socket.", Mask.c_str()));
        return false;
    }

    struct ifreq ifr;
    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, m_NodeVector[0].m_Name.c_str(), sizeof(ifr.ifr_name) - 1);

    struct sockaddr_in *MaskAddress = (struct sockaddr_in *)&ifr.ifr_addr;
    inet_pton(AF_INET, Mask.c_str(), &MaskAddress->sin_addr);
    MaskAddress->sin_family = AF_INET;

    if (ioctl(SetMaskSocket, SIOCSIFNETMASK, &ifr) < 0)
    {
        g_Log.WriteError(BoostFormat("Ethernet set local mask = %s error = %s.", Mask.c_str(), strerror(errno)));
        close(SetMaskSocket);
        return false;
    }

    close(SetMaskSocket);
    return true;
}

bool Ethernet::SetIPandMask(const std::string &IP, const std::string &Mask)
{
    return SetLocalIP(IP) && SetLocalMask(Mask);
}
