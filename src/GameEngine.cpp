#include "GameEngine.hpp"
#include <fstream>
#include <iostream>

GameEngine::GameEngine(const std::string& path)
    : m_assets()
{
    init(path); 
}

void GameEngine::init(const std::string& path) {
    m_window.create(sf::VideoMode(800, 600), "Slime Game");
    m_window.setFramerateLimit(60);

    std::ifstream fin(path);


    while (!fin.eof()) {
        std::string asset_type;
        fin >> asset_type; 

        if (asset_type == "Texture") {
            std::string N, P;
            fin >> N >> P;
            m_assets.addTexture(N, P);
        } else if (asset_type == "Animation") {
            std::string N, T;
            size_t      F, S;
            fin >> N >> T >> F >> S;
            m_assets.addAnimation(N, Animation(N, m_assets.getTexture(T), F, S));
        } else if (asset_type == "Font") {
            std::string N, P;
            fin >> N >> P;
            m_assets.addFont(N, P);
        } else {
            fin >> asset_type;
        }
    }
}

void GameEngine::update() {

}

void GameEngine::sUserInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            quit();
        }
    }
}

void GameEngine::run() {
    sf::Event event;

    while (m_window.isOpen()) {
        update();

        // Systems
        if (m_running) {
        }
        sUserInput();
    }
}

void GameEngine::quit() {
    m_window.close();
    m_running = false;
}

sf::RenderWindow& GameEngine::window() {
    return m_window;
}

bool GameEngine::isRunning() {
    return m_running;
}
