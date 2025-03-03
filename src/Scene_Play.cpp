#include "Scene_Play.hpp"
#include "Physics.hpp"
#include "Vec2.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

Scene_Play::Scene_Play(GameEngine *gameEngine, const std::string &levelPath)
    : Scene(gameEngine), m_levelPath(levelPath) {
    init(levelPath);
}

Scene_Play::Scene_Play(GameEngine *gameEngine, const std::string &levelPath, bool replay)
    : Scene(gameEngine), m_levelPath(levelPath), m_replay(replay) {
    init(levelPath);
}

void Scene_Play::init(const std::string &levelPath) {
    // Debug and Control
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");

    // Gameplay
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::W, "JUMP");
    registerAction(sf::Keyboard::Space, "SHOOT");

    m_scoreText.setCharacterSize(32);
    m_scoreText.setFont(m_game->assets().getFont("PrimaryFont"));
    m_scoreText.setFillColor(sf::Color::White);

    m_energyBar.setFillColor(sf::Color::Yellow);

    m_energyBarOutline.setOutlineThickness(2);

    loadLevel(m_levelPath);

    // Construct grid
    auto windowSize = m_game->window().getSize();
    auto& font = m_game->assets().getFont("PrimaryFont");
    for (int row = 0; row < m_levelWidth; row++) {
        // Horizontal line
        sf::RectangleShape horizontalLine(sf::Vector2f(m_gridSize.x * m_levelWidth, 2.0f));
        horizontalLine.setFillColor(sf::Color::Blue);
        horizontalLine.setPosition(0, windowSize.y - m_gridSize.x * row);
        m_gridLines.push_back(horizontalLine);

        // Vertical line
        sf::RectangleShape verticalLine(sf::Vector2f(2.0f, m_gridSize.x * m_levelWidth));
        verticalLine.setFillColor(sf::Color::Blue);
        verticalLine.setPosition(m_gridSize.x * row, 0);
        m_gridLines.push_back(verticalLine);

        for (int col = 0; col < m_levelWidth; col++) {
            // Labels
            sf::Text label;
            label.setCharacterSize(m_labelFontSize);
            label.setFont(font);
            label.setFillColor(sf::Color::Blue);
            label.setOutlineColor(sf::Color::Blue);
            std::stringstream ss;
            ss << row << ", " << col;
            label.setString(ss.str());
            label.setPosition(
                row * m_gridSize.x + m_labelPadding,
                windowSize.y - m_gridSize.y * (col + 1) + m_labelPadding
            );
            m_gridLabels.push_back(label);
        }
    }

    m_playerView.setSize(
        sf::Vector2f(
            m_game->window().getSize().x,
            m_game->window().getSize().y
        )
    );

    if (m_replay) {
        std::ifstream replay_file("replays/replay.txt"); 

        std::string type, name, level_path;
        size_t frame;
        replay_file >> level_path;
        while (!replay_file.eof()) {
            replay_file >> type >> name >> frame;
            m_replay_action_map.insert(std::make_pair(frame, new Action(name, type)));
        }

        replay_file.close();
    } else {
        m_replay_stream.open("replays/replay.txt");
        m_replay_stream << m_levelPath << "\n";
    }
}

void Scene_Play::onEnd() {
    if (!m_replay) {
        m_replay_stream.close();
    }

    m_scene_ended = true;

    m_game->window().setView(m_game->window().getDefaultView());
    m_game->changeScene("menu", nullptr, true);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
    auto &Animation = entity->getComponent<CAnimation>();
    auto bounds = Animation.animation.getSprite().getGlobalBounds();

    float x = 
        m_gridSize.x * 
        gridX + 
        (bounds.width / 2.0f);
    float y = 
        m_gridSize.y * 
        gridY + 
        (bounds.height / 2.0f);

    return Vec2(x, m_game->window().getSize().y - y);
}

