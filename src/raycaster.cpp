#pragma once

#include "game.cpp"
#include <math.h>

class RayHit {
public:
    float distance;
    float hitX;
    float hitY;
    float wallU;         // 0.0 to 1.0 — position along the wall face for texture sampling
    int wallType;
    bool hitVerticalWall;
    float angle;
};

enum class WallType {
    HORIZONTAL,
    VERTICAL
};

class Raycaster {
private:
    std::vector<std::vector<int>> worldMap;
    float maxRayDistance;

    // Helper function to normalize angle difference to [-π, π]
    float normalizeAngleDiff(float angle) {
        while (angle > M_PI) angle -= 2 * M_PI;
        while (angle < -M_PI) angle += 2 * M_PI;
        return angle;
    }

    RayHit castSingleRay(float startX, float startY, float angle, float playerAngle) {
        float rayDirectionX = cos(angle);
        float rayDirectionY = sin(angle);

        int currentMapX = static_cast<int>(floor(startX));
        int currentMapY = static_cast<int>(floor(startY));

        float deltaDistX = abs(1 / rayDirectionX);
        float deltaDistY = abs(1 / rayDirectionY);

        int stepX;
        int stepY;
        float sideDistX;
        float sideDistY;

        if (rayDirectionX < 0) {
            stepX = -1;
            sideDistX = (startX - currentMapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (currentMapX + 1 - startX) * deltaDistX;
        }

        if (rayDirectionY < 0) {
            stepY = -1;
            sideDistY = (startY - currentMapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (currentMapY + 1 - startY) * deltaDistY;
        }

        bool hitWall = false;
        WallType hitSide = WallType::VERTICAL;

        float distance = 0.0;

        int mapHeight = static_cast<int>(worldMap.size());
        int mapWidth  = static_cast<int>(worldMap[0].size());

        while (!hitWall && distance < maxRayDistance) {
            if (sideDistX < sideDistY) {
                sideDistX    += deltaDistX;
                currentMapX  += stepX;
                hitSide = WallType::VERTICAL;
            } else {
                sideDistY    += deltaDistY;
                currentMapY  += stepY;
                hitSide = WallType::HORIZONTAL;
            }

            if (currentMapY < 0 || currentMapY >= mapHeight ||
                currentMapX < 0 || currentMapX >= mapWidth) {
                hitWall = true;
            } else if (worldMap[currentMapY][currentMapX] != 0) {
                hitWall = true;
            }
        }

        // Use the DDA's accumulated distance directly
        float wallDistance;
        if (hitSide == WallType::VERTICAL) {
            wallDistance = sideDistX - deltaDistX;
        } else {
            wallDistance = sideDistY - deltaDistY;
        }

        // Calculate hit point using the actual ray distance (before fisheye correction)
        float hitPointX = startX + rayDirectionX * wallDistance;
        float hitPointY = startY + rayDirectionY * wallDistance;

        // Calculate wallU: fractional position along the wall face
        float wallU;
        if (hitSide == WallType::VERTICAL) {
            wallU = hitPointY - floor(hitPointY);
            if (rayDirectionX > 0) wallU = 1.0f - wallU; // flip so texture isn't mirrored
        } else {
            wallU = hitPointX - floor(hitPointX);
            if (rayDirectionY < 0) wallU = 1.0f - wallU;
        }

        // Apply fisheye correction to get perpendicular distance
        float angleDiff = normalizeAngleDiff(angle - playerAngle);
        wallDistance = wallDistance * cos(angleDiff);

        RayHit rayHit;
        rayHit.distance       = wallDistance;
        rayHit.hitX           = hitPointX;
        rayHit.hitY           = hitPointY;
        rayHit.wallU          = wallU;
        rayHit.angle          = angle;
        rayHit.hitVerticalWall = (hitSide == WallType::VERTICAL);

        if (currentMapY >= 0 && currentMapY < mapHeight &&
            currentMapX >= 0 && currentMapX < mapWidth) {
            rayHit.wallType = worldMap[currentMapY][currentMapX];
        } else {
            rayHit.wallType = 1; // default for out-of-bounds
        }

        return rayHit;
    }

public:
    Raycaster(WorldMap worldMapObj) {
        worldMap = worldMapObj.getMap();
    }

    std::vector<RayHit> castAllRays(Player& player, int screenWidth) {
        std::vector<RayHit> rayResults;

        float playerAngle = player.getAngle();

        float dirX = cos(playerAngle);
        float dirY = sin(playerAngle);

        float FOVRadians  = player.getFieldOfView() * (M_PI / 180.0f);
        float planeLength = tan(FOVRadians / 2.0f);
        float planeX      = -dirY * planeLength;
        float planeY      =  dirX * planeLength;

        for (int x = 0; x < screenWidth; x++) {
            // Camera plane position: -1 (left) to +1 (right)
            float cameraX = 2.0f * x / static_cast<float>(screenWidth) - 1.0f;

            float rayDirX = dirX + planeX * cameraX;
            float rayDirY = dirY + planeY * cameraX;

            // Normalise ray direction
            float rayLength = sqrt(rayDirX * rayDirX + rayDirY * rayDirY);
            rayDirX /= rayLength;
            rayDirY /= rayLength;

            float currentRayAngle = atan2(rayDirY, rayDirX);

            rayResults.push_back(castSingleRay(player.getX(), player.getY(), currentRayAngle, playerAngle));
        }
        return rayResults;
    }

    void setMaxDistance(float distance) { maxRayDistance = distance; }
};
