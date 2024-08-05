#pragma once

#include "Action.hpp"
#include "Scene.hpp"
#include <SFML/Graphics.hpp>

class Scene_Menu : public Scene {

protected:
    std::string              m_title;
    std::vector<std::string> m_menuStrings;
    std::vector<std::string> m_levelPaths;
    sf::Text                 m_menuText;
    sf::Color                m_backgroundColor = sf::Color(100, 100, 255, 255);
    int                      m_selectedMenuIndex;

    void init();
    void update();
    void onEnd();
    void sDoAction(const Action& action);

public:
    
    Scene_Menu(GameEngine* gameEngine = nullptr);

    // Systems
    void sRender();
};
