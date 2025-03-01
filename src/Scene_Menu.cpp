#include <iostream>
#include "SFML/Graphics/View.hpp"
#include "SFML/Window/Event.hpp"
#include "Action.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Scene_Menu.h"
#include "Logger.h"
#include "Scene_Galaxy.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Menu::init() {
    // Center the view.
    sf::View view = m_game->window().getView();
    view.setCenter(sf::Vector2f{
        static_cast<float>(m_game->window().getSize().x) / 2.0f,
        static_cast<float>(m_game->window().getSize().y) / 2.0f
        });
    m_game->window().setView(view);

    // Register input actions using enum values.
    registerAction(static_cast<int>(sf::Keyboard::Scancode::W), ActionName::Up);
    registerAction(static_cast<int>(sf::Keyboard::Scancode::S), ActionName::Down);
    registerAction(static_cast<int>(sf::Keyboard::Scancode::D), ActionName::Activate);
    registerAction(static_cast<int>(sf::Keyboard::Scancode::M), ActionName::Mute);
    registerAction(static_cast<int>(sf::Keyboard::Scancode::Escape), ActionName::Quit);

    // Retrieve music from Assets and play it.
    m_music = &m_game->assets().getSound("BackgroundMusic1");
    m_music->play();

    // Set up the menu title.
    m_title = "Astral Reign";
    int titleSize = 30;
    // Construct m_menuText using a valid font. Wrap m_title in sf::String.
    m_menuText = std::make_unique<sf::Text>(m_game->assets().getFont("tech"), 
        sf::String(m_title),
        static_cast<unsigned int>(titleSize));
    m_menuText->setFillColor(sf::Color::Black);
    m_menuText->setPosition(sf::Vector2f{
        static_cast<float>(m_game->window().getSize().x) / 2.0f -
        static_cast<float>(titleSize * (m_title.length() + 1)) / 2.0f,
        static_cast<float>(titleSize * 3)
        });

    // Define menu options.
    m_menuStrings.push_back("New Game");
    m_menuStrings.push_back("Load Game");
    m_menuStrings.push_back("Exit");

    // Create text objects for each menu item.
    m_menuItems.clear();
    for (size_t i = 0; i < m_menuStrings.size(); ++i) {
        sf::Text text(m_game->assets().getFont("tech"), 
            sf::String(m_menuStrings[i]),
            26);
        text.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color::Black);
        text.setPosition(sf::Vector2f{
            static_cast<float>(m_game->window().getSize().x) / 2.0f -
            static_cast<float>(26 * (m_menuStrings[i].length() + 1)) / 2.0f,
            m_menuText->getGlobalBounds().position.y + 10.0f + 30.0f * static_cast<float>(i + 1)
            });
        m_menuItems.push_back(text);
    }
}

void Scene_Menu::update() {
    sRender();
}

void Scene_Menu::onEnd() {
    m_game->quit();
}

void Scene_Menu::sDoAction(const Action& action) {
    if (action.type() == ActionType::Start) {
        switch (action.name()) {
        case ActionName::Up:
            if (m_selectedMenuIndex > 0)
                m_selectedMenuIndex--;
            else
                m_selectedMenuIndex = m_menuStrings.size() - 1;
            break;
        case ActionName::Down:
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
            break;
        case ActionName::Activate:
            LOG("On action Activate");
            switch (m_selectedMenuIndex) {
            case 0:
                m_music->stop();
                m_game->changeScene("Galaxy View", std::make_shared<Scene_Galaxy>(m_game));
                break;
            case 1:

                break;
            case 2:
                onEnd();
                break;
            default:
                LOG("Default selected. Doing nothing.");
                break;
            }
             
            break;
        case ActionName::Mute:
            if (m_music->getVolume()) {
                m_music_volume = m_music->getVolume();
                m_music->setVolume(0.0f);
            }
            else {
                m_music->setVolume(m_music_volume);
            }
            break;
        case ActionName::Quit:
            LOG("On action Quit");
            onEnd();
            break;
        default:
            break;
        }
    }
}

void Scene_Menu::sRender() {
    // Clear window with a background color.
    m_game->window().clear(sf::Color(100, 100, 255));

    // Draw the title.
    m_game->window().draw(*m_menuText);

    // Draw each menu item.
    for (size_t i = 0; i < m_menuStrings.size(); ++i) {
        m_menuItems[i].setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color::Black);
        m_menuItems[i].setPosition(sf::Vector2f{
            static_cast<float>(m_game->window().getSize().x) / 2.0f -
            static_cast<float>(26 * (m_menuStrings[i].length() + 1)) / 2.0f,
            m_menuText->getGlobalBounds().position.y + 10.0f + 30.0f * static_cast<float>(i + 1)
            });
        m_game->window().draw(m_menuItems[i]);
    }

    // Draw help text.
    sf::Text help(m_game->assets().getFont("tech"), 
        sf::String("W:UP  S:DOWN  D:ENTER  M:MUTE  ESC:BACK/QUIT"),
        26);
    help.setFillColor(sf::Color::Black);
    help.setPosition(sf::Vector2f{
        static_cast<float>(m_game->window().getSize().x) / 2.0f -
        static_cast<float>(26 * (help.getString().getSize() + 1)) / 2.0f,
        static_cast<float>(m_game->window().getSize().y) - 60.0f
        });
    m_game->window().draw(help);
}
