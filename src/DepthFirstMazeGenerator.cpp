#include "DepthFirstMazeGenerator.h"
#include <iostream>

bool DepthFirstMazeGenerator::isValid(int x, int y) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool DepthFirstMazeGenerator::isUnvisited(int x, int y) {
    return isValid(x, y) && maze[y][x] == 1;
}

std::vector<int> DepthFirstMazeGenerator::getRandomDirections() {
    std::vector<int> directions = {0, 1, 2, 3};
    rng.shuffle(directions);
    return directions;
}

DepthFirstMazeGenerator::DepthFirstMazeGenerator(int w, int h, unsigned int seed)
    : width(w), height(h) {
    rng = (seed == 0) ? RandomNumberGenerator() : RandomNumberGenerator(seed);
    maze.assign(height, std::vector<int>(width, 1));
}

DepthFirstMazeGenerator::DepthFirstMazeGenerator(int w, int h, RandomNumberGenerator& externalRng)
    : width(w), height(h), rng(externalRng) {
    maze.assign(height, std::vector<int>(width, 1));
}

void DepthFirstMazeGenerator::generateMaze() {
    std::stack<std::pair<int, int>> stack;
    maze[1][1] = 0;
    stack.push({1, 1});

    while (!stack.empty()) {
        auto [currentX, currentY] = stack.top();

        std::vector<std::pair<int, int>> neighbors;
        for (int dir : getRandomDirections()) {
            int newX = currentX + dx[dir] * 2;
            int newY = currentY + dy[dir] * 2;
            if (isUnvisited(newX, newY))
                neighbors.push_back({newX, newY});
        }

        if (!neighbors.empty()) {
            auto [nextX, nextY] = neighbors[0];
            maze[(currentY + nextY) / 2][(currentX + nextX) / 2] = 0;
            maze[nextY][nextX] = 0;
            stack.push({nextX, nextY});
        } else {
            stack.pop();
        }
    }

    maze[1][0]            = 2;
    maze[height-2][width-1] = 2;
}

void DepthFirstMazeGenerator::printMaze() {
    for (const auto& row : maze) {
        for (int cell : row)
            std::cout << (cell == 1 ? '#' : ' ');
        std::cout << '\n';
    }
}

void DepthFirstMazeGenerator::printMazeAsArray() {
    std::cout << "int map[] = {\n";
    for (int y = 0; y < height; y++) {
        std::cout << "    ";
        for (int x = 0; x < width; x++) {
            std::cout << maze[y][x];
            if (y < height - 1 || x < width - 1)
                std::cout << ",";
        }
        std::cout << "\n";
    }
    std::cout << "};\n";
}

std::vector<int> DepthFirstMazeGenerator::getMazeAsVector() {
    std::vector<int> flatMaze;
    flatMaze.reserve(width * height);
    for (const auto& row : maze)
        for (int cell : row)
            flatMaze.push_back(cell);
    return flatMaze;
}

int DepthFirstMazeGenerator::getWidth()  const { return width;  }
int DepthFirstMazeGenerator::getHeight() const { return height; }

const std::vector<std::vector<int>>& DepthFirstMazeGenerator::getMaze() const {
    return maze;
}

void DepthFirstMazeGenerator::reseedRNG(unsigned int seed) {
    rng.setSeed(seed);
}
