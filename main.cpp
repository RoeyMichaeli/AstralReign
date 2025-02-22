#include <bitset>
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <fstream>
#include <iostream>
//#include <Eigen/Dense>

//using Eigen::Vector2f;
using sf::Vector2f;

// Components
struct CTransform {
	Vector2f position = { 0, 0 };
	Vector2f velocity = { 0, 0 };
	float angle = 0;

	CTransform() = default;
	CTransform(const Vector2f& pos, const Vector2f& vel, float ang)
		: position(pos), velocity(vel), angle(ang) {
	}
};

struct CColission {
	float radius = 0;

	CColission() = default;
	CColission(float r) : radius(r) {}
};

struct CScore {
	int score = 0;

	CScore() = default;
	CScore(int s) : score(s) {}
};

struct CShape {
	sf::CircleShape circle;

	CShape(float radius, std::size_t points,
		const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(Vector2f(radius, radius));
	}
};

struct CLifespan {
	unsigned short lifespan = 0;
	unsigned short remaining = 0;

	CLifespan() = default;
	CLifespan(unsigned short l) : lifespan(l), remaining(l) {}
};

struct CInput {
	unsigned up : 1;
	unsigned down : 1;
	unsigned left : 1;
	unsigned right : 1;
	unsigned shoot : 1;

	CInput() : up(0), down(0), left(0), right(0), shoot(0) {}
};

// Tags

struct TEnemy {};
struct TSmallEnemy {};
struct TPlayer {};
struct TBullet {};
struct TPendingDestruction {};

// Configurations
// Shape Radius   SR
// Collision Radius CR
// Fill Color     FR, FG, FB
// Outline Color  OR, OG, OB
// Outline Thickness OT
// Shape Verticies  V
// Min / Max Verticies  VMIN, VMAX
// Speed           S
// Min / Max Speed  SMIN, SMAX
// Lifespan L
// Spawn Interval   SI

struct PlayerConfig {
	short SR, CR, OR, OG, OB, FR, FG, FB, OT, V;
	float S;
};

struct EnemyConfig {
	short SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
	float SMIN, SMAX;
};

struct BulletConfig {
	short SR, CR, OR, OG, OB, FR, FG, FB, OT, V;
	float S;
};

struct EnemySpawnArea {
	short minX, minY, maxX, maxY;
};

class Game {
	sf::RenderWindow m_window;
	entt::registry m_registry;

	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;

	EnemySpawnArea m_enemySpawnArea;

	sf::Clock m_deltaClock;
	int m_score = 0;
	int m_currentFrame = 0;
	int m_lastEnemySpawnTime = 0;

	Vector2f m_screenSize;

	bool m_running = true;
	bool m_paused = false;

	int init(const std::string& path)
	{
		// Setup SFML window
		m_window.create(sf::VideoMode::getDesktopMode(), "AstralReign", sf::State::Fullscreen);

		// Set window properties
		m_window.setFramerateLimit(60); // 33ms per frame (now 60 FPS)
		m_window.setVerticalSyncEnabled(true);

		m_screenSize = Vector2f(m_window.getSize());

		// Seed random number generator
		srand((unsigned int)time(NULL));

		// Load Config Files
		std::ifstream file(path); // open file
		if (!file) {
			std::cerr << "Failed to open config file: " << path << std::endl;
			std::cerr << "Current working directory: " << std::filesystem::current_path() << std::endl;
			return 1;
		}

		// Read player config
		file >> m_playerConfig.SR >> m_playerConfig.CR >>
			m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB >>
			m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >>
			m_playerConfig.OT >> m_playerConfig.V >> m_playerConfig.S;

		// Read enemy config
		file >> m_enemyConfig.SR >> m_enemyConfig.CR >>
			m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >>
			m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >>
			m_enemyConfig.L >> m_enemyConfig.SI >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX;

		// Read bullet config
		file >> m_bulletConfig.SR >> m_bulletConfig.CR >>
			m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB >>
			m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >>
			m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.S;

		file.close(); // close file

		// Setup ImGui binding (ignore return value to silence warnings)
		(void)ImGui::SFML::Init(m_window);
		ImGui::GetStyle().ScaleAllSizes(2.0f);
		ImGui::GetIO().FontGlobalScale = 2.0f;

		// Setup Enemy Spawn Area
		m_enemySpawnArea.minX = static_cast<short>(m_enemyConfig.SR * 1.5);
		m_enemySpawnArea.minY = static_cast<short>(m_enemyConfig.SR * 1.5);
		m_enemySpawnArea.maxX = static_cast<short>(m_screenSize.x - m_enemySpawnArea.minX);
		m_enemySpawnArea.maxY = static_cast<short>(m_screenSize.y - m_enemySpawnArea.minY);

		spawnPlayer();

		return 0;
	}

