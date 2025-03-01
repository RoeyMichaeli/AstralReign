#include "Assets.h"
#include "nlohmann/json.hpp"
#include "Logger.h"  // Added for logging

using json = nlohmann::json;

void Assets::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG("Could not open asset file: " + path);
        throw std::runtime_error("Could not open asset file: " + path);
    }

    json j;
    file >> j;

    // The JSON file must contain an "assets" array.
    if (!j.contains("assets") || !j["assets"].is_array()) {
        LOG("Invalid asset file format: missing 'assets' array");
        throw std::runtime_error("Invalid asset file format: missing 'assets' array");
    }

    for (const auto& asset : j["assets"]) {
        if (!asset.contains("type") || !asset["type"].is_string()) {
            LOG("Asset entry missing 'type' field or it is not a string");
            throw std::runtime_error("Asset entry missing 'type' field or it is not a string");
        }
        std::string type = asset["type"].get<std::string>();

        if (type == "Texture") {
            // Expect "name" and "path" fields.
            if (!asset.contains("name") || !asset.contains("path")) {
                LOG("Texture asset missing 'name' or 'path'");
                throw std::runtime_error("Texture asset missing 'name' or 'path'");
            }
            std::string name = asset["name"].get<std::string>();
            std::string texturePath = asset["path"].get<std::string>();
            addTexture(name, texturePath);
        }
        else if (type == "Animation") {
            // Expect "name", "texture", "frames", and "speed" fields.
            if (!asset.contains("name") || !asset.contains("texture") ||
                !asset.contains("frames") || !asset.contains("speed")) {
                LOG("Animation asset missing required fields");
                throw std::runtime_error("Animation asset missing required fields");
            }
            std::string animName = asset["name"].get<std::string>();
            std::string texName = asset["texture"].get<std::string>();
            int frames = asset["frames"].get<int>();
            int speed = asset["speed"].get<int>();

            // Retrieve the texture; this will throw if the texture is not found.
            const sf::Texture& tex = getTexture(texName);
            // Use brace initialization to construct an Animation object.
            addAnimation(animName, Animation{ animName, tex, static_cast<size_t>(frames), static_cast<size_t>(speed) });
        }
        else if (type == "Font") {
            // Expect "name" and "path" fields.
            if (!asset.contains("name") || !asset.contains("path")) {
                LOG("Font asset missing 'name' or 'path'");
                throw std::runtime_error("Font asset missing 'name' or 'path'");
            }
            std::string fontName = asset["name"].get<std::string>();
            std::string fontPath = asset["path"].get<std::string>();
            addFont(fontName, fontPath);
        }
        else if (type == "Sound") {
            // Expect "name" and "path" fields.
            if (!asset.contains("name") || !asset.contains("path")) {
                LOG("Sound asset missing 'name' or 'path'");
                throw std::runtime_error("Sound asset missing 'name' or 'path'");
            }
            std::string soundName = asset["name"].get<std::string>();
            std::string soundPath = asset["path"].get<std::string>();
            addSound(soundName, soundPath);
        }
        else {
            LOG("Unknown asset type: " + type);
            throw std::runtime_error("Unknown asset type: " + type);
        }
    }
}

void Assets::addTexture(const std::string& name, const std::string& path) {
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        LOG("Could not load texture from file: " + path);
        throw std::runtime_error("Could not load texture from file: " + path);
    }
    m_textureMap.emplace(name, std::move(texture));
}

void Assets::addAnimation(const std::string& name, const Animation& animation) {
    m_animationMap.emplace(name, animation);
}

void Assets::addFont(const std::string& name, const std::string& path) {
    sf::Font font;
    if (!font.openFromFile(path)) {
        LOG("Could not load font from file: " + path);
        throw std::runtime_error("Could not load font from file: " + path);
    }
    m_fontMap.emplace(name, std::move(font));
}

void Assets::addSound(const std::string& name, const std::string& path) {
    sf::SoundBuffer sb;
    if (!sb.loadFromFile(path)) {
        LOG("Could not load sound from file: " + path);
        throw std::runtime_error("Could not load sound from file: " + path);
    }
    m_soundBuffers[name] = std::move(sb);
    m_sounds.emplace(name, sf::Sound(m_soundBuffers.at(name)));
}

const sf::Texture& Assets::getTexture(const std::string& name) const {
    auto it = m_textureMap.find(name);
    if (it == m_textureMap.end()) {
        LOG("Error: Texture \"" + name + "\" not found.");
        throw std::runtime_error("Texture \"" + name + "\" not found");
    }
    return it->second;
}

const Animation& Assets::getAnimation(const std::string& name) const {
    auto it = m_animationMap.find(name);
    if (it == m_animationMap.end()) {
        LOG("Error: Animation \"" + name + "\" not found.");
        throw std::runtime_error("Animation \"" + name + "\" not found");
    }
    return it->second;
}

const sf::Font& Assets::getFont(const std::string& name) const {
    auto it = m_fontMap.find(name);
    if (it == m_fontMap.end()) {
        LOG("Error: Font \"" + name + "\" not found.");
        throw std::runtime_error("Font \"" + name + "\" not found");
    }
    return it->second;
}

sf::Sound& Assets::getSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it == m_sounds.end()) {
        LOG("Error: Sound \"" + name + "\" not found.");
        throw std::runtime_error("Sound \"" + name + "\" not found");
    }
    return it->second;
}

const std::unordered_map<std::string, sf::Texture>& Assets::getTextures() const {
    return m_textureMap;
}

const std::unordered_map<std::string, Animation>& Assets::getAnimations() const {
    return m_animationMap;
}

std::unordered_map<std::string, sf::Sound>& Assets::getSounds() {
    return m_sounds;
}
