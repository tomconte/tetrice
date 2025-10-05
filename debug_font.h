#ifndef DEBUG_FONT_H
#define DEBUG_FONT_H

#include <stdint.h>

/* 4x4 bitmap font data */
extern const uint8_t debug_font_4x4[][4];

/* Convert character to font index */
uint8_t char_to_font_index(char c);

/* Debug print functions */
void debug_clear_area(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void debug_draw_char(uint8_t x, uint8_t y, char c);
void debug_print(uint8_t x, uint8_t y, char* text);
void debug_print_hex(uint8_t x, uint8_t y, uint8_t value);

#endif /* DEBUG_FONT_H */
