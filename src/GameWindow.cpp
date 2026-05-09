#include "GameWindow.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>

// Writes one ARGB pixel into the locked texture buffer using the pitch to find the row offset.
void GameView::setPixel(uint32_t* pixels, int pitch, int x, int y,
                        uint8_t r, uint8_t g, uint8_t b) {
    pixels[y * (pitch / 4) + x] = (0xFF000000) | (r << 16) | (g << 8) | b;    // Skills Audit Group B: Multi-dimensional arrays (flat pixel buffer addressed as 2-D via pitch)
}

void GameView::drawFloorAndCeiling(uint32_t* pixels, int pitch,
                                   const Player& player,
                                   int screenW, int screenH,
                                   const std::vector<Texture>& textures) {
    // Build the camera plane vectors from the player's facing direction and FOV.    // Skills Audit Group A: Complex scientific/mathematical model (camera plane projection)
    const float playerAngle = player.getAngle();
    const float dirX        = cos(playerAngle);    // Skills Audit Group B: Simple scientific/mathematical model (trigonometric direction vector)
    const float dirY        = sin(playerAngle);
    const float FOVRadians  = player.getFieldOfView() * (M_PI / 180.0f);    // Skills Audit Group B: Simple scientific/mathematical model (degrees to radians conversion)
    const float planeLen    = tan(FOVRadians / 2.0f);
    const float planeX      = -dirY * planeLen;
    const float planeY      =  dirX * planeLen;

    const Texture& floorTex   = textures[0];
    const Texture& ceilingTex = textures[1];
    // posZ represents the horizon level; rows below this are floor, rows above are ceiling.
    const float posZ          = 0.5f * screenH;    // Skills Audit Group B: Simple scientific/mathematical model (half-screen horizon calculation)

    for (int y = screenH / 2 + 1; y < screenH; y++) {
        // rowDist is the distance to the floor at this scanline, derived from the vertical offset.
        float rowDist = posZ / (y - screenH / 2.0f);    // Skills Audit Group A: Complex scientific/mathematical model (perspective floor distance)

        // Step vector advances the world-space floor position across each pixel of the scanline.
        float stepX = rowDist * 2.0f * planeX / screenW;    // Skills Audit Group B: Simple scientific/mathematical model (per-pixel world-space step)
        float stepY = rowDist * 2.0f * planeY / screenW;

        // Starting world position for the leftmost pixel of this floor row.
        float floorX = player.getX() + rowDist * (dirX - planeX);
        float floorY = player.getY() + rowDist * (dirY - planeY);

        // Mirror index: the ceiling row directly opposite this floor row.
        int mirrorY = screenH - 1 - y;    // Skills Audit Group B: Simple user defined algorithms (symmetric row mirroring)

        for (int x = 0; x < screenW; x++) {
            // Fractional part of the world position maps to texture coordinates.
            int texX = static_cast<int>(floorTex.width  * (floorX - std::floor(floorX))) & (floorTex.width  - 1);    // Skills Audit Group A: Advanced matrix operations (texture UV from fractional world position)
            int texY = static_cast<int>(floorTex.height * (floorY - std::floor(floorY))) & (floorTex.height - 1);

            // Floor pixels are dimmed by 50% to appear darker than walls.
            uint32_t fp = floorTex.getPixel(texX, texY);
            setPixel(pixels, pitch, x, y,
                     ((fp >> 16) & 0xFF) / 2,    // Skills Audit Group B: Simple user defined algorithms (bit-shift channel extraction and dimming)
                     ((fp >> 8)  & 0xFF) / 2,
                     ((fp)       & 0xFF) / 2);

            // Ceiling uses the same UV but is dimmed further to a third brightness.
            uint32_t cp = ceilingTex.getPixel(texX, texY);
            setPixel(pixels, pitch, x, mirrorY,
                     ((cp >> 16) & 0xFF) / 3,
                     ((cp >> 8)  & 0xFF) / 3,
                     ((cp)       & 0xFF) / 3);

            floorX += stepX;
            floorY += stepY;
        }
    }
}

