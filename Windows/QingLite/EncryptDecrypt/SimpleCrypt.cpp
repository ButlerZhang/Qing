#include "stdafx.h"
#include "SimpleCrypt.h"
#include "..\..\Qing\HeaderFiles\BoostLog.h"
#include "..\..\Qing\HeaderFiles\CommonFunction.h"



enum
{
    FILE_NAME = 0,
    FILE_SIZE = 1,
    PASSWORD = 2,
    SEPERATOR = 3,
    BUFFER_UNIT = 1024,                                 //1kB
    BUFFER_SIZE = BUFFER_UNIT * BUFFER_UNIT * 100,      //100MB
};



SimpleCrypt::SimpleCrypt()
{
    m_IsEncryptFileName = true;
    m_IsDeleteOriginalFile = false;

    m_FileExtension = L".qing";
    m_Password = L"89ec5b3a9bc86ec1005c8d230e29db98d7a4a240";

    m_HeaderVector.push_back(L"FileName=");
    m_HeaderVector.push_back(L"FileSize=");
    m_HeaderVector.push_back(L"Password=");
    m_HeaderVector.push_back(L"|");

    m_FileDataBuffer = new wchar_t[BUFFER_SIZE];
}

SimpleCrypt::~SimpleCrypt()
{
    delete m_FileDataBuffer;
    m_FileDataBuffer = NULL;
}

void SimpleCrypt::SetPassword(const std::wstring &Password)
{
    if (!Password.empty())
    {
        m_Password = Qing::GetSHA1(Password, true);
    }
}

