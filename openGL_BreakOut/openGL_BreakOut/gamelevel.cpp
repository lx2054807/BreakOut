#include "gamelevel.h"
#include <fstream>
#include <sstream>
using namespace std;
using namespace glm;

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight) 
{
	this->Bricks.clear();

	unsigned int tileCode;
	GameLevel level;
	string line;
	ifstream fstream(file);
	vector<vector<unsigned int>> tileData;
	if (fstream) 
	{
		while (getline(fstream, line)) 
		{
			istringstream sstream(line);
			vector<unsigned int> row;
			while (sstream >> tileCode)
				row.push_back(tileCode);
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			this->init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer& renderer) 
{
	for (GameObject& tile : this->Bricks) 
	{
		if (!tile.Destroyed)
			tile.Draw(renderer);
	}
}

bool GameLevel::IsCompleted() 
{
	for (GameObject& tile : this->Bricks)
		if (!tile.Destroyed && !tile.IsSolid)
			return false;
	return true;
}

void GameLevel::init(vector<vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight) 
{
	unsigned int height = tileData.size();
	unsigned int width = tileData[0].size();
	float unit_width = levelWidth / width, unit_height = levelHeight / height;
	for (unsigned int y = 0; y < height; ++y) 
	{
		for (unsigned int x = 0; x < width; ++x) 
		{
			unsigned int data = tileData[y][x];
			if (data == 1) //Solid
			{
				vec2 pos(unit_width * x, unit_height * y);
				vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), vec3(0.8f, 0.8f, 0.7f));
				obj.IsSolid = true;
				this->Bricks.push_back(obj);
			}
			else if (data > 1)
			{
				vec3 color = vec3(1.0f);
				if (data == 2)
					color = vec3(0.2f, 0.6f, 1.0f);
				else if (data == 3)
					color = vec3(0.0f, 0.7f, 0.0f);
				else if (data == 4)
					color = vec3(0.8f, 0.8f, 0.4f);
				else if (data == 5)
					color = vec3(1.0f, 0.5f, 0.0f);
				vec2 pos(unit_width * x, unit_height * x);
				vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::GetTexture("block"), color);
				this->Bricks.push_back(obj);
			}
		}
	}
}