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
    int high_score = load_high_score();
    // Board extins cu VISIBLE_OFFSET
    char board[Height + VISIBLE_OFFSET][Width];

    if (!init_screen())
    {
        fprintf(stderr, "Error initializing SDL!\n");
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
                update_game(board, &e,&high_score);
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
            draw_menu();
            draw_high_score(high_score, font, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60);
            render_frame();
        }
        else if (gameState == GAME)
        {
            update_game(board, NULL,&high_score);
            draw_board(board);
            draw_piece(current_piece.shape, current_piece.x, current_piece.y,current_piece.type, current_piece.has_bomb,current_piece.bomb_x, current_piece.bomb_y);
            draw_score_and_level();
            int x=15;
            int y = 30;
            draw_text("High Score:", x, y);
            char score_text[32];
            sprintf(score_text, "%d", high_score);
            draw_text(score_text, x + 40, y + 40); 
            draw_next_pieces();
            render_frame();
        }
        else if (gameState == GAME_OVER)
        {
            draw_board(board);
            draw_piece(current_piece.shape, current_piece.x, current_piece.y,current_piece.type, current_piece.has_bomb, current_piece.bomb_x, current_piece.bomb_y);
            draw_game_over();
            draw_high_score(high_score, font, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 80);
            if (get_score() == high_score)
            {
                draw_new_high_score(font, SCREEN_WIDTH / 2 - 130, SCREEN_HEIGHT / 2 + 120);
            }
            render_frame();
        }

        SDL_Delay(100);
    }

    
    close_screen();
    return 0;
}