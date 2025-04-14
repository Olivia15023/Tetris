#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "screen.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int init_screen()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 0;
    }

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
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


void draw_board(char board[Height + VISIBLE_OFFSET][Width])
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fundal negru
    SDL_RenderClear(renderer);

    for (int i = -VISIBLE_OFFSET; i < Height; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            int bi = i + VISIBLE_OFFSET;
            if (bi >= 0 && bi < Height + VISIBLE_OFFSET && board[bi][j] != ' ')
            {
                SDL_Color white = {255, 255, 255, 255};
                draw_block(j, i, white);
            }
        }
    }
}
void draw_piece(char piece[4][4], int x, int y)
{
    SDL_Color red = {255, 0, 0, 255};

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (piece[i][j] != ' ')
            {
                draw_block(x + j, y + i, red);
            }
        }
    }
}

void render_frame()
{
    SDL_RenderPresent(renderer);
}

void close_screen()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_menu(TTF_Font *font)
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    SDL_Rect button = { SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 40, 200, 80 };
    SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
    SDL_RenderFillRect(renderer, &button);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Start Game", white);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);

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

void draw_game_over(TTF_Font *font)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Game Over", white);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {
        (SCREEN_WIDTH - textSurface->w) / 2,
        SCREEN_HEIGHT / 2 - 120, // ridicat puțin
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, text, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(text);

    SDL_Rect button = { SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 200, 60 };
    SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
    SDL_RenderFillRect(renderer, &button);

    SDL_Surface *btnSurface = TTF_RenderText_Solid(font, "Restart", white);
    SDL_Texture *btnText = SDL_CreateTextureFromSurface(renderer, btnSurface);

    SDL_Rect btnRect = {
        button.x + (button.w - btnSurface->w) / 2,
        button.y + (button.h - btnSurface->h) / 2,
        btnSurface->w,
        btnSurface->h
    };
    SDL_RenderCopy(renderer, btnText, NULL, &btnRect);

    SDL_FreeSurface(btnSurface);
    SDL_DestroyTexture(btnText);
}