#include "..\HeaderFiles\SimpleEncrypt.h"
#include "..\HeaderFiles\BoostLog.h"
#include "..\HeaderFiles\CommonFunction.h"

#include <Shlwapi.h>
#include <iostream>
#include <sstream>

QING_NAMESPACE_BEGIN



enum
{
    FILE_NAME = 0,
    FILE_SIZE = 1,
    SEPERATOR = 2,
    BUFFER_UNIT = 1024,                                 //1kB
    BUFFER_SIZE = BUFFER_UNIT * BUFFER_UNIT * 10,       //10MB
};



SimpleEncrypt::SimpleEncrypt()
{
    m_IsForceStop = false;
    m_IsEncryptFileName = false;
    m_IsDeleteOriginalFile = false;

    m_FileSize = 0;
    m_DataBufferSize = 0;
    m_FileDataBuffer = NULL;

    m_FileEncryptExtension = L".qing";
    m_FileDisguiseExtension = L".ling";
    m_Password = L"89ec5b3a9bc86ec1005c8d230e29db98d7a4a240";

    m_HeaderVector.push_back(L"FileName=");
    m_HeaderVector.push_back(L"FileSize=");
    m_HeaderVector.push_back(L"|");
}

SimpleEncrypt::~SimpleEncrypt()
{
    delete m_FileDataBuffer;
    m_FileDataBuffer = NULL;
}

void SimpleEncrypt::SetPassword(const std::wstring &Password)
{
    if (!Password.empty())
    {
        m_Password = Qing::GetSHA1(Password, true);
    }
}

bool SimpleEncrypt::Encrypt(const std::wstring &SourceFile, const std::wstring &TargetPath)
{
    if (IsEncrypt(SourceFile))
    {
        return true;
    }

    HANDLE SourceFileHandle = ::CreateFile(SourceFile.c_str(),
        GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"Encrypt failed, source file = " + SourceFile + L", error = " + m_ErrorMessage);
        return false;
    }

    if (!Prepare(SourceFileHandle, SourceFile))
    {
        return false;
    }

    const std::wstring &TargetFile = GetEncryptFileName(SourceFile, TargetPath);
    HANDLE TargetFileHandle = ::CreateFile(TargetFile.c_str(),
        GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (TargetFileHandle == INVALID_HANDLE_VALUE)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"Encrypt failed, target file = " + TargetFile + L", error = " + m_ErrorMessage);
        CloseHandle(SourceFileHandle);
        return false;
    }

    if (!EncryptHeader(SourceFile, SourceFileHandle, TargetFileHandle))
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

    if (m_IsForceStop)
    {
        Delete(TargetFile);
        return false;
    }

    if (m_IsDeleteOriginalFile)
    {
        Delete(SourceFile);
    }

    return true;
}

bool SimpleEncrypt::DeCrypt(const std::wstring & SourceFile, const std::wstring & TargetPath)
{
    if (!IsEncrypt(SourceFile))
    {
        return true;
    }

    HANDLE SourceFileHandle = ::CreateFile(SourceFile.c_str(),
        GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (SourceFileHandle == INVALID_HANDLE_VALUE)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"DeCrypt failed, source file = " + SourceFile + L", error = " + m_ErrorMessage);
        return false;
    }

    if (!Prepare(SourceFileHandle, SourceFile))
    {
        return false;
    }

    const std::wstring &TargetFile = GetDecryptFileName(SourceFileHandle, SourceFile, TargetPath);
    HANDLE TargetFileHandle = ::CreateFile(TargetFile.c_str(),
        GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (TargetFileHandle == INVALID_HANDLE_VALUE)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"DeCrypt failed, target file = " + TargetFile + L", error = " + m_ErrorMessage);
        CloseHandle(SourceFileHandle);
        return false;
    }

    bool DecryptResult = EncryptDecryptFileData(SourceFileHandle, TargetFileHandle, BUFFER_UNIT);
    CloseHandle(SourceFileHandle);
    CloseHandle(TargetFileHandle);

    if (m_IsForceStop)
    {
        Delete(TargetFile);
        return false;
    }

    if (m_IsDeleteOriginalFile)
    {
        Delete(SourceFile);
    }

    return DecryptResult;
}

bool SimpleEncrypt::Disguise(const std::wstring &SourceFile)
{
    if (IsDisguise(SourceFile))
    {
        return true;
    }

    std::wstring RecoveryName;
    const std::wstring &SourceName = PathFindFileName(SourceFile.c_str());
    if (!Base64Encode(SourceName, RecoveryName))
    {
        m_ErrorMessage = L"Base 64 encode failed.";
        BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    RecoveryName += m_FileDisguiseExtension;
    std::wstring TargetFile(SourceFile);
    TargetFile.replace(SourceFile.find(SourceName.c_str()), SourceName.size(), RecoveryName.c_str());

    if (_wrename(SourceFile.c_str(), TargetFile.c_str()) != 0)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"Disguise failed, source file = " + SourceFile + L", error = " + m_ErrorMessage);
        return false;
    }

    return true;
}

