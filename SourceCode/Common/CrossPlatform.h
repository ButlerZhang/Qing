#pragma once

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif // WIN32





inline long GetProgramID()
{
#ifdef _WIN32

    return _getpid();

#else

    return getpid();

#endif // WIN32
}

inline std::string GetWorkDirectory()
{
#ifdef _WIN32

    //TODO

#else

    char WorkPath[PATH_MAX];
    memset(WorkPath, 0, PATH_MAX);

    if (getcwd(WorkPath, PATH_MAX) != NULL)
    {
        return std::string(WorkPath);
    }

#endif

    return std::string();
}

inline std::string GetProgramName()
{
#ifdef _WIN32



#else

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

#endif

    return std::string();
}

inline bool IsDirectory(const std::string &Directory)
{
#ifdef _WIN32

    return true;

#else

    struct stat statbuff;
    return lstat(Directory.c_str(), &statbuff) == 0 && S_ISDIR(statbuff.st_mode);

#endif
}