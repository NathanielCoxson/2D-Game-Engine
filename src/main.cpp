#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>

int main() {
	sf::Font myFont;

	if (!myFont.loadFromFile("bin/Roboto-Regular.ttf")) {
		std::cout << "Font not found\n";
	}

	std::ifstream infile;
	infile.open("bin/config.txt");
    int         wWidth;
    int         wHeight;
    std::string objectType;
    std::string fontFileName;
    if (infile.is_open()) {
        std::cout << "Config loaded successfully\n";

        // Load window dimensions and font information
        infile >> objectType >> wWidth >> wHeight;
        infile >> objectType >> fontFileName;
    } else {
        std::cout << "File failed to open\n";
        exit(1);
    }

    // Read shapes
    std::vector<std::tuple<sf::CircleShape, float, float>>    circles;
    std::vector<std::tuple<sf::RectangleShape, float, float>> rectangles;
    int         x, y;
    int         red, green, blue;
    int         radius;
    int         width, height;
    float       dx, dy;
    std::string color;
    while (infile) {
        infile >> objectType;
        if (objectType.compare("Circle") == 0) {
            infile >> color >> x >> y >> dx >> dy;
            infile >> red >> green >> blue;
            infile >> radius;

            sf::CircleShape circle(radius);
            sf::Color color(red, green, blue, 255);
            circle.setFillColor(color);
            circle.setPosition(x, y);
            circles.push_back(std::make_tuple(circle, dx, dy));
        } else if (objectType.compare("Rectangle") == 0) {
            infile >> color >> x >> y >> dx >> dy;
            infile >> red >> green >> blue;
            infile >> width >> height;

            sf::RectangleShape rectangle(sf::Vector2f(width, height));
            sf::Color          color(red, green, blue, 255);
            rectangle.setFillColor(color);
            rectangle.setPosition(x, y);
            rectangles.push_back(std::make_tuple(rectangle, dx, dy));
        }
    }
    
    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML Game");

	sf::Event e;
	while (window.isOpen()) {
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear(sf::Color::Black);

        for (auto &c: circles) {
            std::get<0>(c).move(std::get<1>(c), std::get<2>(c));
            window.draw(std::get<0>(c));
        }
        for (auto &r: rectangles) {
            std::get<0>(r).move(std::get<1>(r), std::get<2>(r));
            window.draw(std::get<0>(r));
        }

		window.display();
	}

	return 0;
}
