#ifndef SCREEN_H
#define SCREEN_H

#include <SDL2/SDL.h>   // Necesara pentru SDL_Renderer, SDL_Color, etc.
#include <SDL2/SDL_ttf.h> // Necesara pentru TTF_Font
#include "config.h"     // Include constante globale și structuri
#include "game.h"       // Include structura Piece și tipuri de piese

// Declarații `extern` pentru variabilele globale din screen.c sau tetris.c
extern SDL_Renderer* renderer; // Renderer-ul SDL
extern TTF_Font *font;         // Fontul folosit pentru text
extern Color color_board[Height + VISIBLE_OFFSET][Width]; // Tabla de culori a blocurilor

// Funcții de inițializare și închidere ecran
int init_screen();
void close_screen();
SDL_Renderer* get_renderer(void); // Utilitate, dacă e nevoie să accesezi renderer-ul din alte module

// Funcții de desenare a elementelor de joc
void draw_block(int x, int y, SDL_Color color, bool is_bomb);
void draw_board(char board[Height + VISIBLE_OFFSET][Width], Color color_board[Height + VISIBLE_OFFSET][Width]);
void draw_piece(char piece[4][4], int x, int y, PieceType type, bool has_bomb, int bomb_x, int bomb_y);
void render_frame(); // Prezintă conținutul renderer-ului pe ecran

// Funcții pentru interfața utilizatorului (UI) și texte
Color get_piece_color(PieceType type); // Returnează culoarea specifică unui tip de piesă
void draw_centered_text(const char *text, int center_x, int y, SDL_Color color, Uint8 alpha);
void draw_menu(int high_score);
void draw_game_over(int high_score);
void draw_score_and_level(void);
void draw_text(const char *text, int x, int y);
void draw_piece_preview(char shape[4][4], int x, int y, int box_size, PieceType type, bool has_bomb, int bomb_x, int bomb_y);
void draw_next_pieces(void);
void draw_high_score(int high_score, TTF_Font *font, int x, int y);
void draw_new_high_score(TTF_Font *font, int x, int y);

// Funcția principală de desenare a întregului frame de joc
void draw_full_game_frame(char board[Height + VISIBLE_OFFSET][Width], Color color_board[Height + VISIBLE_OFFSET][Width], int high_score);

// Funcții pentru efecte vizuale (animații)
void draw_explosion_effect(int ex, int ey, Uint32 elapsed_time);
void draw_smoke_overlay(int cx, int cy, int alpha);
void draw_line_flash_effect(int line_y, Uint32 elapsed_time);

#endif // SCREEN_H