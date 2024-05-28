#include "Scene_Menu.hpp"
#include "Action.hpp"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
    : Scene(gameEngine)
    , m_menuText(sf::Text())
    , m_selectedMenuIndex(0) {
    init();
}

void Scene_Menu::init() {
    m_title = "Slime Game";
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Enter, "PLAY");
    registerAction(sf::Keyboard::Escape, "QUIT");

}

void Scene_Menu::update() {
    m_currentFrame += 1;

}

void Scene_Menu::onEnd() {

}

void Scene_Menu::sDoAction(const Action& action) {
    if (action.getType() == "START") {
        if (action.getName() == "UP") {
            if (m_selectedMenuIndex > 0) m_selectedMenuIndex--;
            else {
                m_selectedMenuIndex = m_menuStrings.size() - 1;
            }
        } else if (action.getName() == "DOWN") {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
        } else if (action.getName() == "PLAY") {
            // change game engine scene to current index
            // m_game->changeScene("Play", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
        } else if (action.getName() == "QUIT") {
            onEnd();
        }
    } 
}

void Scene_Menu::sRender() {
    update();

    m_game->window().clear(sf::Color::Black);

    m_game->window().draw(m_menuText);

    m_game->window().display();
}
