#ifndef GAME_H
#define GAME_H

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
	unsigned int Height, Width;
	Game(unsigned int height, unsigned int width);
	~Game();
	void Init();
	void ProcessInput(float deltaTime);
	void Update(float deltaTime);
	void Render();
};
#endif // !
