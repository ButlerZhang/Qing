#include "SystemShare.h"
#include "../../SourceCode/Common/Boost/BoostLog.h"
#include "../../SourceCode/Common/Standard/StandardShare.h"

#include <cctype>
#include <sstream>
#include <iomanip>
#include <Shlwapi.h>
#include <algorithm>
#include <functional>
#include <tchar.h>
#include <DbgHelp.h>
#include <strsafe.h>



std::wstring GetLastErrorString(DWORD LastErrorCode)
{
    LPVOID lpMsgBuf;
    std::wstring ErrorString;

    if (FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |         //获得与系统定义的错误代码对应的字符串
        FORMAT_MESSAGE_IGNORE_INSERTS |      //允许获得含有%占位符的消息
        FORMAT_MESSAGE_ALLOCATE_BUFFER,      //分配容纳文本描述的内存
        NULL,                                       //通常为NULL
        LastErrorCode,                              //错误代码
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  //使用系统默认语言
        (LPTSTR)&lpMsgBuf,                          //存放文本信息的缓冲区
        0,                                          //缓冲区大小,已指定,所以为零
        NULL))                                      //参数列表,通常不用
    {
        ErrorString.append((LPCTSTR)lpMsgBuf);
        std::wstring::size_type DeleteCharCount = 2;//delete "\r\n"
        std::wstring::size_type StartIndex = ErrorString.size() - DeleteCharCount;
        ErrorString.erase(StartIndex, DeleteCharCount);

        LocalFree(lpMsgBuf);                        //系统分配内存，需要自己释放
    }

    return ErrorString;
}

std::wstring StringToWString(const std::string &String, int Codepage)
{
    int nLen = MultiByteToWideChar(
        Codepage,               //标识了与多字节字符串关联的一个代码页值
        0,                      //允许进行额外的控制，会影响变音符号
        (LPCSTR)String.c_str(), //要转换的字符串
        -1,                     //要转换的字符串长度，如果传-1，函数可自行判断
        NULL,                   //转换的结果的缓冲区
        0);                     //缓冲区的大小，如果为零，函数不执行转换，返回字符数

    std::wstring ResultWString;
    ResultWString.resize(nLen, L'\0');

    int nResult = MultiByteToWideChar(
        Codepage,               //一般默认为CP_UTF8
        0,                      //一般不用
        (LPCSTR)String.c_str(), //源字符串
        static_cast<int>(String.length()), //源字符串长度
        (LPWSTR)ResultWString.c_str(),     //结果缓冲区
        nLen);                  //由第一次调用获得的缓冲区大小

    return ResultWString;
}

std::string  WStringToString(const std::wstring &WString, int Codepage)
{
    int nLen = WideCharToMultiByte(
        Codepage,               //标识了要与新转换的字符串关联的代码页
        0,                      //指定额外的转换控制，例如变音符号
        (LPCWSTR)WString.c_str(),//要转换的字符串的内存地址
        -1,                     //字符串长度，传-1自行判断
        NULL,                   //结果缓冲区
        0,                      //结果缓冲区大小
        NULL,                   //遇到不能转换的字符时会使用这个参数指向的字符
        NULL);                  //指向一个布尔变量，如果在宽字符字符串中，如果
                                //至少有一个字符不能转换为对应的多字节形式，函数
                                //就会把这个变量设置为TRUE，否则为FALSE

    std::string ResultString;
    ResultString.resize(nLen + 1, '\0');
    int WStringLength = static_cast<int>(WString.length()) + 1;

    int nResult = WideCharToMultiByte(
        Codepage,                   //一般默认为CP_UTF8
        0,                          //一般不用
        (LPCWSTR)WString.c_str(),   //源字符串
        WStringLength,              //源字符串长度
        (LPSTR)ResultString.c_str(),//结果缓冲区
        nLen + 1,                   //结果缓冲区大小
        NULL,                       //系统默认为问号
        NULL);                      //通常传入NULL

    return ResultString;
}

void SplitCommandLine(std::vector<std::wstring> &CommandLineVector)
{
    int ArgsCount = 0;

    //CommandLineToArgvW仅有Unicode版本
    PWSTR *ppArgv = CommandLineToArgvW(GetCommandLineW(), &ArgsCount);

    CommandLineVector.clear();
    CommandLineVector.assign(ppArgv, ppArgv + ArgsCount);

    HeapFree(GetProcessHeap(), 0, ppArgv);
}

void SplitEnvironment(std::vector<std::wstring>& EnvironmentVector)
{
    //获取环境变量有两种方法:
    //(1)采用GetEnvironmentStrings函数;
    //(2)CUI程序的main参数TCHAR* env[]。
    PTSTR pEnvBlock = GetEnvironmentStrings();

    PTSTR pszCurrent = pEnvBlock;
    while (pszCurrent != NULL && *pszCurrent != TEXT('\0'))
    {
        EnvironmentVector.push_back(pszCurrent);

        while (*pszCurrent != TEXT('\0'))
        {
            pszCurrent++;
        }

        pszCurrent++;
    }

    FreeEnvironmentStrings(pEnvBlock);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    g_Log.WriteError("Return wrong program name.");
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

    g_Log.WriteError("Return wrong running path.");
    return L"Qing";
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
