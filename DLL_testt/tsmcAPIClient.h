#ifndef TSMCAPI_CLIENT_H
#define TSMCAPI_CLIENT_H

#ifdef BUILDING_DLL
#define EXPORT_DLL __declspec(dllexport)
#else
#define EXPORT_DLL __declspec(dllimport)
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

#define VERSION_TSMCAPI L"Version-20240905-XP"

// #define LOG_LEVEL LOG_ERROR
// #define LOG_LEVEL LOG_WARNING
#define LOG_LEVEL LOG_INFO

class EXPORT_DLL tsmcAPIClient {
public:
    StatusCode INIT(std::wstring inipath);
    ~tsmcAPIClient();
    StatusCode tsmcAPIClient::AOIApi(const std::string& str, const std::vector<BYTE>& fileData1, const std::vector<BYTE>& fileData2, std::string& result);
    std::string FDCApi(const char* data);
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
    StatusCode sendMultipartRequest(const std::string& textPart, const std::vector<BYTE>& fileData1, const std::vector<BYTE>& fileData2, std::string& result);
    StatusCode _check_connected();
    void CloseConnection();
};

extern std::stringstream log_stream;

#endif // TSMCAPI_CLIENT_H
