#pragma once
#ifndef POWERUP_H
#define POWERUP_H

#include "texture.h"
#include "gameobject.h"
#include <glm/glm/glm.hpp>
using namespace glm;
enum PowerUpType
{
	SPEED,
	STICKY,
	PASSTHROUGH,
	INCREASE,
	CONFUSE,
	CHAOS
};

const vec2 POWERUP_SIZE(60.0f, 20.0f);
const vec2 POWERUP_VELOCITY(0.0f, 150.0f);
class PowerUp : public GameObject 
{
public:
	PowerUpType Type;
	float Duration;
	bool Activated;
	PowerUp(PowerUpType type, vec3 color, float duration, vec2 position, Texture2D texture) :GameObject(position, POWERUP_SIZE, texture, color, POWERUP_VELOCITY), Type(type), Duration(duration), Activated() {}
};
#endif // !POWERUP_H
