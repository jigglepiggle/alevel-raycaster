#pragma once

// Map
constexpr int MAP_WIDTH  = 63;
constexpr int MAP_HEIGHT = 63;

// Player
constexpr float PLAYER_START_X      = 1.5f;
constexpr float PLAYER_START_Y      = 1.5f;
constexpr float PLAYER_START_ANGLE  = 0.0f;
constexpr float PLAYER_FOV          = 90.0f;
constexpr float PLAYER_MOVE_SPEED   = 0.2f;
constexpr float PLAYER_ROTATE_SPEED = 2.0f;

// Renderer
constexpr float SCREEN_WIDTH     = 1280.0f;
constexpr float SCREEN_HEIGHT    = 720.0f;
constexpr float RAY_MAX_DISTANCE = 20.0f;

// Maze generation
enum class MazeAlgorithm { DEPTH_FIRST, RECURSIVE_DIVISION };
constexpr MazeAlgorithm DEFAULT_MAZE_ALGORITHM = MazeAlgorithm::RECURSIVE_DIVISION;

// Debug
// constexpr bool DEBUG_SHOW_MAP  = true;
// constexpr bool DEBUG_SHOW_RAYS = false;
