#pragma once

#include "Action.hpp"
#include "Entity.hpp"
#include "Scene.hpp"
#include "Vec2.hpp"
#include <SFML/Graphics/Text.hpp>
#include <memory>

class Scene_Play : public Scene {
    struct PlayerConfig {
        float X, Y, CX, CY, SX, SY, MAXSPEED, JUMP, GRAVITY, XSCALE, YSCALE, WEAPON_SPEED, WEAPON_LIFESPAN;
        std::string WEAPON;
    };

protected:

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_drawGrid = false;
    const Vec2              m_gridSize = { 64, 64 };
    sf::Text                m_gridText;
    sf::View                m_playerView;
    int                     m_levelWidth = 0;

    void init(const std::string& levelPath);
    void onEnd();
    void loadLevel(const std::string& path);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

    void spawnBullet();

    void destroyBlock(std::shared_ptr<Entity> e);

    void sDoAction(const Action& action);
    void sAnimation();
    void sMovement();
    void sEnemySpawner();
    void sCollision();
    void sDebug();

public:
    Scene_Play(GameEngine* gameEngine, const std::string& levelPath);

    void update();

    void sRender();
};
