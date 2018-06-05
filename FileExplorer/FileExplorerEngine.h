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

template<typename Action>
void FileExplorerEngine::makeAction(Action & action)
{
	std::for_each(g_fileMap.begin(), g_fileMap.end(), action);
}

