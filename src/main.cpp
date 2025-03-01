#include "GameEngine.h"


int main() {
    GameEngine gameEngine("config/config.json");
    gameEngine.run();

    return 0;
}