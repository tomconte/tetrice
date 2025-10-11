/* game_font.c - Bitmap font rendering for PHC-25 */

#include "game_font.h"
#include "phc25.h"

/* Font data: 10 digits (0-9), each 8 bytes (4x8 pixels) */
/* Bits are packed in upper nibble (bits 7-4), MSB first */
const uint8_t game_font[10][8] = {
    { 0x80, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x80, 0xF0 },  /* '0' */
    { 0x90, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0x80, 0xF0 },  /* '1' */
    { 0x80, 0xA0, 0xE0, 0x80, 0xB0, 0xB0, 0x80, 0xF0 },  /* '2' */
    { 0x80, 0xE0, 0x80, 0xE0, 0xA0, 0xA0, 0x80, 0xF0 },  /* '3' */
    { 0xA0, 0xA0, 0xA0, 0x80, 0xE0, 0xE0, 0xE0, 0xF0 },  /* '4' */
    { 0x80, 0xB0, 0x80, 0xE0, 0xA0, 0xA0, 0x80, 0xF0 },  /* '5' */
    { 0x80, 0xA0, 0xB0, 0x80, 0xA0, 0xA0, 0x80, 0xF0 },  /* '6' */
    { 0x80, 0xA0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0 },  /* '7' */
    { 0x80, 0xA0, 0xA0, 0x80, 0xA0, 0xA0, 0x80, 0xF0 },  /* '8' */
    { 0x80, 0xA0, 0xA0, 0x80, 0xE0, 0xA0, 0x80, 0xF0 }   /* '9' */
};

/* Draw a single digit at pixel position (x, y) */
void draw_digit(uint8_t x, uint8_t y, uint8_t digit)
{
    uint8_t row;
    uint16_t vram_addr;
    uint8_t existing_byte, font_bits;

    /* Draw each row of the 4x8 digit */
    for (row = 0; row < 8; row++) {
        vram_addr = VRAM_START + ((y + row) << 5) + (x >> 3);
        existing_byte = PEEK(vram_addr);
        font_bits = game_font[digit][row];

        /* Check alignment: bit 2 of x determines nibble position */
        if ((x & 0x04) == 0) {
            /* Byte-aligned: digit in upper nibble (bits 7-4) */
            existing_byte = (existing_byte & 0x0F) | font_bits;
        } else {
            /* Half-byte aligned: digit in lower nibble (bits 3-0) */
            existing_byte = (existing_byte & 0xF0) | (font_bits >> 4);
        }

        POKE(vram_addr, existing_byte);
    }
}

/* Draw a 3-digit number at pixel position (x, y) with leading zeros */
void draw_number(uint8_t x, uint8_t y, uint8_t number, uint8_t num_digits, uint8_t leading_zeros)
{
    uint8_t digits[3];
    uint8_t i, digit_x;

    /* Extract digits from number (right to left) */
    digits[0] = number % 10;
    digits[1] = (number / 10) % 10;
    digits[2] = (number / 100) % 10;

    /* Draw digits from left to right */
    digit_x = x;
    for (i = 0; i < num_digits; i++) {
        draw_digit(digit_x, y, digits[num_digits - 1 - i]);
        digit_x += 4;
    }
}
