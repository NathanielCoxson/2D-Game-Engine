#include "Scene_Menu.hpp"
#include "Scene_Play.hpp"
#include "Action.hpp"
#include <fstream>
#include <iostream>

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
    : Scene(gameEngine)
    , m_menuText(sf::Text())
    , m_selectedMenuIndex(0)
    , m_menuStrings({ "Level 1", "Level 2", "Level 3", "Replay" }){
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
    m_menuText.setFont(m_game->assets().getFont("PrimaryFont"));

    m_levelPaths.push_back("bin/level_1.txt");
    m_levelPaths.push_back("bin/level_2.txt");
    m_levelPaths.push_back("bin/level_3.txt");
    m_levelPaths.push_back("bin/level_1.txt");
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
            std::ifstream replay_file("replays/replay.txt");
            std::string replay_level_path;
            if (replay_file.is_open()) {
                replay_file >> replay_level_path;
            }
            replay_file.close();

            // change game engine scene to current index
            if (m_selectedMenuIndex == m_levelPaths.size() - 1) {
                m_game->changeScene("level", std::make_shared<Scene_Play>(m_game, replay_level_path, true));
            } else {
                m_game->changeScene("level", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
            }
        } else if (action.getName() == "QUIT") { 
            onEnd();
        }
    } 
}

void Scene_Menu::sRender() {
    update();

    m_game->window().clear(m_backgroundColor);

    // Draw title
    m_menuText.setString(m_title);
    m_menuText.setPosition(sf::Vector2f(10, 10));
    m_game->window().draw(m_menuText);

    // Draw level options
    for (int i = 0; i < m_menuStrings.size(); i++) {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setPosition(sf::Vector2f(10, (i + 1) * 44));
        if (m_selectedMenuIndex == i) m_menuText.setFillColor(sf::Color::Blue);
        else m_menuText.setFillColor(sf::Color::White);
        m_game->window().draw(m_menuText);
    }

    // Draw keybinds
    m_menuText.setString("UP:W DOWN:S PLAY:ENTER BACK:ESC");
    m_menuText.setPosition(sf::Vector2f(10, height() - 34));
    m_menuText.setFillColor(sf::Color::White);
    m_game->window().draw(m_menuText);

    m_game->window().display();
}
