#pragma once

#include "Vec2.hpp"
#include <SFML/Graphics.hpp>
#include "Animation.hpp"

class Component {

public:
    bool has = false;
};

class CScore : public Component {

public:
    int score;
    CScore(): CScore(0) {}
    CScore(int s): score(s) {}
};

class CTransform : public Component {
    
public: 
    Vec2  pos      = { 0.0, 0.0 };
    Vec2  prevPos  = { 0.0, 0.0 };
    Vec2  velocity = { 0.0, 0.0 };
    Vec2  scale    = { 1.0, 1.0 };
    float angle    = 0;

    CTransform() {}
    CTransform(const Vec2& p, const Vec2& v, float a)
        : pos(p), prevPos(p), velocity(v), angle(a) {}
};

class CShape : public Component {

public:
    sf::CircleShape circle;

    CShape(): CShape(0, 0, sf::Color(0, 0, 0, 0), sf::Color(0, 0, 0, 0), 0) {}
    CShape( float            radius, 
            int              points, 
            const sf::Color& fill,
            const sf::Color& outline, 
            float            thickness) 
        : circle(radius, points) {

        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius);
    }
};

class CCollision : public Component {

public:
    float radius = 0;
    CCollision(): CCollision(0) {}
    CCollision(float r): radius(r) {}
};

class CLifeSpan : public Component {

public:
    int remaining = 0; // amount of lifespan remaining
    int total     = 0; // total initial lifespan

    CLifeSpan() {}
    CLifeSpan(int t): total(t), remaining(t) {}
};

class CInput : public Component {

public:
    bool up    = false;
    bool down  = false;
    bool left  = false;
    bool right = false;
    bool shoot = false;

    CInput() {}
};

class CBoundingBox : public Component {

public:
    Vec2 size = { 0.0, 0.0 }; 
    Vec2 halfSize = { 0.0, 0.0 };

    CBoundingBox() {}
    CBoundingBox(Vec2 s)
        : size(s.x, s.y)
        , halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component {

public:
    Animation animation;

    CAnimation() {}
    CAnimation(Animation& animation): animation(animation) {};
    CAnimation(const std::string& name, const sf::Texture& t)
        : CAnimation(name, t, 1, 0) {};
    CAnimation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
        : animation(name, t, frameCount, speed) {}
};

class CGravity : public Component {

public:
    float gravity;

    CGravity() {}
    CGravity(float g): gravity(g) {};
};

class CState : public Component {

public:
    bool on_ground = true;
    bool can_shoot = true;
    int jump_frame_count = 0;
    CState() {}
};
