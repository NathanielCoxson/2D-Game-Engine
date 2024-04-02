#include "Game.hpp"
#include "Vec2.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

int Game::getRandomInt(int min, int max) {
    return (min + (rand() % (1 + max - min)));
}

void Game::fadeEntity(std::shared_ptr<Entity> e, sf::Color fill, sf::Color outline) {
    e->cLifespan->remaining -= 1;
    float newOpacity = 255 * (float(e->cLifespan->remaining) / float(e->cLifespan->total));
    e->cShape->circle.setFillColor(sf::Color(
                fill.r,
                fill.g,
                fill.b,
                newOpacity
                ));
    e->cShape->circle.setOutlineColor(sf::Color(
                outline.r,
                outline.g,
                outline.b,
                newOpacity
                ));
    if (e->cLifespan->remaining <= 0) {
        e->destroy();
    }
}

bool Game::checkForCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    Vec2  aPos = a->cTransform->pos, bPos = b->cTransform->pos;
    float aCR = a->cCollision->radius, bCR = b->cCollision->radius;
    float distSq = ((aPos.x - bPos.x) * (aPos.x - bPos.x)) + ((aPos.y - bPos.y) * (aPos.y - bPos.y));
    float minDistSq = (aCR + bCR) * (aCR + bCR);
    return distSq <= minDistSq;
}

void Game::killPlayer() {
    m_player->destroy();
    spawnPlayer();

    // Reset score
    m_score = 0;
    updateScore(m_score);

    // Remove currently traveling bullets
    for (auto bullet: m_entities.getEntities("bullet")) {
        bullet->destroy();
    }
}

void Game::updateScore(int score) {
    std::stringstream ss;
    ss << "Score: " << score;

    m_text.setString(ss.str());
}

void Game::init(const std::string& config) {
    srand(time(NULL));
    std::ifstream fin(config);
    
    // Read config in
    std::string type;

    while (!fin.eof()) {
        fin >> type;
        //std::cout << "Reading: " << type << "\n";

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

            m_text = sf::Text("Score: 0", m_font, fontSize);
            m_text.setPosition(0, 0);
            m_text.setFillColor(sf::Color(FR, FG, FB, 255));
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

            //std::cout << "Player: \n";
            //std::cout << "\t" << m_playerConfig.SR << "\n";
            //std::cout << "\t" << m_playerConfig.CR << "\n";
            //std::cout << "\t" << m_playerConfig.S << "\n";
            //std::cout << "\t" << m_playerConfig.FR << " " << m_playerConfig.FG << " " << m_playerConfig.FB << "\n";
            //std::cout << "\t" << m_playerConfig.OR << " " << m_playerConfig.OG << " " << m_playerConfig.OB << "\n";
            //std::cout << "\t" << m_playerConfig.OT << "\n";
            //std::cout << "\t" << m_playerConfig.V << "\n";
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

            //std::cout << "Enemy: \n";
            //std::cout << "\t" << m_enemyConfig.SR << "\n";
            //std::cout << "\t" << m_enemyConfig.CR << "\n";
            //std::cout << "\t" << m_enemyConfig.SMIN << " " << m_enemyConfig.SMAX << "\n";
            //std::cout << "\t" << m_enemyConfig.OR << " " << m_enemyConfig.OG << " " << m_enemyConfig.OB << "\n";
            //std::cout << "\t" << m_enemyConfig.OT << "\n";
            //std::cout << "\t" << m_enemyConfig.VMIN << " " << m_enemyConfig.VMAX << "\n";
            //std::cout << "\t" << m_enemyConfig.L << "\n";
            //std::cout << "\t" << m_enemyConfig.SI << "\n";
        } else if (type.compare("Bullet") == 0) {
            fin >> m_bulletConfig.SR;
            fin >> m_bulletConfig.CR;
            fin >> m_bulletConfig.S;
            fin >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB;
            fin >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB;
            fin >> m_bulletConfig.OT;
            fin >> m_bulletConfig.V;
            fin >> m_bulletConfig.L;

            //std::cout << "Bullet: \n";
            //std::cout << "\t" << m_bulletConfig.SR << "\n";
            //std::cout << "\t" << m_bulletConfig.CR << "\n";
            //std::cout << "\t" << m_bulletConfig.S << "\n";
            //std::cout << "\t" << m_bulletConfig.FR << " " << m_bulletConfig.FG << " " << m_bulletConfig.FB << "\n";
            //std::cout << "\t" << m_bulletConfig.OR << " " << m_bulletConfig.OG << " " << m_bulletConfig.OB << "\n";
            //std::cout << "\t" << m_bulletConfig.OT << "\n";
            //std::cout << "\t" << m_bulletConfig.V << "\n";
            //std::cout << "\t" << m_bulletConfig.L << "\n";
        } else if (type.compare("SpecialWeapon") == 0) {
            fin >> m_specialWeaponConfig.FR;
            fin >> m_specialWeaponConfig.FG;
            fin >> m_specialWeaponConfig.FB;
            fin >> m_specialWeaponConfig.OR;
            fin >> m_specialWeaponConfig.OG;
            fin >> m_specialWeaponConfig.OB;
            fin >> m_specialWeaponConfig.OT;
            fin >> m_specialWeaponConfig.C;
            fin >> m_specialWeaponConfig.L;
            fin >> m_specialWeaponConfig.CD;
        } else {
            fin >> type;
        }
    }
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

        // KeyPress events
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

        // KeyRelease events
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
                case sf::Keyboard::P:
                    if (paused) {
                        paused = false;
                        running = true;
                    } else {
                        paused = true;
                        running = false;
                    }
                    break;
                case sf::Keyboard::Space:
                    if (paused) break; 
                    if (m_currentFrame - m_lastSpecialWeaponUseTime >= m_specialWeaponConfig.CD) {
                        spawnSpecialWeapon(m_player);
                        m_lastSpecialWeaponUseTime = m_currentFrame;
                    }
                    break;
                default:
                    break;
            }
        }

        // Mouse left click input
        if (
                e.type == sf::Event::MouseButtonPressed &&
                e.mouseButton.button == sf::Mouse::Button::Left
            ) {
            spawnBullet(m_player, Vec2(e.mouseButton.x, e.mouseButton.y));
        }

    }
}

