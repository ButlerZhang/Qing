#include "FileManager.h"
#include "../../SourceCode/Common/Boost/BoostLog.h"
#include <io.h>
#include <list>
#include <Windows.h>
#include <algorithm>



bool CompareFileName(const std::wstring &Left, const std::wstring &Right)
{
    std::wstring LeftName(Left);
    std::wstring RightName(Right);

    transform(LeftName.begin(), LeftName.end(), LeftName.begin(), ::toupper);
    transform(RightName.begin(), RightName.end(), RightName.begin(), ::toupper);

    return LeftName < RightName;
}



FileManager::FileManager(void)
{
    m_TotalFileSize = 0;
    m_FileMaxSize = 0;
}

FileManager::~FileManager(void)
{
}

bool FileManager::IsDirectory(const std::wstring &Path) const
{
    return ((GetFileAttributes(Path.c_str()) & FILE_ATTRIBUTE_DIRECTORY) > 0);
}

unsigned long long FileManager::GetFileSize(const std::wstring &FileName) const
{
    HANDLE FileHandle = ::CreateFile(FileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (FileHandle == INVALID_HANDLE_VALUE)
    {
        g_Log.WriteDebug(BoostFormat("GetFileSize create file handler is invalid handler, file name is %s.", FileName.c_str()));
        return 0;
    }

    LARGE_INTEGER LargeInteger;
    if (GetFileSizeEx(FileHandle, &LargeInteger))
    {
        return LargeInteger.QuadPart;
    }

    return 0;
}

unsigned int FileManager::ReadFile(const std::wstring &FileName, wchar_t *FileBuffer, int BufferSize, wchar_t *Mode) const
{
    if (FileBuffer == NULL || BufferSize <= 0)
    {
        g_Log.WriteError("ReadFile, FileBuffer == NULL || BufferSize <= 0.");
        return 0;
    }

    FILE *File = NULL;
    _wfopen_s(&File, FileName.c_str(), Mode);
    if (File == NULL)
    {
        g_Log.WriteError(BoostFormat("ReadFile, can not open file = %s.", FileName.c_str()));
        return 0;
    }

    long FileSize = _filelength(_fileno(File));
    if (FileSize <= 0 || FileSize > BufferSize)
    {
        g_Log.WriteError(BoostFormat("ReadFile, FileSize = %I64d, BufferSize = %d.", FileSize, BufferSize));
        fclose(File);
        return 0;
    }

    size_t ReadByte = fread(FileBuffer, 1, FileSize, File);
    fclose(File);

    if (ReadByte != FileSize)
    {
        g_Log.WriteError(BoostFormat("ReadFile, FileSize = %I64d, ReadByte = %d.", FileSize, ReadByte));
        return 0;
    }

    return FileSize;
}

bool FileManager::GetFileNameRecursion(const std::wstring &Directory, std::vector<std::wstring> &FileNameVector)
{
    struct _wfinddata_t FileInfo;
    std::wstring TempDirectory;
    intptr_t hFile = 0;

    if ((hFile = _wfindfirst(TempDirectory.assign(Directory).append(L"\\*").c_str(), &FileInfo)) != -1L)
    {
        do
        {
            if ((FileInfo.attrib & _A_SUBDIR))
            {
                if (wcscmp(FileInfo.name, L".") != 0 && wcscmp(FileInfo.name, L"..") != 0)
                {
                    GetFileNameRecursion(TempDirectory.assign(Directory).append(L"\\").append(FileInfo.name), FileNameVector);
                }
            }
            else
            {
                if (static_cast<long>(FileInfo.size) > m_FileMaxSize)
                {
                    m_FileMaxSize = FileInfo.size;
                }
                m_TotalFileSize += FileInfo.size;

                FileNameVector.push_back(TempDirectory.assign(Directory).append(L"\\").append(FileInfo.name));
            }
        } while (_wfindnext(hFile, &FileInfo) == 0);

        _findclose(hFile);
        return true;
    }

    return false;
}

bool FileManager::GetFileNameNonRecursion(const std::wstring &Directory, std::vector<std::wstring> &FileNameVector)
{
    std::list<std::wstring> DirectoryList;
    DirectoryList.push_back(Directory);

    std::vector<std::wstring> NameVector;
    std::vector<std::wstring> NextDirectoryVector;

    struct _wfinddata_t FileInfo;
    std::wstring TempDirectory;
    intptr_t hFile = 0;

    while (!DirectoryList.empty())
    {
        const std::wstring &DirectoryString = *DirectoryList.begin();
        if ((hFile = _wfindfirst(TempDirectory.assign(DirectoryString).append(L"\\*").c_str(), &FileInfo)) != -1L)
        {
            do
            {
                if ((FileInfo.attrib & _A_SUBDIR))
                {
                    if (wcscmp(FileInfo.name, L".") != 0 && wcscmp(FileInfo.name, L"..") != 0)
                    {
                        NextDirectoryVector.push_back(TempDirectory.assign(DirectoryString).append(L"\\").append(FileInfo.name));
                    }
                }
                else
                {
                    if (static_cast<long>(FileInfo.size) > m_FileMaxSize)
                    {
                        m_FileMaxSize = FileInfo.size;
                    }
                    m_TotalFileSize += FileInfo.size;

                    NameVector.push_back(FileInfo.name);
                }
            } while (_wfindnext(hFile, &FileInfo) == 0);

            if (!NameVector.empty())
            {
                sort(NameVector.begin(), NameVector.end(), CompareFileName);
                for (std::vector<std::string>::size_type Index = 0; Index < NameVector.size(); Index++)
                {
                    FileNameVector.push_back(DirectoryString + L"\\" + NameVector[Index]);
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
