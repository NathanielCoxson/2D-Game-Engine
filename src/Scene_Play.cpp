#include "Scene_Play.hpp"
#include "Vec2.hpp"
#include <fstream>
#include <iostream>
#include "Physics.hpp"

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

            auto& bb = tile->addComponent<CBoundingBox>(Vec2(m_gridSize.x, m_gridSize.y));

            auto& transform = tile->addComponent<CTransform>(pos, Vec2(0, 0), 0);
            transform.scale = Vec2(SX, SY);
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

            Vec2 pos = gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player);
            auto& transform = m_player->addComponent<CTransform>(pos, Vec2(0, 0), 0);
            transform.scale = Vec2(m_playerConfig.XSCALE, m_playerConfig.YSCALE);

            auto& bb = m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));

            auto& state = m_player->addComponent<CState>();

            auto& gravity = m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
        } else {
            fin >> asset_type;
        }
    }
}

void Scene_Play::update() {
    m_entities.update();
    sMovement();
    sCollision();
    sAnimation();

    for (auto e: m_entities.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            if (e->hasComponent<CTransform>()) {
                Vec2 pos = e->getComponent<CTransform>().pos;
                e->getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f(pos.x, pos.y));
            }
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
        } else if (action.getName() == "JUMP") {
            input.up = true;
        } else if (action.getName() == "QUIT") {
            onEnd();
        } 
    } else if (action.getType() == "END") {
        if (action.getName() == "LEFT" && input.left) {
            input.left = false;
        } else if (action.getName() == "RIGHT" && input.right) {
            input.right = false;
        } else if (action.getName() == "JUMP") {
            input.up = false;
        } else if (action.getName() == "TOGGLE_COLLISION") {
            if (m_drawCollision) m_drawCollision = false;
            else m_drawCollision = true;
        } else if (action.getName() == "TOGGLE_TEXTURE") {
            if (m_drawTextures) m_drawTextures = false;
            else m_drawTextures = true;
        }
    }
}

void Scene_Play::sMovement() {
    CInput input = m_player->getComponent<CInput>();
    CTransform& player_transform = m_player->getComponent<CTransform>();
    CAnimation& player_animation = m_player->getComponent<CAnimation>();
    CState&     player_state = m_player->getComponent<CState>();
    CGravity&   player_gravity = m_player->getComponent<CGravity>();

    Vec2 player_velocity(0, m_player->getComponent<CTransform>().velocity.y);
    if (input.left) {
        player_velocity.x -= m_playerConfig.SX;

        if (player_animation.animation.getName() != "MegaManRun" || player_animation.animation.getSprite().getScale().x < 0) {
            player_animation.animation = m_game->assets().getAnimation("MegaManRun");
            player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE, m_playerConfig.YSCALE);
        }
    }
    if (input.right) {
        player_velocity.x += m_playerConfig.SX;

        if (player_animation.animation.getName() != "MegaManRun" || player_animation.animation.getSprite().getScale().x > 0) {
            player_animation.animation = m_game->assets().getAnimation("MegaManRun");
            player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE * -1, m_playerConfig.YSCALE);
        }
    } 
    // Add initial velocity if jump key is pressed
    if (input.up && player_state.on_ground) {
        player_velocity.y = m_playerConfig.SY;
    }
    // Set velocity to 0 if jump key is released
    if (!input.up && !player_state.on_ground) {
        if (player_velocity.y < 0) player_velocity.y = 0;
    }
    player_transform.velocity = player_velocity;

    for (auto e: m_entities.getEntities()) {
        if (e->hasComponent<CTransform>()) {
            if (e->hasComponent<CGravity>() && !e->getComponent<CState>().on_ground) {
                e->getComponent<CTransform>().velocity.y += e->getComponent<CGravity>().gravity;
            }

            // Cap speeds to maxspeed
            auto& t = e->getComponent<CTransform>();
            if (t.velocity.x > m_playerConfig.MAXSPEED)      t.velocity.x = m_playerConfig.MAXSPEED;
            if (t.velocity.y > m_playerConfig.MAXSPEED)      t.velocity.y = m_playerConfig.MAXSPEED;
            if (t.velocity.x < m_playerConfig.MAXSPEED * -1) t.velocity.x = m_playerConfig.MAXSPEED * -1;
            if (t.velocity.y < m_playerConfig.MAXSPEED * -1) t.velocity.y = m_playerConfig.MAXSPEED * -1;
            e->getComponent<CTransform>().prevPos = e->getComponent<CTransform>().pos;
            e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
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
    if (!player_state.on_ground) {
        float direction = player_animation.animation.getSprite().getScale().x / m_playerConfig.XSCALE;

        player_animation.animation = m_game->assets().getAnimation("MegaManJump");
        player_animation.animation.getSprite().setScale(player_transform.scale.x * direction, player_transform.scale.y);
    }

}

void Scene_Play::sAnimation() {

}

void Scene_Play::sCollision() {
    CState&     player_state = m_player->getComponent<CState>();
    CTransform& player_transform = m_player->getComponent<CTransform>();

    player_state.on_ground = false;
    for (auto tile: m_entities.getEntities("Block")) {
        Vec2 overlap = Physics::GetOverlap(m_player, tile);
        Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);

        bool overlapping = overlap.y >= 0 && overlap.x >= 0;
        bool vertical_collision = prevOverlap.x > 0;
        bool horizontal_collision = prevOverlap.y > 0;
        bool came_from_above = player_transform.pos.y <= tile->getComponent<CTransform>().pos.y;
        bool came_from_below = player_transform.pos.y >= tile->getComponent<CTransform>().pos.y;
        bool came_from_left  = player_transform.pos.x <= tile->getComponent<CTransform>().pos.x;
        bool came_from_right = player_transform.pos.x >= tile->getComponent<CTransform>().pos.x;

        if (overlapping && vertical_collision && came_from_above) {
            player_state.on_ground = true;
            player_transform.pos.y -= overlap.y;
            player_transform.velocity.y = 0;
            return;
        } else if (overlapping && vertical_collision && came_from_below) {
            player_transform.pos.y += overlap.y;
            player_transform.velocity.y = 0;
            return;
        } else if (overlapping && horizontal_collision && came_from_left) {
            player_transform.pos.x -= overlap.x;
            player_transform.velocity.x = 0;
            return;
        } else if (overlapping && horizontal_collision && came_from_right) {
            player_transform.pos.x += overlap.x;
            player_transform.velocity.x = 0;
            return;
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
        if (e->hasComponent<CAnimation>()) {
            if (m_drawTextures) {
                m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
            }
        }
        if (e->hasComponent<CBoundingBox>()) {
            if (m_drawCollision) {
                auto& transform = e->getComponent<CTransform>();
                auto& bb = e->getComponent<CBoundingBox>();
                sf::RectangleShape outline;
                outline.setSize(sf::Vector2f(bb.size.x, bb.size.y));
                outline.setOrigin(sf::Vector2f(outline.getSize().x / 2, outline.getSize().y / 2));
                outline.setPosition(sf::Vector2f(transform.pos.x, transform.pos.y));
                outline.setOutlineThickness(2);
                outline.setOutlineColor(sf::Color::Blue);
                outline.setFillColor(sf::Color::Transparent);
                m_game->window().draw(outline);
            }
        }
    }

    //Display
    m_game->window().display();
}


