#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include<glad/glad.h>
#include<glm/glm/glm.hpp>
using namespace glm;
#include "texture.h"
#include "sprite_renderer.h"

class GameObject
{
public:
	vec2 Position, Size, Velocity;
	vec3 Color;
	float Rotation;
	bool IsSolid;
	bool Destroyed;
	Texture2D Sprite;
	GameObject();
	GameObject(vec2 pos, vec2 size, Texture2D sprite, vec3 color = vec3(1.0f), vec2 velocity = vec2(0.0f));
	virtual void Draw(SpriteRenderer& spriteRenderer);
};

#endif // !GAMEOBJECT_H
