#pragma once

// Stores the result of casting a single ray into the world.    // Skills Audit Group A: Complex scientific/mathematical/robotics/control/business model
struct RayHit {
  float distance;        // Perpendicular (fisheye-corrected) distance to the wall.
  float hitX;            // World-space X coordinate of the hit point.
  float hitY;            // World-space Y coordinate of the hit point.
  float wallU;           // Horizontal texture coordinate in [0, 1] along the hit wall face.
  int   wallType;        // Tile value at the hit cell (e.g. 1 = brick, 2 = stone).
  bool  hitVerticalWall; // True if the ray hit a vertical (N/S) wall face.
  float angle;           // Direction the ray was cast, in radians.
};
