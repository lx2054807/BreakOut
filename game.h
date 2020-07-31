#ifndef GAME_H
#define GAME_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>

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
	void ProcessInput();
	void Update();
	void Render();
};
#endif // !
