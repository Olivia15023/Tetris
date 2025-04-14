#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"
#include "screen.h"

GameState gameState = MENU;

int main()
{
    // Board extins cu VISIBLE_OFFSET
    char board[Height + VISIBLE_OFFSET][Width];

    if (!init_screen())
    {
        fprintf(stderr, "Error initializing SDL!\n");
        return 1;
    }

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("arial.ttf", 32);
    if (!font)
    {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        return 1;
    }

    srand((unsigned int)time(NULL));
    create_board(board);

    int quit = 0;
    SDL_Event e;

    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }

            if (gameState == MENU)
            {
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x = e.button.x;
                    int y = e.button.y;

                    if (x >= SCREEN_WIDTH / 2 - 100 && x <= SCREEN_WIDTH / 2 + 100 &&
                        y >= SCREEN_HEIGHT / 2 - 40 && y <= SCREEN_HEIGHT / 2 + 40)
                    {
                        restart_game(board);
                    }
                }
            }
            else if (gameState == GAME)
            {
                update_game(board, &e);
            }
            else if (gameState == GAME_OVER)
            {
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x = e.button.x;
                    int y = e.button.y;

                    if (x >= SCREEN_WIDTH / 2 - 100 && x <= SCREEN_WIDTH / 2 + 100 &&
                        y >= SCREEN_HEIGHT / 2 && y <= SCREEN_HEIGHT / 2 + 60)
                    {
                        restart_game(board);
                    }
                }
            }
        }

        if (gameState == MENU)
        {
            draw_menu(font);
        }
        else if (gameState == GAME)
        {
            update_game(board, NULL);
            draw_board(board);
            draw_piece(current_piece.shape, current_piece.x, current_piece.y);
            render_frame();
        }
        else if (gameState == GAME_OVER)
        {
            draw_board(board);
            draw_piece(current_piece.shape, current_piece.x, current_piece.y);
            draw_game_over(font);
            render_frame();
        }

        SDL_Delay(100);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    close_screen();
    return 0;
}