	void update()
	{
		// Destroy entities
		auto view = m_registry.view<TPendingDestruction>();
		for (auto entity : view)
		{
			m_registry.destroy(entity);
		}
	}

	// TODO: Correct angle calculation
	void sMovement()
	{
		// Update movement for enemies (non-player entities)
		auto enemyView = m_registry.view<TEnemy, CTransform>();
		for (auto entity : enemyView)
		{
			auto& transform = enemyView.get<CTransform>(entity);

			// Update enemy position
			transform.position += transform.velocity;

			float radius = static_cast<float>(m_enemyConfig.SR * 1.5);

			// Bounce off left/right walls
			if (transform.position.x - radius < 0)
			{
				transform.position.x = radius;
				transform.velocity.x = -transform.velocity.x;
			}
			else if (transform.position.x + radius > m_screenSize.x)
			{
				transform.position.x = m_screenSize.x - radius;
				transform.velocity.x = -transform.velocity.x;
			}

			// Bounce off top/bottom walls
			if (transform.position.y - radius < 0)
			{
				transform.position.y = radius;
				transform.velocity.y = -transform.velocity.y;
			}
			else if (transform.position.y + radius > m_screenSize.y)
			{
				transform.position.y = m_screenSize.y - radius;
				transform.velocity.y = -transform.velocity.y;
			}
		}

		// Update the player separately, since its movement is handled by user input.
		auto player = m_registry.view<TPlayer>().front();
		auto& playerTransform = m_registry.get<CTransform>(player);
		auto& playerInput = m_registry.get<CInput>(player);

		if (playerInput.left) playerTransform.position.x -= playerTransform.velocity.x;
		if (playerInput.right) playerTransform.position.x += playerTransform.velocity.x;
		if (playerInput.up) playerTransform.position.y -= playerTransform.velocity.y;
		if (playerInput.down) playerTransform.position.y += playerTransform.velocity.y;

		int playerBound = static_cast<int>(m_playerConfig.SR * 1.5);

		// Clamp player position so it stays within the screen
		if (playerTransform.position.x < playerBound)
			playerTransform.position.x = static_cast<float>(playerBound);
		else if (playerTransform.position.x > m_screenSize.x - playerBound)
			playerTransform.position.x = m_screenSize.x - static_cast<float>(playerBound);

		if (playerTransform.position.y < playerBound)
			playerTransform.position.y = static_cast<float>(playerBound);
		else if (playerTransform.position.y > m_screenSize.y - playerBound)
			playerTransform.position.y = m_screenSize.y - static_cast<float>(playerBound);
	}

