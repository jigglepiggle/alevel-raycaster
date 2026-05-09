#pragma once

#include <vector>
#include <stack>
#include "RandomNumberGenerator.h"

// Generates a perfect maze using iterative depth-first search (recursive backtracker).    // Skills Audit Group A: Stack/Queue Operations
class DepthFirstMazeGenerator {
private:
    int width, height;
    // 2-D grid; 1 = wall, 0 = passage, 2 = entrance/exit marker.            // Skills Audit Group A: Advanced matrix operations
    std::vector<std::vector<int>> maze;
    RandomNumberGenerator rng;

    // Cardinal direction offsets for grid traversal (N, E, S, W).
    const int dx[4] = {0, 1, 0, -1};
    const int dy[4] = {-1, 0, 1, 0};

    // Returns true if (x, y) is within the grid bounds.
    bool isValid(int x, int y);
    // Returns true if the cell is inside the grid and still marked as wall (unvisited).
    bool isUnvisited(int x, int y);
    // Returns a shuffled list of direction indices {0,1,2,3} for random neighbour selection.
    std::vector<int> getRandomDirections();

public:
    // Constructs the generator with an optional seed; seed 0 = time-based random.
    DepthFirstMazeGenerator(int w, int h, unsigned int seed = 0);
    // Alternate constructor that shares an existing RNG instance.
    DepthFirstMazeGenerator(int w, int h, RandomNumberGenerator& externalRng);

    // Runs the maze generation algorithm and populates the internal grid.
    void generateMaze();
    // Prints the maze to stdout using '#' for walls and ' ' for passages.
    void printMaze();
    // Prints the maze as a C integer array literal to stdout.
    void printMazeAsArray();

    // Returns the maze as a flat 1-D vector (row-major order).
    std::vector<int> getMazeAsVector();
    int getWidth() const;
    int getHeight() const;
    // Returns a const reference to the internal 2-D grid.
    const std::vector<std::vector<int>>& getMaze() const;

    // Reseeds the internal RNG without regenerating the maze.
    void reseedRNG(unsigned int seed);
};
