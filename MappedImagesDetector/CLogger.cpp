#include "StdInc.h"

static const char* GetColorCode(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Info:
            return "\x1b[32m";            // green
        case LogLevel::Warning:
            return "\x1b[33m";            // yellow
        case LogLevel::Error:
            return "\x1b[31m";            // red
        case LogLevel::Debug:
            return "\x1b[36m";            // cyan
        default:
            return "\x1b[0m";
    }
}

static const char* ResetColor()
{
    return "\x1b[0m";
}

Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() : m_Stop(false), m_bUseFile(false)
{
    // enable ANSI escape sequence processing on Windows console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE)
    {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode))
        {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }

    m_Worker = std::thread(&Logger::WorkerThreadProc, this);
}

Logger::~Logger()
{
    m_Stop = true;
    m_CondVar.notify_all();
    if (m_Worker.joinable())
        m_Worker.join();

    if (m_LogFile.is_open())
        m_LogFile.close();
}

void Logger::SetLogFile(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (m_LogFile.is_open())
        m_LogFile.close();

    m_LogFile.open(filePath, std::ios::out | std::ios::app);
    m_bUseFile = m_LogFile.is_open();
}

const char* Logger::LevelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARN";
        case LogLevel::Error:
            return "ERR";
        case LogLevel::Debug:
            return "DBG";
        default:
            return "UNK";
    }
}

void Logger::Log(LogLevel level, const char* format, ...)
{
    char    msgbuf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf_s(msgbuf, sizeof(msgbuf), _TRUNCATE, format, args);
    va_end(args);

    LogEntry entry;
    entry.level = level;
    entry.timestamp = std::time(nullptr);
    entry.message = msgbuf;

    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Queue.push(std::move(entry));
    }
    m_CondVar.notify_one();
}

void Logger::WorkerThreadProc()
{
    while (!m_Stop)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_CondVar.wait(lock, [this]() { return !m_Queue.empty() || m_Stop.load(); });

        while (!m_Queue.empty())
        {
            LogEntry entry = std::move(m_Queue.front());
            m_Queue.pop();
            lock.unlock();

            // format time
            std::tm tm{};
            localtime_s(&tm, &entry.timestamp);
            char timebuf[64];
            std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm);

            // final line
            std::string line = "[" + std::string(timebuf) + "]" + GetColorCode(entry.level) + " [" + LevelToString(entry.level) + "] " + entry.message + "\n";

            // colored console output
            std::cout << line << ResetColor();

            // write to file if enabled
            if (m_bUseFile && m_LogFile.is_open())
            {
                m_LogFile << line;
                m_LogFile.flush();
            }

            lock.lock();
        }
    }

    // flush remaining entries if any
    std::lock_guard<std::mutex> lock(m_Mutex);
    while (!m_Queue.empty())
    {
        LogEntry entry = std::move(m_Queue.front());
        m_Queue.pop();

        std::tm tm{};
        localtime_s(&tm, &entry.timestamp);
        char timebuf[64];
        std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", &tm);
        std::string line = std::string(timebuf) + " [" + LevelToString(entry.level) + "] " + entry.message + "\n";
        std::cout << GetColorCode(entry.level) << line << ResetColor();
        if (m_bUseFile && m_LogFile.is_open())
        {
            m_LogFile << line;
            m_LogFile.flush();
        }
    }
}
