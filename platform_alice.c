#include "platform.h"

#ifdef ALICE
#include "game_state.h"

// Colors for each tetromino - Alice specific mapping
char tetrominos_colors[] = {
    yellow, cyan, pink, green, red, blue, orange};


// Keyboard matrix for Alice
uint8_t keys_per_column[8][8] = {
    {'@', 'H', 'P', 'X', '0', '8', 0 /*(control)*/, 0},
    {'Q', 'I', 'A', 'Y', '1', '9', '_', 0},
    {'B', 'J', 'R', 'W', '2', ':', 0 /*(break)*/, 0},
    {'C', 'K', 'S', '_', '3', 'M', 0 /*(left)*/, 0},
    {'D', 'L', 'T', '_', '4', ',', 0 /*(right)*/, 0},
    {'E', '/', 'U', '_', '5', '-', 0 /*(down)*/, 0},
    {'F', 'N', 'V', 0 /*(enter)*/, '6', '.', 0 /*(up)*/, 0},
    {'G', 'O', 'Z', ' ', '7', '+', 0 /*(shift)*/, 0},
};

uint8_t all_key_columns[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};

/************************************************************/
/* Display functions for Alice 32/90                        */
/************************************************************/

void posxy(unsigned char column, unsigned char line)
{
    if (line > 0)
        line += 7;

    POKE(R6, line);
    POKE(R7, column);
}

void color(unsigned char foreground, unsigned char background)
{
    unsigned char b = 0, a;
    if (foreground > pink)
    {
        b = 1;
        foreground -= 8;
    }
    if (background > pink)
    {
        b = 1;
        background -= 8;
    }
    a = background + (foreground * 16);
    POKE(R2, b);
    POKE(R3, a);
}

void prints(unsigned char x, unsigned char y, unsigned char *text)
{
    char *c = text;
    posxy(x, y);
    for (; *c != '\0'; c++)
    {
        POKE(R1, *c);
        POKE(R0EXEC, 1);
        BUSY();
    }
}

void printsg(unsigned char x, unsigned char y, unsigned char *text)
{
    char *c = text;
    posxy(x, y);
    for (; *c != '\0'; c++)
    {
        POKE(R1, *c);
        POKE(R2, 0x20);
        POKE(R0EXEC, 1);
        BUSY();
    }
}

void printc(unsigned char x, unsigned char y, unsigned char c)
{
    posxy(x, y);
    POKE(R1, c);
    POKE(R0EXEC, 1);
    BUSY();
}

void printcg(unsigned char x, unsigned char y, unsigned char c)
{
    posxy(x, y);
    POKE(R1, c);
    POKE(R2, 0x20);
    POKE(R0EXEC, 1);
    BUSY();
}

uint8_t charatxy(uint8_t column, uint8_t line)
{
    if (line > 0)
        line += 7;

    POKE(R6, line);
    POKE(R7, column);

    POKE(R0EXEC, 8);
    BUSY();

    return PEEK(R1);
}

/************************************************************/
/* Keyboard and clock                                       */
/************************************************************/

uint8_t scankey()
{
    uint8_t key;
    uint8_t col;
    uint8_t rank;

    for (col = 0; col < 8; col++)
    {
        POKE(0x0002, all_key_columns[col]);
        key = PEEK(0xBFFF);

        if (key == 0xFF)
            continue;

        key = ~key;

        rank = 0;
        while ((key & 0x01) == 0)
        {
            key = key >> 1;
            rank++;
        }

        if (rank > 5)
            continue;

        return keys_per_column[col][rank];
    }

    return 0;
}

void sleep(uint8_t seconds)
{
    uint8_t clock;
    uint8_t tick = 0;

    while (1)
    {
        clock = PEEK(0x0008);
        PEEK(0x0009);

        if (clock & 0x20)
        {
            tick++;
            if (tick == 15 * seconds)
                break;
        }
    }
}

void ticks(uint8_t ticks)
{
    uint8_t clock;
    uint8_t tick = 0;

    while (1)
    {
        clock = PEEK(0x0008);
        PEEK(0x0009);

        if (clock & 0x20)
        {
            tick++;
            if (tick == ticks)
                break;
        }
    }
}

