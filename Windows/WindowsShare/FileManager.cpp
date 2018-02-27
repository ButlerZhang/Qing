#include "FileManager.h"
#include <io.h>
#include <list>
#include <Windows.h>
#include <algorithm>



namespace QING
{
    //***************************************************************************
    // CompareFileName: sort by file name.
    //***************************************************************************
    bool CompareFileName(const std::string &Left, const std::string &Right)
    {
        std::string LeftName(Left);
        std::string RightName(Right);

        transform(LeftName.begin(), LeftName.end(), LeftName.begin(), ::toupper);
        transform(RightName.begin(), RightName.end(), RightName.begin(), ::toupper);

        return LeftName < RightName;
    }



    //***************************************************************************
    // Constructor
    //***************************************************************************
    FileManager::FileManager(void)
    {
    }

    //***************************************************************************
    // Destructor
    //***************************************************************************
    FileManager::~FileManager(void)
    {
    }

    bool FileManager::IsDirectory(const std::string & FileName) const
    {
        return ((GetFileAttributesA(FileName.c_str()) & FILE_ATTRIBUTE_DIRECTORY) > 0);
    }

    //***************************************************************************
    // GetFileSize: return file size
    //***************************************************************************
    unsigned long long FileManager::GetFileSize(const std::string &FileName) const
    {
        HANDLE FileHandle = ::CreateFileA(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (FileHandle == INVALID_HANDLE_VALUE)
        {
            return 0;
        }

        LARGE_INTEGER LargeInteger;
        if (GetFileSizeEx(FileHandle, &LargeInteger))
        {
            return LargeInteger.QuadPart;
        }

        return 0;
    }

    //***************************************************************************
    // ReadFile: Read file's contents at once
    //***************************************************************************
    int FileManager::ReadFile(const std::string &FileName, char *FileBuffer, int BufferSize, char *Mode) const
    {
        if (FileBuffer == NULL || BufferSize <= 0)
        {
            return 0;
        }

        FILE *File = NULL;
        fopen_s(&File, FileName.c_str(), "rb");
        if (File == NULL)
        {
            return 0;
        }

        int FileSize = _filelength(_fileno(File));
        if (FileSize <= 0 || FileSize > BufferSize)
        {
            fclose(File);
            return 0;
        }

        size_t ReadByte = fread(FileBuffer, 1, FileSize, File);
        fclose(File);

        if (ReadByte != FileSize)
        {
            return 0;
        }

        return FileSize;
    }

    //***************************************************************************
    // GetFileNameRecursion: Recursively read the file name
    //***************************************************************************
    bool FileManager::GetFileNameRecursion(const std::string &Directory, std::vector<std::string> &FileNameVector, long &TotalSize, long &MaxSize) const
    {
        struct _finddata_t FileInfo;
        std::string TempDirectory;
        long long hFile = 0;

        if ((hFile = _findfirst(TempDirectory.assign(Directory).append("\\*").c_str(), &FileInfo)) != -1L)
        {
            do
            {
                if ((FileInfo.attrib & _A_SUBDIR))
                {
                    if (strcmp(FileInfo.name, ".") != 0 && strcmp(FileInfo.name, "..") != 0)
                    {
                        GetFileNameRecursion(TempDirectory.assign(Directory).append("\\").append(FileInfo.name), FileNameVector, TotalSize, MaxSize);
                    }
                }
                else
                {
                    if (static_cast<long>(FileInfo.size) > MaxSize)
                    {
                        MaxSize = FileInfo.size;
                    }
                    TotalSize += FileInfo.size;

                    FileNameVector.push_back(TempDirectory.assign(Directory).append("\\").append(FileInfo.name));
                }
            } while (_findnext(hFile, &FileInfo) == 0);

            _findclose(hFile);
            return true;
        }

        return false;
    }

    //***************************************************************************
    // GetFileNameNonRecursion: Non-recursive read the file name
    //***************************************************************************
    bool FileManager::GetFileNameNonRecursion(const std::string &Directory, std::vector<std::string> &FileNameVector, long &TotalSize, long &MaxSize) const
    {
        std::list<std::string> DirectoryList;
        DirectoryList.push_back(Directory);

        std::vector<std::string> NameVector;
        std::vector<std::string> NextDirectoryVector;

        struct _finddata_t FileInfo;
        std::string TempDirectory;
        long long hFile = 0;

        while (!DirectoryList.empty())
        {
            const std::string &DirectoryString = *DirectoryList.begin();
            if ((hFile = _findfirst(TempDirectory.assign(DirectoryString).append("\\*").c_str(), &FileInfo)) != -1L)
            {
                do
                {
                    if ((FileInfo.attrib & _A_SUBDIR))
                    {
                        if (strcmp(FileInfo.name, ".") != 0 && strcmp(FileInfo.name, "..") != 0)
                        {
                            NextDirectoryVector.push_back(TempDirectory.assign(DirectoryString).append("\\").append(FileInfo.name));
                        }
                    }
                    else
                    {
                        if (static_cast<long>(FileInfo.size) > MaxSize)
                        {
                            MaxSize = FileInfo.size;
                        }
                        TotalSize += FileInfo.size;

                        NameVector.push_back(FileInfo.name);
                    }
                } while (_findnext(hFile, &FileInfo) == 0);

                if (!NameVector.empty())
                {
                    sort(NameVector.begin(), NameVector.end(), CompareFileName);
                    for (std::vector<std::string>::size_type Index = 0; Index < NameVector.size(); Index++)
                    {
                        FileNameVector.push_back(DirectoryString + "\\" + NameVector[Index]);
                    }

                    NameVector.clear();
                }

                if (!NextDirectoryVector.empty())
                {
                    sort(NextDirectoryVector.begin(), NextDirectoryVector.end());
                    DirectoryList.insert(++DirectoryList.begin(), NextDirectoryVector.begin(), NextDirectoryVector.end());
                    NextDirectoryVector.clear();
                }

                _findclose(hFile);
            }

            DirectoryList.erase(DirectoryList.begin());
        }

        return FileNameVector.size() > 0;
    }
}
