#include <SDL2/SDL.h>
#include "screen.h"

SDL_Window* window = NULL;//fereasta aplicatiei
SDL_Renderer* renderer = NULL;//context de desenare

int init_screen() {
    if (SDL_Init(SDL_INIT_VIDEO)<0) //initializeaza partea video
    {
        return 0;
    }

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    return 1;
}

void draw_block(int x, int y, SDL_Color color) 
{
    SDL_Rect block;
    block.x = x * BLOCK_SIZE;
    block.y = y * BLOCK_SIZE;
    block.w = BLOCK_SIZE;
    block.h = BLOCK_SIZE;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &block);
    
}

// Functie pentru a desena tabla de joc
void draw_board(char tabla[Inaltime][Latime]) 
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//fundal negru
    SDL_RenderClear(renderer);

    for (int i = 0; i < Inaltime; i++) 
    {
        for (int j = 0; j < Latime; j++) 
        {
            if (tabla[i][j] != ' ') 
            {
                SDL_Color alb = {255, 255, 255, 255};
                draw_block(j, i, alb);
            }
        }
    }


}  

//Functie pentru a desena piesa curenta
void draw_piece(char piesa[4][4], int x, int y) 
{
    SDL_Color rosu = {255, 0, 0, 255};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piesa[i][j] != ' ') {
                draw_block(x + j, y + i, rosu);
            }
        }
    }
    
}

// Functie pentru a afisa frame-ul
void render_frame() 
{
    SDL_RenderPresent(renderer);
}

void close_screen() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}