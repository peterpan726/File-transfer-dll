#include "tsmcAPIClient.h"


StatusCode tsmcAPIClient::INIT(std::wstring inipath) {
    Config config;
    readIniFile(inipath, READ_LOG_PATH_FLAG, config); // read log path
    if (!Logger::Initialize(config.log_path)) return TSMC_LOGGER_INIT_FAILED;
    Logger::SetLogLevel(LOG_LEVEL);

    Logger::Log(LOG_INFO, "Current Version: " + WStringToString(VERSION_TSMCAPI));
    if (!readIniFile(inipath, READ_INI_FLAG, config)) return TSMC_CONFIG_INIT_FAILED;

    char agent[] = "VC6 HTTP Client/1.0";
    log_stream.str(""); log_stream.clear();

    _hSession = InternetOpen(agent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    if (_hSession == NULL) {
        log_stream << "InternetOpen failed with error: " << GetLastError();
        Logger::Log(LOG_ERROR, log_stream.str());
        return TSMC_INIT_FAILED;
    }

    DWORD timeout = config.request_timeout;
    InternetSetOption(_hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOption(_hSession, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOption(_hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));


    _hConnect = InternetConnect(_hSession, config.host.c_str(), config.port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

    if (!_hConnect) {
        log_stream << "InternetConnect failed with error: " << GetLastError();
        Logger::Log(LOG_ERROR, log_stream.str());

        InternetCloseHandle(_hSession);
        _hSession = NULL;
        return TSMC_INIT_FAILED;
    }

    _endpoint = config.endpoint;
    _retry_sleep = config.retry_sleep;
    _max_request_retry = config.max_request_retry;
    
    _initialized = true;
    return TSMC_OK;
}

tsmcAPIClient::~tsmcAPIClient() {
    CloseConnection();
}

StatusCode tsmcAPIClient::FDCApi(const std::string& str, const std::vector<BYTE>& fileData1, std::string& result) {
    if (!_initialized) return TSMC_NOT_INITAILIZED;

    StatusCode res = TSMC_FAIL;

    for (int i = 0 ; i < _max_request_retry; ++i){
        log_stream.str(""); log_stream.clear();
        res = _check_connected();
        if (res == TSMC_OK){
            res = sendMultipartRequest(str, fileData1, result);
            if (res == TSMC_OK || res == TSMC_POST_FAILED) break;
        }
        log_stream << "Retry now: " << i+1 << " ";
        Logger::Log(LOG_INFO, log_stream.str());
        Sleep(_retry_sleep);
        res = TSMC_EXCEED_MAX_RETRY;
    }

    if (res != TSMC_OK){
        log_stream << "AOIapi_get_result func failed err: " << res ;
        Logger::Log(LOG_ERROR, log_stream.str());
    }

    return res;
}

StatusCode tsmcAPIClient::sendMultipartRequest(const std::string& textPart, const std::vector<BYTE>& fileData, std::string& result) {
    HINTERNET hRequest = HttpOpenRequest(_hConnect, "POST", _endpoint.c_str(), NULL, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    log_stream.str(""); log_stream.clear();

    if (!hRequest) {
        log_stream << L"HttpOpenRequest failed with error " << GetLastError();
        Logger::Log(LOG_ERROR, log_stream.str());

        return TSMC_OPEN_REQUEST_FAILED;
    }

    std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
    std::string formData;

    formData += "--" + boundary + "\r\n";
    formData += "Content-Disposition: form-data; name=\"cmd\"\r\n\r\n";
    formData += textPart + "\r\n";
    formData += "--" + boundary + "\r\n";
    formData += "Content-Disposition: form-data; name=\"file1\"; filename=\"file1.csv\"\r\n";
    formData += "Content-Type: text/csv\r\n\r\n";
    formData.append(reinterpret_cast<const char*>(&fileData[0]), fileData.size());
    formData += "\r\n";
    formData += "--" + boundary + "--\r\n";
    
    std::string headers = "Content-Type: multipart/form-data; boundary=" + boundary;
    BOOL bResults = HttpSendRequest(hRequest, headers.c_str(), headers.length(), (LPVOID)formData.c_str(), formData.size());

    if (!bResults) {
        log_stream << L"HttpSendRequest failed with error " << GetLastError();
        Logger::Log(LOG_ERROR, log_stream.str());
        InternetCloseHandle(hRequest);
        return TSMC_SEND_REQUEST_FAILED;
    }

    DWORD dwStatusCode = 0;
    DWORD dwSize = sizeof(dwStatusCode);
    if (HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwSize, NULL)) {
        if (dwStatusCode >= 400) {
            log_stream << L"POST failed with StatusCode: " << GetLastError();
            Logger::Log(LOG_ERROR, log_stream.str());
            return TSMC_POST_FAILED;
        }
    } else {
        log_stream << "HttpQueryInfo failed. Error: " << GetLastError();
        Logger::Log(LOG_ERROR, log_stream.str());
        return TSMC_POST_FAILED;
    }

    dwSize = 0;
    DWORD dwDownloaded = 0;
    char szBuffer[4096];
    std::string responseString;

    do {
        dwSize = sizeof(szBuffer) - 1;
        if (InternetReadFile(hRequest, (LPVOID)szBuffer, dwSize, &dwDownloaded) && dwDownloaded) {
            szBuffer[dwDownloaded] = '\0';
            responseString.append(szBuffer);
        } else {
            break;
        }
    } while (dwDownloaded);

    InternetCloseHandle(hRequest);
    result = responseString;

    return TSMC_OK;
}


StatusCode tsmcAPIClient::_check_connected(){
    HINTERNET hRequest = HttpOpenRequest(_hConnect, "HEAD", "/", NULL, NULL, NULL, 0, 0);
    if (!hRequest) {
        // std::cerr << "HttpOpenRequest failed. Error: " << GetLastError() << std::endl;
        return TSMC_OPEN_REQUEST_FAILED;
    }

    BOOL bSend = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
    InternetCloseHandle(hRequest);

    if (!bSend) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_INTERNET_DISCONNECTED) {
            // std::cerr << "Connection is offline." << std::endl;
            return TSMC_CONNECTION_OFFLINE;
        } else {
            // std::cerr << "HttpSendRequest failed. Error: " << dwError << std::endl;
            return TSMC_SEND_REQUEST_FAILED;
        }
    }

    return TSMC_OK;
}

void tsmcAPIClient::CloseConnection() {
    if (_hConnect) {
        InternetCloseHandle(_hConnect);
        _hConnect = NULL;
    }
    if (_hSession) {
        InternetCloseHandle(_hSession);
        _hSession = NULL;
    }
}

std::wstring tsmcAPIClient::GetVersion(void) {
    return VERSION_TSMCAPI;
}

StatusCode tsmcAPIClient::tsmc_ping(){
    if (!_initialized) return TSMC_NOT_INITAILIZED;
    
    if (tsmcAPIClient::_check_connected() != TSMC_OK)
        return TSMC_FAIL;
    
    return TSMC_OK;
}
