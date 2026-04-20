#include <SDL3/SDL.h>
#include <iostream>
#include <vector>

#include "raycaster.cpp"
#include "game.cpp"


// ─── Debug state ─────────────────────────────────────────────────────────────
struct DebugState {
    bool showSolutionPath = true;   // Toggle with F1
};


// ─── SolutionPath ─────────────────────────────────────────────────────────────
class SolutionPath {
public:
    void render(SDL_Renderer* renderer,
                const std::vector<std::pair<int,int>>& path,
                int cellSize = 9) {
        if (path.empty()) return;

        SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255); // Yellow

        for (const auto& [x, y] : path) {
            SDL_FRect rect = {
                static_cast<float>(x * cellSize + cellSize / 2 - 1),
                static_cast<float>(y * cellSize + cellSize / 2 - 1),
                3.0f, 3.0f
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};


// ─── PlayerView ───────────────────────────────────────────────────────────────
class PlayerView {
private:
    void drawPlayer(SDL_Renderer* renderer, const Player& player) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        int playerPX = static_cast<int>(player.getX() * 9);
        int playerPY = static_cast<int>(player.getY() * 9);

        SDL_FRect rect = { (float)playerPX, (float)playerPY, 3, 3 };
        SDL_RenderFillRect(renderer, &rect);
    }

    void drawRays(SDL_Renderer* renderer, const Player& player, const std::vector<RayHit>& rayResults) {
        for (const auto& ray : rayResults) {
            float startX = player.getX() * 9;
            float startY = player.getY() * 9;
            float endX   = ray.hitX * 9;
            float endY   = ray.hitY * 9;

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderLine(renderer, startX, startY, endX, endY);
        }
    }

public:
    void render(SDL_Renderer* renderer, const Player& player, const std::vector<RayHit>& rayResults) {
        drawPlayer(renderer, player);
        drawRays(renderer, player, rayResults);
    }
};


// ─── TeleportCursor ───────────────────────────────────────────────────────────
// Draws a highlight on whichever map cell the mouse is hovering over.
class TeleportCursor {
public:
    void render(SDL_Renderer* renderer, float mouseX, float mouseY, int cellSize = 9) {
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
};


// ─── Grid ─────────────────────────────────────────────────────────────────────
class Grid {
private:
    int squareSize;
    int borderSize;
    int cellSize;
    int width;
    int height;
    std::vector<std::vector<int>> worldMap;

public:
    Grid(int squareSize = 8, int borderSize = 1, int width = 566, int height = 566)
        : squareSize(squareSize), borderSize(borderSize), width(width), height(height) {
        cellSize = squareSize + borderSize;
    }

    void setWorldMap(const std::vector<std::vector<int>>& map) {
        worldMap = map;
    }

    void render(SDL_Renderer* renderer) {
        if (worldMap.empty()) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            return;
        }

        int mapHeight = static_cast<int>(worldMap.size());
        int mapWidth  = static_cast<int>(worldMap[0].size());

        for (int mapY = 0; mapY < mapHeight; mapY++) {
            for (int mapX = 0; mapX < mapWidth; mapX++) {
                if (worldMap[mapY][mapX] > 0) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                }

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
};


// ─── MapWindow ────────────────────────────────────────────────────────────────
class MapWindow {
private:
    SDL_Window*   window;
    SDL_Renderer* renderer;
    bool          running;

    Grid           grid;
    PlayerView     playerView;
    SolutionPath   solutionPathView;
    TeleportCursor teleportCursor;

    std::vector<std::pair<int,int>> solution;

    int windowWidth;
    int windowHeight;

    // Debug state
    DebugState debug;

    // Mouse position tracked inside this window
    float mouseX = 0.0f;
    float mouseY = 0.0f;

    // Reference to the world map for wall checks during teleport
    const WorldMap* worldMap = nullptr;

    static const int CELL_SIZE = 9;

    void cleanup() {
        if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
        if (window)   { SDL_DestroyWindow(window);     window   = nullptr; }
        SDL_Quit();
    }

    void handleEvents(Player& player) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {

                    case SDLK_ESCAPE:
                        running = false;
                        break;

                    // ── Debug: toggle solution path overlay ──
                    case SDLK_F1:
                        debug.showSolutionPath = !debug.showSolutionPath;
                        std::cout << "[Debug] Solution path overlay: "
                                  << (debug.showSolutionPath ? "ON" : "OFF") << std::endl;
                        break;

                    // ── Debug: teleport player to cursor cell ──
                    case SDLK_T: {
                        // Only act if the mouse is inside this window
                        SDL_WindowID hoveredID = SDL_GetWindowID(
                            SDL_GetWindowFromEvent(&event) ? SDL_GetWindowFromEvent(&event) : window);

                        int cellX = static_cast<int>(mouseX) / CELL_SIZE;
                        int cellY = static_cast<int>(mouseY) / CELL_SIZE;

                        if (worldMap && !worldMap->isWall(cellX, cellY)) {
                            // Place player in the centre of the cell
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
            }

            // Track mouse position relative to this window
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (SDL_GetMouseFocus() == window) {
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;
                }
            }
        }
    }

public:
    MapWindow(int width = 566, int height = 566)
        : window(nullptr), renderer(nullptr),
          running(false), windowWidth(width), windowHeight(height) {}

    ~MapWindow() { cleanup(); }

    bool isRunning() { return running; }

    bool init() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("Grid Window", windowWidth, windowHeight, 0);
        if (!window) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        return true;
    }

    void initRun(WorldMap& map) {
        running   = true;
        worldMap  = &map;

        grid.setWorldMap(map.getMap());

        map.solveMaze();
        solution = map.getSolutionPath();

        std::cout << "[Debug] F1 — toggle solution path overlay" << std::endl;
        std::cout << "[Debug] T  — teleport to hovered map cell" << std::endl;
    }

    void update(Player& player, const std::vector<RayHit>& rayResults) {
        handleEvents(player);

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        grid.render(renderer);

        // ── Debug overlays ──────────────────────────────────────────────────
        if (debug.showSolutionPath) {
            solutionPathView.render(renderer, solution, CELL_SIZE);
        }

        // Show cursor highlight when mouse is inside the window
        if (SDL_GetMouseFocus() == window) {
            teleportCursor.render(renderer, mouseX, mouseY, CELL_SIZE);
        }
        // ────────────────────────────────────────────────────────────────────

        playerView.render(renderer, player, rayResults);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
};
