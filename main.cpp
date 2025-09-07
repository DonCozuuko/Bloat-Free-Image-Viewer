#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <filesystem>
#include "raylib.h"

#include "cursor-utils.hpp"

enum Png { CLOSE, MAXIMIZE, MINIMIZE, RESTORE };

constexpr int MAX_NUM_ICONS { 4 };
constexpr char* imgStatOGScale { "1.0 Original Scale!" };
constexpr char* shortCutsBlurb { "r = restore image default\nf = maximize\nd = restore down\nh,j,k,l vim nav\ni = hide this on/off" };
bool isHidden { false };

class Window {
private:
    int m_winWidth;
    int m_winHeight;
    int m_monWidth;
    int m_monHeight;
    static constexpr int m_titleBarHeight { 30 };
    static constexpr int m_titleBtnWidth { 50 };
    static constexpr int m_fontSize { 16 };
    Font m_font { LoadFontEx("./assets/JetBrainsMono-Bold.ttf", m_fontSize, 0, 0) };  // LoadFontEx() must be called after InitWindow()
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
    bool m_imgIsClicked { false };
    bool m_imgIsDragged { false };

    Image m_currImage;
    Texture2D m_currImageTexture;
    Rectangle m_imgRec;
    Vector2 m_imgCurrPos;
    Vector2 m_imgCenterPos;
    Vector2 m_imgMinimizePos { 0.0f, m_titleBarHeight };
    double m_imgScale { 1.0 };
    const char* m_fileName;

    float m_textStartPosX;  // this starting x pos will center the text on the titlebar (when not maximized)
    int m_titleBarLineWidth;
    Rectangle m_titleBarRec;

    std::array<Texture2D, MAX_NUM_ICONS> m_icons;
    std::array<bool, MAX_NUM_ICONS> m_btnIsHovered { false, false, false, false };
    std::array<Rectangle, MAX_NUM_ICONS> m_btnRec;

public:
    Window(const Image& currImage, const char* fileName)
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

        m_icons.at(CLOSE) = LoadTexture("./assets/close.png");
        m_icons.at(MAXIMIZE) = LoadTexture("./assets/maximize.png");
        m_icons.at(MINIMIZE) = LoadTexture("./assets/minimize.png");
        m_icons.at(RESTORE) = LoadTexture("./assets/restore.png"); 

        m_btnRec.at(CLOSE) = Rectangle { static_cast<float>(m_closeBtnStartPosX), 0.0f, static_cast<float>(m_titleBtnWidth), static_cast<float>(m_titleBarHeight) };
        m_btnRec.at(MAXIMIZE) = Rectangle { static_cast<float>(m_maxBtnStartPosX), 0.0f, static_cast<float>(m_titleBtnWidth), static_cast<float>(m_titleBarHeight) };
        m_btnRec.at(MINIMIZE) = Rectangle { static_cast<float>(m_minBtnStartPosX), 0.0f, static_cast<float>(m_titleBtnWidth), static_cast<float>(m_titleBarHeight) };
        m_btnRec.at(RESTORE) = Rectangle { static_cast<float>(m_maxBtnStartPosX), 0.0f, static_cast<float>(m_titleBtnWidth), static_cast<float>(m_titleBarHeight) };

        const Vector2 length { MeasureTextEx(m_font, m_fileName, m_fontSize, 0) } ;
        m_fileNameLengthPixels = length.x;

        m_textStartPosX = static_cast<int>(static_cast<float>(m_currImage.width / 2.0) - static_cast<float>(m_fileNameLengthPixels / 2.0));
        m_titleBarLineWidth = m_currImage.width;
        m_titleBarRec.x = 0;
        m_titleBarRec.y = 0;
        m_titleBarRec.width = m_minBtnStartPosX;
        m_titleBarRec.height = m_titleBarHeight;
        // the range of the y pos is [m_titleBarHeight, m_monHeight - 40] inclusive
        m_imgCenterPos.x = static_cast<int>(static_cast<float>(m_monWidth / 2.0) - static_cast<float>(m_currImage.width / 2.0));
        m_imgCenterPos.y = static_cast<int>(static_cast<float>(m_monHeight / 2.0) - static_cast<float>(m_currImage.height / 2.0) - 10.0);
        m_imgCurrPos.x = 0;
        m_imgCurrPos.y = m_titleBarHeight;
    
