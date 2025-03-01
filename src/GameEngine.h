#pragma once
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <map>
#include <memory>
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include "Scene.h"
#include "Assets.h"
#include "ConfigManager.h"

// Mapping from scene name to scene pointer.
using SceneMap = std::map<std::string, std::shared_ptr<Scene>>;

class GameEngine {
protected:
    sf::RenderWindow m_window;
    sf::Clock        m_deltaClock;
    Assets           m_assets;
    std::string      m_currentScene;
    SceneMap         m_sceneMap;
    size_t           m_simulationSpeed = 1;
    bool             m_running = true;

    // New: configuration manager instance.
    std::unique_ptr<ConfigManager> m_configManager;

    // Initialize the engine using configuration from configPath.
    int init(const std::string& configPath);
    void update();
    void sUserInput();
    std::shared_ptr<Scene> currentScene();

public:
    // The constructor now accepts a configuration file path.
    explicit GameEngine(const std::string& configPath);

    // Scene management
    void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);
    void quit();
    void run();

    // Sound controls.
    void playSound(const std::string& soundName);
    void stopSound(const std::string& soundName);

    // Accessors.
    sf::RenderWindow& window();
    Assets& assets();
    bool isRunning();
};

#endif // GAME_ENGINE_H
