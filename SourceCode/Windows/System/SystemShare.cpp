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
        FORMAT_MESSAGE_FROM_SYSTEM |         //�����ϵͳ����Ĵ�������Ӧ���ַ���
        FORMAT_MESSAGE_IGNORE_INSERTS |      //�����ú���%ռλ������Ϣ
        FORMAT_MESSAGE_ALLOCATE_BUFFER,      //���������ı��������ڴ�
        NULL,                                       //ͨ��ΪNULL
        LastErrorCode,                              //�������
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),  //ʹ��ϵͳĬ������
        (LPTSTR)&lpMsgBuf,                          //����ı���Ϣ�Ļ�����
        0,                                          //��������С,��ָ��,����Ϊ��
        NULL))                                      //�����б�,ͨ������
    {
        ErrorString.append((LPCTSTR)lpMsgBuf);
        std::wstring::size_type DeleteCharCount = 2;//delete "\r\n"
        std::wstring::size_type StartIndex = ErrorString.size() - DeleteCharCount;
        ErrorString.erase(StartIndex, DeleteCharCount);

        LocalFree(lpMsgBuf);                        //ϵͳ�����ڴ棬��Ҫ�Լ��ͷ�
    }

    return ErrorString;
}

std::wstring StringToWString(const std::string &String, int Codepage)
{
    int nLen = MultiByteToWideChar(
        Codepage,               //��ʶ������ֽ��ַ���������һ������ҳֵ
        0,                      //������ж���Ŀ��ƣ���Ӱ���������
        (LPCSTR)String.c_str(), //Ҫת�����ַ���
        -1,                     //Ҫת�����ַ������ȣ������-1�������������ж�
        NULL,                   //ת���Ľ���Ļ�����
        0);                     //�������Ĵ�С�����Ϊ�㣬������ִ��ת���������ַ���

    std::wstring ResultWString;
    ResultWString.resize(nLen, L'\0');

    int nResult = MultiByteToWideChar(
        Codepage,               //һ��Ĭ��ΪCP_UTF8
        0,                      //һ�㲻��
        (LPCSTR)String.c_str(), //Դ�ַ���
        static_cast<int>(String.length()), //Դ�ַ�������
        (LPWSTR)ResultWString.c_str(),     //���������
        nLen);                  //�ɵ�һ�ε��û�õĻ�������С

    return ResultWString;
}

std::string  WStringToString(const std::wstring &WString, int Codepage)
{
    int nLen = WideCharToMultiByte(
        Codepage,               //��ʶ��Ҫ����ת�����ַ��������Ĵ���ҳ
        0,                      //ָ�������ת�����ƣ������������
        (LPCWSTR)WString.c_str(),//Ҫת�����ַ������ڴ��ַ
        -1,                     //�ַ������ȣ���-1�����ж�
        NULL,                   //���������
        0,                      //�����������С
        NULL,                   //��������ת�����ַ�ʱ��ʹ���������ָ����ַ�
        NULL);                  //ָ��һ����������������ڿ��ַ��ַ����У����
                                //������һ���ַ�����ת��Ϊ��Ӧ�Ķ��ֽ���ʽ������
                                //�ͻ�������������ΪTRUE������ΪFALSE

    std::string ResultString;
    ResultString.resize(nLen + 1, '\0');
    int WStringLength = static_cast<int>(WString.length()) + 1;

    int nResult = WideCharToMultiByte(
        Codepage,                   //һ��Ĭ��ΪCP_UTF8
        0,                          //һ�㲻��
        (LPCWSTR)WString.c_str(),   //Դ�ַ���
        WStringLength,              //Դ�ַ�������
        (LPSTR)ResultString.c_str(),//���������
        nLen + 1,                   //�����������С
        NULL,                       //ϵͳĬ��Ϊ�ʺ�
        NULL);                      //ͨ������NULL

    return ResultString;
}

void SplitCommandLine(std::vector<std::wstring> &CommandLineVector)
{
    int ArgsCount = 0;

    //CommandLineToArgvW����Unicode�汾
    PWSTR *ppArgv = CommandLineToArgvW(GetCommandLineW(), &ArgsCount);

    CommandLineVector.clear();
    CommandLineVector.assign(ppArgv, ppArgv + ArgsCount);

    HeapFree(GetProcessHeap(), 0, ppArgv);
}

void SplitEnvironment(std::vector<std::wstring>& EnvironmentVector)
{
    //��ȡ�������������ַ���:
    //(1)����GetEnvironmentStrings����;
    //(2)CUI�����main����TCHAR* env[]��
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

        MINIDUMP_TYPE Mdt = (MINIDUMP_TYPE)(MiniDumpWithFullMemory |      //�������̵�ַ�ռ������пɶ�ҳ�������
            MiniDumpWithFullMemoryInfo |                                        //�������������ڴ沼�ֵ�������Ϣ
            MiniDumpWithHandleData |                                            //��������ʱ�̽��̹��ϱ���������о��
            MiniDumpWithThreadInfo |                                            //�����������̵߳ĸ�����Ϣ
            MiniDumpWithUnloadedModules);                                      //��������ģ����Ϣ

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
