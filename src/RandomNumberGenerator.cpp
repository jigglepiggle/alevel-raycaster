#include <ctime>
#include <random>
#include <stdexcept>
#include "RandomNumberGenerator.h"

RandomNumberGenerator::RandomNumberGenerator() {
    generator.seed(std::time(nullptr));
}

RandomNumberGenerator::RandomNumberGenerator(unsigned int seed) {
    generator.seed(seed);
}

int RandomNumberGenerator::randomInt(int min, int max) {
    if (min > max) throw std::invalid_argument("min cannot be greater than max");
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator);
}

int RandomNumberGenerator::randomOdd(int min, int max) {
    if (min % 2 == 0) min++;
    if (max % 2 == 0) max--;
    if (min > max) return min;

    int numOdds = (max - min) / 2 + 1;
    std::uniform_int_distribution<int> dist(0, numOdds - 1);
    return min + (dist(generator) * 2);
}

int RandomNumberGenerator::randomEven(int min, int max) {
    if (min % 2 == 1) min++;
    if (max % 2 == 1) max--;
    if (min > max) return min;

    int numEvens = (max - min) / 2 + 1;
    std::uniform_int_distribution<int> dist(0, numEvens - 1);
    return min + (dist(generator) * 2);
}

bool RandomNumberGenerator::randomBoolean() {
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(generator) == 1;
}

void RandomNumberGenerator::setSeed(unsigned int seed) {
    generator.seed(seed);
}
