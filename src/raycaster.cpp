#pragma once

#include "game.cpp"
#include <math.h>

class RayHit {
public:
    float distance;
    float hitX;
    float hitY;
    float wallU;
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

    RayHit castSingleRay(float startX, float startY, float rayDirectionX, float rayDirectionY, float playerAngle) {

        // Normalise so wallDistance formula and wallU ratio are correct
        float rayLength = sqrt(rayDirectionX * rayDirectionX + rayDirectionY * rayDirectionY);
        rayDirectionX /= rayLength;
        rayDirectionY /= rayLength;

        int currentMapX = static_cast<int>(floor(startX));
        int currentMapY = static_cast<int>(floor(startY));

        float deltaDistX = abs(1.0f / rayDirectionX);
        float deltaDistY = abs(1.0f / rayDirectionY);

        int stepX, stepY;
        float sideDistX, sideDistY;

        if (rayDirectionX < 0) {
            stepX     = -1;
            sideDistX = (startX - currentMapX) * deltaDistX;
        } else {
            stepX     = 1;
            sideDistX = (currentMapX + 1.0f - startX) * deltaDistX;
        }

        if (rayDirectionY < 0) {
            stepY     = -1;
            sideDistY = (startY - currentMapY) * deltaDistY;
        } else {
            stepY     = 1;
            sideDistY = (currentMapY + 1.0f - startY) * deltaDistY;
        }

        bool hitWall = false;
        WallType hitSide = WallType::VERTICAL;

        int mapHeight = static_cast<int>(worldMap.size());
        int mapWidth  = static_cast<int>(worldMap[0].size());

        while (!hitWall) {
            if (sideDistX < sideDistY) {
                if (sideDistX > maxRayDistance) break;
                sideDistX   += deltaDistX;
                currentMapX += stepX;
                hitSide = WallType::VERTICAL;
            } else {
                if (sideDistY > maxRayDistance) break;
                sideDistY   += deltaDistY;
                currentMapY += stepY;
                hitSide = WallType::HORIZONTAL;
            }

            if (currentMapY < 0 || currentMapY >= mapHeight ||
                currentMapX < 0 || currentMapX >= mapWidth) {
                hitWall = true;
            } else if (worldMap[currentMapY][currentMapX] != 0) {
                hitWall = true;
            }
        }

        // Perpendicular distance — correct formula for normalised direction
        float wallDistance;
        if (hitSide == WallType::VERTICAL) {
            wallDistance = (currentMapX - startX + (1.0f - stepX) / 2.0f) / rayDirectionX;
        } else {
            wallDistance = (currentMapY - startY + (1.0f - stepY) / 2.0f) / rayDirectionY;
        }

        // wallU from exact intercept coordinate via direction ratio
        float wallU;
        if (hitSide == WallType::VERTICAL) {
            float exactY = startY + wallDistance * (rayDirectionY / rayDirectionX);
            wallU = exactY - floor(exactY);
            if (rayDirectionX > 0) wallU = 1.0f - wallU;
        } else {
            float exactX = startX + wallDistance * (rayDirectionX / rayDirectionY);
            wallU = exactX - floor(exactX);
            if (rayDirectionY < 0) wallU = 1.0f - wallU;
        }

        // Fisheye correction: project onto the camera plane rather than using
        // raw ray distance. dot product of the ray direction with the player
        // forward direction gives the perpendicular component.
        float dirX = cos(playerAngle);
        float dirY = sin(playerAngle);
        wallDistance = wallDistance * (rayDirectionX * dirX + rayDirectionY * dirY);

        float hitPointX = startX + rayDirectionX * wallDistance;
        float hitPointY = startY + rayDirectionY * wallDistance;

        RayHit rayHit;
        rayHit.distance        = wallDistance;
        rayHit.hitX            = hitPointX;
        rayHit.hitY            = hitPointY;
        rayHit.wallU           = wallU;
        rayHit.angle           = atan2(rayDirectionY, rayDirectionX);
        rayHit.hitVerticalWall = (hitSide == WallType::VERTICAL);

        if (currentMapY >= 0 && currentMapY < mapHeight &&
            currentMapX >= 0 && currentMapX < mapWidth) {
            rayHit.wallType = worldMap[currentMapY][currentMapX];
        } else {
            rayHit.wallType = 1;
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
            float cameraX = 2.0f * x / static_cast<float>(screenWidth) - 1.0f;

            float rayDirX = dirX + planeX * cameraX;
            float rayDirY = dirY + planeY * cameraX;

            rayResults.push_back(castSingleRay(player.getX(), player.getY(), rayDirX, rayDirY, playerAngle));
        }
        return rayResults;
    }

    void setMaxDistance(float distance) { maxRayDistance = distance; }
};
