#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>
#include <SDL2/SDL.h>
#include"game.h"
#include "screen.h"

#define Inaltime 20
#define Latime 10

int main()
{
    char tabla[Inaltime][Latime];

    if (!init_screen()) {
        fprintf(stderr, "Eroare la initializarea SDL!\n");
        return 1;
    }
    srand(time(NULL)); // pentru piese random
    creeare_tabla(tabla);
    spawn_random_piece(tabla);

    int quit = 0;
    SDL_Event e;
    
    while (!quit) {
        // Evenimente SDL (ieșire din joc)
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            update_game(tabla, &e); // mișcări + cădere
        }
        update_game(tabla, NULL); // doar cădere automată (fără eveniment)
        // Desenare tabla
        draw_board(tabla);

        // Desenare piesa curenta
        draw_piece(current_piece.shape, current_piece.x, current_piece.y);
        render_frame();

        SDL_Delay(100); // intarziere pentru a controla viteza jocului
    }
    close_screen();

    return 0;
}