#ifndef BMP_UTILS_H
#define BMP_UTILS_H

#include <afxwin.h>
#include <vector>
#include "Logger.h"

bool LoadBmpToVector(const char* filePath, std::vector<BYTE>& byteVector);
bool SaveVectorToBmp(const char* filePath, const std::vector<BYTE>& byteVector);

#endif // BMP_UTILS_H