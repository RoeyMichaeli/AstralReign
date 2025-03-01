#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <SFML/Graphics.hpp>
#include <vector>

using sf::Vector2f;

// Transform2D: holds an entity's position, rotation, and scale.
struct Transform2D {
	Vector2f position{ 0.f, 0.f }; // World position
	Vector2f prevPosition{ 0.f, 0.f }; // Previous World Position
	float rotation = 0.f;             // Rotation in degrees
	Vector2f scale{ 1.f, 1.f };     // Scale factors (default is no scaling)
};

// Movement: stores velocity and acceleration, plus an optional maximum speed.
struct Movement {
	Vector2f velocity{ 0.f, 0.f };      // Current velocity (units per frame or per second)
	Vector2f acceleration{ 0.f, 0.f };  // Current acceleration
	float maxSpeed = 0.f;                  // Maximum allowed speed (0 if unused)
};

// Renderable: contains a sprite (or other drawable) plus a layer (z-index) for drawing order.
struct Renderable {
	sf::Sprite sprite;  // Sprite used for drawing the entity.
	int layer = 0;      // Draw order; lower values render first.
};

// BoxCollider: defines an axis-aligned rectangular collider.
struct BoxCollider {
	Vector2f size{ 0.f, 0.f };    // Width and height of the collider.
	Vector2f offset{ 0.f, 0.f };  // Offset relative to the entity's origin.
};

// CircleCollider: defines a circular collision shape.
struct CircleCollider {
	float radius = 0.f;              // Radius of the collider.
	Vector2f offset{ 0.f, 0.f };   // Offset relative to the entity's origin.
};

// ComplexCollider: for non-primitive collision shapes (e.g., convex polygons).
struct ComplexCollider {
	std::vector<Vector2f> points; // Points in local space defining the collider.
};

// Input: holds the state of typical input keys (for player-controlled entities).
struct Input {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
};

// Health: simple health component.
struct Health {
	int current = 100;
	int maximum = 100;
};

// Tag components: empty structs to mark entities by type.
struct TEnemy {};
struct TPlayer {};
struct TBullet {};
struct TProjectile {};
struct TPlanet {};

// Optionally, you might include a Faction component to mark sides, teams, or alliances:
struct Faction {
	int id = 0; // Alternatively, use an enum to represent factions.
};

#endif // COMPONENTS_HPP
