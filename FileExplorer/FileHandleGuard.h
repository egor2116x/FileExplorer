#pragma once
class FileHandleGuard
{
    public:
        explicit FileHandleGuard(HANDLE h = 0) : m_handle(h) {}
        ~FileHandleGuard();
        HANDLE Get() const { return m_handle; }
        HANDLE Release();
        void Reset(HANDLE h);
    private:
        FileHandleGuard(const FileHandleGuard &) = delete;
        FileHandleGuard(const FileHandleGuard &&) = delete;
        FileHandleGuard & operator=(FileHandleGuard &) = delete;
        FileHandleGuard && operator=(FileHandleGuard &&) = delete;
    private:
        HANDLE m_handle;
};

