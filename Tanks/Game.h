#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <list>
#include <set>
#include <algorithm>
#include "Tank.h"
#include "Projectile.h"
#include "Position.h"

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
	void updateProjectiles();
	std::list<Tank>enemies;
	std::list<Projectile>projectiles;
	Tank player;
private:
	SDL_Window* win;
	SDL_Renderer* ren;
	SDL_Texture *tex_tank1, *tex_tank2, *tex_health, *tex_projectile, *tex_scrap;
	SDL_Texture* tex_won, *tex_lost;
	SDL_Rect dstrect_won{ 200, 200 }, dstrect_lost{ 200, 200 };
	TTF_Font* font;
	bool isRunned = false;
	int** cells;
	int cells_count;
	int health = 3;
	const Uint8* keys;
};

struct Node
{
	Node(Position pos, Node* prev) : pos(pos), prev(prev) {};
	Position* generateNext(Node& basic, int cells_count, int** map, std::list<Position>firepos);
	void findWay(Position pos, std::list<Position>&way);
	int getTotalNodesAmount();
	void delete_repeats(std::list<Node>& temp_nodes);

	Node* prev;
	Position pos;
	std::list<Node>nodes;
	bool generated = false;
};

SDL_Texture* CreateTextureFromText(SDL_Renderer* ren, TTF_Font* font, const char* text, SDL_Color color);