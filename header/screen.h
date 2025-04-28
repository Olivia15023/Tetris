#ifndef SCREEN_H
#define SCREEN_H

#include "game.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SIDE_PANEL_WIDTH 6 
#define RIGHT_PANEL_WIDTH 6

#define SCREEN_WIDTH ((Width + SIDE_PANEL_WIDTH + RIGHT_PANEL_WIDTH)* BLOCK_SIZE)
#define SCREEN_HEIGHT (Height * BLOCK_SIZE)



int init_screen();
void draw_block(int x, int y, SDL_Color color);
void draw_board(char board[Height + VISIBLE_OFFSET][Width]);  
Color get_piece_color(PieceType type);
void draw_piece(char piece[4][4], int x, int y, PieceType type);
void render_frame();
void close_screen();
void draw_menu(void);
void draw_game_over(void);
void draw_score_and_level(void);

#endif