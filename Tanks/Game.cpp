#include "Game.h"

Game::Game()
{
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING);
    win = SDL_CreateWindow("Tanks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, 0);
    
    cells = new int* [cells_count];
    for (int i = 0; i < cells_count; i++)
    {
        cells[i] = new int[cells_count];
        for (int j = 0; j < cells_count; j++)
            cells[i][j] = CELL_EMPTY;
    }

    for (int i = 0; i < rand() % 100; i++)
        cells[rand() % cells_count][rand() % cells_count] = CELL_WALL;

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

    // render lines
    SDL_SetRenderDrawColor(ren, 60, 60, 60, 0);
    for (int x = 0; x < cells_count; x++)
    {
        SDL_RenderDrawLine(ren, x * 25, 0, x * 25, 800);
        SDL_RenderDrawLine(ren, 0, x * 25, 800, x * 25);
    }

    // render cells content
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
    for (int x = 0; x < cells_count; x++)
    {
        SDL_Rect cell_rect{ x * 25, 0, 25, 25 };
        for (int y = 0; y < cells_count; y++)
        {
            if (cells[x][y] == CELL_WALL)
                SDL_RenderFillRect(ren, &cell_rect);

            cell_rect.y += 25;
        }
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