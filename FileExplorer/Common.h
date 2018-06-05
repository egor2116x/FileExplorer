#pragma once

#define ERROR_CODE unsigned long
#define INCORRECT_USER_INPUT 1
enum {FILE_MARK, DIR_MARK};

struct Item
{
	unsigned m_level;
	long m_itemType;
	std::wstring m_path;
	std::wstring m_fName;
	size_t m_size;
	HANDLE m_pFile;
	Item(unsigned l, long it, const std::wstring & p, const std::wstring & fn, size_t s, HANDLE pFile = nullptr)
		: m_level(l), m_itemType(it), m_path(p), m_fName(fn), m_size(s), m_pFile(pFile) {}
};

extern std::vector <Item> g_fileMap;

namespace FilePathOperation
{
	std::wstring GetFileExtantion(const std::wstring & fName);
	std::wstring GetFileNameWithExtantion(const std::wstring & fName);
	void CutFileNameWithExtantion(std::wstring & fName);
	void CutFileExtantion(std::wstring & fName);
	void CutLastFilePathItem(std::wstring & fName);
}