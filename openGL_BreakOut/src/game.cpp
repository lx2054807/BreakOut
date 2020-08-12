#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"
#include "ball.h"
#include "gameobject.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "powerup.h"
#include <algorithm>
#include "text_renderer.h"
#include <sstream>

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
ParticleGenerator *Particles;
PostProcessor* PostProcessing;
TextRenderer* Text;

const float BALL_RADIUS = 12.5f;
const vec2 BALL_VELOCITY(100.0f, -300.0f);

Game::Game(unsigned int height, unsigned int width)
	:State(GAME_ACTIVE), Keys(), KeysProcessed(),Width(width), Height(height), Lives(3)
{
}

Game::~Game()
{
	delete spriteRenderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete PostProcessing;
	delete Text;
}

void Game::Init()
{
	// load SpriteRenderer
	ResourceManager::LoadShader("sprite.vs", "sprite.fs", nullptr, "sprite");
	mat4 projection = ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	Shader myShader;
	myShader = ResourceManager::GetShader("sprite");
	spriteRenderer = new SpriteRenderer(myShader);
	// load ParticleGenerator
	ResourceManager::LoadShader("particle.vs", "particle.fs", nullptr, "particle");
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
	ResourceManager::LoadTexture("particle.png", true, "particle");
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	// load PostProcessor
	ResourceManager::LoadShader("postprocessing.vs", "postprocessing.fs", nullptr, "postprocess");
	PostProcessing = new PostProcessor(ResourceManager::GetShader("postprocess"), this->Width, this->Height);
	// load Textures
	ResourceManager::LoadTexture("awesomeface.png", true, "awesomeface");
	ResourceManager::LoadTexture("background.jpg", false, "background");
	ResourceManager::LoadTexture("goodpic.png", false, "block_solid");
	ResourceManager::LoadTexture("greatpic.png", false, "block");
	ResourceManager::LoadTexture("powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture("powerup_sticky.png", true, "powerup_sticky");
	ResourceManager::LoadTexture("powerup_passthrough.png", true, "powerup_passthrough");
	ResourceManager::LoadTexture("powerup_increase.png", true, "powerup_increase");
	ResourceManager::LoadTexture("powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture("powerup_chaos.png", true, "powerup_chaos");

	// load Levels
	GameLevel lv1;
	lv1.Load("lv1.lvl", this->Width, this->Height / 2);
	GameLevel lv2;
	lv2.Load("lv2.lvl", this->Width, this->Height / 2);
	GameLevel lv3;
	lv3.Load("lv3.lvl", this->Width, this->Height / 2);
	GameLevel lv4;
	lv4.Load("lv4.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(lv1);
	this->Levels.push_back(lv2);
	this->Levels.push_back(lv3);
	this->Levels.push_back(lv4);
	this->Level = 0;
	// init Player
	ResourceManager::LoadTexture("paddle.png", true, "paddle");
	vec2 playerPos = vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	// init Ball
	vec2 ballPos = playerPos + vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, BALL_VELOCITY, ResourceManager::GetTexture("awesomeface"));
	// load Text
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("OCRAEXT.TTF", 24);
}

//// AABB collision
bool CheckCollision(GameObject& one, GameObject& two) 
{
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x
		&& two.Position.x + two.Position.x >= one.Position.x;
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y
		&& two.Position.y + two.Size.y >= one.Position.y;
	return collisionX && collisionY;
}

// match direction for target
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

// check collision between ball and other gameobjects
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

void activatePowerUp(PowerUp& powerup)
{
	switch (powerup.Type)
	{
	case(SPEED):Ball->Velocity *= 1.2f;
		break;
	case(STICKY): Ball->Sticky = true;
		Player->Color = vec3(1.0f, 0.5f, 1.0f);
		break;
	case(PASSTHROUGH):Ball->PassThrough = true;
		Ball->Color = vec3(1.0f, 0.5f, 0.5f);
		break;
	case(INCREASE):Player->Size += vec2(50.0f, 0.0f);
		break;
	case(CONFUSE):
		if (!PostProcessing->Chaos)
			PostProcessing->Confuse = true;
		break;
	default:
		if (!PostProcessing->Confuse)
			PostProcessing->Chaos = true;
		break;
	}
}

// 1 in range chance to return true
bool shouldSpawn(unsigned int range)
{
	unsigned int random = rand() % range;
	return random == 0;
}

float shakeTime = 0.0f;
// change ball's direction and velocity after collision
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
				{
					brick.Destroyed = true;
					this->SpawnPowerUps(brick);
				}
				else
				{
					PostProcessing->Shake = true;
					shakeTime = 0.05f;
				}
					
				Direction dir = std::get<1>(collision);
				vec2 diff_vector = std::get<2>(collision);
				if (!(Ball->PassThrough && !brick.IsSolid)) 
				{
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
	}
	// check collision between ball and player paddle
	Collision result = CheckCollision(*Player, *Ball);
	if (!Ball->Stuck && std::get<0>(result)) 
	{
		Ball->Stuck = Ball->Sticky;
		float centerPaddle = Player->Position.x + Player->Size.x / 2;
		float distance = (Ball->Position.x + Ball->Radius) - centerPaddle;
		float percentage = distance / (Player->Size.x / 2);
		float strengh = 2.0f;
		vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = BALL_VELOCITY.x * percentage * strengh;
		Ball->Velocity.y = -Ball->Velocity.y;
		Ball->Velocity = normalize(Ball->Velocity) * length(oldVelocity);
	}

	for (PowerUp& powerup : this->PowerUps)
	{
		if (!powerup.Destroyed)
		{
			if (powerup.Position.y >= this->Height)
				powerup.Destroyed = true;
			if (CheckCollision(powerup, *Player))
			{
				activatePowerUp(powerup);
				powerup.Destroyed = true;
				powerup.Activated = true;
			}
		}
	}
}

void Game::SpawnPowerUps(GameObject &block)
{
	if (shouldSpawn(50)) 
	{
		this->PowerUps.push_back(PowerUp(SPEED, vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	}
	if (shouldSpawn(45))
	{
		this->PowerUps.push_back(PowerUp(STICKY, vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	}
	if (shouldSpawn(60))
	{
		this->PowerUps.push_back(PowerUp(PASSTHROUGH, vec3(0.5f, 1.0f, 1.0f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	}
	if (shouldSpawn(60))
	{
		this->PowerUps.push_back(PowerUp(INCREASE, vec3(1.0f, 0.6f, 0.4f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	}
	if (shouldSpawn(25))
	{
		this->PowerUps.push_back(PowerUp(CONFUSE, vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
	}
	if (shouldSpawn(45))
	{
		this->PowerUps.push_back(PowerUp(CHAOS, vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
	}
}

bool isOtherPowerUpActive(vector<PowerUp> &powerups, PowerUpType type)
{
	for (PowerUp& powerup : powerups)
	{
		if (powerup.Type == type)
		{
			if (powerup.Activated)
				return true;
		}
	}
	return false;
}

void Game::UpdatePowerUps(float deltaTime) 
{
	for (PowerUp& powerup : this->PowerUps) 
	{
		powerup.Position += powerup.Velocity * deltaTime;
		if (powerup.Activated) 
		{
			powerup.Duration -= deltaTime;
			if (powerup.Duration <= 0.0f) 
			{
				powerup.Activated = false;
				switch (powerup.Type)
				{
				case(STICKY):
					if (!isOtherPowerUpActive(this->PowerUps, STICKY))
					{
						Ball->Sticky = false;
						Player->Color = vec3(1.0f);
					}
					break;
				case(PASSTHROUGH):
					if (!isOtherPowerUpActive(this->PowerUps, PASSTHROUGH)) 
					{
						Ball->PassThrough = false;
						Ball->Color = vec3(1.0f);
					}
					break;
				case(CONFUSE):
					if (!isOtherPowerUpActive(this->PowerUps, CONFUSE))
					{
						PostProcessing->Confuse = false;
					}
					break;
				case(CHAOS):
					if (!isOtherPowerUpActive(this->PowerUps, CHAOS))
					{
						PostProcessing->Chaos = false;
					}
					break;
				default:
					break;
				}
			}
		}
	}
	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), this->PowerUps.end());
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
		if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE]) 
		{
			Ball->Stuck = false;
			this->KeysProcessed[GLFW_KEY_SPACE] = true;
		}
		if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
		{
			PostProcessing->Chaos = !PostProcessing->Chaos;
			this->KeysProcessed[GLFW_KEY_P] = true;
		}
		if (this->Keys[GLFW_KEY_O] &&! this->KeysProcessed[GLFW_KEY_O])
		{
			PostProcessing->Confuse = !PostProcessing->Confuse;
			this->KeysProcessed[GLFW_KEY_O] = true;
		}
	}

	if (this->State == GAME_MENU) 
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
		{
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = true;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
		{
			this->Level = (this->Level > 0 ? this->Level - 1 : 3);
			this->KeysProcessed[GLFW_KEY_S] = true;
		}
	}

	if (this->State == GAME_WIN) 
	{
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
		{
			this->ResetLevel();
			this->ResetPlayer();
			this->State = GAME_MENU;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
		}
	}
}

bool Game::IsCompleted() 
{
	for (GameObject& brick : this->Levels[this->Level].Bricks) 
	{
		if (!brick.Destroyed && !brick.IsSolid)
			return false;
	}
	return true;
}

void Game::Update(float deltaTime)
{
	if (this->State == GAME_ACTIVE) {
		Ball->Move(deltaTime, this->Width);
		this->DoCollisions();
		this->UpdatePowerUps(deltaTime);
		Particles->Update(deltaTime, *Ball, 2, vec2(Ball->Radius / 2.0f));
		if (Ball->Position.y > this->Height)
		{
			this->Lives--;
			this->ResetPlayer();
			if (this->Lives == 0)
			{
				this->ResetLevel();
				this->State = GAME_MENU;
			}
		}
		if (shakeTime > 0.0f)
		{
			shakeTime -= deltaTime;
			if (shakeTime <= 0.0f)
				PostProcessing->Shake = false;
		}
		if (this->Levels[this->Level].IsCompleted())
		{
			this->State = GAME_WIN;
			PostProcessing->Chaos = PostProcessing->Confuse = false;
		}
	}
}

void Game::ResetPlayer()
{
	vec2 playerPos = vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		this->Height - PLAYER_SIZE.y);
	Player->Position = playerPos;
	Player->Size = PLAYER_SIZE;
	vec2 ballPos = playerPos + vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball->Reset(ballPos, BALL_VELOCITY);
	PostProcessing->Chaos = PostProcessing->Confuse = false;
	Ball->Color = Player->Color = vec3(1.0f);
}

void Game::ResetLevel() 
{
	this->Levels[this->Level].Reset();
	this->Lives = 3;
}

void Game::Render()
{
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU)
	{
		PostProcessing->BeginRender();
		Texture2D myTexture;
		myTexture = ResourceManager::GetTexture("background");
		spriteRenderer->DrawSprite(myTexture,
			vec2(0.0f), vec2(this->Width, this->Height), 0.0f);
		this->Levels[this->Level].Draw(*spriteRenderer);
		Player->Draw(*spriteRenderer);
		for (PowerUp& powerup : this->PowerUps)
		{
			if (!powerup.Destroyed)
				powerup.Draw(*spriteRenderer);
		}
		Particles->Draw();
		Ball->Draw(*spriteRenderer);
		std::stringstream ss; ss<< this->Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
		PostProcessing->EndRender();
		PostProcessing->Render(glfwGetTime());
		
	}
	if (this->State == GAME_MENU) 
	{
		Text->RenderText("Press Enter to Start", 220.0f, this->Height / 2, 1.1f);
		Text->RenderText("Press W or S to Select Level", 200.0f, this->Height / 2 + 30.0f, 1.0f);
	}
	if (this->State == GAME_WIN) 
	{
		Text->RenderText("Congratulation!! You Win!!", 180, this->Height / 2 - 30, 1.2f, vec3(0.0, 1.0f, 0.0));
		Text->RenderText("Press Enter to Continue or Press ESC to Quit", 80, this->Height / 2, 1.0f);
	}
}								  
														  
