#include "stdafx.h"
#include "SimpleCrypt.h"
#include "..\..\Qing\HeaderFiles\BoostLog.h"
#include "..\..\Qing\HeaderFiles\CommonFunction.h"



enum
{
    FILE_NAME = 0,
    FILE_SIZE = 1,
    SEPERATOR = 2,
    BUFFER_UNIT = 1024,                                 //1kB
    BUFFER_SIZE = BUFFER_UNIT * BUFFER_UNIT * 100,      //100MB
};



SimpleCrypt::SimpleCrypt()
{
    m_IsForceStop = false;
    m_IsEncryptFileName = false;
    m_IsDeleteOriginalFile = false;

    m_FileExtension = L".qing";
    m_Password = L"89ec5b3a9bc86ec1005c8d230e29db98d7a4a240";

    m_HeaderVector.push_back(L"FileName=");
    m_HeaderVector.push_back(L"FileSize=");
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

bool SimpleCrypt::Encrypt(const std::wstring &SourceFile, const std::wstring &TargetPath)
{
    if (IsEncrypt(SourceFile))
    {
        return true;
    }

    HANDLE SourceFileHandle = ::CreateFile(SourceFile.c_str(),
        GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
        Qing::BoostLog::WriteError(L"Encrypt failed, source file = " + SourceFile + L", error = " + m_ErrorMessage);
        return false;
    }

    const std::wstring &TargetFile = GetEncryptFileName(SourceFile, TargetPath);
    HANDLE TargetFileHandle = ::CreateFile(TargetFile.c_str(),
        GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (TargetFileHandle == INVALID_HANDLE_VALUE)
    {
        m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
        Qing::BoostLog::WriteError(L"Encrypt failed, target file = " + TargetFile + L", error = " + m_ErrorMessage);
        CloseHandle(SourceFileHandle);
        return false;
    }

    if (m_IsEncryptFileName)
    {
        if (!EncryptHeader(SourceFile, SourceFileHandle, TargetFileHandle))
        {
            CloseHandle(SourceFileHandle);
            CloseHandle(TargetFileHandle);
            return false;
        }
    }

    if (!EncryptDecryptFileData(SourceFileHandle, TargetFileHandle, 0))
    {
        CloseHandle(SourceFileHandle);
        CloseHandle(TargetFileHandle);
        return false;
    }

    CloseHandle(SourceFileHandle);
    CloseHandle(TargetFileHandle);

    Delete(SourceFile);
    return true;
}

bool SimpleCrypt::DeCrypt(const std::wstring & SourceFile, const std::wstring & TargetPath)
{
    if (!IsEncrypt(SourceFile))
    {
        return true;
    }

    HANDLE SourceFileHandle = ::CreateFile(SourceFile.c_str(),
        GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
        Qing::BoostLog::WriteError(L"DeCrypt failed, source file = " + SourceFile + L", error = " + m_ErrorMessage);
        return false;
    }

    const std::wstring &TargetFile = GetDecryptFileName(SourceFileHandle, SourceFile, TargetPath);
    HANDLE TargetFileHandle = ::CreateFile(TargetFile.c_str(),
        GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (TargetFileHandle == INVALID_HANDLE_VALUE)
    {
        m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
        Qing::BoostLog::WriteError(L"DeCrypt failed, target file = " + TargetFile + L", error = " + m_ErrorMessage);
        CloseHandle(SourceFileHandle);
        return false;
    }

    bool DecryptResult = EncryptDecryptFileData(SourceFileHandle, TargetFileHandle, BUFFER_UNIT);
    CloseHandle(SourceFileHandle);
    CloseHandle(TargetFileHandle);

    Delete(SourceFile);
    return DecryptResult;
}

bool SimpleCrypt::Delete(const std::wstring &SourceFile)
{
    if (!m_IsDeleteOriginalFile)
    {
        return true;
    }

    if (DeleteFile(SourceFile.c_str()))
    {
        return true;
    }

    m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
    Qing::BoostLog::WriteError(L"Delete failed, file = " + SourceFile + L", error = " + m_ErrorMessage);
    return false;
}

bool SimpleCrypt::IsEncrypt(const std::wstring &SourceFile)
{
    const std::wstring &Extension = PathFindExtension(SourceFile.c_str());
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

bool SimpleCrypt::EncryptHeader(const std::wstring &SourceFile, HANDLE SourceFileHandle, HANDLE TargetFileHandle)
{
    DWORD FileSizeHigh = 0;
    DWORD FileSize = GetFileSize(SourceFileHandle, &FileSizeHigh);
    if (FileSize == INVALID_FILE_SIZE)
    {
        m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
        Qing::BoostLog::WriteError(L"Encrypt header, get file = " + SourceFile + L" size failed, error = " + m_ErrorMessage);
        return false;
    }

    const std::wstring &FileName = PathFindFileName(SourceFile.c_str());
    std::wstring HeaderContext = m_HeaderVector[FILE_NAME] + FileName + m_HeaderVector[SEPERATOR] +
        m_HeaderVector[FILE_SIZE] + std::to_wstring(FileSize) + m_HeaderVector[SEPERATOR];
    if (HeaderContext.size() > BUFFER_UNIT)
    {
        m_ErrorMessage = L"Header context size is more than BUFFER_UNIT.";
        Qing::BoostLog::WriteError(L"Encrypt header, file = " + SourceFile + L" , error = " + m_ErrorMessage);
        return false;
    }

    wmemset(m_FileDataBuffer, 0, BUFFER_UNIT);
    wmemcpy(m_FileDataBuffer, HeaderContext.c_str(), HeaderContext.size());
    EncryptDecryptBuffer(m_FileDataBuffer, HeaderContext.size());

    DWORD RealWriteLength = 0;
    if (::WriteFile(TargetFileHandle, m_FileDataBuffer, BUFFER_UNIT, &RealWriteLength, NULL) <= 0)
    {
        m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
        Qing::BoostLog::WriteError(L"Encrypt header, write file = " + SourceFile + L"failed , error = " + m_ErrorMessage);
        return false;
    }

    return true;
}

bool SimpleCrypt::DecryptHeader(HANDLE SourceFileHandle, std::wstring &OriginalFile)
{
    DWORD RealReadLength = 0;
    wmemset(m_FileDataBuffer, 0, BUFFER_UNIT);
    if (::ReadFile(SourceFileHandle, m_FileDataBuffer, BUFFER_UNIT, &RealReadLength, NULL) <= 0)
    {
        m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
        Qing::BoostLog::WriteError(L"Decrypt Header, read file, error = " + m_ErrorMessage);
        return false;
    }

    EncryptDecryptBuffer(m_FileDataBuffer, BUFFER_UNIT);

    std::wstring HeaderContext;
    HeaderContext.assign(m_FileDataBuffer, BUFFER_UNIT);
    if (HeaderContext.find(m_HeaderVector[FILE_NAME]) == std::wstring::npos)
    {
        m_ErrorMessage = L"Can not find encrypt information.";
        Qing::BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    std::vector<std::wstring> SplitVector;
    Qing::SplitString(HeaderContext, SplitVector, m_HeaderVector[SEPERATOR]);
    if (SplitVector.empty())
    {
        m_ErrorMessage = L"Can not find encrypt information.";
        Qing::BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    std::wstring::size_type StartIndex = SplitVector[FILE_NAME].find(m_HeaderVector[FILE_NAME]) + m_HeaderVector[FILE_NAME].size();
    OriginalFile = SplitVector[FILE_NAME].substr(StartIndex, SplitVector[FILE_NAME].size() - StartIndex);
    return true;
}

bool SimpleCrypt::EncryptDecryptFileData(HANDLE SourceFileHandle, HANDLE TargetFileHandle, DWORD FileOffset)
{
    DWORD RealWriteLength = 0;
    DWORD RealReadLength = BUFFER_SIZE;

    while (RealReadLength == BUFFER_SIZE)
    {
        if (::SetFilePointer(SourceFileHandle, FileOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
            Qing::BoostLog::WriteError(L"EncryptDecryptFileData::SetFilePointer, error = " + m_ErrorMessage);
            return false;
        }

        if (m_IsForceStop)
        {
            break;
        }

        wmemset(m_FileDataBuffer, 0, sizeof(m_FileDataBuffer));
        if (::ReadFile(SourceFileHandle, m_FileDataBuffer, BUFFER_SIZE, &RealReadLength, NULL) <= 0)
        {
            m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
            Qing::BoostLog::WriteError(L"EncryptDecryptFileData::ReadFile, error = " + m_ErrorMessage);
            return false;
        }

        if (m_IsForceStop)
        {
            break;
        }

        EncryptDecryptBuffer(m_FileDataBuffer, RealReadLength);

        if (m_IsForceStop)
        {
            break;
        }

        if (::WriteFile(TargetFileHandle, m_FileDataBuffer, RealReadLength, &RealWriteLength, NULL) <= 0)
        {
            m_ErrorMessage = Qing::ConvertErrorCodeToString(GetLastError());
            Qing::BoostLog::WriteError(L"EncryptDecryptFileData::WriteFile, error = " + m_ErrorMessage);
            return false;
        }

        FileOffset += BUFFER_SIZE;
    }

    if (m_IsForceStop)
    {
        m_ErrorMessage = L"Force stop.";
        Qing::BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    return true;
}

std::wstring SimpleCrypt::GetEncryptFileName(const std::wstring & SourceFile, const std::wstring & TargetPath)
{
    std::wstring TargetName;
    if (m_IsEncryptFileName)
    {
        const std::wstring &FileName = PathFindFileName(SourceFile.c_str());
        const std::wstring &FileNameSHA1 = Qing::GetSHA1(FileName, true) + m_FileExtension;

        if (TargetPath.empty())
        {
            std::wstring NewFilePath(SourceFile);
            NewFilePath.replace(SourceFile.find(FileName.c_str()), FileName.size(), FileNameSHA1.c_str());

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
            TargetName = SourceFile;
            PathRemoveExtension((LPWSTR)TargetName.c_str());
        }
        else
        {
            std::wstring FileName = PathFindFileName(SourceFile.c_str());
            PathRemoveExtension((LPWSTR)FileName.c_str());
            TargetName = TargetPath + L"\\" + FileName;
        }

        PathAddExtension((LPWSTR)TargetName.c_str(), (LPCWSTR)m_FileExtension.c_str());
    }

    return TargetName;
}

std::wstring SimpleCrypt::GetDecryptFileName(HANDLE SourceFileHandle, const std::wstring &SourceFile, const std::wstring &TargetPath)
{
    std::wstring TargetName;
    std::wstring OriginalFileName;
    if (DecryptHeader(SourceFileHandle, OriginalFileName) || !OriginalFileName.empty())
    {
        if (TargetPath.empty())
        {
            std::wstring::size_type FileNameStartIndex = SourceFile.find_last_of(L"\\");
            if (FileNameStartIndex != std::wstring::npos)
            {
                TargetName = SourceFile.substr(0, FileNameStartIndex) + L"\\" + OriginalFileName;
            }
        }
        else
        {
            const std::wstring &FileName = PathFindFileName(OriginalFileName.c_str());
            TargetName = TargetPath + FileName;
        }
    }

    return TargetName;
}
