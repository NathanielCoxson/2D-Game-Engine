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

  m_gridText.setCharacterSize(12);
  m_gridText.setFont(m_game->assets().getFont("PrimaryFont"));

  m_scoreText.setCharacterSize(32);
  m_scoreText.setFont(m_game->assets().getFont("PrimaryFont"));
  m_scoreText.setFillColor(sf::Color::White);

  loadLevel(m_levelPath);

  m_playerView.setSize(
      sf::Vector2f(m_game->window().getSize().x, m_game->window().getSize().y));
}

void Scene_Play::onEnd() {
  m_game->window().setView(m_game->window().getDefaultView());
  m_game->changeScene("menu", nullptr, true);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY,
                                std::shared_ptr<Entity> entity) {
  auto &Animation = entity->getComponent<CAnimation>();

  float x = m_gridSize.x * gridX +
            (Animation.animation.getSprite().getGlobalBounds().width / 2.0f);
  float y = m_gridSize.y * gridY +
            (Animation.animation.getSprite().getGlobalBounds().height / 2.0f);

  return Vec2(x, m_game->window().getSize().y - y);
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
      auto &animation =
          tile->addComponent<CAnimation>(m_game->assets().getAnimation(N));
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

      if (int(X) > m_levelWidth)
        m_levelWidth = int(X);

      std::shared_ptr<Entity> dec = m_entities.addEntity(N);
      auto &animation =
          dec->addComponent<CAnimation>(m_game->assets().getAnimation(N));

      animation.animation.getSprite().setScale(SX, SY);

      Vec2 pos = gridToMidPixel(X, Y, dec);
      animation.animation.getSprite().setPosition(sf::Vector2f(pos.x, pos.y));

      if (N == "Slime") {
          dec->addComponent<CBoundingBox>(Vec2(64, 64));
          dec->addComponent<CTransform>(pos, Vec2(2, 0), 0);
      } else if (N == "Coin") {
          dec->addComponent<CBoundingBox>(Vec2(32, 32));
          dec->addComponent<CTransform>(pos, Vec2(0, 0), 0);
      }
    } else if (asset_type == "Player") {
      fin >> m_playerConfig.X >> m_playerConfig.Y;
      fin >> m_playerConfig.CX >> m_playerConfig.CY;
      fin >> m_playerConfig.SX >> m_playerConfig.SY >> m_playerConfig.MAXSPEED;
      fin >> m_playerConfig.GRAVITY;
      fin >> m_playerConfig.WEAPON >> m_playerConfig.WEAPON_SPEED >>
          m_playerConfig.WEAPON_LIFESPAN;
      fin >> m_playerConfig.XSCALE >> m_playerConfig.YSCALE;

      m_player = m_entities.addEntity("Player");

      auto &animation = m_player->addComponent<CAnimation>(
          m_game->assets().getAnimation("MegaManIdle"));
      animation.animation.getSprite().setScale(m_playerConfig.XSCALE,
                                               m_playerConfig.YSCALE);

      Vec2 pos = gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player);
      auto &transform = m_player->addComponent<CTransform>(pos, Vec2(0, 0), 0);
      transform.scale = Vec2(m_playerConfig.XSCALE, m_playerConfig.YSCALE);

      auto &bb = m_player->addComponent<CBoundingBox>(
          Vec2(m_playerConfig.CX, m_playerConfig.CY));

      auto &state = m_player->addComponent<CState>();

      auto &gravity = m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
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

            bool animationEnded =
                !animation.animation.isInfinite() && animation.animation.hasEnded();
            if (animationEnded) {
                e->destroy();
                continue;
            }
        }
        if (e->hasComponent<CLifeSpan>()) {
            e->getComponent<CLifeSpan>().remaining--;
            if (e->tag() == "Bullet" && e->getComponent<CLifeSpan>().remaining <= 0) {
                e->getComponent<CAnimation>().animation =
                    m_game->assets().getAnimation("BulletExplosion");
                e->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);
                e->getComponent<CAnimation>().animation.setInfinite(false);
                e->getComponent<CTransform>().velocity = Vec2(0, 0);
                e->removeComponent<CLifeSpan>();
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
        m_playerView.setCenter(sf::Vector2f(player_transform.pos.x,
            m_game->window().getSize().y / 2.0f));
    }

}

