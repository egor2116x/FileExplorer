#pragma once
#include "stdafx.h"
#include "Common.h"

template<typename T>
class RenameFileAction
{
	public:
		RenameFileAction(const std::wstring & ext, const std::wstring & addName, size_t maxFileSize) 
			: m_ext(ext), m_addName(addName), m_size(maxFileSize) {}
		void operator()(T & i);
	private:
		std::wstring m_ext;
		std::wstring m_addName;
		size_t m_size;
};

template<typename T>
class RenameDirAction
{
	public:
		RenameDirAction(const std::wstring & dirPreffix) : m_dirPreffix(dirPreffix) {}
		void operator()(T & i);
	private:
		std::wstring m_dirPreffix;
};

template<typename T>
class ReadFileAction
{
	public:
		ReadFileAction(const std::wstring & filePath,size_t offset = 0) 
			: m_filePath(filePath), m_offset(offset) {}
		void operator()(T & i);
	private:
		std::wstring m_filePath;
		size_t m_offset;
		std::wstring m_readData;
};


template<typename T>
class WriteFileAction
{
	public:
		WriteFileAction(const std::wstring & filePath, const std::wstring & data, size_t offset = 0)
			: m_filePath(filePath), m_writeData(data), m_offset(offset) {}
		void operator()(T & i);
	private:
		std::wstring m_filePath;
		size_t m_offset;
		std::wstring m_writeData;
};

std::mutex mtx;

template<typename T>
class MultyThreadCopyFileAction
{
	public:
		MultyThreadCopyFileAction(const std::wstring & fromFile, const std::wstring & toFile, size_t fileSizeLimit = 50) /* if >50MB multythread on */
			: m_FromFile(fromFile), m_ToFile(toFile), m_fileSizeLimit(fileSizeLimit) {}
		void operator()(T & i);
	private:
		void CopyFile(size_t idx);
	private:
		std::wstring m_FromFile;
		std::wstring m_ToFile;
		size_t m_fileSizeLimit; // in MB
		size_t m_currentFileSize; // in MB
		size_t m_amountThreads;
		std::vector<std::thread *> m_threads;
};

std::wstring m_FromFile;
std::wstring m_ToFile;

template<typename T>
void RenameFileAction<T>::operator()(T & i)
{
	if (i.m_itemType == DIR_MARK)
	{
		return;
	}
	if (m_ext.compare(FilePathOperation::GetFileExtantion(i.m_fName)) != 0)
	{
		return;
	}
	if (i.m_size > m_size)
	{
		return;
	}
	
	HANDLE hFile = nullptr;
	std::wstring fullFilePath = (i.m_path + i.m_fName);
	hFile = CreateFile(fullFilePath.c_str(), GENERIC_READ, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, hFile);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	std::wstring fName;
	fName.resize(MAX_PATH);
	size_t tmpSizeBuff = 0;
	while ((tmpSizeBuff = GetFinalPathNameByHandleW(hFile, reinterpret_cast<LPWSTR>(&fName[0]), fName.size(), VOLUME_NAME_DOS)) > fName.size())
	{
		fName.resize(tmpSizeBuff);
	}
	CloseHandle(hFile);
	if (!fName.empty())
	{
		std::wstring tmp(fName);
		FilePathOperation::CutFileNameWithExtantion(tmp);
		tmp += m_addName;
		tmp += i.m_fName;
		i.m_fName = m_addName + i.m_fName;
		MoveFile(fName.c_str(), tmp.c_str());
	}
}

template<typename T>
void RenameDirAction<T>::operator()(T & i)
{
	if (i.m_itemType != DIR_MARK || i.m_fName[0] == L'.')
	{
		return;
	}

	HANDLE hFile = nullptr;
	std::wstring fullFilePath = (i.m_path + i.m_fName);
	hFile = CreateFile(fullFilePath.c_str(), GENERIC_READ, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, hFile);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	std::wstring fName;
	fName.resize(MAX_PATH);
	size_t tmpSizeBuff = 0;
	while ((tmpSizeBuff = GetFinalPathNameByHandleW(hFile, reinterpret_cast<LPWSTR>(&fName[0]), fName.size(), VOLUME_NAME_DOS)) > fName.size())
	{
		fName.resize(tmpSizeBuff);
	}
	CloseHandle(hFile);
	if (!fName.empty())
	{
		std::wstring tmp(fName);
		FilePathOperation::CutFileNameWithExtantion(tmp);
		tmp += m_dirPreffix;
		tmp += i.m_fName;
		for (std::vector<T>::iterator it = g_fileMap.begin(); it != g_fileMap.end(); it++)
		{
			if (it->m_fName == i.m_fName)
			{
				continue;
			}
			std::wstring::size_type pos = it->m_path.find(fullFilePath);
			if (pos != std::wstring::npos)
			{
				std::wstring firstPart = it->m_path.substr(0, fullFilePath.size());
				std::wstring secondPart = it->m_path.substr(fullFilePath.size() + 1, it->m_path.size());
				FilePathOperation::CutFileNameWithExtantion(firstPart);
				firstPart += m_dirPreffix + i.m_fName;
				firstPart += L'\\';
				firstPart += secondPart;
				it->m_path = firstPart;
			}
				
		}
		i.m_fName = m_dirPreffix + i.m_fName;
		MoveFile(fName.c_str(), tmp.c_str());
	}
}

