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