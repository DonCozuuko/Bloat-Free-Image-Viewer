#include <iostream>
#include <vector>
#include <array>
#include <string>
#include "raylib.h"

#include "cursor-pos.hpp"

// windows photo viewer has three resizing modes:
// - has a minimum width and height + padding based on the images dimensions
// - remembers the dimensions of the window from the previous session
// - full screen mode + resizable

enum Png { CLOSE, MAXIMIZE, MINIMIZE, RESTORE };

namespace val {
    // Misc globals that are important to the function of the window
    // To be overrided during display loop, so no const qualifer
    bool closeWindow { false };
    bool dragging { false };
    bool clickTitleBar { false };
    bool btnIsPressed { false };
    Vector2 dragOffset {};
    Vector2 winAbsPos {};
    Vector2 dragNewPos {};
    std::vector<int> cursorPosVec {};
    // const std::vector<int> monDims { FetchMonDimensions() };
}

namespace win {
    // Data for the window
    // Main Window
    // int width { 100 };
    // int height { 100 };
    // constexpr Color windowColor { 36, 40, 59, 255 };
    constexpr const char* title { "Enjoy Your Images Bitch!" };
    // Font font {};
    // constexpr int fontSize { 16 };
    // const int monWidth { val::monDims[0] };  // this is not safe, but idc
    // const int monHeight { val::monDims[1] }; // this is not safe, but idc
    // constexpr int NUM_ICONS { 4 };
    // bool isMaximized { false };
    // Title Bar
    // constexpr int titleBarHeight { 30 };
    // constexpr int titleBtnWidth { 50 };
    // float exitBtnWidth {};  // cant be constexpr since width and height will be changed
    // constexpr Color hoverColor { 62, 64, 80, 255 };
    // int textLengthPixels {};
    // Cursor Shit
    int cursorX {};
    int cursorY {};
    // Image Shit
    // Image currImage {};
    // Texture2D currImageTexture {};
    // char* fileName {};
    // Member functions
    // void LoadFileNameLengthPixels() {
    //     const Vector2 length { MeasureTextEx(win::font, fileName, fontSize, 10) } ;
    //     textLengthPixels = length.x;
    //     std::cout << length.x << " " << length.y << " \n"; 
    // }
    // void LoadTitleBarFont() {
    //     // font = LoadFontEx("res/segoeui.ttf", win::fontSize, 0, 0);
    //     font = LoadFontEx("res/JetBrainsMono-Bold.ttf", fontSize, 0, 0);
    // }
    // void LoadDisplayedImageAsTexture() {
    //     currImageTexture = LoadTextureFromImage(currImage);
    // }
    // void addPadding() {
    //     win::width = currImage.width;
    //     win::height = currImage.height + win::titleBarHeight; 
    //     win::exitBtnWidth = static_cast<float>(win::width - win::titleBtnWidth);  // cant be constexpr since width and height will be changed
    // }
}

// namespace arr {
//     // To hold title bar shit
//     std::array<Texture2D, win::NUM_ICONS> icons {};
//     std::array<bool, win::NUM_ICONS> btnIsHovered { false, false, false, false };
//     std::array<Rectangle, win::NUM_ICONS> btnRec {};

//     void LoadTitleBarIconTextures() {
//         icons.at(CLOSE) = LoadTexture("./res/close.png");
//         icons.at(MAXIMIZE) = LoadTexture("./res/maximize.png");
//         icons.at(MINIMIZE) = LoadTexture("./res/minimize.png");
//         icons.at(RESTORE) = LoadTexture("./res/restore.png");  
//     }

//     void LoadTitleBarBtnRecs() {
//         btnRec.at(CLOSE) = Rectangle { win::exitBtnWidth, 0, win::titleBtnWidth, win::titleBarHeight };
//         btnRec.at(MAXIMIZE) = Rectangle { win::exitBtnWidth - win::titleBtnWidth, 0, win::titleBtnWidth, win::titleBarHeight };
//         btnRec.at(MINIMIZE) = Rectangle { win::exitBtnWidth - (win::titleBtnWidth * 2), 0, win::titleBtnWidth, win::titleBarHeight };
//         btnRec.at(RESTORE) = Rectangle { win::exitBtnWidth - win::titleBtnWidth, 0, win::titleBtnWidth, win::titleBarHeight };
//     }
// }

