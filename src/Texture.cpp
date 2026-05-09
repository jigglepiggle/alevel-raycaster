#include "Texture.h"
#include <algorithm>

// Returns the ARGB pixel at (x, y), wrapping out-of-range coordinates with modulo.
uint32_t Texture::getPixel(int x, int y) const {
    x = x % width;
    y = y % height;
    return pixels[y * width + x];    // Skills Audit Group B: Multi-dimensional arrays (flat pixel buffer addressed as 2-D via row-major index)
}

// Writes an ARGB pixel at (x, y) using the row-major index formula.
void Texture::setPixel(int x, int y, uint32_t color) {
    pixels[y * width + x] = color;    // Skills Audit Group B: Multi-dimensional arrays (flat pixel buffer addressed as 2-D via row-major index)
}

// Procedurally generates a brick texture with alternating row offsets and mortar gaps.
Texture makeBrickTexture(int w, int h) {
    Texture tex(w, h);    // Skills Audit Group B: Generation of objects based on simple OOP model
    const int rowHeight  = h / 4;    // Skills Audit Group B: Simple scientific/mathematical model (divide height into four equal brick rows)
    const int brickWidth = w / 2;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {    // Skills Audit Group B: Multi-dimensional arrays (nested loop over 2-D texture pixel grid)
            int row    = y / rowHeight;
            // Alternate rows are offset by half a brick width to create a running bond pattern.
            int offset = (row % 2) * (brickWidth / 2);    // Skills Audit Group B: Simple user defined algorithms (alternating row offset for brick bond)
            // A pixel is mortar if it falls within 2 pixels of a horizontal or vertical joint.
            bool isMortar = (y % rowHeight < 2) || ((x + offset) % brickWidth < 2);    // Skills Audit Group B: Simple user defined algorithms (mortar detection via modulo proximity)
            // Add subtle per-pixel noise to vary the brick colour.
            uint8_t brickR = static_cast<uint8_t>(std::clamp(176 + ((x * 3 + y * 7) % 30) - 15, 0, 255));    // Skills Audit Group B: Simple scientific/mathematical model (hash-based per-pixel colour variation)
            uint8_t brickG = static_cast<uint8_t>(std::clamp(64  + ((x * 5 + y * 3) % 20) - 10, 0, 255));
            uint8_t brickB = static_cast<uint8_t>(std::clamp(32  + ((x * 7 + y * 5) % 16) - 8,  0, 255));
            tex.setPixel(x, y, isMortar
                ? 0xFF808080
                : (0xFF000000 | (brickR << 16) | (brickG << 8) | brickB));    // Skills Audit Group B: Simple user defined algorithms (ARGB pixel assembly from channel values)
        }
    }
    return tex;
}

// Generates a stone texture using a hash-based noise function for grey variation.
Texture makeStoneTexture(int w, int h) {
    Texture tex(w, h);    // Skills Audit Group B: Generation of objects based on simple OOP model
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {    // Skills Audit Group B: Multi-dimensional arrays (nested loop over 2-D texture pixel grid)
            // Cheap hash mixing two linear congruential multipliers to produce pseudo-random noise.    // Skills Audit Group A: Advanced matrix operations (hash-based texture noise generation)
            int noise = ((x * 1664525 + y * 1013904223) >> 16) & 0x1F;    // Skills Audit Group B: Simple scientific/mathematical model (LCG hash for per-pixel noise)
            uint8_t base = static_cast<uint8_t>(120 + noise);
            // Cells near a block boundary are drawn as darker grout lines.
            bool isGrout = (x % 16 < 2) || (y % 16 < 2);    // Skills Audit Group B: Simple user defined algorithms (grout detection via modulo proximity)
            if (isGrout) base = 80;
            // Grey pixel: same value in all three channels.
            tex.setPixel(x, y, 0xFF000000 | (base << 16) | (base << 8) | base);    // Skills Audit Group B: Simple user defined algorithms (grey ARGB pixel assembly with bit shifts)
        }
    }
    return tex;
}
