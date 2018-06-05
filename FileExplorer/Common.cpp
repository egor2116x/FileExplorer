#include "stdafx.h"
#include "Common.h"

std::vector <Item> g_fileMap;

std::wstring FilePathOperation::GetFileExtantion(const std::wstring & fName)
{
	std::wstring::size_type pos = fName.rfind(L'.');
	if (pos != std::wstring::npos)
	{
		return fName.substr(pos + 1, fName.size());
	}
	return std::wstring();
}

std::wstring FilePathOperation::GetFileNameWithExtantion(const std::wstring & fName)
{
	std::wstring tmp(fName);
	std::wstring::size_type pos = tmp.rfind(L'\\');
	if (pos != std::wstring::npos)
	{
		return tmp.substr(pos + 1, tmp.size());
	}

	return std::wstring();
}

void FilePathOperation::CutFileNameWithExtantion(std::wstring & fName)
{
	std::wstring::size_type pos = fName.rfind(L'\\');
	if (pos != std::wstring::npos)
	{
		fName = fName.substr(0, pos + 1);
	}
}

void FilePathOperation::CutFileExtantion(std::wstring & fName)
{
	std::wstring::size_type pos = fName.rfind(L'.');
	if (pos != std::wstring::npos)
	{
		fName = fName.substr(pos + 1, fName.size());
	}
}

void FilePathOperation::CutLastFilePathItem(std::wstring & fName)
{
	std::wstring::size_type pos = fName.rfind(L'\\');
	if (pos != std::wstring::npos)
	{
		fName = fName.substr(0, pos);
	}
}