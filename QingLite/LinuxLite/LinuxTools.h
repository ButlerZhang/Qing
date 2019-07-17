#pragma once
#include <string>
#include <random>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <uuid/uuid.h>



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

inline unsigned int GetRandomUIntInRange(int Min, int Max)
{
    static std::default_random_engine RandomEngine;
    static std::uniform_int_distribution<unsigned int> u(Min, Max);
    return u(RandomEngine);
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