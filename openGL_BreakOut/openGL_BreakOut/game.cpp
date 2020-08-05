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
	delete spriteRenderer;
}

void Game::Init()
{
	// load shader
	ResourceManager::LoadShader("sprite.vs", "sprite.fs", nullptr, "sprite");
	mat4 projection = ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	Shader myShader;
	myShader = ResourceManager::GetShader("sprite");
	spriteRenderer = new SpriteRenderer(myShader);
	// load textures
	ResourceManager::LoadTexture("awesomeface.png", true, "awesomeface");
	ResourceManager::LoadTexture("background.jpg", false, "background");
	ResourceManager::LoadTexture("block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("block.png", false, "block");
	// load levels
	GameLevel lv1;
	lv1.Load("lv1.lvl", this->Width, this->Height / 2);
	GameLevel lv2;
	lv1.Load("lv2.lvl", this->Width, this->Height / 2);
	GameLevel lv3;
	lv1.Load("lv3.lvl", this->Width, this->Height / 2);
	GameLevel lv4;
	lv1.Load("lv4.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(lv1);
	this->Levels.push_back(lv2);
	this->Levels.push_back(lv3);
	this->Levels.push_back(lv4);
	this->Level = 0;
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
	myTexture = ResourceManager::GetTexture("background");
	spriteRenderer->DrawSprite(myTexture, 
		vec2(0.0f), vec2(this->Width, this->Height),0.0f);
	this->Levels[this->Level].Draw(*spriteRenderer);
}								  
														  
