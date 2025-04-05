#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
// Functie pentru a desena meniul
void draw_menu(TTF_Font *font) {
    // Setează culoarea de fundal a ferestrei (gri închis)
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Definim un dreptunghi pentru butonul "Start Game"
    SDL_Rect button = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 200, 80 };
    // Setăm culoarea butonului (albastru)
    SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
    SDL_RenderFillRect(renderer, &button);

    SDL_Color white = {255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Start Game", white);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Calculăm un dreptunghi pentru poziționarea textului în interiorul butonului (centrare)
    SDL_Rect textRect = {
        button.x + (button.w - textSurface->w) / 2,
        button.y + (button.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, text, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(text);
    SDL_RenderPresent(renderer);
}
