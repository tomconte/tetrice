#include "debug_font.h"

/* In test mode, use mock definitions; otherwise use real hardware */
#ifdef TEST_MODE
#include "tests/test_mock.h"
#else
#include "phc25.h"
#endif

/* 4x4 bitmap font - each character is 4 bytes (4 rows of 4 bits) */
const uint8_t debug_font_4x4[][4] = {
    /* 0 */ {0x6, 0x9, 0x9, 0x6},  /* 0110, 1001, 1001, 0110 */
    /* 1 */ {0x2, 0x6, 0x2, 0x7},  /* 0010, 0110, 0010, 0111 */
    /* 2 */ {0x6, 0x1, 0x6, 0xF},  /* 0110, 0001, 0110, 1111 */
    /* 3 */ {0x6, 0x1, 0x2, 0x6},  /* 0110, 0001, 0010, 0110 */
    /* 4 */ {0x9, 0x9, 0xF, 0x1},  /* 1001, 1001, 1111, 0001 */
    /* 5 */ {0xF, 0x8, 0xE, 0x1},  /* 1111, 1000, 1110, 0001 */
    /* 6 */ {0x6, 0x8, 0xE, 0x6},  /* 0110, 1000, 1110, 0110 */
    /* 7 */ {0xF, 0x1, 0x2, 0x4},  /* 1111, 0001, 0010, 0100 */
    /* 8 */ {0x6, 0x9, 0x6, 0x9},  /* 0110, 1001, 0110, 1001 */
    /* 9 */ {0x6, 0x7, 0x1, 0x6},  /* 0110, 0111, 0001, 0110 */
    /* A */ {0x6, 0x9, 0xF, 0x9},  /* 0110, 1001, 1111, 1001 */
    /* B */ {0xE, 0x9, 0xE, 0xE},  /* 1110, 1001, 1110, 1110 */
    /* C */ {0x6, 0x8, 0x8, 0x6},  /* 0110, 1000, 1000, 0110 */
    /* D */ {0xE, 0x9, 0x9, 0xE},  /* 1110, 1001, 1001, 1110 */
    /* E */ {0xF, 0x8, 0xE, 0xF},  /* 1111, 1000, 1110, 1111 */
    /* F */ {0xF, 0x8, 0xE, 0x8},  /* 1111, 1000, 1110, 1000 */
    /* G */ {0x6, 0x8, 0xB, 0x6},  /* 0110, 1000, 1011, 0110 */
    /* H */ {0x9, 0x9, 0xF, 0x9},  /* 1001, 1001, 1111, 1001 */
    /* I */ {0x7, 0x2, 0x2, 0x7},  /* 0111, 0010, 0010, 0111 */
    /* K */ {0x9, 0xA, 0xC, 0xA},  /* 1001, 1010, 1100, 1010 */
    /* L */ {0x8, 0x8, 0x8, 0xF},  /* 1000, 1000, 1000, 1111 */
    /* M */ {0x9, 0xF, 0xF, 0x9},  /* 1001, 1111, 1111, 1001 */
    /* N */ {0x9, 0xD, 0xB, 0x9},  /* 1001, 1101, 1011, 1001 */
    /* O */ {0x6, 0x9, 0x9, 0x6},  /* 0110, 1001, 1001, 0110 */
    /* P */ {0xE, 0x9, 0xE, 0x8},  /* 1110, 1001, 1110, 1000 */
    /* R */ {0xE, 0x9, 0xE, 0xA},  /* 1110, 1001, 1110, 1010 */
    /* S */ {0x6, 0x8, 0x1, 0x6},  /* 0110, 1000, 0001, 0110 */
    /* T */ {0x7, 0x2, 0x2, 0x2},  /* 0111, 0010, 0010, 0010 */
    /* U */ {0x9, 0x9, 0x9, 0x6},  /* 1001, 1001, 1001, 0110 */
    /* - */ {0x0, 0x0, 0x7, 0x0},  /* 0000, 0000, 0111, 0000 */
    /*' '*/ {0x0, 0x0, 0x0, 0x0},  /* 0000, 0000, 0000, 0000 */
    /* . */ {0x0, 0x0, 0x0, 0x2},  /* 0000, 0000, 0000, 0010 */
};

/* Character to font index mapping */
uint8_t char_to_font_index(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    switch (c) {
        case 'G': return 16;
        case 'H': return 17;
        case 'I': return 18;
        case 'K': return 19;
        case 'L': return 20;
        case 'M': return 21;
        case 'N': return 22;
        case 'O': return 23;
        case 'P': return 24;
        case 'R': return 25;
        case 'S': return 26;
        case 'T': return 27;
        case 'U': return 28;
        case '-': return 29;
        case ' ': return 30;
        case '.': return 31;
        default: return 30; /* space for unknown chars */
    }
}

/* Clear a rectangular area by setting pixels to 0 */
void debug_clear_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    uint8_t row, col;
    uint16_t addr;

    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            /* Calculate VRAM address for this pixel */
            addr = VRAM_START + ((y + row) << 5) + ((x + col) >> 3);
            uint8_t bit_pos = 7 - ((x + col) & 7);
            uint8_t byte_val = PEEK(addr);
            byte_val &= ~(1 << bit_pos);
            POKE(addr, byte_val);
        }
    }
}

/* Draw a single character using 4x4 bitmap font */
void debug_draw_char(uint8_t x, uint8_t y, char c) {
    uint8_t font_index = char_to_font_index(c);
    uint8_t row, bit;
    uint16_t addr;
    uint8_t pixel_row;

    /* Draw 4x4 character */
    for (row = 0; row < 4; row++) {
        pixel_row = debug_font_4x4[font_index][row];
        for (bit = 0; bit < 4; bit++) {
            if (pixel_row & (1 << (3-bit))) {
                /* Calculate VRAM address for this pixel */
                addr = VRAM_START + ((y + row) << 5) + ((x + bit) >> 3);
                uint8_t bit_pos = 7 - ((x + bit) & 7);
                uint8_t byte_val = PEEK(addr);
                byte_val |= (1 << bit_pos);
                POKE(addr, byte_val);
            }
        }
    }
}

/* Debug print function - draws text at pixel coordinates */
void debug_print(uint8_t x, uint8_t y, char* text) {
    uint8_t char_x = x;
    char* c = text;

    while (*c != '\0' && char_x < 252) { /* Leave space for 4-pixel char */
        debug_draw_char(char_x, y, *c);
        char_x += 4; /* Move to next character position */
        c++;
    }
}

/* Debug print number in hex format */
void debug_print_hex(uint8_t x, uint8_t y, uint8_t value) {
    char hex_str[3];
    /* Clear area for 2 hex digits (2 chars * 4 pixels wide * 4 pixels high) */
    debug_clear_area(x, y, 8, 4);
    hex_str[0] = (value >> 4) < 10 ? '0' + (value >> 4) : 'A' + (value >> 4) - 10;
    hex_str[1] = (value & 0xF) < 10 ? '0' + (value & 0xF) : 'A' + (value & 0xF) - 10;
    hex_str[2] = '\0';
    debug_print(x, y, hex_str);
}
