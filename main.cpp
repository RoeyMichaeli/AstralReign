#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
//#include <Eigen/Dense>

//using Eigen::Vector2f;
using sf::Vector2f;

struct ShipData {
	uint32_t id;
};

struct Transform {
	Vector2f position;
	Vector2f velocity;
	Vector2f acceleration;
};

struct Spatial {
	Vector2f scale;
	Vector2f size;
	Vector2f origin;
	float rotation = 0.0f;
};

int Update(entt::registry& registry, sf::Time dt)
{
	auto view = registry.view<Transform>();
	
	for (auto entity : view) {
		auto& transform = view.get<Transform>(entity);
		transform.position += transform.velocity * dt.asSeconds();
		transform.velocity += transform.acceleration * dt.asSeconds();
	}
	return 0;
}

int main()
{
	// Setup SFML window
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(desktop, "AstralReign", sf::State::Fullscreen);
	
	window.setFramerateLimit(30); // 33ms per frame
	window.setVerticalSyncEnabled(true);
	

	// Setup ImGui binding
	// ImGui::SFML::Init(window);

	// Setup entt registry
	entt::registry registry;

	// Main loop
	while (window.isOpen())
	{
		// Poll and handle events
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
					window.close();
			}
		}

		window.clear();

		Update(registry, sf::seconds(1.0f / 30.0f));	// 30fps

		window.display();
	}

	return 0;
}