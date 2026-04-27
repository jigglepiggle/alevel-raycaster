#pragma once

#include <vector>
#include <stack>
#include "RandomNumberGenerator.h"

class DepthFirstMazeGenerator {
private:
    int width, height;
    std::vector<std::vector<int>> maze;
    RandomNumberGenerator rng;

    const int dx[4] = {0, 1, 0, -1};
    const int dy[4] = {-1, 0, 1, 0};

    bool isValid(int x, int y);
    bool isUnvisited(int x, int y);
    std::vector<int> getRandomDirections();

public:
    DepthFirstMazeGenerator(int w, int h, unsigned int seed = 0);
    DepthFirstMazeGenerator(int w, int h, RandomNumberGenerator& externalRng);

    void generateMaze();
    void printMaze();
    void printMazeAsArray();

    std::vector<int> getMazeAsVector();
    int getWidth() const;
    int getHeight() const;
    const std::vector<std::vector<int>>& getMaze() const;

    void reseedRNG(unsigned int seed);
};
