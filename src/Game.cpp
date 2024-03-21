#include "Game.hpp"
#include "Vec2.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <iostream>

void Game::init(const std::string& config) {
    std::ifstream fin(config);
    
    // Read config in
    std::string type;

    while (!fin.eof()) {
        fin >> type;
        std::cout << "Reading: " << type << "\n";

        if (type.compare("Window") == 0) {
            std::string title = "Geometry Battle";
            int width, height, frameLimit, fullScreen;
            fin >> width >> height >> frameLimit >> fullScreen;

            // Create the window
            if (fullScreen == 0) m_window.create(sf::VideoMode(width, height), title);
            else m_window.create(sf::VideoMode(width, height), "Geometry Battle", sf::Style::Fullscreen);
        } else if (type.compare("Font") == 0) {
            std::string fontFile;
            int fontSize, FR, FG, FB;
            fin >> fontFile >> fontSize >> FR >> FG >> FB;

            // Load the font
            if (!m_font.loadFromFile(fontFile)) {
                std::cout << "Failed to load font file\n";
                exit(1);
            } 
            
        } else if (type.compare("Player") == 0) {
            fin >> m_playerConfig.SR;
            fin >> m_playerConfig.CR;
            fin >> m_playerConfig.S;
            fin >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB;
            fin >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB;
            fin >> m_playerConfig.OT;
            fin >> m_playerConfig.V;
            std::cout << "Player: \n";
            std::cout << "\t" << m_playerConfig.SR << "\n";
            std::cout << "\t" << m_playerConfig.CR << "\n";
            std::cout << "\t" << m_playerConfig.S << "\n";
            std::cout << "\t" << m_playerConfig.FR << " " << m_playerConfig.FG << " " << m_playerConfig.FB << "\n";
            std::cout << "\t" << m_playerConfig.OR << " " << m_playerConfig.OG << " " << m_playerConfig.OB << "\n";
            std::cout << "\t" << m_playerConfig.OT << "\n";
            std::cout << "\t" << m_playerConfig.V << "\n";
        } else if (type.compare("Enemy") == 0) {
            fin >> m_enemyConfig.SR;
            fin >> m_enemyConfig.CR;
            fin >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX;
            fin >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB;
            fin >> m_enemyConfig.OT;
            fin >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX;
            fin >> m_enemyConfig.L;
            fin >> m_enemyConfig.SI;
            std::cout << "Enemy: \n";
            std::cout << "\t" << m_enemyConfig.SR << "\n";
            std::cout << "\t" << m_enemyConfig.CR << "\n";
            std::cout << "\t" << m_enemyConfig.SMIN << " " << m_enemyConfig.SMAX << "\n";
            std::cout << "\t" << m_enemyConfig.OR << " " << m_enemyConfig.OG << " " << m_enemyConfig.OB << "\n";
            std::cout << "\t" << m_enemyConfig.OT << "\n";
            std::cout << "\t" << m_enemyConfig.VMIN << " " << m_enemyConfig.VMAX << "\n";
            std::cout << "\t" << m_enemyConfig.L << "\n";
            std::cout << "\t" << m_enemyConfig.SI << "\n";
        } else if (type.compare("Bullet") == 0) {
            fin >> m_bulletConfig.SR;
            fin >> m_bulletConfig.CR;
            fin >> m_bulletConfig.S;
            fin >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB;
            fin >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB;
            fin >> m_bulletConfig.OT;
            fin >> m_bulletConfig.V;
            fin >> m_bulletConfig.L;
            std::cout << "Bullet: \n";
            std::cout << "\t" << m_bulletConfig.SR << "\n";
            std::cout << "\t" << m_bulletConfig.CR << "\n";
            std::cout << "\t" << m_bulletConfig.S << "\n";
            std::cout << "\t" << m_bulletConfig.FR << " " << m_bulletConfig.FG << " " << m_bulletConfig.FB << "\n";
            std::cout << "\t" << m_bulletConfig.OR << " " << m_bulletConfig.OG << " " << m_bulletConfig.OB << "\n";
            std::cout << "\t" << m_bulletConfig.OT << "\n";
            std::cout << "\t" << m_bulletConfig.V << "\n";
            std::cout << "\t" << m_bulletConfig.L << "\n";
        } else {
            fin >> type;
        }
    }
}

void Game::spawnPlayer() {
    m_player = m_entities.addEntity("player");


    m_player->cTransform = std::make_shared<CTransform>(Vec2(100, 100), Vec2(0, 0), 0);

    m_player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    m_player->cShape = std::make_shared<CShape>(
            m_playerConfig.SR,
            m_playerConfig.V,
            sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB, 255),
            sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB, 255),
            m_playerConfig.OT);
    m_player->cShape->circle.setPosition(
            m_player->cTransform->pos.x, 
            m_player->cTransform->pos.y);
}

Game::Game(const std::string& config) {
    init(config);
}

void Game::run() {
    sf::Event e;

    while (m_window.isOpen()) {
        while (m_window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                m_window.close();
            }
        }

        // Clear previous frame
		m_window.clear(sf::Color::Black);

        // Drawing

        // Display new frame
        m_window.display();
    }
}
