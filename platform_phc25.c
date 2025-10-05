#include "platform.h"

#ifdef PHC25
#include "game_state.h"
#include "debug_font.h"

/************************************************************/
/* PHC-25 Mode 12 Graphics Implementation                   */
/* Based on successful POC using 256x192 monochrome        */
/************************************************************/

/* Global variables */
static uint8_t current_fg_color = green;
static uint8_t current_bg_color = black;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

/* Timeout for input functions - defined in tetrice.c */
extern uint8_t timeout_ticks;

/* I/O port access functions using inline assembly */
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

/* Initialize MC6847 for Mode 12 (256x192 monochrome graphics) */
void init_graphics_mode12(void)
{
    uint8_t mode_byte;

    /* Configure MC6847 mode via port $40 */
    mode_byte = MODE12_AG | MODE12_GM1 | MODE12_GM0 | MODE12_CSS;
    out_port(PORT_40, mode_byte);

    /* Clear VRAM to background (all pixels off) */
    clear_screen();
}

/* Clear entire screen to background color */
void clear_screen(void)
{
    uint16_t i;
    for (i = 0; i < VRAM_SIZE; i++) {
        POKE(VRAM_START + i, 0x00);  /* All pixels off (black) */
    }
}

/* Draw a tetris block at grid position (block_x, block_y) */
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

/* Draw a tetris block with outline pattern */
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

/* Erase a tetris block (set to background) */
void erase_tetris_block(uint8_t block_x, uint8_t block_y)
{
    draw_tetris_block(block_x, block_y, 0); /* 0 = empty/black */
}

/* Draw horizontal line from x1 to x2 at y */
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

/* Draw vertical line from y1 to y2 at x */
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

/************************************************************/
/* Platform Abstraction Layer Implementation               */
/************************************************************/

void posxy(unsigned char column, unsigned char line)
{
    /* Store cursor position for text rendering */
    cursor_x = column;
    cursor_y = line;
}

void color(unsigned char foreground, unsigned char background)
{
    /* Store colors for text rendering - in monochrome, only distinguish black vs non-black */
    current_fg_color = (foreground == black) ? black : green;
    current_bg_color = (background == black) ? black : green;
}

void prints(unsigned char x, unsigned char y, unsigned char *text)
{
    /* Simple text rendering - for now just update cursor position */
    /* Full bitmap font implementation would be needed for complete text */
    posxy(x, y);
    /* TODO: Implement bitmap font rendering */
}

void printsg(unsigned char x, unsigned char y, unsigned char *text)
{
    /* Graphic character rendering - similar to prints for now */
    posxy(x, y);
    /* TODO: Implement graphic character patterns */
}

void printc(unsigned char x, unsigned char y, unsigned char c)
{
    /* Single character rendering */
    posxy(x, y);
    /* TODO: Implement bitmap character rendering */
}

void printcg(unsigned char x, unsigned char y, unsigned char c)
{
    /* Graphic character rendering */
    posxy(x, y);
    /* TODO: Implement graphic patterns */
}

uint8_t charatxy(uint8_t column, uint8_t line)
{
    /* Return character at screen position - not easily possible in pixel mode */
    /* Would need to maintain a shadow buffer for character positions */
    return ' ';  /* Default to space */
}

/************************************************************/
/* Keyboard and clock                                       */
/************************************************************/

/* PHC25 keyboard mapping table based on hardware documentation */
/* Maps scan codes to ASCII characters for each port/bit combination */
/* Table organized as [port_offset * 8 + bit_position] */
static uint8_t keymap_table[64] = {
    /* Port $80 (offset 0): bits 0-7 */
    '1', 'W', 'S', 'X', 0, 0, 0, 0,
    /* Port $81 (offset 1): bits 0-7 */
    0, 'Q', 'A', 'Z', 0, 0, 0, 0,
    /* Port $82 (offset 2): bits 0-7 */
    '3', 'R', 'F', 'V', 0, 0, 0, 0,
    /* Port $83 (offset 3): bits 0-7 */
    '2', 'E', 'D', 'C', 0, 0, 0, ' ',
    /* Port $84 (offset 4): bits 0-7 */
    '5', 'Y', 'H', 'N', 0, '0', 'P', 0,
    /* Port $85 (offset 5): bits 0-7 */
    '4', 'T', 'G', 'B', 0, 0, 0, 0,
    /* Port $86 (offset 6): bits 0-7 */
    '6', 'U', 'J', 'M', 0, '9', 'O', 0,
    /* Port $87 (offset 7): bits 0-7 */
    '7', 'I', 'K', 0, 0, '8', 'L', 0
};

/* Convert scan code to ASCII character */
uint8_t convert_scancode_to_ascii(uint8_t scancode, uint8_t port)
{
    uint8_t port_offset, bit_position, table_index;
    uint8_t temp_scancode;

    /* Calculate port offset (0-7) */
    port_offset = port - KEYBOARD_PORT_BASE;
    if (port_offset > 7) return 0;

    /* Find which bit is set in scan code */
    bit_position = 0;
    temp_scancode = scancode;

    while (bit_position < 8) {
        if (temp_scancode & 1) {
            /* Found the set bit, calculate table index */
            table_index = (port_offset << 3) + bit_position;  /* port_offset * 8 + bit_position */
            return keymap_table[table_index];
        }
        temp_scancode >>= 1;
        bit_position++;
    }

    return 0;  /* No bit found */
}

