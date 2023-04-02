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
	x > 0 ? rotating = 90 : rotating = 270;
}

void Tank::addY(int y)
{
	next_move = SDL_GetTicks64() + 200;
	this->y += y;
	y > 0 ? rotating = 180 : rotating = 0;
}