#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "screen.h"
#include "config.h"

#define NUM_PIECES 7

typedef struct
{
    char shape[4][4];
    int x, y; // x = column, y = row
} Piece;

extern GameState gameState;
extern Piece current_piece;
extern char pieces[NUM_PIECES][4][4];

void rotate_piece(char shape[4][4]);
void create_board(char board[Height + VISIBLE_OFFSET][Width]);
void clear_full_lines(char board[Height + VISIBLE_OFFSET][Width]);
void spawn_random_piece(char board[Height + VISIBLE_OFFSET][Width]);
int check_collision(char board[Height + VISIBLE_OFFSET][Width], int newX, int newY, char shape[4][4]);
void restart_game(char board[Height + VISIBLE_OFFSET][Width]);
void update_game(char board[Height + VISIBLE_OFFSET][Width], SDL_Event *e);

#endif