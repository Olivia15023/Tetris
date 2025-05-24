#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h> // Necesara pentru Uint32 si SDL_Event
#include <SDL2/SDL_mixer.h>
#include "config.h"   // Include constante precum Width, Height, VISIBLE_OFFSET, MAX_ACTIVE_EXPLOSIONS, etc.
#include <stdbool.h>  // Necesara pentru tipul bool

// Structură pentru a ține evidența unei explozii active
typedef struct {
    int x, y; // Coordonatele centrului exploziei (coloană, rând, în sistemul de coordonate al tablei vizibile)
    Uint32 startTime; // Timpul la care a început explozia (milisecunde)
    bool active; // Este explozia încă activă?
} ActiveExplosion;

// Structură pentru a ține evidența unei animații de flash de linie
typedef struct {
    int line_y; // Rândul pe care se face flash-ul (în sistemul de coordonate al tablei vizibile)
    Uint32 startTime; // Timpul la care a început flash-ul (milisecunde)
    bool active; // Este flash-ul încă activ?
} ActiveLineFlash;

// Structură pentru o piesă de Tetris
typedef struct
{
    char shape[4][4]; // Matricea de 4x4 care definește forma piesei
    int x, y; // x = coloană, y = rând (poziția colțului superior-stânga al matricei de 4x4 pe tablă)
    PieceType type; // Tipul piesei (L, J, Z, etc.)
    bool has_bomb; // Indica dacă piesa conține o bombă
    int bomb_x, bomb_y; // Coordonatele bombei în cadrul matricei de 4x4 a piesei (-1 dacă nu are bombă)
} Piece;

// Declarații `extern` pentru variabilele globale
// Aceste variabile sunt definite o singură dată în `game.c` sau `tetris.c`
extern char board[Height + VISIBLE_OFFSET][Width]; // Tabla de joc
extern Color color_board[Height + VISIBLE_OFFSET][Width]; // Tabla de culori a blocurilor

extern GameState gameState; // Starea curentă a jocului (MENU, GAME, GAME_OVER)
extern Piece current_piece; // Piesa curentă pe care o controlează jucătorul
extern Piece next_piece;    // Piesa următoare (preview)
extern Piece next_next_piece; // Piesa de după următoarea (preview)

extern char pieces[NUM_PIECES][4][4]; // Formele predefinite ale pieselor

extern ActiveExplosion active_explosions[MAX_ACTIVE_EXPLOSIONS]; // Array de explozii active
// Nu mai e nevoie de num_active_explosions, deoarece iterăm pe întregul array și verificăm 'active'
extern ActiveLineFlash active_line_flashes[MAX_ACTIVE_LINE_FLASHES]; // Array de flash-uri de linii active
// Nu mai e nevoie de num_active_line_flashes
extern Mix_Chunk *line_clear_sound;
extern Mix_Chunk *explosion_sound;

// Declarații de funcții (prototipuri)

// Funcții legate de piese
void rotate_piece(char shape[4][4], int *bomb_x, int *bomb_y);
Piece generate_random_piece(void);
void spawn_random_piece(char board[Height + VISIBLE_OFFSET][Width], int *high_score);
int check_collision(char board[Height + VISIBLE_OFFSET][Width], int newX, int newY, char shape[4][4]);

// Funcții legate de tablă și joc
void create_board(char board[Height + VISIBLE_OFFSET][Width]);
void apply_line_gravity(char board[Height + VISIBLE_OFFSET][Width], Color color_board[Height + VISIBLE_OFFSET][Width], bool deleted_lines[Height]);
void clear_full_lines(char board[Height + VISIBLE_OFFSET][Width]);

// Funcții pentru scor și nivel
void update_score_and_level(int lines_cleared);
int get_score(void);
int get_level(void);
void reset_score_and_level(void);

// Funcții pentru salvare/încărcare scor
int load_high_score();
void save_high_score(int score);

// Funcții pentru starea jocului
void restart_game(char board[Height + VISIBLE_OFFSET][Width]);
void update_game(char board[Height + VISIBLE_OFFSET][Width], SDL_Event *e, int *high_score);

// Funcții pentru efecte vizuale (legate de logică)
void trigger_explosion(int x, int y);
void apply_explosion_effect_to_board(int center_x, int center_y);
void trigger_line_flash(int line_y);

#endif // GAME_H