#pragma once

// Simple RGBA colour value stored as four unsigned bytes.
struct Color {
    unsigned char r, g, b, a;
    // Constructor with default values of zero (transparent black).
    Color(unsigned char red = 0,
          unsigned char green = 0,
          unsigned char blue = 0,
          unsigned char alpha = 0);
};
