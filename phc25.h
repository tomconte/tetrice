#ifndef PHC25_H
#define PHC25_H

#include <stdint.h>

/************************************************************/
/* Thomson PHC-25 platform definitions                      */
/*                                                          */
/* TODO: Fill in real hardware addresses and color values.  */
/************************************************************/

/* Memory access macros (update addresses if needed) */
#define POKE(addr, value) (*((volatile uint8_t *)(addr)) = (value))
#define PEEK(addr) (*((volatile uint8_t *)(addr)))

/* Basic color codes - placeholder values */
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

/* Screen characteristics for PHC-25 (placeholder values) */
/* TODO: Update these based on actual PHC-25 display capabilities */
#define SCREEN_WIDTH_CHARS 32      /* Assuming smaller character display */
#define SCREEN_HEIGHT_CHARS 24     /* Or could be pixel-based measurements */

/* Playfield dimensions - same logical size as Alice */
#define PLAYFIELD_WIDTH 12
#define PLAYFIELD_HEIGHT 22

/* Display layout configuration for PHC-25 (placeholder values) */
/* TODO: Adjust these for optimal PHC-25 bitmap display layout */
#define PLAYFIELD_START_X 8        /* Centered for narrower screen */
#define PLAYFIELD_START_Y 1        /* Start near top */
#define UI_START_X 22              /* Score/Level display position */
#define PIECE_START_X 14           /* Starting X position for new pieces (center) */
#define PIECE_START_Y 1            /* Starting Y position for new pieces */

/* Derived constants */
#define PLAYFIELD_END_X (PLAYFIELD_START_X + PLAYFIELD_WIDTH - 1)

/* Bitmap display constants (if using pixel-based rendering) */
/* TODO: Define block size, colors, and drawing functions */
#define BLOCK_WIDTH_PIXELS 8       /* Width of each tetris block in pixels */
#define BLOCK_HEIGHT_PIXELS 8      /* Height of each tetris block in pixels */

#endif // PHC25_H