	void sUserInput()
	{
		while (const std::optional event = m_window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(m_window, *event);

			if (event->is<sf::Event::Closed>())
			{
				m_running = false;
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				switch (keyPressed->scancode)
				{
				case sf::Keyboard::Scancode::W:
					m_registry.get<CInput>(m_registry.view<TPlayer>().front()).up = 1;
					break;
				case sf::Keyboard::Scancode::S:
					m_registry.get<CInput>(m_registry.view<TPlayer>().front()).down = 1;
					break;
				case sf::Keyboard::Scancode::A:
					m_registry.get<CInput>(m_registry.view<TPlayer>().front()).left = 1;
					break;
				case sf::Keyboard::Scancode::D:
					m_registry.get<CInput>(m_registry.view<TPlayer>().front()).right = 1;
					break;
				case sf::Keyboard::Scancode::Escape:
					m_running = false;
					break;
				default:
					break;
				}
			}

			if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
			{
				switch (keyReleased->scancode)
				{
				case sf::Keyboard::Scancode::W:
					m_registry.get<CInput>(m_registry.view<TPlayer>().front()).up = 0;
					break;
				case sf::Keyboard::Scancode::S:
					m_registry.get<CInput>(m_registry.view<TPlayer>().front()).down = 0;
					break;
				case sf::Keyboard::Scancode::A:
					m_registry.get<CInput>(m_registry.view<TPlayer>().front()).left = 0;
					break;
				case sf::Keyboard::Scancode::D:
					m_registry.get<CInput>(m_registry.view<TPlayer>().front()).right = 0;
					break;
				default:
					break;
				}
			}
		}
	}

	void sLifespan() {}

	void sCollision()
	{
		// Get a view over entities with both components
		auto view = m_registry.view<CTransform, CColission>();

		float distSquared;
		float sumRadii;

		// Iterate over each pair of entities (using structured bindings)
		for (auto [entity1, transform1, col1] : view.each())
		{
			for (auto [entity2, transform2, col2] : view.each())
			{
				// Avoid checking a pair twice
				if (entity1 < entity2)
				{
					// Use squared distance for efficiency
					sf::Vector2f diff = transform1.position - transform2.position;
					distSquared = diff.x * diff.x + diff.y * diff.y;
					sumRadii = col1.radius + col2.radius;
					if (distSquared < sumRadii * sumRadii)
					{
						// If one entity is a player and the other an enemy or small enemy, mark both for destruction, and respawn the player
						if (m_registry.all_of<TPlayer>(entity1) &&
							(m_registry.all_of<TEnemy>(entity2) || m_registry.all_of<TSmallEnemy>(entity2))
							|| m_registry.all_of<TPlayer>(entity2) &&
							(m_registry.all_of<TEnemy>(entity1) || m_registry.all_of<TSmallEnemy>(entity1)))
						{
							m_registry.emplace<TPendingDestruction>(entity1);
							m_registry.emplace<TPendingDestruction>(entity2);
							m_score -= 1000;
							spawnPlayer();
						}
						// If one entity is a bullet and the other an enemy or small enemy, mark both for destruction
						else if (m_registry.all_of<TBullet>(entity1) &&
							(m_registry.all_of<TEnemy>(entity2) || m_registry.all_of<TSmallEnemy>(entity2))
							|| m_registry.all_of<TBullet>(entity2) &&
							(m_registry.all_of<TEnemy>(entity1) || m_registry.all_of<TSmallEnemy>(entity1)))
						{
							m_registry.emplace<TPendingDestruction>(entity1);
							m_registry.emplace<TPendingDestruction>(entity2);
							m_score += 100;
						}
					}
				}
			}
		}
	}

	void sEnemySpawner()
	{
		if (m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI)
			spawnEnemy();
	}

