#pragma once
#include <vector>
#include <cstdint>

struct Texture {
    int width;
    int height;
    std::vector<uint32_t> pixels;

    Texture(int w, int h) : width(w), height(h), pixels(w * h, 0xFF000000) {}

    uint32_t getPixel(int x, int y) const;
    void setPixel(int x, int y, uint32_t color);
};

Texture makeBrickTexture(int w = 64, int h = 64);
Texture makeStoneTexture(int w = 64, int h = 64);
