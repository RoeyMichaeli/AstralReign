#ifndef RANDOM_UTILS_HPP
#define RANDOM_UTILS_HPP

#include <SFML/Graphics/Color.hpp>

namespace RandomUtils {

    // Returns a random integer in the inclusive range [min, max]
    int randomInt(int min, int max);

    // Returns a random float in the range [min, max)
    float randomFloat(float min, float max);

    // Returns a random angle in degrees in the range [0, 360)
    float randomAngle();

    // Returns a random SFML color with each component in the range [0, 255]
    sf::Color randomColor();
}

#endif // RANDOM_UTILS_HPP
