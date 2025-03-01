#ifndef SCENE_MENU_H
#define SCENE_MENU_H

#include <vector>
#include <string>
#include <memory>
#include "SFML/Graphics/Text.hpp"
#include "Scene.h"

// Scene_Menu is the main menu scene.
class Scene_Menu : public Scene {
protected:
    std::string m_title;
    std::vector<std::string> m_menuStrings;

    // Use a unique_ptr so we can initialize the text once we have a valid font.
    std::unique_ptr<sf::Text> m_menuText;
    std::vector<sf::Text> m_menuItems;

    size_t m_selectedMenuIndex = 0;

    // Pointer to title music (retrieved from Assets – assumed to remain valid)
    sf::Sound* m_music = nullptr;

    // Initialize menu layout and register input actions.
    void init();

    // Update and rendering functions.
    void update() override;
    void onEnd() override;
    void sDoAction(const Action& action) override;

public:
    explicit Scene_Menu(GameEngine* gameEngine = nullptr);

    void sRender() override;
};

#endif // SCENE_MENU_H
