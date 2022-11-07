#include "IniFile.h"
#include "../System/SystemShare.h"
#include "../../ProjectLinux/Share/Boost/BoostLog.h"



IniFile::IniFile()
{
    m_FileData = NULL;
}

IniFile::~IniFile()
{
    delete m_FileData;
    m_FileData = NULL;
}

bool IniFile::Open(const std::wstring &FileName)
{
    m_FileName = FileName;
    return _waccess(m_FileName.c_str(), 0) != -1;
}

bool IniFile::LoadData()
{
    HANDLE FileHandle = ::CreateFile(m_FileName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (FileHandle == INVALID_HANDLE_VALUE)
    {
        g_Log.WriteError(BoostFormat("Load data create file failed, error = %d.", GetLastError()));
        return false;
    }

    DWORD FileSizeHigh = 0;
    m_FileSize = GetFileSize(FileHandle, &FileSizeHigh);
    m_FileData = new wchar_t[m_FileSize + 1];
    memset(m_FileData, 0, sizeof(m_FileData));

    if (!::ReadFile(FileHandle, m_FileData, m_FileSize, &FileSizeHigh, 0) || m_FileSize != FileSizeHigh)
    {
        g_Log.WriteError(BoostFormat("Load data read file failed, error = %d.", GetLastError()));
        return false;
    }

    return true;
}

bool IniFile::LoadData(void *DataBuffer, int BufferSize, const std::wstring &DefaultFileName)
{
    if (DataBuffer == NULL || BufferSize <= 0)
    {
        g_Log.WriteError(BoostFormat("Load data buffer is empty and/or buffersize = %d.", BufferSize));
        return false;
    }

    wchar_t FilePath[_MAX_PATH + 1];
    if (::GetTempPathW(_MAX_PATH, FilePath) == 0)
    {
        g_Log.WriteError(BoostFormat("Load data get temp path failed, error = %d.", GetLastError()));
        return false;
    }

    m_FileName = FilePath;
    m_FileName += DefaultFileName.empty() ? GetGUID() : DefaultFileName;

    HANDLE FileHandle = ::CreateFileW(m_FileName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        0,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        0);

    if (FileHandle == INVALID_HANDLE_VALUE)
    {
        g_Log.WriteError(BoostFormat("Load data create file failed, error = %d.", GetLastError()));
        return false;
    }

    DWORD WriteLength = 0;
    bool Ret = ::WriteFile(FileHandle, DataBuffer, BufferSize, &WriteLength, 0) == TRUE ? true : false;

    ::CloseHandle(FileHandle);

    if (Ret)
    {
        DWORD FileSizeHigh = 0;
        m_FileSize = GetFileSize(FileHandle, &FileSizeHigh);
        m_FileData = new wchar_t[m_FileSize + 1];
        memset(m_FileData, 0, sizeof(m_FileData));
    }

    return Ret;
}

const wchar_t *IniFile::GetString(const wchar_t *Section, const wchar_t *Key, const wchar_t *DefaultValue)
{
    DWORD Count = ::GetPrivateProfileStringW(Section, Key, DefaultValue, m_FileData, m_FileSize, m_FileName.c_str());
    if (Count <= 0)
    {
        memset(m_FileData, 0, sizeof(m_FileData));
    }

    return m_FileData;
}

int IniFile::GetInt(const wchar_t *Section, const wchar_t *Key, int DefaultValue)
{
    DWORD Count = ::GetPrivateProfileStringW(Section, Key, 0, m_FileData, m_FileSize, m_FileName.c_str());
    return (Count > 0) ? _wtoi(m_FileData) : DefaultValue;
}

double IniFile::GetDouble(const wchar_t *Section, const wchar_t *Key, double DefaultValue)
{
    DWORD Count = ::GetPrivateProfileStringW(Section, Key, 0, m_FileData, m_FileSize, m_FileName.c_str());
    return (Count > 0) ? _wtof(m_FileData) : DefaultValue;
}

bool IniFile::GetBool(const wchar_t *Section, const wchar_t *Key, bool DefaultValue)
{
    DWORD Count = ::GetPrivateProfileStringW(Section, Key, 0, m_FileData, m_FileSize, m_FileName.c_str());
    return (Count > 0) ? (tolower(m_FileData[0]) == 'y') : DefaultValue;
}

inline bool IniFile::WriteString(const wchar_t *Section, const wchar_t *Key, const wchar_t *Value) const
{
    return (::WritePrivateProfileStringW(Section, Key, Value, m_FileName.c_str()) == TRUE);
}

inline bool IniFile::WriteInt(const wchar_t *Section, const wchar_t *Key, int Value) const
{
    return (::WritePrivateProfileStringW(Section, Key, std::to_wstring(Value).c_str(), m_FileName.c_str()) == TRUE);
}

inline bool IniFile::WriteDouble(const wchar_t *Section, const wchar_t *Key, double Value) const
{
    return (::WritePrivateProfileStringW(Section, Key, std::to_wstring(Value).c_str(), m_FileName.c_str()) == TRUE);
}

inline bool IniFile::WriteBool(const wchar_t *Section, const wchar_t *Key, bool Value) const
{
    return (::WritePrivateProfileStringW(Section, Key, Value ? L"Y" : L"N", m_FileName.c_str()) == TRUE);
}
