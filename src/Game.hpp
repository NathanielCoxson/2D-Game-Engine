#pragma once

#define PI 3.14159265

#include "EntityManager.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>

struct PlayerConfig        { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig         { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig        { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };
struct SpecialWeaponConfig { int FR, FG, FB, OR, OG, OB, OT, C, L, CD; };
struct EnemySpawnBox       { int x_min, x_max, y_min, y_max; };
struct PlayerSpawnBox      { int x_min, x_max, y_min, y_max; };

class Game {

    // Private member variables
    sf::RenderWindow        m_window;
    EntityManager           m_entities;
    sf::Font                m_font;
    sf::Text                m_text;
    PlayerConfig            m_playerConfig;
    EnemyConfig             m_enemyConfig;
    BulletConfig            m_bulletConfig;
    SpecialWeaponConfig     m_specialWeaponConfig;
    EnemySpawnBox           m_enemySpawnBox;
    PlayerSpawnBox          m_playerSpawnBox;
    int                     m_score = 0;
    int                     m_currentFrame = 0;
    int                     m_lastEnemySpawnTime = 0;
    int                     m_lastSpecialWeaponUseTime = 0;
    bool                    paused  = false;
    bool                    running = true;
    std::shared_ptr<Entity> m_player;

    // Private memeber functions
    void init(const std::string& config); // Initialize game using config file.
    void setPaused(bool paused);
    int  getRandomInt(int min, int max); 
    void fadeEntity(std::shared_ptr<Entity> e, sf::Color fill, sf::Color outline);
    bool checkForCollision(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
    void killPlayer();
    void updateScore(int score);

    // Systems
    void sMovement();
    void sUserInput();
    void sRender();
    void sEnemySpawner();
    void sCollision();

    // Spawners
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> originEntity);
    void spawnBullet(std::shared_ptr<Entity> originEntity, const Vec2& mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> originEntity);

public:

    Game(const std::string& config); // Constructor using game config

    void run(); // Starts the game

};