void Game::sRender() {
    // Score rendering
    m_window.draw(m_text);

    // Bullet fading
    for (auto e: m_entities.getEntities("bullet")) {
        fadeEntity(
                e,
                e->cShape->circle.getFillColor(),
                e->cShape->circle.getOutlineColor()
                );
    }

    // Small enemy fading
    for (auto e: m_entities.getEntities("smallEnemy")) {
        fadeEntity(
                e,
                e->cShape->circle.getFillColor(),
                e->cShape->circle.getOutlineColor()
                );
    }

    // All entities
    for (auto e: m_entities.getEntities()) {
        if (e->cTransform != nullptr && e->cShape != nullptr) {
            // Update shape position
            e->cShape->circle.setPosition(
                    e->cTransform->pos.x,
                    e->cTransform->pos.y
                    );

            // Update angle
            e->cShape->circle.setRotation(e->cTransform->angle);
            e->cTransform->angle++;
        }
        if (e->cShape != nullptr) {
            m_window.draw(e->cShape->circle);
        }
    }

    m_window.display();
}

void Game::sEnemySpawner() {
    if (m_currentFrame % m_enemyConfig.SI == 0) {
        spawnEnemy();
    }
}

void Game::sCollision() {
    // Bullet collisions
    for (auto bullet: m_entities.getEntities("bullet")) {
        for (auto enemy: m_entities.getEntities("enemy")) {
            if (checkForCollision(bullet, enemy)) {
                spawnSmallEnemies(enemy);
                enemy->destroy();
                bullet->destroy();
                updateScore(++m_score);
            }
        }

        for (auto smallEnemy: m_entities.getEntities("smallEnemy")) {
            if (checkForCollision(bullet, smallEnemy)) {
                smallEnemy->destroy();
                bullet->destroy();
                updateScore(++m_score);
            }
        }
    }

    // Enemy-player collisions
    for (auto enemy: m_entities.getEntities("enemy")) {
        if (checkForCollision(m_player, enemy)) {
            killPlayer();
        }
    }
    for (auto enemy: m_entities.getEntities("smallEnemy")) {
        if (checkForCollision(m_player, enemy)) {
            killPlayer();
        }
    }

    // Boundary collisions
    for (auto e: m_entities.getEntities("enemy")) {
        if (e->cCollision != nullptr && e->cShape != nullptr && e->cTransform != nullptr) {
            float x = e->cTransform->pos.x;
            float y = e->cTransform->pos.y;

            bool updateAngle = false;
            if (x <= m_enemySpawnBox.x_min || x >= m_enemySpawnBox.x_max) {
                e->cTransform->velocity.x *= -1;
                updateAngle = true;
            }
            if (y <= m_enemySpawnBox.y_min || y >= m_enemySpawnBox.y_max) {
                e->cTransform->velocity.y *= -1;
                updateAngle = true;
            }
        }
    }
}

