#pragma once

#include <SDL.h>

class Projectile
{
public:
	enum Direction {TOP, RIGHT, BOTTOM, LEFT};
	Projectile(int x, int y, int direction);
	~Projectile();
	void move();
	int getX() { return x; }
	int getY() { return y; }
	Uint64 getNextMove() { return next_move; }
private:
	int x, y, direction;
	Uint64 next_move = 0;
};