#include "stdafx.h"
#include "FileExplorerEngine.h"
#include "FSAction.h"

int main()
{
	FileExplorerEngine fee;
    fee.SetRootFolderPath(std::wstring(L"C:\\test\\*"));
    fee.GetListFilesInFolder();

	fee.print(std::wcout);

	//fee.makeAction(WriteFileAction<Item>(L"C:\\test\\DIR_f1\\t2.txt", L"It's new data"));
	//fee.makeAction(ReadFileAction<Item>(L"C:\\test\\DIR_f1\\t2.txt"));
	//fee.makeAction(RenameDirAction<Item>(L"DIR_"));
	//fee.makeAction(RenameFileAction<Item>(L"txt", L"NewAddName", 10));
	//fee.makeAction(MultyThreadCopyFileAction<Item>(L"C:\\test\\DIR_f1\\t1.txt", L"C:\\test\\DIR_f1\\t2.txt", 1));
    
    try
    {
        fee.makeAction(MultyThreadCopyFileAction<Item>(L"C:\\test\\DIR_f1\\CATIA_Sample.7z", L"C:\\test\\DIR_f1\\CATIA_Sample2.7z", 1));
    }
    catch (const ThreadExeption & ex)
    {
        std::cout << ex.what() << std::endl;
    }
    catch (const FileExeption & ex)
    {
        std::cout << ex.what() << std::endl;
    }

	std::cout << "Press any key for exit";
    std::cin.get();
    return 0;
}

