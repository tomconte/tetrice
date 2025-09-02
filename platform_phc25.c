#include "platform.h"

#ifdef PHC25

/************************************************************/
/* Display functions for Thomson PHC-25                     */
/*                                                          */
/* TODO: Implement these using the PHC-25 hardware specs.   */
/************************************************************/

void posxy(unsigned char column, unsigned char line)
{
    /* TODO: set cursor position */
}

void color(unsigned char foreground, unsigned char background)
{
    /* TODO: set foreground/background colors */
}

void prints(unsigned char x, unsigned char y, unsigned char *text)
{
    /* TODO: print a null terminated string at given coords */
}

void printsg(unsigned char x, unsigned char y, unsigned char *text)
{
    /* TODO: print string using graphic characters */
}

void printc(unsigned char x, unsigned char y, unsigned char c)
{
    /* TODO: print a single character at given coords */
}

void printcg(unsigned char x, unsigned char y, unsigned char c)
{
    /* TODO: print a graphic character at given coords */
}

uint8_t charatxy(uint8_t column, uint8_t line)
{
    /* TODO: return character at screen position */
    return 0;
}

/************************************************************/
/* Keyboard and clock                                       */
/************************************************************/

uint8_t scankey()
{
    /* TODO: scan keyboard matrix */
    return 0;
}

void sleep(uint8_t seconds)
{
    /* TODO: busy-wait or use timer to sleep */
}

void ticks(uint8_t ticks)
{
    /* TODO: wait for given number of timer ticks */
}

uint8_t wait()
{
    /* TODO: wait for keypress or timeout based on timeout_ticks */
    return 0;
}

uint8_t wait_key()
{
    /* TODO: wait until a key is pressed */
    return 0;
}

uint8_t platform_random()
{
    /* TODO: return a random number */
    return 0;
}

#endif // PHC25