bool SimpleEncrypt::Recovery(const std::wstring &SourceFile)
{
    if (!IsDisguise(SourceFile))
    {
        return true;
    }

    std::wstring SourceName = PathFindFileName(SourceFile.c_str());
    PathRemoveExtension((LPWSTR)SourceName.c_str());

    std::wstring RecoveryName;
    if(!Base64Decode(SourceName, RecoveryName))
    {
        m_ErrorMessage = L"Base 64 decode failed.";
        BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    std::wstring TargetFile(SourceFile);
    TargetFile.replace(SourceFile.find(SourceName.c_str()), SourceName.size(), RecoveryName.c_str());

    if (_wrename(SourceFile.c_str(), TargetFile.c_str()) != 0)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"Disguise failed, source file = " + SourceFile + L", error = " + m_ErrorMessage);
        return false;
    }

    return true;
}

bool SimpleEncrypt::Delete(const std::wstring &SourceFile)
{
    if (!DeleteFile(SourceFile.c_str()))
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"Delete failed, file = " + SourceFile + L", error = " + m_ErrorMessage);
        return false;
    }

    return true;
}

bool SimpleEncrypt::Prepare(HANDLE SourceFileHandle, const std::wstring &SourceFile)
{
    m_FileSize = 0;
    m_ErrorMessage.clear();

    DWORD FileSizeHigh = 0;
    m_FileSize = GetFileSize(SourceFileHandle, &FileSizeHigh);
    if (m_FileSize == INVALID_FILE_SIZE)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"Prepare init file size error = " + m_ErrorMessage);
        return false;
    }

    if (!IsSpaceEnough(m_FileSize, SourceFile))
    {
        return false;
    }

    if (m_DataBufferSize >= m_FileSize || m_DataBufferSize == BUFFER_SIZE)
    {
        return true;
    }

    unsigned long OldBufferSize = m_DataBufferSize;
    m_DataBufferSize = min(m_FileSize, BUFFER_SIZE);
    if (m_DataBufferSize % BUFFER_UNIT != 0)
    {
        int Counter = m_DataBufferSize / BUFFER_UNIT + 1;
        m_DataBufferSize = BUFFER_UNIT * Counter;
    }

    delete m_FileDataBuffer;
    m_FileDataBuffer = new wchar_t[m_DataBufferSize];

    std::wstring LogString(L"Recreate buffer, old size = " + std::to_wstring(OldBufferSize));
    LogString.append(L", new size = " + std::to_wstring(m_DataBufferSize));
    BoostLog::WriteInfo(LogString);
    return true;
}

bool SimpleEncrypt::IsEncrypt(const std::wstring &SourceFile) const
{
    const std::wstring &Extension = PathFindExtension(SourceFile.c_str());
    return Extension == m_FileEncryptExtension;
}

bool SimpleEncrypt::IsDisguise(const std::wstring & SourceFile) const
{
    const std::wstring &Extension = PathFindExtension(SourceFile.c_str());
    return Extension == m_FileDisguiseExtension;
}

