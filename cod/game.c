#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<time.h>
#include"game.h"
#include"screen.h"

Piece current_piece;
Piece next_piece;
Piece next_next_piece;
extern GameState gameState;

static Uint32 lastFall = 0;
const Uint32 fallInterval = 500;

static int score = 0;
static int level = 0;
static int total_lines_cleared = 0;

char pieces[NUM_PIECES][4][4] = {
    { // L
        { ' ', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // Inverted L
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // Z
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { '#', '#', ' ', ' ' },
        { ' ', '#', '#', ' ' }
    },
    { // Inverted Z
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', '#', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // O
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', '#', ' ' },
        { ' ', '#', '#', ' ' }
    },
    { // I
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' }
    },
    { // T
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { '#', '#', '#', ' ' }
    }
};

Point queue[Width * Height];

void rotate_piece(char shape[4][4], int *bomb_x, int *bomb_y)
{
    char temp[4][4];
    int bomb_marker[4][4] = {0};

    if (bomb_x && bomb_y && *bomb_x >= 0 && *bomb_y >= 0)
        bomb_marker[*bomb_y][*bomb_x] = 1;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j][3 - i] = shape[i][j];

            if (bomb_marker[i][j])
            {
                *bomb_y = j;
                *bomb_x = 3 - i;
            }
        }
    }

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            shape[i][j] = temp[i][j];
}

void create_board(char board[Height + VISIBLE_OFFSET][Width])
{
    for (int i = 0; i < Height + VISIBLE_OFFSET; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            board[i][j] = ' ';
        }
    }
}


void update_score_and_level(int lines_cleared)
{
    static const int score_table[] = {0, 40, 100, 300, 1200};

    if(lines_cleared >= 1 && lines_cleared <= 4)
    {
        score += score_table[lines_cleared] * (level + 1);
        total_lines_cleared += lines_cleared;
        level = total_lines_cleared/10;
    }
}
int get_score(void)
{
    return score;
}

int get_level(void)
{
    return level;
}

void reset_score_and_level(void)
{
    score = 0;
    level = 0;
    total_lines_cleared = 0;
}
void explode_around(char board[Height + VISIBLE_OFFSET][Width], int start_x, int start_y)
{
    typedef struct { int x, y; } Point;
    Point queue[Width * Height];
    int front = 0, rear = 0;

    queue[rear++] = (Point){ start_x, start_y };

    while (front < rear)
    {
        Point p = queue[front++];
        
        for (int i = -2; i <= 1; i++)
        {
            for (int j = -2; j <= 1; j++)
            {
                int x = p.x + j;
                int y = p.y + i;
                
                if (x >= 0 && x < Width && y >= 0 && y < Height)
                {
                    char *cell = &board[y + VISIBLE_OFFSET][x];

                    if (*cell == 'B')
                    {
                        queue[rear++] = (Point){ x, y }; // bombă nouă descoperită
                    }

                    *cell = ' ';
                }
            }
        }
    }
}
void clear_full_lines(char board[Height + VISIBLE_OFFSET][Width])
{
    int lines_cleared = 0;
    for (int i = Height - 1; i >= 0; i--)
    {
        int full_line = 1;
        for (int j = 0; j < Width; j++)
        {
            if (board[i + VISIBLE_OFFSET][j] == ' ')
            {
                full_line = 0;
                break;
            }
        }
        if (full_line)
        {
            lines_cleared++;
            for (int j = 0; j < Width; j++)
            {
                if (board[i + VISIBLE_OFFSET][j] == 'B')
                {
                    explode_around(board, j, i); // x = col, y = row
                }
            }
            for (int k = i; k > -VISIBLE_OFFSET; k--)
            {
                for (int j = 0; j < Width; j++)
                {
                    board[k + VISIBLE_OFFSET][j] = board[k - 1 + VISIBLE_OFFSET][j];
                }
            }
            for (int j = 0; j < Width; j++)
            {
                board[0][j] = ' ';
            }
            i++; // Recheck same line
        }
    }
    if(lines_cleared > 0)
    {
        update_score_and_level(lines_cleared);
    }
}

