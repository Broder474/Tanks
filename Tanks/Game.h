#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <list>
#include "Tank.h"
#include "Projectile.h"

class Game
{
public:
	enum {CELL_EMPTY, CELL_PLAYER, CELL_ENEMY, CELL_WALL, CELL_SCRAP};
	Game(int cells_count);
	~Game();
	void loop();
	void quit();
	void handleEvents();
	void render();
	void moveTankX(Tank& tank, int tank_team, int x);
	void moveTankY(Tank& tank, int tank_team, int y);
	void tankFire(Tank& tank);
	std::list<Tank>enemies;
	std::list<Projectile>projectiles;
	Tank player;
private:
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Texture *tex_tank1, *tex_tank2, *tex_health, *tex_projectile;
	bool isRunned = false;
	int** cells;
	int cells_count;
	int health = 3;
	const Uint8* keys;
};