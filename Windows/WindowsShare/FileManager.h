#pragma once

#include <string>
#include <vector>



namespace QING
{
    class FileManager
    {
    public:

        FileManager(void);
        ~FileManager(void);

        bool IsDirectory(const std::string &FileName) const;
        unsigned long long GetFileSize(const std::string &FileName) const;
        int  ReadFile(const std::string &FileName, char *FileBuffer, int BufferSize, char *Mode = "rb") const;

        bool GetFileNameRecursion(const std::string &Directory, std::vector<std::string> &FileNameVector, long &TotalSize, long &MaxSize) const;
        bool GetFileNameNonRecursion(const std::string &Directory, std::vector<std::string> &FileNameVector, long &TotalSize, long &MaxSize) const;
    };
}
