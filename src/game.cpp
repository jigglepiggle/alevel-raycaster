#pragma once

#include <vector>
#include <math.h>

class WorldMap {
private:
    std::vector<std::vector<int>> mapData;
    int height;
    int width;
    
public:
    WorldMap(const std::vector<std::vector<int>>& mapData, int height, int width) 
        : mapData(mapData), height(height), width(width) {
        // Validate dimensions match mapData
        //if (mapData.size() != height || (height > 0 && mapData[0].size() != width)) {
        //    throw std::invalid_argument("Map dimensions don't match data");
        //}
    }

    // Return const reference to avoid copying
    const std::vector<std::vector<int>>& getMap() const { return mapData; }
    
    // Validate bounds before access
    bool isWall(int x, int y) const { 
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return true; // Treat out-of-bounds as walls
        }
        return mapData[y][x] != 0; 
    }
    
    int getWallType(int x, int y) const { 
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return 1; // Default wall type
        }
        return mapData[y][x]; 
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

class Player {
private:
    float x;
    float y;
    float angle;
    float FOV;
    float moveSpeed;
    float rotateSpeed;
    
    // Add reference to map for collision detection
    const WorldMap* worldMap;

public:
    Player(float startX, float startY, float startAngle, float fov, float rotateSpeed = 2.0f) 
        : x(startX), y(startY), angle(startAngle), FOV(fov), 
          rotateSpeed(rotateSpeed), moveSpeed(0.2f), worldMap(nullptr) {}
    
    // Allow setting world map for collision detection
    void setWorldMap(const WorldMap* map) { worldMap = map; }
    
    // Improved movement with collision detection
    void moveForward() { 
        float newX = x + cos(angle) * moveSpeed;
        float newY = y + sin(angle) * moveSpeed;
        
        if (worldMap && !worldMap->isWall(static_cast<int>(newX), static_cast<int>(newY))) {
            x = newX;
            y = newY;
        }
    }
    
    void moveBackwards() { 
        float newX = x - cos(angle) * moveSpeed;
        float newY = y - sin(angle) * moveSpeed;
        
        if (worldMap && !worldMap->isWall(static_cast<int>(newX), static_cast<int>(newY))) {
            x = newX;
            y = newY;
        }
    }
    
    void turnLeft() { angle -= 0.1f * rotateSpeed; }
    void turnRight() { angle += 0.1f * rotateSpeed; }
    
    // Const getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }
    float getFieldOfView() const { return FOV; }
    
    // Validated setters
    void setPosition(float nx, float ny) { 
        if (worldMap && worldMap->isWall(static_cast<int>(nx), static_cast<int>(ny))) {
            return; // Don't allow moving into walls
        }
        x = nx; 
        y = ny; 
    }
    
    void setAngle(float nAngle) { angle = nAngle; }
};