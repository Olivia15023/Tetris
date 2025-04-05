#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>

#define NUM_PIECES 7
#define Inaltime 20
#define Latime 10

typedef struct {
    char shape[4][4];
    int x, y;//x-coloana,y-linia
} Piece;

extern Piece current_piece;

extern char pieces[NUM_PIECES][4][4];
void rotire_piesa(char shape[4][4]);
void creeare_tabla(char tabla[Inaltime][Latime]);
int check_game_over(char tabla[Inaltime][Latime]);  // Modificare aici
void clear_full_lines(char tabla[Inaltime][Latime]);
void spawn_random_piece(char tabla[Inaltime][Latime]);  // Modificare aici
int check_collision(char tabla[Inaltime][Latime], int newX, int newY, char shape[4][4]);  // Modificare aici
void update_game(char tabla[Inaltime][Latime], SDL_Event *e);
#endif