bool Scene_Play::canDestroyEntity(std::shared_ptr<Entity> e) {

    CState &state= e->getComponent<CState>();

    bool is_invincible = e->hasComponent<CState>() && state.timers.count("invincibility");
    if (is_invincible) return false;

    return true;
}

void Scene_Play::loadLevel(const std::string &path) {

    std::ifstream fin(path);
    while (!fin.eof()) {
        std::string asset_type;
        fin >> asset_type;

        if (asset_type == "Tile") {
            std::string N;
            float X, Y, SX, SY;
            fin >> N >> X >> Y >> SX >> SY;

            if (int(X) > m_levelWidth)
                m_levelWidth = int(X);

            // Animation
            std::shared_ptr<Entity> tile = m_entities.addEntity(N);
            auto &animation = tile->addComponent<CAnimation>(m_game->assets().getAnimation(N));
            animation.animation.getSprite().setScale(SX, SY);

            // Sprite positioning
            Vec2 pos = gridToMidPixel(X, Y, tile);
            animation.animation.getSprite().setPosition(sf::Vector2f(pos.x, pos.y));

            // Bounding boxes
            if (N == "Block") {
                tile->addComponent<CBoundingBox>(Vec2(m_gridSize.x, m_gridSize.y));
            } else if (N == "Flagpole") {
                CBoundingBox& boundingBox = tile->addComponent<CBoundingBox>(
                    Vec2(
                        animation.animation.getSize().x * SX / 2.0f,
                        animation.animation.getSize().y * SY / 1.25f
                    )
                );
                auto flag = m_entities.addEntity("Flag");
                CAnimation& flagAnimation = flag->addComponent<CAnimation>(m_game->assets().getAnimation("Flag"));

                Vec2 flagPos = Vec2(
                    pos.x - (flagAnimation.animation.getSprite().getTexture()->getSize().x),
                    pos.y + boundingBox.halfSize.y - flagAnimation.animation.getSprite().getTexture()->getSize().y
                );
                flagAnimation.animation.getSprite().setPosition(flagPos.x, flagPos.y);
                flagAnimation.animation.getSprite().setScale(SX, SY);
                flag->addComponent<CTransform>(flagPos, Vec2(0, 0), 0);
            }

            // Transforms
            auto &transform = tile->addComponent<CTransform>(pos, Vec2(0, 0), 0);
            transform.scale = Vec2(SX, SY);
        } else if (asset_type == "Dec") {
            std::string N;
            float X, Y, SX, SY;
            fin >> N >> X >> Y >> SX >> SY;
            std::string tag = N;

            if (int(X) > m_levelWidth) m_levelWidth = int(X);

            // TODO: Refactor this to use a single 'Enemy' tag
            if (N == "Toad" || N == "Slime") tag = "Slime";

            std::shared_ptr<Entity> dec = m_entities.addEntity(tag);
            auto &animation = dec->addComponent<CAnimation>(m_game->assets().getAnimation(N));

            animation.animation.getSprite().setScale(SX, SY);

            Vec2 pos = gridToMidPixel(X, Y, dec);
            animation.animation.getSprite().setPosition(sf::Vector2f(pos.x, pos.y));

            if (N == "Slime" || N == "Toad") {
                dec->addComponent<CBoundingBox>(Vec2(64, 64));
                dec->addComponent<CTransform>(pos, Vec2(2, 0), 0);
            } else if (N == "Coin") {
                dec->addComponent<CBoundingBox>(Vec2(32, 32));
                dec->addComponent<CTransform>(pos, Vec2(0, 0), 0);
            } else if (N == "Gumball") {
                dec->addComponent<CBoundingBox>(Vec2(32, 32));
                dec->addComponent<CTransform>(pos, Vec2(0, 0), 0);
            }
        } else if (asset_type == "Player") {
            fin >> m_playerConfig.X >> m_playerConfig.Y;
            fin >> m_playerConfig.CX >> m_playerConfig.CY;
            fin >> m_playerConfig.SX >> m_playerConfig.SY >> m_playerConfig.MAXSPEED;
            fin >> m_playerConfig.GRAVITY;
            fin >> m_playerConfig.WEAPON >> m_playerConfig.WEAPON_SPEED >> m_playerConfig.WEAPON_LIFESPAN;
            fin >> m_playerConfig.FIRERATE;
            fin >> m_playerConfig.XSCALE >> m_playerConfig.YSCALE;

            m_player = m_entities.addEntity("Player");

            auto &animation = m_player->addComponent<CAnimation>(m_game->assets().getAnimation("MegaManIdle"));
            animation.animation.getSprite().setScale(m_playerConfig.XSCALE, m_playerConfig.YSCALE);

            Vec2 pos = gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player);
            auto &transform = m_player->addComponent<CTransform>(pos, Vec2(0, 0), 0);
            transform.scale = Vec2(m_playerConfig.XSCALE, m_playerConfig.YSCALE);

            auto &bb = m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));

            auto &state = m_player->addComponent<CState>();

            auto &gravity = m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);

            auto &cooldowns = m_player->addComponent<CCooldown>();

            cooldowns.registerCooldown("ATTACK", m_playerConfig.FIRERATE);
            cooldowns.clearCooldown("ATTACK");

        } else {
            fin >> asset_type;
        }
    }
}

