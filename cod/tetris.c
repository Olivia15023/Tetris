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
    if (TTF_Init() == -1) {
        fprintf(stderr, "Eroare la initializarea SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }
    TTF_Font *font = TTF_OpenFont("arial.ttf", 32);
    if (!font) {
        fprintf(stderr, "Eroare la încărcarea fontului: %s\n", TTF_GetError());
        return 1;
    }
    srand(time(NULL)); // pentru piese random
    creeare_tabla(tabla);
    
    GameState gameState = MENU;
    int quit = 0;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;

            if (gameState == MENU) {
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int x = e.button.x;
                    int y = e.button.y;
                    // Verificăm dacă s-a dat click pe butonul "Start Game"
                    if (x >= SCREEN_WIDTH / 2 - 100 && x <= SCREEN_WIDTH / 2 + 100 &&
                        y >= SCREEN_HEIGHT / 2 - 40 && y <= SCREEN_HEIGHT / 2 + 40) {
                        gameState = GAME;
                        spawn_random_piece(tabla);
                    }
                }
            } else if (gameState == GAME) {
                // Dacă suntem în joc, tratăm inputul 
                update_game(tabla, &e);
            }
        }
        
        // Desenare în funcție de starea curentă
        if (gameState == MENU) {
            draw_menu(font);
        } else if (gameState == GAME) {
            update_game(tabla, NULL);
            draw_board(tabla);
            draw_piece(current_piece.shape, current_piece.x, current_piece.y);
            render_frame();
        }

        SDL_Delay(100);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    close_screen();
    return 0;
}