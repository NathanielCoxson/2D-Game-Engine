#pragma once

#include "Animation.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

class Assets {
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, Animation>   animations;
    std::map<std::string, sf::SoundBuffer>   sounds;
    std::map<std::string, sf::Font>    fonts;

public:
    Assets();

    void addTexture(std::string name, std::string path);
    void addAnimation(std::string name, Animation animation);
    void addSoundBuffer(std::string name, std::string path);
    void addFont(std::string name, std::string path);

    const sf::Texture& getTexture(std::string name);
    Animation& getAnimation(std::string name);
    const sf::SoundBuffer& getSoundBuffer(std::string name);
    const sf::Font& getFont(std::string name);

    void removeTexture(std::string name);
    void removeAnimation(std::string name);
    void removeSound(std::string name);
    void removeFont(std::string name);
};
