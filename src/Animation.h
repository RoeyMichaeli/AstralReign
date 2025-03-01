#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

using sf::Vector2f;

class Animation {
    sf::Sprite m_sprite;       // Sprite constructed with a texture.
    size_t m_frameCount = 1;   // Total number of frames.
    size_t m_currentFrame = 0; // Current frame index.
    size_t m_speed = 1;        // Animation speed (frames per update or similar).
    Vector2f m_size = { 1.f, 1.f }; // Size of one frame.
    std::string m_id;          // String-based identifier for the animation.

public:
    // Constructors now take a string for the animation ID.
    Animation(const std::string& id, const sf::Texture& t);
    Animation(const std::string& id, const sf::Texture& t, size_t frameCount, size_t speed);

    void update();
    bool hasEnded() const;

    // Returns the animation ID (as string).
    const std::string& getID() const;

    const Vector2f& getSize() const;
    const sf::Sprite& getSprite() const;
    sf::Sprite& getSprite();

    void reset();
};

#endif // ANIMATION_H
