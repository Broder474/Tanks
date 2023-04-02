#include "Game.h"

Game::Game(int cells_count): cells_count(cells_count), player(rand() % cells_count, rand() % cells_count)
{
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING);
    win = SDL_CreateWindow("Tanks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, 0);

    keys = SDL_GetKeyboardState(nullptr);

    // textures loading
    tex_tank1 = IMG_LoadTexture(ren, "images/tank1.png");
    tex_tank2 = IMG_LoadTexture(ren, "images/tank2.png");
    tex_health = IMG_LoadTexture(ren, "images/health.png");
    SDL_SetTextureAlphaMod(tex_health, 128);
    
    cells = new int* [cells_count];
    for (int i = 0; i < cells_count; i++)
    {
        cells[i] = new int[cells_count];
        for (int j = 0; j < cells_count; j++)
            cells[i][j] = CELL_EMPTY;
    }

    // spawn walls
    for (int i = 0; i < rand() % 100; i++)
        cells[rand() % cells_count][rand() % cells_count] = CELL_WALL;

    // spawn player
    cells[player.getX()][player.getY()] = CELL_PLAYER;

    // spawn enemies
    for (int i = 1; i < 1 + rand() % 10; i++)
    {
        int x, y;
        while (true)
        {
            x = rand() % cells_count, y = rand() % cells_count;
            if (cells[x][y] == CELL_EMPTY)
            {
                cells[x][y] = CELL_ENEMY;
                break;
            }
        }
        Tank tank(x, y);
        enemies.push_back(tank);
    }
    

    isRunned = true;
}

Game::~Game() {};

void Game::loop()
{
    while (isRunned)
    {
        SDL_Delay(50);
        render();
        handleEvents();
    }
}

void Game::handleEvents()
{
    if (keys[SDL_SCANCODE_W])
        moveTankY(player, CELL_PLAYER, -1);
    else if (keys[SDL_SCANCODE_S])
        moveTankY(player, CELL_PLAYER, 1);
    else if (keys[SDL_SCANCODE_A])
        moveTankX(player, CELL_PLAYER, -1);
    else if (keys[SDL_SCANCODE_D])
        moveTankX(player, CELL_PLAYER, 1);

    SDL_Event e;
    SDL_PollEvent(&e);
    switch (e.type)
    {
    case SDL_QUIT:
        quit();
        break;
    }
}

void Game::render()
{
    SDL_SetRenderDrawColor(ren, 40, 40, 40, 0);
    SDL_RenderClear(ren);

    // render walls and player
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
    for (int x = 0; x < cells_count; x++)
    {
        SDL_Rect cell_rect{ x * 25, 0, 25, 25 };
        for (int y = 0; y < cells_count; y++)
        {
            switch (cells[x][y])
            {
            case CELL_EMPTY: // empty cells
                break;
            case CELL_WALL: // walls
                SDL_RenderFillRect(ren, &cell_rect);
                break;
            case CELL_PLAYER: // player
                SDL_RenderCopyEx(ren, tex_tank1, nullptr, &cell_rect, player.getRotating(), nullptr, SDL_FLIP_NONE);
                break;
            }
            cell_rect.y += 25;
        }
    }

    // render enemies
    for (auto& enemy : enemies)
    {
        SDL_Rect cell_rect{ enemy.getX() * 25, enemy.getY() * 25, 25, 25 };
        SDL_RenderCopy(ren, tex_tank2, nullptr, &cell_rect);
    }

    // render grid
    SDL_SetRenderDrawColor(ren, 60, 60, 60, 0);
    for (int x = 0; x < cells_count; x++)
    {
        SDL_RenderDrawLine(ren, x * 25, 0, x * 25, 800);
        SDL_RenderDrawLine(ren, 0, x * 25, 800, x * 25);
    }

    // render health points
    SDL_Rect dstrect{ 0, 0, 25, 25 };
    for (int i = 0; i < health; i++)
    {
        dstrect.x = i * 30;
        SDL_RenderCopy(ren, tex_health, nullptr, &dstrect);
    }

    SDL_RenderPresent(ren);
}

void Game::quit()
{
    for (int i = 0; i < cells_count; i++)
        delete[] cells[i];
    delete[] cells;

    isRunned = false;
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void Game::moveTankX(Tank& tank, int tank_team, int x)
{
    int x2 = tank.getX() + x, y2 = tank.getY();
    if (x2 < 0 || x2 > cells_count - 1 || tank.getNextMove() > SDL_GetTicks64())
        return;
    if (cells[x2][y2] == CELL_EMPTY)
    {
        cells[tank.getX()][y2] = CELL_EMPTY;
        cells[x2][y2] = tank_team;
        tank.addX(x);
    }
}

void Game::moveTankY(Tank& tank, int tank_team, int y)
{
    int x2 = tank.getX(), y2 = tank.getY() + y;
    if (y2 < 0 || y2 > cells_count - 1 || tank.getNextMove() > SDL_GetTicks64())
        return;
    if (cells[x2][y2] == CELL_EMPTY)
    {
        cells[x2][tank.getY()] = CELL_EMPTY;
        cells[x2][y2] = tank_team;
        tank.addY(y);
    }
}