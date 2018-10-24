#include "stdafx.h"
#include "SimpleCrypt.h"
#include "..\..\Qing\HeaderFiles\BoostLog.h"
#include "..\..\Qing\HeaderFiles\CommonFunction.h"



SimpleCrypt::SimpleCrypt() : m_Password(L"SimpleCrypt"), m_BufferUnit(1024), m_BufferSize(m_BufferUnit * m_BufferUnit * 100)
{
    m_FileBuffer = new wchar_t[m_BufferSize];
}

SimpleCrypt::~SimpleCrypt()
{
    delete m_FileBuffer;
    m_FileBuffer = NULL;
}

bool SimpleCrypt::Encrypt(const std::wstring &SourceFileName, const std::wstring &TargetPath)
{
    HANDLE SourceFileHandle = ::CreateFile(SourceFileName.c_str(),       //File Name
        GENERIC_READ,                                                    //Read
        FILE_SHARE_READ,                                                 //Shared
        NULL,                                                            //No Focus
        OPEN_EXISTING,                                                   //File Must Exist
        FILE_ATTRIBUTE_NORMAL,                                           //Attribute
        NULL);                                                           //No Focus

    if (SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        Qing::BoostLog::WriteError(L"Encryption create read file handle is invalid handle.");
        return false;
    }

    const std::wstring &TargetName = GetEncryptFileName(SourceFileName, TargetPath);
    HANDLE TargetFileHandle = ::CreateFile(TargetName.c_str(),           //File Name
        GENERIC_WRITE,                                              //write
        FILE_SHARE_READ,                                            //Shared
        NULL,                                                       //No Focus
        CREATE_NEW,                                                 //create
        FILE_ATTRIBUTE_NORMAL,                                      //Attribute
        NULL);                                                      //No Focus

    if (TargetFileHandle == INVALID_HANDLE_VALUE)
    {
        int Error = GetLastError();
        CloseHandle(SourceFileHandle);
        Qing::BoostLog::WriteError(L"Encryption create target file handle is invalid handle.");
        return false;
    }

    DWORD FileSizeHigh = 0;
    DWORD FileSize = GetFileSize(SourceFileHandle, &FileSizeHigh);
    if (FileSize == INVALID_FILE_SIZE)
    {
        int Error = GetLastError();
        CloseHandle(SourceFileHandle);
        CloseHandle(TargetFileHandle);
        Qing::BoostLog::WriteError(L"Encryption get file size error.");
        return false;
    }

    DWORD FileOffset = 0;
    DWORD RealReadLength = 0;
    DWORD RealWriteLength = 0;

    //step 1, save file name
    memset(m_FileBuffer, 0, m_BufferUnit);
    memcpy(m_FileBuffer, SourceFileName.c_str(), SourceFileName.size());
    EncryptDecryptData(m_FileBuffer, static_cast<int>(SourceFileName.size()));
    if (::WriteFile(TargetFileHandle, m_FileBuffer, m_BufferUnit, &RealWriteLength, NULL) <= 0)
    {
        CloseHandle(SourceFileHandle);
        CloseHandle(TargetFileHandle);
        return false;
    }

    //step2, encrypt file context
    RealReadLength = m_BufferSize;
    while (RealReadLength == m_BufferSize)
    {
        if (::SetFilePointer(SourceFileHandle, FileOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            break;
        }

        memset(m_FileBuffer, 0, sizeof(m_FileBuffer));
        if (::ReadFile(SourceFileHandle, m_FileBuffer, m_BufferSize, &RealReadLength, NULL) <= 0)
        {
            break;
        }

        EncryptDecryptData(m_FileBuffer, RealReadLength);

        if (::WriteFile(TargetFileHandle, m_FileBuffer, RealReadLength, &RealWriteLength, NULL) <= 0)
        {
            break;
        }

        FileOffset += m_BufferSize;
    }

    CloseHandle(SourceFileHandle);
    CloseHandle(TargetFileHandle);
    return true;
}

bool SimpleCrypt::DeCrypt(const std::wstring & SourceFileName, const std::wstring & TargetFileName)
{
    HANDLE SourceFileHandle = ::CreateFile(SourceFileName.c_str(),       //File Name
        GENERIC_READ,                                                    //Read
        FILE_SHARE_READ,                                                 //Shared
        NULL,                                                            //No Focus
        OPEN_EXISTING,                                                   //File Must Exist
        FILE_ATTRIBUTE_NORMAL,                                           //Attribute
        NULL);                                                           //No Focus

    if (SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        Qing::BoostLog::WriteError(L"Encryption create read file handle is invalid handle.");
        return false;
    }

    HANDLE TargetFileHandle = ::CreateFile(TargetFileName.c_str(),       //File Name
        GENERIC_WRITE,                                                   //write
        FILE_SHARE_READ,                                                 //Shared
        NULL,                                                            //No Focus
        CREATE_NEW,                                                      //create
        FILE_ATTRIBUTE_NORMAL,                                           //Attribute
        NULL);                                                           //No Focus

    if (TargetFileHandle == INVALID_HANDLE_VALUE)
    {
        int Error = GetLastError();
        CloseHandle(SourceFileHandle);
        Qing::BoostLog::WriteError(L"Encryption create target file handle is invalid handle.");
        return false;
    }

    DWORD FileSizeHigh = 0;
    DWORD FileSize = GetFileSize(SourceFileHandle, &FileSizeHigh);
    if (FileSize == INVALID_FILE_SIZE)
    {
        int Error = GetLastError();
        CloseHandle(SourceFileHandle);
        CloseHandle(TargetFileHandle);
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
        if (::SetFilePointer(SourceFileHandle, FileOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            break;
        }

        memset(m_FileBuffer, 0, sizeof(m_FileBuffer));
        if (::ReadFile(SourceFileHandle, m_FileBuffer, m_BufferSize, &RealReadLength, NULL) <= 0)
        {
            break;
        }

        EncryptDecryptData(m_FileBuffer, RealReadLength);

        if (::WriteFile(TargetFileHandle, m_FileBuffer, RealReadLength, &RealWriteLength, NULL) <= 0)
        {
            break;
        }

        FileOffset += m_BufferSize;
    }

    CloseHandle(SourceFileHandle);
    CloseHandle(TargetFileHandle);
    return true;
}

bool SimpleCrypt::IsEncrypt(const std::wstring & SourceFileName) const
{
    if (PathFindSuffixArray(SourceFileName.c_str(), (const LPCWSTR*)L"qing", sizeof(L"qing")))
    {
        return true;
    }

    return false;
}

void SimpleCrypt::EncryptDecryptData(wchar_t * DataBuffer, int DataSize) const
{
    std::wstring::size_type PasswordIndex = 0;
    for (int Index = 0; Index < DataSize; Index++)
    {
        DataBuffer[Index] ^= m_Password[PasswordIndex];
        PasswordIndex = (PasswordIndex + 1) % m_Password.size();
    }
}

std::wstring SimpleCrypt::GetEncryptFileName(const std::wstring & SourceFileName, const std::wstring & TargetPath)
{
    const std::wstring Extension(L".qing");
    std::wstring TargetName;

    if (m_IsEncryptFileName)
    {
        const std::wstring &FileName = PathFindFileName(SourceFileName.c_str());
        const std::wstring &FileNameSHA1 = Qing::GetSHA1(FileName, true) + Extension;

        std::wstring NewFilePath(SourceFileName);
        NewFilePath.replace(SourceFileName.find(FileName.c_str()), FileName.size(), FileNameSHA1.c_str());

        TargetName = NewFilePath;
    }
    else
    {
        if (TargetPath.empty())
        {
            TargetName = SourceFileName;
            PathRemoveExtension((LPWSTR)TargetName.c_str());
            PathAddExtension((LPWSTR)TargetName.c_str(), (LPCWSTR)Extension.c_str());
        }
        else
        {
            std::wstring FileName = PathFindFileName(SourceFileName.c_str());
            PathRemoveExtension((LPWSTR)FileName.c_str());
            TargetName = TargetPath + L"\\" + FileName + Extension;
        }
    }

    return TargetName;
}
