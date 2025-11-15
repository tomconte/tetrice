#include "platform.h"

#ifdef PHC25
#include "game_state.h"
#include "game_font.h"

/* Tetromino data structures - external declarations to avoid duplicate definitions */
typedef uint8_t packed_tetromino[4];
extern packed_tetromino all_tetrominos[];
extern uint8_t tetromino_offsets[];
extern uint8_t tetrominos_nb_shapes[];

/* Bit manipulation macros for packed format */
#define GET_BLOCK_X(block) ((block) & 0x03)
#define GET_BLOCK_Y(block) (((block) >> 2) & 0x03)
#define GET_BLOCK_SIDES(block) (((block) >> 4) & 0x07)

/* Macro to access a specific tetromino rotation */
#define GET_TETROMINO(piece, rotation) (&all_tetrominos[tetromino_offsets[piece] + (rotation)])

extern void decompress_ui_left(void);
extern void decompress_ui_right(void);
extern void decompress_ui_title(void);
extern void decompress_ui_bottom(void);
extern void decompress_splash(void);
extern void decompress_gameover(void);

/************************************************************/
/* PHC-25 Mode 12 Graphics Implementation                   */
/* Based on successful POC using 256x192 monochrome        */
/************************************************************/

/* Block patterns for each tetromino type (8x8 pixels) */
/* Generated from gfx/blocks.png */
const uint8_t block_patterns[7][8] = {
    {0xFE, 0xAA, 0xD6, 0xAA, 0xD6, 0xAA, 0xFE, 0x00},  /* I */
    {0xFE, 0x82, 0x82, 0x92, 0x82, 0x82, 0xFE, 0x00},  /* O */
    {0xFE, 0x82, 0x86, 0x8E, 0x9E, 0xBE, 0xFE, 0x00},  /* T */
    {0xFE, 0x82, 0xBA, 0xAA, 0xBA, 0x82, 0xFE, 0x00},  /* S */
    {0xFE, 0xC6, 0x82, 0x82, 0x82, 0xC6, 0xFE, 0x00},  /* Z */
    {0xFE, 0xD6, 0x92, 0xFE, 0x92, 0xD6, 0xFE, 0x00},  /* J */
    {0xFE, 0xAA, 0xEE, 0x82, 0xEE, 0xAA, 0xFE, 0x00}   /* L */
};

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


/* Draw a tetris block with pattern based on color at pixel coordinates */
void draw_tetris_block_pattern(uint8_t pixel_x, uint8_t pixel_y, uint8_t color)
{
    uint16_t start_addr;
    uint8_t row;

    if (color == 0 || color > 7) return; /* Invalid color */

    /* Draw 8x8 block using pattern */
    for (row = 0; row < BLOCK_SIZE; row++) {
        /* Calculate VRAM address for this row */
        start_addr = VRAM_START + ((pixel_y + row) << 5) + (pixel_x >> 3);
        POKE(start_addr, block_patterns[color-1][row]);
    }
}

/* Erase a tetris block (set to background) at pixel coordinates */
void erase_tetris_block(uint8_t pixel_x, uint8_t pixel_y)
{
    uint16_t start_addr;
    uint8_t row;

    /* Erase 8x8 block */
    for (row = 0; row < BLOCK_SIZE; row++) {
        start_addr = VRAM_START + ((pixel_y + row) << 5) + (pixel_x >> 3);
        POKE(start_addr, 0x00);  /* All pixels off */
    }
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
        for (j = 0; j < 2000; j++) {
            /* Busy wait loop */
        }
    }
}

void ticks(uint8_t ticks)
{
    /* Simple delay implementation */
    uint16_t i, j;
    for (i = 0; i < ticks; i++) {
        for (j = 0; j < 1000; j++) {
            /* Busy wait loop */
        }
    }
}

