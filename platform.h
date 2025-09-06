#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#ifdef ALICE
#include "alice.h"
#endif
#ifdef PHC25
#include "phc25.h"
#endif

void posxy(unsigned char column, unsigned char line);
void color(unsigned char foreground, unsigned char background);
void prints(unsigned char x, unsigned char y, unsigned char *text);
void printsg(unsigned char x, unsigned char y, unsigned char *text);
void printc(unsigned char x, unsigned char y, unsigned char c);
void printcg(unsigned char x, unsigned char y, unsigned char c);
uint8_t charatxy(uint8_t column, uint8_t line);

uint8_t scankey();
void sleep(uint8_t seconds);
void ticks(uint8_t ticks);

extern uint8_t timeout_ticks;
uint8_t wait();
uint8_t wait_key();

uint8_t platform_random();

typedef enum {
    INPUT_NONE = 0,
    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_ROTATE_CW,
    INPUT_ROTATE_CCW,
    INPUT_DROP,
    INPUT_TIMEOUT
} input_action_t;

input_action_t platform_get_input();

#endif // PLATFORM_H
