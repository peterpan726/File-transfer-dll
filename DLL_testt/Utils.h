#ifndef UTILS_H
#define UTILS_H

#include <afxwin.h> 
#include <vector>
#include "Logger.h"
#include <memory> 


#define DEFAULT_TIMEOUT 1000
#define DEFAULT_SLEEP 1000
#define DEFAULT_RETRY 3

#define READ_LOG_PATH_FLAG true
#define READ_INI_FLAG false

enum StatusCode {
    TSMC_OK = 0,
    TSMC_FAIL, // Unknown
    
    gRPC_CHANNEL_FAIL = 101,
    gRPC_STUB_TAIL,
    gRPC_CHANNEL_IDLE,
    gRPC_CHANNEL_TRANSIENT_FAILURE,
    gRPC_CHANNEL_SHUTDOWN,
    gRPC_CHANNEL_CONNECTING,

    TSMC_TIMEOUT_EXCEED = 201,
    
    TSMC_EXCEED_MAX_RETRY = 301,
    TSMC_NOT_INITAILIZED, // if you call api without init
    TSMC_CONFIG_INIT_FAILED,
    TSMC_LOGGER_INIT_FAILED,
    TSMC_INIT_FAILED,
    TSMC_OPEN_REQUEST_FAILED,
    TSMC_CONNECTION_OFFLINE,
    TSMC_POST_FAILED,
    TSMC_SEND_REQUEST_FAILED,

    TSMC_BITMAP_FAILED = 401,

    OTHER_CLIENT = 901, // Use by Client (Shibaura)
};

struct Config {
    std::string host;
    int port;
    std::string endpoint;
    std::string log_path;
    int request_timeout;
    int retry_sleep;
    int max_request_retry;
};

bool readIniFile(const std::wstring& filePath, const bool readLogPath, Config &conf);
std::string WStringToString(const std::wstring& wstr);

#endif // UTILS_H