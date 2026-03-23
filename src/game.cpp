#pragma once

#include <algorithm>
#include <vector>
#include <queue>
#include <math.h>

class WorldMap {
private:
    std::vector<std::vector<int>> mapData;
    int height;
    int width;

    std::vector<std::pair<int,int>> solutionPath;

public:
    WorldMap(const std::vector<std::vector<int>>& mapData, int height, int width) 
        : mapData(mapData), height(height), width(width) {
    }

    // Return const reference to avoid copying
    const std::vector<std::vector<int>>& getMap() const { return mapData; }
    
    // Cells with value 0 or 2 are passable (2 = entrance/exit marker used by DFS generator)
    bool isWall(int x, int y) const { 
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return true; // Treat out-of-bounds as walls
        }
        return mapData[y][x] == 1; 
    }
    
    int getWallType(int x, int y) const { 
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return 1; // Default wall type
        }
        return mapData[y][x]; 
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void solveMaze() {
        solutionPath.clear();

        // Entrance / exit in (x, y) world coordinates
        std::pair<int,int> start = {0, 1};
        std::pair<int,int> goal  = {width - 1, height - 2};

        // BFS
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        // Store parent cell for path reconstruction: parent[y][x] = {px, py}
        std::vector<std::vector<std::pair<int,int>>> parent(
            height, std::vector<std::pair<int,int>>(width, {-1, -1}));

        std::queue<std::pair<int,int>> q;
        q.push(start);
        visited[start.second][start.first] = true;

        const int dx[4] = {0, 1, 0, -1};
        const int dy[4] = {-1, 0, 1, 0};

        bool found = false;
        while (!q.empty() && !found) {
            auto [cx, cy] = q.front();
            q.pop();

            if (cx == goal.first && cy == goal.second) {
                found = true;
                break;
            }

            for (int d = 0; d < 4; d++) {
                int nx = cx + dx[d];
                int ny = cy + dy[d];
                if (nx >= 0 && nx < width && ny >= 0 && ny < height
                    && !visited[ny][nx] && !isWall(nx, ny)) {
                    visited[ny][nx] = true;
                    parent[ny][nx] = {cx, cy};
                    q.push({nx, ny});
                }
            }
        }

        if (!found) return;

        // Reconstruct path by walking back from goal to start
        std::pair<int,int> cur = goal;
        while (cur != std::pair<int,int>{-1, -1}) {
            solutionPath.push_back(cur);
            auto [px, py] = parent[cur.second][cur.first];
            if (cur == start) break;
            cur = {px, py};
        }
        std::reverse(solutionPath.begin(), solutionPath.end());
    }

    const std::vector<std::pair<int,int>>& getSolutionPath() const {
        return solutionPath;
    }
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