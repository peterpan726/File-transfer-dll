#include "Logger.h"

LogLevel Logger::current_log_level = LOG_INFO;
HANDLE Logger::_hLogSemaphore = NULL;
std::string Logger::_log_base = "";

bool Logger::Initialize(const std::string& path) {
    bool res = false;
    Logger::_log_base = path;
    std::string init_message = "";
    
    init_message += GetCurrentTime();
    init_message += " Init......";

    _hLogSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
    if (_hLogSemaphore == NULL) {
        std::cerr << "CreateSemaphore error: " << GetLastError() << std::endl;
        return false;
    }

    res = CheckFolder(path);

    if (res){
        std::cout << "Check log folder pass." << std::endl;
        res = WriteToFile(init_message);
    }
    
    return res;
}

bool Logger::CheckFolder(const std::string& path){
    if (_access(path.c_str(), 0) == 0) {
        return true;
    }

    size_t pos = path.find_last_of("\\");
    if (pos != std::string::npos) {
        std::string parentPath = path.substr(0, pos);

        if (!CheckFolder(parentPath)) {
            return false;
        }
    }

    return _mkdir(path.c_str()) == 0 || _access(path.c_str(), 0) == 0;
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (level >= current_log_level) {
        std::string log_message;
        switch (level) {
            case LOG_DEBUG  : log_message = "DEBUG  : "; break;
            case LOG_INFO   : log_message = "INFO   : "; break;
            case LOG_WARNING: log_message = "WARNING: "; break;
            case LOG_ERROR  : log_message = "ERROR  : "; break;
        }
        log_message += GetCurrentTime() + " - " + message;
        WaitForSingleObject(_hLogSemaphore, INFINITE);
        if (!WriteToFile(log_message)) 
            std::cerr << "Fail to write log : " << log_message << std::endl;

        ReleaseSemaphore(_hLogSemaphore, 1, NULL);
    }
}

void Logger::SetLogLevel(LogLevel level) {
    current_log_level = level;
}

bool Logger::WriteToFile(const std::string& message) {
    std::ofstream log_file;
    log_file.open(GetLogPath().c_str(), std::ios_base::app);
    if (log_file.is_open()) {
        log_file << message.c_str() << std::endl;
        log_file.close();
    }else{
        return false;
    }
    return true;
}

std::string Logger::GetCurrentTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    std::ostringstream oss;
    oss << (1900 + ltm->tm_year) << "-"
        << std::setw(2) << std::setfill('0') << (1 + ltm->tm_mon) << "-"
        << std::setw(2) << std::setfill('0') << ltm->tm_mday << " "
        << std::setw(2) << std::setfill('0') << ltm->tm_hour << ":"
        << std::setw(2) << std::setfill('0') << ltm->tm_min << ":"
        << std::setw(2) << std::setfill('0') << ltm->tm_sec;

    return oss.str();
}

std::string Logger::GetLogPath() {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    std::ostringstream oss;
    oss << _log_base
        << (1900 + ltm->tm_year) << "_"
        << std::setw(2) << std::setfill('0') << (1 + ltm->tm_mon) << "_"
        << std::setw(2) << std::setfill('0') << ltm->tm_mday << ".log";

    return oss.str();
}