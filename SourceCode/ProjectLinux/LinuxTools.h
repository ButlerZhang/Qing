#pragma once
#include <string>
#include <vector>
#include <random>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
    return (getcwd(WorkPath, PATH_MAX) != NULL) ? std::string(WorkPath) : std::string();
}

inline std::string GetProgramFullPath()
{
    char ProgramFullPath[PATH_MAX];
    memset(ProgramFullPath, 0, PATH_MAX);
    if (readlink("/proc/self/exe", ProgramFullPath, PATH_MAX) > 0)
    {
        return std::string(ProgramFullPath);
    }

    return std::string();
}

inline std::string GetProgramName(bool IsIncludeExtension)
{
    std::string ProgramPath(GetProgramFullPath());
    std::string::size_type StartIndex = ProgramPath.find_last_of('/') + 1;

    if (StartIndex != std::string::npos)
    {
        std::size_t CharCount = ProgramPath.size() - StartIndex;
        if (!IsIncludeExtension)
        {
            CharCount -= 4;//sizeof(.out) == 4
        }

        return ProgramPath.substr(StartIndex, CharCount);
    }

    return std::string();
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

    StringVector.clear();
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

inline bool GetShellResult(const std::string &Command, std::vector<std::string> &ResultVector)
{
    ResultVector.clear();
    if (Command.empty())
    {
        return false;
    }

    FILE *fp = popen(Command.c_str(), "r");
    if (fp == NULL)
    {
        return false;
    }

    std::vector<char> LineBuffer(1024, 0);
    while (fgets(&LineBuffer[0], static_cast<int>(LineBuffer.size()), fp) != NULL)
    {
        ResultVector.push_back(std::string(LineBuffer.begin(), LineBuffer.begin() + strlen(&LineBuffer[0])));
    }

    pclose(fp);
    return !ResultVector.empty();
}

inline pid_t GetProcessIDByProcessName(const std::string &ProcessName) //only use in the same section
{
    std::vector<std::string> ResultVector;
    std::string Command("pidof " + ProcessName);
    return GetShellResult(Command, ResultVector) ? atoi(ResultVector[0].c_str()) : 0;
}