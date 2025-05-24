#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h> // Includem SDL pentru SDL_GetTicks()
#include <SDL2/SDL_mixer.h>
#include "game.h"
#include "screen.h"   // Pentru Color, PieceType, draw_block etc.
#include "config.h"   // Pentru Width, Height, VISIBLE_OFFSET, MAX_ACTIVE_EXPLOSIONS, etc.

// Declarări globale ale variabilelor principale ale jocului
// Acestea sunt definite o singură dată aici, iar în alte fișiere se folosește `extern`
Piece current_piece;
Piece next_piece;
Piece next_next_piece;
GameState gameState; // Nu mai e extern aici, ci e definită global.

static Uint32 lastFall = 0; // Timpul la care a căzut ultima piesă
const Uint32 baseFallInterval = 800; // Intervalul de cădere inițial (în ms)

// Tablele de joc și de culori
char board[Height + VISIBLE_OFFSET][Width];
Color color_board[Height + VISIBLE_OFFSET][Width];

static int score = 0;
static int level = 0;
static int total_lines_cleared = 0;

// Array-uri pentru gestionarea exploziilor și flash-urilor de linii active
ActiveExplosion active_explosions[MAX_ACTIVE_EXPLOSIONS];
// int num_active_explosions = 0; // Eliminat - nu mai este necesar un contor direct

ActiveLineFlash active_line_flashes[MAX_ACTIVE_LINE_FLASHES];
// int num_active_line_flashes = 0; // Eliminat - nu mai este necesar un contor direct

// Formele pieselor Tetris
char pieces[NUM_PIECES][4][4] = {
    { // L
        { ' ', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // Inverted L (J)
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // Z
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { '#', '#', ' ', ' ' },
        { ' ', '#', '#', ' ' }
    },
    { // Inverted Z (S)
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', '#', ' ' },
        { '#', '#', ' ', ' ' }
    },
    { // O
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', '#', ' ' },
        { ' ', '#', '#', ' ' }
    },
    { // I
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' },
        { '#', ' ', ' ', ' ' }
    },
    { // T
        { ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ' },
        { ' ', '#', ' ', ' ' },
        { '#', '#', '#', ' ' }
    }
};

/**
 * @brief Roteste o piesă de Tetris (matrice 4x4) la 90 de grade în sens orar.
 * Actualizează și poziția bombei, dacă există.
 * @param shape Matricea 4x4 a piesei.
 * @param bomb_x Pointer la coordonata X a bombei în cadrul formei.
 * @param bomb_y Pointer la coordonata Y a bombei în cadrul formei.
 */
void rotate_piece(char shape[4][4], int *bomb_x, int *bomb_y)
{
    char temp[4][4]; // Matrice temporară pentru rotație
    int bomb_marker[4][4] = {0}; // Marchează poziția bombei înainte de rotație

    // Dacă există o bombă, marchează-i poziția
    if (bomb_x && bomb_y && *bomb_x >= 0 && *bomb_y >= 0)
        bomb_marker[*bomb_y][*bomb_x] = 1;

    // Efectuează rotația
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j][3 - i] = shape[i][j]; // Rotația la 90 de grade în sens orar

            // Actualizează poziția bombei după rotație
            if (bomb_marker[i][j])
            {
                *bomb_y = j;
                *bomb_x = 3 - i;
            }
        }
    }

    // Copiază forma rotită înapoi în 'shape'
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            shape[i][j] = temp[i][j];
}

/**
 * @brief Inițializează tabla de joc (board și color_board) cu spații goale și culori negre.
 * @param board Tabla de joc (caractere).
 */
void create_board(char board[Height + VISIBLE_OFFSET][Width])
{
    for (int i = 0; i < Height + VISIBLE_OFFSET; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            board[i][j] = ' '; // Setează celula ca fiind goală
            color_board[i][j] = (Color){0, 0, 0}; // Setează culoarea la negru (transparent implicit)
        }
    }
}

/**
 * @brief Actualizează scorul și nivelul jocului în funcție de numărul de linii șterse.
 * @param lines_cleared Numărul de linii șterse simultan.
 */
void update_score_and_level(int lines_cleared)
{
    // Tabelul de scoruri pentru 1, 2, 3 sau 4 linii
    static const int score_table[] = {0, 40, 100, 300, 1200};

    if (lines_cleared >= 1 && lines_cleared <= 4)
    {
        score += score_table[lines_cleared] * (level + 1); // Scorul crește cu nivelul
        total_lines_cleared += lines_cleared;              // Adaugă la totalul de linii șterse
        level = total_lines_cleared / 10;                  // Nivelul crește la fiecare 10 linii
    }
}