constexpr int MAX_NUM_ICONS { 4 };

class Window {
private:
    int m_winWidth;
    int m_winHeight;
    int m_monWidth;
    int m_monHeight;
    static constexpr int m_titleBarHeight { 30 };
    static constexpr int m_titleBtnWidth { 50 };
    static constexpr int m_fontSize { 16 };
    Font m_font { LoadFontEx("res/JetBrainsMono-Bold.ttf", m_fontSize, 0, 0) };  // LoadFontEx() must be called after InitWindow()
    const Color m_windowColor { 36, 40, 59, 255 };
    const Color m_hoverColor { 62, 64, 80, 255 };
    int m_closeBtnStartPosX;
    int m_maxBtnStartPosX;
    int m_minBtnStartPosX;
    int m_fileNameLengthPixels;
    
    bool m_isMaximized { false };
    bool m_btnIsPressed { false };
    bool m_winIsClosed { false };
    bool m_titleBarIsClicked { false };
    bool m_winIsDragged { false };
    bool m_imageIsClicked { false };
    bool m_imageIsDragged { false };

    Image m_currImage;
    Texture2D m_currImageTexture;
    Rectangle m_imgRec;
    double m_imgScale { 1.0 };
    char* m_fileName;

    float m_textStartPosX;  // this starting x pos will center the text on the titlebar (when not maximized)
    int m_titleBarLineWidth;
    float m_imgCenterPosX;
    float m_imgCenterPosY;

    std::array<Texture2D, MAX_NUM_ICONS> m_icons;
    std::array<bool, MAX_NUM_ICONS> m_btnIsHovered { false, false, false, false };
    std::array<Rectangle, MAX_NUM_ICONS> m_btnRec;
    
public:
    Window(const Image& currImage, char* fileName)
        :  m_winWidth { currImage.width }, m_winHeight { currImage.height + m_titleBarHeight }, m_currImage { currImage }
         , m_fileName { fileName }
    {
        const std::array<int, 2> monDims { FetchMonDimensions() };
        m_monWidth = monDims.at(0);
        m_monHeight = monDims.at(1);
        m_closeBtnStartPosX = m_winWidth - m_titleBtnWidth;
        m_maxBtnStartPosX = m_closeBtnStartPosX - m_titleBtnWidth;
        m_minBtnStartPosX = m_maxBtnStartPosX - m_titleBtnWidth;
        m_currImageTexture = LoadTextureFromImage(m_currImage);

        m_icons.at(CLOSE) = LoadTexture("./res/close.png");
        m_icons.at(MAXIMIZE) = LoadTexture("./res/maximize.png");
        m_icons.at(MINIMIZE) = LoadTexture("./res/minimize.png");
        m_icons.at(RESTORE) = LoadTexture("./res/restore.png"); 

        m_btnRec.at(CLOSE) = Rectangle { static_cast<float>(m_closeBtnStartPosX), 0.0f, static_cast<float>(m_titleBtnWidth), static_cast<float>(m_titleBarHeight) };
        m_btnRec.at(MAXIMIZE) = Rectangle { static_cast<float>(m_maxBtnStartPosX), 0.0f, static_cast<float>(m_titleBtnWidth), static_cast<float>(m_titleBarHeight) };
        m_btnRec.at(MINIMIZE) = Rectangle { static_cast<float>(m_minBtnStartPosX), 0.0f, static_cast<float>(m_titleBtnWidth), static_cast<float>(m_titleBarHeight) };
        m_btnRec.at(RESTORE) = Rectangle { static_cast<float>(m_maxBtnStartPosX), 0.0f, static_cast<float>(m_titleBtnWidth), static_cast<float>(m_titleBarHeight) };

        const Vector2 length { MeasureTextEx(m_font, m_fileName, m_fontSize, 0) } ;
        m_fileNameLengthPixels = length.x;

        m_textStartPosX = static_cast<float>((m_currImage.width / 2.0f) - (m_fileNameLengthPixels / 2.0f));
        m_titleBarLineWidth = m_currImage.width;
        // the range of the y pos is [m_titleBarHeight, m_monHeight - 40] inclusive
        m_imgCenterPosX = static_cast<float>((m_monWidth / 2.0f) - (m_currImage.width / 2.0f));
        m_imgCenterPosY = static_cast<float>((m_monHeight / 2.0f) - (m_currImage.height / 2.0f) - 10.0f);
        // std::cout << m_monWidth << " " << m_monHeight << "\n";
    }

