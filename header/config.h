#ifndef CONFIG_H
#define CONFIG_H


#define Width 10
#define Height 20
#define VISIBLE_OFFSET 3


typedef enum {
    PIECE_L,
    PIECE_J,
    PIECE_Z,
    PIECE_S,
    PIECE_O,
    PIECE_I,
    PIECE_T,
    NUM_PIECE_TYPES
} PieceType;

typedef struct {
    Uint8 r, g, b;
} Color;

typedef enum
{
    MENU,
    GAME,
    GAME_OVER
} GameState;

#define BLOCK_SIZE 30
#endif