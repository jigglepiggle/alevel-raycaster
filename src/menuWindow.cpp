#pragma once

#include <SDL3/SDL.h>
#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>

#include "windowBase.h"

enum class MenuAction { START, OPTIONS, EXIT };

struct MenuResult {
    MenuAction action;
    int        seed;
};


// ─── UI primitives ────────────────────────────────────────────────────────────

static void drawRoundedRect(SDL_Renderer* renderer, const Rect& r, float radius,
                            uint8_t fillR, uint8_t fillG, uint8_t fillB, uint8_t fillA,
                            bool hovered = false) {
    if (hovered) {
        fillR = static_cast<uint8_t>(std::min(255, (int)fillR + 40));
        fillG = static_cast<uint8_t>(std::min(255, (int)fillG + 40));
        fillB = static_cast<uint8_t>(std::min(255, (int)fillB + 40));
    }

    SDL_SetRenderDrawColor(renderer, fillR, fillG, fillB, fillA);

    SDL_FRect centre = { r.x + radius, r.y, r.w - radius * 2, r.h };
    SDL_FRect left   = { r.x,                r.y + radius, radius, r.h - radius * 2 };
    SDL_FRect right  = { r.x + r.w - radius, r.y + radius, radius, r.h - radius * 2 };
    SDL_RenderFillRect(renderer, &centre);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);

    int steps = static_cast<int>(radius);
    for (int i = 0; i < steps; i++) {
        float angle = (float)i       / steps * (3.14159f / 2.0f);
        float nextA = (float)(i + 1) / steps * (3.14159f / 2.0f);
        float x0 = cos(angle) * radius,  x1 = cos(nextA) * radius;
        float y0 = sin(angle) * radius,  y1 = sin(nextA) * radius;

        SDL_FRect tl = { r.x + radius - x0,       r.y + radius - y0,       x0 - x1 + 1, y1 - y0 + 1 };
        SDL_FRect tr = { r.x + r.w - radius + x1, r.y + radius - y0,       x0 - x1 + 1, y1 - y0 + 1 };
        SDL_FRect bl = { r.x + radius - x0,       r.y + r.h - radius + y0, x0 - x1 + 1, y1 - y0 + 1 };
        SDL_FRect br = { r.x + r.w - radius + x1, r.y + r.h - radius + y0, x0 - x1 + 1, y1 - y0 + 1 };
        SDL_RenderFillRect(renderer, &tl);
        SDL_RenderFillRect(renderer, &tr);
        SDL_RenderFillRect(renderer, &bl);
        SDL_RenderFillRect(renderer, &br);
    }
}

static void drawRectBorder(SDL_Renderer* renderer, const Rect& r,
                           uint8_t bR, uint8_t bG, uint8_t bB) {
    SDL_SetRenderDrawColor(renderer, bR, bG, bB, 255);
    SDL_FRect sides[4] = {
        { r.x,           r.y,           r.w, 1   },
        { r.x,           r.y + r.h - 1, r.w, 1   },
        { r.x,           r.y,           1,   r.h },
        { r.x + r.w - 1, r.y,           1,   r.h },
    };
    for (auto& s : sides) SDL_RenderFillRect(renderer, &s);
}

static void drawText(SDL_Renderer* renderer, const std::string& text,
                     float x, float y, float scale,
                     uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    float cx = x;
    for (char c : text) {
        SDL_RenderDebugTextFormat(renderer, cx, y, "%c", c);
        cx += 8.0f * scale;
    }
}

static void drawTextCentred(SDL_Renderer* renderer, const std::string& text,
                            const Rect& bounds, float scale,
                            uint8_t r, uint8_t g, uint8_t b) {
    float tx = bounds.x + (bounds.w - text.size() * 8.0f * scale) / 2.0f;
    float ty = bounds.y + (bounds.h - 8.0f * scale) / 2.0f;
    drawText(renderer, text, tx, ty, scale, r, g, b);
}


// ─── Widgets ──────────────────────────────────────────────────────────────────

struct Button {
    Rect        bounds;
    std::string label;
    uint8_t     colR, colG, colB;
    bool        hovered = false;

    void render(SDL_Renderer* renderer) const {
        drawRoundedRect(renderer, bounds, 8.0f, colR, colG, colB, 255, hovered);
        drawRectBorder(renderer, bounds, 200, 200, 200);
        drawTextCentred(renderer, label, bounds, 2.0f, 255, 255, 255);
    }

    bool contains(float mx, float my) const { return pointInRect(mx, my, bounds); }
};

struct TextBox {
    Rect        bounds;
    std::string label;
    std::string value;
    bool        focused = false;
    int         maxLen  = 12;

    void render(SDL_Renderer* renderer) const {
        uint8_t bgR = focused ? 60 : 35;
        drawRoundedRect(renderer, bounds, 6.0f, bgR, bgR, bgR + 10, 255, false);
        drawRectBorder(renderer, bounds,
                       focused ? 120 : 80,
                       focused ? 180 : 80,
                       focused ? 255 : 80);
        drawText(renderer, label, bounds.x, bounds.y - 20.0f, 1.5f, 180, 180, 180);

        std::string display = value.empty() ? "0" : value;
        uint8_t alpha = value.empty() ? 100 : 255;
        drawText(renderer, display + (focused ? "_" : ""),
                 bounds.x + 10, bounds.y + (bounds.h - 12) / 2.0f,
                 1.5f, alpha, alpha, alpha);
    }

