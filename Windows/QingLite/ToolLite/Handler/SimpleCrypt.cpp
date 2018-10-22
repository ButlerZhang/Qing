#include "stdafx.h"
#include "SimpleCrypt.h"
#include "..\..\..\Qing\HeaderFiles\BoostLog.h"



SimpleCrypt::SimpleCrypt() : m_Password(L"SimpleCrypt")
{
}

SimpleCrypt::~SimpleCrypt()
{
    Reset();
}

void SimpleCrypt::Reset()
{
    CloseHandle(m_SourceFileHandle);
    CloseHandle(m_TargetFileHandle);

    m_SourceFileHandle = NULL;
    m_TargetFileHandle = NULL;

    delete m_FileBuffer;
    m_FileBuffer = NULL;

    m_Password.clear();
}

void SimpleCrypt::SetPassword(const std::wstring &Password)
{
    m_Password = Password;
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

    const int BufferUnit = 1024;
    const int BufferSize = BufferUnit * BufferUnit * 100;
    m_FileBuffer = new wchar_t[BufferSize];

    DWORD FileOffset = 0;
    DWORD RealReadLength = 0;
    DWORD RealWriteLength = 0;
    DWORD CurrentReadSize = BufferSize;

    //step 1
    memset(m_FileBuffer, 0, BufferUnit);
    memcpy(m_FileBuffer, SourceFileName.c_str(), SourceFileName.size());
    if (::WriteFile(m_TargetFileHandle, m_FileBuffer, BufferUnit, &RealWriteLength, NULL) <= 0)
    {
        return false;
    }

    //step 2
    memset(m_FileBuffer, 0, BufferUnit);
    const std::wstring &FileSizeString = std::to_wstring(FileSize);
    memcpy(m_FileBuffer, FileSizeString.c_str(), FileSizeString.size());
    if (::WriteFile(m_TargetFileHandle, m_FileBuffer, BufferUnit, &RealWriteLength, NULL) <= 0)
    {
        return false;
    }

    //step 3
    while (FileOffset < FileSize)
    {
        if (::SetFilePointer(m_SourceFileHandle, FileOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            break;
        }

        memset(m_FileBuffer, 0, sizeof(m_FileBuffer));
        if (::ReadFile(m_SourceFileHandle, m_FileBuffer, CurrentReadSize, &RealReadLength, NULL) <= 0)
        {
            break;
        }

        if (::WriteFile(m_TargetFileHandle, m_FileBuffer, CurrentReadSize, &RealWriteLength, NULL) <= 0)
        {
            break;
        }

        FileOffset += BufferSize;
        if (FileOffset + BufferSize > FileSize)
        {
            CurrentReadSize = FileSize - FileOffset;
        }
    }

    return true;
}

bool SimpleCrypt::DeCrypt(const std::wstring & SourceFileName, const std::wstring & TargetFileName)
{
    return false;
}

bool SimpleCrypt::IsEncrypt(const std::wstring & SourceFileName) const
{
    return false;
}