/**
 * @brief Returnează scorul curent al jocului.
 * @return Scorul curent.
 */
int get_score(void)
{
    return score;
}

/**
 * @brief Returnează nivelul curent al jocului.
 * @return Nivelul curent.
 */
int get_level(void)
{
    return level;
}

/**
 * @brief Resetează scorul, nivelul și totalul de linii șterse la zero.
 */
void reset_score_and_level(void)
{
    score = 0;
    level = 0;
    total_lines_cleared = 0;
}

/**
 * @brief Declanseaza o animație de explozie la o anumită locație.
 * @param x Coloana centrului exploziei.
 * @param y Rândul centrului exploziei.
 */
void trigger_explosion(int x, int y) {
    // Caută primul slot inactiv în array-ul de explozii
    for (int i = 0; i < MAX_ACTIVE_EXPLOSIONS; i++) {
        if (!active_explosions[i].active) {
            active_explosions[i].x = x;
            active_explosions[i].y = y;
            active_explosions[i].startTime = SDL_GetTicks(); // Setează timpul de start
            active_explosions[i].active = true; // Marchează explozia ca fiind activă
            return; // Am găsit un slot și am declanșat explozia, ieșim
        }
    }
    // Dacă nu a fost găsit niciun slot inactiv (array-ul este plin), explozia nu este declanșată
}

/**
 * @brief Declanseaza o animație de flash pe o linie specifică.
 * @param line_y Rândul pe care se va face flash-ul.
 */
void trigger_line_flash(int line_y) {
    // Caută primul slot inactiv în array-ul de flash-uri de linii
    for (int i = 0; i < MAX_ACTIVE_LINE_FLASHES; i++) {
        if (!active_line_flashes[i].active) {
            active_line_flashes[i].line_y = line_y;
            active_line_flashes[i].startTime = SDL_GetTicks(); // Setează timpul de start
            active_line_flashes[i].active = true; // Marchează flash-ul ca fiind activ
            return; // Am găsit un slot și am declanșat flash-ul, ieșim
        }
    }
}

/**
 * @brief Aplica efectul unei explozii pe tabla de joc, ștergând blocurile dintr-o zonă 4x4.
 * @param center_x Coloana centrului exploziei.
 * @param center_y Rândul centrului exploziei.
 */
void apply_explosion_effect_to_board(int center_x, int center_y) {
    // Coordonatele de start pentru pătratul 4x4 afectat
    // Centrul exploziei (bomba) este în mijlocul zonei de 4x4, deci 1 bloc la stânga și 1 bloc deasupra
    int start_x = center_x - 1;
    int start_y = center_y - 1;

    // Iterăm pe o zonă de 4x4 blocuri
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int target_y = start_y + i;
            int target_x = start_x + j;
    
            if (target_x >= 0 && target_x < Width &&
                target_y >= 0 && target_y < Height)
            {
                int by = target_y + VISIBLE_OFFSET;
                int bx = target_x;
    
                // Dacă este o altă bombă, declanșează explozia ei
                if (board[by][bx] == 'B') {
                    trigger_explosion(bx, target_y); // lansează o explozie nouă în acea bombă
                }
    
                // Șterge oricum celula
                board[by][bx] = ' ';
                color_board[by][bx] = (Color){0, 0, 0};
            }
        }
    }
}

/**
 * @brief Aplica gravitația liniilor, mutând liniile de deasupra celor șterse în jos.
 * @param board Tabla de joc.
 * @param color_board Tabla de culori.
 * @param deleted_lines Array boolean care indică ce linii au fost șterse.
 */
void apply_line_gravity(char board[Height + VISIBLE_OFFSET][Width],
    Color color_board[Height + VISIBLE_OFFSET][Width],
    bool deleted_lines[Height])
{
    char new_board[Height + VISIBLE_OFFSET][Width];
    Color new_color[Height + VISIBLE_OFFSET][Width];

    // Inițializăm noile table cu spații goale și culori negre
    for (int i = 0; i < Height + VISIBLE_OFFSET; i++)
    {
        for (int j = 0; j < Width; j++)
        {
            new_board[i][j] = ' ';
            new_color[i][j] = (Color){0, 0, 0};
        }
    }

    int dst = Height - 1; // Rândul destinație, începând de jos

    // Parcurgem tabla de la jos în sus
    for (int src = Height - 1; src >= 0; src--)
    {
        // Dacă linia curentă NU a fost marcată pentru ștergere
        if (!deleted_lines[src])
        {
            // Copiem linia de la src la dst
            for (int col = 0; col < Width; col++)
            {
                new_board[dst + VISIBLE_OFFSET][col] = board[src + VISIBLE_OFFSET][col];
                new_color[dst + VISIBLE_OFFSET][col] = color_board[src + VISIBLE_OFFSET][col];
            }
            dst--; // Mutăm destinația un rând mai sus
        }
    }

    // Copiem conținutul noilor table peste cele originale
    memcpy(board, new_board, sizeof(new_board));
    memcpy(color_board, new_color, sizeof(new_color));
}

