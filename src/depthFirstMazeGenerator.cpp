#pragma once

#include "depthFirstMazeGenerator.h"
#include <iostream>
#include <ctime>

/**
 * @brief Check if coordinates are within the maze boundaries
 * 
 * Validates that the given coordinates fall within the valid maze dimensions.
 * This is essential for preventing array bounds violations during maze generation.
 * 
 * @param x X-coordinate to validate
 * @param y Y-coordinate to validate
 * @return true if coordinates are within bounds [0, width) and [0, height)
 */
bool DepthFirstMazeGenerator::isValid(int x, int y) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

/**
 * @brief Check if a cell is unvisited and can be carved into
 * 
 * In the depth-first maze generation algorithm, unvisited cells are represented
 * as walls (value 1). This function checks if a cell is both valid (within bounds)
 * and unvisited (contains a wall that can be carved into a path).
 * 
 * @param x X-coordinate of the cell to check
 * @param y Y-coordinate of the cell to check
 * @return true if the cell is valid and unvisited (wall), false otherwise
 */
bool DepthFirstMazeGenerator::isUnvisited(int x, int y) {
    return isValid(x, y) && maze[y][x] == 1;
}

/**
 * @brief Generate a randomized list of direction indices
 * 
 * Creates a vector containing the four cardinal direction indices {0, 1, 2, 3}
 * and shuffles them randomly. This ensures that the maze generation explores
 * directions in a random order, creating varied and unpredictable maze layouts.
 * 
 * Direction mapping:
 * - 0: North (up)
 * - 1: East (right)
 * - 2: South (down)
 * - 3: West (left)
 * 
 * @return Vector of shuffled direction indices for random exploration
 */
std::vector<int> DepthFirstMazeGenerator::getRandomDirections() {
    std::vector<int> directions = {0, 1, 2, 3};  // All four directions
    rng.shuffle(directions);  // Randomize the order
    return directions;
}

/**
 * @brief Constructor with dimensions and optional seed
 * 
 * Initializes the maze generator with the specified dimensions and sets up
 * the random number generator. If seed is 0, uses current time for seeding.
 * The maze is initially filled with walls (1s).
 * 
 * @param w Width of the maze (preferably odd for proper wall/path structure)
 * @param h Height of the maze (preferably odd for proper wall/path structure)
 * @param seed Random seed (0 = auto-seed with current time)
 */
DepthFirstMazeGenerator::DepthFirstMazeGenerator(int w, int h, unsigned int seed) : width(w), height(h) {
    // Initialize random number generator with appropriate seed
    if (seed == 0) {
        rng = RandomNumberGenerator();  // Auto-seed with current time
    } else {
        rng = RandomNumberGenerator(seed);  // Use provided seed
    }
    
    // Initialize maze grid with all walls (1s)
    maze.assign(height, std::vector<int>(width, 1));
}

/**
 * @brief Alternative constructor using external RandomNumberGenerator
 * 
 * This constructor allows using a shared RandomNumberGenerator instance,
 * which can be useful for coordinated randomization across multiple maze
 * generators or for testing with controlled random sequences.
 * 
 * @param w Width of the maze
 * @param h Height of the maze  
 * @param externalRng Reference to an existing RandomNumberGenerator
 */
DepthFirstMazeGenerator::DepthFirstMazeGenerator(int w, int h, RandomNumberGenerator& externalRng) 
    : width(w), height(h), rng(externalRng) {
    // Initialize maze grid with all walls (1s)
    maze.assign(height, std::vector<int>(width, 1));
}

/**
 * @brief Generate maze using depth-first search with recursive backtracking
 * 
 * This is the core maze generation algorithm implementation:
 * 
 * Algorithm Steps:
 * 1. Start at position (1,1) and mark it as a path (0)
 * 2. Push starting position onto the stack
 * 3. While stack is not empty:
 *    a. Look at current position (top of stack)
 *    b. Find all unvisited neighbors that are 2 cells away
 *    c. If unvisited neighbors exist:
 *       - Randomly select one neighbor
 *       - Carve path to neighbor (remove wall between current and neighbor)
 *       - Mark neighbor as visited
 *       - Push neighbor onto stack
 *    d. If no unvisited neighbors, backtrack (pop from stack)
 * 4. Create entrance and exit points
 * 
 * Note: We move 2 cells at a time to maintain wall structure between paths.
 */
