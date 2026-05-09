#include "MapWindow.h"
#include <iostream>

// Renders each cell of the BFS solution path as a small 3x3 yellow dot.    // Skills Audit Group A: Graph/Tree Traversal
void SolutionPath::render(SDL_Renderer* renderer,
                          const std::vector<std::pair<int,int>>& path,
                          int cellSize) {
    if (path.empty()) return;
    SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
    for (const auto& [x, y] : path) {
        // Centre the dot within the cell by offsetting by half the cell size.
        SDL_FRect rect = {    // Skills Audit Group B: Records (SDL_FRect struct describing dot position and size)
            static_cast<float>(x * cellSize + cellSize / 2 - 1),    // Skills Audit Group B: Simple scientific/mathematical model (cell-to-pixel coordinate conversion)
            static_cast<float>(y * cellSize + cellSize / 2 - 1),
            3.0f, 3.0f
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}

// Draws a small red square at the player's current world-space tile position.
void PlayerView::drawPlayer(SDL_Renderer* renderer, const Player& player) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // Multiply by 9 (cell size) to convert from tile coordinates to screen pixels.
    SDL_FRect rect = {    // Skills Audit Group B: Records (SDL_FRect struct describing player marker)
        static_cast<float>(player.getX() * 9),    // Skills Audit Group B: Simple scientific/mathematical model (world-to-screen coordinate scaling)
        static_cast<float>(player.getY() * 9),
        3.0f, 3.0f
    };
    SDL_RenderFillRect(renderer, &rect);
}

// Draws a line from the player's position to each ray's hit point for debugging.
void PlayerView::drawRays(SDL_Renderer* renderer, const Player& player,
                          const std::vector<RayHit>& rayResults) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (const auto& ray : rayResults) {
        SDL_RenderLine(renderer,
            player.getX() * 9, player.getY() * 9,    // Skills Audit Group B: Simple scientific/mathematical model (world-to-screen scaling for ray lines)
            ray.hitX * 9,      ray.hitY * 9);
    }
}

// Renders the player marker and, if enabled, the cast-ray debug lines.
void PlayerView::render(SDL_Renderer* renderer, const Player& player,
                        const std::vector<RayHit>& rayResults, bool showRays) {
    drawPlayer(renderer, player);
    if (showRays)
        drawRays(renderer, player, rayResults);
}

// Highlights the grid cell under the mouse in semi-transparent green to show the teleport target.
void TeleportCursor::render(SDL_Renderer* renderer, float mouseX, float mouseY, int cellSize) {
    // Integer-divide to snap the mouse position to the nearest cell origin.
    int cellX = static_cast<int>(mouseX) / cellSize;    // Skills Audit Group B: Simple user defined algorithms (pixel-to-cell coordinate snapping)
    int cellY = static_cast<int>(mouseY) / cellSize;
    SDL_SetRenderDrawColor(renderer, 0, 255, 100, 180);
    SDL_FRect rect = {    // Skills Audit Group B: Records (SDL_FRect struct describing the cursor highlight area)
        static_cast<float>(cellX * cellSize),
        static_cast<float>(cellY * cellSize),
        static_cast<float>(cellSize),
        static_cast<float>(cellSize)
    };
    SDL_RenderFillRect(renderer, &rect);
}

// Stores grid dimensions and computes cellSize from squareSize + borderSize.
Grid::Grid(int squareSize, int borderSize, int width, int height)
    : squareSize(squareSize), borderSize(borderSize), width(width), height(height) {
    cellSize = squareSize + borderSize;    // Skills Audit Group B: Simple user defined algorithms (cell pixel size including border)
}

void Grid::setWorldMap(const std::vector<std::vector<int>>& map) {
    worldMap = map;    // Skills Audit Group B: Multi-dimensional arrays (storing a 2-D map grid)
}

