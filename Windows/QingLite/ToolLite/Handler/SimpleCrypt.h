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

    bool Encrypt(const std::wstring &SourceFileName, const std::wstring &TargetFileName);
    bool DeCrypt(const std::wstring &SourceFileName, const std::wstring &TargetFileName);

private:

    bool IsEncrypt(const std::wstring &SourceFileName) const;
    void EncryptData(wchar_t *DataBuffer, int DataSize) const;
    void DecryptData(wchar_t *DataBuffer, int DataSize) const;

private:

    const int                           m_BufferUnit;
    const int                           m_BufferSize;
    std::wstring                        m_Password;
    wchar_t                            *m_FileBuffer;
    HANDLE                              m_SourceFileHandle;
    HANDLE                              m_TargetFileHandle;
};
