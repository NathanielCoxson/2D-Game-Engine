#pragma once

#include <SFML/Graphics.hpp>
#include "Vec2.hpp"
#include <string>

class Animation {
    std::string m_name;
    sf::Sprite  m_sprite;
    Vec2        m_size = { 0.0, 0.0 };
    size_t      m_frameCount;
    size_t      m_currentFrame;
    size_t      m_speed;

public:
    Animation(); 
    Animation(const std::string& name, const sf::Texture& t);
    Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed);

    void update();
    bool hasEnded() const;

    const std::string& getName()   const;
    const Vec2&        getSize()   const;
    sf::Sprite&        getSprite();
};
