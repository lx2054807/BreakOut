#pragma once
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "shader.h"
#include "texture.h"

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

using namespace glm;

class SpriteRenderer
{
public:
	SpriteRenderer(Shader &shader);
	~SpriteRenderer();

	void DrawSprite(Texture2D &texture, vec2 position,
		vec2 size = vec2(10.0f, 10.0f), float rotate = 0.0f,
		vec3 color = vec3(1.0f));
private:
	Shader shader;
	unsigned int quadVAO;
	void initRenderData();
};
#endif // !SPRITE_RENDERER_H