bool SimpleCrypt::Encrypt(const std::wstring &SourceFileName, const std::wstring &TargetPath)
{
    if (IsEncrypt(SourceFileName))
    {
        return true;
    }

    HANDLE SourceFileHandle = ::CreateFile(SourceFileName.c_str(),       //File Name
        GENERIC_READ,                                                    //Read
        FILE_SHARE_READ,                                                 //Shared
        NULL,                                                            //No Focus
        OPEN_EXISTING,                                                   //File Must Exist
        FILE_ATTRIBUTE_NORMAL,                                           //Attribute
        NULL);                                                           //No Focus

    if (SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        int Error = GetLastError();
        Qing::BoostLog::WriteError(L"Encryption create read file handle is invalid handle.");
        return false;
    }

    const std::wstring &TargetName = GetEncryptFileName(SourceFileName, TargetPath);
    HANDLE TargetFileHandle = ::CreateFile(TargetName.c_str(),           //File Name
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

    if (m_IsEncryptFileName && !EncryptHeader(SourceFileName, SourceFileHandle, TargetFileHandle))
    {
        CloseHandle(SourceFileHandle);
        CloseHandle(TargetFileHandle);
        return false;
    }

    if (!EncryptDecryptFileData(SourceFileHandle, TargetFileHandle, 0))
    {
        CloseHandle(SourceFileHandle);
        CloseHandle(TargetFileHandle);
        return false;
    }

    CloseHandle(SourceFileHandle);
    CloseHandle(TargetFileHandle);

    Delete(SourceFileName);
    return true;
}

bool SimpleCrypt::DeCrypt(const std::wstring & SourceFileName, const std::wstring & TargetPath)
{
    if (!IsEncrypt(SourceFileName))
    {
        return true;
    }

    HANDLE SourceFileHandle = ::CreateFile(SourceFileName.c_str(),       //File Name
        GENERIC_READ,                                                    //Read
        FILE_SHARE_READ,                                                 //Shared
        NULL,                                                            //No Focus
        OPEN_EXISTING,                                                   //File Must Exist
        FILE_ATTRIBUTE_NORMAL,                                           //Attribute
        NULL);                                                           //No Focus

    if (SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        int Error = GetLastError();
        Qing::BoostLog::WriteError(L"Encryption create read file handle is invalid handle.");
        return false;
    }

    const std::wstring &TargetName = GetDecryptFileName(SourceFileHandle, SourceFileName, TargetPath);
    HANDLE TargetFileHandle = ::CreateFile(TargetName.c_str(),           //File Name
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

    bool DecryptResult = EncryptDecryptFileData(SourceFileHandle, TargetFileHandle, BUFFER_UNIT);
    CloseHandle(SourceFileHandle);
    CloseHandle(TargetFileHandle);

    Delete(SourceFileName);
    return DecryptResult;
}

bool SimpleCrypt::Delete(const std::wstring &SourceFileName) const
{
    if (!m_IsDeleteOriginalFile)
    {
        return true;
    }

    if (DeleteFile(SourceFileName.c_str()))
    {
        return true;
    }

    int Error = GetLastError();
    return false;
}

bool SimpleCrypt::IsEncrypt(const std::wstring & SourceFileName) const
{
    const std::wstring &Extension = PathFindExtension(SourceFileName.c_str());
    return Extension == m_FileExtension;
}

void SimpleCrypt::EncryptDecryptBuffer(wchar_t * DataBuffer, int DataSize) const
{
    std::wstring::size_type PasswordIndex = 0;
    for (int Index = 0; Index < DataSize; Index++)
    {
        DataBuffer[Index] ^= m_Password[PasswordIndex];
        PasswordIndex = (PasswordIndex + 1) % m_Password.size();
    }
}

bool SimpleCrypt::EncryptHeader(const std::wstring & SourceFileName, HANDLE SourceFileHandle, HANDLE TargetFileHandle)
{
    DWORD FileSizeHigh = 0;
    DWORD FileSize = GetFileSize(SourceFileHandle, &FileSizeHigh);
    if (FileSize == INVALID_FILE_SIZE)
    {
        int Error = GetLastError();
        return false;
    }

    std::wstring HeaderContext = m_HeaderVector[FILE_NAME] + SourceFileName + m_HeaderVector[SEPERATOR] +
        m_HeaderVector[FILE_SIZE] + std::to_wstring(FileSize) + m_HeaderVector[SEPERATOR] +
        m_HeaderVector[PASSWORD] + m_Password;
    if (HeaderContext.size() > BUFFER_UNIT)
    {
        return false;
    }

    wmemset(m_FileDataBuffer, 0, BUFFER_UNIT);
    wmemcpy(m_FileDataBuffer, HeaderContext.c_str(), HeaderContext.size());
    EncryptDecryptBuffer(m_FileDataBuffer, static_cast<int>(HeaderContext.size()));

    DWORD RealWriteLength = 0;
    if (::WriteFile(TargetFileHandle, m_FileDataBuffer, BUFFER_UNIT, &RealWriteLength, NULL) <= 0)
    {
        int Error = GetLastError();
        return false;
    }

    return true;
}

bool SimpleCrypt::DecryptHeader(HANDLE SourceFileHandle, std::wstring &OriginalFileName)
{
    DWORD RealReadLength = 0;
    wmemset(m_FileDataBuffer, 0, BUFFER_UNIT);
    if (::ReadFile(SourceFileHandle, m_FileDataBuffer, BUFFER_UNIT, &RealReadLength, NULL) <= 0)
    {
        int Error = GetLastError();
        return false;
    }

    EncryptDecryptBuffer(m_FileDataBuffer, BUFFER_UNIT);
    std::wstring HeaderContext(m_FileDataBuffer);
    if (HeaderContext.find(m_HeaderVector[FILE_NAME]) == std::wstring::npos)
    {
        return false;
    }

    std::vector<std::wstring> SplitVector;
    Qing::SplitString(HeaderContext, SplitVector, m_HeaderVector[SEPERATOR]);
    if (SplitVector.empty())
    {
        return false;
    }

    std::wstring::size_type StartIndex = SplitVector[PASSWORD].find(m_HeaderVector[PASSWORD]) + m_HeaderVector[PASSWORD].size();
    const std::wstring &Password = SplitVector[PASSWORD].substr(StartIndex, SplitVector[PASSWORD].size() - StartIndex);
    if (Password != m_Password)
    {
        return false;
    }

    StartIndex = SplitVector[FILE_NAME].find(m_HeaderVector[FILE_NAME]) + m_HeaderVector[FILE_NAME].size();
    OriginalFileName = SplitVector[FILE_NAME].substr(StartIndex, SplitVector[FILE_NAME].size() - StartIndex);
    return true;
}

bool SimpleCrypt::EncryptDecryptFileData(HANDLE SourceFileHandle, HANDLE TargetFileHandle, DWORD FileOffset)
{
    DWORD RealReadLength = 0;
    DWORD RealWriteLength = 0;

    RealReadLength = BUFFER_SIZE;
    while (RealReadLength == BUFFER_SIZE)
    {
        if (::SetFilePointer(SourceFileHandle, FileOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            int Error = GetLastError();
            return false;
        }

        wmemset(m_FileDataBuffer, 0, sizeof(m_FileDataBuffer));
        if (::ReadFile(SourceFileHandle, m_FileDataBuffer, BUFFER_SIZE, &RealReadLength, NULL) <= 0)
        {
            int Error = GetLastError();
            return false;
        }

        EncryptDecryptBuffer(m_FileDataBuffer, RealReadLength);
        if (::WriteFile(TargetFileHandle, m_FileDataBuffer, RealReadLength, &RealWriteLength, NULL) <= 0)
        {
            int Error = GetLastError();
            return false;
        }

        FileOffset += BUFFER_SIZE;
    }

    return true;
}

std::wstring SimpleCrypt::GetEncryptFileName(const std::wstring & SourceFileName, const std::wstring & TargetPath)
{
    std::wstring TargetName;
    if (m_IsEncryptFileName)
    {
        const std::wstring &FileName = PathFindFileName(SourceFileName.c_str());
        const std::wstring &FileNameSHA1 = Qing::GetSHA1(FileName, true) + m_FileExtension;

        if (TargetPath.empty())
        {
            std::wstring NewFilePath(SourceFileName);
            NewFilePath.replace(SourceFileName.find(FileName.c_str()), FileName.size(), FileNameSHA1.c_str());

            TargetName = NewFilePath;
        }
        else
        {
            TargetName = TargetPath + FileNameSHA1;
        }
    }
    else
    {
        if (TargetPath.empty())
        {
            TargetName = SourceFileName;
            PathRemoveExtension((LPWSTR)TargetName.c_str());
            PathAddExtension((LPWSTR)TargetName.c_str(), (LPCWSTR)m_FileExtension.c_str());
        }
        else
        {
            std::wstring FileName = PathFindFileName(SourceFileName.c_str());
            PathRemoveExtension((LPWSTR)FileName.c_str());
            TargetName = TargetPath + L"\\" + FileName + m_FileExtension;
        }
    }

    return TargetName;
}

std::wstring SimpleCrypt::GetDecryptFileName(HANDLE SourceFileHandle, const std::wstring &SourceFileName, const std::wstring &TargetPath)
{
    std::wstring TargetName;
    std::wstring OriginalFileName;
    if (DecryptHeader(SourceFileHandle, OriginalFileName) || !OriginalFileName.empty())
    {
        if (TargetPath.empty())
        {
            TargetName = OriginalFileName;
        }
        else
        {
            const std::wstring &FileName = PathFindFileName(OriginalFileName.c_str());
            TargetName = TargetPath + FileName;
        }
    }
    else
    {
        if (TargetPath.empty())
        {
            TargetName = SourceFileName;//error
        }
        else
        {
            const std::wstring &FileName = PathFindFileName(SourceFileName.c_str());
            TargetName = TargetPath + FileName;
        }
    }

    return TargetName;
}
