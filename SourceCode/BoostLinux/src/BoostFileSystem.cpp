#include "BoostFileSystem.h"
#include <boost/filesystem.hpp>



BoostFileSystem::BoostFileSystem()
{
}

BoostFileSystem::~BoostFileSystem()
{
}

bool BoostFileSystem::IsFile(const std::string &File)
{
    return boost::filesystem::is_regular(File);
}

bool BoostFileSystem::IsPathExisted(const std::string &Path)
{
    return boost::filesystem::exists(Path);
}

bool BoostFileSystem::IsDirectory(const std::string &Diectroy)
{
    return boost::filesystem::is_directory(Diectroy);
}

unsigned long long BoostFileSystem::GetFileSize(const std::string &File)
{
    return boost::filesystem::file_size(boost::filesystem::path(File));
}

unsigned long long BoostFileSystem::GetAvailable(const std::string &Path)
{
    boost::filesystem::space_info LogSpace(boost::filesystem::space(Path));

    unsigned long long AvailableSize = LogSpace.available;
    //unsigned long long TotalSize = LogSpace.capacity;
    //unsigned long long FreeSize = LogSpace.free;
    //add log

    return AvailableSize;
}

std::string BoostFileSystem::GetWorkDirectory()
{
    return boost::filesystem::initial_path().string();
}

std::string BoostFileSystem::GetFileName(const std::string &FileFullPath)
{
    boost::filesystem::path FullPath(FileFullPath);
    return FullPath.filename().string();
}

bool BoostFileSystem::RemoveFile(const std::string &File)
{
    return boost::filesystem::remove(boost::filesystem::path(File));
}

bool BoostFileSystem::GetDirectoryList(std::vector<std::string> &FilesNameVector, const std::string &Path)
{
    boost::filesystem::path Directory(Path);
    if (!boost::filesystem::exists(Directory))
    {
        return false;
    }

    boost::filesystem::directory_iterator end;
    for (boost::filesystem::directory_iterator it(Directory); it != end; it++)
    {
        FilesNameVector.push_back(it->path().filename().string());
    }

    return !FilesNameVector.empty();
}

bool BoostFileSystem::GetDirectoryAllFilesList(std::vector<std::string> &FilesVector, const std::string &Path)
{
    boost::filesystem::directory_iterator end;
    std::list<std::string> DirectoryList;
    DirectoryList.push_back(Path);

    while (!DirectoryList.empty())
    {
        const std::string &DirectoryName = *DirectoryList.begin();

        boost::filesystem::path Directory(DirectoryName);
        if (!boost::filesystem::exists(Directory))
        {
            DirectoryList.erase(DirectoryList.begin());
            continue;
        }

        for (boost::filesystem::directory_iterator it(Directory); it != end; it++)
        {
            const std::string &CurrentPath = DirectoryName + "/" + it->path().filename().string();
            if (BoostFileSystem::IsFile(CurrentPath))
            {
                FilesVector.push_back(CurrentPath);
            }
            else
            {
                DirectoryList.push_back(CurrentPath);
            }
        }

        DirectoryList.erase(DirectoryList.begin());
    }

    return !FilesVector.empty();
}