void Scene_Play::update() {
    m_currentFrame++;
    m_entities.update();

    if (!m_levelEnded) {
        sMovement();
        sCollision();
        sAnimation();
    }

    for (auto e : m_entities.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            CAnimation &animation = e->getComponent<CAnimation>();
            if (e->hasComponent<CTransform>()) {
                Vec2 pos = e->getComponent<CTransform>().pos;
                animation.animation.getSprite().setPosition(sf::Vector2f(pos.x, pos.y));
            }
            animation.animation.update();

            bool animationEnded = !animation.animation.isInfinite() && animation.animation.hasEnded();
            if (animationEnded) {
                e->destroy();
                continue;
            }
        }
        if (e->hasComponent<CLifeSpan>()) {
            e->getComponent<CLifeSpan>().remaining--;
            if (e->tag() == "Bullet" && e->getComponent<CLifeSpan>().remaining <= 0) {
                e->getComponent<CAnimation>().animation = m_game->assets().getAnimation("BulletExplosion");
                e->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);
                e->getComponent<CAnimation>().animation.setInfinite(false);
                e->getComponent<CTransform>().velocity = Vec2(0, 0);
                e->removeComponent<CLifeSpan>();
            }
        }
        if (e->hasComponent<CCooldown>()) {
            e->getComponent<CCooldown>().update();
        }
        if (e->hasComponent<CState>()) {
            CState &state = e->getComponent<CState>();

            for (auto it = state.timers.begin(); it != state.timers.end(); it++) {
                if (--it->second <= 0) {
                    state.timers.erase(it);
                }
            }
        }
    }

    // Score text update
    std::stringstream ss;
    ss << "Score: " << m_playerScore;
    m_scoreText.setString(ss.str());
    sf::FloatRect textRect = m_scoreText.getGlobalBounds();
    m_scoreText.setPosition(sf::Vector2f(
        m_playerView.getCenter().x - m_playerView.getSize().x / 2.0f + m_scoreTextPadding,
        m_playerView.getCenter().y - m_playerView.getSize().y / 2.0f + m_scoreTextPadding
    ));

    // Center player view
    CTransform &player_transform = m_player->getComponent<CTransform>();
    float viewHalfWidth = m_playerView.getSize().x / 2.0f;
    if (player_transform.pos.x - viewHalfWidth < 0) {
        m_playerView.setCenter(m_game->window().getSize().x / 2.0f,
            m_game->window().getSize().y / 2.0f);
    } else if (player_transform.pos.x + viewHalfWidth >
               (m_levelWidth + 1) * m_gridSize.x) {
        m_playerView.setCenter(
            (m_levelWidth + 1) * m_gridSize.x - viewHalfWidth,
            m_game->window().getSize().y / 2.0f
        );
    } else {
        m_playerView.setCenter(sf::Vector2f(
            player_transform.pos.x,
            m_game->window().getSize().y / 2.0f
        ));
    }

}