uint8_t wait()
{
    uint8_t clock;
    uint8_t tick = 0;
    uint8_t c;

    while (1)
    {
        clock = PEEK(0x0008);
        PEEK(0x0009);

        if (clock & 0x20)
        {
            tick++;
            if (tick >= timeout_ticks)
                return 0;
        }

        c = scankey();
        if (c != 0)
        {
            timeout_ticks -= tick;
            return c;
        }
    }
}

uint8_t wait_key()
{
    uint8_t c;

    while (1)
    {
        c = scankey();
        if (c != 0)
            return c;
    }
}

uint8_t platform_random()
{
    // 0x0009-0x000A contains the clock value in μs
    return PEEK(0x000A);
}

input_action_t platform_get_input()
{
    uint8_t c = wait();
    
    switch (c) {
        case 'O':
            return INPUT_MOVE_LEFT;
        case 'P':
            return INPUT_MOVE_RIGHT;
        case 'Z':
            return INPUT_ROTATE_CW;
        case 'A':
            return INPUT_ROTATE_CCW;
        case ' ':
            return INPUT_DROP;
        case 0:
            return INPUT_TIMEOUT;
        default:
            return INPUT_NONE;
    }
}

/************************************************************/
/* Display Sync API Implementation                         */
/************************************************************/

void display_sync_playfield(game_state_t* state)
{
    uint8_t x, y, cell_data, cell_content;
    
    for (y = 0; y < PLAYFIELD_HEIGHT; y++) {
        for (x = 0; x < PLAYFIELD_WIDTH; x++) {
            cell_data = state->playfield[y][x];
            
            // Only redraw cells that are marked dirty
            if (GET_CELL_DIRTY(cell_data)) {
                cell_content = GET_CELL_CONTENT(cell_data);
                
                if (cell_content != CELL_EMPTY) {
                    color(tetrominos_colors[cell_content - CELL_PIECE_1], black);
                    printc(PLAYFIELD_START_X + x, PLAYFIELD_START_Y + y, '\x7F');
                } else {
                    color(black, black);
                    printc(PLAYFIELD_START_X + x, PLAYFIELD_START_Y + y, ' ');
                }
                
                // Clear dirty flag after redraw
                CLEAR_CELL_DIRTY(state->playfield[y][x]);
            }
        }
    }
}


void display_sync_ui(game_state_t* state)
{
    char print_str[4];
    extern void int_to_string(uint8_t score, char *str);
    
    color(white, black);
    
    // Display score
    int_to_string(state->score, print_str);
    prints(UI_START_X, 3, print_str);
    
    // Display level
    int_to_string(state->level, print_str);
    prints(UI_START_X, 6, print_str);
}

void display_clear_screen()
{
    unsigned char x, y;
    color(white, black);
    for (y = 0; y < 25; y++)
    {
        for (x = 0; x < 40; x++)
        {
            printc(x, y, ' ');
        }
    }
}

void display_draw_borders()
{
    unsigned char y;
    
    // Print title
    color(yellow, black);
    prints(9, 0, "Tetris + Alice = TETRICE");

    // Title graphics
    color(yellow, black);
    printsg(0, 2, "\x6b\x41\x77\x41\x6b\x41\x66\x55\x55\x57\x41\x77\x41");
    printsg(0, 3, "\x4a\x40\x4d\x44\x4a\x40\x45\x45\x45\x4d\x44\x4d\x44");

    // Instructions
    color(pink, black);
    prints(2, 11, "O: LEFT");
    prints(2, 12, "P: RIGHT");
    prints(2, 13, "Z: ROTATE");
    prints(2, 14, "A: UNROTATE");
    prints(2, 15, "SPACE: DROP");

    // Draw playfield borders
    color(magenta, black);
    for (y = 0; y < 23; y++)
    {
        printcg(PLAYFIELD_START_X - 1, 2+y, '\x6A');
        printcg(PLAYFIELD_END_X + 1, 2+y, '\x55');
    }
    prints(PLAYFIELD_START_X - 1, 24, "\x42\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x41");
    prints(PLAYFIELD_START_X - 1, 1, "\x60\x70\x70\x70\x70\x70\x70\x70\x70\x70\x70\x70\x70\x50");

    // UI labels
    color(white, black);
    prints(UI_START_X, 2, "SCORE");
    prints(UI_START_X, 5, "LEVEL");
}

#endif // ALICE
