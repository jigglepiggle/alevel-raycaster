#include <ctime>
#include <random>
#include <stdexcept>
#include "RandomNumberGenerator.h"

// Seed from system time so every run produces a different sequence.
RandomNumberGenerator::RandomNumberGenerator() {
    generator.seed(std::time(nullptr));    // Skills Audit Group B: Simple user defined algorithms (time-based non-deterministic seeding)
}

// Seed with a fixed value for reproducible results across runs.
RandomNumberGenerator::RandomNumberGenerator(unsigned int seed) {
    generator.seed(seed);    // Skills Audit Group B: Simple user defined algorithms (deterministic seeding for repeatable output)
}

// Returns a random integer uniformly distributed in [min, max].
int RandomNumberGenerator::randomInt(int min, int max) {
    if (min > max) throw std::invalid_argument("min cannot be greater than max");
    std::uniform_int_distribution<int> dist(min, max);    // Skills Audit Group B: Simple scientific/mathematical model (uniform integer distribution)
    return dist(generator);
}

// Adjusts min/max inward to the nearest odd value, then picks uniformly among odd values.
int RandomNumberGenerator::randomOdd(int min, int max) {
    if (min % 2 == 0) min++;    // Skills Audit Group B: Simple user defined algorithms (nudge to nearest odd boundary)
    if (max % 2 == 0) max--;
    if (min > max) return min;

    // Map the range of odd numbers to a contiguous index and pick one.
    int numOdds = (max - min) / 2 + 1;    // Skills Audit Group B: Simple scientific/mathematical model (count of odd values in range)
    std::uniform_int_distribution<int> dist(0, numOdds - 1);
    return min + (dist(generator) * 2);    // Skills Audit Group B: Simple scientific/mathematical model (index back to odd value)
}

// Same logic as randomOdd but biases to even values instead.
int RandomNumberGenerator::randomEven(int min, int max) {
    if (min % 2 == 1) min++;    // Skills Audit Group B: Simple user defined algorithms (nudge to nearest even boundary)
    if (max % 2 == 1) max--;
    if (min > max) return min;

    int numEvens = (max - min) / 2 + 1;    // Skills Audit Group B: Simple scientific/mathematical model (count of even values in range)
    std::uniform_int_distribution<int> dist(0, numEvens - 1);
    return min + (dist(generator) * 2);
}

// Returns true or false with equal probability.
bool RandomNumberGenerator::randomBoolean() {
    std::uniform_int_distribution<int> dist(0, 1);    // Skills Audit Group B: Simple scientific/mathematical model (50/50 Bernoulli trial)
    return dist(generator) == 1;
}

// Reseeds the engine in place — cheaper than constructing a new generator.
void RandomNumberGenerator::setSeed(unsigned int seed) {
    generator.seed(seed);    // Skills Audit Group B: Simple user defined algorithms (in-place re-seeding)
}
