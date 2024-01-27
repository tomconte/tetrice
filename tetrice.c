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
            {
                tick = 0;
                break;
            }
        }
    }
}

// Wait for key or timeout
uint8_t wait(uint8_t seconds)
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
            // 15 ticks == 1 second
            if (tick == 15 * seconds)
            {
                tick = 0;
                return 0;
            }
        }
        // scan the keyboard
        POKE(0x0002, 0xBF); // scan keyboard column 6
        c = PEEK(0xBFFF);   // read column
        c = ~c; // invert bits
        if (c == 0x08) // bit 3 = enter key
            return 'X';
    }
}

/************************************************************/
/* Proto loop                                                */
/************************************************************/

void protoloop()
{
    unsigned char x, y;
    unsigned char i, j, k;
    char c;
    tetromino *piece;
    char cur_shape[] = {0, 0, 0, 0, 0, 0, 0};

    // Print pieces with all shapes
    j = 0;
    while (1)
    {
        for (y = 1; y < 6; y++)
        {
            prints(0, y, "                                        ");
        }
        y = 2;
        x = 0;
        for (i = 0; i < 7; i++)
        {
            piece = tetrominos[i][cur_shape[i]];
            color(tetrominos_colors[i], black);
            // Display the 4 blocks
            for (k = 0; k < 4; k++)
            {
                printc(x + (*piece)[k][0], y + (*piece)[k][1], '\x7F');
            }
            // Next column
            x += 5;
            // If over, next line
            if (x > 39)
            {
                x = 0;
                y += 5;
            }
            // Increment shape index
            for (j = 0; j < 7; j++)
            {
                ++cur_shape[j];
                if (cur_shape[j] == tetrominos_nb_shapes[j])
                {
                    cur_shape[j] = 0;
                }
            }
        }
        // Wait for a key
        c = wait(1);
        if (c == 'X')
            return;
    }
}

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
/* Game loop                                                */
/************************************************************/

void gameloop()
{
    // Select a random piece
    // 0x0009-0x000A contains the clock value in μs
    unsigned char piece = PEEK(0x000A) % 7;
    // Set start position to 0,0
    unsigned char x = 0;
    unsigned char y = 0;
    // Set start rotation to 0
    unsigned char rotation = 0;
    // Set start speed to 1
    unsigned char speed = 1;
    // Set start score to 0
    unsigned char score = 0;
    // Input key
    unsigned char c;

    // Loop until game over
    while (1)
    {
        // Display piece
        display_piece(piece, x, y, rotation);
        // Wait for a key
        c = getchar();
        // Erase piece
        erase_piece(piece, x, y, rotation);
        // Move piece
        switch (c)
        {
        case 'X':
            return;
        case 'L':
            if (x > 0)
                x--;
            break;
        case 'R':
            if (x < 9)
                x++;
            break;
        case 'D':
            if (y < 19)
                y++;
            break;
        case 'U':
            if (y > 0)
                y--;
            break;
        case 'A':
            if (rotation < tetrominos_nb_shapes[piece] - 1)
                rotation++;
            else
                rotation = 0;
            break;
        case 'B':
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
    //gameloop();
    protoloop();
}
