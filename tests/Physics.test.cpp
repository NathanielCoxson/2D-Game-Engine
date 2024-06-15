#include "doctest.hpp"
#include "../src/Physics.hpp"
#include "../src/Entity.hpp"
#include "../src/EntityManager.hpp"
#include "../src/Vec2.hpp"
#include "../src/Components.hpp"

TEST_CASE("GetOverlap returns the correct overlap for non-overlapping entities") {
    Physics phys;
    EntityManager entities;
    auto a = entities.addEntity("a");
    auto b = entities.addEntity("b");

    a->addComponent<CTransform>();
    b->addComponent<CTransform>();
    a->addComponent<CBoundingBox>(Vec2(10, 10));
    b->addComponent<CBoundingBox>(Vec2(10, 10));

    a->getComponent<CTransform>().pos = Vec2(5, 5);
    b->getComponent<CTransform>().pos = Vec2(16, 16);

    Vec2 overlap = phys.GetOverlap(a, b);

    CHECK(overlap.x == -1);
    CHECK(overlap.y == -1);
}

TEST_CASE("GetOverlap returns the correct overlap for overlapping entities") {
    Physics phys;
    EntityManager entities;
    auto a = entities.addEntity("a");
    auto b = entities.addEntity("b");

    a->addComponent<CTransform>();
    b->addComponent<CTransform>();
    a->addComponent<CBoundingBox>(Vec2(10, 10));
    b->addComponent<CBoundingBox>(Vec2(10, 10));

    a->getComponent<CTransform>().pos = Vec2(5, 5);
    b->getComponent<CTransform>().pos = Vec2(6, 6);

    Vec2 overlap = phys.GetOverlap(a, b);

    CHECK(overlap.x == 9);
    CHECK(overlap.y == 9);
}

TEST_CASE("GetPreviousOverlap returns the correct overlap for non-overlapping entities") {
    Physics phys;
    EntityManager entities;
    auto a = entities.addEntity("a");
    auto b = entities.addEntity("b");

    a->addComponent<CTransform>();
    b->addComponent<CTransform>();
    a->addComponent<CBoundingBox>(Vec2(10, 10));
    b->addComponent<CBoundingBox>(Vec2(10, 10));

    a->getComponent<CTransform>().prevPos = Vec2(5, 5);
    b->getComponent<CTransform>().prevPos = Vec2(16, 16);

    Vec2 overlap = phys.GetPreviousOverlap(a, b);

    CHECK(overlap.x == -1);
    CHECK(overlap.y == -1);
}

TEST_CASE("GetPreviousOverlap returns the correct overlap for overlapping entities") {
    Physics phys;
    EntityManager entities;
    auto a = entities.addEntity("a");
    auto b = entities.addEntity("b");

    a->addComponent<CTransform>();
    b->addComponent<CTransform>();
    a->addComponent<CBoundingBox>(Vec2(10, 10));
    b->addComponent<CBoundingBox>(Vec2(10, 10));

    a->getComponent<CTransform>().prevPos = Vec2(5, 5);
    b->getComponent<CTransform>().prevPos = Vec2(14, 14);

    Vec2 overlap = phys.GetPreviousOverlap(a, b);

    CHECK(overlap.x == 1);
    CHECK(overlap.y == 1);
}
