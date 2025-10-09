/* game_font.h - Bitmap font for score/level display */
/* Generated from digit sprite sheet */
/* Each digit is 4x8 pixels, stored as 8 bytes */
/* Bits are packed MSB first (leftmost pixel = bit 7) */

#ifndef GAME_FONT_H
#define GAME_FONT_H

#include <stdint.h>

/* Font data: 10 digits (0-9), each 8 bytes */
/* Indexed by digit value (0-9) */
extern const uint8_t game_font[10][8];

/* Font rendering functions */

/* Draw a single digit (0-9) at pixel position (x, y) */
void draw_digit(uint8_t x, uint8_t y, uint8_t digit);

/* Draw a multi-digit number at pixel position (x, y)
 * number: value to display (0-255)
 * num_digits: number of digits to display (1-3)
 * leading_zeros: 1 = show leading zeros, 0 = suppress them */
void draw_number(uint8_t x, uint8_t y, uint8_t number, uint8_t num_digits, uint8_t leading_zeros);

#endif /* GAME_FONT_H */
