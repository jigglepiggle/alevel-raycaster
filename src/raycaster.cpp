#pragma once

#include "game.cpp"
#include <math.h>

class RayHit {
public:
    float distance;
    float hitX;
    float hitY;
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

        int currentMapX = floor(startX);
        int currentMapY = floor(startY);

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
        WallType hitSide;

        float distance = 0.0;

        while (!hitWall && distance < maxRayDistance) {
            if (sideDistX < sideDistY) {
                sideDistX = sideDistX + deltaDistX;
                currentMapX = currentMapX + stepX;
                hitSide = WallType::VERTICAL;
            } else {
                sideDistY = sideDistY + deltaDistY;
                currentMapY = currentMapY + stepY;
                hitSide = WallType::HORIZONTAL;
            }

            if (currentMapY < 0 || currentMapY >= worldMap.size() || 
                currentMapX < 0 || currentMapX >= worldMap[0].size()) {
                hitWall = true;
            } else if (worldMap[currentMapY][currentMapX] != 0) {
                hitWall = true;
            }
        }

        // FIXED: Use the DDA's accumulated distance directly
        // This is the actual ray travel distance from the DDA algorithm
        float wallDistance;
        if (hitSide == WallType::VERTICAL) {
            wallDistance = sideDistX - deltaDistX;  // Step back one delta to get distance to wall
        } else {
            wallDistance = sideDistY - deltaDistY;
        }

        // Calculate hit point using the actual ray distance (BEFORE fisheye correction)
        float hitPointX = startX + rayDirectionX * wallDistance;
        float hitPointY = startY + rayDirectionY * wallDistance;

        // Apply fisheye correction to get perpendicular distance
        float angleDiff = normalizeAngleDiff(angle - playerAngle);
        wallDistance = wallDistance * cos(angleDiff);

        RayHit rayHit = RayHit();
        rayHit.distance = wallDistance;
        rayHit.hitX = hitPointX;
        rayHit.hitY = hitPointY;
        rayHit.angle = angle;
            
        // Add bounds check before accessing worldMap
        if (currentMapY >= 0 && currentMapY < worldMap.size() && 
            currentMapX >= 0 && currentMapX < worldMap[0].size()) {
            rayHit.wallType = worldMap[currentMapY][currentMapX];
        } else {
            rayHit.wallType = 1;  // Default wall type for out of bounds
        }
        
        rayHit.hitVerticalWall = (hitSide == WallType::VERTICAL);
        return rayHit;
    }

public:
    Raycaster(WorldMap worldMapObj) {
        worldMap = worldMapObj.getMap();
    }

    std::vector<RayHit> castAllRays(Player& player, int screenWidth) {
        std::vector<RayHit> rayResults;

        float playerAngle = player.getAngle();

        // Calculate camera direction and plane vectors
        float dirX = cos(playerAngle);
        float dirY = sin(playerAngle);

        // Camera plane perpendicular to direction (90 degrees rotated)
        float FOVRadians = player.getFieldOfView() * (M_PI / 180.0f);
        float planeLength = tan(FOVRadians / 2.0f);
        float planeX = -dirY * planeLength;
        float planeY = dirX * planeLength;

        for (int x = 0; x < screenWidth; x++) {
            // Calculate ray position on camera plane (-1 to +1)
            float cameraX = 2.0f * x / (float)screenWidth - 1.0f;

            // Calculate ray direction
            float rayDirX = dirX + planeX * cameraX;
            float rayDirY = dirY + planeY * cameraX;

            // Normalize ray direction
            float rayLength = sqrt(rayDirX * rayDirX + rayDirY * rayDirY);
            rayDirX /= rayLength;
            rayDirY /= rayLength;

            // Calculate angle for this ray
            float currentRayAngle = atan2(rayDirY, rayDirX);

            RayHit rayHit = castSingleRay(player.getX(), player.getY(), currentRayAngle, playerAngle);

            rayResults.push_back(rayHit);
        }
        return rayResults;
    }

// This doesn't work, Fixed angle incrementation only works if the display is curver around the viewer in real life.  Vector Plane projection fixes this as done above
/*
        std::vector<RayHit> rayResults;

        float FOVRadians = player.getFieldOfView() * (M_PI / 180.0f);

        float angleStep = FOVRadians / screenWidth;
        float startAngle = player.getAngle() - FOVRadians/2;

        for (int i = 0; i < screenWidth; i++) {
            float currentRayAngle = startAngle + (i * angleStep);

            RayHit rayHit = castSingleRay(player.getX(), player.getY(), currentRayAngle, player.getAngle());

            rayResults.push_back(rayHit);
        }
        return rayResults;
    }
*/
    void setMaxDistance(float distance){ maxRayDistance = distance; }
};