void Scene_Play::sDoAction(const Action &action) {
    // Temporary solution to disable user input if there is no record
    // of the input in the replay file's list of actions.
    if (m_replay && !m_replay_action_map.count(m_currentFrame)) return;

    auto &input = m_player->getComponent<CInput>();
    auto &state = m_player->getComponent<CState>();
    auto &cooldowns = m_player->getComponent<CCooldown>();
    if (m_replay_stream.is_open()) {
        m_replay_stream 
            << action.getType() << " " 
            << action.getName() << " " 
            << m_currentFrame   << "\n";
    } 

    if (action.getType() == "START") {
        if (action.getName() == "LEFT" && !input.left) {
            input.left = true;
        } else if (action.getName() == "RIGHT" && !input.right) {
            input.right = true;
        } else if (action.getName() == "JUMP") {
            input.up = true;
        } else if (action.getName() == "SHOOT") {
            bool can_shoot = 
                state.can_shoot && 
                !state.on_flagpole &&
                cooldowns.getCooldown("ATTACK") <= 0;
            if (can_shoot) {
                m_sound.setBuffer(m_game->assets().getSoundBuffer("Gunshot"));
                m_sound.play();

                spawnBullet();
                m_player->getComponent<CState>().can_shoot = false;
                cooldowns.startCooldown("ATTACK");
            }
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
        } else if (action.getName() == "SHOOT") {
            m_player->getComponent<CState>().can_shoot = true;
        } else if (action.getName() == "TOGGLE_COLLISION") {
            if (m_drawCollision)
                m_drawCollision = false;
            else
                m_drawCollision = true;
        } else if (action.getName() == "TOGGLE_TEXTURE") {
            if (m_drawTextures)
                m_drawTextures = false;
            else
                m_drawTextures = true;
        } else if (action.getName() == "TOGGLE_GRID") {
            if (m_drawGrid) m_drawGrid = false;
            else m_drawGrid = true;
        }
    }
}

void Scene_Play::sMovement() {
    CInput      input            = m_player->getComponent<CInput>();
    CTransform& player_transform = m_player->getComponent<CTransform>();
    CAnimation& player_animation = m_player->getComponent<CAnimation>();
    CState&     player_state     = m_player->getComponent<CState>();
    CGravity&   player_gravity   = m_player->getComponent<CGravity>();

    Vec2 player_velocity(0, player_transform.velocity.y);
    if (input.left) {
        player_velocity.x -= m_playerConfig.SX;
    }
    if (input.right) {
        player_velocity.x += m_playerConfig.SX;
    }
    // Add initial velocity if jump key is pressed
    if (input.up && player_state.on_ground) {
        player_velocity.y = m_playerConfig.SY;
    }
    // Set velocity to 0 if jump key is released
    if (!input.up && !player_state.on_ground) {
        if (player_velocity.y < 0)
            player_velocity.y = 0;
    }

    player_transform.velocity = player_velocity;

    if (player_state.on_flagpole) {
        player_transform.pos.y += 2;
        // TODO: figure out a way to select the correct flag because
        // if there is more than one flag allowed in a level, they will
        // all move up even if the player is not on that flagpole.
        for (auto flag: m_entities.getEntities("Flag")) {
            flag->getComponent<CTransform>().pos.y -= 2;
        }
        return;
    }

    for (auto e : m_entities.getEntities()) {
        if (e->hasComponent<CTransform>()) {
            if (e->hasComponent<CGravity>() && !e->getComponent<CState>().on_ground) {
                e->getComponent<CTransform>().velocity.y +=
                    e->getComponent<CGravity>().gravity;
            }

            // Cap speeds to maxspeed
            auto &t = e->getComponent<CTransform>();
            if (t.velocity.x > m_playerConfig.MAXSPEED)
                t.velocity.x = m_playerConfig.MAXSPEED;
            if (t.velocity.y > m_playerConfig.MAXSPEED)
                t.velocity.y = m_playerConfig.MAXSPEED;
            if (t.velocity.x < m_playerConfig.MAXSPEED * -1)
                t.velocity.x = m_playerConfig.MAXSPEED * -1;
            if (t.velocity.y < m_playerConfig.MAXSPEED * -1)
                t.velocity.y = m_playerConfig.MAXSPEED * -1;
            e->getComponent<CTransform>().prevPos = e->getComponent<CTransform>().pos;
            e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
        }
    }
}

