#ifndef TSMCAPI_CLIENT_H
#define TSMCAPI_CLIENT_H

#ifdef MyDLL_EXPORTS
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <fstream>
#include <memory> 
#include <iostream>
#include <ctime>

#include <afxwin.h>
#include <wininet.h>
#include "Utils.h"
#include "BmpUtils.h"
#include "Logger.h"

#pragma comment(lib, "wininet.lib")

#define VERSION_TSMCAPI L"Disco-x86-http-1.0"

// #define LOG_LEVEL LOG_ERROR
// #define LOG_LEVEL LOG_WARNING
#define LOG_LEVEL LOG_INFO

class DLL_EXPORT tsmcAPIClient {
public:
    StatusCode INIT(std::wstring inipath);
    ~tsmcAPIClient();
    StatusCode tsmcAPIClient::FDCApi(const std::string& str, const std::vector<BYTE>& fileData1, std::string& result);
    std::wstring GetVersion(void);
    StatusCode tsmc_ping();
    tsmcAPIClient() : _initialized(false) {}

private:
    HINTERNET _hSession;
    HINTERNET _hConnect;
    std::string _endpoint;
    bool _initialized;
    int _request_timeout;
    int _retry_sleep;
    int _max_request_retry;
    StatusCode sendMultipartRequest(const std::string& textPart, const std::vector<BYTE>& fileData, std::string& result);
    StatusCode _check_connected();
    void CloseConnection();
};

extern std::stringstream log_stream;

#endif // TSMCAPI_CLIENT_H
