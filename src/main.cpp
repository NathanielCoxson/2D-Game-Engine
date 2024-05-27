#include "Assets.hpp"
#include <SFML/Graphics.hpp>
#include "Animation.hpp"

int main() {
    Assets assets;

    assets.addTexture("block" , "src/assets/tiles/block.png");
    assets.addTexture("slime_jump_sheet", "src/assets/sprites/slime_jump-Sheet.png");
    Animation slime_jump = Animation("slime_jump", assets.getTexture("slime_jump_sheet"), 17, 120);
    slime_jump.getSprite().setScale(sf::Vector2f(0.5, 0.5));
    assets.addAnimation("slime_jump_animation", slime_jump);

    assets.addFont("roboto", "bin/fonts/RobotoRegular.ttf");

    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(800, 600), "Window");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        sf::RectangleShape r = sf::RectangleShape();
        r.setSize(sf::Vector2f(50, 50));
        r.setPosition(sf::Vector2f(100, 100));
        r.setTexture(&assets.getTexture("block"));

        assets.getAnimation("slime_jump_animation").getSprite().setPosition(sf::Vector2f(200, 200));
        window.draw(assets.getAnimation("slime_jump_animation").getSprite());
        assets.getAnimation("slime_jump_animation").update();

        sf::Text text;
        text.setFont(assets.getFont("roboto"));
        text.setPosition(sf::Vector2f(10, 10));
        text.setFillColor(sf::Color::White);
        text.setString("Example Text");
        text.setCharacterSize(24);
        window.draw(text);

        window.draw(r);
        window.display();
    }

    return 0;
}
