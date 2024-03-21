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
            m_window.setFramerateLimit(frameLimit);
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

void Game::sMovement() {
    // Player velocity updates
    m_player->cTransform->velocity = { 0.0, 0.0 };
    // Up input 
    if (m_player->cInput->up) {
        m_player->cTransform->velocity.y = -1 * m_playerConfig.S;
    }
    // Down input 
    if (m_player->cInput->down) {
        m_player->cTransform->velocity.y = m_playerConfig.S;
    }
    // Left input 
    if (m_player->cInput->left) {
        m_player->cTransform->velocity.x = -1 * m_playerConfig.S;
    }
    // Right input 
    if (m_player->cInput->right) {
        m_player->cTransform->velocity.x = m_playerConfig.S;
    }

    // Entity position updates
    for (auto e: m_entities.getEntities()) {
        if (e->cTransform != nullptr) {
            e->cTransform->pos.x += e->cTransform->velocity.x;            
            e->cTransform->pos.y += e->cTransform->velocity.y;            
        }
    }
}

void Game::sUserInput() {
    sf::Event e;
    while (m_window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            m_window.close();
        }

        if (e.type == sf::Event::KeyPressed) {
            switch (e.key.code) {
                case sf::Keyboard::W:
                    std::cout << "W Key pressed\n";
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::A:
                    std::cout << "A Key pressed\n";
                    m_player->cInput->left = true;
                    break;
                case sf::Keyboard::S:
                    std::cout << "S Key pressed\n";
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::D:
                    std::cout << "D Key pressed\n";
                    m_player->cInput->right = true;
                    break;
                default:
                    break;
            }
        }

        if (e.type == sf::Event::KeyReleased) {
            switch (e.key.code) {
                case sf::Keyboard::W:
                    std::cout << "W Key released\n";
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::A:
                    std::cout << "A Key released\n";
                    m_player->cInput->left = false;
                    break;
                case sf::Keyboard::S:
                    std::cout << "S Key released\n";
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::D:
                    std::cout << "D Key released\n";
                    m_player->cInput->right = false;
                    break;
                default:
                    break;
            }
        }
    }
}

void Game::sRender() {
    for (auto e: m_entities.getEntities()) {
        if (e->cTransform != nullptr && e->cShape != nullptr) {
            e->cShape->circle.setPosition(
                    e->cTransform->pos.x,
                    e->cTransform->pos.y
                    );
        }
        if (e->cShape != nullptr) {
            m_window.draw(e->cShape->circle);
        }
    }
}

void Game::spawnPlayer() {
    m_player = m_entities.addEntity("player");

    m_player->cTransform = std::make_shared<CTransform>(
            Vec2(m_window.getSize().x/2.f, m_window.getSize().y/2.f),
            Vec2(0, 0),
            0
            );

    m_player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    m_player->cInput = std::make_shared<CInput>();

    m_player->cShape = std::make_shared<CShape>(
            m_playerConfig.SR,
            m_playerConfig.V,
            sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB, 255),
            sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB, 255),
            m_playerConfig.OT
            );
    m_player->cShape->circle.setPosition(
            m_player->cTransform->pos.x,
            m_player->cTransform->pos.y
            );
}

Game::Game(const std::string& config) {
    init(config);
    spawnPlayer();
}

void Game::run() {
    sf::Event e;

    while (m_window.isOpen()) {

        m_entities.update();

        // Clear previous frame
		m_window.clear(sf::Color::Black);

        // Systems
        sMovement();
        sUserInput();
        sRender();


        // Display new frame
        m_window.display();
    }
}
