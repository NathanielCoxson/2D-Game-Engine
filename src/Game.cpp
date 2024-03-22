#include "Game.hpp"
#include "Vec2.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <iostream>

void Game::init(const std::string& config) {
    srand(time(NULL));
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

            // Set bounds on player spawn area
            m_playerSpawnBox.x_min = m_playerConfig.SR;
            m_playerSpawnBox.y_min = m_playerConfig.SR;
            m_playerSpawnBox.x_max = m_window.getSize().x - m_playerConfig.SR;
            m_playerSpawnBox.y_max = m_window.getSize().y - m_playerConfig.SR;

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

            // Set bounds on enemey spawn area
            m_enemySpawnBox.x_min = m_enemyConfig.SR;
            m_enemySpawnBox.y_min = m_enemyConfig.SR;
            m_enemySpawnBox.x_max = m_window.getSize().x - m_enemyConfig.SR;
            m_enemySpawnBox.y_max = m_window.getSize().y - m_enemyConfig.SR;

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

int Game::getRandomInt(int min, int max) {
    return min + (rand() % (1 + min - max));
}

void Game::sMovement() {
    // Player velocity updates
    m_player->cTransform->velocity = { 0.0, 0.0 };
    float player_x = m_player->cTransform->pos.x;
    float player_y = m_player->cTransform->pos.y;
    // Up input 
    if (m_player->cInput->up && player_y > m_playerSpawnBox.y_min) {
        m_player->cTransform->velocity.y = -1 * m_playerConfig.S;
    }
    // Down input 
    if (m_player->cInput->down && player_y < m_playerSpawnBox.y_max) {
        m_player->cTransform->velocity.y = m_playerConfig.S;
    }
    // Left input 
    if (m_player->cInput->left && player_x > m_playerSpawnBox.x_min) {
        m_player->cTransform->velocity.x = -1 * m_playerConfig.S;
    }
    // Right input 
    if (m_player->cInput->right && player_x < m_playerSpawnBox.x_max) {
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
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = true;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = true;
                    break;
                default:
                    break;
            }
        }

        if (e.type == sf::Event::KeyReleased) {
            switch (e.key.code) {
                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = false;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::D:
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

void Game::sEnemySpawner() {
    if (m_currentFrame % m_enemyConfig.SI == 0) {
        spawnEnemy();
    }
}

void Game::sCollision() {
    // Boundary collisions
    for (auto e: m_entities.getEntities("enemy")) {
        if (e->cCollision != nullptr && e->cShape != nullptr && e->cTransform != nullptr) {
            float x = e->cTransform->pos.x;
            float y = e->cTransform->pos.y;

            if (x <= m_enemySpawnBox.x_min || x >= m_enemySpawnBox.x_max) {
                e->cTransform->velocity.x *= -1;
            }
            if (y <= m_enemySpawnBox.y_min || y >= m_enemySpawnBox.y_max) {
                e->cTransform->velocity.y *= -1;
            }
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

    m_player->cShape = std::make_shared<CShape>(
            m_playerConfig.SR,
            m_playerConfig.V,
            sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB, 255),
            sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB, 255),
            m_playerConfig.OT
            );

    m_player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

    m_player->cInput = std::make_shared<CInput>();

}

void Game::spawnEnemy() {
    std::shared_ptr<Entity> newEnemy = m_entities.addEntity("enemy");  
    
    // Determine random velocity settings for new enemy
    float speed = getRandomInt(m_enemyConfig.SMIN, m_enemyConfig.SMAX);  
    Vec2 randomVelocity(speed, speed);
    int moveRight = rand() % 100 + 1;
    int moveDown  = rand() % 100 + 1;
    if (moveRight >= 50) randomVelocity.x *= -1;
    if (moveDown  >= 50) randomVelocity.y *= -1;

    newEnemy->cTransform = std::make_shared<CTransform>(
            Vec2(
                getRandomInt(m_enemySpawnBox.x_min, m_enemySpawnBox.x_max),
                getRandomInt(m_enemySpawnBox.y_min, m_enemySpawnBox.y_max)
                ),
            randomVelocity,
            0
            );

    newEnemy->cShape = std::make_shared<CShape>(
            m_enemyConfig.SR,
            getRandomInt(m_enemyConfig.VMIN, m_enemyConfig.VMAX),
            sf::Color(
                getRandomInt(0, 255),
                getRandomInt(0, 255),
                getRandomInt(0, 255),
                255
                ),
            sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB, 255),
            m_enemyConfig.OT
            );
    
    newEnemy->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

}

Game::Game(const std::string& config) {
    init(config);
    spawnPlayer();
}

void Game::run() {
    sf::Event e;

    while (m_window.isOpen()) {
        m_currentFrame++;
        m_entities.update();

        // Clear previous frame
		m_window.clear(sf::Color::Black);

        // Systems
        sMovement();
        sUserInput();
        sRender();
        sEnemySpawner();
        sCollision();

        // Display new frame
        m_window.display();
    }

}
