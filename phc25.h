#ifndef PHC25_H
#define PHC25_H

#include <stdint.h>

/************************************************************/
/* Thomson PHC-25 platform definitions                      */
/* Based on successful POC using Mode 12 (256x192 mono)    */
/************************************************************/

/* Memory access macros */
#define POKE(addr, value) (*((volatile uint8_t *)(addr)) = (value))
#define PEEK(addr) (*((volatile uint8_t *)(addr)))

/* PHC-25 Hardware Definitions */
#define VRAM_START 0x6000    /* Screen page 1 start address */
#define VRAM_SIZE  6144      /* 6KB video memory */
#define PORT_40    0x40      /* Graphics control port */

/* MC6847 Mode 12 settings (256x192 monochrome) */
#define MODE12_AG   0x80     /* A/G = 1 (graphics mode) */
#define MODE12_GM1  0x10     /* GM1 = 1 */
#define MODE12_GM0  0x20     /* GM0 = 1 */
#define MODE12_CSS  0x00     /* CSS = 0 (black/green) or 1 (black/white) */

/* Keyboard matrix ports */
#define KEYBOARD_PORT_BASE 0x80  /* Base port for 8x8 keyboard matrix */

/* Color definitions (monochrome - using patterns/intensities) */
#define black 0
#define green 1     /* Foreground color in Mode 12 */
#define white 1     /* Same as green in monochrome */
#define red 1       /* All non-black map to foreground */
#define orange 1
#define blue 1
#define magenta 1
#define cyan 1
#define pink 1
#define lgreen 1
#define yellow 1
#define lmagenta 1

/* Mode 12 Screen Layout (256x192 pixels) */
#define SCREEN_WIDTH_PIXELS 256
#define SCREEN_HEIGHT_PIXELS 192
#define PIXELS_PER_BYTE 8        /* 1 bit per pixel in Mode 12 */
#define BYTES_PER_ROW 32         /* 256 pixels / 8 pixels per byte */

/* Tetris Layout Constants */
#define BLOCK_SIZE 8                    /* 8x8 pixel blocks */
#define PLAYFIELD_WIDTH 10              /* Official Tetris width */
#define PLAYFIELD_HEIGHT 22             /* Extended height: 22 rows */

/* Optimized full-screen layout (256x192 pixels) - no wasted margins */
#define PLAYFIELD_START_X 88            /* Center playfield: (256-80)/2 = 88 */
#define PLAYFIELD_START_Y 8             /* Start after title: 8 pixels */
#define PLAYFIELD_PIXEL_WIDTH  (PLAYFIELD_WIDTH * BLOCK_SIZE)   /* 80 pixels */
#define PLAYFIELD_PIXEL_HEIGHT (PLAYFIELD_HEIGHT * BLOCK_SIZE)  /* 176 pixels */

/* UI Areas - full width utilization */
#define UI_LEFT_X 0                     /* Left UI area starts at left edge */
#define UI_RIGHT_X 168                  /* Right UI area (after playfield) */
#define UI_WIDTH 88                     /* UI area width (expanded from 72) */
#define UI_START_X 22                   /* Character-based UI start for compatibility */

/* Piece starting position (playfield coordinates) */
#define PIECE_START_X 5                 /* Starting X in playfield coords (0-9) */
#define PIECE_START_Y 1                 /* Starting Y in playfield coords (0-21) */

/* Derived constants */
#define PLAYFIELD_END_X (PLAYFIELD_START_X + PLAYFIELD_PIXEL_WIDTH - 1)

/* I/O port access functions for Z80 */
void out_port(uint8_t port, uint8_t value);
uint8_t in_port(uint8_t port);

/* Low-level graphics functions */
void init_graphics_mode12(void);
void clear_screen(void);
void draw_tetris_block(uint8_t block_x, uint8_t block_y, uint8_t filled);
void draw_tetris_block_outline(uint8_t block_x, uint8_t block_y);
void erase_tetris_block(uint8_t block_x, uint8_t block_y);
void draw_horizontal_line(uint8_t x1, uint8_t x2, uint8_t y);
void draw_vertical_line(uint8_t x, uint8_t y1, uint8_t y2);

/* Debug functions */
void debug_print(uint8_t x, uint8_t y, char* text);
void debug_print_hex(uint8_t x, uint8_t y, uint8_t value);
void debug_draw_char(uint8_t x, uint8_t y, char c);

#endif // PHC25_H