uint8_t scankey()
{
    uint8_t port;
    uint8_t key_data, scancode;
    uint8_t ascii_char;

    /* Scan each keyboard port ($80-$87) */
    for (port = 0; port < 8; port++) {
        /* Read the keyboard port */
        key_data = in_port(KEYBOARD_PORT_BASE + port);

        /* Check if any key is pressed (not $FF) */
        if (key_data != 0xFF) {
            /* Complement to get scan code (active low) */
            scancode = ~key_data;

            /* Convert scan code to ASCII character */
            ascii_char = convert_scancode_to_ascii(scancode, KEYBOARD_PORT_BASE + port);

            if (ascii_char != 0) {
                return ascii_char;
            }
        }
    }

    return 0;  /* No key pressed */
}

void sleep(uint8_t seconds)
{
    /* Simple busy-wait implementation */
    /* TODO: Implement using hardware timer if available */
    uint16_t i, j;
    for (i = 0; i < seconds; i++) {
        for (j = 0; j < 1000; j++) {
            /* Busy wait loop */
        }
    }
}

void ticks(uint8_t ticks)
{
    /* Simple delay implementation */
    uint16_t i, j;
    for (i = 0; i < ticks; i++) {
        for (j = 0; j < 100; j++) {
            /* Busy wait loop */
        }
    }
}

uint8_t wait()
{
    /* Wait for keypress or timeout */
    uint8_t tick_count = 0;
    uint8_t key;

    while (tick_count < timeout_ticks) {
        key = scankey();
        if (key != 0) {
            timeout_ticks -= tick_count;
            return key;
        }

        /* Simple delay */
        ticks(1);
        tick_count++;
    }

    return 0;  /* Timeout */
}

uint8_t wait_key()
{
    /* Wait indefinitely for a key press */
    uint8_t key;

    while (1) {
        key = scankey();
        if (key != 0) {
            return key;
        }
        /* Small delay to avoid busy waiting */
        ticks(1);
    }
}

uint8_t platform_random()
{
    /* Simple pseudo-random number generator */
    /* TODO: Use hardware timer for better randomness */
    static uint8_t seed = 1;
    seed = (seed * 7 + 5) & 0xFF;
    return seed;
}

input_action_t platform_get_input()
{
    uint8_t c = wait();

    switch (c) {
        case 'A':
            return INPUT_MOVE_LEFT;      /* A key for left movement */
        case 'D':
            return INPUT_MOVE_RIGHT;     /* D key for right movement */
        case 'W':
            return INPUT_ROTATE_CW;      /* W key for rotate clockwise */
        case 'S':
            return INPUT_ROTATE_CCW;     /* S key for rotate counter-clockwise */
        case ' ':
            return INPUT_DROP;           /* Space for hard drop */
        case 0:
            return INPUT_TIMEOUT;
        default:
            return INPUT_NONE;
    }
}

/************************************************************/
/* Display Sync API Implementation                         */
/************************************************************/

void display_sync_playfield(game_state_t* state)
{
    uint8_t x, y, cell_data, cell_content;

    for (y = 0; y < PLAYFIELD_HEIGHT; y++) {
        for (x = 0; x < PLAYFIELD_WIDTH; x++) {
            cell_data = state->playfield[y][x];

            /* Only redraw cells that are marked dirty */
            if (GET_CELL_DIRTY(cell_data)) {
                cell_content = GET_CELL_CONTENT(cell_data);

                if (cell_content != CELL_EMPTY) {
                    /* Draw filled block for any piece type */
                    draw_tetris_block_outline(x, y);
                } else {
                    /* Erase block */
                    erase_tetris_block(x, y);
                }

                /* Clear dirty flag after redraw */
                CLEAR_CELL_DIRTY(state->playfield[y][x]);
            }
        }
    }
}

void display_sync_ui(game_state_t* state)
{
    /* Simple UI implementation - would need bitmap font for full text */
    /* For now, just store the values (could be displayed as simple patterns) */
    /* TODO: Implement score/level display using bitmap patterns */
}

void display_clear_screen()
{
    /* Initialize graphics mode and clear screen */
    init_graphics_mode12();
}

void display_draw_borders()
{
    uint8_t pf_x1 = PLAYFIELD_START_X - 1;
    uint8_t pf_x2 = PLAYFIELD_START_X + PLAYFIELD_PIXEL_WIDTH;
    uint8_t pf_y1 = PLAYFIELD_START_Y - 1;
    uint8_t pf_y2 = PLAYFIELD_START_Y + PLAYFIELD_PIXEL_HEIGHT;

    /* Draw playfield border */
    draw_horizontal_line(pf_x1, pf_x2, pf_y1);         /* Top */
    draw_horizontal_line(pf_x1, pf_x2, pf_y2);         /* Bottom */
    draw_vertical_line(pf_x1, pf_y1, pf_y2);           /* Left */
    draw_vertical_line(pf_x2, pf_y1, pf_y2);           /* Right */

    /* Draw UI area separators - full height */
    //draw_vertical_line(UI_LEFT_X + UI_WIDTH - 1, PLAYFIELD_START_Y, 191);   /* Left UI separator */
    //draw_vertical_line(UI_RIGHT_X, PLAYFIELD_START_Y, 191);                  /* Right UI separator */

    /* Title area border - full width */
    //draw_horizontal_line(0, 255, 7);                    /* Title bottom border */
}

#endif // PHC25
