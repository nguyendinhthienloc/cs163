#pragma once

#include "raylib.h"
#include <string>
#include <cctype>
#include <sstream>
#include <vector>
#include <algorithm>

class ShPTextBox {
public:
    bool m_isTyping;
    int m_scrollOffset = 0;
    ShPTextBox(Vector2 position, Vector2 size, Color boxColor, Color textColor, int maxLength, int maxLineLength);
    void update();
    void render();
    void setText(std::string content);
    std::string getText();
    void clearContent();
    void setPosition(Vector2 position);
    void setBoxSize(Vector2 size);
    std::vector<std::string> splitContentIntoLines(const std::string& content);
    std::string rebuildContentFromLines(const std::vector<std::string>& lines);
    int getCursorIndexForClick(const std::string& line, int clickPosX);
    void handleBackspace();
    void scrollUp();
    void scrollDown();
    void DrawScrollBar();

private:
    Rectangle m_box;
    Color m_boxColor;
    Color m_textColor;
    std::string m_content;
    int m_maxLength;
    int m_maxLineLength;
    bool m_showBlinker;
    float m_timer;
    bool m_isHovering;
    size_t m_cursorCharIndex;
    size_t m_cursorLine;
    const int m_maxVisibleLines = 16;
    bool lastLineWasDeleted = false;

    static const Color textBoxColor;
    static const Color textBoxColorInputing;
    static const int textBoxTextSize;
};
