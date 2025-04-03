#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>
#include "game.h"

#define BLOCK_SIZE 30
#define SCREEN_WIDTH (Latime * BLOCK_SIZE)
#define SCREEN_HEIGHT (Inaltime * BLOCK_SIZE)

int init_screen();
void draw_block(int x, int y, SDL_Color color);
void draw_board(char tabla[Inaltime][Latime]);
void draw_piece(char piesa[4][4], int x, int y);
void render_frame();
void close_screen();

#endif