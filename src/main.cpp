#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>

int main() {
	const int wWidth = 640;
	const int wHeight = 480;
	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML Game");
	sf::CircleShape circle(50);
	circle.setFillColor(sf::Color::Green);
	circle.setPosition(300, 300);

	sf::Font myFont;

	if (!myFont.loadFromFile("bin/Roboto-Regular.ttf")) {
		std::cout << "Font not found\n";
	}

	std::ifstream infile;
	infile.open("input.txt");
	int input;
	infile >> input;
	std::cout << input << "\n";

	sf::Event e;
	while (window.isOpen()) {
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear(sf::Color::Black);

		window.draw(circle);

		window.display();
	}

	return 0;
}