void Scene_Play::sAnimation() {
    CTransform&   player_transform = m_player->getComponent<CTransform>();
    CAnimation&   player_animation = m_player->getComponent<CAnimation>();
    CBoundingBox& player_bb        = m_player->getComponent<CBoundingBox>();
    CState&       player_state     = m_player->getComponent<CState>();
    CInput&       input            = m_player->getComponent<CInput>();

    if (m_levelEnded) {
        auto scale = player_animation.animation.getSprite().getScale();
        player_animation.animation = m_game->assets().getAnimation("MegaManIdle");
        player_animation.animation.getSprite().setScale(scale);
        return;
    }

    bool is_idle = !input.left && !input.right;
    bool net_zero_horizontal_movement = input.left && input.right;
    bool player_has_run_animation = player_animation.animation.getName() == "MegaManRun";
    bool player_facing_left = player_animation.animation.getSprite().getScale().x > 0;

    if (input.left && (!player_has_run_animation || !player_facing_left)) {
        player_animation.animation = m_game->assets().getAnimation("MegaManRun");
        player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE, m_playerConfig.YSCALE);
    }
    if (input.right && (!player_has_run_animation || player_facing_left)) {
        player_animation.animation = m_game->assets().getAnimation("MegaManRun");
        player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE * -1, m_playerConfig.YSCALE);
    }

    if (is_idle || net_zero_horizontal_movement) {
        if (player_animation.animation.getName() != "MegaManIdle") {
            float direction = player_animation.animation.getSprite().getScale().x / m_playerConfig.XSCALE;

            player_animation.animation = m_game->assets().getAnimation("MegaManIdle");
            player_animation.animation.getSprite().setScale(
                m_playerConfig.XSCALE * direction,
                m_playerConfig.YSCALE
            );
        }
    }
    if (!player_state.on_ground) {
        float direction = player_animation.animation.getSprite().getScale().x / m_playerConfig.XSCALE;

        player_animation.animation = m_game->assets().getAnimation("MegaManJump");
        player_animation.animation.getSprite().setScale(
            player_transform.scale.x * direction,
            player_transform.scale.y
        );
    }

    // Energy bar
    CCooldown &cooldowns = m_player->getComponent<CCooldown>();
    float energyPercentage = 1 - cooldowns.getCooldown("ATTACK") / m_playerConfig.FIRERATE;
    float playerWidth = player_bb.size.x;
    float energyBarWidth = playerWidth * energyPercentage;

    // Hide the bar when it is finished charging
    if (energyBarWidth >= playerWidth) {
        m_energyBar.setSize(sf::Vector2f(0, m_energyBarHeight));
        m_energyBarOutline.setOutlineColor(sf::Color::Transparent);
        m_energyBarOutline.setFillColor(sf::Color::Transparent);
    } else {
        m_energyBar.setSize(sf::Vector2f(energyBarWidth, m_energyBarHeight));
        m_energyBarOutline.setSize(sf::Vector2f(playerWidth, m_energyBarHeight));
        m_energyBarOutline.setOutlineColor(sf::Color::Black);
        m_energyBarOutline.setFillColor(sf::Color::Transparent);
    }
    m_energyBar.setPosition(
        player_transform.pos.x - player_bb.size.x / 2,
        player_transform.pos.y + player_bb.size.y / 2
    );
    m_energyBarOutline.setPosition(
        player_transform.pos.x - player_bb.size.x / 2,
        player_transform.pos.y + player_bb.size.y / 2
    );
}

