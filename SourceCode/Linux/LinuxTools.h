#pragma once
#include <string>
#include <vector>
#include <random>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <uuid/uuid.h>
#include <arpa/inet.h>
#include <sys/socket.h>



inline std::string GetUUID()
{
    uuid_t uuid;
    char uuidstring[36];

    uuid_generate(uuid);
    uuid_unparse(uuid, uuidstring);

    return std::string(uuidstring);
}

inline std::string GetWorkDirectory()
{
    char WorkPath[PATH_MAX];
    memset(WorkPath, 0, PATH_MAX);

    getcwd(WorkPath, PATH_MAX);
    return std::string(WorkPath);
}

inline std::string GetProgramName()
{
    char ProgramFullPath[PATH_MAX];
    memset(ProgramFullPath, 0, PATH_MAX);
    if (readlink("/proc/self/exe", ProgramFullPath, PATH_MAX) > 0)
    {
        std::string ProgramPath(ProgramFullPath);
        std::string::size_type StartIndex = ProgramPath.find_last_of('/') + 1;
        if (StartIndex != std::string::npos)
        {
            std::size_t CharCount = ProgramPath.size() - StartIndex - 4; //sizeof(.out) == 4
            std::string ProgramName = ProgramPath.substr(StartIndex, CharCount);
            return ProgramName;
        }
    }

    return std::string();
}

inline bool GetHostIPv4(std::vector<std::string> &IPVector)
{
    struct ifaddrs *ifaddrsList;
    if (getifaddrs(&ifaddrsList) != 0)
    {
        return false;
    }

    std::vector<char> IP(64, 0);
    for (struct ifaddrs *CurrentAddress = ifaddrsList; CurrentAddress != NULL; CurrentAddress = CurrentAddress->ifa_next)
    {
        if (CurrentAddress->ifa_addr == NULL)
            continue;

        if (!(CurrentAddress->ifa_flags & IFF_UP))
            continue;

        if (CurrentAddress->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)CurrentAddress->ifa_addr;
            void *in_addr = &ipv4->sin_addr;

            if (inet_ntop(CurrentAddress->ifa_addr->sa_family, in_addr, &IP[0], (socklen_t)IP.size()))
            {
                IPVector.push_back(std::string(IP.begin(), IP.end()));
            }
        }
    }

    freeifaddrs(ifaddrsList);
    return true;
}

inline bool GetHostIPv6(std::vector<std::string> &IPVector)
{
    struct ifaddrs *ifaddrsList;
    if (getifaddrs(&ifaddrsList) != 0)
    {
        return false;
    }

    std::vector<char> IP(64, 0);
    for (struct ifaddrs *CurrentAddress = ifaddrsList; CurrentAddress != NULL; CurrentAddress = CurrentAddress->ifa_next)
    {
        if (CurrentAddress->ifa_addr == NULL)
            continue;

        if (!(CurrentAddress->ifa_flags & IFF_UP))
            continue;

        if (CurrentAddress->ifa_addr->sa_family == AF_INET6)
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)CurrentAddress->ifa_addr;
            void *in_addr = &ipv6->sin6_addr;

            if (inet_ntop(CurrentAddress->ifa_addr->sa_family, in_addr, &IP[0], (socklen_t)IP.size()))
            {
                IPVector.push_back(std::string(IP.begin(), IP.end()));
            }
        }
    }

    freeifaddrs(ifaddrsList);
    return true;
}

inline unsigned int GetRandomUIntInRange(int Min, int Max)
{
    static std::default_random_engine RandomEngine(time(NULL));
    static std::uniform_int_distribution<unsigned int> u(Min, Max);
    return u(RandomEngine);
}

inline std::string GenerateRandomKey(unsigned int CharacterCount)
{
    const std::string CHARACTER("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    const int CHARACTER_SIZE = static_cast<int>(CHARACTER.size());

    std::string RandomKey;
    while (static_cast<unsigned int>(RandomKey.size()) < CharacterCount)
    {
        std::string::size_type Index = GetRandomUIntInRange(0, CHARACTER_SIZE);
        RandomKey.push_back(CHARACTER[Index]);
    }

    return RandomKey;
}

inline bool SplitString(const std::string &SourceString, std::vector<std::string> &StringVector, const std::string &Seperator)
{
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = SourceString.find(Seperator);

    while (std::string::npos != pos2)
    {
        const std::string &TempString = SourceString.substr(pos1, pos2 - pos1);
        if (!TempString.empty())
        {
            StringVector.push_back(TempString);
        }

        pos1 = pos2 + Seperator.size();
        pos2 = SourceString.find(Seperator, pos1);
    }

    if (pos1 != SourceString.length())
    {
        StringVector.push_back(SourceString.substr(pos1));
    }

    return !StringVector.empty();
}
