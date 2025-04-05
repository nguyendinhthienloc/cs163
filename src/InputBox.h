#pragma once

#include "raylib.h"
#include <string>
#include <cctype>
#include <sstream>
#include <vector>

class ShPTextBox {
public:
    ShPTextBox(Vector2 position, Vector2 size, Color boxColor, Color textColor, int maxLength, int maxLineLength);
    void update();
    void render();
    void setText(std::string content);
    std::string getText();
    void clearContent();
    void setPosition(Vector2 position);
    void setBoxSize(Vector2 size);

private:
    Rectangle m_box;            // Position and size of the textbox
    Color m_boxColor;           // Background color
    Color m_textColor;          // Text color
    std::string m_content;      // Text content
    int m_maxLength;            // Maximum allowed characters
    int m_maxLineLength;
    bool m_isTyping;            // Whether the textbox is active for typing
    bool m_showBlinker;         // Blinking cursor visibility
    float m_timer;              // Timer for blinker
    bool m_isHovering;          // Whether mouse is hovering over textbox

    // Constants for styling (adjust as needed)
    static const Color textBoxColor;         // Idle color
    static const Color textBoxColorInputing; // Active color
    static const int textBoxTextSize;        // Text size
};