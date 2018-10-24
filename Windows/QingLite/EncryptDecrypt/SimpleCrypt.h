#pragma once
#include <string>
#include <memory>
#include <Windows.h>



class SimpleCrypt
{
public:

    SimpleCrypt();
    ~SimpleCrypt();

    void SetPassword(const std::wstring &Password) { m_Password = Password; }
    void SetIsEncryptFileName(bool IsEncryptFileName) { m_IsEncryptFileName = IsEncryptFileName; }
    void SetIsDeleteOriginalFile(bool IsDeleteOriginalFile) { m_IsDeleteOriginalFile = IsDeleteOriginalFile; }

    bool Encrypt(const std::wstring &SourceFileName, const std::wstring &TargetPath);
    bool DeCrypt(const std::wstring &SourceFileName, const std::wstring &TargetPath);

private:

    bool IsEncrypt(const std::wstring &SourceFileName) const;
    void EncryptDecryptData(wchar_t *DataBuffer, int DataSize) const;
    std::wstring GetEncryptFileName(const std::wstring &SourceFileName, const std::wstring &TargetPath);

private:

    const int                           m_BufferUnit;
    const int                           m_BufferSize;
    bool                                m_IsEncryptFileName;
    bool                                m_IsDeleteOriginalFile;
    std::wstring                        m_Password;
    wchar_t                            *m_FileBuffer;
};
