#include "tsmcAPIClient.h"

#define FILE_1 "test.csv"
#define FILE_2 "test.csv"

const std::string command = "Disco test";

StatusCode Upload_test(tsmcAPIClient &client, std::string file_path_1, std::string file_path_2);
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
    if ((ret = Upload_test(client, FILE_1, FILE_2))!= TSMC_OK){
        std::cerr << "Upload test failed. Code: " << ret << std::endl;
    }else{
        std::cout << "Upload test pass." << std::endl;
    }


	system("pause");
    return 0;
}

StatusCode Upload_test(tsmcAPIClient &client, std::string file_path_1, std::string file_path_2){
    std::string result;
    std::string path1 = TEST_FILE_PATH + file_path_1;
    std::string path2 = TEST_FILE_PATH + file_path_2;
    std::vector<BYTE> file1, file2;

    if (!LoadBmpToVector(path1.c_str(), file1) || !LoadBmpToVector(path2.c_str(), file2)) {
        return TSMC_BITMAP_FAILED;
    }

    StatusCode ret = client.AOIApi(command, file1, file2, result);
    return ret;
}