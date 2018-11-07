#include "..\HeaderFiles\CommonFunction.h"
#include "..\HeaderFiles\BoostLog.h"

#include <cctype>
#include <sstream>
#include <iomanip>
#include <Shlwapi.h>
#include <algorithm>
#include <functional>
#include <tchar.h>
#include <DbgHelp.h>

#include <boost\beast\core\detail\base64.hpp>
#include <boost\uuid\name_generator_sha1.hpp>

QING_NAMESPACE_BEGIN



std::wstring GetGUID()
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

    return StringToWString(std::string(Data));
}

std::wstring GetProgramName()
{
    wchar_t FullPathCharArray[MAX_PATH];
    memset(FullPathCharArray, 0, sizeof(FullPathCharArray));
    ::GetModuleFileName(NULL, FullPathCharArray, MAX_PATH);

    std::wstring FullPathString(FullPathCharArray);
    if (!FullPathString.empty())
    {
        std::size_t StartIndex = FullPathString.find_last_of('\\') + 1;
        if (StartIndex != std::wstring::npos)
        {
            std::size_t CharCount = FullPathString.size() - StartIndex - 4; //sizeof(.exe) == 4
            std::wstring ProgramName = FullPathString.substr(StartIndex, CharCount);
            return ProgramName;
        }
    }

    BoostLog::WriteError(L"Return wrong program name.");
    return L"Qing";
}

std::wstring GetRunningPath()
{
    wchar_t FullPathCharArray[MAX_PATH];
    memset(FullPathCharArray, 0, sizeof(FullPathCharArray));
    ::GetModuleFileName(NULL, FullPathCharArray, MAX_PATH);

    std::wstring FullPathString(FullPathCharArray);
    if (!FullPathString.empty())
    {
        std::size_t StopIndex = FullPathString.find_last_of('\\') + 1;
        if (StopIndex != std::wstring::npos)
        {
            return FullPathString.substr(0, StopIndex);
        }
    }

    BoostLog::WriteError(L"Return wrong running path.");
    return L"Qing";
}

std::wstring QING_DLL ConvertErrorCodeToString(DWORD LastErrorCode)
{
    std::wstring ErrorCodeString;

    LPVOID lpMsgBuf;
    DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    if (FormatMessage(dwFlags, NULL, LastErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL))
    {
        ErrorCodeString.append((LPCTSTR)lpMsgBuf);
        std::wstring::size_type DeleteCharCount = 2;    //delete "\r\n"
        std::wstring::size_type StartIndex = ErrorCodeString.size() - DeleteCharCount;
        ErrorCodeString.erase(StartIndex, DeleteCharCount);
    }

    return ErrorCodeString;
}

std::wstring QING_DLL ConvertDoubleToString(double Value, int precision)
{
    std::wostringstream ConvertString;

    ConvertString.precision(precision);
    ConvertString.setf(std::ios::fixed);
    ConvertString << Value;

    return ConvertString.str();
}

std::string QING_DLL GetSHA1(const std::string & DataString, bool IsUpper)
{
    boost::uuids::detail::sha1 sha;
    sha.process_bytes(DataString.c_str(), DataString.size());

    const int DigestArraySize = 5;
    unsigned int DigestArray[DigestArraySize];
    sha.get_digest(DigestArray);

    std::ostringstream ConvertBuffer;
    for (int Index = 0; Index < DigestArraySize; ++Index)
    {
        ConvertBuffer << std::hex << std::setfill('0') << std::setw(8) << DigestArray[Index];
    }

    if (IsUpper)
    {
        std::string ResultString(ConvertBuffer.str());
        std::transform(ResultString.begin(), ResultString.end(), ResultString.begin(), ::toupper);
        return ResultString;
    }

    return std::string(ConvertBuffer.str());
}

std::wstring QING_DLL GetSHA1(const std::wstring & DataString, bool IsUpper)
{
    boost::uuids::detail::sha1 sha;
    sha.process_bytes(DataString.c_str(), DataString.size());

    const int DigestArraySize = 5;
    unsigned int DigestArray[DigestArraySize];
    sha.get_digest(DigestArray);

    std::wostringstream ConvertBuffer;
    for (int Index = 0; Index < DigestArraySize; ++Index)
    {
        ConvertBuffer << std::hex << std::setfill(L'0') << std::setw(8) << DigestArray[Index];
    }

    if (IsUpper)
    {
        std::wstring ResultString(ConvertBuffer.str());
        std::transform(ResultString.begin(), ResultString.end(), ResultString.begin(), ::toupper);
        return ResultString;
    }

    return std::wstring(ConvertBuffer.str());
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

static LONG WINAPI ProgramUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* ExceptionPointers)
{
    const std::wstring &DMPFileName = GetProgramName() + L".dmp";

    HANDLE hFile = CreateFile(DMPFileName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION Mdei;

        Mdei.ThreadId = GetCurrentThreadId();
        Mdei.ExceptionPointers = ExceptionPointers;
        Mdei.ClientPointers = FALSE;

        MINIDUMP_TYPE Mdt = (MINIDUMP_TYPE)(MiniDumpWithFullMemory |      //包含进程地址空间中所有可读页面的内容
            MiniDumpWithFullMemoryInfo |                                        //包含进程虚拟内存布局的完整信息
            MiniDumpWithHandleData |                                            //包含故障时刻进程故障表里面的所有句柄
            MiniDumpWithThreadInfo |                                            //包含进程中线程的附加信息
            MiniDumpWithUnloadedModules);                                      //包含所有模块信息

        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, Mdt, &Mdei, NULL, NULL);
        CloseHandle(hFile);
    }

    MessageBox(NULL, _T("Program is encountering a fatal error and going to shutdown"), _T("Crash Tip"), MB_OK | MB_ICONERROR);
    return EXCEPTION_EXECUTE_HANDLER;
}

void SetProgramDMPEnable(bool EnableProgramDMP)
{
    if (EnableProgramDMP)
    {
        SetUnhandledExceptionFilter(ProgramUnhandledExceptionFilter);
    }
}

void SplitString(const std::wstring & SourceString, std::vector<std::wstring>& StringVector, const std::wstring & Seperator)
{
    std::wstring::size_type pos1 = 0;
    std::wstring::size_type pos2 = SourceString.find(Seperator);

    while (std::wstring::npos != pos2)
    {
        StringVector.push_back(SourceString.substr(pos1, pos2 - pos1));

        pos1 = pos2 + Seperator.size();
        pos2 = SourceString.find(Seperator, pos1);
    }

    if (pos1 != SourceString.length())
    {
        StringVector.push_back(SourceString.substr(pos1));
    }
}

bool Base64Encode(const std::wstring &Input, std::wstring &Output)
{
    const std::string &TempInput = WStringToString(Input);
    const std::string &TempResult = boost::beast::detail::base64_encode(TempInput);
    Output = StringToWString(TempResult);

    return !Output.empty();
}

bool Base64Decode(const std::wstring &Input, std::wstring &Output)
{
    const std::string &TempInput = WStringToString(Input);
    const std::string &TempResult = boost::beast::detail::base64_decode(TempInput);
    Output = StringToWString(TempResult);

    return !Output.empty();
}

QING_NAMESPACE_END