#pragma once

#include <SDL3/SDL.h>
#include <iostream>
#include <vector>

#include "raycaster.cpp"

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
            // If no map is set, just draw black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            return;
        }

        int mapHeight = worldMap.size();
        int mapWidth = worldMap[0].size();

        // Draw grid based on worldMap
        for (int mapY = 0; mapY < mapHeight; mapY++) {
            for (int mapX = 0; mapX < mapWidth; mapX++) {
                int cellValue = worldMap[mapY][mapX];
                
                // Set color based on cell value
                if (cellValue > 0) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for walls
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black for empty
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

// Do this
// Move drawRays / drawPlayer into this
class PlayerView {
public:
    void drawPlayer(SDL_Renderer* renderer, float playerX, float playerY, float playerAngle) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        int playerPX = playerX * 9;
        int playerPY = playerY * 9;

        SDL_RenderPoint(renderer, playerX, playerY);

        SDL_FRect rect = { playerPX, playerPY, 20, 20 };
        SDL_RenderFillRect(renderer, &rect);
    }

    void drawRays(SDL_Renderer* renderer, std::vector<RayHit> rayResults) {
        for (const auto& ray : rayResults) {
            std::cout << "Distance: " << ray.distance << std::endl;
            std::cout << "Hit X: " << ray.hitX << std::endl;
            std::cout << "Hit Y: " << ray.hitY << std::endl;
            std::cout << "Wall Type: " << ray.wallType << std::endl;
            std::cout << "Hit Vertical Wall: " << (ray.hitVerticalWall ? "Yes" : "No") << std::endl;
            std::cout << "---" << std::endl;
            
            float angleRad = ray.angle * M_PI / 180.0f;  // Convert to radians
            
            int x = ray.hitX * 9;
            int y = ray.hitY * 9;

            float endX = x + (int)(ray.distance * cos(ray.angle));
            float endY = y + (int)(ray.distance * sin(ray.angle));

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderLine(renderer, ray.hitX, ray.hitY, endX, endY);

        }
    }
};

class MapWindow {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Grid grid;
    PlayerView playerView;
    int windowWidth;
    int windowHeight;

public:
    MapWindow(int width = 566, int height = 566)
        : window(nullptr), renderer(nullptr), running(false),
          windowWidth(width), windowHeight(height) {}

    ~MapWindow() {
        cleanup();
    }

    void setWorldMap(const std::vector<std::vector<int>>& map) {
        grid.setWorldMap(map);
    }

    bool initialize() {
        // Initialize SDL
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create window
        window = SDL_CreateWindow(
            "Grid Window",
            windowWidth, windowHeight,
            0
        );
        if (!window) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        return true;
    }

    bool isRunning() { return running; };

    void initRun() {
        running = true;

        renderFrame();
    }

    void update() {
        if (running == true)
        {
            handleEvents();
            renderFrame();
            SDL_Delay(16); // ~60 FPS
        }

    }

private:
    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }
    }

    void renderFrame() {
        // Clear screen with background color (dark gray)
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        // Draw the grid
        grid.render(renderer);
        
        //playerView.drawPlayer(renderer);  // How to pass player data to these
                                          // Probably should use the player Obj in game.cpp
        //playerView.drawRays(renderer);
        

        // Present
        SDL_RenderPresent(renderer);
    }

    void cleanup() {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
    }
};