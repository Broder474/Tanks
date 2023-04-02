#pragma once
#include <SDL.h>

class Tank
{
public:
	Tank();
	Tank(int x, int y);
	~Tank();
	int getX() { return x; }
	void setX(int x) { this->x = x; }
	int getY() { return y; }
	void setY(int y) { this->y = y; }
	void addX(int x);
	void addY(int y);
	int getRotating() { return rotating; }
	Uint64 getNextMove() { return next_move; }
	Uint64 getNextFire() { return next_fire; }
	void setNextFire(Uint64 next_fire) { this->next_fire = next_fire; }
private:
	int x = 0, y = 0, rotating = 0;
	Uint64 next_move; // next time when the tank can move
	Uint64 next_fire; // next time when the tank can shoot
};