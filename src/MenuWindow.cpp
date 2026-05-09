#include "MenuWindow.h"
#include "OptionsWindow.h"
#include "UIHelpers.h"

// Draws the button background (brightened on hover), a border, and the centred label.
void Button::render(SDL_Renderer* renderer) const {
    drawRoundedRect(renderer, bounds, 8.0f, colR, colG, colB, 255, hovered);    // Skills Audit Group B: Simple user defined algorithms (hover-based colour brightening)
    drawRectBorder(renderer, bounds, 200, 200, 200);
    drawTextCentred(renderer, label, bounds, 1.5f, 255, 255, 255);
}

// Returns true if the given mouse coordinate falls within the button rectangle.
bool Button::contains(float mx, float my) const {
    return pointInRect(mx, my, bounds);    // Skills Audit Group B: Simple user defined algorithms (point-in-rect hit testing)
}

// Draws the text box with a blue border when focused, grey otherwise.
void TextBox::render(SDL_Renderer* renderer) const {
    uint8_t bgR = focused ? 60 : 35;    // Skills Audit Group B: Simple user defined algorithms (conditional background shade based on focus state)
    drawRoundedRect(renderer, bounds, 6.0f, bgR, bgR, bgR + 10, 255, false);
    drawRectBorder(renderer, bounds,
                   focused ? 120 : 80,
                   focused ? 180 : 80,
                   focused ? 255 : 80);

    // Draw the helper label just above the box.
    drawText(renderer, label, bounds.x, bounds.y - 16.0f, 1.2f, 180, 180, 180);

    // Show a blinking cursor character when focused; show a placeholder "0" when empty.
    std::string display = value.empty() ? "0" : value;
    uint8_t alpha = value.empty() ? 100 : 255;    // Skills Audit Group B: Simple user defined algorithms (placeholder dimming when value is empty)
    drawText(renderer, display + (focused ? "_" : ""),
             bounds.x + 10, bounds.y + (bounds.h - 10) / 2.0f,
             1.2f, alpha, alpha, alpha);
}

bool TextBox::contains(float mx, float my) const {
    return pointInRect(mx, my, bounds);    // Skills Audit Group B: Simple user defined algorithms (point-in-rect hit testing)
}

// Handles Backspace by removing the last character from the value string.
void TextBox::handleKey(SDL_Keycode key, const std::string&) {
    if (focused && key == SDLK_BACKSPACE && !value.empty())
        value.pop_back();    // Skills Audit Group B: Simple user defined algorithms (delete last character on Backspace)
}

// Appends only digit characters from SDL text-input events, up to the maximum length.
void TextBox::handleTextInput(const std::string& inputText) {
    if (!focused) return;
    for (char c : inputText)
        if (std::isdigit(c) && static_cast<int>(value.size()) < maxLen)    // Skills Audit Group B: Simple user defined algorithms (digit-only input filtering with length cap)
            value += c;
}

// Parses the stored string as an integer; returns 0 if the string is empty or invalid.
int TextBox::getIntValue() const {
    if (value.empty()) return 0;
    try { return std::stoi(value); } catch (...) { return 0; }    // Skills Audit Group B: Simple user defined algorithms (safe string-to-int conversion)
}

MenuWindow::MenuWindow() { winW = 600; winH = 400; }    // Skills Audit Group B: Generation of objects based on simple OOP model

bool MenuWindow::init() {
    if (!initSDL("Maze Runner", winW, winH)) return false;
    layout();
    return true;
}

// Positions the three buttons evenly below centre of the window.
void MenuWindow::layout() {
    float cx   = winW / 2.0f;    // Skills Audit Group B: Simple scientific/mathematical model (horizontal centre calculation)
    float btnW = 260.0f;
    float btnH = 36.0f;
    float gap  = 6.0f;
    float top  = 220.0f;

    btnStart   = { { cx - btnW/2, top,                    btnW, btnH }, "START GAME", 30,  110, 60  };    // Skills Audit Group B: Records (Button struct initialised with position, size and colour)
    btnOptions = { { cx - btnW/2, top + (btnH + gap),     btnW, btnH }, "OPTIONS",    60,  60,  130 };
    btnExit    = { { cx - btnW/2, top + (btnH + gap) * 2, btnW, btnH }, "EXIT",       130, 40,  40  };
}

// Fills the background with a dark colour and overlays a faint grid pattern.
void MenuWindow::drawBackground() {
    SDL_SetRenderDrawColor(renderer, 18, 18, 28, 255);
    SDL_FRect full = { 0, 0, (float)winW, (float)winH };    // Skills Audit Group B: Records (SDL_FRect struct used as fill region)
    SDL_RenderFillRect(renderer, &full);

    SDL_SetRenderDrawColor(renderer, 30, 30, 45, 255);
    for (int x = 0; x < winW; x += 30) SDL_RenderLine(renderer, x, 0, x, winH);    // Skills Audit Group B: Simple user defined algorithms (evenly spaced grid lines)
    for (int y = 0; y < winH; y += 30) SDL_RenderLine(renderer, 0, y, winW, y);
}

// Draws "MAZE RUNNER" in two passes to create a dark drop-shadow effect.
void MenuWindow::drawTitle() {
    drawTextCentredInWindow(renderer, "MAZE",   winW, 62.0f,  6.0f, 20,  20,  20 );    // Skills Audit Group B: Simple user defined algorithms (drop-shadow by drawing text twice with a 2-pixel offset)
    drawTextCentredInWindow(renderer, "MAZE",   winW, 60.0f,  6.0f, 80,  160, 255);
    drawTextCentredInWindow(renderer, "RUNNER", winW, 114.0f, 5.0f, 20,  20,  20 );
    drawTextCentredInWindow(renderer, "RUNNER", winW, 112.0f, 5.0f, 200, 220, 255);
}

MenuResult MenuWindow::run() {
    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                cleanup();
                return { MenuAction::EXIT, options };    // Skills Audit Group B: Records (MenuResult struct returned to caller)
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                // Update each button's hover state as the mouse moves.
                float mx = event.motion.x, my = event.motion.y;
                btnStart.hovered   = btnStart.contains(mx, my);    // Skills Audit Group B: Simple user defined algorithms (point-in-rect per button)
                btnOptions.hovered = btnOptions.contains(mx, my);
                btnExit.hovered    = btnExit.contains(mx, my);
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                       event.button.button == SDL_BUTTON_LEFT) {
                float mx = event.button.x, my = event.button.y;

                // START closes the menu window and returns the play action.
                if (btnStart.contains(mx, my)) {
                    cleanup();
                    return { MenuAction::START, options };    // Skills Audit Group B: Records (MenuResult struct carrying chosen action and settings)
                }
                if (btnExit.contains(mx, my)) {
                    cleanup();
                    return { MenuAction::EXIT, options };
                }
                // OPTIONS opens the OptionsWindow modally; the menu stays open underneath.
                if (btnOptions.contains(mx, my)) {
                    OptionsWindow opts(options);    // Skills Audit Group B: Generation of objects based on simple OOP model
                    if (opts.init()) opts.run();
                }
            } else if (event.type == SDL_EVENT_KEY_DOWN &&
                       event.key.key == SDLK_ESCAPE) {
                cleanup();
                return { MenuAction::EXIT, options };
            }
        }

        // Redraw the full menu each frame.
        drawBackground();
        drawTitle();
        btnStart.render(renderer);
        btnOptions.render(renderer);
        btnExit.render(renderer);

        presentFrame();
    }
}
