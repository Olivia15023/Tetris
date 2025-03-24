#ifndef GAME_H
#define GAME_H

#define Inaltime 20
#define Latime 10

void creeare_tabla(char tabla[Inaltime][Latime]);
void desenare_tabla(char tabla[Inaltime][Latime]);
void desenare_piesa(char piesa[4][4], int x, int y);

extern char piesa_L[4][4]; //pot fi folosite si in alte fisiere ce contin game.h
extern char piesa_L_invers[4][4];
extern char piesa_Z[4][4];
extern char piesa_Z_invers[4][4];
extern char piesa_O[4][4];
extern char piesa_I[4][4];
char piesa_T[4][4];

#endif