#pragma once

#include <string>
#include <mutex>
#include <fstream>
#include <thread>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <ctime>

enum class LogLevel
{
    Info,
    Warning,
    Error,
    Debug
};

class Logger
{
public:
    static Logger& Instance();

    void SetLogFile(const std::string& filePath);
    void Log(LogLevel level, const char* format, ...);

private:
    Logger();
    ~Logger();

    void        WorkerThreadProc();
    const char* LevelToString(LogLevel level);

    struct LogEntry
    {
        LogLevel    level;
        std::time_t timestamp;
        std::string message;
    };

    std::mutex              m_Mutex;
    std::condition_variable m_CondVar;
    std::queue<LogEntry>    m_Queue;

    std::ofstream     m_LogFile;
    std::thread       m_Worker;
    std::atomic<bool> m_Stop;
    bool              m_bUseFile;
};