    bool contains(float mx, float my) const { return pointInRect(mx, my, bounds); }

    void handleKey(SDL_Keycode key, const std::string&) {
        if (focused && key == SDLK_BACKSPACE && !value.empty())
            value.pop_back();
    }

    void handleTextInput(const std::string& inputText) {
        if (!focused) return;
        for (char c : inputText)
            if (std::isdigit(c) && static_cast<int>(value.size()) < maxLen)
                value += c;
    }

    int getIntValue() const {
        if (value.empty()) return 0;
        try { return std::stoi(value); } catch (...) { return 0; }
    }
};


// ─── MenuWindow ───────────────────────────────────────────────────────────────

class MenuWindow : public WindowBase {
private:
    Button  btnStart;
    Button  btnOptions;
    Button  btnExit;
    TextBox tbSeed;

    void drawBackground() {
        SDL_SetRenderDrawColor(renderer, 18, 18, 28, 255);
        SDL_FRect full = { 0, 0, (float)winW, (float)winH };
        SDL_RenderFillRect(renderer, &full);

        SDL_SetRenderDrawColor(renderer, 30, 30, 45, 255);
        for (int x = 0; x < winW; x += 30) SDL_RenderLine(renderer, x, 0, x, winH);
        for (int y = 0; y < winH; y += 30) SDL_RenderLine(renderer, 0, y, winW, y);
    }

    void drawTitle() {
        drawText(renderer, "MAZE",   122, 62,  6.0f, 20,  20,  20 );
        drawText(renderer, "MAZE",   120, 60,  6.0f, 80,  160, 255);
        drawText(renderer, "RUNNER", 82,  122, 4.0f, 20,  20,  20 );
        drawText(renderer, "RUNNER", 80,  120, 4.0f, 200, 220, 255);
    }

    void layout() {
        float cx    = winW / 2.0f;
        float btnW  = 260.0f;
        float btnH  = 52.0f;
        float gap   = 18.0f;
        float top   = 220.0f;

        btnStart   = { { cx - btnW/2, top,                        btnW, btnH }, "START GAME", 30,  110, 60  };
        tbSeed     = { { cx - btnW/2, top + btnH + gap,           btnW, 44   }, "Seed", "", false, 10 };
        btnOptions = { { cx - btnW/2, top + (btnH + gap) * 2 + 10, btnW, btnH }, "OPTIONS",   60,  60,  130 };
        btnExit    = { { cx - btnW/2, top + (btnH + gap) * 3 + 10, btnW, btnH }, "EXIT",      130, 40,  40  };
    }

public:
    MenuWindow() { winW = 600; winH = 500; }

    bool init() {
        if (!initSDL("Maze Runner", winW, winH)) return false;
        layout();
        return true;
    }

    MenuResult run() {
        SDL_StartTextInput(window);

        while (true) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {

                if (event.type == SDL_EVENT_QUIT) {
                    SDL_StopTextInput(window);
                    cleanup();
                    return { MenuAction::EXIT, 0 };
                }
                else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                    float mx = event.motion.x, my = event.motion.y;
                    btnStart.hovered   = btnStart.contains(mx, my);
                    btnOptions.hovered = btnOptions.contains(mx, my);
                    btnExit.hovered    = btnExit.contains(mx, my);
                }
                else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                         event.button.button == SDL_BUTTON_LEFT) {
                    float mx = event.button.x, my = event.button.y;
                    auto finish = [&](MenuAction a) -> MenuResult {
                        SDL_StopTextInput(window); cleanup();
                        return { a, tbSeed.getIntValue() };
                    };
                    if (btnStart.contains(mx, my))   return finish(MenuAction::START);
                    if (btnOptions.contains(mx, my)) return finish(MenuAction::OPTIONS);
                    if (btnExit.contains(mx, my))    return finish(MenuAction::EXIT);
                    tbSeed.focused = tbSeed.contains(mx, my);
                }
                else if (event.type == SDL_EVENT_KEY_DOWN) {
                    if (event.key.key == SDLK_ESCAPE) {
                        SDL_StopTextInput(window); cleanup();
                        return { MenuAction::EXIT, 0 };
                    }
                    if (event.key.key == SDLK_RETURN) tbSeed.focused = false;
                    tbSeed.handleKey(event.key.key, "");
                }
                else if (event.type == SDL_EVENT_TEXT_INPUT) {
                    tbSeed.handleTextInput(event.text.text);
                }
            }

            drawBackground();
            drawTitle();
            btnStart.render(renderer);
            tbSeed.render(renderer);
            btnOptions.render(renderer);
            btnExit.render(renderer);

            std::string hint = "Seed: " + (tbSeed.value.empty() ? "random (0)" : tbSeed.value);
            drawText(renderer, hint, 12, winH - 22, 1.0f, 80, 80, 80);

            presentFrame();
        }
    }
};
