#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
map<string, Texture2D> ResourceManager::Textures;
map<string, Shader> ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFIle, string name) 
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFIle);
	return Shaders[name];
}

Shader ResourceManager::GetShader(string name) 
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, string name) 
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(string name)
{
	return Textures[name];
}

void ResourceManager::Clear() 
{
	for (auto shader : Shaders)
	{
		glDeleteProgram(shader.second.ID);
	}
	for (auto texture : Textures)
	{
		glDeleteTextures(1, &texture.second.ID);
	}
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	string vertexCode;
	string fragmentCode;
	string geometryCode;
	try 
	{
		ifstream vertexShaderFile(vShaderFile);
		ifstream fragmentShaderFile(fShaderFile);
		stringstream vShaderStream, fShaderStream;

		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (gShaderFile != nullptr) 
		{
			ifstream geometryShaderFile(gShaderFile);
			stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch(exception e)
	{
		cout << "ERROR::SHADER Failed to read shader files" << endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();

	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha) 
{
	Texture2D texture;
	if (alpha) 
	{
		texture.Image_Format = GL_RGBA;
		texture.Internal_Format = GL_RGBA;
	}
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	if (data) {
		texture.Generate(width, height, data);
		stbi_image_free(data);
	}
	return texture;
}