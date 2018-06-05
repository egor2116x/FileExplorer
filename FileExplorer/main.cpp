#include "stdafx.h"
#include "FileExplorerEngine.h"
#include "FSAction.h"

int main()
{
	FileExplorerEngine fee;
    fee.SetRootFolderPath(std::wstring(L"C:\\test\\*"));
    fee.GetListFilesInFolder();

	fee.print<std::wostream>(std::wcout);

	//fee.makeAction<WriteFileAction<Item>>(WriteFileAction<Item>(L"C:\\test\\DIR_f1\\t2.txt", L"It's new data"));
	//fee.makeAction<ReadFileAction<Item>>(ReadFileAction<Item>(L"C:\\test\\DIR_f1\\t2.txt"));
	//fee.makeAction<RenameDirAction<Item>>(RenameDirAction<Item>(L"DIR_"));
	//fee.makeAction<RenameFileAction<Item>>(RenameFileAction<Item>(L"txt", L"NewAddName", 10));
	fee.makeAction<MultyThreadCopyFileAction<Item>>(MultyThreadCopyFileAction<Item>(L"C:\\test\\DIR_f1\\t1.txt", L"C:\\test\\DIR_f1\\t2.txt", 1));
	
	std::cout << "Press any key for exit";
    std::cin.get();
    return 0;
}

