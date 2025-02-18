#include "Assets.hpp"
#include <iostream>

Assets::Assets() {}

void Assets::addTexture(std::string name, std::string path) {
    sf::Texture newTexture;

    if (!newTexture.loadFromFile(path)) {
        std::cout << "Error loading texture with name: " << name << "\n";
    }

    textures[name] = newTexture;
}

void Assets::addAnimation(std::string name, Animation animation) {
    animations[name] = animation;
}

void Assets::addSoundBuffer(std::string name, std::string path) {
    sf::SoundBuffer newSoundBuffer;

    if (!newSoundBuffer.loadFromFile(path)) {
        std::cout << "Error loading sound with name: " << name << "\n";
    }

    sounds[name] = newSoundBuffer;
}

void Assets::addFont(std::string name, std::string path) {
    sf::Font newFont;

    if (!newFont.loadFromFile(path)) {
        std::cout << "Error loading font with name: " << name << "\n";
    }

    fonts[name] = newFont;
}

const sf::Texture& Assets::getTexture(std::string name) {
    return textures[name];
}

Animation& Assets::getAnimation(std::string name) {
    return animations[name];
}

const sf::SoundBuffer& Assets::getSoundBuffer(std::string name) {
    return sounds[name];
}

const sf::Font& Assets::getFont(std::string name) {
    return fonts[name];
}

void Assets::removeTexture(std::string name) {
    textures.erase(name);
}

void Assets::removeAnimation(std::string name) {
    animations.erase(name);
}

void Assets::removeSound(std::string name) {
    sounds.erase(name);
}

void Assets::removeFont(std::string name) {
    fonts.erase(name);
}
