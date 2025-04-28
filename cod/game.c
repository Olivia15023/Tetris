#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<time.h>
#include"game.h"
#include"screen.h"

Piece current_piece;
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

void rotate_piece(char shape[4][4])
{
    char temp[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j][3 - i] = shape[i][j];
        }
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            shape[i][j] = temp[i][j];
        }
    }
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

void spawn_random_piece(char board[Height + VISIBLE_OFFSET][Width])
{
    int r = rand() % NUM_PIECES;
    current_piece.type = (PieceType)r;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            current_piece.shape[i][j] = pieces[r][i][j];
        }
    }

    current_piece.x = (Width / 2) - 2;
    current_piece.y = 0;

    
    while (check_collision(board, current_piece.x, current_piece.y, current_piece.shape))
    {
        current_piece.y--;

        if (current_piece.y < -VISIBLE_OFFSET)
        {
            gameState = GAME_OVER;
            return;
        }
    }
}
void restart_game(char board[Height + VISIBLE_OFFSET][Width])
{
    create_board(board);
    reset_score_and_level();
    spawn_random_piece(board);
    gameState = GAME;
}
void update_game(char board[Height + VISIBLE_OFFSET][Width], SDL_Event *e)
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

                rotate_piece(current_piece.shape);

                if (check_collision(board, current_piece.x, current_piece.y, current_piece.shape))
                {
                    for (int i = 0; i < 4; i++)
                        for (int j = 0; j < 4; j++)
                            current_piece.shape[i][j] = backup[i][j];
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
                        board[y + VISIBLE_OFFSET][x] = '#';
                    }
                }
            }
        }
            clear_full_lines(board);
            spawn_random_piece(board);
        }
        else
        {
            current_piece.y++;
        }
        lastFall = currentTime;
    }
}

