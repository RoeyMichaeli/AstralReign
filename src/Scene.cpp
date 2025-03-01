#include "Scene.h"
#include "GameEngine.h"

Scene::Scene(GameEngine* gameEngine)
    : m_game(gameEngine) {
}

void Scene::doAction(const Action& action) {
    sDoAction(action);
}

void Scene::setPaused(bool paused) {
    m_paused = paused;
}

void Scene::simulate(const size_t /*frames*/) {
    // Default implementation: no simulation step.
}

void Scene::registerAction(int inputKey, const ActionName& actionName) {
    m_actionMap.emplace(inputKey, actionName);
}

float Scene::width() const {
    return static_cast<float>(m_game->window().getSize().x);
}

float Scene::height() const {
    return static_cast<float>(m_game->window().getSize().y);
}

size_t Scene::currentFrame() const {
    return m_currentFrame;
}

bool Scene::hasEnded() const {
    return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const {
    return m_actionMap;
}

void Scene::drawLine(const sf::Vector2f& p1, const sf::Vector2f& p2) {
    sf::Vertex vertices[2];
    vertices[0].position = p1;
    vertices[1].position = p2;
    m_game->window().draw(vertices, 2, sf::PrimitiveType::Lines);
}