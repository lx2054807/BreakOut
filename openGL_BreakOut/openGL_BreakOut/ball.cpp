#include "ball.h"

BallObject::BallObject() :GameObject(), Radius(12.5f), Stuck(true){}

BallObject::BallObject(vec2 pos, float radius, vec2 velocity, Texture2D texture) : GameObject(pos, vec2(radius * 2.0f, radius * 2.0f), texture, vec3(1.0f), velocity), Radius(radius), Stuck(true) {}

vec2 BallObject::Move(float deltaTime, unsigned int window_width) 
{
	if (!this->Stuck) 
	{
		this->Position += this->Velocity * deltaTime;
		if (this->Position.x <= 0.0f) 
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = 0.0f;
		}
		else if (this->Position.x >= window_width - this->Size.x) 
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = window_width - this->Size.x;
		}
		if (this->Position.y <= 0.0f) 
		{
			this->Velocity.y = -this->Velocity.y;
			this->Position.y = 0.0f;
		}
	}
	return this->Position;
}

void BallObject::Reset(vec2 pos, vec2 velocity) 
{
	this->Position = pos;
	this->Velocity = velocity;
	this->Stuck = true;
}