void Scene_Play::destroyBlock(std::shared_ptr<Entity> e) {
    // Play coin animation
    std::shared_ptr<Entity> coin = m_entities.addEntity("Coin");
    coin->addComponent<CTransform>(e->getComponent<CTransform>().pos, Vec2(0.0, -5.0), 0);
    coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"));
    coin->getComponent<CAnimation>().animation.getSprite().setScale(2.5, 2.5);
    coin->getComponent<CAnimation>().animation.setSpeed(5);
    coin->getComponent<CAnimation>().animation.setInfinite(false);
    
    m_playerScore += m_coinValue;

    e->getComponent<CAnimation>().animation = m_game->assets().getAnimation("BulletExplosion");
    e->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);
    e->getComponent<CAnimation>().animation.setInfinite(false);
    e->removeComponent<CBoundingBox>();
}

void Scene_Play::destroyEnemy(std::shared_ptr<Entity> e) {
    e->destroy();
    m_playerScore += m_coinValue;
}

void Scene_Play::destroyPlayer() {
    if (canDestroyEntity(m_player)) {
        m_player->destroy();
        m_levelEnded = true;
        m_levelWon = false;
    }
}

void Scene_Play::sCollision() {
    CState&                 player_state               = m_player->getComponent<CState>();
    CTransform&             player_transform           = m_player->getComponent<CTransform>();
    bool                    hit_block_from_below       = false;
    float                   max_horizontal_overlap     = 0;
    float                   hit_block_vertical_overlap = 0;
    std::shared_ptr<Entity> block_to_destroy           = nullptr;

    player_state.on_ground = false;

    for (auto tile : m_entities.getEntities("Block")) {
        Vec2  overlap     = Physics::GetOverlap(m_player, tile);
        Vec2  prevOverlap = Physics::GetPreviousOverlap(m_player, tile);
        auto& tile_transform   = tile->getComponent<CTransform>();

        bool overlapping = overlap.y >= 0 && overlap.x >= 0;
        bool vertical_collision = prevOverlap.x > 0;
        bool horizontal_collision = prevOverlap.y > 0;
        bool came_from_above = player_transform.pos.y <= tile_transform.pos.y;
        bool came_from_below = player_transform.pos.y >= tile_transform.pos.y;
        bool came_from_left  = player_transform.pos.x <= tile_transform.pos.x;
        bool came_from_right = player_transform.pos.x >= tile_transform.pos.x;

        // TODO: pull these conditions into meaningful variables
        if (overlapping && vertical_collision && came_from_above) {
            if (player_state.on_flagpole) {
                m_levelEnded = true;
                m_levelWon = true;
                m_playerScore += std::floor(player_state.flag_contact_height / m_coinValue) * m_coinValue;
                return;
            }
            player_state.on_ground = true;
            player_transform.pos.y -= overlap.y;
            player_transform.velocity.y = 0;
            continue;
        } else if (overlapping && vertical_collision && came_from_below) {
            if (!hit_block_from_below) hit_block_from_below = true;
            if (overlap.x > max_horizontal_overlap) {
                max_horizontal_overlap = overlap.x;
                hit_block_vertical_overlap = overlap.y;
                block_to_destroy = tile;
            }
            continue;
        } else if (overlapping && horizontal_collision && came_from_left) {
            player_transform.pos.x -= overlap.x;
            player_transform.velocity.x = 0;
            continue;
        } else if (overlapping && horizontal_collision && came_from_right) {
            player_transform.pos.x += overlap.x;
            player_transform.velocity.x = 0;
            continue;
        }
    }
    if (hit_block_from_below) {
        player_transform.pos.y += hit_block_vertical_overlap;
        player_transform.velocity.y = 0;
        destroyBlock(block_to_destroy);
    }

    for (auto slime : m_entities.getEntities("Slime")) {
        CTransform& transform = slime->getComponent<CTransform>();
        CAnimation& animation = slime->getComponent<CAnimation>();

        // Player collision checking
        Vec2 overlap     = Physics::GetOverlap(m_player, slime);
        Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, slime);

        bool overlapping          = overlap.y >= 0 && overlap.x >= 0;
        bool vertical_collision   = prevOverlap.x > 0;
        bool horizontal_collision = prevOverlap.y > 0;
        bool came_from_above      = player_transform.pos.y <= transform.pos.y;

        if (overlapping && vertical_collision && came_from_above) {
            destroyEnemy(slime);
            continue;
        } else if (overlapping) {
            destroyPlayer();
        }

        // Block collision checking
        for (auto block : m_entities.getEntities("Block")) {
            Vec2 overlap     = Physics::GetOverlap(block, slime);
            Vec2 prevOverlap = Physics::GetPreviousOverlap(block, slime);

            bool overlapping          = overlap.y >= 0 && overlap.x >= 0;
            bool horizontal_collision = prevOverlap.y > 0;
            if (overlapping && horizontal_collision) {
                transform.velocity.x *= -1;
                auto& scale = animation.animation.getSprite().getScale();
                animation.animation.getSprite().setScale(scale.x * -1, scale.y);
            }
        }

        // Level boundary collision checking
        CBoundingBox& boundingBox = slime->getComponent<CBoundingBox>();
        if (transform.pos.x - boundingBox.halfSize.x <= 0 || 
            transform.pos.x + boundingBox.halfSize.x >= (m_levelWidth + 1) * m_gridSize.x) {
            transform.velocity.x *= -1;
            auto& scale = animation.animation.getSprite().getScale();
            animation.animation.getSprite().setScale(scale.x * -1, scale.y);
        }
    }
    for (auto bullet : m_entities.getEntities("Bullet")) {
        for (auto tile : m_entities.getEntities("Block")) {
            CTransform& bullet_transform = bullet->getComponent<CTransform>();
            Vec2        overlap          = Physics::GetOverlap(tile, bullet);
            Vec2        prevOverlap      = Physics::GetPreviousOverlap(tile, bullet);

            bool overlapping = overlap.y >= 0 && overlap.x >= 0;
            if (overlapping) {
                bullet->destroy();
                destroyBlock(tile);
            }
        }
        for (auto slime : m_entities.getEntities("Slime")) {
            CTransform& bullet_transform = bullet->getComponent<CTransform>();
            Vec2        overlap          = Physics::GetOverlap(slime, bullet);
            Vec2        prevOverlap      = Physics::GetPreviousOverlap(slime, bullet);

            bool overlapping = overlap.y >= 0 && overlap.x >= 0;
            if (overlapping) {
                bullet->destroy();
                slime->getComponent<CAnimation>().animation = m_game->assets().getAnimation("BulletExplosion");
                slime->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);
                slime->getComponent<CAnimation>().animation.setInfinite(false);
                slime->getComponent<CTransform>().velocity = Vec2(0, 0);
                slime->removeComponent<CBoundingBox>();

                m_playerScore += m_coinValue;
            }
        }
    }
    for (auto coin : m_entities.getEntities("Coin")) {
        Vec2 overlap = Physics::GetOverlap(m_player, coin);

        bool overlapping = overlap.y >= 0 && overlap.x >= 0;
        if (overlapping) {
            m_playerScore += m_coinValue;
            coin->removeComponent<CBoundingBox>();
            coin->destroy();
        }
    }
    for (auto gumball : m_entities.getEntities("Gumball")) {
        Vec2 overlap = Physics::GetOverlap(m_player, gumball);

        bool overlapping = overlap.y >= 0 && overlap.x >= 0;
        if (overlapping) {
            CState &state = m_player->getComponent<CState>();
            // TODO: Make this value represent a number in seconds
            // when updates are no longer tied to framerate.
            state.timers["invincibility"] = 500;

            gumball->destroy();
        }
    }

    // Flagpole collision checking
    for (auto flagpole : m_entities.getEntities("Flagpole")) {
        Vec2 overlap     = Physics::GetOverlap(m_player, flagpole);
        Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, flagpole);

        bool overlapping = overlap.y >= 0 && overlap.x >= 0;
        if (overlapping) {
            player_state.on_flagpole = true;
            player_state.flag_contact_height = player_transform.pos.x;
            for (auto bullet: m_entities.getEntities("Bullet")) {
                bullet->destroy();
            }
        }
    }
}

