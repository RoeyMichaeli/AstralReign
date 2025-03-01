#include <iostream>
#include <optional>
#include <cstdint> // Include for uint32_t
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "GameEngine.h"
#include "Scene_Menu.h"
#include "ConfigManager.h"
#include "Action.h"
#include "Logger.h"


// Constructor: load configuration and initialize engine.
GameEngine::GameEngine(const std::string& configPath) {
    m_configManager = std::make_unique<ConfigManager>();
    init(configPath);
}

int GameEngine::init(const std::string& configPath) {
    EngineConfig config = m_configManager->load(configPath);
    m_assets.loadFromFile(config.assetConfigPath);

    // SFML 3.0 window style handling
    if (config.fullscreen)
        m_window.create(sf::VideoMode::getDesktopMode(), "AstralReign", sf::State::Fullscreen);
    else
        m_window.create(sf::VideoMode({ config.windowWidth, config.windowHeight }), "AstralReign", sf::State::Windowed);

    m_window.setFramerateLimit(config.frameRateLimit);
    m_window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(m_window)) {
        std::cerr << "Failed to initialize ImGui-SFML!";
            return -1; // Exit early if ImGui fails to initialize
    }
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;
    if (!ImGui::SFML::Init(m_window)) {
        std::cerr << "Failed ImGui initialization\n";
    }

    changeScene("MENU", std::make_shared<Scene_Menu>(this));
    return 0;
}

std::shared_ptr<Scene> GameEngine::currentScene() {
    return m_sceneMap[m_currentScene];
}

bool GameEngine::isRunning() {
    return m_running && m_window.isOpen();
}

sf::RenderWindow& GameEngine::window() {
    return m_window;
}

void GameEngine::run() {
    while (isRunning()) {
        sUserInput();
        ImGui::SFML::Update(m_window, m_deltaClock.restart());
        update();
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
}

void GameEngine::sUserInput() {
    std::optional<sf::Event> eventOpt;
    while ((eventOpt = m_window.pollEvent())) {
        ImGui::SFML::ProcessEvent(m_window, *eventOpt);

        if (auto* closeEvent = eventOpt->getIf<sf::Event::Closed>()) {
            LOG("Window close event detected");
            quit();
            continue;
        }

        if (auto* keyEvent = eventOpt->getIf<sf::Event::KeyPressed>()) {
            LOG("Key pressed: " + std::to_string(static_cast<int>(keyEvent->code)));
            ActionType type = ActionType::Start;
            int key = static_cast<int>(keyEvent->code);
            const auto& actionMap = currentScene()->getActionMap();
            if (actionMap.find(key) != actionMap.end()) {
                ActionName actionName = actionMap.at(key);
                currentScene()->doAction(Action(actionName, type));
            }
        }

        if (auto* keyEvent = eventOpt->getIf<sf::Event::KeyReleased>()) {
            LOG("Key released: " + std::to_string(static_cast<int>(keyEvent->code)));
            ActionType type = ActionType::End;
            int key = static_cast<int>(keyEvent->code);
            const auto& actionMap = currentScene()->getActionMap();
            if (actionMap.find(key) != actionMap.end()) {
                ActionName actionName = actionMap.at(key);
                currentScene()->doAction(Action(actionName, type));
            }
        }

        if (auto* mouseEvent = eventOpt->getIf<sf::Event::MouseButtonPressed>()) {
            ActionType type = ActionType::Start;
            sf::Vector2f pos(static_cast<float>(mouseEvent->position.x), static_cast<float>(mouseEvent->position.y));
            switch (mouseEvent->button) {
            case sf::Mouse::Button::Left:
                currentScene()->doAction(Action(ActionName::LeftClick, type, pos));
                break;
            case sf::Mouse::Button::Middle:
                currentScene()->doAction(Action(ActionName::MiddleClick, type, pos));
                break;
            case sf::Mouse::Button::Right:
                currentScene()->doAction(Action(ActionName::RightClick, type, pos));
                break;
            default:
                break;
            }
        }

        if (auto* mouseEvent = eventOpt->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2f pos(static_cast<float>(mouseEvent->position.x), static_cast<float>(mouseEvent->position.y));
            currentScene()->doAction(Action(ActionName::MouseMove, pos));
        }

        if (auto* scrollEvent = eventOpt->getIf<sf::Event::MouseWheelScrolled>()) {
            // Use a zoom factor based on scroll direction:
            // (For instance, scroll up (delta > 0) zooms in, scroll down zooms out.)
            if ((scrollEvent->delta > 0))
                currentScene()->doAction(Action(ActionName::ScrollUp, ActionType::Start));
            else
                currentScene()->doAction(Action(ActionName::ScrollDown, ActionType::Start));
        }
    }
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool /*endCurrentScene*/) {
    m_currentScene = sceneName;
    m_sceneMap[sceneName] = std::move(scene);
}

void GameEngine::quit() {
    m_running = false;
    m_window.close();
}

void GameEngine::update() {
    currentScene()->update();
}

void GameEngine::playSound(const std::string& soundName) {
    m_assets.getSound(soundName).play();
}

void GameEngine::stopSound(const std::string& soundName) {
    m_assets.getSound(soundName).stop();
}

Assets& GameEngine::assets() {
    return m_assets;
}
