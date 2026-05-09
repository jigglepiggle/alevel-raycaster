#include "DepthFirstMazeGenerator.h"
#include <iostream>

// Returns true if (x, y) is within the grid — prevents out-of-bounds access.
bool DepthFirstMazeGenerator::isValid(int x, int y) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

// A cell is unvisited if it is valid and still holds its initial wall value of 1.
bool DepthFirstMazeGenerator::isUnvisited(int x, int y) {
    return isValid(x, y) && maze[y][x] == 1;    // Skills Audit Group B: Multi-dimensional arrays (2-D grid row/column indexing)
}

// Shuffles the four direction indices so neighbours are explored in random order.
std::vector<int> DepthFirstMazeGenerator::getRandomDirections() {
    std::vector<int> directions = {0, 1, 2, 3};
    rng.shuffle(directions);    // Skills Audit Group B: Simple user defined algorithms (randomised direction selection)
    return directions;
}

// Seed 0 triggers a time-based seed for a different maze each run; any other value is deterministic.
DepthFirstMazeGenerator::DepthFirstMazeGenerator(int w, int h, unsigned int seed)
    : width(w), height(h) {
    rng = (seed == 0) ? RandomNumberGenerator() : RandomNumberGenerator(seed);    // Skills Audit Group B: Generation of objects based on simple OOP model
    // Initialise every cell to 1 (wall) before carving passages.
    maze.assign(height, std::vector<int>(width, 1));    // Skills Audit Group B: Multi-dimensional arrays (2-D grid allocation)
}

// Constructor that shares an externally constructed RNG for coordinated seeding.
DepthFirstMazeGenerator::DepthFirstMazeGenerator(int w, int h, RandomNumberGenerator& externalRng)
    : width(w), height(h), rng(externalRng) {
    maze.assign(height, std::vector<int>(width, 1));    // Skills Audit Group B: Multi-dimensional arrays (2-D grid allocation)
}

void DepthFirstMazeGenerator::generateMaze() {
    // Iterative DFS using an explicit stack; avoids call-stack overflow on large mazes.    // Skills Audit Group A: Stack/Queue Operations
    std::stack<std::pair<int, int>> stack;
    // Start carving from (1,1) — the first interior cell.
    maze[1][1] = 0;
    stack.push({1, 1});

    while (!stack.empty()) {
        auto [currentX, currentY] = stack.top();

        // Check all four directions in random order for unvisited neighbours two steps away.
        std::vector<std::pair<int, int>> neighbors;
        for (int dir : getRandomDirections()) {
            int newX = currentX + dx[dir] * 2;
            int newY = currentY + dy[dir] * 2;
            if (isUnvisited(newX, newY))
                neighbors.push_back({newX, newY});
        }

        if (!neighbors.empty()) {
            // Carve through the wall between the current cell and the chosen neighbour.
            auto [nextX, nextY] = neighbors[0];
            maze[(currentY + nextY) / 2][(currentX + nextX) / 2] = 0;    // Skills Audit Group B: Multi-dimensional arrays (writing to a 2-D grid cell)
            maze[nextY][nextX] = 0;
            stack.push({nextX, nextY});    // Skills Audit Group A: Stack/Queue Operations
        } else {
            // No unvisited neighbours — backtrack by popping the stack.
            stack.pop();    // Skills Audit Group A: Stack/Queue Operations
        }
    }

    // Mark the entrance (left side, row 1) and exit (right side, second-to-last row) with value 2.
    maze[1][0]              = 2;    // Skills Audit Group B: Multi-dimensional arrays (writing boundary marker cells)
    maze[height-2][width-1] = 2;
}

// Prints a human-readable ASCII representation of the maze for debugging.
void DepthFirstMazeGenerator::printMaze() {
    for (const auto& row : maze) {    // Skills Audit Group B: Multi-dimensional arrays (iterating rows of the 2-D grid)
        for (int cell : row)
            std::cout << (cell == 1 ? '#' : ' ');
        std::cout << '\n';
    }
}

// Prints the maze as a C-style integer array literal, useful for embedding in source code.
void DepthFirstMazeGenerator::printMazeAsArray() {
    std::cout << "int map[] = {\n";
    for (int y = 0; y < height; y++) {
        std::cout << "    ";
        for (int x = 0; x < width; x++) {    // Skills Audit Group B: Multi-dimensional arrays (sequential row-major traversal)
            std::cout << maze[y][x];
            if (y < height - 1 || x < width - 1)
                std::cout << ",";
        }
        std::cout << "\n";
    }
    std::cout << "};\n";
}

// Flattens the 2-D grid to a 1-D vector in row-major order for easy transfer.
std::vector<int> DepthFirstMazeGenerator::getMazeAsVector() {
    std::vector<int> flatMaze;
    // Reserve space up front to avoid repeated reallocations.
    flatMaze.reserve(width * height);    // Skills Audit Group B: Simple user defined algorithms (pre-allocation by calculated size)
    for (const auto& row : maze)
        for (int cell : row)
            flatMaze.push_back(cell);    // Skills Audit Group A: Advanced matrix operations (flattening 2-D to 1-D)
    return flatMaze;
}

int DepthFirstMazeGenerator::getWidth()  const { return width;  }
int DepthFirstMazeGenerator::getHeight() const { return height; }

const std::vector<std::vector<int>>& DepthFirstMazeGenerator::getMaze() const {
    return maze;
}

// Allows the caller to produce a different maze layout from the same generator object.
void DepthFirstMazeGenerator::reseedRNG(unsigned int seed) {
    rng.setSeed(seed);    // Skills Audit Group B: Simple user defined algorithms (resetting PRNG state)
}
