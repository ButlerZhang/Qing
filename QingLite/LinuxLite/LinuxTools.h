#pragma once
#include <string>
#include <random>
#include <unistd.h>
#include <dirent.h>
#include <uuid/uuid.h>



std::string GetUUID()
{
    uuid_t uuid;
    char uuidstring[36];

    uuid_generate(uuid);
    uuid_unparse(uuid, uuidstring);

    return std::string(uuidstring);
}

std::string GetWorkDirectory()
{
    char WorkPath[PATH_MAX];
    if (getcwd(WorkPath, PATH_MAX) == NULL)
    {
        //printf("ERROR: Get work path failed.\n");
    }
    else
    {
        //printf("Work Path = %s\n", WorkPath);
    }

    return std::string(WorkPath);
}

unsigned int GetRandomUIntInRange(int Min, int Max)
{
    static std::default_random_engine RandomEngine;
    static std::uniform_int_distribution<unsigned int> u(Min, Max);
    return u(RandomEngine);
}