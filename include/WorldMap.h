#pragma once
#include <vector>
#include <math.h>

class WorldMap {
private:
    int height;
    int width;

    std::vector<std::vector<int>> mapData;
    std::vector<std::pair<int,int>> solutionPath;

public:
    explicit WorldMap(const std::vector<std::vector<int>>& mapData, int height, int width);

    const std::vector<std::vector<int>>& getMap() const;

    bool isWall(int x, int y) const;
    int getWallType(int x, int y) const;
    int getWidth() const;
    int getHeight() const;
    void solveMaze();
    const std::vector<std::pair<int,int>>& getSolutionPath() const;
};
