#include <stdint.h>
#include <stdio.h>

#include "alice.h"
#include "tetromino.h"

// Colors for each tetromino
char tetrominos_colors[] = {
    yellow, cyan, pink, green, red, blue, orange};

/************************************************************/
/* Macros                                                   */
/************************************************************/

#define POKE(addr, value) (*((volatile uint8_t *)(addr)) = (value))

#define PEEK(address) (*((volatile uint8_t *)(address)))

// Wait for EF9345
#define BUSY()                \
    while (PEEK(0xBF20) <= 0) \
    {                         \
    }

/************************************************************/
/* Display functions for Alice 32/90                        */
/************************************************************/

// Set position registers
void posxy(unsigned char column, unsigned char line)
{
    // Lines 1 to 7 do not exist... skip them
    if (line > 0)
        line += 7;

    POKE(R6, line);   // R6 (MP / LINE)
    POKE(R7, column); // R7 (MP / COL)
}

// Set color registers
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
    POKE(R2, b); // R2 (B)
    POKE(R3, a); // R3 (A) color
}

// Display a string with position
void prints(unsigned char x, unsigned char y, unsigned char *text)
{
    char *c = text;
    posxy(x, y);
    for (; *c != '\0'; c++)
    {
        POKE(R1, *c);
        POKE(R0EXEC, 1);
    }
}

// Display a char with position
void printc(unsigned char x, unsigned char y, unsigned char c)
{
    posxy(x, y);
    POKE(R1, c);
    POKE(R0EXEC, 1);
}

/************************************************************/
/* Keyboard and clock                                       */
/************************************************************/

// Scan the keyboard and return the key pressed, or 0 if no key is pressed.
uint8_t scankey()
{
    uint8_t key;
    uint8_t col;
    uint8_t rank;

    for (col = 0; col < 8; col++)
    {
        // Column to scan
        POKE(0x0002, all_key_columns[col]);
        // Get the result (row)
        key = PEEK(0xBFFF);

        // Nothing pressed, go to next column
        if (key == 0xFF)
            continue;

        // Invert the bits
        key = ~key;

        // Find the rank of the "on" bit
        rank = 0;
        while ((key & 0x01) == 0)
        {
            key = key >> 1;
            rank++;
        }

        // Return the key
        return keys_per_column[col][rank];
    }

    // No key found
    return 0;
}

// Sleep for a number of seconds
void sleep(uint8_t seconds)
{
    uint8_t clock;
    uint8_t tick = 0;

    while (1)
    {
        clock = PEEK(0x0008); // control register
        // wait for clock bit 5 to be set
        if (clock & 0x20)
        {
            tick++;
            // 15 ticks == 1 second
            if (tick == 15 * seconds)
                break;
        }
    }
}

// Sleep for a number of ticks
void ticks(uint8_t ticks)
{
    uint8_t clock;
    uint8_t tick = 0;

    while (1)
    {
        clock = PEEK(0x0008); // control register
        // wait for clock bit 5 to be set
        if (clock & 0x20)
        {
            tick++;
            if (tick == ticks)
                break;
        }
    }
}

// Wait for key or timeout in ticks
// Remember: 15 ticks == 1 second

// Global variable to store the timeout in ticks
// this needs to be set before calling wait()
uint8_t timeout_ticks = 0;

uint8_t wait()
{
    uint8_t clock;
    uint8_t tick = 0;
    uint8_t c;

    while (1)
    {
        clock = PEEK(0x0008); // control register
        // wait for clock bit 5 to be set
        if (clock & 0x20)
        {
            tick++;
            // if timeout is reached, return 0
            if (tick >= timeout_ticks)
                return 0;
        }
        // scan the keyboard
        c = scankey();
        if (c != 0)
        {
            // if a key is pressed, decrement the timeout
            timeout_ticks -= tick;
            // and return the key
            return c;
        }
    }
}

/************************************************************/
/* Pieces                                                   */
/************************************************************/

