#pragma once
#include <string>
#include <vector>
#include <Windows.h>



class SimpleEncrypt
{
public:

    SimpleEncrypt();
    ~SimpleEncrypt();

    void SetPassword(const std::wstring &Password);
    inline void SetIsForceStop(bool IsForceStop) { m_IsForceStop = IsForceStop; }
    inline void SetIsEncryptFileName(bool IsEncryptFileName) { m_IsEncryptFileName = IsEncryptFileName; }
    inline void SetIsDeleteOriginalFile(bool IsDeleteOriginalFile) { m_IsDeleteOriginalFile = IsDeleteOriginalFile; }

    bool Encrypt(const std::wstring &SourceFile, const std::wstring &TargetPath);
    bool Decrypt(const std::wstring &SourceFile, const std::wstring &TargetPath);

    bool Disguise(const std::wstring &SourceFile);
    bool Recovery(const std::wstring &SourceFile);

    inline bool IsForceStop() const { return m_IsForceStop; }
    inline bool IsEncryptFileName() const { return m_IsEncryptFileName; }
    inline bool IsDeleteOriginalFile() const { return m_IsDeleteOriginalFile; }
    inline std::wstring GetErrorMessage() const { return m_ErrorMessage; }

private:

    bool Delete(const std::wstring &SourceFile);
    bool Prepare(HANDLE SourceFileHandle, const std::wstring &SourceFile);

    bool IsEncrypt(const std::wstring &SourceFile) const;
    bool IsDisguise(const std::wstring &SourceFile) const;
    bool IsDiskSpaceEnough(unsigned long FileSize, std::wstring SourceFile);

    void EncryptDecryptBuffer(wchar_t *DataBuffer, int DataSize) const;
    bool EncryptDecryptFileData(HANDLE SourceFileHandle, HANDLE TargetFileHandle, DWORD FileOffset);

    bool DecryptHeader(HANDLE SourceFileHandle, std::wstring &OriginalFile);
    bool EncryptHeader(const std::wstring &SourceFile, HANDLE SourceFileHandle, HANDLE TargetFileHandle);

    std::wstring GetEncryptFileName(const std::wstring &SourceFile, const std::wstring &TargetPath) const;
    std::wstring GetDecryptFileName(HANDLE SourceFileHandle, const std::wstring &SourceFile, const std::wstring &TargetPath);

private:

    bool                                     m_IsForceStop;
    bool                                     m_IsEncryptFileName;
    bool                                     m_IsDeleteOriginalFile;

    unsigned long                            m_FileSize;
    unsigned long                            m_DataBufferSize;
    wchar_t*                                 m_FileDataBuffer;

    std::wstring                             m_Password;
    std::wstring                             m_ErrorMessage;
    const std::wstring                       m_FileEncryptExtension;
    const std::wstring                       m_FileDisguiseExtension;
    std::vector<std::wstring>                m_HeaderVector;
};
