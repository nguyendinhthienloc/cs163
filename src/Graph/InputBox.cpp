#include "../../header/Graph/InputBox.h"

// Static constant definitions
const Color ShPTextBox::textBoxColor = WHITE;
const Color ShPTextBox::textBoxColorInputing = LIGHTGRAY;
const int ShPTextBox::textBoxTextSize = 20;

ShPTextBox::ShPTextBox(Vector2 position, Vector2 size, Color boxColor, Color textColor, int maxLength, int maxLineLength)
    : m_box({ position.x, position.y, size.x, size.y }),
    m_boxColor(boxColor),
    m_textColor(textColor),
    m_content(""),
    m_maxLength(maxLength),
    m_maxLineLength(maxLineLength),
    m_isTyping(false),
    m_showBlinker(true),
    m_timer(0.0f),
    m_isHovering(false),
    m_cursorCharIndex(0),     // Initialize the cursor position
    m_cursorLine(0),          // Start at the first line
    lastLineWasDeleted(false) // Initialize the flag
{}

void ShPTextBox::update() {
    m_timer += GetFrameTime();
    if (m_timer > 0.75f) {
        m_timer = 0;
        m_showBlinker = !m_showBlinker;
    }

    std::vector<std::string> lines = splitContentIntoLines(m_content);

    m_isHovering = CheckCollisionPointRec(GetMousePosition(), m_box);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_isHovering) {
        m_isTyping = true;
        Vector2 mousePos = GetMousePosition();
        size_t lineClicked = (mousePos.y - m_box.y) / textBoxTextSize;
        lineClicked += m_scrollOffset;

        if (lineClicked < lines.size()) {
            m_cursorLine = lineClicked;
        }
        else {
            m_cursorLine = lines.size() - 1;
        }

        int clickPosX = mousePos.x - m_box.x - 7;
        std::string& clickedLine = lines[m_cursorLine];
        m_cursorCharIndex = getCursorIndexForClick(clickedLine, clickPosX);
        lastLineWasDeleted = false; // Reset on click
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !m_isHovering) {
        m_isTyping = false;
    }

    if (m_isTyping) {
        int input = GetCharPressed();
        while (input > 0) {
            std::string& currentLine = lines[m_cursorLine];

            if (currentLine.length() >= m_maxLineLength) {
                m_content += '\n';
                lines.push_back("");
                m_cursorLine++;
            }

            if (m_content.length() < m_maxLength) {
                if (isalpha(input) || isdigit(input) || input == ' ') {
                    currentLine.insert(m_cursorCharIndex, 1, (char)input);
                    m_cursorCharIndex++;
                    m_content = rebuildContentFromLines(lines);
                    lastLineWasDeleted = false; // Reset on typing
                }
            }
            input = GetCharPressed();
        }

        handleBackspace();

        if (IsKeyPressed(KEY_ENTER) && m_content.length() < m_maxLength) {
            std::string& currentLine = lines[m_cursorLine];
            std::string beforeCursor = currentLine.substr(0, m_cursorCharIndex);
            std::string afterCursor = currentLine.substr(m_cursorCharIndex);
            currentLine = beforeCursor + "\n" + afterCursor;

            if (m_cursorLine == lines.size() - 1) {
                lines.push_back("");
            }

            m_content.clear();
            for (size_t i = 0; i < lines.size(); ++i) {
                m_content += lines[i];
                if (i != lines.size() - 1) {
                    m_content += '\n';
                }
            }

            m_cursorLine++;
            m_cursorCharIndex = 0;
            lastLineWasDeleted = false; // Reset on Enter
        }

        if (IsKeyPressed(KEY_LEFT) && m_cursorCharIndex > 0) {
            m_cursorCharIndex--;
            lastLineWasDeleted = false;
        }
        else if (IsKeyPressed(KEY_LEFT) && m_cursorCharIndex == 0 && m_cursorLine > 0) {
            m_cursorLine--;
            m_cursorCharIndex = lines[m_cursorLine].length();
            lastLineWasDeleted = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && m_cursorCharIndex < lines[m_cursorLine].length()) {
            m_cursorCharIndex++;
            lastLineWasDeleted = false;
        }
        else if (IsKeyPressed(KEY_RIGHT) && m_cursorCharIndex == lines[m_cursorLine].length() && m_cursorLine < lines.size() - 1) {
            m_cursorLine++;
            m_cursorCharIndex = 0;
            lastLineWasDeleted = false;
        }
        if (IsKeyPressed(KEY_UP) && m_cursorLine > 0) {
            m_cursorLine--;
            m_cursorCharIndex = std::min(m_cursorCharIndex, (size_t)lines[m_cursorLine].length());
            lastLineWasDeleted = false;
        }
        if (IsKeyPressed(KEY_DOWN) && m_cursorLine < lines.size() - 1) {
            m_cursorLine++;
            m_cursorCharIndex = std::min(m_cursorCharIndex, (size_t)lines[m_cursorLine].length());
            lastLineWasDeleted = false;
        }

        if (m_cursorLine >= m_scrollOffset + m_maxVisibleLines) {
            m_scrollOffset = m_cursorLine - m_maxVisibleLines + 1;
        }
        else if (m_cursorLine < m_scrollOffset) {
            m_scrollOffset = m_cursorLine;
        }
    }
}

