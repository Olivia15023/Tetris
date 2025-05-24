#include <stdio.h>
#include <stdlib.h> // Pentru rand() în draw_explosion_effect
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>
#include "screen.h"
#include "game.h"   // Pentru Piece, ActiveExplosion, ActiveLineFlash, etc.
#include "config.h" // Pentru toate macro-urile de dimensiuni și durate

// Variabile globale pentru SDL (fereastră, renderer, font)
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font *font = NULL;
Mix_Chunk *line_clear_sound = NULL;
Mix_Chunk *explosion_sound = NULL;

// Declaratii extern pentru variabilele globale definite in game.c
extern ActiveExplosion active_explosions[MAX_ACTIVE_EXPLOSIONS];
extern ActiveLineFlash active_line_flashes[MAX_ACTIVE_LINE_FLASHES];
extern char board[Height + VISIBLE_OFFSET][Width];
extern Color color_board[Height + VISIBLE_OFFSET][Width];
extern Piece current_piece;
extern Piece next_piece;
extern Piece next_next_piece;
extern int high_score;

/**
 * @brief Returnează renderer-ul SDL curent.
 * @return Pointer la SDL_Renderer.
 */
SDL_Renderer* get_renderer(void)
{
    return renderer;
}

/**
 * @brief Inițializează subsistemele SDL și fereastra/renderer-ul.
 * @return 1 pe succes, 0 pe eșec.
 */
int init_screen()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() == -1)
    {
        fprintf(stderr, "Failed to initialize SDL_ttf: %s\n", TTF_GetError());
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
    {
        fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
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
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    // Încarcă sunetele (presupune că ai declarat variabilele globale în screen.c)
    line_clear_sound = Mix_LoadWAV("sounds/line_clear.wav");
    explosion_sound = Mix_LoadWAV("sounds/explosion.wav");

    if (!line_clear_sound || !explosion_sound)
    {
        fprintf(stderr, "Failed to load sound effects! %s\n", Mix_GetError());
        return 0;
    }

    return 1;
}

/**
 * @brief Desenează un singur bloc pe ecran.
 * @param x Coloana blocului (în sistemul de coordonate al tablei vizibile).
 * @param y Rândul blocului (în sistemul de coordonate al tablei vizibile).
 * @param color Culoarea SDL a blocului, inclusiv componenta alpha.
 * @param is_bomb True dacă blocul este o bombă, false altfel.
 */
void draw_block(int x, int y, SDL_Color color, bool is_bomb)
{
    SDL_Rect block;
    block.x = (x + SIDE_PANEL_WIDTH) * BLOCK_SIZE; // Ajustează pentru panoul lateral stânga
    block.y = y * BLOCK_SIZE; // Coordonata Y directă
    block.w = BLOCK_SIZE;
    block.h = BLOCK_SIZE;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Permite transparența

    // Desenează umplerea blocului cu culoarea și opacitatea specificate
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &block);

    // Desenează bordura blocului (albă, semi-transparentă, adaptată la opacitatea blocului)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, (Uint8)(color.a * 0.3)); // 30% din opacitatea blocului
    SDL_RenderDrawRect(renderer, &block);

    // Desenează highlight-ul (lumină reflectată), alb, semi-transparent
    SDL_Rect highlight = {
        block.x + 3,
        block.y + 3,
        BLOCK_SIZE / 2,
        BLOCK_SIZE / 6
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, (Uint8)(color.a * 0.4)); // 40% din opacitatea blocului
    SDL_RenderFillRect(renderer, &highlight);

    // Desenează detaliile bombei dacă este cazul
    if (is_bomb)
    {
        // Corpul bombei (cerc negru)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Complet opac
        SDL_Rect bomb_circle = {
            block.x + BLOCK_SIZE / 4,
            block.y + BLOCK_SIZE / 4,
            BLOCK_SIZE / 2,
            BLOCK_SIZE / 2
        };
        SDL_RenderFillRect(renderer, &bomb_circle);

        // Fitilul bombei (dreptunghi roșu)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Complet opac
        SDL_Rect wick = {
            block.x + BLOCK_SIZE / 2 - 2, // Centrat pe X
            block.y + 2,                  // Aproape de partea de sus
            4,                            // Lățime
            4                             // Înălțime
        };
        SDL_RenderFillRect(renderer, &wick);
    }
}

