#ifndef PHYSICS_H
#define PHYSICS_H

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>
#include "Components.hpp"
// New Components are declared in Components.hpp, which should define:
// struct Transform2D { sf::Vector2f position, prevPosition; float rotation; sf::Vector2f scale; };
// struct BoxCollider { sf::Vector2f size; sf::Vector2f offset; };

using sf::Vector2f;

struct Intersect {
    bool intersect = false;
    Vector2f position = { 0.f, 0.f };
};

enum struct ODirection {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

struct RectOverlap {
    ODirection direction = ODirection::NONE;
    Vector2f overlap = { 0.f, 0.f };
};

class Physics {
public:
    // Calculate current overlap between entity a and b based on their Transform2D and BoxCollider components.
    static Vector2f GetOverlap(entt::entity a, entt::entity b, entt::registry& registry);

    // Calculate previous overlap using previous positions from Transform2D.
    static Vector2f GetPreviousOverlap(entt::entity a, entt::entity b, entt::registry& registry);

    // Check if a point lies inside the entity's collision area.
    static bool IsInside(const Vector2f& pos, entt::entity entity, entt::registry& registry);

    // Compute the intersection between two lines defined by points a→b and c→d.
    static Intersect LineIntersect(const Vector2f& a, const Vector2f& b,
        const Vector2f& c, const Vector2f& d);

    // Check if the line from a to b intersects the collision box of the entity.
    static bool EntityIntersect(const Vector2f& a, const Vector2f& b,
        entt::entity entity, entt::registry& registry);

    // Determine if entity A is near entity B (for collision resolution),
    // returning an overlap and a direction.
    static RectOverlap AisNearB(entt::entity a, entt::entity b, const Vector2f& maxDist, entt::registry& registry);

    // Returns a velocity vector pointing from posA to posB with magnitude 'speed'
    static Vector2f getSpeedAB(const Vector2f& posA, const Vector2f& posB, float speed);
};

#endif // PHYSICS_H
