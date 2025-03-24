#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<ncurses.h>
#include"game.h"

#define Inaltime 20
#define Latime 10

char piesa_L[4][4] = {
    { ' ', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', '#', ' ', ' ' }
};

char piesa_L_invers[4][4] = {
    { ' ', ' ', ' ', ' ' },
    { ' ', '#', ' ', ' ' },
    { ' ', '#', ' ', ' ' },
    { '#', '#', ' ', ' ' }
};
char piesa_Z[4][4] = {
    { ' ', ' ', ' ', ' ' },
    { ' ', ' ', ' ', ' ' },
    { '#', '#', ' ', ' ' },
    { ' ', '#', '#', ' ' }
};
char piesa_Z_invers[4][4] = {
    { ' ', ' ', ' ', ' ' },
    { ' ', ' ', ' ', ' ' },
    { ' ', '#', '#', ' ' },
    { '#', '#', ' ', ' ' }
};
char piesa_O[4][4] = {
    { ' ', ' ', ' ', ' ' },
    { ' ', ' ', ' ', ' ' },
    { ' ', '#', '#', ' ' },
    { ' ', '#', '#', ' ' }
};
char piesa_I[4][4] = {
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' },
    { '#', ' ', ' ', ' ' }
};

char piesa_T[4][4] = {
    { ' ', ' ', ' ', ' ' },
    { ' ', ' ', ' ', ' ' },
    { ' ', '#', ' ', ' ' },
    { '#', '#', '#', ' ' }
};


void desenare_piesa(char piesa[4][4], int x, int y) {
    for (int i=0; i<4;i++) {
        for (int j=0;j<4;j++) {
            if (piesa[i][j]=='#') {  
                move(y + i,x + j);//seteaza unde va fi deseanat blocul
                addch(ACS_BLOCK);  
            }
        }
    }
    refresh();
}

void creeare_tabla(char tabla[Inaltime][Latime])
{
    for (int i=0;i<Inaltime;i++)
    {
        for(int j=0;j<Latime;j++)
        {
            tabla[i][j]=' ';

        }
        
    }
}

void desenare_tabla(char tabla[Inaltime][Latime])
{
    for(int i=0;i<Inaltime;i++)
    {
        move(i,0);
        addch('|');
        for(int j=0;j<Latime;j++)
        {
            addch(tabla[i][j]);
           
        }
    
        addch('|');
    }
    move(Inaltime,0);
    for(int j=0;j<Latime;j++)
    {
        addch('-');
    
    }
   
    refresh();//functia de actualizare a ecranului
}