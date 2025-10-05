#ifndef ALICE_H
#define ALICE_H

/************************************************************/
/* Alice 32/90                                              */
/************************************************************/

// EF9345 registers
#define R0 0xBF20     // Command
#define R1 0xBF21     // Argument
#define R2 0xBF22     // Argument
#define R3 0xBF23     // Argument
#define R4 0xBF24     // Auxiliary Pointer (AP) high
#define R5 0xBF25     // Auxiliary Pointer (AP) low
#define R6 0xBF26     // Main Pointer (MP) high
#define R7 0xBF27     // Main Pointer (MP) low
#define R0EXEC 0xBF28 // Execute

#define POKE(addr, value) (*((volatile uint8_t *)(addr)) = (value))
#define PEEK(address) (*((volatile uint8_t *)(address)))
#define BUSY() while (PEEK(R0) & 0x80) {}

// Color codes
#define black 0
#define red 1
#define green 2
#define orange 3
#define blue 4
#define magenta 5
#define cyan 6
#define pink 7
#define lgreen 10
#define yellow 11
#define lmagenta 13
#define white 15

// This is the key matrix, based on the table in "Les Astuces d'Alice" p. 18.
// The table below is organized by columns, and each column is a list of keys.
// The first 7 elements of each row are the keys, the last one is a 0 sentinel.
// NB: the position for the 'X' is wrong in the book, it's fixed here.
extern uint8_t keys_per_column[8][8];

// This is the list of columns to scan, in order.
extern uint8_t all_key_columns[8];

// Screen characteristics for Alice 32/90
#define SCREEN_WIDTH_CHARS 40
#define SCREEN_HEIGHT_CHARS 25

// Playfield dimensions
#define PLAYFIELD_WIDTH 12
#define PLAYFIELD_HEIGHT 22

// Display layout configuration (screen coordinates)
#define PLAYFIELD_START_X 15       /* Screen column where playfield starts */
#define PLAYFIELD_START_Y 2        /* Screen row where playfield starts */
#define UI_START_X 29              /* Score/Level display position */

// Piece starting position (playfield coordinates)
#define PIECE_START_X 5            /* Starting X in playfield coords (0-11) */
#define PIECE_START_Y 0            /* Starting Y in playfield coords (0-21) */

// Derived constants  
#define PLAYFIELD_END_X (PLAYFIELD_START_X + PLAYFIELD_WIDTH - 1)

#endif // ALICE_H
