#include "..\HeaderFiles\Utility.h"

QING_NAMESPACE_BEGIN



std::string GetGUID()
{
    GUID guid;
    char Data[64];
    memset(Data, 0, sizeof(Data));

    ::CoInitialize(0);
    if (::CoCreateGuid(&guid) == S_OK)
    {
        _snprintf_s(Data,
            sizeof(Data) - 1,
            "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
            guid.Data1,
            guid.Data2,
            guid.Data3,
            guid.Data4[0],
            guid.Data4[1],
            guid.Data4[2],
            guid.Data4[3],
            guid.Data4[4],
            guid.Data4[5],
            guid.Data4[6],
            guid.Data4[7]);
    }
    ::CoUninitialize();

    return std::string(Data);
}

std::string GetProgramName()
{
    char FullPathCharArray[MAX_PATH];
    memset(FullPathCharArray, 0, sizeof(FullPathCharArray));
    ::GetModuleFileNameA(NULL, FullPathCharArray, MAX_PATH);

    std::string FullPathString(FullPathCharArray);
    if (!FullPathString.empty())
    {
        std::size_t StartIndex = FullPathString.find_last_of('\\') + 1;
        if (StartIndex != std::string::npos)
        {
            std::size_t CharCount = FullPathString.size() - StartIndex - 4; //sizeof(.exe) == 4
            std::string ProgramName = FullPathString.substr(StartIndex, CharCount);
            return ProgramName;
        }
    }

    return "Qing";
}

std::wstring StringToWString(const std::string &String, int Codepage)
{
    int Length = MultiByteToWideChar(Codepage, 0, (LPCSTR)String.c_str(), static_cast<int>(String.length()), NULL, 0);

    std::wstring ResultWString(Length, L'\0');
    int ResultLength = MultiByteToWideChar(Codepage, 0, (LPCSTR)String.c_str(), static_cast<int>(String.length()), (LPWSTR)ResultWString.c_str(), Length);

    return ResultWString;
}

std::string WStringToString(const std::wstring &WString, int Codepage)
{
    int SourceLength = static_cast<int>(WString.length()) + 1;
    int TargetLength = WideCharToMultiByte(Codepage, 0, (LPCWSTR)WString.c_str(), SourceLength, NULL, 0, NULL, FALSE) + 1;

    std::string ResultString(TargetLength, '\0');
    int ResultLength = WideCharToMultiByte(Codepage, 0, (LPCWSTR)WString.c_str(), SourceLength, (LPSTR)ResultString.c_str(), TargetLength, NULL, FALSE);

    return ResultString;
}



QING_NAMESPACE_END