#include "GameWindow.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>

void GameView::setPixel(uint32_t* pixels, int pitch, int x, int y,
                        uint8_t r, uint8_t g, uint8_t b) {
    pixels[y * (pitch / 4) + x] = (0xFF000000) | (r << 16) | (g << 8) | b;
}

void GameView::drawFloorAndCeiling(uint32_t* pixels, int pitch,
                                   const Player& player,
                                   int screenW, int screenH,
                                   const std::vector<Texture>& textures) {
    const float playerAngle = player.getAngle();
    const float dirX        = cos(playerAngle);
    const float dirY        = sin(playerAngle);
    const float FOVRadians  = player.getFieldOfView() * (M_PI / 180.0f);
    const float planeLen    = tan(FOVRadians / 2.0f);
    const float planeX      = -dirY * planeLen;
    const float planeY      =  dirX * planeLen;

    const Texture& floorTex   = textures[0];
    const Texture& ceilingTex = textures[1];

    for (int y = screenH / 2 + 1; y < screenH; y++) {
        // How far below the horizon this row is (0.5 = exactly at horizon)
        float rowAngle  = static_cast<float>(y) / screenH - 0.5f;
        float rowDist   = 0.5f / rowAngle;

        // Step in world space per screen pixel across this row
        float stepX = rowDist * 2.0f * planeX / screenW;
        float stepY = rowDist * 2.0f * planeY / screenW;

        // World coordinate at the leftmost pixel of this row
        float floorX = player.getX() + rowDist * (dirX - planeX);
        float floorY = player.getY() + rowDist * (dirY - planeY);

        int mirrorY = screenH - 1 - y;

        for (int x = 0; x < screenW; x++) {
            int texX = static_cast<int>(floorTex.width  * (floorX - floor(floorX))) & (floorTex.width  - 1);
            int texY = static_cast<int>(floorTex.height * (floorY - floor(floorY))) & (floorTex.height - 1);

            // Floor
            uint32_t fp = floorTex.getPixel(texX, texY);
            uint8_t fr = ((fp >> 16) & 0xFF) / 2;
            uint8_t fg = ((fp >> 8)  & 0xFF) / 2;
            uint8_t fb = ((fp)       & 0xFF) / 2;
            setPixel(pixels, pitch, x, y, fr, fg, fb);

            // Ceiling — mirrored row, same UV
            int ctx = static_cast<int>(ceilingTex.width  * (floorX - floor(floorX))) & (ceilingTex.width  - 1);
            int cty = static_cast<int>(ceilingTex.height * (floorY - floor(floorY))) & (ceilingTex.height - 1);
            uint32_t cp = ceilingTex.getPixel(ctx, cty);
            uint8_t cr = ((cp >> 16) & 0xFF) / 3;
            uint8_t cg = ((cp >> 8)  & 0xFF) / 3;
            uint8_t cb = ((cp)       & 0xFF) / 3;
            setPixel(pixels, pitch, x, mirrorY, cr, cg, cb);

            floorX += stepX;
            floorY += stepY;
        }
    }
}

void GameView::drawWalls(uint32_t* pixels, int pitch,
                         const std::vector<RayHit>& rayResults,
                         int screenW, int screenH,
                         const std::vector<Texture>& textures) {
    for (int i = 0; i < static_cast<int>(rayResults.size()); i++) {
        const auto& ray = rayResults[i];

        int fullWallHeight = static_cast<int>((ray.distance > 0.01f)
                             ? (360.0f / ray.distance)
                             : screenH * 2);

        int wallTop   = (screenH - fullWallHeight) / 2;
        int drawStart = std::max(0, wallTop);
        int drawEnd   = std::min(screenH - 1, wallTop + fullWallHeight);
        int texIndex  = ray.wallType - 1;

        if (texIndex < 0 || texIndex >= static_cast<int>(textures.size())) {
            uint8_t r = 100, g = 100, b = 100;
            switch (ray.wallType) {
                case 1: r = 255; g = 0;   b = 0;   break;
                case 2: r = 255; g = 255; b = 0;   break;
            }
            for (int y = drawStart; y <= drawEnd; y++)
                setPixel(pixels, pitch, i, y, r, g, b);
            continue;
        }

        const Texture& tex = textures[texIndex];
        int texX = static_cast<int>(ray.wallU * tex.width);
        texX = std::max(0, std::min(texX, tex.width - 1));

        for (int y = drawStart; y <= drawEnd; y++) {
            float wallProgress = static_cast<float>(y - wallTop) / fullWallHeight;
            int texY = static_cast<int>(wallProgress * tex.height);
            texY = std::max(0, std::min(texY, tex.height - 1));

            uint32_t pixel = tex.getPixel(texX, texY);
            uint8_t r = (pixel >> 16) & 0xFF;
            uint8_t g = (pixel >> 8)  & 0xFF;
            uint8_t b = (pixel)       & 0xFF;

            if (!ray.hitVerticalWall) { r /= 2; g /= 2; b /= 2; }

            setPixel(pixels, pitch, i, y, r, g, b);
        }
    }
}

void GameView::drawScene(uint32_t* pixels, int pitch,
                         const Player& player,
                         const std::vector<RayHit>& rayResults,
                         int screenW, int screenH,
                         const std::vector<Texture>& textures) {
    drawFloorAndCeiling(pixels, pitch, player, screenW, screenH, textures);
    drawWalls(pixels, pitch, rayResults, screenW, screenH, textures);
}

bool GameView::init(SDL_Renderer* renderer, int width, int height) {
    bufferWidth  = width;
    bufferHeight = height;
    pixelBuffer  = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!pixelBuffer) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void GameView::destroy() {
    if (pixelBuffer) { SDL_DestroyTexture(pixelBuffer); pixelBuffer = nullptr; }
}

void GameView::render(SDL_Renderer* renderer,
                      const Player& player,
                      const std::vector<RayHit>& rayResults,
                      int screenW, int screenH,
                      const std::vector<Texture>& textures) {
    void* rawPixels = nullptr;
    int   pitch     = 0;

    if (!SDL_LockTexture(pixelBuffer, nullptr, &rawPixels, &pitch)) {
        std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << std::endl;
        return;
    }

    drawScene(static_cast<uint32_t*>(rawPixels), pitch, player, rayResults, screenW, screenH, textures);
    SDL_UnlockTexture(pixelBuffer);
    SDL_RenderTexture(renderer, pixelBuffer, nullptr, nullptr);
}

GameWindow::GameWindow(int width, int height, int fov) : FOV(fov) {
    winW = width;
    winH = height;
}

GameWindow::~GameWindow() { cleanup(); }

void GameWindow::cleanup() {
    gameView.destroy();
    BaseWindow::cleanup();
}

bool GameWindow::isRunning() { return running; }

bool GameWindow::init() {
    if (!initSDL("Game Window", winW, winH)) return false;
    if (!gameView.init(renderer, winW, winH)) return false;
    return true;
}

void GameWindow::initRun() { running = true; }

void GameWindow::handleEvents(Player& player) {
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_ESCAPE])                         running = false;
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    player.moveForward();
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  player.moveBackward();
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  player.turnLeft();
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) player.turnRight();
}

void GameWindow::update(Player& player, const std::vector<RayHit>& rayResults,
                        const std::vector<Texture>& textures) {
    handleEvents(player);
    SDL_RenderClear(renderer);
    gameView.render(renderer, player, rayResults, winW, winH, textures);
    presentFrame();
}
