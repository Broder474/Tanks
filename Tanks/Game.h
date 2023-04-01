#pragma once

#include <iostream>
#include <SDL.h>
#include <list>
#include "Tank.h"

class Game
{
public:
	enum {CELL_EMPTY, CELL_PLAYER, CELL_ENEMY, CELL_WALL};
	Game();
	~Game();
	void loop();
	void quit();
	void handleEvents();
	void render();
	std::list<Tank>tanks;
private:
	SDL_Window* win;
	SDL_Renderer* ren;
	bool isRunned = false;
	int** cells;
	int cells_count = 32;
	int health = 3;
};