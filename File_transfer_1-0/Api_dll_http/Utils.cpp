#include "Utils.h"

std::stringstream log_stream;

bool readIniFile(const std::wstring& filePath, const bool readLogPath, Config &conf) {
    log_stream.str(""); log_stream.clear();

    wchar_t buffer[1024];

    if (readLogPath){
        GetPrivateProfileStringW(L"TsmcService", L"LogPath", L".\\", buffer, sizeof(buffer) / sizeof(buffer[0]), filePath.c_str());
        conf.log_path = WStringToString(buffer);
        std::cout << "Log Path: " << conf.log_path << std::endl;
        return true;
    }
    
    GetPrivateProfileStringW(L"TsmcService", L"host", L"", buffer, sizeof(buffer) / sizeof(buffer[0]), filePath.c_str());
    conf.host = WStringToString(buffer);
    if (conf.host.length() == 0) {
        Logger::Log(LOG_ERROR, "Config host not found.");
        return false;
    }

    GetPrivateProfileStringW(L"TsmcService", L"port", L"Not Found", buffer, sizeof(buffer) / sizeof(buffer[0]), filePath.c_str());
    conf.port = _wtoi(buffer); 

    if (conf.port > 65535 || conf.port <= 0){
        log_stream << "Config Port error. -> " << buffer;
        Logger::Log(LOG_ERROR, log_stream.str());
        return false;
    } 
    
    GetPrivateProfileStringW(L"TsmcService", L"endpoint", L"", buffer, sizeof(buffer) / sizeof(buffer[0]), filePath.c_str());
    conf.endpoint = WStringToString(buffer);

    GetPrivateProfileStringW(L"Connection", L"request_timeout_seconds", L"Not Found", buffer, sizeof(buffer) / sizeof(buffer[0]), filePath.c_str());
    conf.request_timeout = _wtoi(buffer) * 1000;
    if (conf.request_timeout <= 0){
        conf.request_timeout = DEFAULT_TIMEOUT;
        log_stream << ";Timeout error. -> " << buffer << "  Set default timeout: " << DEFAULT_TIMEOUT;
    }

    GetPrivateProfileStringW(L"Connection", L"retries_sleep_seconds", L"Not Found", buffer, sizeof(buffer) / sizeof(buffer[0]), filePath.c_str());
    conf.retry_sleep = _wtoi(buffer) * 1000;
    if (conf.retry_sleep == 0){
        conf.retry_sleep = DEFAULT_SLEEP;
        log_stream << ";Retry sleep error. -> " << buffer << "  Set default retry sleep: " << DEFAULT_SLEEP;
    }
    
    GetPrivateProfileStringW(L"Connection", L"max_request_retry", L"Not Found", buffer, sizeof(buffer) / sizeof(buffer[0]), filePath.c_str());
    conf.max_request_retry = _wtoi(buffer);
    if (conf.max_request_retry <= 0){
        conf.max_request_retry = DEFAULT_RETRY;
        log_stream << ";Max retry error. -> " << buffer
        << "  Set default max retry : " << DEFAULT_RETRY;
    }

    if (log_stream.str().length() > 0){
        Logger::Log(LOG_ERROR, log_stream.str());
    }
        
    return true;
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], sizeNeeded, NULL, NULL);
    return strTo;
}
