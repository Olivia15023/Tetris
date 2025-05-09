#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "screen.h"
#include "config.h"
#include <stdbool.h>

#define NUM_PIECES 7


typedef struct
{
    char shape[4][4];
    int x, y; // x = column, y = row
    PieceType type;
    bool has_bomb;
    int bomb_x, bomb_y;
} Piece;

extern GameState gameState;
extern Piece current_piece;
extern Piece next_piece;
extern Piece next_next_piece;
extern char pieces[NUM_PIECES][4][4];
typedef struct { 
    int x, y; 
} Point;


void rotate_piece(char shape[4][4], int *bomb_x, int *bomb_y);
void create_board(char board[Height + VISIBLE_OFFSET][Width]);
void update_score_and_level(int lines_cleared);
int get_score(void);
int get_level(void);
void reset_score_and_level(void);
void explode_around(char board[Height + VISIBLE_OFFSET][Width], int cx, int cy);
void clear_full_lines(char board[Height + VISIBLE_OFFSET][Width]);
Piece generate_random_piece(void);
int load_high_score();
void save_high_score(int);
void spawn_random_piece(char board[Height + VISIBLE_OFFSET][Width],int *high_score);
int check_collision(char board[Height + VISIBLE_OFFSET][Width], int newX, int newY, char shape[4][4]);
void restart_game(char board[Height + VISIBLE_OFFSET][Width]);
void update_game(char board[Height + VISIBLE_OFFSET][Width], SDL_Event *e, int *high_score);


#endif