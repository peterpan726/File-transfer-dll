#include "BmpUtils.h"

bool LoadBmpToVector(const char* filePath, std::vector<BYTE>& byteVector)
{
    HANDLE hFile = CreateFileA(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE)
    {
        CloseHandle(hFile);
        return false;
    }

    byteVector.resize(fileSize);

    DWORD bytesRead;
    if (!ReadFile(hFile, &byteVector[0], fileSize, &bytesRead, NULL) || bytesRead != fileSize)
    {
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}

bool SaveVectorToBmp(const char* filePath, const std::vector<BYTE>& byteVector)
{
    HANDLE hFile = CreateFileA(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    DWORD bytesWritten;
    if (!WriteFile(hFile, &byteVector[0], byteVector.size(), &bytesWritten, NULL) || bytesWritten != byteVector.size())
    {
        CloseHandle(hFile);
        return false;
    }

    CloseHandle(hFile);
    return true;
}