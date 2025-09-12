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

std::string GetFullPath(const std::string& fileName, bool getFilePath) {
    char buffer[MAX_PATH];
    if (getFilePath) { GetFullPathNameA(fileName.c_str(), MAX_PATH, buffer, nullptr); }
    else { GetModuleFileNameA(NULL, buffer, MAX_PATH); }
    // !getFilePath == getExePath
    return buffer;
}