    void draggingLogic(const Vector2& mousePos, std::array<int, 2>& cursorPosVec, int& cursorX, int& cursorY,
                        Vector2& winAbsPos, Vector2& dragOffset, Vector2& dragNewPos) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (!m_isMaximized) {
                if (mousePos.y < m_titleBarHeight && mousePos.x < m_minBtnStartPosX) {
                    m_titleBarIsClicked = true;
                }
            }
            else {
                ;
            }
        }
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && m_titleBarIsClicked) {
            cursorPosVec = FetchCursorPosition();
            cursorX = cursorPosVec[0];
            cursorY = cursorPosVec[1];
            if (!m_winIsDragged) {
                m_winIsDragged = true;
                winAbsPos = GetWindowPosition();
                dragOffset.x = cursorX - winAbsPos.x;
                dragOffset.y = cursorY - winAbsPos.y;
            }
            dragNewPos.x = cursorX - dragOffset.x;
            dragNewPos.y = cursorY - dragOffset.y; 
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            m_winIsDragged = false;
            m_titleBarIsClicked = false;
        }
        if (m_winIsDragged) {
            // y collisions, 60 is roughly the height of the windows taskbar (in pixels)
            if (dragNewPos.y < 0) {
                SetWindowPosition(dragNewPos.x, 0);
            }
            else if (dragNewPos.y + m_titleBarHeight > m_monHeight - 60) {
                SetWindowPosition(dragNewPos.x, m_monHeight - 60);
            }
            else {
                SetWindowPosition(dragNewPos.x, dragNewPos.y);
            }
        }
    }

    void checkClickOrHoverOnTitleBar(const Vector2& mousePos) {
        for (int i {}; i < MAX_NUM_ICONS; ++i) {
            if (!m_isMaximized && i == RESTORE) {
                continue;
            }
            if (m_isMaximized && i == MAXIMIZE) {
                continue;
            }

            if (CheckCollisionPointRec(mousePos, m_btnRec[i]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !m_btnIsPressed) {
                m_btnIsPressed = true;
                switch(i) {
                    case CLOSE:
                        m_winIsClosed = true;
                        break;
                    case MAXIMIZE:
                        maximizeWindowBitch();
                        break;
                    case MINIMIZE:
                        MinimizeWindow();
                        m_btnIsPressed = false;
                        break;
                    case RESTORE:
                        restoreWindowBitch();
                        break;
                    default:
                        break;
                }
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                m_btnIsPressed = false;
            }
            else if (CheckCollisionPointRec(mousePos, m_btnRec[i]) && IsCursorOnScreen()) {
                m_btnIsHovered[i] = true;
            }
            else if (m_btnIsHovered[i]) {  // if m_btnIsHovered is true
                m_btnIsHovered[i] = false;
            }
        }
    }

    void maximizeWindowBitch() {
        MaximizeWindow();
        m_isMaximized = true;
        // change starting x pos
        m_btnRec[CLOSE].x = static_cast<float>(m_monWidth - m_titleBtnWidth);
        m_btnRec[MAXIMIZE].x = static_cast<float>(m_btnRec[CLOSE].x - m_titleBtnWidth);
        m_btnRec[RESTORE].x = m_btnRec[MAXIMIZE].x;
        m_btnRec[MINIMIZE].x = static_cast<float>(m_btnRec[MAXIMIZE].x - m_titleBtnWidth);

        m_textStartPosX = (m_monWidth / 2.0f) - (m_fileNameLengthPixels / 2.0f);
        m_titleBarLineWidth = m_monWidth;
    }

    void restoreWindowBitch() {
        RestoreWindow();
        m_isMaximized = false;
        m_btnRec[CLOSE].x = static_cast<float>(m_closeBtnStartPosX);
        m_btnRec[MAXIMIZE].x = static_cast<float>(m_maxBtnStartPosX);
        m_btnRec[RESTORE].x = m_btnRec[MAXIMIZE].x;
        m_btnRec[MINIMIZE].x = static_cast<float>(m_minBtnStartPosX);
        
        m_textStartPosX = (m_currImage.width / 2.0f) - (m_fileNameLengthPixels / 2.0f);
        m_titleBarLineWidth = m_currImage.width;
    }

    void drawTitleBarBtns() {
        ClearBackground(m_windowColor);
        DrawTextEx(m_font, m_fileName, (Vector2){m_textStartPosX, 5.0f}, m_fontSize, 0, RAYWHITE);
        for (int i {}; i < MAX_NUM_ICONS; ++i) {
            if (!m_isMaximized && i == RESTORE) {
                continue;
            }
            if (m_isMaximized && i == MAXIMIZE) {
                continue;
            }

            if (i != CLOSE) {
                (m_btnIsHovered[i]) ? DrawRectangleRec(m_btnRec[i], m_hoverColor) : DrawRectangleRec(m_btnRec[i], BLANK);  
            }
            else {
                (m_btnIsHovered[CLOSE]) ? DrawRectangleRec(m_btnRec[CLOSE], RED) : DrawRectangleRec(m_btnRec[CLOSE], BLANK);  
            }

            if (i == RESTORE) {
                DrawTextureEx(m_icons[RESTORE], (Vector2){m_btnRec[RESTORE].x + 15, m_btnRec[RESTORE].y + 6}, 0, 0.5f, WHITE);
            }
            else {
                DrawTextureEx(m_icons[i], (Vector2){m_btnRec[i].x + 13, m_btnRec[i].y + 2}, 0, 1.0f, WHITE);
            }
        }
        DrawLine(0, m_titleBarHeight, m_titleBarLineWidth, m_titleBarHeight, WHITE);
        DrawLine(m_textStartPosX, m_titleBarHeight, m_textStartPosX + m_fileNameLengthPixels, m_titleBarHeight, RED);
    }

    void zoomingInNOutLogic() {
        if (m_isMaximized) {
            // std::cout << "You son of a bitch, im in!\n";
            if (IsKeyDown(KEY_LEFT_CONTROL)) {
                if (IsKeyPressed(KEY_EQUAL)) {
                    m_imgScale += 0.05;
                }
                else if (IsKeyDown(KEY_EQUAL)) {
                    m_imgScale += 0.05;
                }
                if (IsKeyPressed(KEY_MINUS)) {
                    m_imgScale -= 0.05;
                }
                else if (IsKeyDown(KEY_MINUS)) {
                    m_imgScale -= 0.05;
                }
            }
            
        }
    }

    void drawImage() {
        if (m_isMaximized) {DrawTextureEx(m_currImageTexture, (Vector2){m_imgCenterPosX, m_imgCenterPosY}, 0, m_imgScale, WHITE); }
        else { DrawTextureEx(m_currImageTexture, (Vector2){0, m_titleBarHeight}, 0, m_imgScale, WHITE); }
    }

    int getWinIsClosedFlag() { return m_winIsClosed; }
};


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Input an image file you stupid fuck\n";
        return 1;
    }

    char* fileName { argv[1] };
    const Image currImage { LoadImage(fileName) };

    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_RESIZABLE);
    InitWindow(currImage.width, currImage.height, "Enjoy Your Images Bitch!");
    SetTargetFPS(60);
    
    Window win { currImage, fileName };
    std::array<int, 2> cursorPosVec {};

    int cursorX {}, cursorY {};
    Vector2 dragOffset {};
    Vector2 winAbsPos {};
    Vector2 dragNewPos {};

    bool ctrl { false };
    while (!WindowShouldClose() && !win.getWinIsClosedFlag()) {
        const Vector2 mousePos = GetMousePosition();
        win.checkClickOrHoverOnTitleBar(mousePos);
        win.draggingLogic(mousePos, cursorPosVec, cursorX, cursorY, winAbsPos, dragOffset, dragNewPos);
        win.zoomingInNOutLogic();
        
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyDown(KEY_SPACE) && IsKeyPressed(KEY_X)) {
            win.maximizeWindowBitch();
        }
        BeginDrawing();
            win.drawTitleBarBtns();
            win.drawImage();
    
        EndDrawing();
    }
    UnloadImage(currImage);
    CloseWindow();
}