#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "BaseWindow.h"

// Draws text at world position (x, y) using SDL's built-in debug font, scaled by 'scale'.
void drawText(SDL_Renderer* renderer, const std::string& text,
              float x, float y, float scale,
              uint8_t r, uint8_t g, uint8_t b);

// Draws text centred horizontally and vertically inside the given bounds rectangle.
void drawTextCentred(SDL_Renderer* renderer, const std::string& text,
                     const Rect& bounds, float scale,
                     uint8_t r, uint8_t g, uint8_t b);

// Draws text centred horizontally across the full window width at the given Y position.
void drawTextCentredInWindow(SDL_Renderer* renderer, const std::string& text,
                             float winW, float y, float scale,
                             uint8_t r, uint8_t g, uint8_t b);

// Fills a rectangle with rounded corners approximated by multiple rects and arc segments.
// If hovered is true, the fill colour is brightened to give visual feedback.
void drawRoundedRect(SDL_Renderer* renderer, const Rect& r, float radius,
                     uint8_t fillR, uint8_t fillG, uint8_t fillB, uint8_t fillA,
                     bool hovered = false);

// Draws a 1-pixel border around the rectangle in the given colour.
void drawRectBorder(SDL_Renderer* renderer, const Rect& r,
                    uint8_t bR, uint8_t bG, uint8_t bB);

// Convenience function: draws a rounded, bordered button with a centred white label.
void drawButton(SDL_Renderer* renderer, const Rect& bounds,
                uint8_t r, uint8_t g, uint8_t b, bool hovered,
                const std::string& label);
