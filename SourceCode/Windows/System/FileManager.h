#pragma once
#include <string>
#include <vector>



class FileManager
{
public:

    FileManager(void);
    ~FileManager(void);

    bool IsDirectory(const std::wstring &Path) const;
    unsigned long long GetFileSize(const std::wstring &FileName) const;
    unsigned int ReadFile(const std::wstring &FileName, wchar_t *FileBuffer, int BufferSize, wchar_t *Mode = L"rb") const;

    bool GetFileNameRecursion(const std::wstring &Directory, std::vector<std::wstring> &FileNameVector);
    bool GetFileNameNonRecursion(const std::wstring &Directory, std::vector<std::wstring> &FileNameVector);

    inline long GetFileMaxSize() const { return m_FileMaxSize; }
    inline long GetTotalFileSize() const { return m_TotalFileSize; }

private:

    long        m_FileMaxSize;
    long        m_TotalFileSize;
};
