#include "Texture.h"
#include <algorithm>

uint32_t Texture::getPixel(int x, int y) const {
    x = x % width;
    y = y % height;
    return pixels[y * width + x];
}

void Texture::setPixel(int x, int y, uint32_t color) {
    pixels[y * width + x] = color;
}

Texture makeBrickTexture(int w, int h) {
    Texture tex(w, h);
    const int rowHeight  = h / 4;
    const int brickWidth = w / 2;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int row    = y / rowHeight;
            int offset = (row % 2) * (brickWidth / 2);
            bool isMortar = (y % rowHeight < 2) || ((x + offset) % brickWidth < 2);
            uint8_t brickR = static_cast<uint8_t>(std::clamp(176 + ((x * 3 + y * 7) % 30) - 15, 0, 255));
            uint8_t brickG = static_cast<uint8_t>(std::clamp(64  + ((x * 5 + y * 3) % 20) - 10, 0, 255));
            uint8_t brickB = static_cast<uint8_t>(std::clamp(32  + ((x * 7 + y * 5) % 16) - 8,  0, 255));
            tex.setPixel(x, y, isMortar
                ? 0xFF808080
                : (0xFF000000 | (brickR << 16) | (brickG << 8) | brickB));
        }
    }
    return tex;
}

Texture makeStoneTexture(int w, int h) {
    Texture tex(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int noise = ((x * 1664525 + y * 1013904223) >> 16) & 0x1F;
            uint8_t base = static_cast<uint8_t>(120 + noise);
            bool isGrout = (x % 16 < 2) || (y % 16 < 2);
            if (isGrout) base = 80;
            tex.setPixel(x, y, 0xFF000000 | (base << 16) | (base << 8) | base);
        }
    }
    return tex;
}
