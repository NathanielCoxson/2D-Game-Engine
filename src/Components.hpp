#pragma once

#include "Vec2.hpp"
#include <SFML/Graphics.hpp>

class CScore {

public:
    int score;
    CScore(int s): score(s) {};
};

class CTransform {
    
public: 
    Vec2  pos      = { 0.0, 0.0 };
    Vec2  velocity = { 0.0, 0.0 };
    float angle    = 0;

    CTransform(const Vec2& p, const Vec2& v, float a)
        : pos(p), velocity(v), angle(a) {}
};

class CShape {

public:
    sf::CircleShape circle;

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

class CCollision {

public:
    float radius = 0;
    CCollision(float r): radius(r) {}
};

class CLifespan {

public:
    int remaining = 0; // amount of lifespan remaining
    int total     = 0; // total initial lifespan

    CLifespan(int t): total(t), remaining(t) {}
};

class CInput {

public:
    bool up    = false;
    bool down  = false;
    bool left  = false;
    bool right = false;
    bool shoot = false;

    CInput() {};
};

class CBoundingBox {

public:
    sf::Vector2f size = { 0.0, 0.0 }; 

    CBoundingBox() {};
    CBoundingBox(float w, float h): size(w, h) {}; 
};