uint8_t wait()
{
    /* Wait for keypress or timeout */
    uint8_t tick_count = 0;
    uint8_t key;
    uint8_t scan_iterations;

    while (tick_count < timeout_ticks) {
        /* Delay for one tick - this MUST happen before key checks */
        /* to ensure timing always progresses */
        ticks(1);
        tick_count++;

        /* Check keys multiple times for responsiveness */
        for (scan_iterations = 0; scan_iterations < 5; scan_iterations++) {
            key = scankey();
            if (key != 0) {
                timeout_ticks -= tick_count;
                return key;
            }
        }
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
    uint8_t c;
    static input_action_t prev_input = INPUT_NONE;
    static uint8_t bounce = 0;
    input_action_t current_action;

    c = wait();

    switch (c) {
        case 'O':
            current_action = INPUT_MOVE_LEFT;
            break;
        case 'P':
            current_action = INPUT_MOVE_RIGHT;
            break;
        case 'Q':
            current_action = INPUT_ROTATE_CW;
            break;
        case 'W':
            current_action = INPUT_ROTATE_CCW;
            break;
        case ' ':
            current_action = INPUT_DROP;
            break;
        case 0:
            current_action = INPUT_TIMEOUT;
            break;
        default:
            current_action = INPUT_NONE;
            break;
    }

    // Anti-bounce and key repeat logic
    if (current_action != INPUT_TIMEOUT && current_action != INPUT_NONE)
    {
        if (current_action == prev_input)
        {
            if (((current_action == INPUT_MOVE_LEFT || current_action == INPUT_MOVE_RIGHT) && bounce > INPUT_LATERAL_SKIP) ||
                ((current_action == INPUT_ROTATE_CW || current_action == INPUT_ROTATE_CCW) && bounce > INPUT_ROTATION_SKIP) ||
                (current_action == INPUT_DROP))
            {
                bounce = 0; // Allow action to be repeated
            }
            else
            {
                bounce++;
                current_action = INPUT_NONE; // Ignore this input
            }
        }
        else
        {
            prev_input = current_action;
            bounce = 0;
        }
    } else {
        // Reset if no key is pressed or timeout occurs
        prev_input = INPUT_NONE;
        bounce = 0;
    }

    return current_action;
}

/************************************************************/
/* Display Sync API Implementation                         */
/************************************************************/

void display_sync_playfield(game_state_t* state)
{
    uint8_t x, y, cell_data, cell_content;
    uint8_t pixel_x, pixel_y;

    for (y = 0; y < PLAYFIELD_HEIGHT; y++) {
        for (x = 0; x < PLAYFIELD_WIDTH; x++) {
            cell_data = state->playfield[y][x];

            /* Only redraw cells that are marked dirty */
            if (GET_CELL_DIRTY(cell_data)) {
                cell_content = GET_CELL_CONTENT(cell_data);

                /* Convert playfield coordinates to pixel coordinates */
                pixel_x = PLAYFIELD_START_X + (x << 3);
                pixel_y = PLAYFIELD_START_Y + (y << 3);

                if (cell_content != CELL_EMPTY) {
                    /* Draw block with pattern based on piece color */
                    draw_tetris_block_pattern(pixel_x, pixel_y, cell_content);
                } else {
                    /* Erase block */
                    erase_tetris_block(pixel_x, pixel_y);
                }

                /* Clear dirty flag after redraw */
                CLEAR_CELL_DIRTY(state->playfield[y][x]);
            }
        }
    }
}

void display_sync_ui(game_state_t* state)
{
    /* Display score at (216, 134) - 3 digits with leading zeros */
    draw_number(216, 134, state->score, 3, 1);

    /* Display level at (216, 174) - 3 digits with leading zeros */
    draw_number(216, 174, state->level, 3, 1);
}

#define PREVIEW_X 202
#define PREVIEW_Y 80

void display_preview_piece(uint8_t piece)
{
    packed_tetromino *tetromino;
    uint8_t i, px, py;

    /* Clear preview area (4x4 blocks = 32x32 pixels) */
    for (py = 0; py < 4; py++) {
        for (px = 0; px < 4; px++) {
            erase_tetris_block(PREVIEW_X + (px << 3), PREVIEW_Y + (py << 3));
        }
    }

    /* Draw the piece (rotation 0) */
    tetromino = GET_TETROMINO(piece, 0);

    for (i = 0; i < 4; i++) {
        px = GET_BLOCK_X((*tetromino)[i]);
        py = GET_BLOCK_Y((*tetromino)[i]);
        draw_tetris_block_pattern(PREVIEW_X + (px << 3), PREVIEW_Y + (py << 3), piece + 1);
    }
}

void display_clear_screen()
{
    /* Initialize graphics mode and clear screen */
    init_graphics_mode12();
}

#define GFX_TITLE_HEIGHT 8
#define GFX_TITLE_BYTES_PER_ROW 32
#define GFX_LEFT_UI_HEIGHT 184
#define GFX_LEFT_UI_BYTES_PER_ROW 11
#define GFX_RIGHT_UI_HEIGHT 184
#define GFX_RIGHT_UI_BYTES_PER_ROW 11
#define GFX_BOTTOM_HEIGHT 8
#define GFX_BOTTOM_BYTES_PER_ROW 10

/* Copy bitmap data to VRAM */
void copy_bitmap(uint8_t x, uint8_t y, const uint8_t* source_data,
                 uint16_t height, uint8_t bytes_per_row)
{
    uint16_t row, col;
    uint16_t vram_addr;
    const uint8_t* src_ptr = source_data;

    /* Validate x is byte-aligned (multiple of 8) */
    if ((x & 7) != 0) return;

    /* Copy bitmap row by row */
    for (row = 0; row < height; row++) {
        vram_addr = VRAM_START + ((y + row) << 5) + (x >> 3);

        for (col = 0; col < bytes_per_row; col++) {
            POKE(vram_addr + col, *src_ptr++);
        }
    }
}

void display_draw_borders()
{
    /* Draw title at top (full width) - zx0 compressed */
    decompress_ui_title();
    copy_bitmap(0, 0, VRAM2_START, GFX_TITLE_HEIGHT, GFX_TITLE_BYTES_PER_ROW);

    /* Draw left UI panel - zx0 compressed */
    decompress_ui_left();
    copy_bitmap(0, 8, VRAM2_START, GFX_LEFT_UI_HEIGHT, GFX_LEFT_UI_BYTES_PER_ROW);

    /* Draw right UI panel - zx0 compressed */
    decompress_ui_right();
    copy_bitmap(168, 8, VRAM2_START, GFX_RIGHT_UI_HEIGHT, GFX_RIGHT_UI_BYTES_PER_ROW);

    /* Draw bottom decoration (centered at bottom) - zx0 compressed */
    decompress_ui_bottom();
    copy_bitmap(88, 184, VRAM2_START, GFX_BOTTOM_HEIGHT, GFX_BOTTOM_BYTES_PER_ROW);

    // Draw splash screen in the playfield area
    decompress_splash();
    copy_bitmap(88, 8, VRAM2_START, 176, 10);
}

void display_game_over()
{
    decompress_gameover();
    copy_bitmap(88, 80, VRAM2_START, 40, 10);
}

#endif // PHC25
