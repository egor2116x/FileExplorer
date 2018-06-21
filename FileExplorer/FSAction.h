#pragma once
#include "stdafx.h"
#include "Common.h"
#include <list>

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

template<typename T>
class MultyThreadCopyFileAction
{
	public:
		MultyThreadCopyFileAction(const std::wstring & fromFile, const std::wstring & toFile, size_t fileSizeLimit = 50) /* if >50MB multythread on */
            : m_FromFile(fromFile), m_ToFile(toFile), m_fileSizeLimit(fileSizeLimit) {}
		void operator()(const T & i);
	private:
		void CopyFile(size_t idxThread);
	private:
		std::wstring m_FromFile;
		std::wstring m_ToFile;
		size_t m_fileSizeLimit; // in MB
		size_t m_currentFileSize; // in MB
		size_t m_amountThreads;
        std::vector<std::thread> m_threads;
        std::mutex m_mtx;
};

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

BOOL RemoveFromEventArray(HANDLE hEvents[], int nSize, int nIndex)
{
    if (nIndex < 0 || nSize <= nIndex)
        return FALSE;

    for (int i = nIndex + 1; i < nSize; i++)
    {
        hEvents[i - 1] = hEvents[i];
    }
    return TRUE;
}

template<typename T>
void MultyThreadCopyFileAction<T>::CopyFile(size_t idxThread)
{
    m_mtx.lock();
    std::wcout << L"Start thread: " << idxThread << std::endl;
    m_mtx.unlock();
    // read
    // calculate size of read/write operation
    bool isLastPartOfData = (m_amountThreads - idxThread - 1 == 0);
	const size_t leftSize = (isLastPartOfData ? m_currentFileSize - (m_fileSizeLimit * idxThread) : m_fileSizeLimit);
    std::string buffer;
    buffer.resize(leftSize);
    size_t sizeBytesIsReaded = 0;
    DWORD dwPtr = 0;
    BOOL res_io = FALSE;

    HANDLE hFileRead = CreateFile(m_FromFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL);
    if (hFileRead == INVALID_HANDLE_VALUE)
    {
        throw FileExeption(FILE_ERRORS, "file didn't open for reading");
    }

    dwPtr = SetFilePointer(hFileRead, m_fileSizeLimit * idxThread, NULL, FILE_BEGIN);
    if (dwPtr == INVALID_SET_FILE_POINTER)
    {
        CloseHandle(hFileRead);
        throw FileExeption(FILE_ERRORS, "invalid set file pointer for reading");
    }

    res_io = ReadFile(hFileRead, reinterpret_cast<LPVOID>(&buffer[0]), buffer.size(), reinterpret_cast<LPDWORD>(&sizeBytesIsReaded), NULL);
    if (!res_io)
    {
        CloseHandle(hFileRead);
        throw FileExeption(FILE_ERRORS, "Error of reading");
    }
    CloseHandle(hFileRead);
    res_io = FALSE;

    m_mtx.lock();
    std::wcout << L"Thread: " << idxThread << L" have read data" << std::endl;
    m_mtx.unlock();

    // write
    HANDLE hFileWrite = CreateFile(m_ToFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL , NULL);
    if (hFileWrite == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFileWrite);
        throw FileExeption(FILE_ERRORS, "file didn't open for writting");
    }

    dwPtr = SetFilePointer(hFileWrite, m_fileSizeLimit * idxThread, NULL, FILE_BEGIN);
    if (dwPtr == INVALID_SET_FILE_POINTER)
    {
        CloseHandle(hFileWrite);
        throw FileExeption(FILE_ERRORS, "invalid set file pointer for writting");
    }

    res_io = WriteFile(hFileWrite, reinterpret_cast<LPVOID>(&buffer[0]), buffer.size(), reinterpret_cast<LPDWORD>(&sizeBytesIsReaded), NULL);
    if (!res_io)
    {
        CloseHandle(hFileWrite);
        throw FileExeption(FILE_ERRORS, "Error of reading");
    }
    m_mtx.lock();
    std::wcout << L"Thread: " << idxThread << L" have write data" << std::endl;
    m_mtx.unlock();
    CloseHandle(hFileWrite);
    m_mtx.lock();
    std::wcout << L"Thread: " << idxThread << L" is ended" << std::endl;
    m_mtx.unlock();
}

template<typename T>
void MultyThreadCopyFileAction<T>::operator()(const T & i)
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
	m_fileSizeLimit = (m_fileSizeLimit > 0 ? m_fileSizeLimit * 1024 * 1024 : 1024 * 1024);
	m_amountThreads = static_cast<size_t>((m_currentFileSize / m_fileSizeLimit) <= 0 
						? 1 : std::ceil(static_cast<double>(m_currentFileSize) / static_cast<double>(m_fileSizeLimit)));

    for (size_t j = 0; j < m_amountThreads; j++)
    {
        std::thread t(&MultyThreadCopyFileAction<T>::CopyFile, this, j);
        m_threads.emplace_back(std::move(t));
    }

    for (auto & th : m_threads)
    {
        th.join();
    }

	std::wcout << L"Amount threads : " << m_amountThreads << std::endl;
}