#pragma once

#include "Assets.hpp"
#include "Scene.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class Scene;

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine {

protected:
    sf::RenderWindow m_window;
    Assets           m_assets;
    std::string      m_currentScene;
    SceneMap         m_sceneMap;
    size_t           m_simulationSpeed = 1;
    bool             m_running = true;

    void init(const std::string& path);
    void update();

    void sUserInput();

    std::shared_ptr<Scene> currentScene();

public:
    GameEngine(const std::string& path);

    void changeScene(
        const std::string& sceneName,
        std::shared_ptr<Scene> scene,
        bool endCurrentScene = false
    );

    void run();
    void quit();

    sf::RenderWindow& window();
    Assets& assets();
    bool isRunning();
};
