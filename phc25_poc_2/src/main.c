/*
 * PHC-25 Mode 12 Tetris POC
 *
 * This program demonstrates proper Tetris block rendering using:
 * - Mode 12 (256x192 monochrome graphics)
 * - 8x8 pixel blocks for crisp appearance
 * - Official 10x20 Tetris playfield
 * - Decorative borders and UI elements
 */

#include <stdint.h>

/* PHC-25 Hardware Definitions */
#define VRAM_START 0x6000    /* Screen page 1 start address */
#define VRAM_SIZE  6144      /* 6KB video memory */
#define PORT_40    0x40      /* Graphics control port */

/* MC6847 Mode 12 settings (256x192 monochrome) */
#define MODE12_AG   0x80     /* A/G = 1 (graphics mode) */
#define MODE12_GM1  0x10     /* GM1 = 1 */
#define MODE12_GM0  0x20     /* GM0 = 1 */
#define MODE12_CSS  0x00     /* CSS = 0 (black/green) or 1 (black/white) */

/* Memory access macros */
#define POKE(addr, value) (*((volatile uint8_t *)(addr)) = (value))
#define PEEK(addr) (*((volatile uint8_t *)(addr)))

/* I/O port access functions for Z80 */
void out_port(uint8_t port, uint8_t value);
uint8_t in_port(uint8_t port);

/* Tetris Layout Constants */
#define BLOCK_SIZE 8                    /* 8x8 pixel blocks */
#define PLAYFIELD_WIDTH 10              /* Official Tetris width */
#define PLAYFIELD_HEIGHT 20             /* Official Tetris height */

/* Screen layout (256x192 pixels) */
#define PLAYFIELD_START_X 88            /* Center playfield: (256-80)/2 = 88 */
#define PLAYFIELD_START_Y 16            /* Top margin: 16 pixels */
#define PLAYFIELD_PIXEL_WIDTH  (PLAYFIELD_WIDTH * BLOCK_SIZE)   /* 80 pixels */
#define PLAYFIELD_PIXEL_HEIGHT (PLAYFIELD_HEIGHT * BLOCK_SIZE)  /* 160 pixels */

/* UI Areas */
#define UI_LEFT_X 8                     /* Left UI area */
#define UI_RIGHT_X 176                  /* Right UI area (after playfield) */
#define UI_WIDTH 72                     /* UI area width */

/* Function prototypes */
void init_graphics_mode12(void);
void draw_tetris_block(uint8_t block_x, uint8_t block_y, uint8_t filled);
void draw_tetris_block_outline(uint8_t block_x, uint8_t block_y);
void erase_tetris_block(uint8_t block_x, uint8_t block_y);
void draw_playfield_border(void);
void draw_tetris_demo(void);
void draw_horizontal_line(uint8_t x1, uint8_t x2, uint8_t y);
void draw_vertical_line(uint8_t x, uint8_t y1, uint8_t y2);
void clear_screen(void);

/*
 * Main function - entry point called from crt0.asm
 */
int main(void)
{
    /* Initialize graphics mode */
    init_graphics_mode12();

    /* Draw the demo */
    draw_tetris_demo();

    /* Infinite loop to keep program running */
    while (1) {
        /* Do nothing - just display the demo */
    }

    return 0;
}

/*
 * Initialize MC6847 for Mode 12 (256x192 monochrome graphics)
 */
void init_graphics_mode12(void)
{
    uint8_t mode_byte;

    /* Configure MC6847 mode via port $40 */
    mode_byte = MODE12_AG | MODE12_GM1 | MODE12_GM0 | MODE12_CSS;
    out_port(PORT_40, mode_byte);

    /* Clear VRAM to background (all pixels off) */
    clear_screen();
}

/*
 * Clear entire screen to background color
 */
void clear_screen(void)
{
    uint16_t i;
    for (i = 0; i < VRAM_SIZE; i++) {
        POKE(VRAM_START + i, 0x00);  /* All pixels off (black) */
    }
}