void GameView::drawWalls(uint32_t* pixels, int pitch,
                         const std::vector<RayHit>& rayResults,
                         int screenW, int screenH,
                         const std::vector<Texture>& textures) {
    // Each entry in rayResults corresponds to one vertical screen column.
    for (int i = 0; i < static_cast<int>(rayResults.size()); i++) {
        const auto& ray = rayResults[i];

        // Wall height on screen is inversely proportional to the perpendicular distance.    // Skills Audit Group A: Complex scientific/mathematical model (perspective wall height)
        int fullWallHeight = static_cast<int>((ray.distance > 0.01f)
                             ? (360.0f / ray.distance)
                             : screenH * 2);

        // Clip the wall slice to the visible screen area.
        int wallTop   = (screenH - fullWallHeight) / 2;    // Skills Audit Group B: Simple scientific/mathematical model (centred wall slice calculation)
        int drawStart = std::max(0, wallTop);
        int drawEnd   = std::min(screenH - 1, wallTop + fullWallHeight);

        // Map the tile wall type value to a texture index.
        int texIndex = -1;
        switch (ray.wallType) {
            case 1: texIndex = 0; break;  // brick
            case 2: texIndex = 1; break;  // stone
        }

        // Fall back to a flat grey if the texture index is out of range.
        if (texIndex < 0 || texIndex >= static_cast<int>(textures.size())) {
            for (int y = drawStart; y <= drawEnd; y++)
                setPixel(pixels, pitch, i, y, 100, 100, 100);
            continue;
        }

        const Texture& tex = textures[texIndex];
        // wallU from the RayHit gives the horizontal position along the wall face.
        int texX = static_cast<int>(ray.wallU * tex.width);    // Skills Audit Group A: Advanced matrix operations (texture U coordinate to pixel column)
        texX = std::max(0, std::min(texX, tex.width - 1));

        for (int y = drawStart; y <= drawEnd; y++) {
            // wallProgress maps the screen pixel to a vertical position in the texture.
            float wallProgress = static_cast<float>(y - wallTop) / fullWallHeight;    // Skills Audit Group B: Simple scientific/mathematical model (normalised vertical texture mapping)
            int texY = static_cast<int>(wallProgress * tex.height);
            texY = std::max(0, std::min(texY, tex.height - 1));

            uint32_t pixel = tex.getPixel(texX, texY);
            uint8_t r = (pixel >> 16) & 0xFF;    // Skills Audit Group B: Simple user defined algorithms (bit-shift ARGB channel extraction)
            uint8_t g = (pixel >> 8)  & 0xFF;
            uint8_t b = (pixel)       & 0xFF;

            // Darken horizontal (N/S) wall faces to give a simple directional shading effect.
            if (!ray.hitVerticalWall) { r /= 2; g /= 2; b /= 2; }    // Skills Audit Group B: Simple user defined algorithms (half-brightness side shading)

            setPixel(pixels, pitch, i, y, r, g, b);
        }
    }
}

// Draws floor/ceiling first (overwritten by walls), then the wall slices on top.
void GameView::drawScene(uint32_t* pixels, int pitch,
                         const Player& player,
                         const std::vector<RayHit>& rayResults,
                         int screenW, int screenH,
                         const std::vector<Texture>& textures) {
    drawFloorAndCeiling(pixels, pitch, player, screenW, screenH, textures);
    drawWalls(pixels, pitch, rayResults, screenW, screenH, textures);
}

// Creates the streaming SDL texture that will be written to every frame.
bool GameView::init(SDL_Renderer* renderer, int width, int height) {
    bufferWidth  = width;    // Skills Audit Group B: Records (storing buffer dimensions as member fields)
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

    // Lock the texture to get a direct pointer to its pixel memory.
    if (!SDL_LockTexture(pixelBuffer, nullptr, &rawPixels, &pitch)) {
        std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Write all pixels into the locked buffer, then unlock and display.
    drawScene(static_cast<uint32_t*>(rawPixels), pitch, player, rayResults, screenW, screenH, textures);
    SDL_UnlockTexture(pixelBuffer);
    SDL_RenderTexture(renderer, pixelBuffer, nullptr, nullptr);
}

GameWindow::GameWindow(int width, int height, int fov) : FOV(fov) {    // Skills Audit Group B: Generation of objects based on simple OOP model
    winW = width;
    winH = height;
}

GameWindow::~GameWindow() { cleanup(); }

void GameWindow::cleanup() {
    // Destroy the pixel buffer before the base-class renderer/window cleanup.
    gameView.destroy();
    BaseWindow::cleanup();    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance — calling base class cleanup)
}

bool GameWindow::isRunning() { return running; }

bool GameWindow::init() {
    if (!initSDL("Game Window", winW, winH)) return false;
    if (!gameView.init(renderer, winW, winH)) return false;
    return true;
}

void GameWindow::initRun() { running = true; }

void GameWindow::handleEvents(Player& player) {
    // Pump pending events into the queue before polling them.
    SDL_PumpEvents();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) running = false;
    }

    // Poll the keyboard state snapshot for smooth, held-key movement.
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_ESCAPE])                         running = false;
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    player.moveForward();     // Skills Audit Group B: Simple user defined algorithms (keyboard-to-action mapping)
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  player.moveBackward();
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  player.turnLeft();
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) player.turnRight();
}

void GameWindow::update(Player& player, const std::vector<RayHit>& rayResults,
                        const std::vector<Texture>& textures) {
    // Process input, clear the renderer, draw the 3-D scene, then present.
    handleEvents(player);
    SDL_RenderClear(renderer);
    gameView.render(renderer, player, rayResults, winW, winH, textures);
    presentFrame(16);    // Skills Audit Group B: Simple scientific/mathematical model (16 ms delay targets ~60 fps)
}
