#include "WorldMap.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <math.h>

WorldMap::WorldMap(const std::vector<std::vector<int>>& mapData, int height, int width) 
                : mapData(mapData), height(height), width(width) {}

const std::vector<std::vector<int>>& WorldMap::getMap() const { return mapData; }

bool WorldMap::isWall(int x, int y) const { 
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return true; // Treat out-of-bounds as walls
    }
    return mapData[y][x] == 1; 
}

int WorldMap::getWallType(int x, int y) const { 
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return 1; // Default wall type
    }
    return mapData[y][x]; 
}

int WorldMap::getWidth()  const { return width;  }
int WorldMap::getHeight() const { return height; }
const std::vector<std::pair<int,int>>& WorldMap::getSolutionPath() const { return solutionPath; }

void WorldMap::solveMaze() {
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
