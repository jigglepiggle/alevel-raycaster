#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <utility>
#include "BaseWindow.h"
#include "RayHit.h"
#include "Player.h"
#include "WorldMap.h"

// Tracks which optional debug overlays are currently enabled.
struct DebugState {
    bool showSolutionPath = false;
    bool showRays         = false;
};

// Renders the BFS solution path as small dots on the minimap.    // Skills Audit Group A: Graph/Tree Traversal
class SolutionPath {
public:
    void render(SDL_Renderer* renderer,
                const std::vector<std::pair<int,int>>& path,
                int cellSize = 9);
};

// Renders the player's position and, optionally, their cast rays on the minimap.
class PlayerView {
private:
    void drawPlayer(SDL_Renderer* renderer, const Player& player);
    void drawRays(SDL_Renderer* renderer, const Player& player,
                  const std::vector<RayHit>& rayResults);
public:
    void render(SDL_Renderer* renderer, const Player& player,
                const std::vector<RayHit>& rayResults, bool showRays);
};

// Highlights the grid cell under the mouse cursor to indicate where a teleport would land.
class TeleportCursor {
public:
    void render(SDL_Renderer* renderer, float mouseX, float mouseY, int cellSize = 9);
};

// Draws the maze as a 2-D grid of coloured squares, one per map cell.    // Skills Audit Group A: Advanced matrix operations
class Grid {
private:
    int squareSize;
    int borderSize;
    int cellSize;    // squareSize + borderSize — computed in constructor.
    int width;
    int height;
    std::vector<std::vector<int>> worldMap;

public:
    Grid(int squareSize = 8, int borderSize = 1, int width = 566, int height = 566);
    // Stores a copy of the 2-D map data for rendering.
    void setWorldMap(const std::vector<std::vector<int>>& map);
    void render(SDL_Renderer* renderer);
};

// SDL window that shows the overhead minimap with optional debug overlays.    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance)
class MapWindow : public BaseWindow {
private:
    bool           running = false;
    Grid           grid;
    PlayerView     playerView;
    SolutionPath   solutionPathView;
    TeleportCursor teleportCursor;
    DebugState     debug;
    // Tracks mouse position for the teleport cursor and teleport action.
    float          mouseX    = 0.0f;
    float          mouseY    = 0.0f;
    const WorldMap* worldMap = nullptr;
    std::vector<std::pair<int,int>> solution;

    // Each map cell is rendered at this pixel size.
    static const int CELL_SIZE = 9;

    void cleanup();
    // Handles SDL events including keyboard shortcuts (F1, F2, T) and mouse movement.
    void handleEvents(Player& player);

public:
    MapWindow(int width = 566, int height = 566);
    ~MapWindow();

    bool isRunning();
    bool init();
    // Stores the world map reference, solves the maze and enables the chosen overlays.
    void initRun(WorldMap& map, bool showRays);
    // Processes events, redraws all layers and presents the frame.
    void update(Player& player, const std::vector<RayHit>& rayResults);
};
