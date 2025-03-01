#include "ConfigManager.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

EngineConfig ConfigManager::load(const std::string& configPath) {
    EngineConfig config;

    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Could not open configuration file: " + configPath);
    }

    try {
        json j;
        configFile >> j;

        // Populate configuration settings, using default values if keys are missing.
        if (j.contains("windowWidth"))
            config.windowWidth = j["windowWidth"].get<unsigned int>();
        if (j.contains("windowHeight"))
            config.windowHeight = j["windowHeight"].get<unsigned int>();
        if (j.contains("windowTitle"))
            config.windowTitle = j["windowTitle"].get<std::string>();
        if (j.contains("frameRateLimit"))
            config.frameRateLimit = j["frameRateLimit"].get<unsigned int>();
        if (j.contains("fullscreen"))
            config.fullscreen = j["fullscreen"].get<bool>();
        if (j.contains("assetConfigPath"))
            config.assetConfigPath = j["assetConfigPath"].get<std::string>();

    }
    catch (const json::exception& e) {
        std::cerr << "Error parsing configuration file: " << e.what() << std::endl;
        throw;
    }

    return config;
}
