#include "GameEngine.hpp"

int main() {
    GameEngine game = GameEngine("./bin/assets.txt");

    game.run();

    return 0;
}
