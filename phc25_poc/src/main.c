/*
 * PHC-25 Proof of Concept Test Program
 *
 * This program tests basic graphics output on the PHC-25 by:
 * 1. Setting up Mode 11 (128x192, 4-color graphics)
 * 2. Drawing a simple test pattern to VRAM
 * 3. Running in an infinite loop
 */

#include <stdint.h>

/* PHC-25 Hardware Definitions */
#define VRAM_START 0x6000    /* Screen page 1 start address */
#define VRAM_SIZE  6144      /* 6KB video memory */
#define PORT_40    0x40      /* Graphics control port */

/* MC6847 Mode 11 settings (128x192, 4-color) */
#define MODE11_AG   0x80     /* A/G = 1 (graphics mode) */
#define MODE11_GM1  0x10     /* GM1 = 1 */
#define MODE11_GM0  0x00     /* GM0 = 0 */
#define MODE11_CSS  0x00     /* CSS = 0 (green/yellow/blue/red palette) */

/* Color values for Mode 11 (2 bits per pixel) */
#define COLOR_GREEN   0x00   /* 00 - background */
#define COLOR_YELLOW  0x01   /* 01 */
#define COLOR_BLUE    0x02   /* 10 */
#define COLOR_RED     0x03   /* 11 */

/* Memory access macros */
#define POKE(addr, value) (*((volatile uint8_t *)(addr)) = (value))
#define PEEK(addr) (*((volatile uint8_t *)(addr)))

/* I/O port access functions for Z80 */
void out_port(uint8_t port, uint8_t value);
uint8_t in_port(uint8_t port);

/* Function prototypes */
void init_graphics_mode11(void);
void draw_test_pattern(void);
void plot_pixel(uint8_t x, uint8_t y, uint8_t color);
void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);

/* Fast block-based drawing functions */
void draw_tetris_block(uint8_t block_x, uint8_t block_y, uint8_t color);
void erase_tetris_block(uint8_t block_x, uint8_t block_y);
void draw_tetris_piece_demo(void);

/*
 * Main function - entry point called from crt0.asm
 */
int main(void)
{
    /* Initialize graphics mode */
    init_graphics_mode11();

    /* Draw tetris piece demonstration */
    draw_tetris_piece_demo();

    /* Infinite loop to keep program running */
    while (1) {
        /* Do nothing - just display the tetris pieces */
    }

    return 0;
}

/*
 * Initialize MC6847 for Mode 11 (128x192, 4-color graphics)
 */
void init_graphics_mode11(void)
{
    uint8_t mode_byte;

    /* Configure MC6847 mode via port $40 */
    mode_byte = MODE11_AG | MODE11_GM1 | MODE11_GM0 | MODE11_CSS;
    out_port(PORT_40, mode_byte);

    /* Clear VRAM to background color (green) */
    {
        uint16_t i;
        for (i = 0; i < VRAM_SIZE; i++) {
            POKE(VRAM_START + i, 0x00);  /* All pixels = green */
        }
    }
}

/*
 * Draw a simple test pattern to verify graphics are working
 */
void draw_test_pattern(void)
{
    /* Draw colored rectangles to test each color */

    /* Yellow rectangle (top-left) */
    fill_rect(10, 10, 30, 20, COLOR_YELLOW);

    /* Blue rectangle (top-right) */
    fill_rect(70, 10, 30, 20, COLOR_BLUE);

    /* Red rectangle (bottom-left) */
    fill_rect(10, 50, 30, 20, COLOR_RED);

    /* Mixed color rectangle (bottom-right) - alternating pixels */
    {
        uint8_t x, y;
        for (y = 50; y < 70; y++) {
            for (x = 70; x < 100; x++) {
                uint8_t color = ((x + y) & 1) ? COLOR_YELLOW : COLOR_BLUE;
                plot_pixel(x, y, color);
            }
        }
    }

    /* Draw border around entire screen */
    /* Top and bottom borders */
    {
        uint8_t x;
        for (x = 0; x < 128; x++) {
            plot_pixel(x, 0, COLOR_RED);      /* Top border */
            plot_pixel(x, 191, COLOR_RED);    /* Bottom border */
        }
    }

    /* Left and right borders */
    {
        uint8_t y;
        for (y = 0; y < 192; y++) {
            plot_pixel(0, y, COLOR_RED);      /* Left border */
            plot_pixel(127, y, COLOR_RED);    /* Right border */
        }
    }
}

/* Lookup tables for optimized pixel operations */
static const uint8_t pixel_shift_table[4] = {6, 4, 2, 0};  /* Bit positions: 6,4,2,0 */
static const uint8_t pixel_mask_table[4] = {0x3F, 0xCF, 0xF3, 0xFC};  /* Inverted masks */

/*
 * Plot a single pixel at (x,y) with given color
 * Mode 11: 128x192, 2 bits per pixel, 4 pixels per byte
 * Optimized version using bit shifts and lookup tables
 */
void plot_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    uint16_t addr;
    uint8_t byte_val, pixel_pos, shift;

    /* Bounds checking */
    if (x >= 128 || y >= 192) return;

    /* Calculate VRAM address using bit shifts */
    /* Each row is 32 bytes: y * 32 = y << 5, x / 4 = x >> 2 */
    addr = VRAM_START + (y << 5) + (x >> 2);

    /* Determine pixel position within byte (0-3) and get shift amount */
    pixel_pos = x & 3;
    shift = pixel_shift_table[pixel_pos];

    /* Read current byte, clear target pixel bits, set new color */
    byte_val = PEEK(addr);
    byte_val = (byte_val & pixel_mask_table[pixel_pos]) | ((color & 0x03) << shift);
    POKE(addr, byte_val);
}

