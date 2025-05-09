#ifndef SCREEN_H
#define SCREEN_H

#include "game.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define SIDE_PANEL_WIDTH 6 
#define RIGHT_PANEL_WIDTH 6

#define SCREEN_WIDTH ((Width + SIDE_PANEL_WIDTH + RIGHT_PANEL_WIDTH)* BLOCK_SIZE)
#define SCREEN_HEIGHT (Height * BLOCK_SIZE)

extern TTF_Font *font;

int init_screen();
void draw_block(int x, int y, SDL_Color color,bool is_bomb);
void draw_board(char board[Height + VISIBLE_OFFSET][Width]);  
Color get_piece_color(PieceType type);
void draw_piece(char piece[4][4], int x, int y, PieceType type, bool has_bomb, int bomb_x, int bomb_y);
void render_frame();
void close_screen();
void draw_menu(void);
void draw_game_over(void);
void draw_score_and_level(void);
void draw_text(const char *text, int x, int y);
void draw_piece_preview(char shape[4][4], int x, int y, int box_size, PieceType type, bool has_bomb, int bomb_x, int bomb_y);
void draw_next_pieces(void);
void draw_high_score(int high_score, TTF_Font *font, int x, int y);
void draw_new_high_score(TTF_Font *font, int x, int y);
#endif