/**
 * @brief Desenează întreaga tablă de joc.
 * @param board Tabla de joc (caractere).
 * @param color_board Tabla de culori.
 */
void draw_board(char board[Height + VISIBLE_OFFSET][Width], Color color_board[Height + VISIBLE_OFFSET][Width])
{
    // Umple întregul ecran cu o culoare de fundal închisă
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    // Desenează zona principală a tablei de joc (dreptunghi central)
    SDL_Rect board_area = {
        SIDE_PANEL_WIDTH * BLOCK_SIZE, // Începe după panoul lateral stânga
        0,                             // Începe de sus
        Width * BLOCK_SIZE,            // Lățimea tablei vizibile
        Height * BLOCK_SIZE            // Înălțimea tablei vizibile
    };
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Culoare fundal pentru zona de joc
    SDL_RenderFillRect(renderer, &board_area);

    // Desenează conturul tablei de joc
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Culoare pentru contur
    SDL_RenderDrawRect(renderer, &board_area);

    // Iterează prin toate celulele tablei vizibile și desenează blocurile
    for (int i = -VISIBLE_OFFSET; i < Height; i++) // Rânduri vizibile (0 la Height-1)
    {
        for (int j = 0; j < Width; j++) // Coloane vizibile (0 la Width-1)
        {
            // Coordonata internă a rândului (incluzând VISIBLE_OFFSET)
            int bi = i + VISIBLE_OFFSET;

            // Desenează blocul doar dacă celula nu este goală
            if (board[bi][j] != ' ' && bi >= 0 && bi < Height + VISIBLE_OFFSET)
            {
                Color c = color_board[bi][j];
                SDL_Color color = {c.r, c.g, c.b, 255}; // Setează opacitatea la 255 pentru blocurile normale
                bool is_bomb = (board[bi][j] == 'B');

                draw_block(j, i, color, is_bomb); // j și i sunt coordonate vizibile
            }
        }
    }
}

/**
 * @brief Returnează o culoare SDL specifică pentru fiecare tip de piesă Tetris.
 * @param type Tipul piesei (enum PieceType).
 * @return Structura Color (RGB).
 */
Color get_piece_color(PieceType type)
{
    switch(type)
    {
        case PIECE_L: return (Color){255, 165, 0}; // Orange
        case PIECE_J: return (Color){0, 0, 255};   // Blue
        case PIECE_Z: return (Color){255, 0, 0};   // Red
        case PIECE_S: return (Color){0, 180, 0};   // Green
        case PIECE_O: return (Color){220, 220, 0}; // Yellow
        case PIECE_I: return (Color){0, 200, 200}; // Cyan
        case PIECE_T: return (Color){128, 0, 128}; // Purple
        default:      return (Color){255, 255, 255}; // Default white
    }
}

/**
 * @brief Desenează o piesă în mișcare pe ecran.
 * @param piece Matricea 4x4 a formei piesei.
 * @param x Coordonata X a piesei pe tablă.
 * @param y Coordonata Y a piesei pe tablă.
 * @param type Tipul piesei.
 * @param has_bomb True dacă piesa conține o bombă.
 * @param bomb_x Coordonata X a bombei în cadrul formei piesei.
 * @param bomb_y Coordonata Y a bombei în cadrul formei piesei.
 */
void draw_piece(char piece[4][4], int x, int y, PieceType type, bool has_bomb, int bomb_x, int bomb_y)
{
    Color c = get_piece_color(type);
    SDL_Color color = {c.r, c.g, c.b, 255}; // Piesele în mișcare sunt complet opace

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (piece[i][j] != ' ') // Dacă este un bloc al piesei
            {
                int gx = x + j; // Coordonata X globală pe tablă
                int gy = y + i; // Coordonata Y globală pe tablă
                bool is_bomb_block = has_bomb && i == bomb_y && j == bomb_x;

                // Desenează blocul piesei dacă este în zona vizibilă a tablei
                if (gy >= 0 && gx >= 0 && gx < Width)
                {
                    draw_block(gx, gy, color, is_bomb_block);
                }
            }
        }
    }
}

/**
 * @brief Prezintă conținutul renderer-ului pe ecran.
 */
void render_frame()
{
    SDL_RenderPresent(renderer);
}

