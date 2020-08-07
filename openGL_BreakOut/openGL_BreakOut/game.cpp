#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"
#include "ball.h"
#include "gameobject.h"
SpriteRenderer *spriteRenderer;

const vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
GameObject *Player;
BallObject *Ball;

const float BALL_RADIUS = 12.5f;
const vec2 BALL_VELOCITY(100.0f, -300.0f);

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
	// init ball
	vec2 ballPos = playerPos + vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, BALL_VELOCITY, ResourceManager::GetTexture("awesomeface"));
}

bool CheckCollision(GameObject& one, GameObject& two) 
{
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x
		&& two.Position.x + two.Position.x >= one.Position.x;
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y
		&& two.Position.y + two.Size.y >= one.Position.y;
	return collisionX && collisionY;
}

bool CheckCollision(GameObject& one, BallObject& ball) 
{
	vec2 center(ball.Position + ball.Radius);
	vec2 aabb_half_extents(one.Size.x / 2, one.Size.y / 2);
	vec2 aabb_center(one.Position.x + aabb_half_extents.x,
		one.Position.y + aabb_half_extents.y);
	vec2 difference = center - aabb_center;
	vec2 clamped = clamp(difference, -aabb_half_extents, aabb_half_extents);
	vec2 closet = aabb_center + clamped;
	difference = closet - center;
	return length(difference) < ball.Radius;
}

void Game::DoCollisions() 
{
	for (GameObject& brick : this->Levels[this->Level].Bricks) 
	{
		if (!brick.Destroyed) 
		{
			if (CheckCollision(brick, *Ball))
				if (!brick.IsSolid)
					brick.Destroyed = true;
		}
	}
}

void Game::ProcessInput(float deltaTime)
{
	if (this->State == GAME_ACTIVE) {
		float velocity = PLAYER_VELOCITY * deltaTime;
		if (this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_LEFT])
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck) 
				{
					Ball->Position.x -= velocity;
				}
					
			}
		}
		if (this->Keys[GLFW_KEY_D] || this->Keys[GLFW_KEY_RIGHT])
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck) 
				{
					Ball->Position.x += velocity;
				}
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}

void Game::Update(float deltaTime)
{
	Ball->Move(deltaTime, this->Width);
	this->DoCollisions();
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE) {
		Texture2D myTexture;
		myTexture = ResourceManager::GetTexture("background");
		spriteRenderer->DrawSprite(myTexture,
			vec2(0.0f), vec2(this->Width, this->Height), 0.0f);
		this->Levels[this->Level].Draw(*spriteRenderer);
		Player->Draw(*spriteRenderer);
		Ball->Draw(*spriteRenderer);
	}
}								  
														  