void Scene_Play::sDoAction(const Action &action) {
    auto &input = m_player->getComponent<CInput>();
    auto &state = m_player->getComponent<CState>();

    if (action.getType() == "START") {
        if (action.getName() == "LEFT" && !input.left) {
            input.left = true;
        } else if (action.getName() == "RIGHT" && !input.right) {
            input.right = true;
        } else if (action.getName() == "JUMP") {
            input.up = true;
        } else if (action.getName() == "SHOOT") {
            bool can_shoot = state.can_shoot && !state.on_flagpole;
            if (can_shoot) {
                spawnBullet();
                m_player->getComponent<CState>().can_shoot = false;
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
        }
    }
}

void Scene_Play::sMovement() {
    CInput input = m_player->getComponent<CInput>();
    CTransform &player_transform = m_player->getComponent<CTransform>();
    CAnimation &player_animation = m_player->getComponent<CAnimation>();
    CState &player_state = m_player->getComponent<CState>();
    CGravity &player_gravity = m_player->getComponent<CGravity>();

    Vec2 player_velocity(0, m_player->getComponent<CTransform>().velocity.y);
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
            e->getComponent<CTransform>().pos +=
                e->getComponent<CTransform>().velocity;
        }
    }
}

void Scene_Play::sAnimation() {
  CTransform &player_transform = m_player->getComponent<CTransform>();
  CAnimation &player_animation = m_player->getComponent<CAnimation>();
  CState &player_state = m_player->getComponent<CState>();
  CInput &input = m_player->getComponent<CInput>();

  if (m_levelEnded) {
      auto scale = player_animation.animation.getSprite().getScale();
      player_animation.animation = m_game->assets().getAnimation("MegaManIdle");
      player_animation.animation.getSprite().setScale(scale);
      return;
  }

  bool is_idle = !input.left && !input.right;
  bool net_zero_horizontal_movement = input.left && input.right;
  bool player_has_run_animation =
      player_animation.animation.getName() == "MegaManRun";
  bool player_facing_left =
      player_animation.animation.getSprite().getScale().x > 0;

  if (input.left && (!player_has_run_animation || !player_facing_left)) {
    player_animation.animation = m_game->assets().getAnimation("MegaManRun");
    player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE,
                                                    m_playerConfig.YSCALE);
  }
  if (input.right && (!player_has_run_animation || player_facing_left)) {
    player_animation.animation = m_game->assets().getAnimation("MegaManRun");
    player_animation.animation.getSprite().setScale(m_playerConfig.XSCALE * -1,
                                                    m_playerConfig.YSCALE);
  }

  if (is_idle || net_zero_horizontal_movement) {
    if (player_animation.animation.getName() != "MegaManIdle") {
      float direction = player_animation.animation.getSprite().getScale().x /
                        m_playerConfig.XSCALE;

      player_animation.animation = m_game->assets().getAnimation("MegaManIdle");
      player_animation.animation.getSprite().setScale(
          m_playerConfig.XSCALE * direction, m_playerConfig.YSCALE);
    }
  }
  if (!player_state.on_ground) {
    float direction = player_animation.animation.getSprite().getScale().x /
                      m_playerConfig.XSCALE;

    player_animation.animation = m_game->assets().getAnimation("MegaManJump");
    player_animation.animation.getSprite().setScale(
        player_transform.scale.x * direction, player_transform.scale.y);
  }
}

void Scene_Play::destroyBlock(std::shared_ptr<Entity> e) {
    // Play coin animation
    std::shared_ptr<Entity> coin = m_entities.addEntity("Coin");
    coin->addComponent<CTransform>(e->getComponent<CTransform>().pos, Vec2(0.0, -5.0), 0);
    coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"));
    coin->getComponent<CAnimation>().animation.getSprite().setScale(2.5, 2.5);
    coin->getComponent<CAnimation>().animation.setSpeed(5);
    coin->getComponent<CAnimation>().animation.setInfinite(false);
    m_playerScore += 100;

    e->getComponent<CAnimation>().animation =
        m_game->assets().getAnimation("BulletExplosion");
    e->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);
    e->getComponent<CAnimation>().animation.setInfinite(false);
    e->removeComponent<CBoundingBox>();

}

