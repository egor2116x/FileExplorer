#pragma once
#include "stdafx.h"
#include "Common.h"
#include "Exceptions.h"

class FileExplorerEngine
{
public:
	FileExplorerEngine() : m_level(1) {}
	void GetListFilesInFolder(std::wstring & folderPath = std::wstring());
	const std::wstring & GetRootFolderPath() const { return m_rootFolderPath; }
	void SetRootFolderPath(const std::wstring & folderPath) { m_rootFolderPath = folderPath; }
	template<typename Output_Type>
	void print(Output_Type & out);
	template<typename Action>
	void makeAction(Action & action);
private:
	BOOL IsFile(DWORD fileAttribute) { return !(fileAttribute & FILE_ATTRIBUTE_DIRECTORY); }
	BOOL IsDirectory(DWORD fileAttribute) { return (fileAttribute & FILE_ATTRIBUTE_DIRECTORY); }
	void BuildNewFolderPath(std::wstring & folderPath, const std::wstring & newFolderName);
	size_t GetFileSize(WIN32_FIND_DATA & _wfd);
private:	
	size_t m_level;
	std::wstring m_rootFolderPath;
};


void FileExplorerEngine::GetListFilesInFolder(std::wstring & folderPath)
{
	if (folderPath.empty())
	{
		if (m_rootFolderPath.empty())
		{
			throw UserInputException(INCORRECT_USER_INPUT, "No such file root directory");
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
			g_fileMap.push_back(Item(m_level, DIR_MARK, folderPath.substr(0, folderPath.size() - 1), std::wstring(wfd.cFileName), this->GetFileSize(wfd), hFile));
			BuildNewFolderPath(folderPath, std::wstring(wfd.cFileName));
			GetListFilesInFolder(folderPath);
			FilePathOperation::CutLastFilePathItem(folderPath);
			FilePathOperation::CutLastFilePathItem(folderPath);
			folderPath += L"\\*";
			--m_level;
		}
		else if (std::wstring(wfd.cFileName)[0] != L'.')
		{
			g_fileMap.push_back(Item(m_level, FILE_MARK, folderPath.substr(0, folderPath.size() - 1), std::wstring(wfd.cFileName), this->GetFileSize(wfd), hFile));
		}
		else
		{
			g_fileMap.push_back(Item(m_level, DIR_MARK, folderPath.substr(0, folderPath.size() - 1), std::wstring(wfd.cFileName), this->GetFileSize(wfd), hFile));
		}

	} while (FindNextFile(hFile, &wfd) != 0);

	FindClose(hFile);
	return;
}

template<typename Output_Type>
void FileExplorerEngine::print(Output_Type & out)
{
	if (g_fileMap.empty())
	{
		out << L"No more files" << std::endl;
	}
	for (auto itm : g_fileMap)
	{
		for (size_t i = 0; i < itm.m_level; ++i)
		{
			if (i == 0)
			{
				out << L"|--";
				continue;
			}
			out << L"--";
		}
		out << itm.m_fName << std::endl;
	}
}

void FileExplorerEngine::BuildNewFolderPath(std::wstring & folderPath, const std::wstring & newFolderName)
{
	folderPath = folderPath.substr(0, folderPath.size() - 1);
	folderPath += newFolderName;
	folderPath += L"\\*";
}

template<typename Action>
void FileExplorerEngine::makeAction(Action & action)
{
	std::for_each(g_fileMap.begin(), g_fileMap.end(), action);
}

size_t FileExplorerEngine::GetFileSize(WIN32_FIND_DATA & _wfd)
{
	return (_wfd.nFileSizeHigh * (MAXDWORD + 1)) + _wfd.nFileSizeLow;
}