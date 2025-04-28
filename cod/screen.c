#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "screen.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

TTF_Font *font=NULL;

int init_screen()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 0;
    }
    if (TTF_Init() == -1)
    {
        fprintf(stderr, "Failed to initialize SDL_ttf: %s\n", TTF_GetError());
        return 0;
    }

    font = TTF_OpenFont("arial.ttf", 32);  
    if (!font)
    {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
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
    block.x = (x+ SIDE_PANEL_WIDTH) * BLOCK_SIZE;
    block.y = y * BLOCK_SIZE;
    block.w = BLOCK_SIZE;
    block.h = BLOCK_SIZE;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 180);  
    SDL_RenderFillRect(renderer, &block);

    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);
    SDL_RenderDrawRect(renderer, &block);

   
    SDL_Rect highlight = {
        block.x + 3,
        block.y + 3,
        BLOCK_SIZE / 2,
        BLOCK_SIZE / 6
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100); 
    SDL_RenderFillRect(renderer, &highlight);
}

void draw_board(char board[Height + VISIBLE_OFFSET][Width])
{
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255); 
    SDL_RenderClear(renderer);

    SDL_Rect board_area = {
        SIDE_PANEL_WIDTH * BLOCK_SIZE,
        0,
        Width * BLOCK_SIZE,
        Height * BLOCK_SIZE
    };
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); 
    SDL_RenderFillRect(renderer, &board_area);

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); 
    SDL_RenderDrawRect(renderer, &board_area);

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
    


Color get_piece_color(PieceType type)
{
    switch(type)
    {
        case PIECE_L: return (Color){255, 0, 0};       // roșu
        case PIECE_J: return (Color){0, 0, 255};       // albastru
        case PIECE_Z: return (Color){0, 255, 0};       // verde
        case PIECE_S: return (Color){255, 255, 0};     // galben
        case PIECE_O: return (Color){255, 165, 0};     // portocaliu
        case PIECE_I: return (Color){255, 105, 180};   // roz
        case PIECE_T: return (Color){128, 0, 128};     // mov
        default:      return (Color){255, 255, 255};   // fallback alb
    }
}
void draw_piece(char piece[4][4], int x, int y,PieceType type)
{
    Color c = get_piece_color(type);
    SDL_Color color = {c.r, c.g, c.b, 255};

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (piece[i][j] != ' ')
            {
                draw_block(x + j, y + i, color);
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
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_menu(void)
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

void draw_game_over(void)
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
        SCREEN_HEIGHT / 2 - 120, 
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

void draw_text(const char *text, int x, int y)
{
    if(!font)
    return;

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_score_and_level(void)
{
    char buffer[64];

    snprintf(buffer, sizeof(buffer), "Score: %d", get_score());
    draw_text(buffer, 20, SCREEN_HEIGHT - 80); 

    snprintf(buffer, sizeof(buffer), "Level: %d", get_level());
    draw_text(buffer, 20, SCREEN_HEIGHT - 40); 
}