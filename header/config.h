#ifndef CONFIG_H
#define CONFIG_H

#include <SDL2/SDL.h> // Necesara pentru Uint8

#define Width 10
#define Height 20
#define VISIBLE_OFFSET 3 // Câte rânduri deasupra zonei vizibile sunt folosite pentru piesele noi

// Constante pentru duratele animațiilor
#define EXPLOSION_TOTAL_DURATION 300 // Durata totală a animației de explozie în ms
#define FLASH_LINE_TOTAL_DURATION 200 // Durata totală a animației de flash de linie în ms

// Constante pentru dimensiuni ecran și panouri laterale
#define BLOCK_SIZE 30
#define SIDE_PANEL_WIDTH 6   // Numărul de blocuri pentru panoul din stânga (scor, nivel)
#define RIGHT_PANEL_WIDTH 6  // Numărul de blocuri pentru panoul din dreapta (next pieces)

// Dimensiunile ecranului bazate pe constantele de mai sus
#define SCREEN_WIDTH ((Width + SIDE_PANEL_WIDTH + RIGHT_PANEL_WIDTH) * BLOCK_SIZE)
#define SCREEN_HEIGHT (Height * BLOCK_SIZE)

// Definiții legate de joc
#define NUM_PIECES 7 // Numărul total de tipuri de piese
#define MAX_ACTIVE_EXPLOSIONS 5 // Numărul maxim de explozii active simultan
#define MAX_ACTIVE_LINE_FLASHES 4 // Numărul maxim de flash-uri de linii simultan


// Enumerare pentru tipurile de piese Tetris
typedef enum {
    PIECE_L,
    PIECE_J,
    PIECE_Z,
    PIECE_S,
    PIECE_O,
    PIECE_I,
    PIECE_T,
    NUM_PIECE_TYPES // Ajutor pentru a ști câte tipuri de piese sunt
} PieceType;

// Structură pentru a stoca o culoare RGB (cu componenta alpha în SDL_Color)
typedef struct {
    Uint8 r, g, b;
} Color;

// Enumerare pentru starea curentă a jocului
typedef enum
{
    MENU,
    GAME,
    GAME_OVER
} GameState;

#endif // CONFIG_H