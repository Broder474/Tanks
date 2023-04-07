#include "Game.h"

Game::Game(int cells_count): cells_count(cells_count), player(rand() % cells_count, rand() % cells_count)
{
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    win = SDL_CreateWindow("Tanks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
    ren = SDL_CreateRenderer(win, -1, 0);

    keys = SDL_GetKeyboardState(nullptr);

    // image textures loading
    tex_tank1 = IMG_LoadTexture(ren, "images/tank1.png");
    tex_tank2 = IMG_LoadTexture(ren, "images/tank2.png");
    tex_health = IMG_LoadTexture(ren, "images/health.png");
    SDL_SetTextureAlphaMod(tex_health, 128);
    tex_projectile = IMG_LoadTexture(ren, "images/projectile.png");
    tex_scrap = IMG_LoadTexture(ren, "images/scrap.png");

    font = TTF_OpenFont("fonts/calibri.ttf", 32);

    // text textures
    SDL_Color color{ 0, 255, 0, 0 };
    tex_won = CreateTextureFromText(ren, font, "You won!", color);
    SDL_QueryTexture(tex_won, nullptr, nullptr, &dstrect_won.w, &dstrect_won.h);
    tex_lost = CreateTextureFromText(ren, font, "You lost...", color);
    SDL_QueryTexture(tex_lost, nullptr, nullptr, &dstrect_lost.w, &dstrect_lost.h);

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
    for (int i = 0; i < 1 + rand() % 10; i++)
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
        enemies.push_back({x, y});
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
        if (health > 0 && enemies.size() != 0)
            handleEvents();
        else
        {
            SDL_Event e;
            SDL_PollEvent(&e);
            if (e.type == SDL_QUIT)
                quit();
        }
    }
}

