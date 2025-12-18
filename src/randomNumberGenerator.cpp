#pragma once

#include <ctime>
#include <random>
#include <stdexcept>
#include "randomNumberGenerator.h"

/**
 * @brief Default constructor - automatically seeds with current time Creates a RandomNumberGenerator instance seeded with the current system time. This ensures different random sequences on each program execution. */ RandomNumberGenerator::RandomNumberGenerator() {
    int seed = std::time(nullptr);  // Get current time as seed
    generator.seed(seed);
}

/**
 * @brief Constructor with explicit seed for reproducible results
 * 
 * Creates a RandomNumberGenerator with a specific seed value.
 * Using the same seed will always produce the same sequence of random numbers,
 * which is useful for testing and debugging maze generation algorithms.
 * 
 * @param seed The seed value for the random number generator
 */
RandomNumberGenerator::RandomNumberGenerator(unsigned int seed) {
    generator.seed(seed);
}

/**
 * @brief Generate random integer within specified range (inclusive)
 * 
 * Uses std::uniform_int_distribution to ensure uniform distribution
 * across the specified range.
 * 
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return Random integer between min and max
 * @throws std::invalid_argument if min > max
 */
int RandomNumberGenerator::randomInt(int min, int max) {
    // Validate input parameters
    if (min > max) {
        throw std::invalid_argument("min cannot be greater than max");
    }

    // Create uniform distribution for the specified range
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator);
}

/**
 * @brief Generate random odd number within specified range (inclusive)
 * 
 * This function is particularly useful for maze generation algorithms where
 * odd coordinates are required for proper wall/path structure.
 * 
 * Algorithm:
 * 1. Adjust min to be odd (increment if even)
 * 2. Adjust max to be odd (decrement if even)
 * 3. Calculate number of odd numbers in adjusted range
 * 4. Generate random index and convert to odd number
 * 
 * @param min Minimum value (will be adjusted to odd if even)
 * @param max Maximum value (will be adjusted to odd if even)
 * @return Random odd integer within the adjusted range
 */
int RandomNumberGenerator::randomOdd(int min, int max) {
    // Ensure min is odd
    if (min % 2 == 0) min++;
    
    // Ensure max is odd
    if (max % 2 == 0) max--;
    
    // Handle edge case where no odd numbers exist in range after adjustment
    if (min > max) return min; 
    
    // Calculate number of odd numbers in range
    // Formula: (max - min) / 2 + 1
    // Example: between 1 and 7, there are (7-1)/2 + 1 = 4 odd numbers: 1,3,5,7
    int numOdds = (max - min) / 2 + 1;
    
    // Generate random index among odd numbers
    std::uniform_int_distribution<int> dist(0, numOdds - 1);
    
    // Convert index to actual odd number
    // min + (index * 2) gives us the index-th odd number starting from min
    return min + (dist(generator) * 2);
}

/**
 * @brief Generate random even number within specified range (inclusive)
 * 
 * Similar to randomOdd but for even numbers. Useful for maze generation
 * where even coordinates are required for passages or specific structures.
 * 
 * Algorithm:
 * 1. Adjust min to be even (increment if odd)
 * 2. Adjust max to be even (decrement if odd)
 * 3. Calculate number of even numbers in adjusted range
 * 4. Generate random index and convert to even number
 * 
 * @param min Minimum value (will be adjusted to even if odd)
 * @param max Maximum value (will be adjusted to even if odd)
 * @return Random even integer within the adjusted range
 */
int RandomNumberGenerator::randomEven(int min, int max) {
    // Ensure min is even
    if (min % 2 == 1) min++;
    
    // Ensure max is even
    if (max % 2 == 1) max--;
    
    // Handle edge case where no even numbers exist in range after adjustment
    if (min > max) return min; 
    
    // Calculate number of even numbers in range
    // Formula: (max - min) / 2 + 1
    // Example: between 2 and 8, there are (8-2)/2 + 1 = 4 even numbers: 2,4,6,8
    int numEvens = (max - min) / 2 + 1;
    
    // Generate random index among even numbers
    std::uniform_int_distribution<int> dist(0, numEvens - 1);
    
    // Convert index to actual even number
    // min + (index * 2) gives us the index-th even number starting from min
    return min + (dist(generator) * 2);
}

/**
 * @brief Generate random boolean value with 50/50 probability
 * 
 * Uses uniform integer distribution between 0 and 1,
 * returning true if 1 is generated, false if 0 is generated.
 * 
 * @return true or false with equal probability
 */
bool RandomNumberGenerator::randomBoolean() {
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(generator) == 1;
}

/**
 * @brief Reset the random number generator with a new seed
 * 
 * This allows reseeding the generator during runtime to create
 * different random sequences or to restart with a known seed
 * for reproducible results.
 * 
 * @param seed The new seed value for the generator
 */
void RandomNumberGenerator::setSeed(unsigned int seed) {
    generator.seed(seed);
} 
