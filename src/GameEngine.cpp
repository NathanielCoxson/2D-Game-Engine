#include "GameEngine.hpp"
#include "Scene_Menu.hpp"
#include "Scene_Play.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstdint>

GameEngine::GameEngine(const std::string& path)
    : m_assets()
    , m_currentScene("menu")
{
    init(path); 
}

void GameEngine::init(const std::string& path) {
    // TODO: move this into a function or use a namespace to improve readability
    m_currTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_prevTime = m_currTime;

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
        } else if (asset_type == "Sound") {
            std::string N, P;
            fin >> N >> P;
            m_assets.addSoundBuffer(N, P);
        } else {
            fin >> asset_type;
        }
    }

    m_window.create(sf::VideoMode(800, 600), "Slime Game");
    m_window.setFramerateLimit(60);

    changeScene("menu", std::make_shared<Scene_Menu>(this));
}

void GameEngine::update() {
    m_prevTime = m_currTime; 
    m_currTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_deltaTime = m_currTime - m_prevTime;
}

void GameEngine::sUserInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            quit();
        }

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) {
                continue;
            }

            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

            currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }
    }
}

std::shared_ptr<Scene> GameEngine::currentScene() {
    return m_sceneMap[m_currentScene];
}

void GameEngine::changeScene(
    const std::string& sceneName,
    std::shared_ptr<Scene> scene,
    bool endCurrentScene
) {
    if (endCurrentScene) {
        m_sceneMap.erase(m_currentScene);
    }

    m_currentScene = sceneName;
    if (!m_sceneMap.count(sceneName)) {
        m_sceneMap[m_currentScene] = scene;
    }
}

void GameEngine::run() {
    sf::Event event;

    while (m_window.isOpen()) {
        sUserInput();

        update();

        // Systems
        if (m_running) {
            m_sceneMap[m_currentScene]->sRender(); 
        }

    }
}

void GameEngine::quit() {
    m_window.close();
    m_running = false;
}

sf::RenderWindow& GameEngine::window() {
    return m_window;
}

Assets& GameEngine::assets() {
    return m_assets;
}

bool GameEngine::isRunning() {
    return m_running;
}
