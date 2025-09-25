#ifndef WINDOWSH_UITLS
#define WINDOWSH_UTILS

#include <array>
std::array<int, 2> FetchCursorPosition();
const std::array<int, 2> FetchMonDimensions();

void setGrabCloseCursor();

std::string GetFullExePath();

std::string GetCWD();

#endif