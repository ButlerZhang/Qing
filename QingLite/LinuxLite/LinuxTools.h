#pragma once
#include <uuid/uuid.h>
#include <string>
#include <random>



std::string GetUUID()
{
    uuid_t uuid;
    char uuidstring[36];

    uuid_generate(uuid);
    uuid_unparse(uuid, uuidstring);

    return std::string(uuidstring);
}

unsigned int GetRandomUIntInRange(int Min, int Max)
{
    static std::default_random_engine RandomEngine;
    static std::uniform_int_distribution<unsigned int> u(Min, Max);
    return u(RandomEngine);
}