/**
 * @brief Verifică și șterge liniile complete de pe tablă, procesează bombele.
 * Apoi, aplică gravitația liniilor rămase.
 * @param board Tabla de joc.
 */
void clear_full_lines(char board[Height + VISIBLE_OFFSET][Width])
{
    int lines_cleared = 0;
    bool deleted_lines[Height] = {false}; // Marchează liniile care vor fi șterse

    // Structura pentru a stoca coordonatele bombelor găsite
    typedef struct {
        int x, y;
    } BombCoord;
    BombCoord bombs_to_explode[MAX_ACTIVE_EXPLOSIONS * 4]; // Un buffer suficient de mare (ex: 4 bombe per linie x MAX_ACTIVE_EXPLOSIONS)
    int bomb_count = 0;

    // Parcurgem tabla de la jos în sus pentru a detecta linii complete
    for (int i = Height - 1; i >= 0; i--)
    {
        bool full_line = true;
        for (int j = 0; j < Width; j++)
        {
            if (board[i + VISIBLE_OFFSET][j] == ' ')
            {
                full_line = false; // Linia nu este completă dacă există un spațiu gol
                break;
            }
        }

        if (full_line)
{
    lines_cleared++;
    deleted_lines[i] = true; // Marchează linia pentru ștergere
    trigger_line_flash(i);  // Animația de flash

    bool line_has_bomb = false;

    // Verifică dacă linia conține bombe
    for (int j = 0; j < Width; j++)
    {
        if (board[i + VISIBLE_OFFSET][j] == 'B')
        {
            line_has_bomb = true;
            if (bomb_count < sizeof(bombs_to_explode) / sizeof(BombCoord))
            {
                bombs_to_explode[bomb_count++] = (BombCoord){.x = j, .y = i};
            }
        }
    }

    // Redă doar sunetul corespunzător
    if (line_has_bomb)
    {
        Mix_PlayChannel(-1, explosion_sound, 0); // Doar explozia
    }
    else
    {
        Mix_PlayChannel(-1, line_clear_sound, 0); // Doar sunetul de linie
    }
}
    }

    // Dacă nu s-au șters linii și nu s-au găsit bombe, nu mai facem nimic
    if (lines_cleared == 0 && bomb_count == 0)
        return;

    // Procesează exploziile bombelor găsite
    for (int b = 0; b < bomb_count; b++)
    {
        Mix_PlayChannel(-1, explosion_sound, 0);
        trigger_explosion(bombs_to_explode[b].x, bombs_to_explode[b].y); // Declanseaza animația de explozie
        apply_explosion_effect_to_board(bombs_to_explode[b].x, bombs_to_explode[b].y); // Aplică efectul exploziei pe tablă
    }

    // Aplică gravitația liniilor după ce toate exploziile și ștergerile au fost procesate
    apply_line_gravity(board, color_board, deleted_lines);

    // Actualizează scorul și nivelul jocului
    update_score_and_level(lines_cleared);
}

/**
 * @brief Verifică dacă o piesă aflată la noile coordonate se va ciocni cu alte blocuri
 * sau cu marginile tablei de joc.
 * @param board Tabla de joc.
 * @param newX Noua coordonată X (coloană) a piesei.
 * @param newY Noua coordonată Y (rând) a piesei.
 * @param shape Forma piesei (matrice 4x4).
 * @return 1 dacă există coliziune, 0 altfel.
 */
int check_collision(char board[Height + VISIBLE_OFFSET][Width], int newX, int newY, char shape[4][4])
{
    for (int i = 0; i < 4; i++) // Iterează pe rândurile formei piesei
    {
        for (int j = 0; j < 4; j++) // Iterează pe coloanele formei piesei
        {
            if (shape[i][j] != ' ') // Dacă este un bloc al piesei
            {
                int y = newY + i; // Coordonata Y absolută pe tablă
                int x = newX + j; // Coordonata X absolută pe tablă

                // Verifică coliziunea cu marginile sau cu blocurile existente pe tablă
                if (y >= Height || x < 0 || x >= Width || board[y + VISIBLE_OFFSET][x] != ' ')
                {
                    return 1; // Coliziune detectată
                }
            }
        }
    }
    return 0; // Nicio coliziune
}

