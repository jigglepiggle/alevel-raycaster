#include "UIHelpers.h"
#include <algorithm>
#include <cmath>

void drawText(SDL_Renderer* renderer, const std::string& text,
              float x, float y, float scale,
              uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    float cx = x;
    for (char c : text) {
        SDL_RenderDebugTextFormat(renderer, cx, y, "%c", c);
        cx += 8.0f * scale;
    }
}

void drawTextCentred(SDL_Renderer* renderer, const std::string& text,
                     const Rect& bounds, float scale,
                     uint8_t r, uint8_t g, uint8_t b) {
    float tx = bounds.x + (bounds.w - text.size() * 8.0f * scale) / 2.0f;
    float ty = bounds.y + (bounds.h - 8.0f * scale) / 2.0f;
    drawText(renderer, text, tx, ty, scale, r, g, b);
}

void drawTextCentredInWindow(SDL_Renderer* renderer, const std::string& text,
                             float winW, float y, float scale,
                             uint8_t r, uint8_t g, uint8_t b) {
    float tx = (winW - text.size() * 8.0f * scale) / 2.0f;
    drawText(renderer, text, tx, y, scale, r, g, b);
}

void drawRoundedRect(SDL_Renderer* renderer, const Rect& r, float radius,
                     uint8_t fillR, uint8_t fillG, uint8_t fillB, uint8_t fillA,
                     bool hovered) {
    if (hovered) {
        fillR = static_cast<uint8_t>(std::min(255, (int)fillR + 40));
        fillG = static_cast<uint8_t>(std::min(255, (int)fillG + 40));
        fillB = static_cast<uint8_t>(std::min(255, (int)fillB + 40));
    }

    SDL_SetRenderDrawColor(renderer, fillR, fillG, fillB, fillA);

    SDL_FRect centre = { r.x + radius, r.y,                r.w - radius * 2, r.h              };
    SDL_FRect left   = { r.x,          r.y + radius,       radius,           r.h - radius * 2 };
    SDL_FRect right  = { r.x + r.w - radius, r.y + radius, radius,           r.h - radius * 2 };
    SDL_RenderFillRect(renderer, &centre);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);

    int steps = static_cast<int>(radius);
    for (int i = 0; i < steps; i++) {
        float angle = (float)i       / steps * (3.14159f / 2.0f);
        float nextA = (float)(i + 1) / steps * (3.14159f / 2.0f);
        float x0 = cos(angle) * radius, x1 = cos(nextA) * radius;
        float y0 = sin(angle) * radius, y1 = sin(nextA) * radius;

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

void drawRectBorder(SDL_Renderer* renderer, const Rect& r,
                    uint8_t bR, uint8_t bG, uint8_t bB) {
    SDL_SetRenderDrawColor(renderer, bR, bG, bB, 255);
    SDL_FRect sides[4] = {
        { r.x,           r.y,               r.w, 1    },
        { r.x,           r.y + r.h - 1,     r.w, 1    },
        { r.x,           r.y,               1,   r.h  },
        { r.x + r.w - 1, r.y,               1,   r.h  },
    };
    for (auto& s : sides) SDL_RenderFillRect(renderer, &s);
}

void drawButton(SDL_Renderer* renderer, const Rect& bounds,
                uint8_t r, uint8_t g, uint8_t b, bool hovered,
                const std::string& label) {
    drawRoundedRect(renderer, bounds, 8.0f, r, g, b, 255, hovered);
    drawRectBorder(renderer, bounds, 200, 200, 200);
    drawTextCentred(renderer, label, bounds, 2.0f, 255, 255, 255);
}
