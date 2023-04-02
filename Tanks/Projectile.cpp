#include "Projectile.h"

Projectile::Projectile(int x, int y, int direction): x(x), y(y), direction(direction)
{

}

Projectile::~Projectile() {};

void Projectile::move()
{
	if (next_move < SDL_GetTicks64())
	{
		next_move = SDL_GetTicks64() + 100;
		if (direction == TOP)
			y--;
		else if (direction == RIGHT)
			x++;
		else if (direction == BOTTOM)
			y++;
		else if (direction == LEFT)
			x--;
	}
}