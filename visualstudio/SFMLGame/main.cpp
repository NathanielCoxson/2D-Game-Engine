#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
	const int wWidth = 640;
	const int wHeight = 480;
	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML Game");
	sf::Event e;

	while (window.isOpen()) {
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}
	}

	return 1;
}