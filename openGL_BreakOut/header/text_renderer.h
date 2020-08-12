#pragma once
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <glm/glm/glm.hpp>
using namespace glm;
#include "shader.h"
#include "texture.h"
struct Character
{
	unsigned int TextureID;
	ivec2 Size;
	ivec2 Bearing;
	unsigned int Avance;
};
class TextRenderer
{
public:
	map<char, Character> Characters;
	Shader TextShader;
	TextRenderer(unsigned int width, unsigned int height);
	void Load(string font, unsigned int fontSize);
	void RenderText(string text, float x, float y, float scale, vec3 color = vec3(1.0f));
private:
	unsigned int VAO, VBO;
};
#endif // !TEXT_RENDERER_H
