#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <memory>

class Shape {

    public:
    std::shared_ptr<sf::Shape> shape;
    std::shared_ptr<sf::Text>  label;
    float                      dx, dy;

    Shape(
        std::shared_ptr<sf::Shape> s,
        std::shared_ptr<sf::Text>  l,
        float                      sx,
        float                      sy
    ): shape(s), label(l), dx(sx), dy(sy) 
    {}


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
    std::vector<Shape> shapes;
    int                x, y;
    int                red, green, blue;
    int                radius;
    int                width, height;
    float              dx, dy;
    std::string        name;
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

            // Center label
            label.setPosition(x + radius, y + radius);

            shapes.push_back(Shape(
                std::make_shared<sf::CircleShape>(circle),
                std::make_shared<sf::Text>(label),
                dx,
                dy
            ));
        } else if (objectType.compare("Rectangle") == 0) {
            infile >> width >> height;
            sf::RectangleShape rectangle(sf::Vector2f(width, height));
            rectangle.setFillColor(color);
            rectangle.setPosition(x, y);

            // Center label
            label.setPosition(x + width/2.f, y + height/2.f);

            shapes.push_back(Shape(
                std::make_shared<sf::RectangleShape>(rectangle),
                std::make_shared<sf::Text>(label),
                dx,
                dy
            ));
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

        // Clear previous frame
		window.clear(sf::Color::Black);
        
        // Drawing
        for (auto &s: shapes) {
            sf::FloatRect bounds = (s.shape)->getGlobalBounds();

            if (bounds.left <= 0 || (bounds.left + bounds.width) >= wWidth) {
                s.dx *= -1;
            } else if (bounds.top <= 0 || (bounds.top + bounds.height >= wHeight)) {
                s.dy *= -1;
            }
        
            s.shape->move(s.dx, s.dy);
            s.label->move(s.dx, s.dy);
            window.draw(*(s.shape));
            window.draw(*(s.label));
        }

        // Display new frame
		window.display();
	}

	return 0;
}
