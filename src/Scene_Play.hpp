#pragma once

#include "Action.hpp"
#include "Entity.hpp"
#include "Scene.hpp"
#include "Vec2.hpp"
#include <SFML/Graphics/Text.hpp>
#include <memory>
#include <fstream>

class Scene_Play : public Scene {
    struct PlayerConfig {
        float X, Y, CX, CY, SX, SY, MAXSPEED, JUMP, GRAVITY, XSCALE, YSCALE, WEAPON_SPEED, WEAPON_LIFESPAN;
        std::string WEAPON;
    };

protected:

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    bool                    m_drawTextures  = true;
    bool                    m_drawCollision = false;
    bool                    m_drawGrid      = false;
    bool                    m_levelEnded    = false;
    bool                    m_levelWon      = false;
    bool                    m_replay        = false;
    bool                    m_scene_ended   = false;
    std::vector<
        sf::RectangleShape
    >                       m_gridLines     = {};
    std::vector<
        sf::Text
    >                       m_gridLabels    = {};
    const int               m_labelFontSize = 10;
    const int               m_labelPadding  = 8;
    const Vec2              m_gridSize = { 64, 64 };
    sf::Text                m_scoreText;
    int                     m_scoreTextPadding = 12;
    sf::View                m_playerView;
    sf::Color               m_backgroundColor = sf::Color(100, 100, 255, 255);
    int                     m_levelWidth = 0;
    int                     m_playerScore = 0;
    std::ofstream           m_replay_stream;
    std::map<
        size_t,
        Action* 
    >                       m_replay_action_map;

    void init(const std::string& levelPath);
    void onEnd();
    void loadLevel(const std::string& path);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

    void spawnBullet();

    void destroyBlock(std::shared_ptr<Entity> e);
    void destroyEnemy(std::shared_ptr<Entity> e);

    void sDoAction(const Action& action);
    void sAnimation();
    void sMovement();
    void sEnemySpawner();
    void sCollision();
    void sDebug();

public:
    Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
    Scene_Play(GameEngine* gameEngine, const std::string& levelPath, bool replay);

    void update();

    void sRender();
};
