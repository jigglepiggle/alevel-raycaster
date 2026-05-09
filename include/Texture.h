#pragma once
#include <vector>
#include <cstdint>

// Stores a procedurally generated ARGB texture as a flat pixel array.
struct Texture {
    int width;
    int height;
    // Pixels stored in row-major order: pixel at (x, y) is at index y * width + x.
    std::vector<uint32_t> pixels;

    // Initialises all pixels to opaque black.
    Texture(int w, int h) : width(w), height(h), pixels(w * h, 0xFF000000) {}

    // Returns the ARGB pixel at (x, y), wrapping coordinates with modulo.
    uint32_t getPixel(int x, int y) const;
    // Writes an ARGB pixel at (x, y).
    void setPixel(int x, int y, uint32_t color);
};

// Generates a brick-pattern texture with randomised colour variation and mortar lines.
Texture makeBrickTexture(int w = 64, int h = 64);
// Generates a stone-block texture using a hash-based noise function.
Texture makeStoneTexture(int w = 64, int h = 64);
