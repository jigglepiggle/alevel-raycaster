#pragma once

// Global compile-time constants for map dimensions, player settings and renderer settings.

// Maze grid dimensions in tiles.
constexpr int MAP_WIDTH  = 63;
constexpr int MAP_HEIGHT = 63;

// Player starting position and orientation.
constexpr float PLAYER_START_X      = 1.5f;
constexpr float PLAYER_START_Y      = 1.5f;
constexpr float PLAYER_START_ANGLE  = 0.0f;   // Radians; 0 = facing right.
constexpr float PLAYER_FOV          = 90.0f;   // Field of view in degrees.
constexpr float PLAYER_MOVE_SPEED   = 0.1f;    // Tiles moved per frame.
constexpr float PLAYER_ROTATE_SPEED = 1.2f;    // Rotation multiplier per frame.

// Renderer output resolution and maximum ray travel distance.
constexpr float SCREEN_WIDTH     = 1280.0f;
constexpr float SCREEN_HEIGHT    = 720.0f;
constexpr float RAY_MAX_DISTANCE = 48.0f;      // Rays stop after this many tiles.

// Available maze generation algorithms selectable from the options menu.
enum class MazeAlgorithm { DEPTH_FIRST, RECURSIVE_DIVISION };

// Holds all user-configurable game settings passed from the menu to the game loop.
struct GameOptions {
    int           seed      = 0;                                   // 0 means use a random seed.
    MazeAlgorithm algorithm = MazeAlgorithm::RECURSIVE_DIVISION;
    bool          showMap   = true;
    bool          showRays  = false;
};