/**
 * @brief Generează o piesă Tetris aleatorie.
 * Poate include o bombă cu o șansă de 30%.
 * @return Structura Piece a piesei generate.
 */
Piece generate_random_piece(void)
{
    int r = rand() % NUM_PIECES; // Alege un tip de piesă aleatoriu
    Piece new_piece;
    new_piece.type = (PieceType)r;

    // Copiază forma piesei din array-ul global 'pieces'
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            new_piece.shape[i][j] = pieces[r][i][j];
        }
    }

    // Setează poziția inițială a piesei (centrul superior al tablei)
    new_piece.x = (Width / 2) - 2;
    new_piece.y = 0; // Începe de sus, în zona invizibilă

    // Decide dacă piesa va avea o bombă (30% șanse)
    new_piece.has_bomb =  (rand() % 100) < 30;

    if (new_piece.has_bomb)
    {
        int found = 0;
        // Alege o poziție aleatorie pentru bombă în cadrul formei piesei
        while (!found)
        {
            int i = rand() % 4;
            int j = rand() % 4;
            if (new_piece.shape[i][j] != ' ') // Bomba trebuie să fie pe un bloc existent al piesei
            {
                new_piece.bomb_y = i;
                new_piece.bomb_x = j;
                found = 1;
            }
        }
    }
    else
    {
        new_piece.bomb_x = -1; // -1 indică că nu există bombă
        new_piece.bomb_y = -1;
    }

    return new_piece;
}

/**
 * @brief Încarcă scorul maxim din fișierul "highscore.txt".
 * @return Scorul maxim încărcat, sau 0 dacă fișierul nu există/este gol.
 */
int load_high_score()
{
    FILE *f = fopen("highscore.txt", "r");
    int score = 0;
    if (f)
    {
        fscanf(f, "%d", &score);
        fclose(f);
    }
    return score;
}

/**
 * @brief Salvează scorul maxim în fișierul "highscore.txt".
 * @param score Scorul de salvat.
 */
void save_high_score(int score)
{
    FILE *f = fopen("highscore.txt", "w");
    if (f)
    {
        fprintf(f, "%d", score);
        fclose(f);
    }
}

/**
 * @brief Generează o piesă nouă și o plasează pe tablă.
 * Verifică coliziunea la spawn și, dacă este cazul, setează jocul în stare de GAME_OVER.
 * @param board Tabla de joc.
 * @param high_score Pointer la scorul maxim curent pentru a-l actualiza la game over.
 */
void spawn_random_piece(char board[Height + VISIBLE_OFFSET][Width], int *high_score)
{
    // Ciclăm piesele: next_next_piece devine next_piece, next_piece devine current_piece
    current_piece = next_piece;
    next_piece = next_next_piece;
    next_next_piece = generate_random_piece(); // Generăm o nouă piesă pentru next_next_piece

    // Setăm poziția inițială a piesei curente
    current_piece.x = (Width / 2) - 2;
    current_piece.y = 0;

    // Verificăm dacă noua piesă se ciocnește la spawn (Game Over)
    while (check_collision(board, current_piece.x, current_piece.y, current_piece.shape))
    {
        current_piece.y--;
        if (current_piece.y < -VISIBLE_OFFSET)
        {
            gameState = GAME_OVER;
        if (score > *high_score)
        {
            *high_score = score;
            save_high_score(*high_score);
        }
            return;
        }
    }
}

/**
 * @brief Resetează jocul la starea inițială, gata pentru o nouă partidă.
 * @param board Tabla de joc.
 */
void restart_game(char board[Height + VISIBLE_OFFSET][Width])
{
    create_board(board); // Golește tabla de joc
    reset_score_and_level(); // Resetează scorul și nivelul

    // Resetează toate exploziile active
    for(int i = 0; i < MAX_ACTIVE_EXPLOSIONS; i++) {
        active_explosions[i].active = false;
    }

    // Resetează toate flash-urile de linii active
    for(int i = 0; i < MAX_ACTIVE_LINE_FLASHES; i++) {
        active_line_flashes[i].active = false;
    }

    // Generează piesele inițiale pentru preview și joc
    next_piece = generate_random_piece();
    next_next_piece = generate_random_piece();

    // Setează prima piesă curentă
    current_piece = next_piece;
    next_piece = next_next_piece;
    next_next_piece = generate_random_piece();

    current_piece.x = (Width / 2) - 2;
    current_piece.y = 0;

    // Verifică din nou coliziunea la spawn, deși ar fi rar aici
    if (check_collision(board, current_piece.x, current_piece.y, current_piece.shape))
    {
        gameState = GAME_OVER; // Dacă totuși se întâmplă, jocul e pierdut imediat
        return;
    }

    gameState = GAME; // Setează starea jocului la "în joc"
}

