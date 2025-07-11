#include "platform.h"

#ifdef ALICE


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

#endif // ALICE
