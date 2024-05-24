#pragma once

#include <SFML/Graphics.hpp>

class GameEngine {
    sf::RenderWindow m_window;

public:
    GameEngine();

    sf::RenderWindow& window();
};
