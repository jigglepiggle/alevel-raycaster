#include <SDL3/SDL.h>
#include <iostream>
#include <vector>

#include "game.cpp"
#include "raycaster.cpp"

//Not finished at all
class GameView {
private:
    int rayWidth;

    void drawRays(SDL_Renderer* renderer, float rayWidth, std::vector<RayHit> rayResults, float screenHeight, float screenWidth) {
        for (size_t i = 0; i < rayResults.size(); i++) {
            const auto& ray = rayResults[i];

            // Scale factor to adjust wall height perception
            float wallHeight = (ray.distance > 0.01f) ? (360.0f / ray.distance) : screenHeight;

            // Cap the wall height to screen height
            if (wallHeight > screenHeight) wallHeight = screenHeight;

            // Calculate top and bottom of the wall slice
            float wallTop = (screenHeight - wallHeight) / 2.0f;

            // Draw each ray as a 1-pixel wide vertical line
            SDL_FRect rect = { (float)i, wallTop, 1.0f, wallHeight };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    void drawFloor(SDL_Renderer* renderer, float windowHeight, float windowWidth) {
        SDL_FRect rect = { 0, 0, windowWidth, windowHeight };
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

public:
    void render(SDL_Renderer* renderer, float rayWidth, std::vector<RayHit> rayResults, float screenHeight, float screenWidth) {
        drawFloor(renderer, screenHeight, screenWidth);
        drawRays(renderer, rayWidth, rayResults, screenHeight, screenWidth);
    }
}; 


class GameWindow {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    GameView gameView;
    bool running;
    int windowWidth;
    int windowHeight;
    int FOV;
    float rayWidth;

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

    void handleEvents(Player& player) {
        const bool *keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_ESCAPE]) {
            running = false;
        }

        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
            player.moveForward();
        }
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
            player.moveBackwards();
        }
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {
            player.turnLeft();
        }
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
            player.turnRight();
        }
    }

public:
    GameWindow(int width = 1280, int height = 720, int fov = 120)
        : window(nullptr), renderer(nullptr),
          running(false), windowWidth(width), windowHeight(height), FOV(fov) {}

    ~GameWindow() {
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
            "Game Window",
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

    void initRun() {
        running = true;

        rayWidth = 1.0f;
    }

    void update(Player& player, std::vector<RayHit> rayResults) {
        // Handle events
        handleEvents(player);

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);
        
        // Render content
        gameView.render(renderer, rayWidth, rayResults, windowHeight, windowWidth);
        
        // Present to screen
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }
};
