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
