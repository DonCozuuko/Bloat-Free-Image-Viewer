#include <array>
#include "windows.h"
#include "cursor-pos.hpp"

POINT cursor {};

std::array<int, 2> FetchCursorPosition() {
    GetCursorPos(&cursor);
    std::array<int, 2> cursorPos {cursor.x, cursor.y};
    return cursorPos;
}

const std::array<int, 2> FetchMonDimensions() {
    const int screenWidth { GetSystemMetrics(SM_CXSCREEN) };
    const int screenHeight = { GetSystemMetrics(SM_CYSCREEN) };
    const std::array<int, 2> dimensions { screenWidth, screenHeight };
    return dimensions;
}