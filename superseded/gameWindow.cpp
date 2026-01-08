#include <SDL3/SDL.h>
#include <iostream>


class GameWindow {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    int windowWidth;
    int windowHeight;

public:
    GameWindow(int width = 1280, int height = 720)
        : window(nullptr), renderer(nullptr), running(false),
          windowWidth(width), windowHeight(height) {}

    ~GameWindow() {
        cleanup();
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