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
#define CELL_PIECE_1  1  // O-piece (yellow)
#define CELL_PIECE_2  2  // I-piece (cyan) 
#define CELL_PIECE_3  3  // T-piece (pink)
#define CELL_PIECE_4  4  // S-piece (green)
#define CELL_PIECE_5  5  // Z-piece (red)
#define CELL_PIECE_6  6  // J-piece (blue)
#define CELL_PIECE_7  7  // L-piece (orange)

// Bit-packed playfield cell layout (zero additional memory)
// Bits 0-2: Cell content (CELL_EMPTY, CELL_PIECE_1-7)
// Bit 7: Dirty flag (1 = needs redraw, 0 = clean)
// Bits 3-6: Reserved for future use
#define CELL_CONTENT_MASK 0x07  // Bits 0-2: piece type
#define CELL_DIRTY_FLAG   0x80  // Bit 7: dirty marker

// Bit manipulation macros for playfield cells
#define GET_CELL_CONTENT(cell) ((cell) & CELL_CONTENT_MASK)
#define GET_CELL_DIRTY(cell)   ((cell) & CELL_DIRTY_FLAG)
#define SET_CELL_DIRTY(cell)   ((cell) |= CELL_DIRTY_FLAG)
#define CLEAR_CELL_DIRTY(cell) ((cell) &= ~CELL_DIRTY_FLAG)
#define SET_CELL_CONTENT(cell, content) \
    ((cell) = ((cell) & ~CELL_CONTENT_MASK) | ((content) & CELL_CONTENT_MASK))
#define SET_CELL_CONTENT_AND_DIRTY(cell, content) \
    ((cell) = ((content) & CELL_CONTENT_MASK) | CELL_DIRTY_FLAG)

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