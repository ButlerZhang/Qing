#include "..\HeaderFiles\GBKandUTF8.h"
#include <Windows.h>

QING_NAMESPACE_BEGIN



GBKandUTF8::GBKandUTF8()
{
}

GBKandUTF8::~GBKandUTF8()
{
}

void GBKandUTF8::UTF8ToGBK(char * szOut)
{
    int Length = MultiByteToWideChar(CP_UTF8, 0 ,(LPCSTR)szOut, -1, NULL, 0);

    memset(m_wszGBK, 0, sizeof(m_wszGBK));
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)szOut, -1, (LPWSTR)m_wszGBK, Length);

    Length = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)m_wszGBK, -1, NULL, 0, NULL,NULL);

    memset(m_szGBK, 0, sizeof(m_szGBK));
    WideCharToMultiByte(CP_ACP,0,(LPWSTR)m_wszGBK, -1, m_szGBK, Length,NULL,NULL);

    memset((char*)szOut, 0, strlen(m_szGBK)+1);
    memcpy((char*)szOut, m_szGBK, strlen(m_szGBK)+1);
}

void GBKandUTF8::GBKToUTF8(char *OutUTF8, const char* InGBK)
{
    int Lenth = MultiByteToWideChar(CP_ACP, 0, InGBK, -1, NULL, 0);

    memset(m_WUTF8, 0, sizeof(m_WUTF8));
    MultiByteToWideChar(CP_ACP, 0, InGBK, -1, m_WUTF8, Lenth);

    Lenth = WideCharToMultiByte(CP_UTF8, 0, m_WUTF8, -1, NULL, 0, NULL, NULL);

    memset(m_UTF8, 0, sizeof(m_UTF8));
    WideCharToMultiByte(CP_UTF8, 0, m_WUTF8, -1, m_UTF8, Lenth, NULL, NULL);

    memcpy(OutUTF8,m_UTF8,strlen(m_UTF8)+1);
}

QING_NAMESPACE_END