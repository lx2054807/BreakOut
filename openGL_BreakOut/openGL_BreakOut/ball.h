#pragma once
#ifndef BALL_H
#define BALL_H
#include "gameobject.h"

class  BallObject:GameObject
{
public:
	float Radius;
	bool Stuck;
	BallObject();
	BallObject(vec2 pos, float radius, vec2 velocity, Texture2D texture);

	vec2 Move(float dt, unsigned int window_width);
	void Reset(vec2 pos, vec2 velocity);
};

#endif // !BALL_H
