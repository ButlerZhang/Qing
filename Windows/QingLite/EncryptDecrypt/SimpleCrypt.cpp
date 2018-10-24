#include "stdafx.h"
#include "SimpleCrypt.h"
#include "..\..\Qing\HeaderFiles\BoostLog.h"



SimpleCrypt::SimpleCrypt() : m_Password(L"SimpleCrypt"), m_BufferUnit(1024), m_BufferSize(m_BufferUnit * m_BufferUnit * 100)
{
    m_FileBuffer = new wchar_t[m_BufferSize];
}

SimpleCrypt::~SimpleCrypt()
{
    CloseHandle(m_SourceFileHandle);
    CloseHandle(m_TargetFileHandle);

    m_SourceFileHandle = NULL;
    m_TargetFileHandle = NULL;

    delete m_FileBuffer;
    m_FileBuffer = NULL;
}

bool SimpleCrypt::Encrypt(const std::wstring &SourceFileName, const std::wstring &TargetFileName)
{
    m_SourceFileHandle = ::CreateFile(SourceFileName.c_str(),       //File Name
        GENERIC_READ,                                               //Read
        FILE_SHARE_READ,                                            //Shared
        NULL,                                                       //No Focus
        OPEN_EXISTING,                                              //File Must Exist
        FILE_ATTRIBUTE_NORMAL,                                      //Attribute
        NULL);                                                      //No Focus

    if (m_SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        Qing::BoostLog::WriteError(L"Encryption create read file handle is invalid handle.");
        return false;
    }

    m_TargetFileHandle = ::CreateFile(TargetFileName.c_str(),       //File Name
        GENERIC_WRITE,                                              //write
        FILE_SHARE_READ,                                            //Shared
        NULL,                                                       //No Focus
        CREATE_NEW,                                                 //create
        FILE_ATTRIBUTE_NORMAL,                                      //Attribute
        NULL);                                                      //No Focus

    if (m_TargetFileHandle == INVALID_HANDLE_VALUE)
    {
        int Error = GetLastError();
        Qing::BoostLog::WriteError(L"Encryption create target file handle is invalid handle.");
        return false;
    }

    DWORD FileSizeHigh = 0;
    DWORD FileSize = GetFileSize(m_SourceFileHandle, &FileSizeHigh);
    if (FileSize == INVALID_FILE_SIZE)
    {
        int Error = GetLastError();
        Qing::BoostLog::WriteError(L"Encryption get file size error.");
        return false;
    }

    DWORD FileOffset = 0;
    DWORD RealReadLength = 0;
    DWORD RealWriteLength = 0;

    memset(m_FileBuffer, 0, m_BufferUnit);
    memcpy(m_FileBuffer, SourceFileName.c_str(), SourceFileName.size());
    if (::WriteFile(m_TargetFileHandle, m_FileBuffer, m_BufferUnit, &RealWriteLength, NULL) <= 0)
    {
        return false;
    }

    RealReadLength = m_BufferSize;
    while (RealReadLength == m_BufferSize)
    {
        if (::SetFilePointer(m_SourceFileHandle, FileOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            break;
        }

        memset(m_FileBuffer, 0, sizeof(m_FileBuffer));
        if (::ReadFile(m_SourceFileHandle, m_FileBuffer, m_BufferSize, &RealReadLength, NULL) <= 0)
        {
            break;
        }

        EncryptData(m_FileBuffer, RealReadLength);

        if (::WriteFile(m_TargetFileHandle, m_FileBuffer, RealReadLength, &RealWriteLength, NULL) <= 0)
        {
            break;
        }

        FileOffset += m_BufferSize;
    }

    return true;
}

bool SimpleCrypt::DeCrypt(const std::wstring & SourceFileName, const std::wstring & TargetFileName)
{
    m_SourceFileHandle = ::CreateFile(SourceFileName.c_str(),       //File Name
        GENERIC_READ,                                               //Read
        FILE_SHARE_READ,                                            //Shared
        NULL,                                                       //No Focus
        OPEN_EXISTING,                                              //File Must Exist
        FILE_ATTRIBUTE_NORMAL,                                      //Attribute
        NULL);                                                      //No Focus

    if (m_SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        Qing::BoostLog::WriteError(L"Encryption create read file handle is invalid handle.");
        return false;
    }

    m_TargetFileHandle = ::CreateFile(TargetFileName.c_str(),       //File Name
        GENERIC_WRITE,                                              //write
        FILE_SHARE_READ,                                            //Shared
        NULL,                                                       //No Focus
        CREATE_NEW,                                                 //create
        FILE_ATTRIBUTE_NORMAL,                                      //Attribute
        NULL);                                                      //No Focus

    if (m_TargetFileHandle == INVALID_HANDLE_VALUE)
    {
        int Error = GetLastError();
        Qing::BoostLog::WriteError(L"Encryption create target file handle is invalid handle.");
        return false;
    }

    DWORD FileSizeHigh = 0;
    DWORD FileSize = GetFileSize(m_SourceFileHandle, &FileSizeHigh);
    if (FileSize == INVALID_FILE_SIZE)
    {
        int Error = GetLastError();
        Qing::BoostLog::WriteError(L"Encryption get file size error.");
        return false;
    }

    DWORD FileOffset = 0;
    DWORD RealReadLength = 0;
    DWORD RealWriteLength = 0;

    FileOffset = m_BufferUnit;
    RealReadLength = m_BufferSize;
    while (RealReadLength == m_BufferSize)
    {
        if (::SetFilePointer(m_SourceFileHandle, FileOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            break;
        }

        memset(m_FileBuffer, 0, sizeof(m_FileBuffer));
        if (::ReadFile(m_SourceFileHandle, m_FileBuffer, m_BufferSize, &RealReadLength, NULL) <= 0)
        {
            break;
        }

        DecryptData(m_FileBuffer, RealReadLength);

        if (::WriteFile(m_TargetFileHandle, m_FileBuffer, RealReadLength, &RealWriteLength, NULL) <= 0)
        {
            break;
        }

        FileOffset += m_BufferSize;
    }

    return true;
}

bool SimpleCrypt::IsEncrypt(const std::wstring & SourceFileName) const
{
    return false;
}

void SimpleCrypt::EncryptData(wchar_t * DataBuffer, int DataSize) const
{
    std::wstring::size_type PasswordIndex = 0;
    for (int Index = 0; Index < DataSize; Index++)
    {
        DataBuffer[Index] += m_Password[PasswordIndex];
        PasswordIndex = (PasswordIndex + 1) % m_Password.size();
    }
}

void SimpleCrypt::DecryptData(wchar_t * DataBuffer, int DataSize) const
{
    std::wstring::size_type PasswordIndex = 0;
    for (int Index = 0; Index < DataSize; Index++)
    {
        DataBuffer[Index] -= m_Password[PasswordIndex];
        PasswordIndex = (PasswordIndex + 1) % m_Password.size();
    }
}
