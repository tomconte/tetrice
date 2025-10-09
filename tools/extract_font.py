#!/usr/bin/env python3
"""
Extract bitmap font from image and generate C header file.
Font is 4x8 pixels per digit, digits are ordered 1-9,0 in the source image.
"""

from PIL import Image
import sys

def extract_digit(img, digit_index):
    """Extract a single 4x8 digit from the image."""
    # Each digit is 4 pixels wide, 8 pixels tall
    digit_width = 4
    digit_height = 8

    # Calculate starting x position for this digit
    x_start = digit_index * digit_width

    # Extract the 8 rows of 4 bits each
    digit_bytes = []

    for y in range(digit_height):
        byte_value = 0
        for x in range(digit_width):
            pixel = img.getpixel((x_start + x, y))

            # Convert pixel to binary (assume bright = 1, dark = 0)
            # Handle both RGB and grayscale images
            if isinstance(pixel, tuple):
                brightness = sum(pixel[:3]) / 3  # Average RGB
            else:
                brightness = pixel

            # Threshold: if bright enough, it's a 1
            if brightness > 128:
                # Pack bits left-to-right, MSB first
                byte_value |= (1 << (7 - x))

        digit_bytes.append(byte_value)

    return digit_bytes

def generate_header(font_data, output_file):
    """Generate C header file with font data."""
    with open(output_file, 'w') as f:
        f.write("/* game_font.h - Bitmap font for score/level display */\n")
        f.write("/* Generated from digit sprite sheet */\n")
        f.write("/* Each digit is 4x8 pixels, stored as 8 bytes */\n")
        f.write("/* Bits are packed MSB first (leftmost pixel = bit 7) */\n\n")
        f.write("#ifndef GAME_FONT_H\n")
        f.write("#define GAME_FONT_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("/* Font data: 10 digits (0-9), each 8 bytes */\n")
        f.write("/* Indexed by digit value (0-9) */\n")
        f.write("const uint8_t game_font[10][8] = {\n")

        # Font data is in order 1-9,0 in image, but we want 0-9 in array
        # So we need to remap: image[9] -> array[0], image[0] -> array[1], etc.
        digit_names = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']

        for digit in range(10):
            # Map array index to image index
            if digit == 0:
                image_index = 9  # '0' is last in image
            else:
                image_index = digit - 1  # '1'-'9' are first in image

            digit_bytes = font_data[image_index]

            f.write(f"    {{ ")
            hex_bytes = ', '.join(f"0x{b:02X}" for b in digit_bytes)
            f.write(hex_bytes)
            f.write(f" }}")

            if digit < 9:
                f.write(",")

            f.write(f"  /* '{digit_names[digit]}' */\n")

        f.write("};\n\n")
        f.write("#endif /* GAME_FONT_H */\n")

def main():
    if len(sys.argv) < 2:
        print("Usage: python extract_font.py <input_image> [output_header]")
        print("Example: python extract_font.py digits.png game_font.h")
        sys.exit(1)

    input_image = sys.argv[1]
    output_header = sys.argv[2] if len(sys.argv) > 2 else "game_font.h"

    # Load the image
    try:
        img = Image.open(input_image)
        print(f"Loaded image: {img.size[0]}x{img.size[1]} pixels, mode: {img.mode}")
    except Exception as e:
        print(f"Error loading image: {e}")
        sys.exit(1)

    # Expected dimensions: 40x8 (10 digits * 4 pixels wide, 8 pixels tall)
    expected_width = 40
    expected_height = 8

    if img.size[0] < expected_width or img.size[1] < expected_height:
        print(f"Warning: Image size {img.size} is smaller than expected {expected_width}x{expected_height}")

    # Extract all 10 digits
    font_data = []
    for i in range(10):
        digit_bytes = extract_digit(img, i)
        font_data.append(digit_bytes)

        # Print extracted data for verification
        digit_num = (i + 1) % 10  # Convert image order to digit value
        print(f"\nDigit '{digit_num}':")
        for y, byte_val in enumerate(digit_bytes):
            # Show visual representation
            binary = format(byte_val, '08b')
            visual = binary.replace('1', '#').replace('0', '.')
            print(f"  Row {y}: 0x{byte_val:02X}  {visual[4:]}")  # Only show 4 bits

    # Generate header file
    generate_header(font_data, output_header)
    print(f"\nGenerated {output_header}")

if __name__ == "__main__":
    main()
