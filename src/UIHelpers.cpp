#include "UIHelpers.h"
#include <algorithm>
#include <cmath>

// Draws each character of the string individually, advancing cx by 8 * scale pixels per glyph.
void drawText(SDL_Renderer* renderer, const std::string& text,
              float x, float y, float scale,
              uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    float cx = x;
    for (char c : text) {
        SDL_RenderDebugTextFormat(renderer, cx, y, "%c", c);
        cx += 8.0f * scale;    // Skills Audit Group B: Simple scientific/mathematical model (advance cursor by scaled glyph width)
    }
}

// Computes a top-left position that centres the string horizontally and vertically in bounds.
void drawTextCentred(SDL_Renderer* renderer, const std::string& text,
                     const Rect& bounds, float scale,
                     uint8_t r, uint8_t g, uint8_t b) {
    float tx = bounds.x + (bounds.w - text.size() * 8.0f * scale) / 2.0f;    // Skills Audit Group B: Simple scientific/mathematical model (horizontal centring calculation)
    float ty = bounds.y + (bounds.h - 8.0f * scale) / 2.0f;                  // Skills Audit Group B: Simple scientific/mathematical model (vertical centring calculation)
    drawText(renderer, text, tx, ty, scale, r, g, b);
}

// Centres the string across the full window width using winW to compute the X offset.
void drawTextCentredInWindow(SDL_Renderer* renderer, const std::string& text,
                             float winW, float y, float scale,
                             uint8_t r, uint8_t g, uint8_t b) {
    float tx = (winW - text.size() * 8.0f * scale) / 2.0f;    // Skills Audit Group B: Simple scientific/mathematical model (window-width centring calculation)
    drawText(renderer, text, tx, y, scale, r, g, b);
}

// Draws a filled rectangle with rounded corners approximated by a set of overlapping rectangles.
void drawRoundedRect(SDL_Renderer* renderer, const Rect& r, float radius,
                     uint8_t fillR, uint8_t fillG, uint8_t fillB, uint8_t fillA,
                     bool hovered) {
    // Brighten the fill colour by 40 when the mouse is hovering to give visual feedback.
    if (hovered) {
        fillR = static_cast<uint8_t>(std::min(255, (int)fillR + 40));    // Skills Audit Group B: Simple user defined algorithms (clamped brightness increment for hover state)
        fillG = static_cast<uint8_t>(std::min(255, (int)fillG + 40));
        fillB = static_cast<uint8_t>(std::min(255, (int)fillB + 40));
    }

    SDL_SetRenderDrawColor(renderer, fillR, fillG, fillB, fillA);

    // Three overlapping rectangles cover the straight sections of the rounded rect.
    SDL_FRect centre = { r.x + radius, r.y,                r.w - radius * 2, r.h              };    // Skills Audit Group B: Records (SDL_FRect structs for each section of the rounded rectangle)
    SDL_FRect left   = { r.x,          r.y + radius,       radius,           r.h - radius * 2 };
    SDL_FRect right  = { r.x + r.w - radius, r.y + radius, radius,           r.h - radius * 2 };
    SDL_RenderFillRect(renderer, &centre);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);

    // Approximate the four corner arcs using small rectangles sampled along a quarter circle.    // Skills Audit Group B: Simple scientific/mathematical model (quarter-circle arc approximation via cosine/sine)
    int steps = static_cast<int>(radius);
    for (int i = 0; i < steps; i++) {
        float angle = (float)i       / steps * (3.14159f / 2.0f);    // Skills Audit Group B: Simple scientific/mathematical model (angle step through quarter circle)
        float nextA = (float)(i + 1) / steps * (3.14159f / 2.0f);
        float x0 = cos(angle) * radius, x1 = cos(nextA) * radius;    // Skills Audit Group B: Simple scientific/mathematical model (cosine x-coordinates on arc)
        float y0 = sin(angle) * radius, y1 = sin(nextA) * radius;    // Skills Audit Group B: Simple scientific/mathematical model (sine y-coordinates on arc)

        // Fill one small rectangle per arc segment at each of the four corners.
        SDL_FRect tl = { r.x + radius - x0,       r.y + radius - y0,       x0 - x1 + 1, y1 - y0 + 1 };    // Skills Audit Group B: Records (SDL_FRect per corner arc segment)
        SDL_FRect tr = { r.x + r.w - radius + x1, r.y + radius - y0,       x0 - x1 + 1, y1 - y0 + 1 };
        SDL_FRect bl = { r.x + radius - x0,       r.y + r.h - radius + y0, x0 - x1 + 1, y1 - y0 + 1 };
        SDL_FRect br = { r.x + r.w - radius + x1, r.y + r.h - radius + y0, x0 - x1 + 1, y1 - y0 + 1 };
        SDL_RenderFillRect(renderer, &tl);
        SDL_RenderFillRect(renderer, &tr);
        SDL_RenderFillRect(renderer, &bl);
        SDL_RenderFillRect(renderer, &br);
    }
}

// Draws four 1-pixel rectangles forming the top, bottom, left and right edges of the border.
void drawRectBorder(SDL_Renderer* renderer, const Rect& r,
                    uint8_t bR, uint8_t bG, uint8_t bB) {
    SDL_SetRenderDrawColor(renderer, bR, bG, bB, 255);
    SDL_FRect sides[4] = {    // Skills Audit Group B: Records (SDL_FRect array for the four border edges)
        { r.x,           r.y,               r.w, 1    },
        { r.x,           r.y + r.h - 1,     r.w, 1    },
        { r.x,           r.y,               1,   r.h  },
        { r.x + r.w - 1, r.y,               1,   r.h  },
    };
    for (auto& s : sides) SDL_RenderFillRect(renderer, &s);
}

// Combines drawRoundedRect, drawRectBorder and drawTextCentred into a single button draw call.
void drawButton(SDL_Renderer* renderer, const Rect& bounds,
                uint8_t r, uint8_t g, uint8_t b, bool hovered,
                const std::string& label) {
    drawRoundedRect(renderer, bounds, 8.0f, r, g, b, 255, hovered);
    drawRectBorder(renderer, bounds, 200, 200, 200);
    drawTextCentred(renderer, label, bounds, 2.0f, 255, 255, 255);    // Skills Audit Group B: Simple user defined algorithms (composing rounded rect, border and centred text into one button)
}