void DepthFirstMazeGenerator::generateMaze() {
    int startX = 1, startY = 1;  // Starting position (must be odd coordinates)
    std::stack<std::pair<int, int>> stack;  // Stack for backtracking
    
    // Mark starting cell as path and push onto stack
    maze[startY][startX] = 0;
    stack.push({startX, startY});
    
    // Continue until all reachable cells are processed
    while (!stack.empty()) {
        // Get current position from top of stack
        auto [currentX, currentY] = stack.top();
        
        // Find all unvisited neighbors (2 cells away in each direction)
        std::vector<std::pair<int, int>> neighbors;
        
        // Check each direction in random order
        for (int dir : getRandomDirections()) {
            // Calculate neighbor position (2 cells away to maintain wall structure)
            int newX = currentX + dx[dir] * 2;
            int newY = currentY + dy[dir] * 2;
            
            // If neighbor is unvisited, add to candidates
            if (isUnvisited(newX, newY)) {
                neighbors.push_back({newX, newY});
            }
        }
        
        // If we found unvisited neighbors
        if (!neighbors.empty()) {
            // Choose first neighbor (list is already randomized)
            auto [nextX, nextY] = neighbors[0];
            
            // Calculate wall position between current and next cell
            int wallX = currentX + (nextX - currentX) / 2;
            int wallY = currentY + (nextY - currentY) / 2;
            
            // Carve path: remove wall and mark neighbor as visited
            maze[wallY][wallX] = 0;  // Remove wall between cells
            maze[nextY][nextX] = 0;  // Mark neighbor as path
            
            // Push neighbor onto stack for further exploration
            stack.push({nextX, nextY});
        } else {
            // No unvisited neighbors, backtrack by popping from stack
            stack.pop();
        }
    }
    
    // Create entrance and exit points
    maze[1][0] = 2;              // Entrance on left side
    maze[height-2][width-1] = 2; // Exit on right side
}

/**
 * @brief Print the maze to console using ASCII characters
 * 
 * Displays the maze using '#' for walls and ' ' (space) for paths.
 * This provides an immediate visual representation of the generated maze
 * that's easy to read in a terminal or console window.
 */
void DepthFirstMazeGenerator::printMaze() {
    for (const auto& row : maze) {
        for (int cell : row) {
            // Print '#' for walls (1) and ' ' for paths (0)
            std::cout << (cell == 1 ? '#' : ' ');
        }
        std::cout << '\n';  // New line after each row
    }
}

/**
 * @brief Print maze as a C-style array declaration
 * 
 * Outputs the maze data in a format that can be directly copied and pasted
 * into C/C++ source code as an integer array declaration. This is useful
 * for embedding static mazes in programs or for testing purposes.
 * 
 * Output format:
 * int map[] = {
 *     1,1,1,1,1,
 *     1,0,0,0,1,
 *     ...
 * };
 */
void DepthFirstMazeGenerator::printMazeAsArray() {
    std::cout << "int map[] = {\n";
    
    for (int y = 0; y < height; y++) {
        std::cout << "    ";  // Indentation for readability
        
        for (int x = 0; x < width; x++) {
            std::cout << maze[y][x];
            
            // Add comma after each element except the last one
            if (y < height - 1 || x < width - 1) {
                std::cout << ",";
            }
        }
        std::cout << "\n";  // New line after each row
    }
    
    std::cout << "};\n";
}

/**
 * @brief Convert 2D maze to 1D vector in row-major order
 * 
 * Flattens the 2D maze representation into a single vector, storing
 * elements in row-major order (left-to-right, top-to-bottom).
 * This format is often required by graphics libraries or game engines
 * that expect linear arrays for texture or collision data.
 * 
 * @return Vector containing all maze cells in row-major order
 */
std::vector<int> DepthFirstMazeGenerator::getMazeAsVector() {
    std::vector<int> flatMaze;
    flatMaze.reserve(width * height);  // Pre-allocate for efficiency
    
    // Copy all cells from 2D maze to flat vector
    for (const auto& row : maze) {
        for (int cell : row) {
            flatMaze.push_back(cell);
        }
    }
    
    return flatMaze;
}

/**
 * @brief Get the width of the maze
 * @return Width of the maze in cells
 */
int DepthFirstMazeGenerator::getWidth() const { 
    return width; 
}

/**
 * @brief Get the height of the maze
 * @return Height of the maze in cells
 */
int DepthFirstMazeGenerator::getHeight() const { 
    return height; 
}

/**
 * @brief Get read-only access to the internal maze structure
 * 
 * Returns a const reference to the 2D vector representing the maze.
 * This allows external code to access the maze data directly without
 * making copies, while preventing accidental modification.
 * 
 * @return Const reference to the 2D maze vector
 */
const std::vector<std::vector<int>>& DepthFirstMazeGenerator::getMaze() const { 
    return maze; 
}

/**
 * @brief Change the seed of the internal random number generator
 * 
 * Allows reseeding the RNG after construction, which can be useful
 * for generating multiple different mazes with the same generator
 * instance or for testing with specific seed values.
 * 
 * @param seed New seed value for the random number generator
 */
void DepthFirstMazeGenerator::reseedRNG(unsigned int seed) {
    rng.setSeed(seed);
}