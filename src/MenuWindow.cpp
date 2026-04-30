#include "MenuWindow.h"
#include "OptionsWindow.h"
#include "UIHelpers.h"

void Button::render(SDL_Renderer* renderer) const {
    drawRoundedRect(renderer, bounds, 8.0f, colR, colG, colB, 255, hovered);
    drawRectBorder(renderer, bounds, 200, 200, 200);
    drawTextCentred(renderer, label, bounds, 1.5f, 255, 255, 255);
}

bool Button::contains(float mx, float my) const {
    return pointInRect(mx, my, bounds);
}

void TextBox::render(SDL_Renderer* renderer) const {
    uint8_t bgR = focused ? 60 : 35;
    drawRoundedRect(renderer, bounds, 6.0f, bgR, bgR, bgR + 10, 255, false);
    drawRectBorder(renderer, bounds,
                   focused ? 120 : 80,
                   focused ? 180 : 80,
                   focused ? 255 : 80);
    drawText(renderer, label, bounds.x, bounds.y - 16.0f, 1.2f, 180, 180, 180);

    std::string display = value.empty() ? "0" : value;
    uint8_t alpha = value.empty() ? 100 : 255;
    drawText(renderer, display + (focused ? "_" : ""),
             bounds.x + 10, bounds.y + (bounds.h - 10) / 2.0f,
             1.2f, alpha, alpha, alpha);
}

bool TextBox::contains(float mx, float my) const {
    return pointInRect(mx, my, bounds);
}

void TextBox::handleKey(SDL_Keycode key, const std::string&) {
    if (focused && key == SDLK_BACKSPACE && !value.empty())
        value.pop_back();
}

void TextBox::handleTextInput(const std::string& inputText) {
    if (!focused) return;
    for (char c : inputText)
        if (std::isdigit(c) && static_cast<int>(value.size()) < maxLen)
            value += c;
}

int TextBox::getIntValue() const {
    if (value.empty()) return 0;
    try { return std::stoi(value); } catch (...) { return 0; }
}

MenuWindow::MenuWindow() { winW = 600; winH = 400; }

bool MenuWindow::init() {
    if (!initSDL("Maze Runner", winW, winH)) return false;
    layout();
    return true;
}

void MenuWindow::layout() {
    float cx   = winW / 2.0f;
    float btnW = 260.0f;
    float btnH = 36.0f;
    float gap  = 6.0f;
    float top  = 220.0f;

    btnStart   = { { cx - btnW/2, top,                    btnW, btnH }, "START GAME", 30,  110, 60  };
    btnOptions = { { cx - btnW/2, top + (btnH + gap),     btnW, btnH }, "OPTIONS",    60,  60,  130 };
    btnExit    = { { cx - btnW/2, top + (btnH + gap) * 2, btnW, btnH }, "EXIT",       130, 40,  40  };
}

void MenuWindow::drawBackground() {
    SDL_SetRenderDrawColor(renderer, 18, 18, 28, 255);
    SDL_FRect full = { 0, 0, (float)winW, (float)winH };
    SDL_RenderFillRect(renderer, &full);

    SDL_SetRenderDrawColor(renderer, 30, 30, 45, 255);
    for (int x = 0; x < winW; x += 30) SDL_RenderLine(renderer, x, 0, x, winH);
    for (int y = 0; y < winH; y += 30) SDL_RenderLine(renderer, 0, y, winW, y);
}

void MenuWindow::drawTitle() {
    drawTextCentredInWindow(renderer, "MAZE",   winW, 62.0f,  6.0f, 20,  20,  20 );
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
                return { MenuAction::EXIT, options };
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                float mx = event.motion.x, my = event.motion.y;
                btnStart.hovered   = btnStart.contains(mx, my);
                btnOptions.hovered = btnOptions.contains(mx, my);
                btnExit.hovered    = btnExit.contains(mx, my);
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                       event.button.button == SDL_BUTTON_LEFT) {
                float mx = event.button.x, my = event.button.y;

                if (btnStart.contains(mx, my)) {
                    cleanup();
                    return { MenuAction::START, options };
                }
                if (btnExit.contains(mx, my)) {
                    cleanup();
                    return { MenuAction::EXIT, options };
                }
                if (btnOptions.contains(mx, my)) {
                    OptionsWindow opts(options);
                    if (opts.init()) opts.run();
                }
            } else if (event.type == SDL_EVENT_KEY_DOWN &&
                       event.key.key == SDLK_ESCAPE) {
                cleanup();
                return { MenuAction::EXIT, options };
            }
        }

        drawBackground();
        drawTitle();
        btnStart.render(renderer);
        btnOptions.render(renderer);
        btnExit.render(renderer);

        presentFrame();
    }
}
