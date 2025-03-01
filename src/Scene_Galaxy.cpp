#include "Scene_Galaxy.h"
#include "Components.hpp"   // For Transform2D, Renderable, and Input components
#include "GameEngine.h"
#include <SFML/Graphics.hpp>
#include <cmath>

// Global enlargement factor for the background
static constexpr float bgScale = 2.0f;

Scene_Galaxy::Scene_Galaxy(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Galaxy::init() {
	// --- Register Camera Movement Keys ---
	registerAction(static_cast<int>(sf::Keyboard::Scancode::A), ActionName::Left);
	registerAction(static_cast<int>(sf::Keyboard::Scancode::D), ActionName::Right);
	registerAction(static_cast<int>(sf::Keyboard::Scancode::W), ActionName::Up);
	registerAction(static_cast<int>(sf::Keyboard::Scancode::S), ActionName::Down);
	registerAction(static_cast<int>(sf::Keyboard::Scancode::M), ActionName::Mute);

	// Retrieve music from Assets and play it.
	m_music = &m_game->assets().getSound("BackgroundMusic2");
	m_music->play();

	// --- Set Up Camera Entity ---
	m_camera = m_registry.create();
	m_registry.emplace<Input>(m_camera);

	// --- Setup Background for Looping & Enlarging ---
	const sf::Texture& bgTex = m_game->assets().getTexture("galaxy_bg");
	// Ensure that bgTex.setRepeated(true) is called during asset loading.
	m_background = std::make_unique<sf::Sprite>(bgTex);
	// Set the scale so that one tile's size in world space equals (texture size * bgScale)
	m_background->setScale({ bgScale, bgScale });
	// (We will draw a grid of tiles in sRender.)

	// --- Create Example Entities (e.g., planets) ---
	// Loop to create planets via the SpawnPlanet function.
	for (int i = 0; i < 3; ++i) {
		sf::Vector2f pos(200.f + i * 300.f, 300.f + (i % 2) * 150.f);
		sf::Vector2f scale(0.2f, 0.2f);
		SpawnPlanet(pos, scale);
	}
}

// SpawnPlanet creates a planet entity at the given position and with the given scale.
void Scene_Galaxy::SpawnPlanet(const sf::Vector2f& position, const sf::Vector2f& scale) {
	auto entity = m_registry.create();

	// Set up Transform2D component.
	Transform2D trans;
	trans.position = position;
	trans.scale = scale;
	m_registry.emplace<Transform2D>(entity, trans);

	// Create the planet sprite.
	sf::Sprite planetSprite(m_game->assets().getTexture("planet"));
	sf::FloatRect bounds = planetSprite.getLocalBounds();
	planetSprite.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
	planetSprite.setScale(scale);
	Renderable renderable{ planetSprite, 1 }; // layer = 1
	m_registry.emplace<Renderable>(entity, renderable);
}

void Scene_Galaxy::sCamera() {
	// Update the view based on the camera entity's Input component.
	auto& camInput = m_registry.get<Input>(m_camera);
	m_view = m_game->window().getView();
	const float panSpeed = 5.f;

	if (camInput.left)
		m_view.move({ -panSpeed, 0.f });
	if (camInput.right)
		m_view.move({ panSpeed, 0.f });
	if (camInput.up)
		m_view.move({ 0.f, -panSpeed });
	if (camInput.down)
		m_view.move({ 0.f, panSpeed });

	m_game->window().setView(m_view);
}

void Scene_Galaxy::sDoAction(const Action& action) {
	// Update the camera entity's Input component based on key press/release.
	if (action.type() == ActionType::Start) {
		auto& input = m_registry.get<Input>(m_camera);
		switch (action.name()) {
		case ActionName::Left:   input.left = true; break;
		case ActionName::Right:  input.right = true; break;
		case ActionName::Up:     input.up = true; break;
		case ActionName::Down:   input.down = true; break;
		case ActionName::ScrollUp:		
			m_view = m_game->window().getView();
			m_view.zoom(0.9f);
			m_game->window().setView(m_view);
			break;
		case ActionName::ScrollDown:
			m_view = m_game->window().getView();
			m_view.zoom(1.1f);
			m_game->window().setView(m_view);
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
		default: break;
		}
	}
	else if (action.type() == ActionType::End) {
		auto& input = m_registry.get<Input>(m_camera);
		switch (action.name()) {
		case ActionName::Left:   input.left = false; break;
		case ActionName::Right:  input.right = false; break;
		case ActionName::Up:     input.up = false; break;
		case ActionName::Down:   input.down = false; break;
		default: break;
		}
	}
}

void Scene_Galaxy::sRender() {
	// Get the current view.
	sf::View currentView = m_game->window().getView();
	sf::Vector2f viewCenter = currentView.getCenter();

	// Retrieve the background texture's original size.
	const sf::Texture& bgTex = m_game->assets().getTexture("galaxy_bg");
	sf::Vector2u texSize = bgTex.getSize();
	// Compute the tile size in world space.
	sf::Vector2f tileSize(static_cast<float>(texSize.x) * bgScale, static_cast<float>(texSize.y) * bgScale);

	// Align the grid with world coordinates.
	float startX = std::floor(viewCenter.x / tileSize.x) * tileSize.x;
	float startY = std::floor(viewCenter.y / tileSize.y) * tileSize.y;

	// Draw a 3x3 grid of background tiles.
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			// Copy the base background sprite.
			sf::Sprite tileSprite(*m_background);
			// Set the texture rectangle to show the entire tile.
			tileSprite.setTextureRect(sf::IntRect({ 0, 0 }, { static_cast<int>(texSize.x), static_cast<int>(texSize.y) }));
			// Position the tile in the grid.
			tileSprite.setPosition({ startX + i * tileSize.x, startY + j * tileSize.y });
			m_game->window().draw(tileSprite);
		}
	}

	// Draw other entities (e.g. planets).
	auto viewEntities = m_registry.view<Renderable, Transform2D>();
	for (auto entity : viewEntities) {
		auto& renderable = viewEntities.get<Renderable>(entity);
		auto& transform = viewEntities.get<Transform2D>(entity);
		renderable.sprite.setPosition(transform.position);
		m_game->window().draw(renderable.sprite);
	}
}

void Scene_Galaxy::onEnd() {
	m_game->quit();
}

void Scene_Galaxy::update() {
	sCamera();
	sRender();
}
