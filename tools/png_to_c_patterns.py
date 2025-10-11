#!/usr/bin/env python3
"""
Convert PNG sprite sheet of 8x8 blocks into C byte patterns.
Each block is represented as 8 bytes (one per row), where each bit represents a pixel.
"""

from PIL import Image
import sys


def pixel_is_set(pixel):
    """Determine if a pixel should be considered 'on' (1) or 'off' (0)."""
    # Handle both RGB and RGBA formats
    if isinstance(pixel, (tuple, list)):
        # Use simple threshold on red channel (assuming grayscale-ish)
        # Pixel is 'on' if it's bright enough
        return pixel[0] > 128
    else:
        # Single value (grayscale or palette index)
        # For palette mode, non-zero values are 'on'
        return pixel != 0


def extract_block_pattern(img, block_index, block_size=8):
    """Extract a single 8x8 block pattern from the sprite sheet."""
    start_x = block_index * block_size
    pattern = []

    for y in range(block_size):
        byte_value = 0
        for x in range(block_size):
            pixel = img.getpixel((start_x + x, y))
            if pixel_is_set(pixel):
                # Set bit (MSB is leftmost pixel)
                bit_pos = 7 - x
                byte_value |= (1 << bit_pos)
        pattern.append(byte_value)

    return pattern


def format_pattern_as_c(pattern, name):
    """Format a pattern as C array initializer."""
    hex_bytes = ', '.join(f'0x{b:02X}' for b in pattern)
    return f'    {{{hex_bytes}}}  /* {name} */'


def main():
    if len(sys.argv) < 2:
        print("Usage: python png_to_c_patterns.py <input.png>")
        sys.exit(1)

    input_file = sys.argv[1]

    # Load the PNG
    img = Image.open(input_file)
    width, height = img.size

    print(f"/* Generated from {input_file} ({width}x{height}) */")
    print(f"/* Each pattern is 8 bytes representing an 8x8 pixel block */")
    print()

    # Calculate number of blocks (assuming 8x8 blocks in a horizontal row)
    num_blocks = width // 8

    # Block names (matching tetromino colors)
    block_names = ['I', 'O', 'T', 'S', 'Z', 'J', 'L']

    print(f"static const uint8_t block_patterns[{num_blocks}][8] = {{")

    for i in range(num_blocks):
        pattern = extract_block_pattern(img, i)
        name = block_names[i] if i < len(block_names) else f'Block{i}'
        print(format_pattern_as_c(pattern, name) +
              (',' if i < num_blocks - 1 else ''))

    print("};")
    print()

    # Generate lookup comment
    print("/* Block pattern indices: */")
    for i in range(num_blocks):
        name = block_names[i] if i < len(block_names) else f'Block{i}'
        print(f"/* {i} = {name} */")


if __name__ == '__main__':
    main()
