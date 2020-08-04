#include "gameobject.h"

GameObject::GameObject():Position(0.0f,0.0f), Size(1.0f,1.0f), Velocity(0.0f,0.0f), Color(1.0f, 1.0f, 1.0f), Rotation(0.0f), IsSolid(false),Destroyed(false), Sprite(){}

GameObject::GameObject(vec2 pos, vec2 size, Texture2D sprite, vec3 color = vec3(1.0f), vec2 velocity = vec2(0.0f)):
Position(pos), Size(size), Velocity(velocity), Sprite(sprite), Color(color), IsSolid(false), Destroyed(false){}

void GameObject::Draw(SpriteRenderer& renderer) 
{
	renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}