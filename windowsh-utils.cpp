#include <iostream>
#include <array>
#include "windows.h"
#include "windowsh-utils.hpp"

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

const HCURSOR grabCloseCursor { LoadCursorFromFileA("./assets/grabbing.cur") };
void setGrabCloseCursor() {
    SetCursor(grabCloseCursor);
}

std::string GetFullExePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return buffer;
}

std::string GetCWD() {
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);
    return std::string { buffer }; 
}