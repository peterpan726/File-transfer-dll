#ifndef LOGGER_H
#define LOGGER_H

#include <io.h>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <afxwin.h>
#include <direct.h>
#include <iostream>
#include <iomanip>

enum LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

class Logger {
public:
    static bool Initialize(const std::string& path);
    static void Log(LogLevel level, const std::string& message);
    static void SetLogLevel(LogLevel level);

private:
    static HANDLE _hLogSemaphore;
    static LogLevel current_log_level;
    static std::string _log_base;
    static bool WriteToFile(const std::string& message);
    static std::string GetCurrentTime();
    static std::string Logger::GetLogPath();
    static bool Logger::CheckFolder(const std::string& path);
};


extern std::stringstream log_stream;

#endif // LOGGER_H
