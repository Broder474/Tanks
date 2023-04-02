#include "Game.h"

Game::Game(int cells_count): cells_count(cells_count), player(rand() % cells_count, rand() % cells_count)
{
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING);
    win = SDL_CreateWindow("Tanks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, 0);

    // textures loading
    tank1 = IMG_LoadTexture(ren, "images/tank1.png");
    tank2 = IMG_LoadTexture(ren, "images/tank2.png");
    
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
    SDL_Event e;
    SDL_PollEvent(&e);
    switch (e.type)
    {
    case SDL_QUIT:
        quit();
    }
}

void Game::render()
{
    SDL_SetRenderDrawColor(ren, 40, 40, 40, 0);
    SDL_RenderClear(ren);

    // render cells content
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
                SDL_RenderCopy(ren, tank1, nullptr, &cell_rect);
                break;
            case CELL_ENEMY: // enemies
                SDL_RenderCopy(ren, tank2, nullptr, &cell_rect);
                break;
            }

            cell_rect.y += 25;
        }
    }

    // render lines
    SDL_SetRenderDrawColor(ren, 60, 60, 60, 0);
    for (int x = 0; x < cells_count; x++)
    {
        SDL_RenderDrawLine(ren, x * 25, 0, x * 25, 800);
        SDL_RenderDrawLine(ren, 0, x * 25, 800, x * 25);
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