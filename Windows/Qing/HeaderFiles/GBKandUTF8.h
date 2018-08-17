#pragma once
#include "QingBase.h"

QING_NAMESPACE_BEGIN



class QING_DLL GBKandUTF8
{
public:

    GBKandUTF8();
    ~GBKandUTF8();

    void UTF8ToGBK(char * szOut);
    void GBKToUTF8(char * OutUTF8, const char* InGBKS);

private:

    enum
    {
        ARRAY_SIZE = 1024,
    };

    char                m_szGBK[ARRAY_SIZE];
    unsigned short      m_wszGBK[ARRAY_SIZE];

    char                m_UTF8[ARRAY_SIZE];
    wchar_t             m_WUTF8[ARRAY_SIZE];
};

QING_NAMESPACE_END