/*
 * Fill a rectangle with solid color
 */
void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    uint8_t dx, dy;

    for (dy = 0; dy < height; dy++) {
        for (dx = 0; dx < width; dx++) {
            plot_pixel(x + dx, y + dy, color);
        }
    }
}

/*
 * Z80 I/O port access functions using inline assembly
 * Parameters are passed on stack for z88dk
 */
void out_port(uint8_t port, uint8_t value)
{
    port; value; /* suppress unused parameter warnings */
    __asm
        pop hl      ; return address
        pop bc      ; value (in C)
        pop de      ; port (in E)
        push de     ; restore stack
        push bc
        push hl

        ld  a, c    ; value to A
        ld  c, e    ; port to C
        out (c), a  ; output to port
    __endasm;
}

uint8_t in_port(uint8_t port)
{
    port; /* suppress unused parameter warning */
    __asm
        pop hl      ; return address
        pop bc      ; port (in C)
        push bc     ; restore stack
        push hl

        ld  c, c    ; port already in C
        in  a, (c)  ; input from port
        ld  l, a    ; return value in L
        ld  h, 0    ; clear H for 16-bit return
    __endasm;
}

/*
 * Fast Tetris Block Drawing Functions
 * Optimized for character-aligned positions
 */

/* Block dimensions and layout */
#define BLOCK_WIDTH_CHARS   8     /* 8 pixels wide (aligned to 4-pixel boundaries) */
#define BLOCK_HEIGHT_CHARS  8     /* 8 pixels tall */
#define PLAYFIELD_BLOCK_START_X 8 /* Starting X position in pixels (aligned) */
#define PLAYFIELD_BLOCK_START_Y 8 /* Starting Y position in pixels */

/*
 * Draw a tetris block at character position (block_x, block_y)
 * Uses optimized horizontal line drawing for speed
 */
void draw_tetris_block(uint8_t block_x, uint8_t block_y, uint8_t color)
{
    uint16_t start_addr;
    uint8_t pixel_x, pixel_y;
    uint8_t row;
    uint8_t color_byte;

    /* Calculate starting pixel position */
    pixel_x = PLAYFIELD_BLOCK_START_X + (block_x << 3);  /* block_x * 8 */
    pixel_y = PLAYFIELD_BLOCK_START_Y + (block_y << 3);  /* block_y * 8 */

    /* Bounds check */
    if (pixel_x + BLOCK_WIDTH_CHARS > 128 || pixel_y + BLOCK_HEIGHT_CHARS > 192) return;

    /* Create color byte pattern (4 pixels of same color) */
    color &= 0x03;
    color_byte = color | (color << 2) | (color << 4) | (color << 6);

    /* Draw block using horizontal lines - all blocks are 4-pixel aligned */
    for (row = 0; row < BLOCK_HEIGHT_CHARS; row++) {
        start_addr = VRAM_START + ((pixel_y + row) << 5) + (pixel_x >> 2);

        /* 8 pixels = exactly 2 bytes, perfectly aligned */
        POKE(start_addr, color_byte);     /* First 4 pixels */
        POKE(start_addr + 1, color_byte); /* Next 4 pixels */
    }
}

/*
 * Erase a tetris block (fill with background color)
 */
void erase_tetris_block(uint8_t block_x, uint8_t block_y)
{
    draw_tetris_block(block_x, block_y, COLOR_GREEN); /* Green = background */
}

/*
 * Demonstrate tetris pieces using the block drawing system
 */
void draw_tetris_piece_demo(void)
{
    /* Clear playfield area first */
    fill_rect(PLAYFIELD_BLOCK_START_X, PLAYFIELD_BLOCK_START_Y,
              15 * BLOCK_WIDTH_CHARS, 22 * BLOCK_HEIGHT_CHARS, COLOR_BLUE);

    /* Draw I-piece (vertical) in yellow */
    draw_tetris_block(1, 1, COLOR_YELLOW);
    draw_tetris_block(1, 2, COLOR_YELLOW);
    draw_tetris_block(1, 3, COLOR_YELLOW);
    draw_tetris_block(1, 4, COLOR_YELLOW);

    /* Draw T-piece in blue */
    draw_tetris_block(4, 2, COLOR_BLUE);
    draw_tetris_block(3, 3, COLOR_BLUE);
    draw_tetris_block(4, 3, COLOR_BLUE);
    draw_tetris_block(5, 3, COLOR_BLUE);

    /* Draw L-piece in red */
    draw_tetris_block(7, 1, COLOR_RED);
    draw_tetris_block(7, 2, COLOR_RED);
    draw_tetris_block(7, 3, COLOR_RED);
    draw_tetris_block(8, 3, COLOR_RED);

    /* Draw O-piece (square) in yellow */
    draw_tetris_block(10, 1, COLOR_YELLOW);
    draw_tetris_block(11, 1, COLOR_YELLOW);
    draw_tetris_block(10, 2, COLOR_YELLOW);
    draw_tetris_block(11, 2, COLOR_YELLOW);

    /* Draw bottom row of blocks to simulate filled line */
    {
        uint8_t i;
        for (i = 0; i < 15; i++) {
            /* Alternate colors for visual effect */
            uint8_t color = (i & 1) ? COLOR_BLUE : COLOR_RED;
            draw_tetris_block(i, 20, color);
        }
    }
}