std::vector<std::string> ShPTextBox::splitContentIntoLines(const std::string& content) {
    std::vector<std::string> lines;
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line, '\n')) {
        lines.push_back(line);
    }
    if (lines.empty()) {
        lines.push_back("");
    }
    return lines;
}

std::string ShPTextBox::rebuildContentFromLines(const std::vector<std::string>& lines) {
    std::string rebuiltContent;
    for (size_t i = 0; i < lines.size(); ++i) {
        rebuiltContent += lines[i];
        if (i != lines.size() - 1) {
            rebuiltContent += '\n';
        }
    }
    return rebuiltContent;
}

int ShPTextBox::getCursorIndexForClick(const std::string& line, int clickPosX) {
    int cumulativeWidth = 0;
    int cursorIndex = 0;

    for (size_t i = 0; i < line.length(); ++i) {
        int charWidth = MeasureText(std::string(1, line[i]).c_str(), textBoxTextSize);
        if (cumulativeWidth + charWidth / 2 >= clickPosX) {
            break;
        }
        cumulativeWidth += charWidth;
        cursorIndex++;
    }

    return cursorIndex;
}

void ShPTextBox::handleBackspace() {
    static bool keyProcessed = false;
    static bool lastLineWasDeleted = false;

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!keyProcessed) {
            std::vector<std::string> lines = splitContentIntoLines(m_content);

            if (lines.empty()) {
                m_cursorLine = 0;
                m_cursorCharIndex = 0;
                m_scrollOffset = 0;
                m_content = "";
                keyProcessed = true;
                lastLineWasDeleted = false;
                return;
            }

            if (m_cursorLine >= lines.size()) {
                m_cursorLine = lines.size() - 1;
            }
            if (m_cursorLine < 0) {
                m_cursorLine = 0;
            }

            std::string& currentLine = lines[m_cursorLine];

            if (m_cursorCharIndex > currentLine.length()) {
                m_cursorCharIndex = currentLine.length();
            }
            if (m_cursorCharIndex < 0) {
                m_cursorCharIndex = 0;
            }

            if (!currentLine.empty() && m_cursorCharIndex > 0) {
                // Normal character deletion
                currentLine.erase(m_cursorCharIndex - 1, 1);
                m_cursorCharIndex--;
            }
            else if (m_cursorCharIndex == 0) {
                // Handle merging
                if (m_cursorLine > 0) {
                    bool isLastLine = (m_cursorLine == lines.size() - 1);
                    bool isEmpty = currentLine.empty();

                    std::string& prevLine = lines[m_cursorLine - 1];
                    size_t prevLineLength = prevLine.length(); // Store the length of the previous line before merging

                    if (isLastLine && isEmpty) {
                        // Merge the empty last line with the line above
                        prevLine += currentLine;
                        lines.erase(lines.begin() + m_cursorLine);
                        m_cursorLine--;
                        m_cursorCharIndex = prevLineLength; // Cursor at the end of the previous line's original content
                        lastLineWasDeleted = true; // Block further merges
                    }
                    else if (!lastLineWasDeleted) {
                        // Normal merge with the line above
                        prevLine += currentLine;
                        lines.erase(lines.begin() + m_cursorLine);
                        m_cursorLine--;
                        m_cursorCharIndex = prevLineLength; // Cursor at the end of the previous line's original content
                    }
                }
                else if (lines.size() > 1 || !currentLine.empty()) {
                    // Delete the first line if there are multiple lines or it's not empty
                    lines.erase(lines.begin());
                    m_cursorLine = 0;
                    m_cursorCharIndex = lines[0].length();
                }
            }

            m_content = rebuildContentFromLines(lines);

            int totalLines = lines.size();
            if (totalLines <= m_maxVisibleLines) {
                m_scrollOffset = 0;
            }
            else {
                int maxOffset = totalLines - m_maxVisibleLines;
                if (m_cursorLine < m_scrollOffset) {
                    m_scrollOffset = m_cursorLine;
                }
                else if (m_cursorLine >= m_scrollOffset + m_maxVisibleLines) {
                    m_scrollOffset = m_cursorLine - m_maxVisibleLines + 1;
                }
                if (m_scrollOffset > maxOffset) {
                    m_scrollOffset = maxOffset;
                }
            }
            if (m_scrollOffset < 0) {
                m_scrollOffset = 0;
            }

            keyProcessed = true;
        }
    }
    else if (IsKeyReleased(KEY_BACKSPACE)) {
        keyProcessed = false;
    }
}

