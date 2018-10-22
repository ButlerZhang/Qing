#pragma once
#include <string>



class SimpleCrypt
{
public:

    SimpleCrypt();
    ~SimpleCrypt();

    void Reset();
    void SetPassword(const std::wstring &Password);

    bool Encrypt(const std::wstring &SourceFileName, const std::wstring &TargetFileName);
    bool DeCrypt(const std::wstring &SourceFileName, const std::wstring &TargetFileName);

private:

    bool IsEncrypt(const std::wstring &SourceFileName) const;

private:

    std::wstring            m_Password;
    wchar_t                *m_FileBuffer;
    HANDLE                  m_SourceFileHandle;
    HANDLE                  m_TargetFileHandle;
};

