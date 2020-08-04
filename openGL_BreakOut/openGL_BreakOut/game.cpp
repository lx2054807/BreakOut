#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

SpriteRenderer *spriteRenderer;

Game::Game(unsigned int height, unsigned int width)
	:State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
}

void Game::Init()
{
	ResourceManager::LoadShader("sprite.vs", "sprite.fs", nullptr, "sprite");
	mat4 projection = ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	Shader myShader;
	myShader = ResourceManager::GetShader("sprite");
	spriteRenderer = new SpriteRenderer(myShader);
	ResourceManager::LoadTexture("awesomeface.png", true, "awesomeface");
}

void Game::ProcessInput(float deltaTime)
{
}

void Game::Update(float deltaTime)
{
}

void Game::Render()
{
	Texture2D myTexture;
	myTexture = ResourceManager::GetTexture("awesomeface");
	spriteRenderer->DrawSprite(myTexture, 
		vec2(200.0f, 200.0f), vec2(300.0f, 400.0f), 45.0f, vec3(0.0f, 1.0f, 0.0f));
}								  
														  
