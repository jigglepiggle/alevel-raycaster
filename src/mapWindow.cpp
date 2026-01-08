#include <SDL3/SDL.h>
#include <iostream>
#include <vector>

#include "raycaster.cpp"
#include "game.cpp"


class PlayerView {
private:
    void drawPlayer(SDL_Renderer* renderer, const Player& player) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        int playerPX = (player.getX() * 9);
        int playerPY = (player.getY() * 9);

        SDL_RenderPoint(renderer, playerPX / 9, playerPY / 9);
        SDL_FRect rect = { playerPX, playerPY, 3, 3 };
        SDL_RenderFillRect(renderer, &rect);
    }

    void drawRays(SDL_Renderer* renderer, const Player& player, std::vector<RayHit> rayResults) {
        for (const auto& ray : rayResults) {
            std::cout << "Distance: " << ray.distance << std::endl;
            std::cout << "Hit X: " << ray.hitX << std::endl;
            std::cout << "Hit Y: " << ray.hitY << std::endl;
            std::cout << "Wall Type: " << ray.wallType << std::endl;
            std::cout << "Hit Vertical Wall: " << (ray.hitVerticalWall ? "Yes" : "No") << std::endl;
            std::cout << "---" << std::endl;

            float startX = player.getX() * 9; //Multiplied by 9 to normalise to pixel form instead of 2d vector
            float startY = player.getY() * 9;

            float endX = ray.hitX * 9;
            float endY = ray.hitY * 9;

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderLine(renderer, startX, startY, endX , endY);

        }
    }
public:
    void render(SDL_Renderer* renderer, Player player, std::vector<RayHit> rayResults) {
        drawPlayer(renderer, player);
        drawRays(renderer, player, rayResults);
    }
}; 

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

class MapWindow {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Grid grid;
    PlayerView playerView;
    int windowWidth;
    int windowHeight;

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

public:
    MapWindow(int width = 566, int height = 566)
        : window(nullptr), renderer(nullptr),
          running(false), windowWidth(width), windowHeight(height) {}

    ~MapWindow() {
        cleanup();
    }

    bool isRunning() {
        return running;
    }

    bool init() {
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

    void initRun(WorldMap map) {
        running = true;

        grid.setWorldMap(map.getMap());
    }

    void update(Player player, std::vector<RayHit> rayResults) {
        // Handle events
        handleEvents();
        
        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);
        
        // Render content
        grid.render(renderer);
        playerView.render(renderer, player, rayResults);
        
        // Present to screen
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }
};
