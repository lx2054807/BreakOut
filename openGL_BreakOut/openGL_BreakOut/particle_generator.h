#pragma once
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <glm/glm/glm.hpp>
#include "shader.h"
#include "texture.h"
#include "gameobject.h"
#include <vector>
using namespace glm;
using namespace std;
struct Particle
{
	vec2 Position, Velocity;
	vec4 Color;
	float Life;
	Particle() :Position(vec2(0.0f)), Velocity(vec2(0.0f)), Color(vec4(1.0f)), Life(0.0f) {}
};

class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
	void Update(float deltaTime, GameObject& object, unsigned int newParticles, vec2 offset = vec2(0.0f));
	void Draw();
private:
	vector<Particle> particles;
	unsigned int amount;
	Shader shader;
	Texture2D texture;
	unsigned int VAO;
	void init();
	unsigned int firstUnusedParticle();
	void respawnParticle(Particle& particle, GameObject& object, vec2 offset = vec2(0.0f));
};
#endif // !PARTICLE_GENERATOR_H