/**
 * @brief Închide subsistemele SDL și eliberează resursele.
 */
void close_screen()
{
    if (font) TTF_CloseFont(font);
    TTF_Quit();

    if (line_clear_sound) Mix_FreeChunk(line_clear_sound);
    if (explosion_sound) Mix_FreeChunk(explosion_sound);
    Mix_CloseAudio();
    
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * @brief Desenează ecranul meniului principal.
 */
void draw_centered_text(const char *text, int center_x, int y, SDL_Color color, Uint8 alpha)
{
    SDL_Color full_color = color;
    full_color.a = alpha;

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, full_color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = {
        center_x - surface->w / 2,
        y,
        surface->w,
        surface->h
    };

    SDL_SetTextureAlphaMod(texture, alpha);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_menu(int high_score)
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Culoare fundal meniu
    SDL_RenderClear(renderer);

    // Desenează butonul "Start Game"
    SDL_Rect button = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 200, 80 };

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    bool hover = (mx >= button.x && mx <= button.x + button.w &&
                  my >= button.y && my <= button.y + button.h);

    if (hover)
        SDL_SetRenderDrawColor(renderer, 100, 160, 220, 255); // Hover
    else
        SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);  // Normal

    SDL_RenderFillRect(renderer, &button);

    // Text "Start Game" centrat
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

    Uint32 ticks = SDL_GetTicks();
    Uint8 alpha = 180 + 75 * sin(ticks / 400.0); // clipire retro: alpha între 180 și 255

    char hs_text[64];
    sprintf(hs_text, "High Score: %d", high_score);
    SDL_Color yellow = {255, 255, 120, alpha};
    draw_centered_text(hs_text, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 60, yellow, alpha);
}

/**
 * @brief Desenează ecranul "Game Over" cu un overlay.
 */
void draw_game_over(int high_score)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Permite transparența
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150); // Overlay negru semi-transparent
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    Uint32 ticks = SDL_GetTicks();
Uint8 alpha = 200 + 55 * sin(ticks / 300.0); // pulsare între 200-255

SDL_Color white = {255, 255, 255, alpha};
draw_centered_text("Game Over", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 120, white, alpha);

char text[64];
sprintf(text, "High Score: %d", high_score);
SDL_Color yellow = {255, 255, 0, alpha};
draw_centered_text(text, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 80, yellow, alpha);

if (get_score() == high_score && high_score > 0) {
    float t = ticks / 300.0f;
    SDL_Color festive = {
        (Uint8)(200 + 55 * sin(t)),
        (Uint8)(100 + 100 * sin(t + 2.0f)),
        (Uint8)(50 + 50 * sin(t + 4.0f)),
        255
    };
    draw_centered_text("New High Score!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 120, festive, 255);
}

    // Desenează butonul "Restart"
    SDL_Rect button = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 60 };

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    bool hover = (mx >= button.x && mx <= button.x + button.w &&
                  my >= button.y && my <= button.y + button.h);

    if (hover)
        SDL_SetRenderDrawColor(renderer, 100, 160, 220, 255); // Hover
    else
        SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);  // Normal

    SDL_RenderFillRect(renderer, &button);

    SDL_Color white_text = {255, 255, 255, 255};
    SDL_Surface *btnSurface = TTF_RenderText_Solid(font, "Restart", white_text);
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

/**
 * @brief Desenează un text pe ecran.
 * @param text Șirul de caractere de desenat.
 * @param x Coordonata X.
 * @param y Coordonata Y.
 */
void draw_text(const char *text, int x, int y)
{
    if(!font) return; // Asigură-te că fontul a fost încărcat

    SDL_Color color = {255, 255, 255, 255}; // Culoare text alb
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color); // Folosim Blended pentru o calitate mai bună
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/**
 * @brief Desenează scorul și nivelul curent pe panoul lateral stânga.
 */
void draw_score_and_level(void)
{
    char buffer[64];

    snprintf(buffer, sizeof(buffer), "Score: %d", get_score());
    draw_text(buffer, 20, SCREEN_HEIGHT - 80); // Poziție ajustată

    snprintf(buffer, sizeof(buffer), "Level: %d", get_level());
    draw_text(buffer, 20, SCREEN_HEIGHT - 40); // Poziție ajustată
}

