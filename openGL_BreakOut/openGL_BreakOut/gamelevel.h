#pragma once
#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>
#include <glad/glad.h>
#include <glm/glm/glm.hpp>

#include "gameobject.h"
#include "sprite_renderer.h"
#include "resource_manager.h"
class GameLevel 
{
public:
	vector<GameObject> Bricks;
	GameLevel() {}
	~GameLevel() {}
	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
	void Draw(SpriteRenderer& renderer);
	bool IsCompleted();
private:
	void init(vector<vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif // !GAMELEVEL_H
