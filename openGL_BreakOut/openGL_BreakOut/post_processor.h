#pragma once
#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glm/glm/glm.hpp>
using namespace glm;
#include "shader.h"
#include "texture.h"

class PostProcessor 
{
public:
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Height, Width;
	bool Confuse, Chaos, Shake;
	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	void BeginRender();
	void EndRender();
	void Render(float time);
private:
	unsigned int MSFBO, FBO;
	unsigned int RBO;
	unsigned int VAO;
	void initRendererData();
};
#endif // !POST_PROCESSOR_H
