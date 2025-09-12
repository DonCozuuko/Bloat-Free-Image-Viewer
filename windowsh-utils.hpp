#ifndef WINDOWSH_UITLS
#define WINDOWSH_UTILS

#include <array>
std::array<int, 2> FetchCursorPosition();
const std::array<int, 2> FetchMonDimensions();

void setGrabCloseCursor();

std::string GetFullPath(const std::string& fileName, bool getFilePath);

#endif