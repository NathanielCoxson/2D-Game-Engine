#include "Scene_Menu.hpp"
#include "Scene_Play.hpp"
#include "Action.hpp"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
    : Scene(gameEngine)
    , m_menuText(sf::Text())
    , m_selectedMenuIndex(0)
    , m_menuStrings({ "Level 1", "Level 2", "Level 3" }){
    init();
}

void Scene_Menu::init() {
    m_title = "Slime Game";
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Enter, "PLAY");
    registerAction(sf::Keyboard::Escape, "QUIT");

    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setCharacterSize(24);
    m_menuText.setFont(m_game->assets().getFont("RobotoRegular"));

    m_levelPaths.push_back("bin/level_1.txt");
    m_levelPaths.push_back("bin/level_2.txt");
}

void Scene_Menu::update() {
    m_currentFrame += 1;
}

void Scene_Menu::onEnd() {
    m_game->quit();
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
            m_game->changeScene("level_1", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
        } else if (action.getName() == "QUIT") {
            onEnd();
        }
    } 
}

void Scene_Menu::sRender() {
    update();

    m_game->window().clear(sf::Color::Black);

    m_menuText.setString(m_title);
    m_menuText.setPosition(sf::Vector2f(10, 10));
    m_game->window().draw(m_menuText);

    m_menuText.setString("Level 1");
    m_menuText.setPosition(sf::Vector2f(10, 44));
    if (m_selectedMenuIndex == 0) m_menuText.setFillColor(sf::Color::Green);
    else m_menuText.setFillColor(sf::Color::White);
    m_game->window().draw(m_menuText);

    m_menuText.setString("Level 2");
    m_menuText.setPosition(sf::Vector2f(10, 78));
    if (m_selectedMenuIndex == 1) m_menuText.setFillColor(sf::Color::Green);
    else m_menuText.setFillColor(sf::Color::White);
    m_game->window().draw(m_menuText);

    m_menuText.setString("Level 3");
    m_menuText.setPosition(sf::Vector2f(10, 112));
    if (m_selectedMenuIndex == 2) m_menuText.setFillColor(sf::Color::Green);
    else m_menuText.setFillColor(sf::Color::White);
    m_game->window().draw(m_menuText);

    m_menuText.setString("UP:W\tDOWN:S\tPLAY:ENTER\tBACK:ESC");
    m_menuText.setPosition(sf::Vector2f(10, height() - 34));
    m_menuText.setFillColor(sf::Color::White);
    m_game->window().draw(m_menuText);

    m_game->window().display();
}
