#pragma once
class Log
{
    public:
        static Log & GetInstance() { return m_instance; }
        void PrintToLog(size_t idxThread, const std::string & mess);
        void PrintToLog(size_t idxThread, const std::wstring && mess);
        bool IsPrint() const { return m_print; }
        void SetPrint(bool p) { m_print = p; }
    private:    
        Log(bool print = true) : m_print(print) {}
    private:
        bool m_print;
        std::mutex m_mtx;
        static Log m_instance;
};

