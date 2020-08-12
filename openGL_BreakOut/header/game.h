#ifndef GAME_H
#define GAME_H
#include <vector>
#include "gamelevel.h"
#include "powerup.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum GameState 
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game 
{
public:
	GameState State;
	bool Keys[1024];
	bool KeysProcessed[1024];
	std::vector<GameLevel> Levels;
	std::vector<PowerUp> PowerUps;
	unsigned int Lives;
	unsigned int Level;
	unsigned int Height, Width;
	Game(unsigned int height, unsigned int width);
	~Game();
	void Init();
	void ProcessInput(float deltaTime);
	void Update(float deltaTime);
	void Render();
	void DoCollisions();
	void ResetPlayer();
	void ResetLevel();
	void SpawnPowerUps(GameObject& block);
	void UpdatePowerUps(float deltaTime);
	bool IsCompleted();
};
#endif // !