void Game::handleEvents()
{
    if (keys[SDL_SCANCODE_LSHIFT])
    {
        // player tank rotating
        if (keys[SDL_SCANCODE_W])
            player.setRotating(0);
        else if (keys[SDL_SCANCODE_S])
            player.setRotating(180);
        else if (keys[SDL_SCANCODE_A])
            player.setRotating(270);
        else if (keys[SDL_SCANCODE_D])
            player.setRotating(90);
    }
    else
    {
        // player tank moving
        if (keys[SDL_SCANCODE_W])
            moveTankY(player, CELL_PLAYER, -1);
        else if (keys[SDL_SCANCODE_S])
            moveTankY(player, CELL_PLAYER, 1);
        else if (keys[SDL_SCANCODE_A])
            moveTankX(player, CELL_PLAYER, -1);
        else if (keys[SDL_SCANCODE_D])
            moveTankX(player, CELL_PLAYER, 1);
    }

    // player tank shooting
    if (keys[SDL_SCANCODE_SPACE])
        tankFire(player);

    // projectiles moving
    for (auto& projectile : projectiles)
    {
        if (projectile.getNextMove() > SDL_GetTicks64())
            continue;
        projectile.move();
    }

    for (auto& enemy : enemies)
        if (enemy.getNextMove() < SDL_GetTicks64() && abs(player.getX() - enemy.getX()) < 10 && abs(player.getY() - enemy.getY()) < 10)
        {
            std::list<Position>fire_positions;
            int target_x = player.getX(), target_y = player.getY();
            int enemy_x = enemy.getX(), enemy_y = enemy.getY();

            // update available fire positions
            // top direction
            for (int y = target_y; y > -1 && cells[target_x][y] != CELL_WALL; y--)
                if (cells[target_x][y] == CELL_EMPTY || (target_x == enemy_x && y == enemy_y))
                    fire_positions.push_back({ target_x, y });

            // bottom direction
            for (int y = target_y; cells[target_x][y] != CELL_WALL && y < cells_count; y++)
                if (cells[target_x][y] == CELL_EMPTY || (target_x == enemy_x && y == enemy_y))
                    fire_positions.push_back({ target_x, y });

            // left direction
            for (int x = target_x; x > -1 && cells[x][target_y] != CELL_WALL; x--)
                if (cells[x][target_y] == CELL_EMPTY || (x == enemy_x && target_y == enemy_y))
                    fire_positions.push_back({ x, target_y });

            // right direction
            for (int x = target_x; x < cells_count && cells[x][target_y] != CELL_WALL; x++)
                if (cells[x][target_y] == CELL_EMPTY || (x == enemy_x && target_y == enemy_y))
                    fire_positions.push_back({ x, target_y });

            Node start({ enemy_x, enemy_y }, nullptr);
            Position* firepos = nullptr;

            // check if the tank is on the fire position
            for (auto& pos : fire_positions)
                if (start.pos == pos)
                    firepos = &pos;

            // searching the nearest fire position
            while (firepos == nullptr)
                firepos = start.generateNext(start, cells_count, cells, fire_positions);

            // way creating
            std::list<Position>way;
            start.findWay(*firepos, way);
            if (way.size())
                way.pop_front(); // remove start position
            else
            {
                // rotate enemy tank to player
                int diff_x = enemy_x - target_x, diff_y = enemy_y - target_y;
                auto rotate_and_fire = [](Tank& tank, int rotating) {
                    if (tank.getRotating() == rotating)
                        return;
                    tank.setRotating(rotating);
                    Uint64 curr_time = SDL_GetTicks64();
                    if (tank.getNextFire() <= curr_time)
                        tank.setNextFire(curr_time + 750);
                };

                if (diff_x > 0)
                    rotate_and_fire(enemy, 270);
                else if (diff_x < 0)
                    rotate_and_fire(enemy, 90);
                else if (diff_y > 0)
                    rotate_and_fire(enemy, 0);
                else if (diff_y < 0)
                    rotate_and_fire(enemy, 180);

                // enemy tank fire
                if (enemy.getNextFire() <= SDL_GetTicks64())
                    tankFire(enemy);
            }
            if (way.size())
            {
                // enemy tank moving
                int diff_x = way.front().x - enemy_x, diff_y = way.front().y - enemy_y;
                if (diff_x)
                    moveTankX(enemy, CELL_ENEMY, diff_x);
                else if (diff_y)
                    moveTankY(enemy, CELL_ENEMY, diff_y);
            }
        }

    updateProjectiles();

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

    // render walls, scrap and player
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
            case CELL_SCRAP:
                SDL_RenderCopy(ren, tex_scrap, nullptr, &cell_rect);
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
        SDL_RenderCopyEx(ren, tex_tank2, nullptr, &cell_rect, enemy.getRotating(), nullptr, SDL_FLIP_NONE);
    }

    // render flying projectils
    for (auto& projectile : projectiles)
    {
        SDL_Rect dstrect{ projectile.getX() * 25, projectile.getY() * 25, 25, 25 };
        SDL_RenderCopy(ren, tex_projectile, nullptr, &dstrect);
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

    // Game over 
    if (health < 1 || enemies.size() == 0)
        if (health < 1)
            SDL_RenderCopy(ren, tex_lost, nullptr, &dstrect_lost);
        else
            SDL_RenderCopy(ren, tex_won, nullptr, &dstrect_won);

    SDL_RenderPresent(ren);
}

void Game::quit()
{
    for (int i = 0; i < cells_count; i++)
        delete[] cells[i];
    delete[] cells;

    isRunned = false;
    TTF_CloseFont(font);
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
    tank.setRotating(x > 0 ? 90 : 270);
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
    tank.setRotating(y > 0 ? 180 : 0);
}

void Game::tankFire(Tank& tank)
{
    if (tank.getNextFire() > SDL_GetTicks64())
        return;

    int x = tank.getX(), y = tank.getY(), direction = Projectile::TOP;
    switch (tank.getRotating())
    {
    case 0:
        direction = Projectile::TOP;
        y--;
        break;
    case 90:
        direction = Projectile::RIGHT;
        x++;
        break;
    case 180:
        direction = Projectile::BOTTOM;
        y++;
        break;
    case 270:
        direction = Projectile::LEFT;
        x--;
        break;
    }
    projectiles.push_back({ x, y, direction });
    updateProjectiles();

    // tank reloading
    tank.setNextFire(SDL_GetTicks64() + 1000);
}

