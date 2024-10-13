#include "doctest.hpp"
#include "../src/Components.hpp"
#include <SFML/Graphics.hpp>

TEST_CASE("CBoundingBox has correct initial size using default constructor.") {
    CBoundingBox bb = CBoundingBox();

    CHECK(bb.size.x == 0);
    CHECK(bb.size.y == 0);
}

TEST_CASE("CBoundingBox has correct initial size when width and height are provided.") {
    CBoundingBox bb = CBoundingBox(Vec2(1.0, 2.0));
    
    CHECK(bb.size.x == 1.0);
    CHECK(bb.size.y == 2.0);
}

TEST_CASE("CAnimation correctly initializes the animation data member.") {
    sf::Texture empty_texture = sf::Texture();
    CAnimation animation = CAnimation("empty_animation", empty_texture, 1, 10); 

    CHECK(animation.animation.getName().compare("empty_animation") == 0);
    CHECK(animation.animation.getSize().x == 0);
    CHECK(animation.animation.getSize().y == 0);
}

TEST_CASE("CCooldown correctly inserts a cooldown.") {
    CCooldown cooldowns = CCooldown();
    cooldowns.registerCooldown("ATTACK", 100);

    CHECK(cooldowns.cooldowns["ATTACK"][0] == 100);
    CHECK(cooldowns.cooldowns["ATTACK"][1] == 100);
}

TEST_CASE("CCooldown correctly decrements cooldowns.") {
    CCooldown cooldowns = CCooldown();
    cooldowns.registerCooldown("ATTACK", 2);

    CHECK(cooldowns.cooldowns["ATTACK"][0] == 2);
    cooldowns.update();
    CHECK(cooldowns.cooldowns["ATTACK"][0] == 1);
    CHECK(cooldowns.cooldowns["ATTACK"][1] == 2);
    cooldowns.update();
    CHECK(cooldowns.cooldowns["ATTACK"][0] == 0);
    CHECK(cooldowns.cooldowns["ATTACK"][1] == 2);
    cooldowns.update();
    CHECK(cooldowns.cooldowns["ATTACK"][0] == 0);
    CHECK(cooldowns.cooldowns["ATTACK"][1] == 2);
}

TEST_CASE("CCooldown correctly resets cooldowns.") {
    CCooldown cooldowns = CCooldown();
    cooldowns.registerCooldown("ATTACK", 10);

    CHECK(cooldowns.getCooldown("ATTACK") == 10);
    cooldowns.update();
    CHECK(cooldowns.getCooldown("ATTACK") == 9);
    cooldowns.startCooldown("ATTACK");
    CHECK(cooldowns.getCooldown("ATTACK") == 10);
}

TEST_CASE("CGravity is initialized correctly.") {
    float epsilon = 0.01;
    CGravity gravity = CGravity(0.8);
    
    CHECK(std::abs(gravity.gravity - 0.8) <= epsilon);
}