/**
 * @brief Actualizează logica jocului (mișcarea piesei, căderea automată, coliziuni).
 * @param board Tabla de joc.
 * @param e Pointer la evenimentul SDL (poate fi NULL pentru căderea automată).
 * @param high_score Pointer la scorul maxim curent.
 */
void update_game(char board[Height + VISIBLE_OFFSET][Width], SDL_Event *e, int *high_score)
{
    // Procesează input-ul de la tastatură (dacă există un eveniment)
    if (e && e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
            case SDLK_SPACE: // Rotația piesei
            {
                char backup[4][4]; // Salvează forma curentă pentru backup
                memcpy(backup, current_piece.shape, sizeof(backup));
                int old_bomb_x = current_piece.bomb_x; // Salvează poziția bombei
                int old_bomb_y = current_piece.bomb_y;

                rotate_piece(current_piece.shape, &current_piece.bomb_x, &current_piece.bomb_y); // Roteste piesa

                // Dacă rotația duce la coliziune, revino la forma anterioară
                if (check_collision(board, current_piece.x, current_piece.y, current_piece.shape))
                {
                    memcpy(current_piece.shape, backup, sizeof(backup));
                    current_piece.bomb_x = old_bomb_x;
                    current_piece.bomb_y = old_bomb_y;
                }
                break;
            }
            case SDLK_LEFT: // Mișcare la stânga
                if (!check_collision(board, current_piece.x - 1, current_piece.y, current_piece.shape))
                    current_piece.x--;
                break;
            case SDLK_RIGHT: // Mișcare la dreapta
                if (!check_collision(board, current_piece.x + 1, current_piece.y, current_piece.shape))
                    current_piece.x++;
                break;
            case SDLK_DOWN: // Mișcare rapidă în jos
                if (!check_collision(board, current_piece.x, current_piece.y + 1, current_piece.shape))
                    current_piece.y++;
                // Dacă piesa cade rapid, actualizăm timpul de cădere pentru a nu mai aștepta intervalul normal
                lastFall = SDL_GetTicks();
                break;
        }
    }

    Uint32 currentTime = SDL_GetTicks(); // Timpul curent

    // Calculează intervalul de cădere în funcție de nivel
    int currentLevel = get_level();
    int interval = baseFallInterval / (1 + currentLevel);
    if (interval < 100) interval = 100; // Limitează viteza minimă de cădere

    // Căderea automată a piesei
    if (currentTime - lastFall > interval)
    {
        // Verifică dacă piesa se poate mișca în jos
        if (check_collision(board, current_piece.x, current_piece.y + 1, current_piece.shape))
        {
            // Dacă nu se poate mișca în jos, înseamnă că s-a așezat pe tablă
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (current_piece.shape[i][j] != ' ') // Dacă este un bloc al piesei
                    {
                        int y = current_piece.y + i;
                        int x = current_piece.x + j;
                        // Asigură-te că blocurile sunt în limitele array-ului intern
                        if (x >= 0 && x < Width && y + VISIBLE_OFFSET >= 0 && y + VISIBLE_OFFSET < Height + VISIBLE_OFFSET)
                        {
                            if (current_piece.has_bomb && i == current_piece.bomb_y && j == current_piece.bomb_x)
                            {
                                board[y + VISIBLE_OFFSET][x] = 'B'; // Setează ca bombă
                            }
                            else
                            {
                                board[y + VISIBLE_OFFSET][x] = '#'; // Setează ca bloc normal
                            }
                            // Setează culoarea blocului pe tabla de culori
                            color_board[y + VISIBLE_OFFSET][x] = get_piece_color(current_piece.type);
                        }
                    }
                }
            }
            clear_full_lines(board); // Verifică și șterge liniile complete/bombele
            spawn_random_piece(board, high_score); // Generează o nouă piesă
        }
        else
        {
            current_piece.y++; // Piesa se mișcă un rând în jos
        }
        lastFall = currentTime; // Actualizează timpul ultimei căderi
    }
}