void Scene_Play::sRender() {
    // Do an action if the replay has one on the current frame
    if (m_replay && m_replay_action_map.count(m_currentFrame)) {
        sDoAction(*m_replay_action_map.at(m_currentFrame));
    }
    if (m_scene_ended) return;

    // Update
    update();

    // Clear
    m_game->window().clear(m_backgroundColor);

    // Draw
    for (auto e : m_entities.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            if (m_drawTextures) {
                m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
            }
        }
        if (e->hasComponent<CBoundingBox>()) {
            if (m_drawCollision) {
                auto& transform = e->getComponent<CTransform>();
                auto& bb        = e->getComponent<CBoundingBox>();

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
        if (e->hasComponent<CState>()) {
            CState     &state = e->getComponent<CState>();
            CTransform &transform = e->getComponent<CTransform>();

            // Draw a barrier if the entity is invincible
            if (state.timers.count("invincibility")) {
                sf::Sprite barrier;
                barrier.setScale(transform.scale.x * 1.5f, transform.scale.y * 1.5f);
                barrier.setTexture(m_game->assets().getTexture("BarrierTex"));
                barrier.setOrigin(barrier.getTexture()->getSize().x / 2.0f, barrier.getTexture()->getSize().y / 2.0f);
                barrier.setPosition(transform.pos.x, transform.pos.y);
                m_game->window().draw(barrier);
            }
        }
    }
    m_game->window().draw(m_scoreText);
    m_game->window().draw(m_energyBar);
    m_game->window().draw(m_energyBarOutline);

    if (m_levelEnded) {
        sf::Text endingText;
        m_levelWon ? 
            endingText.setString("You Win!") : 
            endingText.setString("Game Over!");
        endingText.setFont(m_game->assets().getFont("PrimaryFont"));
        endingText.setCharacterSize(32);
        endingText.setFillColor(sf::Color::White);
        endingText.setOrigin(
            endingText.getGlobalBounds().width / 2.0f,
            endingText.getGlobalBounds().height / 2.0f
        );
        endingText.setPosition(
            m_playerView.getCenter().x,
            m_playerView.getCenter().y
        );
        m_game->window().draw(endingText);
    }
    if (m_drawGrid) {
        for (auto line : m_gridLines) {
            m_game->window().draw(line);
        }
        for (auto label : m_gridLabels) {
            m_game->window().draw(label);
        }
    }

    // Display
    m_game->window().setView(m_playerView);
    m_game->window().display();
}

void Scene_Play::spawnBullet() {
    CTransform& player_transform = m_player->getComponent<CTransform>();
    CAnimation& player_animation = m_player->getComponent<CAnimation>();
    int         direction        = 1;

    bool facing_left = player_animation.animation.getSprite().getScale().x > 0;
    if (facing_left) direction = -1;

    std::shared_ptr<Entity> bullet = m_entities.addEntity("Bullet");
    bullet->addComponent<CLifeSpan>(m_playerConfig.WEAPON_LIFESPAN);
    bullet->addComponent<CBoundingBox>(Vec2(16, 16));
    bullet->addComponent<CAnimation>(m_game->assets().getAnimation(m_playerConfig.WEAPON));
    bullet->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);
    bullet->addComponent<CTransform>(
        player_transform.pos, Vec2(m_playerConfig.WEAPON_SPEED * direction, 0),
        0
    );
}
