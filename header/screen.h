#ifndef SCREEN_H
#define SCREEN_H

#include "game.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH (Width * BLOCK_SIZE)
#define SCREEN_HEIGHT (Height * BLOCK_SIZE)

int init_screen();
void draw_block(int x, int y, SDL_Color color);
void draw_board(char board[Height + VISIBLE_OFFSET][Width]);  // <- modificat aici!
void draw_piece(char piece[4][4], int x, int y);
void render_frame();
void close_screen();
void draw_menu(TTF_Font *font);
void draw_game_over(TTF_Font *font);

#endif