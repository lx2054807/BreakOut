#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"
#include "ball.h"
#include "gameobject.h"
SpriteRenderer *spriteRenderer;

enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};
typedef std::tuple<bool, Direction, vec2> Collision;

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
	ResourceManager::LoadTexture("goodpic.png", false, "block_solid");
	ResourceManager::LoadTexture("goodpic.png", false, "block");
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

//// AABB collision
//bool CheckCollision(GameObject& one, GameObject& two) 
//{
//	bool collisionX = one.Position.x + one.Size.x >= two.Position.x
//		&& two.Position.x + two.Position.x >= one.Position.x;
//	bool collisionY = one.Position.y + one.Size.y >= two.Position.y
//		&& two.Position.y + two.Size.y >= one.Position.y;
//	return collisionX && collisionY;
//}

Direction VectorDirection(vec2 target) 
{
	vec2 compass[] =
	{
		vec2(0.0,1.0),
		vec2(1.0,0.0),
		vec2(0.0,-1.0),
		vec2(-1.0,0.0)
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++) 
	{
		float dot_product = dot(normalize(target), compass[i]);
		if (dot_product > max) 
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

Collision CheckCollision(GameObject& one, BallObject& ball) 
{
	vec2 center(ball.Position + ball.Radius);
	vec2 aabb_half_extents(one.Size.x / 2, one.Size.y / 2);
	vec2 aabb_center(one.Position.x + aabb_half_extents.x,
		one.Position.y + aabb_half_extents.y);
	vec2 difference = center - aabb_center;
	vec2 clamped = clamp(difference, -aabb_half_extents, aabb_half_extents);
	vec2 closet = aabb_center + clamped;
	difference = closet - center;
	if (length(difference) <= ball.Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, vec2(0.0, 0.0));
}

void Game::DoCollisions() 
{
	// check collision between bricks and ball 
	for (GameObject& brick : this->Levels[this->Level].Bricks) 
	{
		if (!brick.Destroyed) 
		{
			Collision collision = CheckCollision(brick, *Ball);
			if (std::get<0>(collision)) 
			{
				if (!brick.IsSolid)
					brick.Destroyed = true;
				Direction dir = std::get<1>(collision);
				vec2 diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT) 
				{
					Ball->Velocity.x = -Ball->Velocity.x;
					float penetration = Ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						Ball->Position.x += penetration;
					else
						Ball->Position.x -= penetration;
				}
				else
				{
					Ball->Velocity.y = -Ball->Velocity.y;
					float penetration = Ball->Radius - std::abs(diff_vector.y);
					if (dir == DOWN)
						Ball->Position.y += penetration;
					else
						Ball->Position.y -= penetration;
				}
			}
		}
	}
	// check collision between ball and player paddle
	Collision result = CheckCollision(*Player, *Ball);
	if (!Ball->Stuck && std::get<0>(result)) 
	{
		float centerPaddle = Player->Position.x + Player->Size.x / 2;
		float distance = (Ball->Position.x + Ball->Radius) - centerPaddle;
		float percentage = distance / (Player->Size.x / 2);
		float strengh = 2.0f;
		vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = BALL_VELOCITY.x * percentage * strengh;
		Ball->Velocity.y = -Ball->Velocity.y;
		Ball->Velocity = normalize(Ball->Velocity) * length(oldVelocity);
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
	if (Ball->Position.y > this->Height) 
	{
		this->ResetPlayer();
		this->ResetLevel();
	}
}

void Game::ResetPlayer()
{
	vec2 playerPos = vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		this->Height - PLAYER_SIZE.y);
	Player->Position = playerPos;
	vec2 ballPos = playerPos + vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball->Reset(ballPos, BALL_VELOCITY);
}

void Game::ResetLevel() 
{
	this->Levels[this->Level].Reset();
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
														  
