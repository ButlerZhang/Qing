#pragma once
#include <string>
#include <vector>
#include <Windows.h>



class SimpleCrypt
{
public:

    SimpleCrypt();
    ~SimpleCrypt();

    void SetPassword(const std::wstring &Password);
    void SetIsForceStop(bool IsForceStop) { m_IsForceStop = IsForceStop; }
    void SetFileExtension(const std::wstring &FileExtension) { m_FileExtension = FileExtension; }
    void SetIsEncryptFileName(bool IsEncryptFileName) { m_IsEncryptFileName = IsEncryptFileName; }
    void SetIsDeleteOriginalFile(bool IsDeleteOriginalFile) { m_IsDeleteOriginalFile = IsDeleteOriginalFile; }

    bool Encrypt(const std::wstring &SourceFile, const std::wstring &TargetPath);
    bool DeCrypt(const std::wstring &SourceFile, const std::wstring &TargetPath);

    bool IsForceStop() const { return m_IsForceStop; }
    std::wstring GetErrorMessage() const { return m_ErrorMessage; }

private:

    bool Delete(const std::wstring &SourceFile);
    bool Reset(HANDLE SourceFileHandle, const std::wstring &SourceFile);
    bool IsSpaceEnough(unsigned long FileSize, std::wstring SourceFile);

    bool IsEncrypt(const std::wstring &SourceFile);
    void EncryptDecryptBuffer(wchar_t *DataBuffer, int DataSize) const;
    bool EncryptDecryptFileData(HANDLE SourceFileHandle, HANDLE TargetFileHandle, DWORD FileOffset);

    bool DecryptHeader(HANDLE SourceFileHandle, std::wstring &OriginalFile);
    bool EncryptHeader(const std::wstring &SourceFile, HANDLE SourceFileHandle, HANDLE TargetFileHandle);

    std::wstring GetEncryptFileName(const std::wstring &SourceFile, const std::wstring &TargetPath);
    std::wstring GetDecryptFileName(HANDLE SourceFileHandle, const std::wstring &SourceFile, const std::wstring &TargetPath);

private:

    bool                                     m_IsForceStop;
    bool                                     m_IsEncryptFileName;
    bool                                     m_IsDeleteOriginalFile;
    std::wstring                             m_Password;
    std::wstring                             m_FileExtension;
    std::wstring                             m_ErrorMessage;
    std::vector<std::wstring>                m_HeaderVector;
    unsigned long                            m_FileSize;
    unsigned long                            m_DataBufferSize;
    wchar_t                                 *m_FileDataBuffer;
};
