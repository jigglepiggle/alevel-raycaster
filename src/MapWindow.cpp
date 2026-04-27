#include "MapWindow.h"
#include <iostream>

void SolutionPath::render(SDL_Renderer* renderer,
                          const std::vector<std::pair<int,int>>& path,
                          int cellSize) {
    if (path.empty()) return;
    SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
    for (const auto& [x, y] : path) {
        SDL_FRect rect = {
            static_cast<float>(x * cellSize + cellSize / 2 - 1),
            static_cast<float>(y * cellSize + cellSize / 2 - 1),
            3.0f, 3.0f
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}

void PlayerView::drawPlayer(SDL_Renderer* renderer, const Player& player) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FRect rect = {
        static_cast<float>(player.getX() * 9),
        static_cast<float>(player.getY() * 9),
        3.0f, 3.0f
    };
    SDL_RenderFillRect(renderer, &rect);
}

void PlayerView::drawRays(SDL_Renderer* renderer, const Player& player,
                          const std::vector<RayHit>& rayResults) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (const auto& ray : rayResults) {
        SDL_RenderLine(renderer,
            player.getX() * 9, player.getY() * 9,
            ray.hitX * 9,      ray.hitY * 9);
    }
}

void PlayerView::render(SDL_Renderer* renderer, const Player& player,
                        const std::vector<RayHit>& rayResults) {
    drawPlayer(renderer, player);
    drawRays(renderer, player, rayResults);
}

void TeleportCursor::render(SDL_Renderer* renderer, float mouseX, float mouseY, int cellSize) {
    int cellX = static_cast<int>(mouseX) / cellSize;
    int cellY = static_cast<int>(mouseY) / cellSize;
    SDL_SetRenderDrawColor(renderer, 0, 255, 100, 180);
    SDL_FRect rect = {
        static_cast<float>(cellX * cellSize),
        static_cast<float>(cellY * cellSize),
        static_cast<float>(cellSize),
        static_cast<float>(cellSize)
    };
    SDL_RenderFillRect(renderer, &rect);
}

Grid::Grid(int squareSize, int borderSize, int width, int height)
    : squareSize(squareSize), borderSize(borderSize), width(width), height(height) {
    cellSize = squareSize + borderSize;
}

void Grid::setWorldMap(const std::vector<std::vector<int>>& map) {
    worldMap = map;
}

void Grid::render(SDL_Renderer* renderer) {
    if (worldMap.empty()) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        return;
    }

    int mapHeight = static_cast<int>(worldMap.size());
    int mapWidth  = static_cast<int>(worldMap[0].size());

    for (int mapY = 0; mapY < mapHeight; mapY++) {
        for (int mapX = 0; mapX < mapWidth; mapX++) {
            uint8_t shade = worldMap[mapY][mapX] > 0 ? 255 : 0;
            SDL_SetRenderDrawColor(renderer, shade, shade, shade, 255);
            SDL_FRect rect = {
                static_cast<float>(mapX * cellSize),
                static_cast<float>(mapY * cellSize),
                static_cast<float>(squareSize),
                static_cast<float>(squareSize)
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

MapWindow::MapWindow(int width, int height) {
    winW = width;
    winH = height;
}

MapWindow::~MapWindow() { cleanup(); }

void MapWindow::cleanup() { BaseWindow::cleanup(); }

bool MapWindow::isRunning() { return running; }

bool MapWindow::init() { return initSDL("Grid Window", winW, winH); }

void MapWindow::initRun(WorldMap& map) {
    running  = true;
    worldMap = &map;
    grid.setWorldMap(map.getMap());
    map.solveMaze();
    solution = map.getSolutionPath();
    std::cout << "[Debug] F1 — toggle solution path | T — teleport to hovered cell" << std::endl;
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
                    debug.showSolutionPath = !debug.showSolutionPath;
                    std::cout << "[Debug] Solution path: "
                              << (debug.showSolutionPath ? "ON" : "OFF") << std::endl;
                    break;
                case SDLK_T: {
                    int cellX = static_cast<int>(mouseX) / CELL_SIZE;
                    int cellY = static_cast<int>(mouseY) / CELL_SIZE;
                    if (worldMap && !worldMap->isWall(cellX, cellY)) {
                        float newX = cellX + 0.5f;
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
            if (SDL_GetMouseFocus() == window) {
                mouseX = event.motion.x;
                mouseY = event.motion.y;
            }
        }
    }
}

void MapWindow::update(Player& player, const std::vector<RayHit>& rayResults) {
    handleEvents(player);
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
    grid.render(renderer);
    if (debug.showSolutionPath)
        solutionPathView.render(renderer, solution, CELL_SIZE);
    if (SDL_GetMouseFocus() == window)
        teleportCursor.render(renderer, mouseX, mouseY, CELL_SIZE);
    playerView.render(renderer, player, rayResults);
    presentFrame();
}