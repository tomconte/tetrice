#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdint.h>

#ifdef ALICE
#include "alice.h"
#endif
#ifdef PHC25
#include "phc25.h"
#endif

// Abstract cell type constants - platform agnostic
#define CELL_EMPTY    0
#define CELL_PIECE_1  1  // I-piece (cyan)
#define CELL_PIECE_2  2  // O-piece (yellow) 
#define CELL_PIECE_3  3  // T-piece (pink)
#define CELL_PIECE_4  4  // S-piece (green)
#define CELL_PIECE_5  5  // Z-piece (red)
#define CELL_PIECE_6  6  // J-piece (blue)
#define CELL_PIECE_7  7  // L-piece (orange)

// Game state structure
typedef struct {
    uint8_t playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];
    uint8_t score;
    uint8_t level;
    uint8_t speed;
    uint8_t piece;
    uint8_t x, y;
    uint8_t rotation;
} game_state_t;

/* Display Sync API - declared here since they need the complete game_state_t definition */
void display_sync_playfield(game_state_t* state);
void display_sync_ui(game_state_t* state);
void display_clear_screen();
void display_draw_borders();

#endif // GAME_STATE_H