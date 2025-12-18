#pragma once

#include <vector>
#include <stack>
#include "randomNumberGenerator.h"

/**
 * @brief Maze generator using Depth-First Search algorithm with backtracking
 * 
 * This class implements a maze generation algorithm based on depth-first search
 * with recursive backtracking. The algorithm creates mazes with a tree-like structure,
 * resulting in long winding paths with relatively few loops.
 * 
 * Algorithm Overview:
 * 1. Start with a grid filled with walls
 * 2. Choose a starting cell and mark it as visited
 * 3. While there are unvisited cells:
 *    a. If current cell has unvisited neighbors, choose one randomly
 *    b. Remove wall between current cell and chosen neighbor
 *    c. Make chosen neighbor the current cell (push to stack)
 *    d. If no unvisited neighbors, backtrack (pop from stack)
 * 
 * Characteristics of generated mazes:
 * - Guaranteed to have exactly one path between any two points (no loops)
 * - Long, winding corridors
 * - Relatively few dead ends
 * - Tree-like structure
 */
class DepthFirstMazeGenerator {
private:
    // Maze dimensions - should be odd numbers for proper wall/path structure
    int width, height;
    
    // 2D maze representation: 1 = wall, 0 = path
    std::vector<std::vector<int>> maze;
    
    // Random number generator for algorithm choices
    RandomNumberGenerator rng;
    
    /**
     * @brief Direction vectors for movement in 4 cardinal directions
     * 
     * These arrays define movement offsets for North, East, South, West directions:
     * - dx[0] = 0, dy[0] = -1 (North: no x change, y decreases)
     * - dx[1] = 1, dy[1] = 0  (East: x increases, no y change)
     * - dx[2] = 0, dy[2] = 1  (South: no x change, y increases)
     * - dx[3] = -1, dy[3] = 0 (West: x decreases, no y change)
     */
    const int dx[4] = {0, 1, 0, -1};
    const int dy[4] = {-1, 0, 1, 0};

    /**
     * @brief Check if coordinates are within maze bounds
     * 
     * @param x X-coordinate to check
     * @param y Y-coordinate to check
     * @return true if coordinates are valid, false otherwise
     */
    bool isValid(int x, int y);
    
    /**
     * @brief Check if a cell is unvisited (contains a wall)
     * 
     * In the context of maze generation, unvisited cells are represented
     * as walls (value 1). This function checks if a cell can be visited.
     * 
     * @param x X-coordinate of the cell
     * @param y Y-coordinate of the cell
     * @return true if cell is valid and unvisited, false otherwise
     */
    bool isUnvisited(int x, int y);
    
    /**
     * @brief Generate a random permutation of direction indices
     * 
     * Creates a vector containing {0, 1, 2, 3} representing the four
     * cardinal directions, then shuffles them to ensure random exploration
     * order during maze generation.
     * 
     * @return Vector of shuffled direction indices
     */
    std::vector<int> getRandomDirections();
    
public:
    /**
     * @brief Constructor with dimensions and optional seed
     * 
     * Creates a maze generator with specified dimensions. If no seed is provided
     * (seed = 0), the generator will be seeded with the current time.
     * 
     * @param w Width of the maze (should be odd for proper structure)
     * @param h Height of the maze (should be odd for proper structure)
     * @param seed Random seed for reproducible mazes (0 = use current time)
     */
    DepthFirstMazeGenerator(int w, int h, unsigned int seed = 0);
    
    /**
     * @brief Alternative constructor that uses an external RandomNumberGenerator
     * 
     * This constructor allows sharing the same RNG across multiple objects,
     * which can be useful for coordinated random behavior or testing.
     * 
     * @param w Width of the maze
     * @param h Height of the maze
     * @param externalRng Reference to an existing RandomNumberGenerator
     */
    DepthFirstMazeGenerator(int w, int h, RandomNumberGenerator& externalRng);
    
    /**
     * @brief Generate the maze using depth-first search algorithm
     * 
     * This is the main algorithm implementation:
     * 1. Initialize maze with all walls
     * 2. Start from position (1,1) and mark as path
     * 3. Use stack-based depth-first search with backtracking
     * 4. For each cell, randomly select an unvisited neighbor
     * 5. Remove wall between current cell and neighbor
     * 6. Continue until all reachable cells are visited
     * 7. Create entrance at (1,0) and exit at (height-2, width-1)
     */
    void generateMaze();
    
    /**
     * @brief Print maze to console using ASCII characters
     * 
     * Prints the maze using '#' for walls and ' ' (space) for paths.
     * This provides a visual representation of the generated maze.
     */
    void printMaze();
    
    /**
     * @brief Print maze as a C-style array declaration
     * 
     * Outputs the maze in a format suitable for copying into C/C++ code
     * as an integer array, with proper formatting and commas.
     */
    void printMazeAsArray();
    
    /**
     * @brief Get maze data as a flattened vector
     * 
     * Converts the 2D maze representation into a 1D vector for easier
     * integration with graphics libraries or other systems that expect
     * linear data arrays.
     * 
     * @return Vector containing maze data in row-major order
     */
    std::vector<int> getMazeAsVector();
    
    /**
     * @brief Get the width of the maze
     * @return Width of the maze
     */
    int getWidth() const;
    
    /**
     * @brief Get the height of the maze
     * @return Height of the maze
     */
    int getHeight() const;
    
    /**
     * @brief Get read-only access to the maze data structure
     * 
     * Returns a const reference to the internal 2D vector representing
     * the maze. This allows external code to access maze data without
     * making a copy and without risking modification.
     * 
     * @return Const reference to the 2D maze vector
     */
    const std::vector<std::vector<int>>& getMaze() const;
    
    /**
     * @brief Reseed the internal random number generator
     * 
     * Allows changing the seed after construction to generate different
     * maze variations without creating a new generator instance.
     * 
     * @param seed New seed value for the random number generator
     */
    void reseedRNG(unsigned int seed);
};