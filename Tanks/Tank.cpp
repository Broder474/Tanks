#include "Tank.h"

Tank::Tank() {};

Tank::Tank(int x, int y) : x(x), y(y)
{

}

Tank::~Tank()
{

}

void Tank::addX(int x)
{
	next_move = SDL_GetTicks64() + 200;
	this->x += x;
}

void Tank::addY(int y)
{
	next_move = SDL_GetTicks64() + 200;
	this->y += y;
}