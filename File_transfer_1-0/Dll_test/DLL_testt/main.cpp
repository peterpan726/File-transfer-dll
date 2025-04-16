#include "tsmcAPIClient.h"

#define FILE "test.csv"

const std::string command = "Disco test";

StatusCode Upload_test(tsmcAPIClient &client, std::string file_path);
std::string TEST_FILE_PATH = ".\\csv_file\\";

int main(int argc, char* argv[]) {
    std::wstring iniPath = L".\\config.ini";
    std::string result;
    StatusCode ret = TSMC_OK;
    Config config;
    tsmcAPIClient client;

    if (argc > 1){
        TEST_FILE_PATH = argv[1];
    }
    std::cout << TEST_FILE_PATH << std::endl;
    ret = client.INIT(iniPath);
    std::wcout << L"Version: " << client.GetVersion() << std::endl << std::endl;

    if (ret > 0){
        std::cerr << "client init error : " << ret << std::endl;
        return 1;
    }

    // Bmp test
    if ((ret = Upload_test(client, FILE))!= TSMC_OK){
        std::cerr << "Upload test failed. Code: " << ret << std::endl;
    }else{
        std::cout << "Upload test pass." << std::endl;
    }


	system("pause");
    return 0;
}

StatusCode Upload_test(tsmcAPIClient &client, std::string file_path){
    std::string result;
    std::string path = TEST_FILE_PATH + file_path;
    std::vector<BYTE> file;

    if (!LoadBmpToVector(path.c_str(), file)) {
        return TSMC_BITMAP_FAILED;
    }

    StatusCode ret = client.FDCApi(command, file, result);
    return ret;
}