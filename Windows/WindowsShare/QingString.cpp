#include "QingString.h"



namespace QING
{
    std::wstring StringToWString(const std::string &str, int codepage)
    {
        int Length = MultiByteToWideChar(codepage, 0, (LPCSTR)str.c_str(), static_cast<int>(str.length()), NULL, 0);

        std::wstring ResultWString(Length, L'\0');
        int ResultLength = MultiByteToWideChar(codepage, 0, (LPCSTR)str.c_str(), static_cast<int>(str.length()), (LPWSTR)ResultWString.c_str(), Length);

        return ResultWString;
    }

    std::string WStringToString(const std::wstring &wstr, int codepage)
    {
        int SourceLength = static_cast<int>(wstr.length()) + 1;
        int TargetLength = WideCharToMultiByte(codepage, 0, (LPCWSTR)wstr.c_str(), SourceLength, NULL, 0, NULL, FALSE) + 1;

        std::string ResultString(TargetLength, '\0');
        int ResultLength = WideCharToMultiByte(codepage, 0, (LPCWSTR)wstr.c_str(), SourceLength, (LPSTR)ResultString.c_str(), TargetLength, NULL, FALSE);

        return ResultString;
    }
}