int check_collision(char board[Height + VISIBLE_OFFSET][Width], int newX, int newY, char shape[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape[i][j] != ' ')
            {
                int y = newY + i;
                int x = newX + j;
                if (y >= Height || x < 0 || x >= Width || board[y + VISIBLE_OFFSET][x] != ' ')
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

Piece generate_random_piece(void)
{
    int r = rand() % NUM_PIECES;
    Piece new_piece;
    new_piece.type = (PieceType)r;

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            new_piece.shape[i][j] = pieces[r][i][j];
        }
    }

    new_piece.x = (Width / 2) - 2;
    new_piece.y = 0;
    new_piece.has_bomb = (rand() % 10 == 0);

    if (new_piece.has_bomb)
    {
        
        int found = 0;
        while (!found)
        {
            int i = rand() % 4;
            int j = rand() % 4;
            if (new_piece.shape[i][j] != ' ')
            {
                new_piece.bomb_y = i;
                new_piece.bomb_x = j;
                found = 1;
            }
        }
    }
    else
    {
        new_piece.bomb_x = -1;
        new_piece.bomb_y = -1;
    }

    return new_piece;
}
int load_high_score()
{
    FILE *f = fopen("highscore.txt", "r");
    int score = 0;
    if (f)
    {
        fscanf(f, "%d", &score);
        fclose(f);
    }
    return score;
}

void save_high_score(int score)
{
    FILE *f = fopen("highscore.txt", "w");
    if (f)
    {
        fprintf(f, "%d", score);
        fclose(f);
    }
}
void spawn_random_piece(char board[Height + VISIBLE_OFFSET][Width],int *high_score)
{
    current_piece = next_piece;
    next_piece = next_next_piece;
    next_next_piece = generate_random_piece();

    current_piece.x = (Width / 2) - 2;
    current_piece.y = 0;

    while (check_collision(board, current_piece.x, current_piece.y, current_piece.shape))
    {
        current_piece.y--;
        if (current_piece.y < -VISIBLE_OFFSET)
        {
            gameState = GAME_OVER;
        if (score > *high_score)
        {
            *high_score = score;
            save_high_score(*high_score);
        }
            return;
        }
    }
}

void restart_game(char board[Height + VISIBLE_OFFSET][Width])
{
    create_board(board);
    reset_score_and_level();

    next_piece = generate_random_piece();
    next_next_piece = generate_random_piece();

    current_piece = next_piece;
    next_piece = next_next_piece;
    next_next_piece = generate_random_piece();

    while (check_collision(board, current_piece.x, current_piece.y, current_piece.shape))
    {
        current_piece.y--;
        if (current_piece.y < -VISIBLE_OFFSET)
        {
            gameState = GAME_OVER;
            return;
        }
    }

    gameState = GAME;
}

void update_game(char board[Height + VISIBLE_OFFSET][Width], SDL_Event *e, int *high_score)
{
    if (e && e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
            case SDLK_SPACE:
            {
                char backup[4][4];
                for (int i = 0; i < 4; i++)
                    for (int j = 0; j < 4; j++)
                        backup[i][j] = current_piece.shape[i][j];
                int old_bomb_x = current_piece.bomb_x;
                int old_bomb_y = current_piece.bomb_y;

                rotate_piece(current_piece.shape, &current_piece.bomb_x, &current_piece.bomb_y);

                if (check_collision(board, current_piece.x, current_piece.y, current_piece.shape))
                {
                    for (int i = 0; i < 4; i++)
                        for (int j = 0; j < 4; j++)
                            current_piece.shape[i][j] = backup[i][j];

                    current_piece.bomb_x = old_bomb_x;
                    current_piece.bomb_y = old_bomb_y;
                }
                break;
            }
            case SDLK_LEFT:
                if (!check_collision(board, current_piece.x - 1, current_piece.y, current_piece.shape))
                    current_piece.x--;
                break;
            case SDLK_RIGHT:
                if (!check_collision(board, current_piece.x + 1, current_piece.y, current_piece.shape))
                    current_piece.x++;
                break;
            case SDLK_DOWN:
                if (!check_collision(board, current_piece.x, current_piece.y + 1, current_piece.shape))
                    current_piece.y++;
                break;
        }
    }

    Uint32 currentTime = SDL_GetTicks();

    int currentLevel = get_level();
    int interval = 800 / (1 + currentLevel);
    if (interval < 100) interval = 100;

if (currentTime - lastFall > interval)
{
    if (check_collision(board, current_piece.x, current_piece.y + 1, current_piece.shape))
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (current_piece.shape[i][j] != ' ')
                {
                    int y = current_piece.y + i;
                    int x = current_piece.x + j;
                    if (x >= 0 && x < Width && y + VISIBLE_OFFSET >= 0 && y + VISIBLE_OFFSET < Height + VISIBLE_OFFSET)
                    {
                        if (current_piece.has_bomb && i == current_piece.bomb_y && j == current_piece.bomb_x)
                        {
                            board[y + VISIBLE_OFFSET][x] = 'B'; // marcăm bomba pe board
                        }
                        else
                        {
                            board[y + VISIBLE_OFFSET][x] = '#';
                        }
                    }
                }
            }
        }
            clear_full_lines(board);
            spawn_random_piece(board, high_score);
        }
        else
        {
            current_piece.y++;
        }
        lastFall = currentTime;
    }
}