bool SimpleEncrypt::IsSpaceEnough(unsigned long FileSize, std::wstring SourceFile)
{
    PathStripToRoot((LPWSTR)SourceFile.c_str());
    if (SourceFile.empty())
    {
        m_ErrorMessage = L"Can not find root directory.";
        BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    DWORD64 qwFreeBytes, qwFreeBytesToCaller, qwTotalBytes;
    BOOL bResult = GetDiskFreeSpaceEx(SourceFile.c_str(),
        (PULARGE_INTEGER)&qwFreeBytesToCaller,              //磁盘总容量
        (PULARGE_INTEGER)&qwTotalBytes,                     //可用的磁盘空间容量
        (PULARGE_INTEGER)&qwFreeBytes);                     //磁盘空闲容量

    if (FileSize >= qwFreeBytes)
    {
        m_ErrorMessage = L"Not enough free disk space.";
        BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    return true;
}

void SimpleEncrypt::EncryptDecryptBuffer(wchar_t * DataBuffer, int DataSize) const
{
    std::wstring::size_type PasswordIndex = 0;
    for (int Index = 0; Index < DataSize; Index++)
    {
        DataBuffer[Index] ^= m_Password[PasswordIndex];
        PasswordIndex = (PasswordIndex + 1) % m_Password.size();
    }
}

bool SimpleEncrypt::EncryptDecryptFileData(HANDLE SourceFileHandle, HANDLE TargetFileHandle, DWORD FileOffset)
{
    DWORD RealWriteLength = 0;
    DWORD RealReadLength = m_DataBufferSize;

    while (RealReadLength == m_DataBufferSize)
    {
        if (::SetFilePointer(SourceFileHandle, FileOffset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
            BoostLog::WriteError(L"EncryptDecryptFileData::SetFilePointer, error = " + m_ErrorMessage);
            return false;
        }

        if (m_IsForceStop)
        {
            break;
        }

        wmemset(m_FileDataBuffer, 0, sizeof(m_FileDataBuffer));
        if (::ReadFile(SourceFileHandle, m_FileDataBuffer, m_DataBufferSize, &RealReadLength, NULL) <= 0)
        {
            m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
            BoostLog::WriteError(L"EncryptDecryptFileData::ReadFile, error = " + m_ErrorMessage);
            return false;
        }

        if (m_IsForceStop || RealReadLength <= 0)
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
            m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
            BoostLog::WriteError(L"EncryptDecryptFileData::WriteFile, error = " + m_ErrorMessage);
            return false;
        }

        FileOffset += m_DataBufferSize;
    }

    if (m_IsForceStop)
    {
        m_ErrorMessage = L"Force stop.";
        BoostLog::WriteError(m_ErrorMessage);
    }

    return true;
}

bool SimpleEncrypt::EncryptHeader(const std::wstring &SourceFile, HANDLE SourceFileHandle, HANDLE TargetFileHandle)
{
    const std::wstring &FileName = PathFindFileName(SourceFile.c_str());
    std::wstring HeaderContext = m_HeaderVector[FILE_NAME] + FileName + m_HeaderVector[SEPERATOR] +
        m_HeaderVector[FILE_SIZE] + std::to_wstring(m_FileSize) + m_HeaderVector[SEPERATOR];
    if (HeaderContext.size() > BUFFER_UNIT)
    {
        m_ErrorMessage = L"Header context size is more than BUFFER_UNIT.";
        BoostLog::WriteError(L"Encrypt header, file = " + SourceFile + L" , error = " + m_ErrorMessage);
        return false;
    }

    wmemset(m_FileDataBuffer, 0, BUFFER_UNIT);
    wmemcpy(m_FileDataBuffer, HeaderContext.c_str(), HeaderContext.size());
    EncryptDecryptBuffer(m_FileDataBuffer, static_cast<int>(HeaderContext.size()));

    DWORD RealWriteLength = 0;
    if (::WriteFile(TargetFileHandle, m_FileDataBuffer, BUFFER_UNIT, &RealWriteLength, NULL) <= 0)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"Encrypt header, write file = " + SourceFile + L"failed , error = " + m_ErrorMessage);
        return false;
    }

    return true;
}

bool SimpleEncrypt::DecryptHeader(HANDLE SourceFileHandle, std::wstring &OriginalFile)
{
    DWORD RealReadLength = 0;
    wmemset(m_FileDataBuffer, 0, BUFFER_UNIT);
    if (::ReadFile(SourceFileHandle, m_FileDataBuffer, BUFFER_UNIT, &RealReadLength, NULL) <= 0)
    {
        m_ErrorMessage = Qing::GetLastErrorString(GetLastError());
        BoostLog::WriteError(L"Decrypt Header, read file, error = " + m_ErrorMessage);
        return false;
    }

    EncryptDecryptBuffer(m_FileDataBuffer, BUFFER_UNIT);

    std::wstring HeaderContext;
    HeaderContext.assign(m_FileDataBuffer, BUFFER_UNIT);
    if (HeaderContext.find(m_HeaderVector[FILE_NAME]) == std::wstring::npos)
    {
        m_ErrorMessage = L"Can not find encrypt information.";
        BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    std::vector<std::wstring> SplitVector;
    Qing::SplitString(HeaderContext, SplitVector, m_HeaderVector[SEPERATOR]);
    if (SplitVector.empty())
    {
        m_ErrorMessage = L"Can not find encrypt information.";
        BoostLog::WriteError(m_ErrorMessage);
        return false;
    }

    std::wstring::size_type StartIndex = SplitVector[FILE_NAME].find(m_HeaderVector[FILE_NAME]) + m_HeaderVector[FILE_NAME].size();
    OriginalFile = SplitVector[FILE_NAME].substr(StartIndex, SplitVector[FILE_NAME].size() - StartIndex);
    return true;
}

std::wstring SimpleEncrypt::GetEncryptFileName(const std::wstring &SourceFile, const std::wstring &TargetPath) const
{
    std::wstring TargetName;
    if (m_IsEncryptFileName)
    {
        const std::wstring &FileName = PathFindFileName(SourceFile.c_str());
        const std::wstring &FileNameSHA1 = Qing::GetSHA1(FileName, true) + m_FileEncryptExtension;

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
            TargetName = TargetPath + FileName;
        }

        PathAddExtension((LPWSTR)TargetName.c_str(), (LPCWSTR)m_FileEncryptExtension.c_str());
    }

    return TargetName;
}

std::wstring SimpleEncrypt::GetDecryptFileName(HANDLE SourceFileHandle, const std::wstring &SourceFile, const std::wstring &TargetPath)
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

QING_NAMESPACE_END