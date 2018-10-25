#pragma once
#include <string>
#include <vector>
#include <Windows.h>



class SimpleCrypt
{
public:

    SimpleCrypt();
    ~SimpleCrypt();

    void SetPassword(const std::wstring &Password) { m_Password = Password; }
    void SetFileExtension(const std::wstring &FileExtension) { m_FileExtension = FileExtension; }
    void SetIsEncryptFileName(bool IsEncryptFileName) { m_IsEncryptFileName = IsEncryptFileName; }
    void SetIsDeleteOriginalFile(bool IsDeleteOriginalFile) { m_IsDeleteOriginalFile = IsDeleteOriginalFile; }

    bool Encrypt(const std::wstring &SourceFileName, const std::wstring &TargetPath);
    bool DeCrypt(const std::wstring &SourceFileName, const std::wstring &TargetPath);

private:

    bool IsEncrypt(const std::wstring &SourceFileName) const;
    void EncryptDecryptBuffer(wchar_t *DataBuffer, int DataSize) const;
    bool EncryptDecryptFileData(HANDLE SourceFileHandle, HANDLE TargetFileHandle, DWORD FileOffset);

    bool EncryptHeader(const std::wstring &SourceFileName, HANDLE SourceFileHandle, HANDLE TargetFileHandle);
    bool DecryptHeader(HANDLE SourceFileHandle, std::wstring &OriginalFileName);

    std::wstring GetEncryptFileName(const std::wstring &SourceFileName, const std::wstring &TargetPath);
    std::wstring GetDecryptFileName(HANDLE SourceFileHandle, const std::wstring &SourceFileName, const std::wstring &TargetPath);

private:

    enum
    {
        FILE_NAME = 0,
        FILE_SIZE = 1,
        SEPERATOR = 2,
        BUFFER_UNIT = 1024,                                 //1kB
        BUFFER_SIZE = BUFFER_UNIT * BUFFER_UNIT * 100,      //100MB
    };

private:

    bool                                m_IsEncryptFileName;
    bool                                m_IsDeleteOriginalFile;
    std::wstring                        m_Password;
    std::wstring                        m_FileExtension;
    std::vector<std::wstring>           m_HeaderVector;
    wchar_t                            *m_FileDataBuffer;
};
