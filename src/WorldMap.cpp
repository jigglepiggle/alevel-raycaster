#include "WorldMap.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <math.h>

// Store the map grid and dimensions for later spatial queries.
WorldMap::WorldMap(const std::vector<std::vector<int>>& mapData, int height, int width)
    : mapData(mapData), height(height), width(width) {}    // Skills Audit Group B: Multi-dimensional arrays (storing the 2-D map grid as a member)

const std::vector<std::vector<int>>& WorldMap::getMap() const { return mapData; }    // Skills Audit Group B: Multi-dimensional arrays (returning reference to 2-D grid)

// Returns true if (x, y) is outside the map or contains any non-zero tile value.
bool WorldMap::isWall(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return true;
    return mapData[y][x] != 0;    // Skills Audit Group B: Multi-dimensional arrays (2-D grid cell lookup)
}

// A passable cell is inside the map and has a tile value other than 1 (solid wall).
bool WorldMap::isPassable(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return false;
    return mapData[y][x] != 1;    // Skills Audit Group B: Multi-dimensional arrays (2-D grid cell lookup for passability)
}

// Returns the raw tile integer at (x, y); out-of-bounds defaults to 1 (wall).
int WorldMap::getWallType(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return 1;
    return mapData[y][x];    // Skills Audit Group B: Multi-dimensional arrays (reading tile type from 2-D grid)
}

int WorldMap::getWidth()  const { return width;  }
int WorldMap::getHeight() const { return height; }

const std::vector<std::pair<int,int>>& WorldMap::getSolutionPath() const { return solutionPath; }

// BFS pathfinding from the entrance tile to the exit tile.    // Skills Audit Group A: Graph/Tree Traversal
void WorldMap::solveMaze() {
    solutionPath.clear();

    // Entrance is the left border opening at row 1; exit is the right border opening at row height-2.
    std::pair<int,int> start = {0, 1};    // Skills Audit Group B: Records (pair used as a coordinate record)
    std::pair<int,int> goal  = {width - 1, height - 2};

    // Visited grid and parent grid track which cells have been enqueued and how we got there.
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));    // Skills Audit Group B: Multi-dimensional arrays (2-D visited flags grid)
    std::vector<std::vector<std::pair<int,int>>> parent(    // Skills Audit Group A: Advanced matrix operations (2-D parent pointer grid for path reconstruction)
        height, std::vector<std::pair<int,int>>(width, {-1, -1}));

    // Use a queue for FIFO expansion, guaranteeing the shortest path is found first.    // Skills Audit Group A: Stack/Queue Operations
    std::queue<std::pair<int,int>> q;
    q.push(start);
    visited[start.second][start.first] = true;    // Skills Audit Group B: Multi-dimensional arrays (marking start cell in 2-D visited grid)

    // Four cardinal directions for BFS expansion.
    const int dx[4] = {0, 1, 0, -1};    // Skills Audit Group B: Simple user defined algorithms (direction offset arrays for 4-connected BFS)
    const int dy[4] = {-1, 0, 1, 0};

    bool found = false;
    while (!q.empty() && !found) {
        auto [cx, cy] = q.front();
        q.pop();    // Skills Audit Group A: Stack/Queue Operations (dequeue front cell for BFS expansion)

        // Check if we have reached the exit.
        if (cx == goal.first && cy == goal.second) {
            found = true;
            break;
        }

        // Enqueue all unvisited passable neighbours.
        for (int d = 0; d < 4; d++) {
            int nx = cx + dx[d];    // Skills Audit Group B: Simple scientific/mathematical model (neighbour coordinate from direction offset)
            int ny = cy + dy[d];
            if (!visited[ny][nx] && isPassable(nx, ny)) {
                visited[ny][nx] = true;    // Skills Audit Group B: Multi-dimensional arrays (marking neighbour visited in 2-D grid)
                parent[ny][nx] = {cx, cy};    // Skills Audit Group A: Advanced matrix operations (recording parent in 2-D parent grid)
                q.push({nx, ny});    // Skills Audit Group A: Stack/Queue Operations (enqueue neighbour for BFS)
            }
        }
    }

    if (!found) return;

    // Walk back through the parent pointers from the goal to reconstruct the path.
    std::pair<int,int> cur = goal;
    while (cur != std::pair<int,int>{-1, -1}) {
        solutionPath.push_back(cur);    // Skills Audit Group B: Simple user defined algorithms (building path by back-tracking parent pointers)
        auto [px, py] = parent[cur.second][cur.first];    // Skills Audit Group A: Advanced matrix operations (reading parent from 2-D grid)
        if (cur == start) break;
        cur = {px, py};
    }
    // Reverse so the path runs from start to goal.
    std::reverse(solutionPath.begin(), solutionPath.end());    // Skills Audit Group B: Simple user defined algorithms (reversing back-tracked path to get start-to-goal order)
}
