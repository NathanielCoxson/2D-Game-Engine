#include "Physics.hpp"
#include "Components.hpp"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    bool a_is_valid = a->hasComponent<CTransform>() && a->hasComponent<CBoundingBox>();
    bool b_is_valid = b->hasComponent<CTransform>() && b->hasComponent<CBoundingBox>();
    if (!(a_is_valid && b_is_valid)) return Vec2(-1, -1);

    CTransform&   aTransform   = a->getComponent<CTransform>();  
    CTransform&   bTransform   = b->getComponent<CTransform>();  
    CBoundingBox& aBoundingBox = a->getComponent<CBoundingBox>();
    CBoundingBox& bBoundingBox = b->getComponent<CBoundingBox>();

    float x1 = aTransform.pos.x, x2 = bTransform.pos.x;
    float y1 = aTransform.pos.y, y2 = bTransform.pos.y;

    float w1 = aBoundingBox.halfSize.x;
    float w2 = bBoundingBox.halfSize.x;
    float h1 = aBoundingBox.halfSize.y;
    float h2 = bBoundingBox.halfSize.y;

    Vec2 delta = Vec2(std::abs(x1 - x2), std::abs(y1 - y2));
    float ox = w1 + w2 - delta.x;
    float oy = h1 + h2 - delta.y;
    
    return Vec2(ox, oy);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b) {
    bool a_is_valid = a->hasComponent<CTransform>() && a->hasComponent<CBoundingBox>();
    bool b_is_valid = b->hasComponent<CTransform>() && b->hasComponent<CBoundingBox>();
    if (!(a_is_valid && b_is_valid)) return Vec2(-1, -1);

    CTransform&   aTransform   = a->getComponent<CTransform>();  
    CTransform&   bTransform   = b->getComponent<CTransform>();  
    CBoundingBox& aBoundingBox = a->getComponent<CBoundingBox>();
    CBoundingBox& bBoundingBox = b->getComponent<CBoundingBox>();

    float x1 = aTransform.prevPos.x, x2 = bTransform.prevPos.x;
    float y1 = aTransform.prevPos.y, y2 = bTransform.prevPos.y;

    float w1 = aBoundingBox.halfSize.x;
    float w2 = bBoundingBox.halfSize.x;
    float h1 = aBoundingBox.halfSize.y;
    float h2 = bBoundingBox.halfSize.y;

    Vec2 delta = Vec2(std::abs(x1 - x2), std::abs(y1 - y2));
    float ox = w1 + w2 - delta.x;
    float oy = h1 + h2 - delta.y;
    
    return Vec2(ox, oy);
}
