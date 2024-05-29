#pragma once

#include "Assets.hpp"
#include "Scene.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class GameEngine {
    sf::RenderWindow             m_window;
    Assets                       m_assets;
    std::string                  m_currentScene;
    //std::map<std::string, Scene> m_sceneMap;
    size_t                       m_simulationSpeed = 1;
    bool                         m_running = true;

    void init(const std::string& path);
    void update();

    void sUserInput();

    //std::shared_ptr<Scene> currentScene();

public:
    GameEngine(const std::string& path);

    //void changeScene(
    //    const std::string& sceneName,
    //    std::shared_ptr<Scene> scene,
    //    bool endCurrentScene = false
    //);

    void run();
    void quit();

    sf::RenderWindow& window();
    const Assets& assets() const;
    bool isRunning();
};