void Game::spawnPlayer() {
    if (paused) return;

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
    if (paused) return;

    std::shared_ptr<Entity> newEnemy = m_entities.addEntity("enemy");  
    
    // Determine random velocity settings for new enemy
    float speed = getRandomInt(m_enemyConfig.SMIN, m_enemyConfig.SMAX);  
    float theta = getRandomInt(0, 360);
    Vec2 velocity(speed * std::cos(theta * PI/180), speed * std::sin(theta * PI/180));

    newEnemy->cTransform = std::make_shared<CTransform>(
            Vec2(
                getRandomInt(m_enemySpawnBox.x_min, m_enemySpawnBox.x_max),
                getRandomInt(m_enemySpawnBox.y_min, m_enemySpawnBox.y_max)
                ),
            velocity,
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

void Game::spawnSmallEnemies(std::shared_ptr<Entity> origin) {
    if (paused) return;

    int V = origin->cShape->circle.getPointCount();

    // Spawn 360 / V small enemy entities
    for (int theta = 360 / V; theta <= 360; theta += 360 / V) {
        std::shared_ptr<Entity> smallEnemy = m_entities.addEntity("smallEnemy");

        Vec2 velocity(std::cos(theta * PI/180), std::sin(theta * PI/180));
        velocity.normalize();
        velocity.x *= origin->cTransform->velocity.length();
        velocity.y *= origin->cTransform->velocity.length();
        
        smallEnemy->cTransform = std::make_shared<CTransform>(
                origin->cTransform->pos,
                velocity,
                0
                );

        smallEnemy->cShape = std::make_shared<CShape>(
                m_enemyConfig.SR / 2,
                V,
                origin->cShape->circle.getFillColor(), 
                origin->cShape->circle.getOutlineColor(), 
                origin->cShape->circle.getOutlineThickness()
                );

        smallEnemy->cCollision = std::make_shared<CCollision>(origin->cCollision->radius / 2);

        smallEnemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> origin, const Vec2& mousePos) {
    if (paused) return;

    std::shared_ptr<Entity> newBullet = m_entities.addEntity("bullet");

    Vec2 diff(mousePos.x - origin->cTransform->pos.x, mousePos.y - origin->cTransform->pos.y);
    diff.normalize();
    Vec2 velocity(m_bulletConfig.S * diff.x, m_bulletConfig.S * diff.y);
    newBullet->cTransform = std::make_shared<CTransform>(
            origin->cTransform->pos,
            velocity,
            0
            );

    newBullet->cShape = std::make_shared<CShape>(
            m_bulletConfig.SR,
            m_bulletConfig.V,
            sf::Color(
                m_bulletConfig.FR,
                m_bulletConfig.FG,
                m_bulletConfig.FB,
                255
                ),
            sf::Color(
                m_bulletConfig.OR,
                m_bulletConfig.OG,
                m_bulletConfig.OB,
                255
                ),
            m_bulletConfig.OT
            );

    newBullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

    newBullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> origin) {
    if (paused) return;

    int interval = 360 / m_specialWeaponConfig.C; 
     
    for (int theta = interval; theta <= 360; theta += interval) {
        std::shared_ptr<Entity> newBullet = m_entities.addEntity("bullet");

        Vec2 velocity(std::cos(theta * PI/180), std::sin(theta * PI/180));
        velocity.normalize();
        velocity.x *= m_bulletConfig.S;
        velocity.y *= m_bulletConfig.S;
        
        newBullet->cTransform = std::make_shared<CTransform>(
                origin->cTransform->pos,
                velocity,
                0
                );

        newBullet->cShape = std::make_shared<CShape>(
                m_bulletConfig.SR,
                m_bulletConfig.V,
                sf::Color(
                    m_specialWeaponConfig.FR,
                    m_specialWeaponConfig.FG,
                    m_specialWeaponConfig.FB,
                    255
                    ),
                sf::Color(
                    m_specialWeaponConfig.OR,
                    m_specialWeaponConfig.OG,
                    m_specialWeaponConfig.OB,
                    255
                    ),
                m_specialWeaponConfig.OT
                );

        newBullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

        newBullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
    }
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
        if (running) {
            sMovement();
            sEnemySpawner();
            sCollision();
        }
        sUserInput();
        sRender();
    }

}
