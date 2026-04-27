#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "BaseWindow.h"

void drawText(SDL_Renderer* renderer, const std::string& text,
              float x, float y, float scale,
              uint8_t r, uint8_t g, uint8_t b);

void drawTextCentred(SDL_Renderer* renderer, const std::string& text,
                     const Rect& bounds, float scale,
                     uint8_t r, uint8_t g, uint8_t b);

void drawTextCentredInWindow(SDL_Renderer* renderer, const std::string& text,
                             float winW, float y, float scale,
                             uint8_t r, uint8_t g, uint8_t b);

void drawRoundedRect(SDL_Renderer* renderer, const Rect& r, float radius,
                     uint8_t fillR, uint8_t fillG, uint8_t fillB, uint8_t fillA,
                     bool hovered = false);

void drawRectBorder(SDL_Renderer* renderer, const Rect& r,
                    uint8_t bR, uint8_t bG, uint8_t bB);

void drawButton(SDL_Renderer* renderer, const Rect& bounds,
                uint8_t r, uint8_t g, uint8_t b, bool hovered,
                const std::string& label);
