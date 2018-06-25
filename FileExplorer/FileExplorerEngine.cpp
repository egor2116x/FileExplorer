#include "stdafx.h"
#include "FileExplorerEngine.h"
#include "FileHandleGuard.h"

size_t FileExplorerEngine::GetFileSize(WIN32_FIND_DATA & _wfd) const noexcept
{
    return (_wfd.nFileSizeHigh * (MAXDWORD + 1)) + _wfd.nFileSizeLow;
}

void FileExplorerEngine::BuildNewFolderPath(std::wstring & folderPath, const std::wstring & newFolderName)
{
    folderPath = folderPath.substr(0, folderPath.size() - 1);
    folderPath += newFolderName;
    folderPath += L"\\*";
}

void FileExplorerEngine::GetListFilesInFolder(std::wstring & folderPath)
{
    if (folderPath.empty())
    {
        if (m_rootFolderPath.empty())
        {
            throw UserInputException(ExceptionErrorTypes::INCORRECT_USER_INPUT, "No such file root directory");
        }
        folderPath = m_rootFolderPath;
    }

    WIN32_FIND_DATA wfd;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    hFile = FindFirstFile(folderPath.c_str(), &wfd);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }

    do
    {

        if (IsDirectory(wfd.dwFileAttributes) && std::wstring(wfd.cFileName)[0] != L'.')
        {
            ++m_level;
            g_fileMap.push_back(Item(m_level, ObjectMark::DIR_MARK, folderPath.substr(0, folderPath.size() - 1), std::wstring(wfd.cFileName), this->GetFileSize(wfd), hFile));
            BuildNewFolderPath(folderPath, std::wstring(wfd.cFileName));
            GetListFilesInFolder(folderPath);
            FilePathOperation::CutLastFilePathItem(folderPath);
            FilePathOperation::CutLastFilePathItem(folderPath);
            folderPath += L"\\*";
            --m_level;
        }
        else if (std::wstring(wfd.cFileName)[0] != L'.')
        {
            g_fileMap.push_back(Item(m_level, ObjectMark::FILE_MARK, folderPath.substr(0, folderPath.size() - 1), std::wstring(wfd.cFileName), this->GetFileSize(wfd), hFile));
        }
        else
        {
            g_fileMap.push_back(Item(m_level, ObjectMark::DIR_MARK, folderPath.substr(0, folderPath.size() - 1), std::wstring(wfd.cFileName), this->GetFileSize(wfd), hFile));
        }

    } while (FindNextFile(hFile, &wfd) != 0);

    FindClose(hFile);
    return;
}
