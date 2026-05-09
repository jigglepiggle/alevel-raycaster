#pragma once
#include <vector>
#include <math.h>

// Wraps the 2-D maze grid and provides spatial queries and BFS pathfinding.    // Skills Audit Group A: Graph/Tree Traversal
class WorldMap {
private:
    int height;
    int width;

    // The raw tile grid: 0 = open, 1 = wall, 2 = entrance/exit.    // Skills Audit Group A: Advanced matrix operations
    std::vector<std::vector<int>> mapData;
    // Cached BFS solution path stored as a list of (x, y) tile coordinates.
    std::vector<std::pair<int,int>> solutionPath;
    // Used internally by solveMaze; returns true if a cell can be traversed.
    bool isPassable(int x, int y) const;

public:
    explicit WorldMap(const std::vector<std::vector<int>>& mapData, int height, int width);

    const std::vector<std::vector<int>>& getMap() const;

    // Returns true if (x, y) is outside the map or contains a non-zero tile.
    bool isWall(int x, int y) const;
    // Returns the tile value (wall type) at (x, y), or 1 if out of bounds.
    int getWallType(int x, int y) const;
    int getWidth() const;
    int getHeight() const;
    // Runs BFS from the entrance to the exit and caches the shortest path.    // Skills Audit Group A: Graph/Tree Traversal
    void solveMaze();
    // Returns the cached solution path computed by solveMaze.
    const std::vector<std::pair<int,int>>& getSolutionPath() const;
};
