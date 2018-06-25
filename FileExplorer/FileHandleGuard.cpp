#include "stdafx.h"
#include "FileHandleGuard.h"

FileHandleGuard::~FileHandleGuard()
{
    if (m_handle)
    {
        CloseHandle(m_handle);
    }
}

HANDLE FileHandleGuard::Release()
{
    HANDLE h = m_handle;
    m_handle = 0;
    return h;
}

void FileHandleGuard::Reset(HANDLE h)
{
    if (m_handle)
    {
        CloseHandle(m_handle);
    }
    m_handle = h;
}
