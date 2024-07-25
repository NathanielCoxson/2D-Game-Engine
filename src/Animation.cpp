#include "Animation.hpp"
#include <cmath>

Animation::Animation() {}

Animation::Animation(const std::string& name, const sf::Texture& t)
    : Animation(name, t, 1, 1) {}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
    : m_name         (name)
    , m_sprite       (t)
    , m_frameCount   (frameCount)
    , m_currentFrame (0)
    , m_speed        (speed)
{
    m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}

void Animation::update() {
    m_currentFrame++;
    float animFrame = (m_currentFrame / m_speed) % m_frameCount;
    sf::IntRect rectangle = sf::IntRect(std::floor(animFrame) * m_size.x, 0, m_size.x, m_size.y);
    m_sprite.setTextureRect(rectangle);
}

bool Animation::hasEnded() const {
    bool allFramesPlayed = (m_currentFrame / m_speed) >= m_frameCount;
    return allFramesPlayed;
}

const std::string& Animation::getName() const {
    return m_name;
}

const Vec2& Animation::getSize() const {
    return m_size;
}

sf::Sprite& Animation::getSprite() {
    return m_sprite;
}

bool Animation::isInfinite() const {
    return m_infinite;
}

void Animation::setInfinite(bool is_infinite) {
    m_infinite = is_infinite;
}

void Animation::setSpeed(size_t speed) {
    m_speed = speed;
}
