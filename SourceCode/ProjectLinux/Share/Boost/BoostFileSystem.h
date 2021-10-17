#pragma once
#include <string>
#include <vector>



class BoostFileSystem
{
public:

    BoostFileSystem();
    ~BoostFileSystem();

    static bool IsFile(const std::string &File);
    static bool IsPathExisted(const std::string &Path);
    static bool IsDirectory(const std::string &Diectroy);

    static bool RemoveFile(const std::string &File);

    static bool GetDirectoryList(std::vector<std::string> &FilesNameVector, const std::string &Path);
    static bool GetDirectoryAllFilesList(std::vector<std::string> &FilesPathVector, const std::string &Path);

    static unsigned long long GetFileSize(const std::string &File);
    static unsigned long long GetAvailable(const std::string &Path);

    static std::string GetWorkDirectory();
    static std::string GetFileName(const std::string &FileFullPath);
};
