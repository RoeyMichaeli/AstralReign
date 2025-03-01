#pragma once
#ifndef ASSETS_H
#define ASSETS_H

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "Animation.h"

// The Assets class now loads its configuration from a JSON file.
class Assets {
public:
    Assets() = default;
    ~Assets() = default;

    // Loads assets from a JSON file.
    void loadFromFile(const std::string& path);

    [[nodiscard]] const sf::Texture& getTexture(const std::string& name) const;
    [[nodiscard]] const Animation& getAnimation(const std::string& name) const;
    [[nodiscard]] const sf::Font& getFont(const std::string& name) const;
    [[nodiscard]] sf::Sound& getSound(const std::string& name);

    // Accessors for collections.
    [[nodiscard]] const std::unordered_map<std::string, sf::Texture>& getTextures() const;
    [[nodiscard]] const std::unordered_map<std::string, Animation>& getAnimations() const;
    [[nodiscard]] std::unordered_map<std::string, sf::Sound>& getSounds();

private:
    // Helper functions to add assets.
    void addTexture(const std::string& name, const std::string& path);
    void addAnimation(const std::string& name, const Animation& animation);
    void addFont(const std::string& name, const std::string& path);
    void addSound(const std::string& name, const std::string& path);

    std::unordered_map<std::string, sf::Texture> m_textureMap;
    std::unordered_map<std::string, Animation> m_animationMap;
    std::unordered_map<std::string, sf::Font> m_fontMap;
    std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::unordered_map<std::string, sf::Sound> m_sounds;
};

#endif // ASSETS_H
