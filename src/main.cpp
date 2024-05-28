#include <SFML/Graphics.hpp>
#include "GameEngine.hpp"
#include "Scene_Menu.hpp"

int main() {

    GameEngine gameEngine = GameEngine();
    Scene_Menu menu_scene = Scene_Menu(&gameEngine);

    while (gameEngine.window().isOpen()) {
        sf::Event event;
        while (gameEngine.window().pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                gameEngine.window().close();
            }
        }

        menu_scene.sRender();

    }

    return 0;
}