/**
 * @brief Desenează o previzualizare a unei piese într-o cutie dată.
 * Ajustează poziția pentru a centra piesa în cutie.
 * @param shape Forma piesei (matrice 4x4).
 * @param x Coordonata X a cutiei.
 * @param y Coordonata Y a cutiei.
 * @param box_size Dimensiunea unui bloc în previzualizare.
 * @param type Tipul piesei.
 * @param has_bomb True dacă piesa are o bombă.
 * @param bomb_x Coordonata X a bombei în cadrul formei.
 * @param bomb_y Coordonata Y a bombei în cadrul formei.
 */
void draw_piece_preview(char shape[4][4], int x, int y, int box_size, PieceType type, bool has_bomb, int bomb_x, int bomb_y)
{
    Color c = get_piece_color(type);
    SDL_Color color = {c.r, c.g, c.b, 255}; // Previzualizările sunt complet opace

    // Calculează dimensiunile reale ale piesei pentru centrare
    int min_i = 4, max_i = 0, min_j = 4, max_j = 0;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            if(shape[i][j] != ' ')
            {
                if(i < min_i) min_i = i;
                if(i > max_i) max_i = i;
                if(j < min_j) min_j = j;
                if(j > max_j) max_j = j;
            }
        }
    }

    int piece_width = (max_j - min_j + 1) * box_size;
    int piece_height = (max_i - min_i + 1) * box_size;

    // Dimensiunile cutiei de previzualizare (6x4 blocuri de previzualizare)
    int target_width = 6 * box_size;
    int target_height = 4 * box_size;

    // Calculează offset-ul pentru a centra piesa în cutie
    int offset_x = x + (target_width - piece_width) / 2;
    int offset_y = y + (target_height - piece_height) / 2;

    // Desenează blocurile piesei de previzualizare
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            if(shape[i][j] != ' ')
            {
                SDL_Rect rect = {
                    offset_x + (j - min_j) * box_size, // Ajustează poziția J relativ la colțul piesei
                    offset_y + (i - min_i) * box_size, // Ajustează poziția I relativ la colțul piesei
                    box_size,
                    box_size
                };

                bool is_bomb_block = has_bomb && i == bomb_y && j == bomb_x;

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Bordură neagră pentru previzualizare
                SDL_RenderDrawRect(renderer, &rect);

                if (is_bomb_block)
                {
                    // Desenează bomba în interiorul blocului de previzualizare
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_Rect bomb = {
                        rect.x + box_size / 4,
                        rect.y + box_size / 4,
                        box_size / 2,
                        box_size / 2
                    };
                    SDL_RenderFillRect(renderer, &bomb);

                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_Rect wick = {
                        rect.x + box_size / 2 - 2,
                        rect.y + 2,
                        4,
                        4
                    };
                    SDL_RenderFillRect(renderer, &wick);
                }
            }
        }
    }
}

/**
 * @brief Desenează previzualizările pieselor următoare (next_piece și next_next_piece).
 */
void draw_next_pieces(void)
{
    int box_size = BLOCK_SIZE / 2; // Jumătate din dimensiunea unui bloc normal
    int box_width = 6 * box_size;  // Lățimea totală a zonei de previzualizare (6 blocuri mici)
    int box_height = 4 * box_size; // Înălțimea totală a unei cutii de previzualizare (4 blocuri mici)

    int preview_x = SCREEN_WIDTH - box_width - 20; // Poziția X pentru panoul de previzualizare (în dreapta)
    int preview_y = 20;                            // Poziția Y (aproape de sus)

    // Desenează fundalul pentru zona de previzualizare
    SDL_Rect preview_area = { preview_x, preview_y, box_width, 2 * box_height + 10 };
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255); // Culoare fundal
    SDL_RenderFillRect(renderer, &preview_area);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Culoare bordură
    SDL_RenderDrawRect(renderer, &preview_area);

    // Desenează "Next Piece"

    draw_piece_preview(
        next_piece.shape,
        preview_x,
        preview_y,
        box_size,
        next_piece.type,
        next_piece.has_bomb,
        next_piece.bomb_x,
        next_piece.bomb_y
    );

    // Desenează "Next Next Piece"
    
    draw_piece_preview(
        next_next_piece.shape,
        preview_x,
        preview_y + box_height + 10, // A doua piesă sub prima
        box_size,
        next_next_piece.type,
        next_next_piece.has_bomb,
        next_next_piece.bomb_x,
        next_next_piece.bomb_y
    );
}

