#ifndef CURSOR_UITLS
#define CURSOR_UTILS

#include <array>
std::array<int, 2> FetchCursorPosition();
const std::array<int, 2> FetchMonDimensions();

void setGrabCloseCursor();

#endif