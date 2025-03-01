#ifndef SCENE_H
#define SCENE_H

#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include "Action.h"

// Using a map to associate input keys with action names.
using ActionMap = std::unordered_map<int, ActionName>;

class GameEngine;

class Scene {
protected:
    GameEngine* m_game = nullptr;
    // Use entt registry for managing entities and components (see Components.hpp for component definitions).
    entt::registry m_registry;
    ActionMap m_actionMap;
    bool m_paused = false;
    bool m_hasEnded = false;
    size_t m_currentFrame = 0;
    float m_music_volume = 25.0f;

    // Called when the scene ends – must be implemented by derived scenes.
    virtual void onEnd() = 0;

    // Set the pause state.
    void setPaused(bool paused);

public:
    Scene() = default;
    explicit Scene(GameEngine* gameEngine);
    virtual ~Scene() = default;

    // Update logic specific to the scene.
    virtual void update() = 0;

    // Handle input actions; to be implemented by derived scenes.
    virtual void sDoAction(const Action& action) = 0;

    // Render the scene; must be implemented by subclasses.
    virtual void sRender() = 0;

    // Wraps the scene-specific action handler.
    virtual void doAction(const Action& action);

    // Simulate the scene for a given number of frames (default: no simulation step).
    virtual void simulate(size_t frames);

    // Associate an input key with an action name.
    void registerAction(int inputKey, const ActionName& actionName);

    // Convenience accessors.
    [[nodiscard]] float width() const;
    [[nodiscard]] float height() const;
    [[nodiscard]] size_t currentFrame() const;
    [[nodiscard]] bool hasEnded() const;
    [[nodiscard]] const ActionMap& getActionMap() const;

    // Draw a line between two points.
    void drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2);
};

#endif // SCENE_H
