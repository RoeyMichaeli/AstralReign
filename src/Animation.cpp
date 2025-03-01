#include <cmath>
#include "Animation.h"

Animation::Animation(const std::string& id, const sf::Texture& t)
    : Animation(id, t, 1, 1)
{
}

Animation::Animation(const std::string& id, const sf::Texture& t, size_t frameCount, size_t speed)
    : m_id(id),
    m_sprite(t, sf::IntRect({ 0, 0 }, { static_cast<int>(t.getSize().x), static_cast<int>(t.getSize().y) })),
    m_frameCount(frameCount),
    m_currentFrame(0),
    m_speed(speed)
{
    // Compute frame size based on the full texture divided by frameCount.
    m_size = Vector2f(static_cast<float>(t.getSize().x) / static_cast<float>(frameCount),
        static_cast<float>(t.getSize().y));
    m_sprite.setOrigin(Vector2f(m_size.x / 2.f, m_size.y / 2.f));

    // Set initial texture rectangle.
    int left = static_cast<int>(std::floor(static_cast<float>(m_currentFrame) * m_size.x));
    int top = 0;
    int width = static_cast<int>(m_size.x);
    int height = static_cast<int>(m_size.y);
    m_sprite.setTextureRect(sf::IntRect({ left, top }, { left + width, top + height }));
}

void Animation::update() {
    m_currentFrame++;
    size_t animationFrame = (m_currentFrame / m_speed) % m_frameCount;
    int left = static_cast<int>(animationFrame * m_size.x);
    int top = 0;
    int width = static_cast<int>(m_size.x);
    int height = static_cast<int>(m_size.y);
    m_sprite.setTextureRect(sf::IntRect({ left, top }, { left + width, top + height }));
}

bool Animation::hasEnded() const {
    return (m_currentFrame / m_speed) % m_frameCount == m_frameCount - 1;
}

const std::string& Animation::getID() const {
    return m_id;
}

const Vector2f& Animation::getSize() const {
    return m_size;
}

const sf::Sprite& Animation::getSprite() const {
    return m_sprite;
}

sf::Sprite& Animation::getSprite() {
    return m_sprite;
}

void Animation::reset() {
    m_currentFrame = 0;
    int left = 0;
    int top = 0;
    int width = static_cast<int>(m_size.x);
    int height = static_cast<int>(m_size.y);
    m_sprite.setTextureRect(sf::IntRect({ left, top }, { left + width, top + height }));
}
