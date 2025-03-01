#pragma once
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>

// EngineConfig holds settings for the game engine.
struct EngineConfig {
    unsigned int windowWidth = 1280;
    unsigned int windowHeight = 768;
    std::string  windowTitle = "Astral Reign";
    unsigned int frameRateLimit = 60;
    bool         fullscreen = false;
    // Path to the asset configuration file (to be loaded by Assets)
    std::string  assetConfigPath;
};

class ConfigManager {
public:
    ConfigManager() = default;
    ~ConfigManager() = default;

    // Loads engine configuration from the given file path.
    // The implementation should support formats like XML, JSON, YAML, etc.
    EngineConfig load(const std::string& configPath);
};

#endif // CONFIG_MANAGER_H
