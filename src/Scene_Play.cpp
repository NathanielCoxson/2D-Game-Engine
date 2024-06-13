#include "Scene_Play.hpp"
#include "Vec2.hpp"
#include <fstream>
#include <iostream>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine)
    , m_levelPath(levelPath)
{
    init(levelPath);
}

void Scene_Play::init(const std::string& levelPath) {
    // Debug and Control
    registerAction(sf::Keyboard::P,      "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T,      "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C,      "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G,      "TOGGLE_GRID");

    // Gameplay
    registerAction(sf::Keyboard::A,     "LEFT");
    registerAction(sf::Keyboard::D,     "RIGHT");
    registerAction(sf::Keyboard::Space, "JUMP");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("RobotoRegular"));

    loadLevel(m_levelPath);
}

void Scene_Play::onEnd() {
    m_game->changeScene("menu");
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
    auto& Animation = entity->getComponent<CAnimation>();
     
    float x = m_gridSize.x * gridX + (Animation.animation.getSprite().getGlobalBounds().width / 2.0f);
    float y = m_gridSize.y * gridY + (Animation.animation.getSprite().getGlobalBounds().height / 2.0f);
    
    return Vec2(x, m_game->window().getSize().y - y);
}
 
void Scene_Play::loadLevel(const std::string& path) {

    std::ifstream fin(path);
    while (!fin.eof()) {
        std::string asset_type;
        fin >> asset_type; 

        if (asset_type == "Tile") {
            std::string N;
            float X, Y, SX, SY;
            fin >> N >> X >> Y >> SX >> SY;
            
            std::shared_ptr<Entity> tile = m_entities.addEntity(N);
            auto& animation = tile->addComponent<CAnimation>(m_game->assets().getAnimation(N));

            animation.animation.getSprite().setScale(SX, SY);

            Vec2 pos = gridToMidPixel(X, Y, tile);
            animation.animation.getSprite().setPosition(sf::Vector2f(pos.x, pos.y));
        } else if (asset_type == "Dec") {
            std::string N;
            float X, Y, SX, SY;
            fin >> N >> X >> Y >> SX >> SY;

            std::shared_ptr<Entity> dec = m_entities.addEntity(N);
            auto& animation = dec->addComponent<CAnimation>(m_game->assets().getAnimation(N));

            animation.animation.getSprite().setScale(SX, SY);

            Vec2 pos = gridToMidPixel(X, Y, dec);
            animation.animation.getSprite().setPosition(sf::Vector2f(pos.x, pos.y));
        } else if (asset_type == "Player") {
            fin >> m_playerConfig.X >> m_playerConfig.Y;
            fin >> m_playerConfig.CX >> m_playerConfig.CY;
            fin >> m_playerConfig.SX >> m_playerConfig.SY >> m_playerConfig.MAXSPEED;
            fin >> m_playerConfig.GRAVITY;
            fin >> m_playerConfig.WEAPON;
            fin >> m_playerConfig.XSCALE >> m_playerConfig.YSCALE;

            m_player = m_entities.addEntity("Player");

            auto& animation = m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegaManIdle"));
            animation.animation.getSprite().setScale(m_playerConfig.XSCALE, m_playerConfig.YSCALE);

            auto& transform = m_player->addComponent<CTransform>();
            transform.pos = gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player);
        } else {
            fin >> asset_type;
        }
    }
}

void Scene_Play::update() {
    m_entities.update();
    sMovement();

    CAnimation& player_animation = m_player->getComponent<CAnimation>();
    CTransform& player_transform = m_player->getComponent<CTransform>();
    player_animation.animation.getSprite().setPosition(sf::Vector2f(player_transform.pos.x, player_transform.pos.y));

    for (auto e: m_entities.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            e->getComponent<CAnimation>().animation.update();
        }
    }
}

void Scene_Play::sDoAction(const Action& action) {
    auto& input = m_player->getComponent<CInput>();
    if (action.getType() == "START") {
        if (action.getName() == "LEFT" && !input.left) {
            input.left = true;
        } else if (action.getName() == "RIGHT" && !input.right) {
            input.right = true;
        } else if (action.getName() == "QUIT") {
            onEnd();
        }
    } else if (action.getType() == "END") {
        if (action.getName() == "LEFT" && input.left) {
            input.left = false;
        } else if (action.getName() == "RIGHT" && input.right) {
            input.right = false;
        }
    }
}

void Scene_Play::sMovement() {
    CInput input = m_player->getComponent<CInput>();
    CTransform& player_transform = m_player->getComponent<CTransform>();
    CAnimation& player_animation = m_player->getComponent<CAnimation>();

    if (input.left) {
        player_transform.pos.x -= 5;

        if (player_animation.animation.getName() != "MegaManRun" || player_animation.animation.getSprite().getScale().x < 0) {
            player_animation.animation = m_game->assets().getAnimation("MegaManRun");
            player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE, m_playerConfig.YSCALE);
        }
    }
    if (input.right) {
        player_transform.pos.x += 5;

        if (player_animation.animation.getName() != "MegaManRun" || player_animation.animation.getSprite().getScale().x > 0) {
            player_animation.animation = m_game->assets().getAnimation("MegaManRun");
            player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE * -1, m_playerConfig.YSCALE);
        }
    } 

    bool is_idle = !input.left && !input.right;
    bool net_zero_horizontal_movement = input.left && input.right;
    if (is_idle || net_zero_horizontal_movement) {
        if (player_animation.animation.getName() != "MegaManIdle") {
            float direction = player_animation.animation.getSprite().getScale().x / m_playerConfig.XSCALE;

            player_animation.animation = m_game->assets().getAnimation("MegaManIdle");
            player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE * direction, m_playerConfig.YSCALE);
        }
    }

}

void Scene_Play::sRender() {
    //Update
    update();

    //Clear
    m_game->window().clear(sf::Color::Black);

    //Draw
    for (auto e: m_entities.getEntities()) {
        if (e->tag() == "Player") continue;
        if (e->hasComponent<CAnimation>()) {
            m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
        }
    }

    m_game->window().draw(m_player->getComponent<CAnimation>().animation.getSprite());

    //Display
    m_game->window().display();
}


