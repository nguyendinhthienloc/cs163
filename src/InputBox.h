#pragma once

#include "raylib.h"
#include <string>
#include <cctype>
#include <sstream>
#include <vector>
#include <algorithm>

class ShPTextBox {
public:
    bool m_isTyping;            // Whether the textbox is active for typing
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
    Rectangle m_box;            // Position and size of the textbox
    Color m_boxColor;           // Background color
    Color m_textColor;          // Text color
    std::string m_content;      // Text content
    int m_maxLength;            // Maximum allowed characters
    int m_maxLineLength;        // Maximum allowed characters per line
    bool m_showBlinker;         // Blinking cursor visibility
    float m_timer;              // Timer for blinker
    bool m_isHovering;          // Whether mouse is hovering over textbox
    size_t m_cursorCharIndex;   // The character index in the current line
    size_t m_cursorLine;        // The line number where the cursor is
    int m_scrollOffset = 0;
    const int m_maxVisibleLines = 16;

    // Constants for styling (adjust as needed)
    static const Color textBoxColor;         // Idle color
    static const Color textBoxColorInputing; // Active color
    static const int textBoxTextSize;        // Text size
};
