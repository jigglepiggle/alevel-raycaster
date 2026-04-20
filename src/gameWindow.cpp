#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <cstring>

#include "game.cpp"
#include "raycaster.cpp"
#include "texture.cpp"

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    Color(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0, unsigned char alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
};

class GameView {
private:
    // Streaming texture used as a pixel buffer — created once, written every frame
    SDL_Texture* pixelBuffer = nullptr;
    int bufferWidth  = 0;
    int bufferHeight = 0;

    // Write a single pixel into the locked pixel buffer
    inline void setPixel(uint32_t* pixels, int pitch, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        // pitch is in bytes; divide by 4 to get uint32_t stride
        pixels[y * (pitch / 4) + x] = (0xFF000000) | (r << 16) | (g << 8) | b;
    }

    void drawScene(uint32_t* pixels, int pitch,
                   const std::vector<RayHit>& rayResults,
                   int screenW, int screenH,
                   const std::vector<Texture>& textures) {

        int halfH = screenH / 2;

        // --- Sky and floor background ---
        for (int y = 0; y < screenH; y++) {
            uint8_t r, g, b;
            if (y < halfH) {
                // Sky
                r = 50; g = 50; b = 80;
            } else {
                // Floor
                r = 30; g = 30; b = 60;
            }
            uint32_t* row = pixels + y * (pitch / 4);
            for (int x = 0; x < screenW; x++) {
                row[x] = (0xFF000000) | (r << 16) | (g << 8) | b;
            }
        }

        // --- Wall columns ---
        for (int i = 0; i < static_cast<int>(rayResults.size()); i++) {
            const auto& ray = rayResults[i];

            int wallHeight = static_cast<int>((ray.distance > 0.01f)
                             ? (360.0f / ray.distance)
                             : screenH);
            if (wallHeight > screenH) wallHeight = screenH;

            int wallTop = (screenH - wallHeight) / 2;

            int texIndex = ray.wallType - 1;

            if (texIndex < 0 || texIndex >= static_cast<int>(textures.size())) {
                // Fallback solid colour
                uint8_t r = 100, g = 100, b = 100;
                switch (ray.wallType) {
                    case 1: r = 255; g = 0;   b = 0;   break;
                    case 2: r = 255; g = 255; b = 0;   break;
                }
                for (int y = 0; y < wallHeight; y++) {
                    setPixel(pixels, pitch, i, wallTop + y, r, g, b);
                }
                continue;
            }

            const Texture& tex = textures[texIndex];
            int texX = static_cast<int>(ray.wallU * tex.width);
            texX = std::max(0, std::min(texX, tex.width - 1));

            for (int y = 0; y < wallHeight; y++) {
                int texY = static_cast<int>((static_cast<float>(y) / wallHeight) * tex.height);
                texY = std::max(0, std::min(texY, tex.height - 1));

                uint32_t pixel = tex.getPixel(texX, texY);
                uint8_t r = (pixel >> 16) & 0xFF;
                uint8_t g = (pixel >> 8)  & 0xFF;
                uint8_t b = (pixel)       & 0xFF;

                // Darken horizontal wall hits for depth shading
                if (!ray.hitVerticalWall) { r /= 2; g /= 2; b /= 2; }

                setPixel(pixels, pitch, i, wallTop + y, r, g, b);
            }
        }
    }

public:
    // Call once after the renderer is ready
    bool init(SDL_Renderer* renderer, int width, int height) {
        bufferWidth  = width;
        bufferHeight = height;

        pixelBuffer = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width, height
        );

        if (!pixelBuffer) {
            std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
            return false;
        }
        return true;
    }

    void destroy() {
        if (pixelBuffer) {
            SDL_DestroyTexture(pixelBuffer);
            pixelBuffer = nullptr;
        }
    }

    void render(SDL_Renderer* renderer,
                const std::vector<RayHit>& rayResults,
                int screenW, int screenH,
                const std::vector<Texture>& textures) {

        void* rawPixels = nullptr;
        int   pitch     = 0;

        if (!SDL_LockTexture(pixelBuffer, nullptr, &rawPixels, &pitch)) {
            std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << std::endl;
            return;
        }

        uint32_t* pixels = static_cast<uint32_t*>(rawPixels);

        drawScene(pixels, pitch, rayResults, screenW, screenH, textures);

        SDL_UnlockTexture(pixelBuffer);

        // Copy the finished pixel buffer to the renderer
        SDL_RenderTexture(renderer, pixelBuffer, nullptr, nullptr);
    }
};


class GameWindow {
private:
    SDL_Window*   window;
    SDL_Renderer* renderer;
    GameView      gameView;
    bool          running;
    int           windowWidth;
    int           windowHeight;
    int           FOV;

    void cleanup() {
        gameView.destroy();
        if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
        if (window)   { SDL_DestroyWindow(window);     window   = nullptr; }
        SDL_Quit();
    }

    void handleEvents(Player& player) {
        const bool* keys = SDL_GetKeyboardState(nullptr);

        if (keys[SDL_SCANCODE_ESCAPE])                           running = false;
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])      player.moveForward();
        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])    player.moveBackwards();
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])    player.turnLeft();
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])   player.turnRight();
    }

public:
    GameWindow(int width = 1280, int height = 720, int fov = 120)
        : window(nullptr), renderer(nullptr),
          running(false), windowWidth(width), windowHeight(height), FOV(fov) {}

    ~GameWindow() { cleanup(); }

    bool isRunning() { return running; }

    bool init() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("Game Window", windowWidth, windowHeight, 0);
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

        if (!gameView.init(renderer, windowWidth, windowHeight)) {
            return false;
        }

        return true;
    }

    void initRun() {
        running = true;
    }

    void update(Player& player, const std::vector<RayHit>& rayResults, const std::vector<Texture>& textures) {
        handleEvents(player);

        SDL_RenderClear(renderer);

        gameView.render(renderer, rayResults, windowWidth, windowHeight, textures);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
};
