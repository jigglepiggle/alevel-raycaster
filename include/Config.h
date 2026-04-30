#pragma once

// Map
constexpr int MAP_WIDTH  = 63;
constexpr int MAP_HEIGHT = 63;

// Player
constexpr float PLAYER_START_X      = 1.5f;
constexpr float PLAYER_START_Y      = 1.5f;
constexpr float PLAYER_START_ANGLE  = 0.0f;
constexpr float PLAYER_FOV          = 90.0f;
constexpr float PLAYER_MOVE_SPEED   = 0.1f;
constexpr float PLAYER_ROTATE_SPEED = 1.2f;

// Renderer
constexpr float SCREEN_WIDTH     = 720.0f;
constexpr float SCREEN_HEIGHT    = 720.0f;
constexpr float RAY_MAX_DISTANCE = 48.0f;

// Maze generation
enum class MazeAlgorithm { DEPTH_FIRST, RECURSIVE_DIVISION };

struct GameOptions {
    int           seed      = 0;
    MazeAlgorithm algorithm = MazeAlgorithm::RECURSIVE_DIVISION;
    bool          showMap   = true;
    bool          showRays  = false;
};
