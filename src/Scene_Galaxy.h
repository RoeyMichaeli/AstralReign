#pragma once
#ifndef SCENE_GALAXY_H
#define SCENE_GALAXY_H

#include "Scene.h"
#include "GameEngine.h"


class Scene_Galaxy : public Scene {
protected:

	entt::entity m_camera;
	std::unique_ptr<sf::Sprite> m_background;
	sf::View m_view;

	// Pointer to title music (retrieved from Assets – assumed to remain valid)
	sf::Sound* m_music = nullptr;

	void init();
	void sRender() override;
	void sDoAction(const Action& action) override;
	void onEnd() override;
	void sCamera();
	void SpawnPlanet(const sf::Vector2f& position, const sf::Vector2f& scale);


public:
	Scene_Galaxy(GameEngine* gameEngine);
	void update() override;
};


#endif // SCENE_GALAXY_H