#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

SpriteRenderer *spriteRenderer;

const vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
GameObject *Player;

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
	// load player
	ResourceManager::LoadTexture("paddle.png", true, "paddle");
	vec2 playerPos = vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
}

void Game::ProcessInput(float deltaTime)
{
	float velocity = PLAYER_VELOCITY * deltaTime;
	if (this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT]) 
	{
		if (Player->Position.x >= 0.0f) 
		{
			Player->Position.x -= velocity;
		}
	}
	if (this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT]) 
	{
		if (Player->Position.x <= this->Width - Player->Size.x) 
		{
			Player->Position.x += velocity;
		}
	}
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
	Player->Draw(*spriteRenderer);
}								  
														  