// Display a piece
void display_piece(unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    unsigned char i;
    color(tetrominos_colors[piece], black);
    for (i = 0; i < 4; i++)
    {
        printc(x + (*tetromino)[i][0], y + (*tetromino)[i][1], '\x7F');
    }
}

// Erase a piece
void erase_piece(unsigned char piece, unsigned char x, unsigned char y, unsigned char rotation)
{
    tetromino *tetromino = tetrominos[piece][rotation];
    unsigned char i;
    color(black, black);
    for (i = 0; i < 4; i++)
    {
        printc(x + (*tetromino)[i][0], y + (*tetromino)[i][1], ' ');
    }
}

/************************************************************/
/* Proto loop                                                */
/************************************************************/

void protoloop()
{
    unsigned char x, y;
    unsigned char i;
    char c;
    char cur_shape[] = {0, 0, 0, 0, 0, 0, 0};

    // Print pieces with all shapes
    y = 2;
    x = 0;
    while (1)
    {
        for (i = 0; i < 7; i++)
        {
            display_piece(i, x + i * 5, y, cur_shape[i]);
        }
        // Wait for a key
        timeout_ticks = 15;
        c = wait();
        if (c == 'X')
            return;
        // Erase pieces
        for (i = 0; i < 7; i++)
        {
            erase_piece(i, x + i * 5, y, cur_shape[i]);
            // Increment shape index
            ++cur_shape[i];
            if (cur_shape[i] == tetrominos_nb_shapes[i])
            {
                cur_shape[i] = 0;
            }
        }
    }
}

/************************************************************/
/* Game loop                                                */
/************************************************************/

#define START_X 15
#define START_Y 3
#define BOUNDS_X1 15
#define BOUNDS_X2 25

void gameloop()
{
    // Select a random piece
    // 0x0009-0x000A contains the clock value in μs
    unsigned char piece = PEEK(0x000A) % 7;
    // Set start position to 0,0
    unsigned char x = START_X;
    unsigned char y = START_Y;
    // Set start rotation to 0
    unsigned char rotation = 0;
    // Previous values
    unsigned char px = START_X, py = START_Y, protation = 0;
    // Set start speed
    unsigned char speed = 20;
    // Set start score to 0
    unsigned char score = 0;
    // Input key
    unsigned char c;

    // Set initial timer
    timeout_ticks = speed;

    // Loop until game over
    while (1)
    {
        // Erase piece
        erase_piece(piece, px, py, protation);

        // Display piece
        display_piece(piece, x, y, rotation);

        // Keep previous position
        px = x;
        py = y;
        protation = rotation;

        // Wait for a key
        c = wait();

        // Piece falls
        if (c == 0 || c == ' ')
        {
            y++;
            timeout_ticks = speed;
        }
        else
        {
            // anti-bounce
            ticks(2);
        }

        // Piece has reached the bottom
        if (y > 20)
        {
            // Reset position
            x = START_X;
            y = START_Y;
            rotation = 0;
            px = x;
            py = y;
            protation = rotation;
            // Select a random piece
            piece = PEEK(0x000A) % 7;
            // Set initial timer
            timeout_ticks = speed;
            // Continue loop
            continue;
        }

        // Move piece
        switch (c)
        {
        case 'X':
            return;
        case 'O':
            if (x > BOUNDS_X1)
                x--;
            break;
        case 'P':
            if (x < BOUNDS_X2)
                x++;
            break;
        case 'Z':
            if (rotation < tetrominos_nb_shapes[piece] - 1)
                rotation++;
            else
                rotation = 0;
            break;
        case 'A':
            if (rotation > 0)
                rotation--;
            else
                rotation = tetrominos_nb_shapes[piece] - 1;
            break;
        }
    }
}

/************************************************************/
/* Main loop                                                */
/************************************************************/

void main()
{
    unsigned char y;

    // Clear screen
    color(white, black);
    for (y = 0; y < 25; y++)
    {
        prints(0, y, "                                        ");
    }

    // Print message
    color(yellow, black);
    prints(7, 0, "Tetris + Alice = TETRICE");

    // Call proto loop
    gameloop();
    // protoloop();
}