/*
 * Draw a tetris block at grid position (block_x, block_y)
 * Mode 12: 256x192, 1 bit per pixel, 8 pixels per byte
 */
void draw_tetris_block(uint8_t block_x, uint8_t block_y, uint8_t filled)
{
    uint16_t start_addr;
    uint8_t pixel_x, pixel_y;
    uint8_t row;
    uint8_t fill_byte;

    /* Calculate starting pixel position */
    pixel_x = PLAYFIELD_START_X + (block_x << 3);  /* block_x * 8 */
    pixel_y = PLAYFIELD_START_Y + (block_y << 3);  /* block_y * 8 */

    /* Bounds check */
    if (block_x >= PLAYFIELD_WIDTH || block_y >= PLAYFIELD_HEIGHT) return;

    /* Fill pattern: 0x00 = empty (black), 0xFF = filled (green/white) */
    fill_byte = filled ? 0xFF : 0x00;

    /* Draw 8x8 block - optimized for Mode 12 */
    for (row = 0; row < BLOCK_SIZE; row++) {
        /* Calculate VRAM address for this row */
        /* Each row is 32 bytes (256 pixels / 8 pixels per byte) */
        start_addr = VRAM_START + ((pixel_y + row) << 5) + (pixel_x >> 3);

        /* 8 pixels = exactly 1 byte in Mode 12 */
        POKE(start_addr, fill_byte);
    }
}

/*
 * Draw a tetris block with outline pattern (border only, hollow inside)
 * Creates a 1-pixel border with empty center for better visual separation
 */
void draw_tetris_block_outline(uint8_t block_x, uint8_t block_y)
{
    uint16_t start_addr;
    uint8_t pixel_x, pixel_y;
    uint8_t row;

    /* Calculate starting pixel position */
    pixel_x = PLAYFIELD_START_X + (block_x << 3);  /* block_x * 8 */
    pixel_y = PLAYFIELD_START_Y + (block_y << 3);  /* block_y * 8 */

    /* Bounds check */
    if (block_x >= PLAYFIELD_WIDTH || block_y >= PLAYFIELD_HEIGHT) return;

    /* Draw 8x8 outline pattern */
    for (row = 0; row < BLOCK_SIZE; row++) {
        /* Calculate VRAM address for this row */
        start_addr = VRAM_START + ((pixel_y + row) << 5) + (pixel_x >> 3);

        /* Create outline pattern based on row position */
        if (row == 0 || row == 7) {
            /* Top and bottom rows: full outline */
            POKE(start_addr, 0xFF);  /* 11111111 */
        } else {
            /* Middle rows: only left and right edges */
            POKE(start_addr, 0x81);  /* 10000001 */
        }
    }
}

/*
 * Erase a tetris block (set to background)
 */
void erase_tetris_block(uint8_t block_x, uint8_t block_y)
{
    draw_tetris_block(block_x, block_y, 0); /* 0 = empty/black */
}

/*
 * Draw horizontal line from x1 to x2 at y
 */
void draw_horizontal_line(uint8_t x1, uint8_t x2, uint8_t y)
{
    uint8_t x;
    for (x = x1; x <= x2; x++) {
        uint16_t addr = VRAM_START + (y << 5) + (x >> 3);
        uint8_t bit_pos = 7 - (x & 7);
        uint8_t byte_val = PEEK(addr);
        byte_val |= (1 << bit_pos);
        POKE(addr, byte_val);
    }
}

/*
 * Draw vertical line from y1 to y2 at x
 */
void draw_vertical_line(uint8_t x, uint8_t y1, uint8_t y2)
{
    uint8_t y;
    for (y = y1; y <= y2; y++) {
        uint16_t addr = VRAM_START + (y << 5) + (x >> 3);
        uint8_t bit_pos = 7 - (x & 7);
        uint8_t byte_val = PEEK(addr);
        byte_val |= (1 << bit_pos);
        POKE(addr, byte_val);
    }
}