void Game::updateProjectiles()
{
    for (auto projectile = projectiles.begin(); projectile != projectiles.end(); projectile++)
    {
        int x = projectile->getX(), y = projectile->getY();

        // delete projectile if it left the map or on collision with wall
        if ((x < 0 || x > cells_count - 1 || y < 0 || y > cells_count - 1) || (cells[x][y] == CELL_WALL))
        {
            projectile = projectiles.erase(projectile);
            break;
        }

        if (x == player.getX() && y == player.getY()) // collision with player tank
        {
            health--;
            projectile = projectiles.erase(projectile);
        }
        else
            for (auto enemy = enemies.begin(); enemy != enemies.end(); enemy++)
                if (x == enemy->getX() && y == enemy->getY()) // collision with enemy tank
                {
                    enemy = enemies.erase(enemy);
                    cells[x][y] = CELL_SCRAP;
                    projectile = projectiles.erase(projectile);
                    break;
                }
        if (projectile == projectiles.end())
            break;
    }
}

Position* Node::generateNext(Node& basic, int cells_count, int** map, std::list<Position>firepos)
{
    Position* found_pos = nullptr;
    if (!generated)
    {
        std::list<Node>temp_nodes;
        if (pos.x - 1 > -1 && map[pos.x - 1][pos.y] == Game::CELL_EMPTY)
            temp_nodes.push_back({ {pos.x - 1, pos.y}, this });
        if (pos.x + 1 < cells_count && map[pos.x + 1][pos.y] == Game::CELL_EMPTY)
            temp_nodes.push_back({ { pos.x + 1, pos.y }, this });
        if (pos.y - 1 > -1 && map[pos.x][pos.y - 1] == Game::CELL_EMPTY)
            temp_nodes.push_back({ { pos.x, pos.y - 1 }, this });
        if (pos.y + 1 < cells_count && map[pos.x][pos.y + 1] == Game::CELL_EMPTY)
            temp_nodes.push_back({ { pos.x, pos.y + 1 }, this });

        // delete repeating nodes
        basic.delete_repeats(temp_nodes);

        for (auto& node : temp_nodes)
            nodes.push_back(node);
        generated = true;

        for (auto& node : nodes)
            if (std::find(firepos.begin(), firepos.end(), node.pos) != firepos.end())
                return &node.pos;
    }
    else
        for (auto& node : nodes)
        {
            found_pos = node.generateNext(basic, cells_count, map, firepos);
            if (found_pos != nullptr)
                return found_pos;
        }
    return nullptr;
}

int Node::getTotalNodesAmount()
{
    int amount = nodes.size();
    for (auto& node : nodes)
        if (node.generated)
            amount += node.getTotalNodesAmount();
    return amount;
}

void Node::delete_repeats(std::list<Node>& temp_nodes)
{
    if (!generated)
        return;
    for (auto it = temp_nodes.begin(); it != temp_nodes.end(); it++)
    {
        if (std::find_if(nodes.begin(), nodes.end(), [&](Node node) {
            return (node.pos == it->pos);
            }) != nodes.end())
        {
            it = temp_nodes.erase(it);
            if (it == temp_nodes.end())
                break;
        }
    }
    for (auto& node : nodes)
        node.delete_repeats(temp_nodes);
}

void Node::findWay(Position pos, std::list<Position>&way)
{
    for (auto& node : nodes)
        if (node.pos == pos)
        {
            way.insert(way.end(), { this->pos, node.pos });
            break;
        }
        else
        {
            node.findWay(pos, way);
            if (way.size())
            {
                way.push_front(this->pos);
                break;
            }
        }
}

SDL_Texture* CreateTextureFromText(SDL_Renderer* ren, TTF_Font* font, const char* text, SDL_Color color)
{
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    return tex;
}