	void spawnPlayer()
	{
		// Create player
		auto player = m_registry.create();

		m_registry.emplace<CTransform>(player,
			Vector2f(m_screenSize.x / 2.0f, m_screenSize.y / 2.0f), // Position
			Vector2f(m_playerConfig.S, m_playerConfig.S),             // Velocity
			1.0f                                                        // Angle
		);

		m_registry.emplace<CShape>(player,
			m_playerConfig.SR,                                  // Shape Radius
			m_playerConfig.V,                                   // Verticies
			sf::Color(static_cast<uint8_t>(m_playerConfig.FR),
				static_cast<uint8_t>(m_playerConfig.FG),
				static_cast<uint8_t>(m_playerConfig.FB)), // Fill Color
			sf::Color(static_cast<uint8_t>(m_playerConfig.OR),
				static_cast<uint8_t>(m_playerConfig.OG),
				static_cast<uint8_t>(m_playerConfig.OB)), // Outline Color
			m_playerConfig.OT                                  // Outline Thickness
		);

		m_registry.emplace<CColission>(player, static_cast<float>(m_playerConfig.CR + m_playerConfig.OT));
		m_registry.emplace<CInput>(player);
		m_registry.emplace<TPlayer>(player);
	}

	void spawnEnemy()
	{
		// Create enemy
		auto enemy = m_registry.create();

		m_registry.emplace<CTransform>(enemy,
			// Position
			Vector2f(
				static_cast<float>((rand() % (1 + m_enemySpawnArea.maxX - m_enemySpawnArea.minX)) + m_enemySpawnArea.minX),
				static_cast<float>((rand() % (1 + m_enemySpawnArea.maxY - m_enemySpawnArea.minY)) + m_enemySpawnArea.minY)
			),
			// Velocity
			Vector2f(
				(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (m_enemyConfig.SMAX - m_enemyConfig.SMIN),
				(static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (m_enemyConfig.SMAX - m_enemyConfig.SMIN)
			),
			// Angle
			static_cast<float>(rand()) / 360
		);

		m_registry.emplace<CShape>(enemy,
			m_enemyConfig.SR,                                           // Shape Radius
			(rand() % (1 + m_enemyConfig.VMIN - m_enemyConfig.VMAX)) + m_enemyConfig.VMIN, // Verticies
			sf::Color(static_cast<uint8_t>(rand() % 255),
				static_cast<uint8_t>(rand() % 255),
				static_cast<uint8_t>(rand() % 255)),             // Fill Color
			sf::Color(static_cast<uint8_t>(m_enemyConfig.OR),
				static_cast<uint8_t>(m_enemyConfig.OG),
				static_cast<uint8_t>(m_enemyConfig.OB)),         // Outline Color
			m_enemyConfig.OT                                            // Outline Thickness
		);

		m_registry.emplace<CColission>(enemy, static_cast<float>(m_enemyConfig.CR + m_enemyConfig.OT));
		m_registry.emplace<TEnemy>(enemy);

		m_lastEnemySpawnTime = m_currentFrame;
	}

	void spawnBullet() {}
	void spawnSmallEnemies() {}
	void spawnSpecialWeapon() {}

	void sGUI()
	{
		ImGui::Begin("AstralReign");
		ImGui::Text("Score: %d", m_score);
		if (ImGui::Button("Exit"))
		{
			m_running = false;
		}
		ImGui::End();
	}

	void sRender()
	{
		m_window.clear();
		auto view = m_registry.view<CTransform, CShape>();
		for (auto [entity, transform, shape] : view.each())
		{
			shape.circle.setPosition(transform.position);
			shape.circle.setRotation(sf::degrees(++transform.angle));
			m_window.draw(shape.circle);
		}
		ImGui::SFML::Render(m_window);
		m_window.display();
	}

public:
	Game(const std::string& config)
	{
		init(config);
	}

	void run()
	{
		// TODO pause function
		while (m_running)
		{
			update();

			ImGui::SFML::Update(m_window, m_deltaClock.restart());

			sEnemySpawner();
			sMovement();
			sCollision();
			sUserInput();
			sLifespan();

			sGUI();
			sRender();

			m_score += 3;
			m_currentFrame++;
		}

		ImGui::SFML::Shutdown();
		m_window.close();
	}
};

int main()
{
	Game game("config.txt");
	game.run();

	return 0;
}
