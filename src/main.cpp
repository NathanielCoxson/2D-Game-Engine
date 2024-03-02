#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>

struct CircleEntity {
    sf::CircleShape circle;
    sf::Text        label;
};

int main() {
	std::ifstream infile;
	infile.open("bin/config.txt");
    int         wWidth;
    int         wHeight;
    int         fontSize;
    int         fontRed, fontGreen, fontBlue;
    std::string objectType;
    std::string fontFileName;
	sf::Font    myFont;
    sf::Color   textColor;
    if (infile.is_open()) {
        std::cout << "Config loaded successfully\n";

        // Load window dimensions and font information
        infile >> objectType >> wWidth >> wHeight;
        infile >> objectType >> fontFileName >> fontSize;
        infile >> fontRed >> fontGreen >> fontBlue;

        if (myFont.loadFromFile(fontFileName)) {
            std::cout << "Font loaded successfully\n";
        } else {
            std::cout << "Failed to load font file\n";
            exit(1);
        }

        textColor = sf::Color(fontRed, fontGreen, fontBlue, 255);

    } else {
        std::cout << "Config file failed to open\n";
        exit(1);
    }

    // Read shapes
    std::vector<std::tuple<sf::CircleShape, float, float>>    circles;
    std::vector<std::tuple<sf::RectangleShape, float, float>> rectangles;
    std::vector<std::tuple<sf::Text, float, float>>           labels;
    int         x, y;
    int         red, green, blue;
    int         radius;
    int         width, height;
    float       dx, dy;
    std::string name;
    while (infile) {
        infile >> objectType >> name;
        infile >> x >> y >> dx >> dy;
        infile >> red >> green >> blue;
        
        // Create color
        sf::Color color(red, green, blue, 255);
        
        // Create label
        sf::Text label(name, myFont);
        label.setFillColor(textColor);
        label.setCharacterSize(fontSize);
        sf::FloatRect textRect = label.getLocalBounds();
        label.setOrigin(textRect.left + textRect.width/2.0f,
                        textRect.top + textRect.height/2.0f);

        if (objectType.compare("Circle") == 0) {
            infile >> radius;
            sf::CircleShape circle(radius);
            circle.setFillColor(color);
            circle.setPosition(x, y);
            circles.push_back(std::make_tuple(circle, dx, dy));

            // Center label
            label.setPosition(x + radius, y + radius);
            labels.push_back(std::make_tuple(label, dx, dy));
        } else if (objectType.compare("Rectangle") == 0) {
            infile >> width >> height;
            sf::RectangleShape rectangle(sf::Vector2f(width, height));
            rectangle.setFillColor(color);
            rectangle.setPosition(x, y);
            rectangles.push_back(std::make_tuple(rectangle, dx, dy));

            // Center label
            label.setPosition(x + width/2.f, y + height/2.f);
            labels.push_back(std::make_tuple(label, dx, dy));
        }
    }
    CircleEntity myEntity;
    myEntity.circle = sf::CircleShape(50);
    myEntity.label = sf::Text("Test", myFont, fontSize);
    myEntity.circle.setPosition(200, 200);
    myEntity.circle.setFillColor(sf::Color::Red);
    
    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML Game");
	sf::Event e;
	while (window.isOpen()) {
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}

        // Clear previous frame
		window.clear(sf::Color::Black);
        
        window.draw(myEntity.circle);


        // Drawing
        for (auto &c: circles) {
            sf::CircleShape circle   = std::get<0>(c);
            float           &dx      = std::get<1>(c);
            float           &dy      = std::get<2>(c);
            sf::FloatRect   bounds   = circle.getGlobalBounds();

            if (bounds.left <= 0 || (bounds.left + bounds.width) >= wWidth) {
                dx *= -1;
            } else if (bounds.top <= 0 || (bounds.top + bounds.height >= wHeight)) {
                dy *= -1;
            }

            std::get<0>(c).move(std::get<1>(c), std::get<2>(c));
            window.draw(std::get<0>(c));
        }
        for (auto &r: rectangles) {
            std::get<0>(r).move(std::get<1>(r), std::get<2>(r));
            window.draw(std::get<0>(r));
        }
        for (auto &l: labels) {
            std::get<0>(l).move(std::get<1>(l), std::get<2>(l));
            window.draw(std::get<0>(l));
        }

        // Display new frame
		window.display();
	}

	return 0;
}