        m_imgRec.x = m_imgCurrPos.x;
        m_imgRec.y = m_imgCurrPos.y;
        m_imgRec.width = m_currImage.width;
        m_imgRec.height = m_currImage.height;
        // std::cout << m_imgRec.width << " " << m_imgRec.height << " \n";
    }

    void draggingLogic(const Vector2& mousePos, std::array<int, 2>& cursorPosVec, int& cursorX, int& cursorY,
                        Vector2& winAbsPos, Vector2& dragOffset, Vector2& dragNewPos) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mousePos, m_titleBarRec)) {
                std::cout << "I DONT GIVE A FUCKKKK\n";
                m_titleBarIsClicked = true;
            }
            else if (m_isMaximized && CheckCollisionPointRec(mousePos, m_imgRec)) {
                std::cout << "JUST SHAKE THAT ASS BITCH\n";
                m_imgIsClicked = true;
                setGrabCloseCursor();
            }
        }

        if ((m_titleBarIsClicked || m_imgIsClicked) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            // Window dragging uses system cursor position, while image dragging uses
            // raylib window cursor position
            if (m_titleBarIsClicked) {
                cursorPosVec = FetchCursorPosition();
                cursorX = cursorPosVec[0];
                cursorY = cursorPosVec[1];
                if (!m_winIsDragged) {
                    // draggOffset should only be initialized once per hold and release
                    m_winIsDragged = true;
                    winAbsPos = GetWindowPosition();
                    dragOffset.x = cursorX - winAbsPos.x;
                    dragOffset.y = cursorY - winAbsPos.y;
                }
                dragNewPos.x = cursorX - dragOffset.x;
                dragNewPos.y = cursorY - dragOffset.y; 
            }
            else if (m_imgIsClicked) {
                if (!m_imgIsDragged) {
                    m_imgIsDragged = true;
                    dragOffset.x = mousePos.x - m_imgCurrPos.x;
                    dragOffset.y = mousePos.y - m_imgCurrPos.y;
                }
                m_imgCurrPos.x = mousePos.x - dragOffset.x;
                m_imgCurrPos.y = mousePos.y - dragOffset.y;
                m_imgRec.x = m_imgCurrPos.x;
                m_imgRec.y = m_imgCurrPos.y;
            }
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            m_winIsDragged = false;
            m_titleBarIsClicked = false;
            m_imgIsDragged = false;
            m_imgIsClicked = false;
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

            if (!m_btnIsPressed && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, m_btnRec[i])) {
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
        m_titleBarRec.x = 0;
        m_titleBarRec.y = 0;
        m_titleBarRec.width = m_btnRec[MINIMIZE].x;
        m_titleBarRec.height = m_titleBarHeight;

        m_imgCurrPos.x = m_imgCenterPos.x;
        m_imgCurrPos.y = m_imgCenterPos.y;
        m_imgRec.x = m_imgCurrPos.x;
        m_imgRec.y = m_imgCurrPos.y;
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
        m_titleBarRec.x = 0;
        m_titleBarRec.y = 0;
        m_titleBarRec.width = m_minBtnStartPosX;
        m_titleBarRec.height = m_titleBarHeight;

        m_imgCurrPos.x = 0;
        m_imgCurrPos.y = m_titleBarHeight;
        m_imgRec.x = m_imgCurrPos.x;
        m_imgRec.y = m_imgCurrPos.y;
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

    void zoomingInNOutLogic(int& currScroll, int& prevScroll) {
        // std::cout << "You son of a bitch, im in!\n";
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyPressed(KEY_EQUAL)) {
                m_imgScale += 0.05;
            }
            if (IsKeyPressed(KEY_MINUS)) {
                m_imgScale -= 0.05;
            }
            const int newImgWidth { static_cast<int>(m_currImage.width * m_imgScale) };
            const int newImgHeight { static_cast<int>(m_currImage.height * m_imgScale) };
            m_imgCurrPos.x = static_cast<int>(m_imgRec.x - ((newImgWidth - m_imgRec.width) / 2.0));
            m_imgCurrPos.y = static_cast<int>(m_imgRec.y - ((newImgHeight - m_imgRec.height) / 2.0));;
            m_imgRec.x = m_imgCurrPos.x;
            m_imgRec.y = m_imgCurrPos.y;
            m_imgRec.width = newImgWidth;
            m_imgRec.height = newImgHeight;
        }
        currScroll = static_cast<int>(GetMouseWheelMove());
        if (currScroll != 0) {
            m_imgScale += (0.05 * currScroll);
            if (m_imgScale == 1.0) {
                m_imgRec.width = m_currImage.width;
                m_imgRec.height = m_currImage.height;
            }
            else {
                const int newImgWidth { static_cast<int>(m_currImage.width * m_imgScale) };
                const int newImgHeight { static_cast<int>(m_currImage.height * m_imgScale) };
                m_imgCurrPos.x = static_cast<int>(m_imgRec.x - ((newImgWidth - m_imgRec.width) / 2.0));
                m_imgCurrPos.y = static_cast<int>(m_imgRec.y - ((newImgHeight - m_imgRec.height) / 2.0));;
                m_imgRec.x = m_imgCurrPos.x;
                m_imgRec.y = m_imgCurrPos.y;
                m_imgRec.width = newImgWidth;
                m_imgRec.height = newImgHeight;
            }
            // std::cout << m_imgRec.y << " " << m_imgRec.x << " " << m_imgRec.width << " " << m_imgRec.height << " \n";
        }
    }

    void setImageToDefaultCenterNScale() {
        m_imgScale = 1.0;
        m_imgRec.x = m_imgCenterPos.x;
        m_imgRec.y = m_imgCenterPos.y;
        m_imgRec.width = m_currImage.width;
        m_imgRec.height = m_currImage.height;
        m_imgCurrPos.x = m_imgRec.x;
        m_imgCurrPos.y = m_imgRec.y;        
    }

    void shortcuts(int& key) {
        key = GetKeyPressed();
        if (key == 0) { ; }
        else {
            switch (key) {
                case KEY_Q:
                    m_winIsClosed = true;
                    break;
                case KEY_F:
                    maximizeWindowBitch();
                    break;
                case KEY_D:
                    restoreWindowBitch();
                    break;
                case KEY_R:
                    setImageToDefaultCenterNScale();
                    break;
                case KEY_J:
                    m_imgCurrPos.y += 20;
                    m_imgRec.y = m_imgCurrPos.y;
                    // go down
                    break;
                case KEY_K:
                    m_imgCurrPos.y -= 20;
                    m_imgRec.y = m_imgCurrPos.y;
                    // go up
                    break;
                case KEY_H:
                    m_imgCurrPos.x -= 20;
                    m_imgRec.x = m_imgCurrPos.x;
                    // go left
                    break;
                case KEY_L:
                    m_imgCurrPos.x += 20;
                    m_imgRec.x = m_imgCurrPos.x;
                    // go right
                    break;
                case KEY_I:
                    if (!isHidden) { isHidden = true; }
                    else { isHidden = false; }
                    break;
                default:
                    break;
            }
        }
    }

    void drawImage() {
        if (m_isMaximized) {DrawTextureEx(m_currImageTexture, m_imgCurrPos, 0, m_imgScale, WHITE); }
        else { DrawTextureEx(m_currImageTexture, m_imgMinimizePos, 0, m_imgScale, WHITE); }
        // DrawRectangleRec(m_imgRec, RED);
        // DrawRectangleRec(m_titleBarRec, YELLOW);
        // std::cout << m_imgCurrPos.x << " " << m_imgCurrPos.y << " \n";
    }

    void drawStats() {
        if (m_imgScale == 1.0) { DrawTextEx(m_font, imgStatOGScale, Vector2{5, 5}, m_fontSize, 0, WHITE); }
        else { DrawTextEx(m_font, TextFormat("%.2f", m_imgScale), Vector2{5, 5}, m_fontSize, 0, WHITE); }
        if (!isHidden) { DrawTextEx(m_font, shortCutsBlurb, Vector2{5, 35}, m_fontSize, 0, WHITE); }
    }

    int getWinIsClosedFlag() { return m_winIsClosed; }
};

void parseFileNameFromPath() {
    
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Input an image file you stupid fuck\n";
        return 1;
    }

    const char* fileName { argv[1] };
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
    int currScroll {};
    int  prevScroll {};

    int key {};

    std::cout << GetDirectoryPath(fileName);

    while (!WindowShouldClose() && !win.getWinIsClosedFlag()) {
        const Vector2 mousePos = GetMousePosition();
        // std::cout << mousePos.x << " " << mousePos.y << " \n";

        // window functions
        win.checkClickOrHoverOnTitleBar(mousePos);
        win.draggingLogic(mousePos, cursorPosVec, cursorX, cursorY, winAbsPos, dragOffset, dragNewPos);
        win.zoomingInNOutLogic(currScroll, prevScroll);
        
        // shortcuts
        win.shortcuts(key);
        
        BeginDrawing();
            win.drawTitleBarBtns();
            win.drawImage();
            win.drawStats();
        EndDrawing();
    }
    UnloadImage(currImage);
    CloseWindow();
}