// Iterates the 2-D map array and draws each cell as a white (wall) or black (passage) square.    // Skills Audit Group A: Advanced matrix operations
void Grid::render(SDL_Renderer* renderer) {
    if (worldMap.empty()) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        return;
    }

    int mapHeight = static_cast<int>(worldMap.size());
    int mapWidth  = static_cast<int>(worldMap[0].size());

    for (int mapY = 0; mapY < mapHeight; mapY++) {
        for (int mapX = 0; mapX < mapWidth; mapX++) {    // Skills Audit Group B: Multi-dimensional arrays (nested loop over 2-D grid rows and columns)
            // Non-zero cells (walls) are drawn white; passage cells are black.
            uint8_t shade = worldMap[mapY][mapX] > 0 ? 255 : 0;    // Skills Audit Group B: Simple user defined algorithms (binary cell shade selection)
            SDL_SetRenderDrawColor(renderer, shade, shade, shade, 255);
            SDL_FRect rect = {    // Skills Audit Group B: Records (SDL_FRect struct per cell)
                static_cast<float>(mapX * cellSize),    // Skills Audit Group B: Simple scientific/mathematical model (grid index to screen pixel)
                static_cast<float>(mapY * cellSize),
                static_cast<float>(squareSize),
                static_cast<float>(squareSize)
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

MapWindow::MapWindow(int width, int height) {    // Skills Audit Group B: Generation of objects based on simple OOP model
    winW = width;
    winH = height;
}

MapWindow::~MapWindow() { cleanup(); }

void MapWindow::cleanup() { BaseWindow::cleanup(); }    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance — delegating to base class)

bool MapWindow::isRunning() { return running; }

bool MapWindow::init() { return initSDL("Grid Window", winW, winH); }

// Sets up the grid data, solves the maze via BFS, and prints available debug shortcuts.
void MapWindow::initRun(WorldMap& map, bool showRays) {
    running          = true;
    worldMap         = &map;
    debug.showRays   = showRays;    // Skills Audit Group B: Records (DebugState struct storing overlay toggle flags)
    grid.setWorldMap(map.getMap());
    // Solve and cache the BFS path so it can be toggled on/off at runtime.
    map.solveMaze();    // Skills Audit Group A: Graph/Tree Traversal (BFS shortest path)
    solution = map.getSolutionPath();
    std::cout << "[Debug] F1 — toggle solution path | F2 — toggle rays | T — teleport to hovered cell" << std::endl;
}

void MapWindow::handleEvents(Player& player) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_F1:
                    // Toggle the BFS solution path overlay.
                    debug.showSolutionPath = !debug.showSolutionPath;    // Skills Audit Group B: Simple user defined algorithms (boolean toggle)
                    std::cout << "[Debug] Solution path: "
                              << (debug.showSolutionPath ? "ON" : "OFF") << std::endl;
                    break;
                case SDLK_F2:
                    // Toggle the ray debug overlay.
                    debug.showRays = !debug.showRays;    // Skills Audit Group B: Simple user defined algorithms (boolean toggle)
                    std::cout << "[Debug] Rays: "
                              << (debug.showRays ? "ON" : "OFF") << std::endl;
                    break;
                case SDLK_T: {
                    // Convert mouse pixel position to tile coordinates and teleport if passable.
                    int cellX = static_cast<int>(mouseX) / CELL_SIZE;    // Skills Audit Group B: Simple scientific/mathematical model (pixel-to-tile coordinate conversion)
                    int cellY = static_cast<int>(mouseY) / CELL_SIZE;
                    if (worldMap && !worldMap->isWall(cellX, cellY)) {
                        float newX = cellX + 0.5f;    // Skills Audit Group B: Simple scientific/mathematical model (offset to tile centre)
                        float newY = cellY + 0.5f;
                        player.setPosition(newX, newY);
                        std::cout << "[Debug] Teleported to cell ("
                                  << cellX << ", " << cellY << ")"
                                  << " -> world (" << newX << ", " << newY << ")" << std::endl;
                    } else {
                        std::cout << "[Debug] Teleport blocked — target cell is a wall." << std::endl;
                    }
                    break;
                }
                default: break;
            }
        } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
            // Only track the mouse position when this window has focus.
            if (SDL_GetMouseFocus() == window) {
                mouseX = event.motion.x;    // Skills Audit Group B: Records (storing mouse state as member fields)
                mouseY = event.motion.y;
            }
        }
    }
}

void MapWindow::update(Player& player, const std::vector<RayHit>& rayResults) {
    handleEvents(player);
    // Clear to a dark grey background before drawing overlays.
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
    grid.render(renderer);
    // Draw optional overlays on top of the grid in order: solution path, cursor, player.
    if (debug.showSolutionPath)
        solutionPathView.render(renderer, solution, CELL_SIZE);
    if (SDL_GetMouseFocus() == window)
        teleportCursor.render(renderer, mouseX, mouseY, CELL_SIZE);
    playerView.render(renderer, player, rayResults, debug.showRays);
    presentFrame(0);
}
