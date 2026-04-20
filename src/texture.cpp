#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>

// Pixels stored as 0xAARRGGBB
struct Texture {
    int width;
    int height;
    std::vector<uint32_t> pixels;

    Texture(int w, int h) : width(w), height(h), pixels(w * h, 0xFF000000) {}

    uint32_t getPixel(int x, int y) const {
        x = x % width;
        y = y % height;
        return pixels[y * width + x];
    }

    void setPixel(int x, int y, uint32_t color) {
        pixels[y * width + x] = color;
    }
};

// Brick wall texture
inline Texture makeBrickTexture(int w = 64, int h = 64) {
    Texture tex(w, h);
    const int rowHeight  = h / 4;   // 4 brick rows
    const int brickWidth = w / 2;   // 2 bricks per row

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int row    = y / rowHeight;
            int offset = (row % 2) * (brickWidth / 2); // stagger alternate rows
            bool isMortar = (y % rowHeight < 2) || ((x + offset) % brickWidth < 2);

            // Add subtle variation to brick colour based on position
            uint8_t brickR = static_cast<uint8_t>(std::clamp(176 + ((x * 3 + y * 7) % 30) - 15, 0, 255));
            uint8_t brickG = static_cast<uint8_t>(std::clamp(64  + ((x * 5 + y * 3) % 20) - 10, 0, 255));
            uint8_t brickB = static_cast<uint8_t>(std::clamp(32  + ((x * 7 + y * 5) % 16) - 8,  0, 255));

            tex.setPixel(x, y, isMortar
                ? 0xFF808080                                          // grey mortar
                : (0xFF000000 | (brickR << 16) | (brickG << 8) | brickB)); // varied brick
        }
    }
    return tex;
}

// Simple stone/checker texture (useful for a second wall type)
inline Texture makeStoneTexture(int w = 64, int h = 64) {
    Texture tex(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Noisy grey stone look via cheap hash
            int noise = ((x * 1664525 + y * 1013904223) >> 16) & 0x1F;
            uint8_t base = static_cast<uint8_t>(120 + noise);

            // Add block grid lines
            bool isGrout = (x % 16 < 2) || (y % 16 < 2);
            if (isGrout) base = 80;

            tex.setPixel(x, y, 0xFF000000 | (base << 16) | (base << 8) | base);
        }
    }
    return tex;
}