void ShPTextBox::render() {
    Color boxColorToUse = m_isTyping ? textBoxColorInputing : textBoxColor;

    // Draw the textbox
    DrawRectangleRec(m_box, boxColorToUse);
    DrawRectangleLinesEx(m_box, 2, BLACK);

    // Split m_content into lines
    std::vector<std::string> lines;
    std::stringstream ss(m_content);
    std::string line;
    while (std::getline(ss, line, '\n')) {
        lines.push_back(line);
    }

    // Calculate visible lines based on scroll offset
    int startLine = m_scrollOffset;
    int endLine = std::min(startLine + m_maxVisibleLines, (int)lines.size());

    // Draw each line
    float y = m_box.y + 5;  // Top padding
    for (size_t i = startLine; i < endLine; ++i) {
        DrawText(lines[i].c_str(), m_box.x + 7, y + (i - startLine) * textBoxTextSize, textBoxTextSize, m_textColor);
    }

    // Draw blinker if typing
    if (m_isTyping && m_showBlinker) {
        if (lines.empty()) {
            DrawText("|", m_box.x + 7, y, textBoxTextSize, m_textColor);
        }
        else {
            // Calculate the cursor's line and position
            size_t cursorLine = m_cursorLine;
            size_t cursorCharIndex = m_cursorCharIndex;
            float blinkerY = y + (cursorLine - m_scrollOffset) * textBoxTextSize;

            if (cursorCharIndex > 0) {
                int textWidth = MeasureText(lines[cursorLine].substr(0, cursorCharIndex).c_str(), textBoxTextSize);
                DrawText("|", m_box.x + 7 + textWidth, blinkerY, textBoxTextSize, m_textColor);
            }
            else {
                DrawText("|", m_box.x + 7, blinkerY, textBoxTextSize, m_textColor);
            }
        }
    }
}

void ShPTextBox::setText(std::string content) {
    m_content = content;
}

std::string ShPTextBox::getText() {
    return m_content;
}

void ShPTextBox::clearContent() {
    m_cursorLine = 0;
    m_cursorCharIndex = 0;
    m_content.clear();
}

void ShPTextBox::setPosition(Vector2 position) {
    m_box.x = position.x;
    m_box.y = position.y;
}

void ShPTextBox::setBoxSize(Vector2 size) {
    m_box.width = size.x;
    m_box.height = size.y;
}

void ShPTextBox::scrollUp() {
    if (m_scrollOffset > 0) {
        m_scrollOffset--;
    }
}

void ShPTextBox::scrollDown() {
    std::vector<std::string> lines = splitContentIntoLines(m_content);
    if (m_scrollOffset + m_maxVisibleLines < (int)lines.size()) {
        m_scrollOffset++;
    }
}

template <typename T>
T Clamp(T value, T minValue, T maxValue) {
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

void ShPTextBox::DrawScrollBar() {
    const float trackTop = 290;
    const float trackBottom = 572;
    const float trackHeight = trackBottom - trackTop;
    Rectangle scrollBar = { 870, trackTop, 20, trackHeight };
    DrawRectangleRec(scrollBar, GRAY);

    // Get lines and trim trailing empty lines
    std::vector<std::string> lines = splitContentIntoLines(m_content);
    while (!lines.empty() && lines.back().empty()) {
        lines.pop_back();
    }
    int totalLines = lines.size();

    if (totalLines > m_maxVisibleLines) {
        float sliderHeight = trackHeight * ((float)m_maxVisibleLines / totalLines);
        const float minSliderHeight = 20.0f;
        if (sliderHeight < minSliderHeight) {
            sliderHeight = minSliderHeight;
        }

        int scrollRange = totalLines - m_maxVisibleLines;
        float availableTrack = trackHeight - sliderHeight;

        float sliderY;
        if (scrollRange > 0) {
            sliderY = trackTop + (m_scrollOffset * availableTrack) / scrollRange;
        }
        else {
            sliderY = trackTop;
        }

        if (sliderY + sliderHeight > trackBottom) {
            sliderY = trackBottom - sliderHeight;
        }
        if (sliderY < trackTop) {
            sliderY = trackTop;
        }

        Rectangle slider = { scrollBar.x, sliderY, scrollBar.width, sliderHeight };
        DrawRectangleRec(slider, DARKGRAY);

        static bool isDragging = false;
        static float dragStartY = 0.0f;
        if (CheckCollisionPointRec(GetMousePosition(), slider)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                m_isTyping = false;
                isDragging = true;
                dragStartY = GetMousePosition().y - slider.y;
            }
        }
        if (isDragging) {
            float mousePosY = GetMousePosition().y;
            float newSliderY = mousePosY - dragStartY;
            newSliderY = Clamp(newSliderY, trackTop, trackBottom - sliderHeight);

            if (availableTrack > 0) {
                float offsetFraction = (newSliderY - trackTop) / availableTrack;
                if (offsetFraction >= 0.99f) {
                    m_scrollOffset = scrollRange;
                }
                else {
                    m_scrollOffset = roundf(offsetFraction * scrollRange);
                    m_scrollOffset = Clamp<float>(m_scrollOffset, 0, scrollRange);
                }
            }
            else {
                m_scrollOffset = 0;
            }

            slider.y = newSliderY;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            isDragging = false;
        }
    }
    else {
        DrawRectangleRec(scrollBar, DARKGRAY);
        if (CheckCollisionPointRec(GetMousePosition(), scrollBar)) {
            m_isTyping = false;
        }
    }
}