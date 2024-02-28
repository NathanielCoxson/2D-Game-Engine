#include <SFML/Graphics.hpp>

int main() {
	const int wWidth = 640;
	const int wHeight = 480;
	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML Game");
	sf::Event e;

    sf::CircleShape circle(50);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(300.0f, 300.0f);
    float circleMoveSpeed = 0.01f;

	while (window.isOpen()) {
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}

        window.clear(sf::Color::Black);

        circle.move(circleMoveSpeed, circleMoveSpeed);
        
        window.draw(circle);

        window.display();
	}

	return 0;
}
