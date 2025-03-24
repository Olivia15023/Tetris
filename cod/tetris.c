#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<ncurses.h>
#include"game.h"

#define Inaltime 20
#define Latime 10

int main()
{
    char tabla[Inaltime][Latime];

    initscr(); //Porneste ncurses
    noecho();//nu arata tastele apasate

    creeare_tabla(tabla);
    desenare_tabla(tabla);

    getch();//Apasarea unei taste pentru inchidere
    endwin();//Inchidere ncurses

    return 0;
}