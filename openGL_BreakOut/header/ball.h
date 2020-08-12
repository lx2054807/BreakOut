#pragma once
#ifndef BALLOBJECT_H
#define BALLOBJECT_H
#include "gameobject.h"
#include "texture.h"
#include <glm/glm/glm.hpp>
#include <glad/glad.h>

class  BallObject:public GameObject
{
public:
	float Radius;
	bool Stuck;
	bool Sticky;
	bool PassThrough;
	BallObject();
	BallObject(vec2 pos, float radius, vec2 velocity, Texture2D texture);

	vec2 Move(float dt, unsigned int window_width);
	void Reset(vec2 pos, vec2 velocity);
};

#endif // !BALLOBJECT_H
