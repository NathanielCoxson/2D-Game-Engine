#include "GameEngine.hpp"

GameEngine::GameEngine()
    : m_window(sf::VideoMode(800, 600), "Slime Game")
{}

sf::RenderWindow& GameEngine::window() {
    return m_window;
}
