#include <random>
#include <cstdlib>
#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include "RandomUtils.hpp"

// Random Number Generation in 
namespace RandomUtils {

    static std::mt19937 engine{ std::random_device{}() };

    // Returns a random integer in the inclusive range [min, max]
    int randomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine);
    }

    // Return a random float in [min, max)
    float randomFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine);
    }

    // Returns a random angle in [0, 360)
    float randomAngle() {
        return randomFloat(0.0f, 360.0f);
    }

    // Returns a random SFML color with each component in the range [0, 255].
    sf::Color randomColor() {
        return sf::Color(
            static_cast<uint8_t>(randomInt(0, 255)),
            static_cast<uint8_t>(randomInt(0, 255)),
            static_cast<uint8_t>(randomInt(0, 255))
        );
    }
}