/**
 * @brief Desenează scorul maxim pe ecran.
 * @param high_score Scorul maxim.
 * @param font_param Fontul de utilizat.
 * @param x Coordonata X.
 * @param y Coordonata Y.
 */
void draw_high_score(int high_score, TTF_Font *font_param, int x, int y)
{
    char text[64];
    sprintf(text, "High Score: %d", high_score);
    SDL_Color color = {255, 255, 0, 255}; // Galben
    SDL_Surface *surface = TTF_RenderText_Solid(font_param, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/**
 * @brief Desenează mesajul "New High Score!" pe ecran.
 * @param font_param Fontul de utilizat.
 * @param x Coordonata X.
 * @param y Coordonata Y.
 */
void draw_new_high_score(TTF_Font *font_param, int x, int y)
{
    SDL_Color color = {255, 50, 50, 255}; // Roșu aprins
    SDL_Surface *surface = TTF_RenderText_Solid(font_param, "New High Score!", color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/**
 * @brief Desenează un frame al efectului de explozie.
 * @param ex Coloana centrului exploziei.
 * @param ey Rândul centrului exploziei.
 * @param elapsed_time Timpul scurs de la începutul exploziei.
 */
void draw_explosion_effect(int ex, int ey, Uint32 elapsed_time) {
    // Folosim EXPLOSION_TOTAL_DURATION din config.h
    const Uint32 INTERNAL_FLASH_DURATION = 100; // Durata flash-ului inițial intens

    if (elapsed_time < EXPLOSION_TOTAL_DURATION) {
        // Efect de flash central intens la început
        if (elapsed_time < INTERNAL_FLASH_DURATION) {
            SDL_Color flash_color = {
                255,
                100 + rand() % 100, // Galben-portocaliu aleatoriu
                rand() % 50,
                255 // Complet opac
            };
            draw_block(ex, ey, flash_color, false);
        }

        // Efect de unde de șoc (pătrate care se răspândesc și se estompează)
        float progress = (float)elapsed_time / EXPLOSION_TOTAL_DURATION; // Progres de la 0.0 la 1.0
        int max_radius_blocks = 2; // Rază maximă în blocuri (afectează 2 blocuri în jurul centrului)

        for (int r_offset = 1; r_offset <= max_radius_blocks; r_offset++) {
            // Calculează progresul pentru unda curentă
            float current_wave_progress = progress * (max_radius_blocks + 1);
            if (current_wave_progress < r_offset || current_wave_progress > r_offset + 1) {
                continue; // Desenează unda doar când este la raza curentă
            }

            // Calculează opacitatea undei (scade pe măsură ce se extinde)
            int alpha_wave = 255 - (int)((current_wave_progress - r_offset) * 255);
            if (alpha_wave < 0) alpha_wave = 0;

            //SDL_Color wave_color = {255, 165, 0, (Uint8)alpha_wave}; // Portocaliu pentru unde

            // Desenează un inel de blocuri la raza curentă
            for (int i = -1; i <= 2; i++) {
                for (int j = -1; j <= 2; j++) {
                    int x = ex + j;
                    int y = ey + i;
            
                    if (x >= 0 && x < Width && y >= 0 && y < Height) {
                        SDL_Color c = {
                            255,
                            120 + rand() % 80,
                            0,
                            (Uint8)(255 - 255 * elapsed_time / EXPLOSION_TOTAL_DURATION)
                        };
                        draw_block(x, y, c, false);
                    }
                }
            }
        }

        // Efect de fum care se disipă
        int alpha_smoke = 150 - (elapsed_time * 150 / EXPLOSION_TOTAL_DURATION); // Opacitatea scade pe durata exploziei
        if (alpha_smoke < 0) alpha_smoke = 0;
        draw_smoke_overlay(ex, ey, alpha_smoke);
    }
}

/**
 * @brief Desenează un overlay de fum semi-transparent.
 * @param cx Coloana centrului fumului.
 * @param cy Rândul centrului fumului.
 * @param alpha Opacitatea fumului (0-255).
 */
void draw_smoke_overlay(int cx, int cy, int alpha)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, (Uint8)alpha); // Fum gri-cenușiu

    // Desenează un pătrat de 3x3 blocuri în jurul centrului pentru efectul de fum
    for (int i = -1; i <= 2; i++) // Rânduri relative
    {
        for (int j = -1; j <= 2; j++) // Coloane relative
        {
            int x_board = cx + j;
            int y_board = cy + i;

            // Desenează doar în zona vizibilă a tablei
            if (x_board >= 0 && x_board < Width && y_board >= 0 && y_board < Height)
            {
                SDL_Rect smoke_rect = {
                    (x_board + SIDE_PANEL_WIDTH) * BLOCK_SIZE, // Ajustează pentru panoul lateral
                    y_board * BLOCK_SIZE,
                    BLOCK_SIZE,
                    BLOCK_SIZE
                };
                SDL_RenderFillRect(renderer, &smoke_rect);
            }
        }
    }
}

/**
 * @brief Desenează un frame al efectului de flash de linie.
 * @param line_y Rândul pe care se face flash-ul.
 * @param elapsed_time Timpul scurs de la începutul flash-ului.
 */
void draw_line_flash_effect(int line_y, Uint32 elapsed_time) {
    if (elapsed_time < FLASH_LINE_TOTAL_DURATION) { // Utilizează constanta din config.h
        // Calculează opacitatea care scade în timp
        int alpha = 255 - (elapsed_time * 255 / FLASH_LINE_TOTAL_DURATION);
        if (alpha < 0) alpha = 0;

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, (Uint8)alpha); // Flash alb

        // Desenează un dreptunghi alb peste linia care a fost ștersă
        SDL_Rect flash_rect = {
            (SIDE_PANEL_WIDTH) * BLOCK_SIZE, // Începe după panoul lateral
            line_y * BLOCK_SIZE,
            Width * BLOCK_SIZE,              // Lățimea tablei
            BLOCK_SIZE                       // Înălțimea unei linii
        };
        SDL_RenderFillRect(renderer, &flash_rect);
    }
}

/**
 * @brief Funcția principală de randare a întregului frame de joc.
 * Desenează tabla, piesa curentă, previzualizările, scorul și efectele vizuale.
 * @param board Tabla de joc.
 * @param color_board Tabla de culori.
 * @param high_score Scorul maxim.
 */
void draw_full_game_frame(char board[Height + VISIBLE_OFFSET][Width], Color color_board[Height + VISIBLE_OFFSET][Width], int high_score)
{
    draw_board(board, color_board); // Desenează fundalul tablei și blocurile statice

    // Desenează piesa curentă (mobilă)
    draw_piece(current_piece.shape, current_piece.x, current_piece.y, current_piece.type, current_piece.has_bomb, current_piece.bomb_x, current_piece.bomb_y);

    // Desenează toate exploziile active
    for (int i = 0; i < MAX_ACTIVE_EXPLOSIONS; i++) {
        if (active_explosions[i].active) {
            Uint32 elapsed = SDL_GetTicks() - active_explosions[i].startTime;
            if (elapsed < EXPLOSION_TOTAL_DURATION) {
                draw_explosion_effect(active_explosions[i].x, active_explosions[i].y, elapsed);
            } else {
                apply_explosion_effect_to_board(active_explosions[i].x, active_explosions[i].y);
                active_explosions[i].active = false; // Dezactivează explozia după ce timpul a expirat
            }
        }
    }

    // Desenează toate flash-urile de linii active
    for (int i = 0; i < MAX_ACTIVE_LINE_FLASHES; i++) {
        if (active_line_flashes[i].active) {
            Uint32 elapsed = SDL_GetTicks() - active_line_flashes[i].startTime;
            if (elapsed < FLASH_LINE_TOTAL_DURATION) {
                draw_line_flash_effect(active_line_flashes[i].line_y, elapsed);
            } else {
                active_line_flashes[i].active = false; // Dezactivează flash-ul după ce timpul a expirat
            }
        }
    }

    // Desenează informații UI
    draw_score_and_level();
    draw_text("High Score:", 15, 30);
    char score_text[32];
    sprintf(score_text, "%d", high_score);
    draw_text(score_text, 15 + 40, 30 + 40); // Ajustează poziția pentru lizibilitate
    draw_next_pieces(); // Desenează previzualizările pieselor următoare

    render_frame(); // Prezintă tot ce s-a desenat pe ecran
}