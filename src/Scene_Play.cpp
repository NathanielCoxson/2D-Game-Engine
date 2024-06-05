#include "Scene_Play.hpp"
#include "Vec2.hpp"

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

void Scene_Play::loadLevel(const std::string& path) {
    m_player = m_entities.addEntity("player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("SlimeJumpAnimation"));
    m_player->getComponent<CAnimation>().animation.getSprite().setPosition(sf::Vector2f(100, 100));

    m_player->addComponent<CTransform>();
    m_player->getComponent<CTransform>().pos.x = 100;
    m_player->getComponent<CTransform>().pos.y = 100;

    m_player->addComponent<CInput>();
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
    if (action.getType() == "START") {
        if (action.getName() == "LEFT") {
            m_player->getComponent<CInput>().left = true;
        } else if (action.getName() == "RIGHT") {
            m_player->getComponent<CInput>().right = true;
        } else if (action.getName() == "QUIT") {
            onEnd();
        }
    } else if (action.getType() == "END") {
        if (action.getName() == "LEFT") {
            m_player->getComponent<CInput>().left = false;
        } else if (action.getName() == "RIGHT") {
            m_player->getComponent<CInput>().right = false;
        }
    }
}

void Scene_Play::sMovement() {
    CInput input = m_player->getComponent<CInput>();
    CTransform& player_transform = m_player->getComponent<CTransform>();

    if (input.left) {
        player_transform.pos.x -= 5;
    } else if (input.right) {
        player_transform.pos.x += 5;
    }
}

void Scene_Play::sRender() {
    //Update
    update();

    //Clear
    m_game->window().clear(sf::Color::Black);

    //Draw
    m_game->window().draw(m_player->getComponent<CAnimation>().animation.getSprite());

    //Display
    m_game->window().display();
}