/*
 * Draw decorative border around playfield and UI elements
 */
void draw_playfield_border(void)
{
    uint8_t pf_x1 = PLAYFIELD_START_X - 1;
    uint8_t pf_x2 = PLAYFIELD_START_X + PLAYFIELD_PIXEL_WIDTH;
    uint8_t pf_y1 = PLAYFIELD_START_Y - 1;
    uint8_t pf_y2 = PLAYFIELD_START_Y + PLAYFIELD_PIXEL_HEIGHT;

    /* Playfield border */
    draw_horizontal_line(pf_x1, pf_x2, pf_y1);         /* Top */
    draw_horizontal_line(pf_x1, pf_x2, pf_y2);         /* Bottom */
    draw_vertical_line(pf_x1, pf_y1, pf_y2);           /* Left */
    draw_vertical_line(pf_x2, pf_y1, pf_y2);           /* Right */

    /* UI area borders */
    draw_vertical_line(UI_LEFT_X + UI_WIDTH, 8, 184);   /* Left UI separator */
    draw_vertical_line(UI_RIGHT_X - 1, 8, 184);         /* Right UI separator */

    /* Title area */
    draw_horizontal_line(8, 247, 8);                    /* Top border */
}

/*
 * Demonstrate tetris pieces and layout
 */
void draw_tetris_demo(void)
{
    /* Draw border and UI elements */
    draw_playfield_border();

    /* Draw classic Tetris pieces */

    /* I-piece (vertical) */
    draw_tetris_block_outline(1, 1);
    draw_tetris_block_outline(1, 2);
    draw_tetris_block_outline(1, 3);
    draw_tetris_block_outline(1, 4);

    /* T-piece */
    draw_tetris_block_outline(4, 2);
    draw_tetris_block_outline(3, 3);
    draw_tetris_block_outline(4, 3);
    draw_tetris_block_outline(5, 3);

    /* L-piece */
    draw_tetris_block_outline(7, 1);
    draw_tetris_block_outline(7, 2);
    draw_tetris_block_outline(7, 3);
    draw_tetris_block_outline(8, 3);

    /* O-piece (square) */
    draw_tetris_block_outline(3, 6);
    draw_tetris_block_outline(4, 6);
    draw_tetris_block_outline(3, 7);
    draw_tetris_block_outline(4, 7);

    /* S-piece */
    draw_tetris_block_outline(6, 6);
    draw_tetris_block_outline(7, 6);
    draw_tetris_block_outline(5, 7);
    draw_tetris_block_outline(6, 7);

    /* Z-piece */
    draw_tetris_block_outline(1, 8);
    draw_tetris_block_outline(2, 8);
    draw_tetris_block_outline(2, 9);
    draw_tetris_block_outline(3, 9);

    /* J-piece */
    draw_tetris_block_outline(8, 6);
    draw_tetris_block_outline(8, 7);
    draw_tetris_block_outline(8, 8);
    draw_tetris_block_outline(7, 8);

    /* Simulate some filled lines at bottom */
    {
        uint8_t x, y;
        for (y = 17; y < 20; y++) {
            for (x = 0; x < PLAYFIELD_WIDTH; x++) {
                /* Create some gaps to look realistic */
                if ((x + y) % 3 != 0) {
                    draw_tetris_block_outline(x, y);
                }
            }
        }
    }

    /* Add some UI indicators in side areas */
    /* Simple pattern in left UI area */
    draw_horizontal_line(UI_LEFT_X + 8, UI_LEFT_X + 32, 24);
    draw_horizontal_line(UI_LEFT_X + 8, UI_LEFT_X + 32, 26);

    /* Simple pattern in right UI area */
    draw_horizontal_line(UI_RIGHT_X + 8, UI_RIGHT_X + 32, 24);
    draw_horizontal_line(UI_RIGHT_X + 8, UI_RIGHT_X + 32, 26);
}

/*
 * Z80 I/O port access functions using inline assembly
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