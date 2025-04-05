#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"game.h"

Piece current_piece;

static Uint32 lastFall = 0;
const Uint32 fallInterval = 500;

char pieces[NUM_PIECES][4][4] = {
    { // L
        { ' ', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // L invers
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // Z
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { '#', '#', ' ', ' ' },
        { ' ', '#', '#', ' ' }
    },
    { // Z invers
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', '#', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // O
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', '#', ' ' },
        { ' ', '#', '#', ' ' }
    },
    { // I
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' }
    },
    { // T
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { '#', '#', '#', ' ' }
    }
};
// Functie pentru a roti piesa
void rotire_piesa(char shape[4][4]) {
    char temp[4][4];

    // Rotim matricea în sensul acelor de ceasornic
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j][3 - i] = shape[i][j];
        }
    }

    // Copiem rezultatul înapoi în piesă
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            shape[i][j] = temp[i][j];
        }
    }
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

int check_game_over(char tabla[Inaltime][Latime]) {
    // Verificăm primele linii pentru a vedea dacă sunt deja ocupate
    for (int j = 0; j < Latime; j++) {
        if (tabla[0][j] != ' ') {  // Verificăm prima linie
            return 1;  // Jocul s-a terminat
        }
    }
    return 0;  // Jocul continuă
}
//Functie pentru stergerea liniei
void clear_full_lines(char tabla[Inaltime][Latime]) {
    for (int i = Inaltime - 1; i >= 0; i--) {
        int linie_plina = 1;
        for (int j = 0; j < Latime; j++) {
            if (tabla[i][j] == ' ') {
                linie_plina = 0;
                break;
            }
        }

        if (linie_plina) {
            // Mutăm liniile de deasupra în jos
            for (int k = i; k > 0; k--) {
                for (int j = 0; j < Latime; j++) {
                    tabla[k][j] = tabla[k - 1][j];
                }
            }

            // Golește prima linie
            for (int j = 0; j < Latime; j++) {
                tabla[0][j] = ' ';
            }

            // Verificăm aceeași linie din nou (deoarece o linie a coborât aici)
            i++;
        }
    }
}

void spawn_random_piece(char tabla[Inaltime][Latime]) {
    // Dacă tabla este plină în partea de sus, înseamnă că jocul s-a terminat
    if (check_game_over(tabla)) {
        printf("Game Over!\n");
        exit(0); // Termină jocul (sau poți opri aici dacă vrei să faci altceva)
    }

    int r = rand() % NUM_PIECES;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            current_piece.shape[i][j] = pieces[r][i][j];

    current_piece.x = (Latime / 2) - 2;
    current_piece.y = 0;
}

int check_collision(char tabla[Inaltime][Latime], int newX, int newY, char shape[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape[i][j] != ' ') {
                int y = newY + i;
                int x = newX + j;

                // Verifică dacă coordonatele sunt în afara tablei sau se suprapun cu o piesă existentă
                if (y >= Inaltime || x < 0 || x >= Latime || tabla[y][x] != ' ') {
                    return 1; // Coliziune
                }
            }
        }
    }
    return 0; // Nu există coliziune
}

void update_game(char tabla[Inaltime][Latime], SDL_Event *e) {
    if (e && e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_SPACE:
            {
                char backup[4][4];
    
                // Salvăm forma curentă (în caz că rotația e invalidă)
                for (int i = 0; i < 4; i++)
                    for (int j = 0; j < 4; j++)
                        backup[i][j] = current_piece.shape[i][j];
    
                // Rotește piesa
                rotire_piesa(current_piece.shape);
    
                // Dacă piesa rotită intră în coliziune, revenim la forma inițială
                if (check_collision(tabla, current_piece.x, current_piece.y, current_piece.shape)) {
                    for (int i = 0; i < 4; i++)
                        for (int j = 0; j < 4; j++)
                            current_piece.shape[i][j] = backup[i][j];
                }
    
                break;
            }
            case SDLK_LEFT:
                // Verifică coliziunea la stânga
                if (!check_collision(tabla, current_piece.x - 1, current_piece.y, current_piece.shape)) {
                    current_piece.x--;  // Mută piesa la stânga
                }
                break;

            case SDLK_RIGHT:
                // Verifică coliziunea la dreapta
                if (!check_collision(tabla, current_piece.x + 1, current_piece.y, current_piece.shape)) {
                    current_piece.x++;  // Mută piesa la dreapta
                }
                break;

            case SDLK_DOWN:
                // Verifică dacă piesa poate cădea mai jos
                if (!check_collision(tabla, current_piece.x, current_piece.y + 1, current_piece.shape)) {
                    current_piece.y++;  // Mută piesa la jos
                }
                break;
        }
    }

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastFall > fallInterval) {
        // Verifică coliziunea cu tabla înainte de a permite piesei să coboare
        if (check_collision(tabla, current_piece.x, current_piece.y + 1, current_piece.shape)) {
            // Lipire piesă pe tablă
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (current_piece.shape[i][j] != ' ') {
                        int y = current_piece.y + i;
                        int x = current_piece.x + j;
                        if (y >= 0 && y < Inaltime && x >= 0 && x < Latime) {
                            tabla[y][x] = '#'; // Plasează piesa pe tablă
                        }
                    }
                }
            }
            clear_full_lines(tabla); // Verifică și șterge liniile complete
            spawn_random_piece(tabla); // Piesă nouă
        } else {
            current_piece.y++;  // Continuă căderea piesei
        }

        lastFall = currentTime;
    }
}