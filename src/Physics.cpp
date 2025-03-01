#include <cmath>
#include "Physics.h"

// --- Helper functions ---
// Compute the 2D cross product (scalar value).
static float cross(const Vector2f& a, const Vector2f& b) {
    return a.x * b.y - a.y * b.x;
}

// Compute the dot product of two vectors.
static float dot(const Vector2f& a, const Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}

// Compute the Euclidean length of a vector.
static float length(const Vector2f& a) {
    return std::sqrt(dot(a, a));
}

// Normalize a vector; if the vector has zero length, returns it unchanged.
static Vector2f normalize(const Vector2f& a) {
    float len = length(a);
    if (len == 0.f)
        return a;
    return Vector2f(a.x / len, a.y / len);
}

// --- Physics functions ---

Vector2f Physics::GetOverlap(entt::entity a, entt::entity b, entt::registry& registry) {
    // Retrieve current positions and compute half sizes from BoxCollider.
    const auto& transformA = registry.get<Transform2D>(a);
    const auto& transformB = registry.get<Transform2D>(b);
    const auto& colliderA = registry.get<BoxCollider>(a);
    const auto& colliderB = registry.get<BoxCollider>(b);

    Vector2f posA = transformA.position;
    Vector2f posB = transformB.position;
    Vector2f halfSizeA = colliderA.size / 2.f;
    Vector2f halfSizeB = colliderB.size / 2.f;

    // Calculate absolute difference between centers.
    Vector2f delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
    // Overlap is (halfA + halfB) minus center-to-center difference.
    return halfSizeA + halfSizeB - delta;
}

Vector2f Physics::GetPreviousOverlap(entt::entity a, entt::entity b, entt::registry& registry) {
    const auto& transformA = registry.get<Transform2D>(a);
    const auto& transformB = registry.get<Transform2D>(b);
    const auto& colliderA = registry.get<BoxCollider>(a);
    const auto& colliderB = registry.get<BoxCollider>(b);

    Vector2f posA = transformA.prevPosition;
    Vector2f posB = transformB.prevPosition;
    Vector2f halfSizeA = colliderA.size / 2.f;
    Vector2f halfSizeB = colliderB.size / 2.f;

    Vector2f delta{ std::abs(posA.x - posB.x), std::abs(posA.y - posB.y) };
    return halfSizeA + halfSizeB - delta;
}

bool Physics::IsInside(const Vector2f& pos, entt::entity entity, entt::registry& registry) {
    const auto& transform = registry.get<Transform2D>(entity);
    const auto& collider = registry.get<BoxCollider>(entity);
    Vector2f center = transform.position;
    Vector2f halfSize = collider.size / 2.f;

    return (pos.x > center.x - halfSize.x &&
        pos.x < center.x + halfSize.x &&
        pos.y > center.y - halfSize.y &&
        pos.y < center.y + halfSize.y);
}

Intersect Physics::LineIntersect(const Vector2f& a, const Vector2f& b,
    const Vector2f& c, const Vector2f& d) {
    Vector2f r = b - a;
    Vector2f s = d - c;

    float rxs = cross(r, s);
    Vector2f cma = c - a;

    float t = cross(cma, s) / rxs;
    float u = cross(cma, r) / rxs;

    if (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f)
        return { true, a + t * r };
    else
        return { false, Vector2f(0.f, 0.f) };
}

bool Physics::EntityIntersect(const Vector2f& a, const Vector2f& b,
    entt::entity entity, entt::registry& registry) {
    // For this example, assume the entity uses a BoxCollider.
    const auto& transform = registry.get<Transform2D>(entity);
    const auto& collider = registry.get<BoxCollider>(entity);
    Vector2f halfSize = collider.size / 2.f;
    Vector2f center = transform.position;

    // Define the four corners of the bounding box.
    Vector2f e1{ center.x - halfSize.x, center.y - halfSize.y };
    Vector2f e2{ center.x + halfSize.x, center.y - halfSize.y };
    Vector2f e3{ center.x + halfSize.x, center.y + halfSize.y };
    Vector2f e4{ center.x - halfSize.x, center.y + halfSize.y };

    if (LineIntersect(a, b, e1, e2).intersect ||
        LineIntersect(a, b, e2, e3).intersect ||
        LineIntersect(a, b, e3, e4).intersect ||
        LineIntersect(a, b, e4, e1).intersect)
    {
        return true;
    }
    return false;
}

Vector2f Physics::getSpeedAB(const Vector2f& posA, const Vector2f& posB, float speed) {
    float theta = std::atan2(posB.y - posA.y, posB.x - posA.x);
    return Vector2f{ speed * std::cos(theta), speed * std::sin(theta) };
}

RectOverlap Physics::AisNearB(entt::entity a, entt::entity b, const Vector2f& maxDist, entt::registry& registry) {
    ODirection dir = ODirection::NONE;
    Vector2f overlap = GetOverlap(a, b, registry);
    Vector2f pOverlap = GetPreviousOverlap(a, b, registry);

    float dy = registry.get<Transform2D>(b).position.y - registry.get<Transform2D>(a).position.y;
    if (overlap.x > 0.f && overlap.y > 0.f && -maxDist.y < overlap.y && pOverlap.y <= 0.f) {
        if (dy > 0.f)
            dir = ODirection::UP;
        else if (dy < 0.f)
            dir = ODirection::DOWN;
    }

    float dx = registry.get<Transform2D>(b).position.x - registry.get<Transform2D>(a).position.x;
    if (overlap.y > 0.f && overlap.x > 0.f && -maxDist.x < overlap.x && pOverlap.x <= 0.f) {
        if (dx > 0.f)
            dir = ODirection::LEFT;
        else if (dx < 0.f)
            dir = ODirection::RIGHT;
    }

    return { dir, overlap };
}
