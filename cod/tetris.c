#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h> // Pentru srand(time(NULL))
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"   // Include funcții și structuri de joc
#include "screen.h" // Include funcții și structuri de randare
#include "config.h" // Include constante globale (Width, Height, SCREEN_WIDTH, etc.)

// Variabile globale declarate ca `extern` aici, deoarece sunt definite în `game.c`
extern GameState gameState; // Starea curentă a jocului (MENU, GAME, GAME_OVER)
extern char board[Height + VISIBLE_OFFSET][Width]; // Tabla de joc
extern Color color_board[Height + VISIBLE_OFFSET][Width]; // Tabla de culori a blocurilor
extern Piece current_piece; // Piesa curentă
// Variabilele `next_piece` și `next_next_piece` sunt de asemenea `extern` și definite în `game.c`
// `font` și `renderer` sunt `extern` și definite în `screen.c`

int main(int argc, char* argv[]) // Folosim argc, argv chiar dacă nu sunt folosite, pentru conformitate cu SDL
{
    // Încarcă scorul maxim la începutul aplicației
    int high_score = load_high_score();

    // Inițializează ecranul și subsistemele SDL
    if (!init_screen())
    {
        fprintf(stderr, "Error initializing SDL!\n");
        return 1; // Ieșire cu eroare
    }

    srand((unsigned int)time(NULL)); // Inițializează generatorul de numere aleatorii
    create_board(board); // Inițializează tabla de joc cu spații goale și culori negre

    int quit = 0; // Flag pentru a controla bucla principală a jocului
    SDL_Event e;  // Variabilă pentru a stoca evenimentele SDL

    // Bucla principală a jocului
    while (!quit)
    {
        // 1. Procesare evenimente (Input)
        while (SDL_PollEvent(&e)) // Verifică dacă există evenimente în coadă
        {
            if (e.type == SDL_QUIT) // Dacă utilizatorul închide fereastra
            {
                quit = 1; // Setează flag-ul de ieșire
            }

            // Logică specifică stării jocului pentru evenimente
            if (gameState == MENU)
            {
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x = e.button.x;
                    int y = e.button.y;

                    // Verifică click pe butonul "Start Game"
                    if (x >= SCREEN_WIDTH / 2 - 100 && x <= SCREEN_WIDTH / 2 + 100 &&
                        y >= SCREEN_HEIGHT / 2 - 40 && y <= SCREEN_HEIGHT / 2 + 40)
                    {
                        restart_game(board); // Resetează și începe un joc nou
                    }
                }
            }
            else if (gameState == GAME)
            {
                update_game(board, &e, &high_score); // Trimite evenimentul de input pentru a actualiza jocul
            }
            else if (gameState == GAME_OVER)
            {
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x = e.button.x;
                    int y = e.button.y;

                    // Verifică click pe butonul "Restart"
                    if (x >= SCREEN_WIDTH / 2 - 100 && x <= SCREEN_WIDTH / 2 + 100 &&
                        y >= SCREEN_HEIGHT / 2 && y <= SCREEN_HEIGHT / 2 + 60)
                    {
                        restart_game(board); // Resetează și începe un joc nou
                    }
                }
            }
        }

        // 2. Logică de joc (Update) - Aici se întâmplă căderea automată a piesei
        if (gameState == GAME)
        {
            update_game(board, NULL, &high_score); // Aici se procesează căderea automată (fără input explicit)
        }

        // 3. Randare (Draw)
        if (gameState == MENU)
        {
            draw_menu(high_score); // Desenează ecranul meniului
            // Desenează scorul maxim sub butonul de start
            render_frame(); // Randează frame-ul complet al meniului
        }
        else if (gameState == GAME)
        {
            draw_full_game_frame(board, color_board, high_score); // Randează frame-ul complet al jocului
        }
        else if (gameState == GAME_OVER)
        {
            // Randăm board-ul și piesa o dată pentru context înainte de overlay
            draw_board(board, color_board);
            // Piesa curentă rămâne vizibilă la Game Over
            draw_piece(current_piece.shape, current_piece.x, current_piece.y, current_piece.type, current_piece.has_bomb, current_piece.bomb_x, current_piece.bomb_y);

            // Apoi desenăm overlay-ul de Game Over
            draw_game_over(high_score);
            // Desenează scorul maxim și mesajul "New High Score!" dacă e cazul
           
            
            render_frame(); // Randează frame-ul complet de Game Over
        }

        // Controlul vitezei frame-urilor.
        // SDL_Delay este o metodă simplă, dar nu garantează un framerate consistent pe sisteme diferite.
        // Pentru un joc simplu, este acceptabil. Viteza de cădere a pieselor este controlată de `fallInterval` în `game.c`.
        SDL_Delay(10); // Reducem delay-ul pentru a permite un framerate mai bun (aproximativ 100 FPS)
    }

    close_screen(); // Eliberează resursele SDL la ieșirea din buclă
    return 0; // Ieșire normală
}