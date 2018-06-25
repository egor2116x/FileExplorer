#include "stdafx.h"
#include "Log.h"

Log Log::m_instance;
void Log::PrintToLog(size_t idThread, const std::string & mess)
{
    if (m_print)
    {
        m_mtx.lock();
        std::cout << "Thread: " << idThread << mess << std::endl;
        m_mtx.unlock();
    }
}

void Log::PrintToLog(size_t idThread, const std::wstring && mess)
{
    if (m_print)
    {
        m_mtx.lock();
        std::wcout << L"Thread: " << idThread << mess << std::endl;
        m_mtx.unlock();
    }
}
