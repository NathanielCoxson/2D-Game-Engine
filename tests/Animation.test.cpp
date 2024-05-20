#include "doctest.hpp"
#include "../src/Animation.hpp"
#include <SFML/Graphics.hpp>

TEST_CASE("Animations are initialized correctly.") {
    sf::Texture empty_texture = sf::Texture(); 

    Animation test_animation = Animation("empty_animation", empty_texture, 1, 10);

    CHECK(test_animation.getName().compare("empty_animation") == 0);
    CHECK(test_animation.getSize().x == 0);
    CHECK(test_animation.getSize().y == 0);
}
