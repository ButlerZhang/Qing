#pragma once
#include "QingBase.h"
#include <string>
#include <vector>

QING_NAMESPACE_BEGIN



class QING_DLL FileManager
{
public:

    FileManager(void);
    ~FileManager(void);

    bool IsDirectory(const std::string &Path) const;
    unsigned long long GetFileSize(const std::string &FileName) const;
    unsigned int ReadFile(const std::string &FileName, char *FileBuffer, int BufferSize, char *Mode = "rb") const;

    bool GetFileNameRecursion(const std::string &Directory, std::vector<std::string> &FileNameVector);
    bool GetFileNameNonRecursion(const std::string &Directory, std::vector<std::string> &FileNameVector);

    inline long GetFileMaxSize() const { return m_FileMaxSize; }
    inline long GetTotalFileSize() const { return m_TotalFileSize; }

private:

    long        m_FileMaxSize;
    long        m_TotalFileSize;
};

QING_NAMESPACE_END