template<typename T>
void ReadFileAction<T>::operator()(T & i)
{
	std::wstring fullFilePath = i.m_path + i.m_fName;
	if (fullFilePath.compare(m_filePath) != 0 )
	{ 
		return;
	}
	/*
	HANDLE hFile = CreateFile(fullFilePath.c_str(), GENERIC_READ, FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, hFile);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "File don't open " << m_filePath << std::endl;
	}

	std::string buff;
	size_t newSize = i.m_size - m_offset;
	if (newSize <= 0)
	{
		std::cout << "Incorrect new read size of data" << std::endl;
		return;
	}

	buff.resize(newSize);
	size_t readData = 0;

	if (!ReadFile(hFile, reinterpret_cast<LPVOID>(&buff[0]), newSize, &readData, NULL))
	{
		std::cout << "Could not read data from file" << std::endl;
		return;
	}
	CloseHandle(hFile);
	*/

	std::wstring buff;
	size_t newSize = i.m_size - m_offset;
	if (newSize <= 0)
	{
		std::cout << "Incorrect new read size of data" << std::endl;
		return;
	}
	buff.resize(newSize);

	std::wifstream is;
	is.open(fullFilePath, std::ios::binary);
	is.seekg(m_offset, std::ios::beg);
	is.read(&buff[0], newSize);
	is.close();

	m_readData = buff;

	std::wcout << m_readData << std::endl;
}

template<typename T>
void WriteFileAction<T>::operator()(T & i)
{
	std::wstring fullFilePath = i.m_path + i.m_fName;
	if (fullFilePath.compare(m_filePath) != 0)
	{
		return;
	}
	if (m_writeData.empty())
	{
		std::wcout << L"No data available" << std::endl;
		return;
	}

	std::wofstream os;
	os.open(fullFilePath, std::ios::binary);
	os.seekp(m_offset, std::ios::beg);
	os.write(&m_writeData[0], m_writeData.size());
	os.close();
}

template<typename T>
void MultyThreadCopyFileAction<T>::CopyFile(size_t idxThread)
{
	std::string buffer;
	buffer.resize(m_fileSizeLimit);

	mtx.lock();
	std::ifstream infile(m_FromFile, std::ios::in | std::ios::binary);
	std::ofstream outfile(m_ToFile, std::ios::out | std::ios::app | std::ios::binary);
	if (!infile || !outfile)
	{ 
		return;
	}

	size_t leftSize = (m_amountThreads - idxThread - 1 != 0 ? m_fileSizeLimit : m_currentFileSize - (m_fileSizeLimit * idxThread));
	infile.seekg(m_fileSizeLimit * idxThread, std::ios::beg);
	infile.read(&buffer[0], leftSize);
	infile.close();

	outfile.write(&buffer[0], leftSize);
	outfile.close();
	
	std::wcout << L"Thread : " << idxThread << L" read data with offset : " << m_fileSizeLimit * idxThread << std::endl;

	mtx.unlock();
}

template<typename T>
void MultyThreadCopyFileAction<T>::operator()(T & i)
{
	std::wstring fullFilePath = i.m_path + i.m_fName;
	if (fullFilePath.compare(m_FromFile) != 0)
	{
		return;
	}
	if (m_ToFile.empty())
	{
		std::wcout << L"Don't set ToFile" << std::endl;
		return;
	}

	m_currentFileSize = i.m_size;
	m_fileSizeLimit = (m_fileSizeLimit > 0 ? m_fileSizeLimit * 1000000 : 1000000);
	m_amountThreads = (m_currentFileSize / m_fileSizeLimit) <= 0 
						? 1 : std::ceil(static_cast<double>(m_currentFileSize) / static_cast<double>(m_fileSizeLimit));
	for (size_t i = 0; i < m_amountThreads; i++)
	{
		std::thread * t = new std::thread(&MultyThreadCopyFileAction<T>::CopyFile, this, i);
		t->join();
		m_threads.push_back(t);
	}
	std::wcout << L"Amount threads : " << m_amountThreads << std::endl;
}