void Scene_Play::destroyEnemy(std::shared_ptr<Entity> e) {
    e->destroy();
    m_playerScore += 100;
}

void Scene_Play::sCollision() {
    CState &player_state = m_player->getComponent<CState>();
    CTransform &player_transform = m_player->getComponent<CTransform>();

    player_state.on_ground = false;
    for (auto tile : m_entities.getEntities("Block")) {
        Vec2 overlap = Physics::GetOverlap(m_player, tile);
        Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);

        bool overlapping = overlap.y >= 0 && overlap.x >= 0;
        bool vertical_collision = prevOverlap.x > 0;
        bool horizontal_collision = prevOverlap.y > 0;
        bool came_from_above =
            player_transform.pos.y <= tile->getComponent<CTransform>().pos.y;
        bool came_from_below =
            player_transform.pos.y >= tile->getComponent<CTransform>().pos.y;
        bool came_from_left =
            player_transform.pos.x <= tile->getComponent<CTransform>().pos.x;
        bool came_from_right =
            player_transform.pos.x >= tile->getComponent<CTransform>().pos.x;

        if (overlapping && vertical_collision && came_from_above) {
            if (player_state.on_flagpole) {
                m_levelEnded = true;
                m_playerScore += std::floor(player_state.flag_contact_height / 100) * 100;
                return;
            }
            player_state.on_ground = true;
            player_transform.pos.y -= overlap.y;
            player_transform.velocity.y = 0;
            continue;
        } else if (overlapping && vertical_collision && came_from_below) {
            player_transform.pos.y += overlap.y;
            player_transform.velocity.y = 0;
            destroyBlock(tile);
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
    for (auto slime : m_entities.getEntities("Slime")) {
        // Player collision checking
        Vec2 overlap = Physics::GetOverlap(m_player, slime);
        Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, slime);

        bool overlapping = overlap.y >= 0 && overlap.x >= 0;
        bool vertical_collision = prevOverlap.x > 0;
        bool horizontal_collision = prevOverlap.y > 0;
        bool came_from_above =
            player_transform.pos.y <= slime->getComponent<CTransform>().pos.y;

        if (overlapping && vertical_collision && came_from_above) {
            destroyEnemy(slime);
            continue;
        } else if (overlapping) {
            m_player->destroy();
        }

        // Block collision checking
        for (auto block : m_entities.getEntities("Block")) {
            Vec2 overlap = Physics::GetOverlap(block, slime);
            Vec2 prevOverlap = Physics::GetPreviousOverlap(block, slime);

            bool overlapping = overlap.y >= 0 && overlap.x >= 0;
            bool horizontal_collision = prevOverlap.y > 0;
            if (overlapping && horizontal_collision) {
                slime->getComponent<CTransform>().velocity.x *= -1;
            }
        }

        // Level boundary collision checking
        CTransform &transform = slime->getComponent<CTransform>();
        CBoundingBox &boundingBox = slime->getComponent<CBoundingBox>();
        if (transform.pos.x - boundingBox.halfSize.x <= 0 ||
                transform.pos.x + boundingBox.halfSize.x >=
                (m_levelWidth + 1) * m_gridSize.x) {
            transform.velocity.x *= -1;
        }
    }
    for (auto bullet : m_entities.getEntities("Bullet")) {
        for (auto tile : m_entities.getEntities("Block")) {
            CTransform &bullet_transform = bullet->getComponent<CTransform>();
            Vec2 overlap = Physics::GetOverlap(tile, bullet);
            Vec2 prevOverlap = Physics::GetPreviousOverlap(tile, bullet);

            bool overlapping = overlap.y >= 0 && overlap.x >= 0;
            if (overlapping) {
                bullet->destroy();
                destroyBlock(tile);
            }
        }
        for (auto slime : m_entities.getEntities("Slime")) {
            CTransform &bullet_transform = bullet->getComponent<CTransform>();
            Vec2 overlap = Physics::GetOverlap(slime, bullet);
            Vec2 prevOverlap = Physics::GetPreviousOverlap(slime, bullet);

            bool overlapping = overlap.y >= 0 && overlap.x >= 0;
            if (overlapping) {
                bullet->destroy();
                slime->getComponent<CAnimation>().animation =
                    m_game->assets().getAnimation("BulletExplosion");
                slime->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);
                slime->getComponent<CAnimation>().animation.setInfinite(false);
                slime->getComponent<CTransform>().velocity = Vec2(0, 0);
                slime->removeComponent<CBoundingBox>();
                m_playerScore += 100;
            }
        }
    }
    for (auto coin : m_entities.getEntities("Coin")) {
        Vec2 overlap = Physics::GetOverlap(m_player, coin);

        bool overlapping = overlap.y >= 0 && overlap.x >= 0;

        if (overlapping) {
            m_playerScore += 200;
            coin->removeComponent<CBoundingBox>();
            coin->destroy();
        }
    }

    // Flagpole collision checking
    for (auto flagpole : m_entities.getEntities("Flagpole")) {
        Vec2 overlap = Physics::GetOverlap(m_player, flagpole);
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
    // Update
    update();

    // Clear
    m_game->window().clear(m_backgroundColor);

    // Draw
    m_game->window().draw(m_scoreText);
    for (auto e : m_entities.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            if (m_drawTextures) {
                m_game->window().draw(
                        e->getComponent<CAnimation>().animation.getSprite());
            }
        }
        if (e->hasComponent<CBoundingBox>()) {
            if (m_drawCollision) {
                auto &transform = e->getComponent<CTransform>();
                auto &bb = e->getComponent<CBoundingBox>();
                sf::RectangleShape outline;
                outline.setSize(sf::Vector2f(bb.size.x, bb.size.y));
                outline.setOrigin(
                        sf::Vector2f(outline.getSize().x / 2, outline.getSize().y / 2));
                outline.setPosition(sf::Vector2f(transform.pos.x, transform.pos.y));
                outline.setOutlineThickness(2);
                outline.setOutlineColor(sf::Color::Blue);
                outline.setFillColor(sf::Color::Transparent);
                m_game->window().draw(outline);
            }
        }
    }
    if (m_levelEnded) {
        sf::Text endingText;
        endingText.setString("You Win!");
        endingText.setFont(m_game->assets().getFont("PrimaryFont"));
        endingText.setCharacterSize(32);
        endingText.setFillColor(sf::Color::White);
        endingText.setOrigin(endingText.getGlobalBounds().width / 2.0f,
                endingText.getGlobalBounds().height / 2.0f);
        endingText.setPosition(m_playerView.getCenter().x,
                m_playerView.getCenter().y);
        m_game->window().draw(endingText);
    }

    // Display
    m_game->window().setView(m_playerView);
    m_game->window().display();
}

void Scene_Play::spawnBullet() {
  CTransform &player_transform = m_player->getComponent<CTransform>();
  CAnimation &player_animation = m_player->getComponent<CAnimation>();
  int direction = 1;
  bool facing_left = player_animation.animation.getSprite().getScale().x > 0;
  if (facing_left)
    direction = -1;

  std::shared_ptr<Entity> bullet = m_entities.addEntity("Bullet");

  bullet->addComponent<CLifeSpan>(m_playerConfig.WEAPON_LIFESPAN);

  bullet->addComponent<CBoundingBox>(Vec2(16, 16));

  bullet->addComponent<CAnimation>(
      m_game->assets().getAnimation(m_playerConfig.WEAPON));
  bullet->getComponent<CAnimation>().animation.getSprite().setScale(4, 4);

  bullet->addComponent<CTransform>(
      player_transform.pos, Vec2(m_playerConfig.WEAPON_SPEED * direction, 0),
      0);
}
