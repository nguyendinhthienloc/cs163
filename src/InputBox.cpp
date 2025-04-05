#include "InputBox.h"

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
    m_cursorLine(0)           // Start at the first line
{}

void ShPTextBox::update() {
    // Update blinker timer
    m_timer += GetFrameTime();
    if (m_timer > 0.75f) {
        m_timer = 0;
        m_showBlinker = !m_showBlinker;
    }

    // Check if mouse is hovering
    m_isHovering = CheckCollisionPointRec(GetMousePosition(), m_box);

    // Enter/exit typing mode
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_isHovering) {
        m_isTyping = true;
        Vector2 mousePos = GetMousePosition();

        // Calculate the Y position of the click (which line)
        size_t lineClicked = (mousePos.y - m_box.y) / textBoxTextSize;

        // Split m_content into lines and determine the number of lines
        std::vector<std::string> lines = splitContentIntoLines(m_content);

        if (lineClicked < lines.size()) {
            m_cursorLine = lineClicked;
        }
        else {
            m_cursorLine = lines.size() - 1;
        }

        // Calculate the X position within the line
        int clickPosX = mousePos.x - m_box.x - 7;
        std::string& clickedLine = lines[m_cursorLine];

        // Determine the cursor position based on click
        m_cursorCharIndex = getCursorIndexForClick(clickedLine, clickPosX);
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !m_isHovering) {
        m_isTyping = false;
    }

    // Handle typing logic
    if (m_isTyping) {
        int input = GetCharPressed();
        while (input > 0) {
            std::vector<std::string> lines = splitContentIntoLines(m_content);
            std::string& currentLine = lines[m_cursorLine];

            if (currentLine.length() >= m_maxLineLength) {
                m_content += '\n';  // Move to the next line if max length is exceeded
                lines.push_back("");
                m_cursorLine++;
            }

            if (m_content.length() < m_maxLength) {
                if (isalpha(input) || isdigit(input) || input == ' ') {
                    currentLine.insert(m_cursorCharIndex, 1, (char)input);
                    m_cursorCharIndex++;  // Move the cursor position forward
                    m_content = rebuildContentFromLines(lines);
                }
            }
            input = GetCharPressed();
        }

        // Handle backspace with delay when holding the backspace key
        handleBackspace();

        // Handle Enter key for new line
        if (IsKeyPressed(KEY_ENTER) && m_content.length() < m_maxLength) {
            // Split content into lines
            std::vector<std::string> lines;
            std::stringstream ss(m_content);
            std::string line;
            while (std::getline(ss, line, '\n')) {
                lines.push_back(line);
            }

            // Get the current line at the cursor position
            std::string& currentLine = lines[m_cursorLine];

            // Split the current line at the cursor position
            std::string beforeCursor = currentLine.substr(0, m_cursorCharIndex);
            std::string afterCursor = currentLine.substr(m_cursorCharIndex);

            // Insert the newline within the current line
            currentLine = beforeCursor + "\n" + afterCursor;

            // If it's the last line, add a new empty line
            if (m_cursorLine == lines.size() - 1) {
                lines.push_back("");  // Add a new line at the end
            }

            // Rebuild the content with the new line inserted within the current line
            m_content.clear();
            for (size_t i = 0; i < lines.size(); ++i) {
                m_content += lines[i];
                if (i != lines.size() - 1) {
                    m_content += '\n';  // Add newline between lines except the last one
                }
            }

            // Move the cursor to the next line and start at the beginning of that line
            m_cursorLine++;  // Move to the next line after the newline
            m_cursorCharIndex = 0;  // Place the cursor at the beginning of the new line
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
    return lines;
}

std::string ShPTextBox::rebuildContentFromLines(const std::vector<std::string>& lines) {
    std::string rebuiltContent;
    for (size_t i = 0; i < lines.size(); ++i) {
        rebuiltContent += lines[i];
        if (i != lines.size() - 1) {
            rebuiltContent += '\n';  // Add newline except for the last line
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
    static float backspaceTimer = 0.0f;
    if (IsKeyDown(KEY_BACKSPACE)) {
        backspaceTimer += GetFrameTime();
        if (backspaceTimer > 0.1f) {  // Set a delay for continuous deletion
            std::vector<std::string> lines = splitContentIntoLines(m_content);
            std::string& currentLine = lines[m_cursorLine];

            // Handle backspace within a non-empty line
            if (!currentLine.empty() && m_cursorCharIndex > 0) {
                // Normal backspace behavior: delete the character at the cursor
                currentLine.erase(m_cursorCharIndex - 1, 1);
                m_cursorCharIndex--;  // Move the cursor position back
                m_content = rebuildContentFromLines(lines); // Rebuild content after modification
            }
            // If the current line is empty and we're at the start of the line, merge with the previous line
            else if ((currentLine.empty() || m_cursorCharIndex == 0) && m_cursorLine > 0) {
                std::string& previousLine = lines[m_cursorLine - 1];

                // Merge the current line (empty) with the previous line
                previousLine += currentLine;  // Merge without adding a newline

                // Remove the empty line
                lines.erase(lines.begin() + m_cursorLine);

                // Update the cursor to the end of the previous line after merging
                m_cursorLine--;
                m_cursorCharIndex = previousLine.length(); // Set cursor at the end of the merged line

                // Rebuild the content with merged lines
                m_content = rebuildContentFromLines(lines);
            }

            backspaceTimer = 0.0f;  // Reset the timer after each deletion
        }
    }
    else {
        backspaceTimer = 0.0f;  // Reset the timer if the key is not held
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

    // Draw each line
    float y = m_box.y + 5;  // Top padding
    for (size_t i = 0; i < lines.size(); ++i) {
        DrawText(lines[i].c_str(), m_box.x + 7, y + i * textBoxTextSize, textBoxTextSize, m_textColor);
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
            float blinkerY = y + cursorLine * textBoxTextSize;

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