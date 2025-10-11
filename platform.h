#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#ifdef ALICE
#include "alice.h"
#endif
#ifdef PHC25
#include "phc25.h"
#endif

/* Forward declaration for game_state_t */
struct game_state_t;

/* Timing functions */
void sleep(uint8_t seconds);
void ticks(uint8_t ticks);

/* Input functions */
extern uint8_t timeout_ticks;
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

/* Display functions */
void display_sync_playfield(struct game_state_t* state);
void display_sync_ui(struct game_state_t* state);
void display_preview_piece(uint8_t piece);
void display_clear_screen();
void display_draw_borders();
void display_game_over();

/* Internal platform functions (used by platform implementations) */
uint8_t scankey();
uint8_t wait();

#endif // PLATFORM_H
